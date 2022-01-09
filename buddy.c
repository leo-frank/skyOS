#include "buddy.h"

#include "list.h"
#include "panic.h"
#include "pmm.h"
#include "printf.h"
#include "strings.h"
#include "type.h"

/*
 * Buddy allocater: FIXME: bug may happen in multi-thread execution.
 * Order grows from 0 to BUDDY_MAX_ORDER - 1;
 * The smallest allocate size is BUDDY_PAGE_SIZE; so if you want to allocate
 * size that smaller than BUDDY_PAGE_SIZE, there will be internal fragments.
 * The second smallest size is (1 << 1) * BUDDY_PAGE_SIZE;
 * ......
 * The largest allocate size is BUDDY_MAX_ORDER_NR_PAGES * BUDDY_PAGE_SIZE;
 */

/* bugs may happens:
 * 1. list_del without "buddy_lists[i].nr --";
 * 2. list_append without "buddy_lists[i].nr ++"
 * 3. list_del(buddy_lists[tmp].head), should be next
 * 4. get_buddy_page may return null, so give a judge
 */

// In 2 situations we think page's order is meaningless.
// PG_TAIL || p->free = 0
#define BUDDY_MAX_ORDER (14ul)
#define BUDDY_ML_ORDER BUDDY_MAX_ORDER
#define BUDDY_MAX_ORDER_NR_PAGES (1 << (BUDDY_MAX_ORDER - 1))
#define BUDDY_PAGE_SIZE (0x1000)  // 4K

struct free_list {
  struct list_head head;  // link all list_link in struct Page
  uint64 nr;              // # of linked struct Page
};

extern struct Page pages[npage];  // define in pmm.c
struct free_list buddy_lists[BUDDY_MAX_ORDER];

// convert list entry to page
#define le2page(le, member) to_struct((le), struct Page, member)

static void buddy_pages_init() {
  memset(pages, 0, npage);
  for (int i = 0; i < npage; i += BUDDY_MAX_ORDER_NR_PAGES) {
    pages[i].order = BUDDY_MAX_ORDER - 1;
    pages[i].free = 1;
    pages[i].flag = PG_HEAD;
    list_append(&(pages[i].list_link),
                &(buddy_lists[BUDDY_MAX_ORDER - 1].head));
    buddy_lists[BUDDY_MAX_ORDER - 1].nr++;
  }
  // now, in buddy_lists, only # of last free_list > 0, # of other free_list = 0
  // so memory can be seen as chuncks of BUDDY_MAX_ORDER_NR_PAGES
}

static void buddy_lists_init() {
  for (int i = 0; i < BUDDY_MAX_ORDER; i++) {
    init_list_head(&(buddy_lists[i].head));
    buddy_lists[i].nr = 0;
  }
}

static inline int is_pow_of_2(uint x) { return !(x & (x - 1)); }

/* size2order: return value should in [0:BUDDY_MAX_ORDER)
 * Given BUDDY_PAGE_SIZE, return 0;
 * Given (1 << 1) * BUDDY_PAGE_SIZE, return 1;
 * ......
 * Given BUDDY_MAX_ORDER_NR_PAGES * BUDDY_PAGE_SIZE,
 *      return BUDDY_MAX_ORDER - 1;
 * so,
 * Given (1 << i) * BUDDY_PAGE_SIZE, return i;
 */
static inline uint size2order(uint64 size) {
  uint nr_page = size / BUDDY_PAGE_SIZE;
  uint tmp = nr_page;
  uint order = 0;
  while (tmp > 1) {
    tmp >>= 1;
    order += 1;
  }
  uint64 res = (1 << order) * BUDDY_PAGE_SIZE;
  if (size > res) order += 1;
  return order;
}

// FIXME: why only return the right buddy, what about
// the left buddy, in buddy_test2, I design a testcase
// showing that only considering the right buddy will
// results in no merge.
static struct Page* get_buddy_page(struct Page* p) {
  struct Page* buddy_page;
  buddy_page = p + (1 << (p->order));
  return buddy_page;
}

/* Used only in allocate process.
 * split first chunck in buddy_lists[order_large] until
 * there is a chunk of order_small generated.
 */
static void* buddy_split(uint order_large, uint order_small) {
  if (buddy_lists[order_large].nr == 0)
    panic("[buddy]: split. # of order_large should not be 0.\n");
  if (order_large == 0)
    panic("[buddy]: split. order_large should larger than 0.\n");

  struct Page *p, *buddy_p;
  struct list_head* lh;
  uint tmp = order_large;  // tmp >= 0
  do {
    lh = buddy_lists[tmp].head.next;
    list_del(lh);
    // every split results in 1 chunck of `tmp` order decrement
    buddy_lists[tmp].nr--;
    // p is the page which has order `tmp` before, and has been
    // split into p and buddy_p, which has order `tmp-1`
    p = le2page(lh, list_link);
    p->order--;
    buddy_p = get_buddy_page(p);
    // what's more, add p & buddy_p to free_list of order `tmp-1`
    lh = &(buddy_lists[tmp - 1].head);
    list_append(&(p->list_link), lh);
    list_append(&(buddy_p->list_link), lh);
    // also results in 2 new chunck of `tmp-1` order
    buddy_lists[tmp - 1].nr += 2;
    // buddy_p used to be a PG_TAIL page, now become a PG_HEAD page,
    p->flag = buddy_p->flag = PG_HEAD;
    // also update it's order as p
    buddy_p->order = p->order;
    // buddy_p & p should be free
    p->free = buddy_p->free = 1;
    // however, continue to split first newly chunck of `tmp-1`
    tmp -= 1;
  } while (tmp > order_small);
  // now, p->order must equal to order_small
  if (p->order != order_small || p == NULL) {
    panic("[buddy] unexpected order\n");
    return NULL;
  } else {
    p->free = 0;
    list_del(&(p->list_link));
    buddy_lists[order_small].nr--;
    return p;
  }
}

void buddy_init() {
  buddy_lists_init();
  buddy_pages_init();
}

struct Page* buddy_allocate(uint64 size) {
  uint order = size2order(size);

  if (order >= BUDDY_MAX_ORDER) {
    goto fail;
  }

  struct Page* p;
  struct list_head* lh;

  // find a free page
  if (buddy_lists[order].nr > 0) {
    lh = buddy_lists[order].head.next;
    list_del(lh);
    buddy_lists[order].nr--;
    p = le2page(lh, list_link);
    p->free = 0;
    // p->order should not change, because it
    // will be used in free_page()
    return p;
  } else {
    uint tmp = order;
    order++;
    // find a free_list that have free chuncks
    while (buddy_lists[order].nr == 0) {
      order++;
      if (order > BUDDY_MAX_ORDER) goto fail;
    }
    return buddy_split(order, tmp);
  }
fail:
  panic("[buddy]: allocate size too large.\n");
  return NULL;
}

void buddy_free_page(struct Page* p) {
  if (p->flag == PG_TAIL)
    panic("[buddy]: free a tail page, this is not allowed.\n");
  if (p->free == 1) panic("[buddy]: free a free page.\n");
  struct Page* buddy_p;
  struct list_head* lh;

  p->free = 1;
  lh = &(buddy_lists[p->order].head);
  list_append(&(p->list_link), lh);
  buddy_lists[p->order].nr++;

  while (p->order < (BUDDY_MAX_ORDER - 1)) {
    buddy_p = get_buddy_page(p);
    // we ensure buddy_p must be a PG_HEAD page
    if (buddy_p != NULL && buddy_p->free == 1 && buddy_p->flag == PG_HEAD &&
        buddy_p->order == p->order) {
      // we can merge p & buddy_p
      buddy_p->flag = PG_TAIL;
      // del buddy_p from list of order x
      list_del(&(buddy_p->list_link));
      buddy_lists[buddy_p->order].nr--;
      // del p from list of order x
      list_del(&(p->list_link));
      buddy_lists[p->order].nr--;
      // change order of p to x+1
      p->order++;
      // append p to the list of order x+1
      lh = &(buddy_lists[p->order].head);
      list_append(&(p->list_link), lh);
      buddy_lists[p->order].nr++;
    } else {
      // if we can't merge, then free is done.
      break;
    }
  }
}

uint64 buddy_statistics() {
  struct free_list* list;
  uint64 csize;
  uint64 total_size = 0;
  for (int i = 0; i < BUDDY_MAX_ORDER; i++) {
    csize = BUDDY_PAGE_SIZE * (1 << i);
    list = &buddy_lists[i];
    total_size += list->nr * csize;
    printf("order: %d, size: 0x%lx, num: %d\n", i, csize, list->nr);
  }
  return total_size;
}

// testcase1 only focus on buddy allocate
// it's expected output is in buddy_test1_bench.txt
void buddy_test1() {
  buddy_statistics();
  printf("1\n");
  buddy_allocate(4097);
  buddy_statistics();
  printf("2\n");
  buddy_allocate(1);
  buddy_statistics();
  printf("3\n");
  buddy_allocate(4097);
  buddy_statistics();
  printf("4\n");
  buddy_allocate(0x1000);
  buddy_statistics();
  printf("5\n");
  // buddy_allocate(0x2000000);
  // buddy_statistics();
  // printf("6\n");
  buddy_allocate(0x1000000);
  buddy_statistics();
  printf("7\n");
  buddy_allocate(0x400000);
  buddy_statistics();
  printf("8\n");
  buddy_allocate(0x400000);
  buddy_statistics();
  printf("9\n");
  buddy_allocate(0x400000);
  buddy_statistics();
  printf("10\n");
  buddy_allocate(0x400000);
  buddy_statistics();
  printf("11\n");
  buddy_allocate(0x400000);
  buddy_statistics();
  printf("12\n");
  buddy_allocate(0x400000);
  buddy_statistics();
  printf("13\n");
}

// testcase2 focus on buddy allocate and free
// it's expected output is in buddy_test2_bench.txt
void buddy_test2() {
  buddy_statistics();
  printf("0\n");
  struct Page* p1 = buddy_allocate(0x1000);
  buddy_statistics();
  printf("1\n");
  struct Page* p2 = buddy_allocate(0x1000);
  buddy_statistics();
  printf("2\n");
  buddy_free_page(p2);
  buddy_statistics();
  printf("3\n");
  buddy_free_page(p1);
  buddy_statistics();
  printf("4\n");
  struct Page* p3 = buddy_allocate(0x1000);
  buddy_statistics();
  printf("5\n");
  struct Page* p4 = buddy_allocate(0x1000);
  buddy_statistics();
  printf("6\n");
  buddy_free_page(p4);
  buddy_statistics();
  printf("7\n");
  buddy_free_page(p3);
  buddy_statistics();
  printf("8\n");
  struct Page* p5 = buddy_allocate(0x8000);
  buddy_statistics();
  printf("9\n");
  struct Page* p6 = buddy_allocate(0x8000);
  buddy_statistics();
  printf("10\n");
  buddy_free_page(p6);  // here should be a merge !
  buddy_statistics();
  printf("11\n");
  buddy_free_page(p5);
  buddy_statistics();
  printf("12\n");
  struct Page* p7 = buddy_allocate(0x8000);
  buddy_statistics();
  printf("13\n");
  struct Page* p8 = buddy_allocate(0x8000);
  buddy_statistics();
  printf("14\n");
  // FIXME: this is a notable error case
  // see buddy.c get_buddy_page for detail
  buddy_free_page(p7);
  buddy_statistics();
  printf("15\n");
  buddy_free_page(p8);
  buddy_statistics();
}

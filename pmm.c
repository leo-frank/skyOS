#include "printf.h"
#include "type.h"
#include "strings.h"
#include "panic.h"
#include "lock.h"
#include "pmm.h"
// TODO: explain char[] matters, char * not.
extern char kernel_start[];
extern char kernel_end[]; // end of .bss
extern char boot_stack_top[];
char *pmm_start, *pmm_end;
#define PGSIZE (1 << 12)
#define mem_size = (1 << 26); 
#define npage (1 << 26) / PGSIZE // # of avliable page

/* Flags describing the status of a page frame */
#define PG_reserved  0       // the page descriptor is reserved for kernel or unusable
#define PG_property  1       // the member 'property' is valid

#define PGADDR(i) pmm_start + i * PGSIZE
#define PGINDEX(addr) (((uint64)addr - (uint64)pmm_start)/PGSIZE)

/* 
 * Now it's a first-fit physical memory allocator.
 * Very simple but low efficient.
 * pages[i] = 0, means i page is unused. 
 */
char pages[npage];
struct spinlock pages_lock;
void pmm_init() {
  printf("kernel_start = %p\n", kernel_start); // 80200000
  printf("kernel_end = 0x%x\n", kernel_end); // 8020b000
  printf("boot_stack_top = 0x%x\n", boot_stack_top); // 8020b000
  pmm_start = kernel_end;
  pmm_end = pmm_start + (1 << 26);  // 64MB mem region
  printf("kernel mem region: [0x%x, 0x%x]\n", kernel_start, kernel_end);
  printf("avliable mem region: [0x%x, 0x%x]\n", pmm_start, pmm_end);
  page_init();
  pmm_test();
}

void page_init() {
  memset(pages, 0, npage);
  init_lock(&pages_lock);
}

/* alloc only one page */
void* kalloc() {
  acquire(&pages_lock);
  for (uint i = 0; i < npage; i ++) {
    if (pages[i] == 0) {
      pages[i] = 1;
      release(&pages_lock);
      return PGADDR(i);
    }
  }
  release(&pages_lock);
  panic("kalloc: No free page\n");
  return NULL;
}

/* free page which pointed by pgaddr  */
void kfree(void *pgaddr) {
  uint i = PGINDEX(pgaddr);
  acquire(&pages_lock);
  pages[i] = 0;
  release(&pages_lock);
}

void pmm_test() {
  char *pg0addr =  kalloc();
  printf("pg0 addr = %p\n", pg0addr);
  printf("pg0 index = %d\n", PGINDEX(pg0addr));
  printf("pg0 attr = %d\n", pages[PGINDEX(pg0addr)]);
  char *pg1addr =  kalloc();
  printf("pg1 addr = %p\n", pg1addr);
  printf("pg1 index = %d\n", PGINDEX(pg1addr));
  printf("pg1 attr = %d\n", pages[PGINDEX(pg1addr)]);
  kfree(pg0addr);
  printf("\nafter free pg0:\n");
  printf("pg0 attr = %d\n", pages[PGINDEX(pg0addr)]);
  printf("pg1 attr = %d\n", pages[PGINDEX(pg1addr)]);
  kfree(pg1addr);
  printf("\nafter free pg0:\n");
  printf("pg0 attr = %d\n", pages[PGINDEX(pg0addr)]);
  printf("pg1 attr = %d\n", pages[PGINDEX(pg1addr)]);
}


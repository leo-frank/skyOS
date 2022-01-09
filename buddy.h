#pragma once

#include "list.h"
#include "type.h"

#define PG_HEAD 1
#define PG_TAIL 2

struct Page {
  uint order;                  // one page
  uint free;                   // allocated or free
  uint flag;                   // PG_HEAD, PG_TAIL
  struct list_head list_link;  // belong to specfic free_list,
                               // also linked with other struct Page
};

void buddy_init();
struct Page* buddy_allocate(uint64 size);
void buddy_free_page(struct Page* p);
uint64 buddy_statistics();
void buddy_test1();
void buddy_test2();
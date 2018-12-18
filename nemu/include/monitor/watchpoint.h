#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  //默认4个字节，以后再改
  //old value
  uint32_t oldvalue;
  uint32_t value;
  //存储表达式
  char exprbuf[65536];

} WP;

#endif

#include "common.h"
extern _Context* do_syscall(_Context *);
extern _Context* schedule(_Context *);

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case 5:{
      printf("_event_yield\n");
      c = schedule(c);
      break;
    }
    case 6:{
      printf("_event_syscall\n");
      c = do_syscall(c);
      break;
    }
    default: panic("Unhandled event ID = %d", e.event);
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}

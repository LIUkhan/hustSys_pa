#include <am.h>
#include <x86.h>
#include"klib.h"

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecnull();

_Context* irq_handle(_Context *tf) {
  _Context *next = tf;
  if (user_handler) {
    _Event ev = {0};
    switch (tf->irq) {
      default: ev.event = _EVENT_ERROR; break;
    }
    printf("esi:0x%08x\n",tf->esi);
    printf("edi:0x%08x\n",tf->edi);
    printf("ebp:0x%08x\n",tf->ebp);
    printf("esp:0x%08x\n",tf->esp);
    printf("ebx:0x%08x\n",tf->ebx);
    printf("edx:0x%08x\n",tf->edx);
    printf("ecx:0x%08x\n",tf->ecx);
    printf("eax:0x%08x\n",tf->eax);
    printf("irq:0x%x\n",tf->irq);
    printf("err:0x%x\n",tf->err);
    printf("eip:0x%08x\n",tf->eip);
    printf("cs:0x%08x\n",tf->cs);
    printf("eflags:0x%08x\n",tf->eflags);
    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }

  return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  return NULL;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}

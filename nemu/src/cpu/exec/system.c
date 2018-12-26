#include "cpu/exec.h"

void difftest_skip_ref();
void difftest_skip_dut();
extern uint32_t pio_read_b(ioaddr_t);
extern uint32_t pio_read_w(ioaddr_t);
extern uint32_t pio_read_l(ioaddr_t);
extern void pio_write_b(ioaddr_t, uint32_t);
extern void pio_write_w(ioaddr_t, uint32_t);
extern void pio_write_l(ioaddr_t, uint32_t);
make_EHelper(lidt) {
  TODO();

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  TODO();

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  TODO();

  print_asm("iret");
}

make_EHelper(in) {
  // TODO();
  uint32_t res;
  switch(id_src->width)
  {
    case 1:{
      res = pio_read_b(id_src->val);
      break;
    }
    case 2:{
      res = pio_read_w(id_src->val);
      break;
    }
    case 4:{
      res = pio_read_l(id_src->val);
      break;
    }
    default: assert(0);
  }
  operand_write(id_dest,&res);
  
  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  // TODO();
  rtlreg_t outdata;
  rtl_lr(&outdata,R_EAX,id_src->width);
  switch(id_dest->width)
  {
    case 1:{
      pio_write_b(id_dest->val, outdata);
      break;
    }
    case 2:{
      pio_write_w(id_dest->val, outdata);
      break;
    }
    case 4:{
      pio_write_l(id_dest->val, outdata);
      break;
    }
    default: assert(0);    
  }
  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

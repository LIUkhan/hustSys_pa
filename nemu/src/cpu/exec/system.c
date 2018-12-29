#include "cpu/exec.h"

void difftest_skip_ref();
void difftest_skip_dut();
extern uint32_t pio_read_b(ioaddr_t);
extern uint32_t pio_read_w(ioaddr_t);
extern uint32_t pio_read_l(ioaddr_t);
extern void pio_write_b(ioaddr_t, uint32_t);
extern void pio_write_w(ioaddr_t, uint32_t);
extern void pio_write_l(ioaddr_t, uint32_t);
extern void raise_intr(uint8_t, vaddr_t);

make_EHelper(lidt) {
  // TODO();
  // rtlreg_t low16,high32,mask,baseaddr;
  // rtl_li(&mask,0xffffff);
  // rtl_li(&baseaddr,id_dest->val+2);
  // rtl_lm(&low16,&(id_dest->val),2);
  // rtl_lm(&high32,&baseaddr,4);
  // rtl_mv(&(cpu.IDTR.limit),&low16);
  
  // if(decoding.is_operand_size_16)   
  //   rtl_and(&high32,&high32,&mask);
  
  // rtl_mv(&(cpu.IDTR.base),&high32);
  rtlreg_t baseaddr;
  rtl_lm(&cpu.IDTR.limit, &(id_dest->val), 2);
  rtl_addi(&baseaddr, &id_dest->addr, 2);
  rtl_lm(&cpu.IDTR.base, &baseaddr, 4);  
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
  // TODO();
  //必须是立即数类型的
  assert(id_dest->type == OP_TYPE_IMM);
  raise_intr(id_dest->val, decoding.seq_eip);
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

#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(decoding.jmp_eip);

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = decoding.opcode & 0xf;
  printf("%x\n",cc);
  rtl_setcc(&t0, cc);
  rtl_li(&t1, 0);
  rtl_jrelop(RELOP_NE, &t0, &t1, decoding.jmp_eip);

  print_asm("j%s %x", get_cc_name(cc), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  rtl_jr(&id_dest->val);

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  // TODO();
  rtlreg_t seq_eip;
  rtl_li(&seq_eip, decoding.seq_eip);
  rtl_push(&seq_eip);
  if(decoding.is_operand_size_16){
  	decoding.jmp_eip &= 0xffff;
  }
  rtl_j(decoding.jmp_eip);
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  // TODO();'
  rtlreg_t tempeip;
  rtl_pop(&tempeip);
  if(decoding.is_operand_size_16)
    tempeip = tempeip & 0xffff;
  rtl_jr(&tempeip);
  print_asm("ret");
}

make_EHelper(call_rm) {
  TODO();

  print_asm("call *%s", id_dest->str);
}

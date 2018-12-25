#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  // TODO();
  rtlreg_t s0 = 0;
  rtlreg_t res = id_dest->val & id_src->val;
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  rtl_update_ZFSF(&res);
  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
  rtlreg_t res,s0 = 0;
  rtl_and(&res,&(id_dest->val),&(id_src->val));
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res);
  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtlreg_t res,s0 = 0;
  rtl_xor(&res,&(id_dest->val),&(id_src->val));
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  operand_write(id_dest, &res);
  rtl_update_ZFSF(&res);
  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
  rtlreg_t res,s0 = 0;
  rtl_or(&res,&(id_dest->val),&(id_src->val));
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  operand_write(id_dest, &res);
  rtl_update_ZFSF(&res);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtlreg_t res;
  rtl_sar(&res,&(id_dest->val),&(id_src->val));
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res);
  print_asm_template2(sar);
}

make_EHelper(shl) {//sal shl have the same effect
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtlreg_t res;
  rtl_shl(&res,&(id_dest->val),&(id_src->val));
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtlreg_t res;
  rtl_shr(&res,&(id_dest->val),&(id_src->val));
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}

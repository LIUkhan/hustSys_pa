#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  // TODO();
  rtlreg_t s0 = 0;
  rtlreg_t res;
  rtl_and(&res,&(id_dest->val),&(id_src->val));
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  // TODO();
  rtlreg_t res,s0 = 0;
  rtl_and(&res,&(id_dest->val),&(id_src->val));
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();
  rtlreg_t res,s0 = 0;
  rtl_xor(&res,&(id_dest->val),&(id_src->val));
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  operand_write(id_dest, &res);
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  // TODO();
  rtlreg_t res,s0 = 0;
  rtl_or(&res,&(id_dest->val),&(id_src->val));
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
  operand_write(id_dest, &res);
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(or);
}

make_EHelper(rol) {
  rtlreg_t sign,newsign,temp,mask,offset,one,CF_c,OF_c;
  rtl_li(&one,1);
  rtl_li(&mask,0x80000000);
  rtl_li(&offset,32-id_dest->val);

  rtl_msb(&sign,&(id_dest->val),id_dest->width);//取出符号位

  rtl_sar(&mask,&mask,&(id_src->val));
  rtl_shl(&mask,&mask,&one);
  rtl_and(&temp,&mask,&(id_dest->val));//取出移位内容

  rtl_shr(&temp,&temp,&offset);
  rtl_shl(&(id_dest->val),&(id_dest->val),&(id_src->val));
  rtl_and(&(id_dest->val),&(id_dest->val),&temp);
  //设置CF
  rtl_and(&CF_c,&(id_dest->val),&one);
  rtl_set_CF(&CF_c);
  if(id_src->val == 1)//设置ＯＦ
  {
    rtl_msb(&newsign,&(id_dest->val),id_dest->width);//取出符号位
    rtl_xor(&OF_c,&sign,&newsign);
    rtl_set_OF(&OF_c);
  }
  print_asm_template2(rol);
}

make_EHelper(sar) {
  // TODO();
  // unnecessary to update CF and OF in NEMUsign
  rtlreg_t res,s_dest;//需要对要扩展的数按照实际宽度做符号扩展
  rtl_sext(&s_dest,&(id_dest->val),id_dest->width);
  rtl_sar(&res,&s_dest,&(id_src->val));
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {//sal shl have the same effect
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtlreg_t res;
  rtl_shl(&res,&(id_dest->val),&(id_src->val));
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtlreg_t res;
  rtl_shr(&res,&(id_dest->val),&(id_src->val));
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  // TODO();
  rtl_not(&(id_dest->val),&(id_dest->val));
  operand_write(id_dest,&(id_dest->val));
  print_asm_template1(not);
}

#include "cpu/exec.h"

make_EHelper(add) {
  rtlreg_t res;
  rtlreg_t CF_c, OF_c, s1,s2;
  rtl_add(&res, &(id_dest->val), &id_src->val);
  rtl_update_ZFSF(&res,id_dest->width);
  //无符号，结果小于加数
  rtl_setrelop(RELOP_LTU, &CF_c, &res, &(id_src->val));
	rtl_set_CF(&CF_c);
	//有符号的小于的判断，设置OF.同符号加法才有可能溢出
	rtl_xor(&s1,&res,&(id_dest->val));
  rtl_xor(&s2,&res,&(id_src->val));
  rtl_and(&OF_c,&s1,&s2);
  rtl_msb(&OF_c, &OF_c, id_dest->width);
  rtl_set_OF(&OF_c);
  operand_write(id_dest,&res);
  print_asm_template2(add);
}

make_EHelper(sub) {
  rtlreg_t result;
  rtlreg_t CF_c, OF_c, ssrc,sdest,sres,rela1,rela2;
  //减法，更新SF和ZF
	rtl_sub(&result, &(id_dest->val), &id_src->val);
	rtl_update_ZFSF(&result,id_dest->width);
	//如果被减数小于减数,无符号的小于判断,无符号只需要考虑一种状况
	rtl_setrelop(RELOP_LTU, &CF_c, &(id_dest->val), &(id_src->val));
	rtl_set_CF(&CF_c);
	//有符号的小于的判断，设置OF.利用的是同符号减法才有可能溢出，利用小于号囊括两种同符号的情况
  rtl_msb(&ssrc,&(id_src->val),id_src->width);
  rtl_msb(&sdest,&(id_dest->val),id_dest->width);
  rtl_msb(&sres,&result,id_dest->width);
  rtl_xor(&rela1,&sres,&sdest);
  rtl_xor(&rela2,&sdest,&ssrc);
  rtl_and(&OF_c,&rela1,&rela2);
  rtl_set_OF(&OF_c);
  operand_write(id_dest,&result);
  print_asm_template2(sub);
}

make_EHelper(cmp) {
  // TODO();
  rtlreg_t result;
  rtlreg_t CF_c, OF_c, ssrc,sdest,sres,rela1,rela2;
  //减法，更新SF和ZF
	rtl_sub(&result, &(id_dest->val), &id_src->val);
	rtl_update_ZFSF(&result,id_dest->width);
	//如果被减数小于减数,无符号的小于判断,无符号只需要考虑一种状况
	rtl_setrelop(RELOP_LTU, &CF_c, &(id_dest->val), &(id_src->val));
	rtl_set_CF(&CF_c);
	//有符号的小于的判断，设置OF.利用的是同符号减法才有可能溢出，利用小于号囊括两种同符号的情况
  rtl_msb(&ssrc,&(id_src->val),id_src->width);
  rtl_msb(&sdest,&(id_dest->val),id_dest->width);
  rtl_msb(&sres,&result,id_dest->width);
  rtl_xor(&rela1,&sres,&sdest);
  rtl_xor(&rela2,&sdest,&ssrc);
  rtl_and(&OF_c,&rela1,&rela2);
  rtl_set_OF(&OF_c);
  print_asm_template2(cmp);
}

make_EHelper(inc) {
  // TODO();
  rtlreg_t res,val,s1,s2,OF_c;
  rtl_li(&val,1);
  rtl_add(&res,&(id_dest->val),&val);
  rtl_update_ZFSF(&res,id_dest->width);
	rtl_xor(&s1,&res,&(id_dest->val));
  rtl_xor(&s2,&res,&val);
  rtl_and(&OF_c,&s1,&s2);
  rtl_msb(&OF_c, &OF_c, id_dest->width);
  rtl_set_OF(&OF_c);
  operand_write(id_dest,&res);
  print_asm_template1(inc);
}

make_EHelper(dec) {
  // TODO();
  rtlreg_t res,val,OF_c,ssrc,sdest,sres,rela1,rela2;
  rtl_li(&val,1);
  rtl_sub(&res,&(id_dest->val),&val);
  rtl_update_ZFSF(&res,id_dest->width);
	//有符号的小于的判断，设置OF.1　< 2,为真１，如果结果为正０，溢出，反之依然
  rtl_msb(&ssrc,&(id_src->val),id_src->width);
  rtl_msb(&sdest,&(id_dest->val),id_dest->width);
  rtl_msb(&sres,&res,id_dest->width);
  rtl_xor(&rela1,&sres,&sdest);
  rtl_xor(&rela2,&sdest,&ssrc);
  rtl_and(&OF_c,&rela1,&rela2);
  rtl_set_OF(&OF_c);
  operand_write(id_dest,&res);
  print_asm_template1(dec);
}

make_EHelper(neg) {
  // TODO();
  rtlreg_t setcf,setof,s0,res,negop;
  rtl_li(&s0,0);
  rtl_li(&negop,-1);
  rtl_setrelop(RELOP_NE,&setcf,&(id_dest->val),&s0);
  rtl_set_CF(&setcf);
  rtl_imul_lo(&res,&(id_dest->val),&negop);
  rtl_update_ZFSF(&res,id_dest->width);
  rtl_setrelop(RELOP_EQ, &setof, &(id_dest->val),&res);
  //不为０，且取反为自身，则溢出
  rtl_and(&setof,&setof,&setcf);
  rtl_set_OF(&setof);
  operand_write(id_dest,&res);
  print_asm_template1(neg);
}

make_EHelper(adc) {
  rtl_add(&t2, &id_dest->val, &id_src->val);
  rtl_setrelop(RELOP_LTU, &t3, &t2, &id_dest->val);
  rtl_get_CF(&t1);
  rtl_add(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2,id_dest->width);

  rtl_setrelop(RELOP_LTU, &t0, &t2, &id_dest->val);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_not(&t0, &t0);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_setrelop(RELOP_LTU, &t3, &id_dest->val, &t2);
  rtl_get_CF(&t1);
  rtl_sub(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2,id_dest->width);

  rtl_setrelop(RELOP_LTU, &t0, &id_dest->val, &t2);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_mul_lo(&t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr(R_AX, &t1, 2);
      break;
    case 2:
      rtl_sr(R_AX, &t1, 2);
      rtl_shri(&t1, &t1, 16);
      rtl_sr(R_DX, &t1, 2);
      break;
    case 4:
      rtl_mul_hi(&t2, &id_dest->val, &t0);
      rtl_sr(R_EDX, &t2, 4);
      rtl_sr(R_EAX, &t1, 4);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_imul_lo(&t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr(R_AX, &t1, 2);
      break;
    case 2:
      rtl_sr(R_AX, &t1, 2);
      rtl_shri(&t1, &t1, 16);
      rtl_sr(R_DX, &t1, 2);
      break;
    case 4:
      rtl_imul_hi(&t2, &id_dest->val, &t0);
      rtl_sr(R_EDX, &t2, 4);
      rtl_sr(R_EAX, &t1, 4);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(&t0, &id_src->val, id_src->width);
  rtl_sext(&t1, &id_dest->val, id_dest->width);

  rtl_imul_lo(&t2, &t1, &t0);
  operand_write(id_dest, &t2);

  print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3) {
  rtl_sext(&t0, &id_src->val, id_src->width);
  rtl_sext(&t1, &id_src2->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul_lo(&t2, &t1, &t0);
  operand_write(id_dest, &t2);

  print_asm_template3(imul);
}

make_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(&t0, R_AX, 2);
      rtl_div_q(&t2, &t0, &id_dest->val);
      rtl_div_r(&t3, &t0, &id_dest->val);
      rtl_sr(R_AL, &t2, 1);
      rtl_sr(R_AH, &t3, 1);
      break;
    case 2:
      rtl_lr(&t0, R_AX, 2);
      rtl_lr(&t1, R_DX, 2);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_div_q(&t2, &t0, &id_dest->val);
      rtl_div_r(&t3, &t0, &id_dest->val);
      rtl_sr(R_AX, &t2, 2);
      rtl_sr(R_DX, &t3, 2);
      break;
    case 4:
      rtl_lr(&t0, R_EAX, 4);
      rtl_lr(&t1, R_EDX, 4);
      rtl_div64_q(&cpu.eax, &t1, &t0, &id_dest->val);
      rtl_div64_r(&cpu.edx, &t1, &t0, &id_dest->val);
      break;
    default: assert(0);
  }

  print_asm_template1(div);
}

make_EHelper(idiv) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(&t0, R_AX, 2);
      rtl_idiv_q(&t2, &t0, &id_dest->val);
      rtl_idiv_r(&t3, &t0, &id_dest->val);
      rtl_sr(R_AL, &t2, 1);
      rtl_sr(R_AH, &t3, 1);
      break;
    case 2:
      rtl_lr(&t0, R_AX, 2);
      rtl_lr(&t1, R_DX, 2);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_idiv_q(&t2, &t0, &id_dest->val);
      rtl_idiv_r(&t3, &t0, &id_dest->val);
      rtl_sr(R_AX, &t2, 2);
      rtl_sr(R_DX, &t3, 2);
      break;
    case 4:
      rtl_lr(&t0, R_EAX, 4);
      rtl_lr(&t1, R_EDX, 4);
      rtl_idiv64_q(&cpu.eax, &t1, &t0, &id_dest->val);
      rtl_idiv64_r(&cpu.edx, &t1, &t0, &id_dest->val);
      break;
    default: assert(0);
  }

  print_asm_template1(idiv);
}

make_EHelper(xchg)
{
  rtlreg_t temp;
  rtl_lr(&temp,id_dest->reg,id_dest->width);
  rtl_sr(id_dest->reg,&(cpu.eax),id_dest->width);
  rtl_sr(0,&temp,id_dest->width);
  print_asm_template1(xchg);
}
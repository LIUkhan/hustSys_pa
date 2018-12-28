#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&(id_dest->val));
  print_asm_template1(push);
}
//有可能pop到reg，也有可能pop到memory
make_EHelper(pop) {
  rtlreg_t temp;
  rtl_pop(&temp);
  operand_write(id_dest, &temp);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  // TODO();
  rtl_lr(&t0,R_ESP,4);
  rtl_push(&(cpu.eax));
  rtl_push(&(cpu.ecx));
  rtl_push(&(cpu.edx));
  rtl_push(&(cpu.ebx));
  rtl_push(&t0);
  rtl_push(&(cpu.ebp));
  rtl_push(&(cpu.esi));
  rtl_push(&(cpu.edi));
  print_asm("pusha");
}

make_EHelper(popa) {
  // TODO();
  rtl_pop(&(cpu.edi));
  rtl_pop(&(cpu.esi));
  rtl_pop(&(cpu.ebp));
  rtl_pop(&t0);
  rtl_pop(&(cpu.ebx));
  rtl_pop(&(cpu.edx));
  rtl_pop(&(cpu.ecx));
  rtl_pop(&(cpu.eax));
  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}
//CWD/CDQ
make_EHelper(cltd) {
  rtlreg_t immreg;
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_lr(&t0,R_AX,2);
    rtl_setrelopi(RELOP_LT,&t1,&t0,0);
    if(t1 == 1) {
      rtl_li(&immreg,0xffff);
      rtl_sr(R_DX,&immreg,2);
    }
    else {
      rtl_li(&immreg,0);
      rtl_sr(R_DX,&immreg,2);
    }
  }
  else {
    // TODO();
    rtl_lr(&t0,R_EAX,4);
    rtl_setrelopi(RELOP_LT,&t1,&t0,0);
    if(t1 == 1) {
      rtl_li(&immreg,0xffffffff);
      rtl_sr(R_EDX,&immreg,4);
    }
    else {
      rtl_li(&immreg,0);
      rtl_sr(R_EDX,&immreg,4);
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}
//CBW/CWDE 
make_EHelper(cwtl) {
  rtlreg_t res;
  if (decoding.is_operand_size_16) {
    // TODO();AX := SignExtend(AL);
    rtl_lr(&t0,R_AL,1);
    rtl_sext(&res,&t0,1);
    rtl_sr(R_AX,&res,2);
  }
  else {
    // TODO();EAX := SignExtend(AX);
    rtl_lr(&t0,R_AX,2);
    rtl_sext(&res,&t0,2);
    rtl_sr(R_EAX,&res,4);
  }
  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}
//读出来设置了width为1或2，使用时应该改width，使得其扩展为16/32位的无符号数
make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}

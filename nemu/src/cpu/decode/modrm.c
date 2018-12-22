#include "cpu/exec.h"
#include "cpu/rtl.h"
//索引模式,24种情况
void load_addr(vaddr_t *eip, ModR_M *m, Operand *rm) {
  assert(m->mod != 3);
  //displacement偏移量
  int32_t disp = 0;
  int disp_size = 4;//偏移量大小
  int base_reg = -1, index_reg = -1, scale = 0;
  rtl_li(&t0, 0);
  //rm区==esp，则有sib字节信息，需要从计算地址
  if (m->R_M == R_ESP) {
    SIB s;
    s.val = instr_fetch(eip, 1);
    base_reg = s.base;
    scale = s.ss;

    if (s.index != R_ESP) { index_reg = s.index; }
  }
  else {
    /* no SIB */
    base_reg = m->R_M;
  }

  if (m->mod == 0) {
    if (base_reg == R_EBP) { base_reg = -1; }
    else { disp_size = 0; }
  }
  else if (m->mod == 1) { disp_size = 1; }
  //t0累计着最后的地址
  if (disp_size != 0) {
    /* has disp */
    disp = instr_fetch(eip, disp_size);
    if (disp_size == 1) { disp = (int8_t)disp; }

    rtl_addi(&t0, &t0, disp);
  }
  //有base——reg的情况
  if (base_reg != -1) {
    rtl_add(&t0, &t0, &reg_l(base_reg));
  }

  if (index_reg != -1) {
    rtl_shli(&t1, &reg_l(index_reg), scale);
    rtl_add(&t0, &t0, &t1);
  }
  rtl_mv(&rm->addr, &t0);

#ifdef DEBUG
  char disp_buf[16];
  char base_buf[8];
  char index_buf[8];

  if (disp_size != 0) {
    /* has disp */
    sprintf(disp_buf, "%s%#x", (disp < 0 ? "-" : ""), (disp < 0 ? -disp : disp));
  }
  else { disp_buf[0] = '\0'; }

  if (base_reg == -1) { base_buf[0] = '\0'; }
  else { 
    sprintf(base_buf, "%%%s", reg_name(base_reg, 4));
  }

  if (index_reg == -1) { index_buf[0] = '\0'; }
  else { 
    sprintf(index_buf, ",%%%s,%d", reg_name(index_reg, 4), 1 << scale);
  }

  if (base_reg == -1 && index_reg == -1) {
    sprintf(rm->str, "%s", disp_buf);
  }
  else {
    sprintf(rm->str, "%s(%s%s)", disp_buf, base_buf, index_buf);
  }
#endif

  rm->type = OP_TYPE_MEM;
}

void read_ModR_M(vaddr_t *eip, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  ModR_M m;
  m.val = instr_fetch(eip, 1);//获取modrm字节
  decoding.ext_opcode = m.opcode;//补充的extra opcode
  //reg区可以索引reg，或者做opcode的补充,reg操作数指针不是null，则看是否要取reg出来
  if (reg != NULL) {
    reg->type = OP_TYPE_REG;
    reg->reg = m.reg;//选择寄存器
    if (load_reg_val) {
      rtl_lr(&reg->val, reg->reg, reg->width);
    }

#ifdef DEBUG
    snprintf(reg->str, OP_STR_SIZE, "%%%s", reg_name(reg->reg, reg->width));
#endif
  }
  //rm区和mod区一起构成指向8个寄存器或者24个索引,mod区为3，rm区选择寄存器
  if (m.mod == 3) {
    rm->type = OP_TYPE_REG;
    rm->reg = m.R_M; //寄存器由rm区决定
    if (load_rm_val) {
      rtl_lr(&rm->val, m.R_M, rm->width);
    }

#ifdef DEBUG
    sprintf(rm->str, "%%%s", reg_name(m.R_M, rm->width));
#endif
  }
  else {//rm区构成索引模式
    load_addr(eip, &m, rm);//这个函数很复杂啊
    if (load_rm_val) {
      rtl_lm(&rm->val, &rm->addr, rm->width);
    }
  }
}

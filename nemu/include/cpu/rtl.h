#ifndef __CPU_RTL_H__
#define __CPU_RTL_H__

#include "nemu.h"
#include "util/c_op.h"
#include "cpu/relop.h"
#include "cpu/rtl-wrapper.h"

extern rtlreg_t t0, t1, t2, t3, at;

void decoding_set_jmp(bool is_jmp);
bool interpret_relop(uint32_t relop, const rtlreg_t src1, const rtlreg_t src2);

/* RTL basic instructions */

static inline void interpret_rtl_li(rtlreg_t* dest, uint32_t imm) {
  *dest = imm;
}

static inline void interpret_rtl_mv(rtlreg_t* dest, const rtlreg_t *src1) {
  *dest = *src1;
}
// ##连接字符
#define make_rtl_arith_logic(name) \
  static inline void concat(interpret_rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  } \
  /* Actually those of imm version are pseudo rtl instructions,
   * but we define them here in the same macro */ \
  static inline void concat(rtl_, name ## i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
    rtl_li(&at, imm); \
    rtl_ ## name (dest, src1, &at); \
  }
//定义了rtl基本指令和伪指令中的这些函数，伪指令的关于立即数的都定义完成
make_rtl_arith_logic(add)
make_rtl_arith_logic(sub)
make_rtl_arith_logic(and)
make_rtl_arith_logic(or)
make_rtl_arith_logic(xor)
make_rtl_arith_logic(shl)
make_rtl_arith_logic(shr)
make_rtl_arith_logic(sar)
make_rtl_arith_logic(mul_lo)
make_rtl_arith_logic(mul_hi)
make_rtl_arith_logic(imul_lo)
make_rtl_arith_logic(imul_hi)
make_rtl_arith_logic(div_q)
make_rtl_arith_logic(div_r)
make_rtl_arith_logic(idiv_q)
make_rtl_arith_logic(idiv_r)

static inline void interpret_rtl_div64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void interpret_rtl_div64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline void interpret_rtl_idiv64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void interpret_rtl_idiv64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline void interpret_rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
  *dest = vaddr_read(*addr, len);
}

static inline void interpret_rtl_sm(const rtlreg_t* addr, const rtlreg_t* src1, int len) {
  vaddr_write(*addr, *src1, len);//虚拟地址转物理地址用memcpy写入对应长度在物理地址
}
//the result is 32bits,for the right of = select lower bits of the number,the whole number become the lower bits
static inline void interpret_rtl_host_lm(rtlreg_t* dest, const void *addr, int len) {
  switch (len) {
    case 4: *dest = *(uint32_t *)addr; return;
    case 1: *dest = *( uint8_t *)addr; return;
    case 2: *dest = *(uint16_t *)addr; return;
    default: assert(0);
  }
}

static inline void interpret_rtl_host_sm(void *addr, const rtlreg_t *src1, int len) {
  switch (len) {
    case 4: *(uint32_t *)addr = *src1; return;
    case 1: *( uint8_t *)addr = *src1; return;
    case 2: *(uint16_t *)addr = *src1; return;
    default: assert(0);
  }
}
//逻辑表达式
static inline void interpret_rtl_setrelop(uint32_t relop, rtlreg_t *dest,
  const rtlreg_t *src1, const rtlreg_t *src2) {
  *dest = interpret_relop(relop, *src1, *src2);
}
//跳到指定地址
static inline void interpret_rtl_j(vaddr_t target) {
  cpu.eip = target;
  decoding_set_jmp(true);
}
//跳到寄存器指定地址
static inline void interpret_rtl_jr(rtlreg_t *target) {
  cpu.eip = *target;
  decoding_set_jmp(true);
}
//有条件跳转
static inline void interpret_rtl_jrelop(uint32_t relop,
  const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
  bool is_jmp = interpret_relop(relop, *src1, *src2);
  if (is_jmp) cpu.eip = target;
  decoding_set_jmp(is_jmp);
}

void interpret_rtl_exit(int state);


/* RTL pseudo instructions */
//通用寄存器访问，guest to host虚拟地址转物理地址，host to guest物理地址转虚拟地址，在memory.h文件中
static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}
//host可以只写对应的字节而不会覆盖后面的，其他的会！！！！
static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}
//按位取反
static inline void rtl_not(rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  *dest = ~(*src1); 
}

//符号扩展为32位
static inline void rtl_sext(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  uint32_t offset = (4 - width) * 8;//前面少的位数
  int32_t temp = (*src1)<<offset;
  *dest = temp>>offset;
}

void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  reg_l(R_ESP) -= 4;
  rtl_sm(&cpu.esp, src1, 4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(dest, &cpu.esp, 4);
  reg_l(R_ESP) += 4;
}

static inline void rtl_setrelopi(uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, int imm) {
  // dest <- (src1 relop imm ? 1 : 0)
  rtl_li(&at,imm);
  rtl_setrelop(relop,dest,src1,&at);
}
//取出符号位
static inline void rtl_msb(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  uint32_t offset = 8 * width-1;
  uint32_t temp = (*src1)>>offset;
  *dest = temp & 0x1;
}
//可能换成位运算
#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.eflags._ ## f = (*src) ? 1 : 0;\
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    rtlreg_t temp = 0;\
    *dest = temp | cpu.eflags._ ## f;\
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

//是否为0标志位
static inline void rtl_update_ZF(const rtlreg_t* result,int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  // TODO();
  switch(width)
  {
    case 1:{
      rtlreg_t aimm;
      rtl_li(&aimm,0xff);
      rtl_and(&at,&aimm,result);
      break;
    }
    case 2:{
      rtlreg_t aimm;
      rtl_li(&aimm,0xffff);
      rtl_and(&at,&aimm,result);
      break;
    }
    case 4:{
      rtlreg_t aimm;
      rtl_li(&aimm,0xffffffff);
      rtl_and(&at,&aimm,result);
      break;
    }
    default: assert(0);
  }
  cpu.eflags._ZF = (at == 0) ? 1 : 0;
}

static inline void rtl_update_SF(const rtlreg_t* result,int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  // TODO();
  rtl_msb(&at,result,width);
  cpu.eflags._SF = (at == 1) ? 1 : 0;
}

static inline void rtl_update_ZFSF(const rtlreg_t* result,int width) {
  rtl_update_ZF(result,width);
  rtl_update_SF(result,width);
}

#endif

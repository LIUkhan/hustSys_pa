#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  assert(NO >= 0);//下标索引需要大于０
  assert(NO * 8u + 7u < cpu.IDTR.limit);//不能超过ＩＤＴ限制的最大地址
  rtlreg_t addr,loff,hoff,mask1,mask2,offset;
  rtlreg_t cons1,cons2;
  rtl_li(&cons1,NO * 8);
  rtl_li(&cons2,4);
  // 依次将EFLAGS, CS(代码段寄存器), EIP寄存器的值压栈
  rtl_push(&(cpu.eflags.content));
  rtl_push(&cpu.cs);
	rtlreg_t seqEIP;
	rtl_li(&seqEIP, decoding.seq_eip);//指令解析完毕后指向下一条指令，exec_wrapper函数和update_eip可以看出
	rtl_push(&seqEIP);  
  //gate地址, 从IDTR中读出IDT的首地址,根据异常号在IDT中进行索引, 找到一个门描述符
	rtl_add(&addr, &cpu.IDTR.base,&cons1);
	// 将门描述符中的offset域组合成目标地址
	rtl_lm(&loff, &addr, 4);	
	rtl_add(&addr, &addr,&cons2);
	rtl_lm(&hoff, &addr, 4); 
  //检查Present位，看门描述符空闲还是占用，１为占用，因为初始化为１
  assert(((hoff >> 15) & 1) == 1);
  //检查ｃｓ是否为８
  assert( (loff >> 16) == 0x8);
  rtl_li(&mask1, 0x0000ffff);
  rtl_li(&mask2, 0xffff0000);
	rtl_and(&mask1, &mask1, &loff);
	rtl_and(&mask2, &mask2, &hoff);  
 
  rtl_or(&offset, &mask1, &mask2);
  printf("%x\n",offset);  
  // 跳转到目标地址
  rtl_j(offset);
  // TODO();
}

void dev_raise_intr() {
}

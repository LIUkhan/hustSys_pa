#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };
  rtlreg_t temp,temp1,temp2;
  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O: {
      rtl_get_OF(&temp);
      *dest = (temp == 1) ? 1 : 0;
    }
    case CC_B: {
      rtl_get_CF(&temp);
      *dest = (temp == 1) ? 1 : 0;
    }
    case CC_E: {
      rtl_get_ZF(&temp);
      *dest = (temp == 1) ? 1 : 0;
    }
    case CC_BE: {
      rtl_get_ZF(&temp1);
      rtl_get_CF(&temp2);
      *dest = ((temp1 == 1) || (temp2 == 1)) ? 1 : 0;
    }
    case CC_S: {
      rtl_get_SF(&temp);
      *dest = (temp == 1) ? 1 : 0;
    }
    case CC_L: {
      rtl_get_SF(&temp1);
      rtl_get_OF(&temp2);
      *dest = (temp1 != temp2) ? 1 : 0;
    }
    case CC_LE: {
      rtl_get_ZF(&temp);
      rtl_get_SF(&temp1);
      rtl_get_OF(&temp2);
      *dest = (temp == 1 || (temp1 != temp2)) ? 1 : 0;
    }
      // TODO();
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}

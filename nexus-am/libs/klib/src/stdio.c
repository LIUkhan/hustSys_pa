#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void i2a(int num,int nindex,char *out)
{
  int low = num%10;
  while(low != 0)
  {
    out[nindex++] = 48 + low;
    num/=10;
    low = num%10;
  }
  out[nindex] = '\0';
  return;
}

void u2a(uint32_t num,int nindex,char *out)
{
  uint32_t low = num%10;
  while(low != 0)
  {
    out[nindex++] = 48 + low;
    num/=10;
    low = num%10;
  }
  out[nindex] = '\0';
  return;
}

int printf(const char *fmt, ...) {
  // va_list ap;
  // int cnt;
  // va_start(ap,fmt);
  // cnt = vprintf(fmt,ap);
  // va_end(ap);
  return 0;
}



int vsprintf(char *out, const char *fmt, va_list ap) {
  int len = strlen(fmt),index = strlen(out);
  for(int i = 0; i < len; i++) {
    if(fmt[i] == '%')
    {
      char c = fmt[++i];
      switch(c)
      {
        case 's':{
          char *str = va_arg(ap,char *);
          strcat(out,str);
          i++;//离开’ｓ‘
          break;
        }
        case 'c':{
          int num = va_arg(ap,int);
          int nindex = strlen(out);
          out[nindex++] = (char)num;
          out[index] = '\0';
          i++;
          break;
        }
        case 'd':{
          int num = va_arg(ap,int);
          int nindex = strlen(out);
          i2a(num,nindex,out);
          i++;
          break;
        }
        case 'u':{
          uint32_t num = va_arg(ap,uint32_t);
          int nindex = strlen(out);
          u2a(num,nindex,out);
          i++;
          break;
        }
        default:{
          break;
        }
      }
    }
    index = strlen(out);
    out[index++] = fmt[i];
    out[index] = '\0';
  }
  return index;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  int cnt;
  va_start(ap,fmt);
  cnt = vsprintf(out,fmt,ap);
  va_end(ap);
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  char buf[65535];
  va_start(ap,fmt);
  vsprintf(buf,fmt,ap);
  if(strlen(buf) > n)
    buf[n] = '\0';
  strcpy(out,buf);
  va_end(ap);
  return n;
}

#endif


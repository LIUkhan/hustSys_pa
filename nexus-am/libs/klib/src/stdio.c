#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void i2a(int num,int nindex,char *out)
{
  int i = 0,len;
  char numbuf[2000];
  int low = num%10;
  if(num == 0)
    numbuf[i++] = 48;
  else{
    while(num != 0)
    {
      numbuf[i++] = 48 + low;
      num /= 10;
      low = num%10;
    }  
  }
  numbuf[i] = '\0';
  len = strlen(numbuf);
  for(int j = len-1; j >= 0; j--)
    out[nindex++] = numbuf[j];
  out[nindex] = '\0';
  return;
}

void u2a(uint32_t num,int nindex,char *out)
{
  int i = 0,len;
  char numbuf[2000];
  uint32_t low = num%10;
  if(num == 0)
    numbuf[i++] = 48;
  else{
    while(num != 0)
    {
      numbuf[i++] = 48 + low;
      num /= 10;
      low = num%10;
    }  
  }
  numbuf[i] = '\0';
  len = strlen(numbuf);
  for(int j = len-1; j >= 0; j--)
    out[nindex++] = numbuf[j];
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
  if(out == NULL)
    return -1;
  int len = strlen(fmt),index = 0;
  out[0] = '\0';
  for(int i = 0; i < len; i++) {
    if(fmt[i] == '%')
    {
      char c = fmt[i+1];
      switch(c)
      {
        case 's':{
          char *str = va_arg(ap,char *);
          strcat(out,str);
          i+=2;//离开’ｓ‘
          break;
        }
        case 'd':{
          int num = va_arg(ap,int);
          int nindex = strlen(out);
          i2a(num,nindex,out);
          i+=2;
          break;
        }
        case 'u':{
          uint32_t num = (uint32_t)va_arg(ap,int);
          int nindex = strlen(out);
          u2a(num,nindex,out);
          i+=2;
          break;
        }
        default:{
          break;
        }
      }
    }
    index = strlen(out);
    if(i >= len) {
      out[index] = '\0';
      break;
    }
    out[index++] = fmt[i];
    out[index] = '\0';
  };
  return index;
}

int sprintf(char *out, const char *fmt, ...) {
  if(out == NULL)
    return -1;
  va_list ap;
  int cnt;
  va_start(ap,fmt);
  cnt = vsprintf(out,fmt,ap);
  va_end(ap);
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  if(out == NULL)
    return -1;
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


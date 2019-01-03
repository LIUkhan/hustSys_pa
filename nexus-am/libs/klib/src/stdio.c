#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

typedef struct info{
  uint32_t addzero;
  uint32_t width;
  uint32_t precise;
  char iden;
  uint32_t step;
}info;

void recognize(const char *out,int index,info *r)
{
  r->addzero = 0;
  r->width = 0;//0不限制长度
  r->precise = 6;
  r->step = 2;//默认跳过标识符和％
  if(out[index] == '0')
  {
    r->addzero = 1;
    index++;
    r->step++;
  }
  while(out[index] >= '0' && out[index] <= '9')
  {
    r->width += r->width *10 + out[index]-48;
    index++;  r->step++;
  }
  if(out[index] >= 'a' && out[index] <= 'z')
    r->iden = out[index++];
  return;
}

void _puts(char *str)
{
  int len = strlen(str);
  for(int i = 0; i < len; i++) {
    _putc(str[i]);
  }
}

char hexnum(uint32_t num)
{
  switch(num)
  {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'a';
    case 11: return 'b';
    case 12: return 'c';
    case 13: return 'd';
    case 14: return 'e';
    case 15: return 'f';
    default:  assert(0);
  }
}

void uori2a(int num,int nindex,char *out,int *index,info form,int type)
{
  int i = 0,len;
  char numbuf[2000];
  if(type == 0) {
    int addsign = 0;
    if(num < 0) {
      addsign = 1;
      num *= -1;
    }
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
    if(addsign)
      numbuf[i++] = '-';
  }
  else if(type == 1)
  {
    uint32_t unum = (uint32_t)num;
    uint32_t low = unum%10;
    if(unum == 0)
      numbuf[i++] = 48;
    else{
      while(unum != 0)
      {
        numbuf[i++] = 48 + low;
        unum /= 10;
        low = unum%10;
      }  
    }
  }
  else if(type == 2)
  {
    uint32_t unum = (uint32_t)num;
    uint32_t low = unum%16;
    char cw = hexnum(low);
    if(unum == 0)
      numbuf[i++] = cw;
    else{
      while(unum != 0)
      {
        numbuf[i++] = cw;
        unum /= 16;
        low = unum%16;
        cw = hexnum(low);
      }  
    }
  }
  numbuf[i] = '\0';
  if(i < form.width)
  {
    int subres = form.width - i;
    if(form.addzero)
      for(int j = 0; j < subres; j++)
        numbuf[i+j] = '0';
    else
      for(int j = 0; j < subres; j++)
        numbuf[i+j] = ' ';
    numbuf[i+subres] = '\0';
  }
  len = strlen(numbuf);
  if(out == NULL) {
    for(int j = len-1; j >= 0; j--)
      _putc(numbuf[j]);
    *index += len;
  }
  else {
    for(int j = len-1; j >= 0; j--)
      out[nindex++] = numbuf[j];
    out[nindex] = '\0';
  }
  return;
}

int printf(const char *fmt, ...) {
  va_list ap;
  int cnt;
  va_start(ap,fmt);
  cnt = vsprintf(NULL,fmt,ap);
  va_end(ap);
  return cnt;
}


//out为NULL时用_putc输出
int vsprintf(char *out, const char *fmt, va_list ap) {
  int len = strlen(fmt),index = 0;
  if(out != NULL)
    out[0] = '\0';
  for(int i = 0; i < len; i++) {
    if(fmt[i] == '%')
    {
      info form;
      recognize(fmt,i+1,&form);
      char tempstr[2000];
      switch(form.iden)
      {
        case 's':{
          char *str = va_arg(ap,char *);
          uint32_t lenstr = strlen(str);
          if(lenstr < form.width)
          {
            int subres = form.width - lenstr;
            if(form.addzero)
              for(int j = 0; j < subres; j++)
                tempstr[j] = '0';
            else
              for(int j = 0; j < subres; j++)
                tempstr[j] = ' ';
            tempstr[subres] = '\0';
            strcat(tempstr,str);
          }
          else
            strcpy(tempstr,str);
          if(out != NULL)
            strcat(out,tempstr);
          else {
            _puts(tempstr);
            index += strlen(tempstr);
          }
          i+=form.step;//离开’ｓ‘
          break;
        }
        case 'd':{
          int num = va_arg(ap,int);
          int nindex = strlen(out);
          uori2a(num,nindex,out,&index,form,0);
          i+=form.step;
          break;
        }
        case 'u':{
          int num = va_arg(ap,int);//在函数内部转为ｕｉｎｔ
          int nindex = strlen(out);
          uori2a(num,nindex,out,&index,form,1);
          i+=form.step;
          break;
        }
        case 'x': {
          int num = va_arg(ap,int);//在函数内部转为ｕｉｎｔ
          int nindex = strlen(out);
          uori2a(num,nindex,out,&index,form,2);
          i+=form.step;
          break;
        }
        case 'p': {
          int num = va_arg(ap,int);//在函数内部转为ｕｉｎｔ
          int nindex = strlen(out);
          uori2a(num,nindex,out,&index,form,2);
          i+=form.step;
          break;
        }
        case 'c':{
          int num = va_arg(ap,int);
          char cw = (char)num;
          if(form.width > 1)
          {
            int subres = form.width - 1;
            if(form.addzero)  
              for(int j = 0; j < subres; j++)
                tempstr[j] = '0';
            else
              for(int j = 0; j < subres; j++)
                tempstr[j] = ' ';
            tempstr[subres] = cw;
            tempstr[subres+1] = '\0';
          }
          else {
            tempstr[0] = cw;
            tempstr[1] = '\0';
          }
          if(out != NULL)
            strcat(out,tempstr);
          else {
            _puts(tempstr);
            index += strlen(tempstr);
          }
          i+=form.step;
          break;
        }
        default:{
          _putc('N');
          _putc('\n');
          break;
        }
      }
    }
    if(out != NULL)
    {
      if(i >= len)
        break;
      index = strlen(out);
      out[index++] = fmt[i];
      out[index] = '\0';
    }
    else {
      if(i >= len) 
        break;
      _putc(fmt[i]);
      index++;
    }
  }
  return index;
}

int sprintf(char *out, const char *fmt, ...) {
  if(out == NULL)
    return -1;
  va_list ap;
  int cnt;
  va_start(ap,fmt);
  cnt = vsprintf(out,fmt,ap);
  // _puts(out);
  va_end(ap);
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  if(out == NULL)
    return -1;
  va_list ap;
  char buf[65535];
  va_start(ap,fmt);
  int cnt = vsprintf(buf,fmt,ap);
  _putc('1');
  if(cnt > n) {
    buf[n-1] = '\0';
    cnt = n;
  }
  _putc('2');
  strcpy(out,buf);
  va_end(ap);
  return cnt;
}

#endif


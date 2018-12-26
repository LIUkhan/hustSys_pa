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

void recognize(char *out,int index,info *r)
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
  while(out[index] >= '1' && out[index] <= '9')
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


void i2a(int num,int nindex,char *out,int *index,info form)
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
  if(len < form.width)
  {
    int subres = form.width - len;
    if(form.addzero)
      for(int j = 0; j < subres; j++)
        numbuf[len+j] = '0';
    else
      for(int j = 0; j < subres; j++)
        numbuf[len+j] = ' ';
    numbuf[len+subres] = '\0';
  }
  if(out == NULL)
  {
    for(int j = len-1; j >= 0; j--)
      _putc(numbuf[j]);
    *index+=strlen(numbuf);
  }
  else
  {
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
      recognize(out,index,&form);
      switch(form.iden)
      {
        case 's':{
          char *str = va_arg(ap,char *);
          uint32_t lenstr = strlen(str);
          char tempstr[65536];
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
            _puts(str);
            index += strlen(tempstr);
          }
          i+=form.step;//离开’ｓ‘
          break;
        }
        case 'd':{
          int num = va_arg(ap,int);
          int nindex = strlen(out);
          i2a(num,nindex,out,&index,form);
          i+=form.step;
          break;
        }
        default:{
          break;
        }
      }
    }
    if(out != NULL)
    {
      index = strlen(out);
      if(i >= len) {
        out[index] = '\0';
        break;
      }
      out[index++] = fmt[i];
      out[index] = '\0';
    }
    else {
      if(i >= len) 
        break;
      _putc(fmt[i]);
      index++;
    }
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
  int len = strlen(out);
  for(int i = 0; i < len; i++) {
    _putc(out[i]);
  }
  _putc('\n');
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


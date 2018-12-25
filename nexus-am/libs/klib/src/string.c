#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t index = 0;
  if(s == NULL)
    return 0;
  while(s[index] != '\0')
    index++;
  return index;
}

char *strcpy(char* dst,const char* src) {
  size_t index = 0;
  if(dst == NULL || src == NULL)
    return NULL;
  while(src[index] != '\0')
  {
    dst[index] = src[index];
    index++;
  }
  dst[index] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  if(dst == NULL || src == NULL)
    return NULL;
  for(int i = 0; i < n; i++)
  {
    dst[i] = src[i];
  }
  dst[n] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  if(dst == NULL)
    return NULL;
  size_t sindex = strlen(dst);
  strcpy(dst+sindex,src);
  return dst; 
}

int strcmp(const char* s1, const char* s2) {
  size_t n1 = strlen(s1);
  size_t n2 = strlen(s2);
  size_t n = n1 > n2 ? n2 : n1;
  for(int i = 0; i < n; i++) {
    if(s1[i] == s2[i])
      continue;
    else
      return s1[i] - s2[i];
  }
  if(n1 > n2)
    return (int)s1[n2];
  else if(n1 < n2)
    return (int)s2[n1];
  else
    return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  for(int i = 0; i < n; i++) {
    if(s1[i] == s2[i])
      continue;
    else
      return s1[i] - s2[i];
  }
  return 0;
}
//按字节设置
void* memset(void* v,int c,size_t n) {
  for(int i = 0; i < n; i++) {
    ((char* )v)[i] = (char)c;
  }
  return v;
}
//四个四个字节拷贝
void* memcpy(void* out, const void* in, size_t n) {
  for(int i = 0; i < n/4; i++){
    ((uint32_t*)out)[i] = ((uint32_t*)in)[i]; 
  }
  size_t yu = n % 4;
  for(int i = 0; i < yu; i++) {
    ((char* )out)[n-yu+i] = ((char*)in)[n-yu+i]; 
  }
  return out;
}
//按字节对比
int memcmp(const void* s1, const void* s2, size_t n){
  const char* cleft = (const char*)s1;
  const char* cright = (const char*)s2;
  for(int i = 0; i < n; i++) {
    if(cleft[i] == cright[i])
      continue;
    else
      return cleft[i] - cright[i];
  }
  return 0;
}

#endif

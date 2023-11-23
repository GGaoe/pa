#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {

  size_t len=0;
  while(*s !='\0'){len++;s++;}
  return len;
}

char *strcpy(char *dst, const char *src) {
  
  char *addr=dst;
  //char *src1=src;
  while(*src!='\0'){
    *dst=*src;
    dst++,src++;
  }
  *dst=*src;
  return addr;
}

char *strncpy(char *dst, const char *src, size_t n) {
  
  char *addr=dst;
  //char *src1=src;
  while(n--){
   if(*src!='\0') *dst=*src;
   else *dst='\0';
    dst++;
    src++;
  }
  return addr;
}

char *strcat(char *dst, const char *src) {
  
  char *addr=dst;
  //char *src1=src;
  while(*dst!='\0')dst++;
  while(*src!='\0'){
    *dst=*src;
    dst++,src++;
  }
  return addr;
}

int strcmp(const char *s1, const char *s2) {
  
  //char *s11=s1;
  //char *s21=s2;
  while(*s1 && * s2 && (*s1==*s2)){
    s1++;s2++;
  }
  return (*s1-*s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  
  //char *s11=s1;
  //char *s21=s2;
  while(*s1 && * s2 && (*s1==*s2) && n--){
    s1++;s2++;
  }
  return (*s1-*s2);
}

void *memset(void *s, int c, size_t n) {
  
  char *s1=(char *)s;
  while(n--){
    *s1++=(char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  
  void *addr=dst;
  char *dst1=(char *)dst+n-1;
  //char *src1=(char *)src+n-1;
  src+=n-1;
  while(n--){
    *dst1--=*(char *)src--;
  }
  return addr;
}//////////////////////

void *memcpy(void *out, const void *in, size_t n) {
  
  void *addr=out;
  char *out1=(char *)out;
  //char *in1=(char *)in;
  while(n--){
    *out1++=*(char *)in++;
  }
  return addr;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  
  //char *s11=(char *)s1;
  //char *s21=(char *)s2;
  while(n--){
    if(*(char *)s1 && *(char *)s2 && (*(char *)s1==*(char *)s2)){
      s1++;
      s2++;
    }
    else return (*(char *)s1-*(char *)s2);
  }
  return 0;
}

#endif
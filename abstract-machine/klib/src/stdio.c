#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args,fmt);
  char *out1=out;
  const char *format=fmt;
  while(*format){
    if(*format=='%'){
      format++;
      switch(*format){
        case 's':{
          const char *c=va_arg(args, const char *);
          while(*c){
            *out1++=*c++;
          }
          break;
          }

        case 'd':{
          int num=va_arg(args,int);
          if(num<0){
            *out1++='-';
            num=-num;
          }
          char buf[30];
          char buf_n[30];
          int len=0;
          while(num!=0){
            buf[len++]=num%10+'0';
            num=num/10;
          }
         
          for(int i=0;i<len;i++){
            buf_n[len-i-1]=buf[i];
          }
          for(int i=0;i<len;i++){
            *out1++=buf_n[i];
          }
          break;
          }

        default:{
          *out1++=*format;
          break;
        }
      }
    }
    else {
      *out1++=*format;
    }
    format++;
  }
   *out1='\0';
    va_end(args);
  return out1-out;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
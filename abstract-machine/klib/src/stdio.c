#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buf[1024];
  va_list args;
  va_start(args,fmt);

  int content=vsnprintf(buf,-1,fmt,args);
  for (const char *p = buf; *p; p++) putch(*p);

  va_end(args);
  return content;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}
/*
int sprintf(char *out, const char *fmt, ...) {
 
}
*/

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args,fmt);

  int content=vsnprintf(out,-1,fmt,args);

  va_end(args);
  return content;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char *out1=out;
  const char *format=fmt;
  int length=0;
  while(*format && length<n){
    if(*format=='%'){
      format++;
      switch(*format){
        case 's':{
          const char *c=va_arg(ap, const char *);
          while(*c){
            *out1++=*c++;
          }
          break;
          }

        case 'd':{
          int num=va_arg(ap,int);
          if(num<0){
            *out1++='-';
            num=-num;
          }
          if(num==0)*out1++='0';
          char buf[300];
          char buf_n[300];
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

        case 'c':{
          char c=(char)va_arg(ap,int);
          *out1++=c;
          break;
        }
        
        case '\n':{
          *out1++='\n';
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
  return out1-out;
}

#endif
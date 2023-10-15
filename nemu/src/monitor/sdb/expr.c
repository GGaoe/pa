/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256,TK_ADD, TK_EQ,
  TK_SUB,TK_MUL,TK_DIV,
  TK_LEFT,TK_RIGHT,TK_NOEQ,TK_AND,TK_REG,TK_HEX,TK_NUM,
  /* TODO: Add more token types */
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {


  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_ADD},         // plus
   {"==", TK_EQ},        // equal
  {"-",TK_SUB},
  {"\\*",TK_MUL},
  {"/",TK_DIV},
  {"\\(",TK_LEFT},
  {"\\)",TK_RIGHT},
  {"\\!\\=",TK_NOEQ},
  {"\\&\\&",TK_AND},
  {"\\$[a-zA-Z]*[0-9]*",TK_REG},
  {"0[xX][0-9a-fA-F]+",TK_HEX},
  {"[0-9]+",TK_NUM},
 
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

int position = 0;//changed

static bool make_token(char *e) {
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        if(rules[i].token_type==TK_NOTYPE)break;
        if(substr_len>31)assert(0);
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        tokens[nr_token].type=rules[i].token_type;
        nr_token++;
        switch (rules[i].token_type) {
          case TK_REG:
          case TK_HEX:
          {for(int i=0;i<substr_len;i++){
            tokens[nr_token-1].str[i]=*(substr_start+i);//Decimal System
          }break;}
          case TK_NUM:{for(int i=0;i<substr_len;i++){
            tokens[nr_token-1].str[i]=*(substr_start+i);//Decimal System
          }break;}
          case TK_EQ:
          strcpy(tokens[nr_token].str, "==");break;
          case TK_NOEQ:
          strcpy(tokens[nr_token].str, "!=");break;
          case TK_AND:
          strcpy(tokens[nr_token].str, "&&");break;
          default: break;
        }

        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p,int  q){
  if(tokens[p].type!=TK_LEFT||tokens[q].type!=TK_RIGHT)return false;
  int count_left=0;
  int count_right=0;
  for(int i=p;i<=q;i++){
    if(tokens[i].type==TK_LEFT)count_left++;
    if(tokens[i].type==TK_RIGHT)count_right++;
    if(count_left==count_right)return (i==q);
  }
  return false;
}

static int The_main_op(int p,int q){
  int op=-1;
  while(p<q)
  {
    if(tokens[p].type==TK_NUM){
    }
    else if(tokens[p].type==TK_LEFT){
      int count=1;
      while(count!=0){
          p++;
          if(tokens[p].type==TK_LEFT)count++;
          else if(tokens[p].type==TK_RIGHT)count--;
      }
    }
    
    else if(op==-1){op=p;}
    else if(tokens[op].type==TK_MUL||tokens[op].type==TK_DIV){op=p;}
    else if(tokens[p].type==TK_AND){op=p;}
    else if((tokens[op].type!=TK_AND)&&(tokens[p].type==TK_EQ||tokens[p].type==TK_NOEQ)){op=p;}
    else if((tokens[op].type!=TK_AND&&tokens[p].type!=TK_EQ&&tokens[p].type!=TK_NOEQ)
           &&(tokens[p].type==TK_ADD||tokens[p].type==TK_SUB)){op=p;}
    p++;
  }
  return op;
  
}

u_int32_t eval(int p,int q) {
  int op;
  u_int32_t val1,val2;
  if (p > q) {
    //printf("p:%d,q:%d\n",p,q);
    printf("BAD Expression");
    assert(p<=q);
    return -1;
  }
  else if (p == q) {
    if(tokens[p].type==TK_NUM){
      //printf("%c\n",tokens[p].str[0]);
      //printf("%d\n",atoi(tokens[p].str));
      return atoi(tokens[p].str);
    }
  }
  else if (check_parentheses(p, q) == true) {
    //printf("1:p+1:%d,q-1:%d\n",p+1,q-1);
    return eval(p + 1, q - 1);
  }
  else {
    op =The_main_op(p,q);
    //printf("2:p:%d,op-1:%d\n",p,op-1);
    val1 = eval(p, op - 1);
   // printf("3:op+1:%d,q:%d\n",op+1,q);
    val2 = eval(op + 1, q);
    switch (tokens[op].type) {
      case TK_ADD: return val1 + val2;
      case TK_SUB: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV: {if(val2==0){
                    printf("div 0 error!");
                    assert(0);
                    return val1/val2;
                    } 
                    return val1 / val2;}
      case TK_AND:return val1 && val2;
      case TK_EQ: return (val1==val2);
      case TK_NOEQ: return (val1!=val2);
      default: assert(0);
    }
  }
  return 0;
}

void int_to_char(int n,char x[]){
  int len=strlen(x);
  memset(x,'\0',len);
  char a[32]="";
  int length=0;
  while(n!=0){
    int p=n%10;
    n/=10;
    a[length]=p+'0';
    length++;
  }
  for(int i=0;i<length;i++){
    x[i]=a[length-i-1];
  }
  return;
}

int char_to_int(char n[]){
  int s_size = strlen(n);
    int tmp = 0 ;
    for(int i=0;i <s_size ; i++)
    {
	tmp += n[i] - '0';
	tmp *= 10;
    }
    tmp /= 10;
    return tmp;
}

void init_exp(){
  for(int i=0;i<nr_token;i++){
    if(tokens[i].type==TK_REG){//reg
      bool flag1 = true;
	    int tmp = isa_reg_str2val(tokens[i].str, &flag1);
	    if(flag1){
		    int_to_char(tmp, tokens[i].str);
       }
       tokens[i].type=TK_NUM;
    }

    else if(tokens[i].type==TK_HEX){//hex
      int m = strtol(tokens[i].str, NULL, 16);
      int_to_char(m,tokens[i].str);
      tokens[i].type=TK_NUM;
    }
  }

  for(int i=0;i<nr_token;i++){ // minus
    if((i>0&&tokens[i].type==TK_SUB&&tokens[i-1].type!=TK_NUM)||(i==0&&tokens[i].type==TK_SUB)){
      if(tokens[i+1].type==TK_SUB){
        for(int j=i;j<nr_token-2;j++){
            tokens[i]=tokens[i+2];
        }
        nr_token-=2;
        i--;
      }
      else if(tokens[i+1].type==TK_NUM){
        //printf("MOther fuck!\n");
        for(int j=i;j<nr_token-1;j++){
            tokens[i]=tokens[i+1];
        }
        for(int j=31;j>0;j--){
          if(tokens[i].str[j-1]!='\0')
          tokens[i].str[j]=tokens[i].str[j-1];
        }
        tokens[i].str[0]='-';
        nr_token-=1;
      }

    }
  }

  return;
} 

word_t expr(char *e, bool *success) {
  position=0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  else *success=1;

  init_exp();

  printf("%d",eval(0,nr_token-1));

  return 0;
}

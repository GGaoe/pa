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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  uint32_t val;
  char str[32];
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

void new_wp(char *args){
  if(free_==NULL){
    printf("No Wp To Use");
    assert(0);
    }
  WP* wp=free_;
  free_=free_->next;

  if(head==NULL){
    wp->next=NULL;
    head=wp;
  }
  else{
    wp->next=head;
    head=wp;
  }
  bool success=0;
  strcpy(wp->str, args);
  wp->val=expr(wp->str,&success);
  if(!success)printf("wrong val!");
  return;
}

void free_wp(int n){
  if(head==NULL){
    printf("empty wp");
    return;
  }
  WP *free_wp=NULL;
  if(head->NO==n){
    free_wp=head;
    head=head->next;
    free_wp->next=free_;
    free_=free_wp;
  }
  else{
    WP *fir=head;
    bool flag1=0;
    while(fir->next!=NULL){
      if(fir->next->NO==n){
        flag1=1;
        free_wp=fir->next;
        fir->next=fir->next->next;
        free_wp->next=free_;
        free_=free_wp;
      }
      else fir=fir->next;
    }
    if(!flag1){
      printf("No match");
    }
  }

  return;
}

void display_wp(void){
  WP *wp = head;
  if(wp==NULL){
    printf("No WP exist\n");
  }
  while(wp != NULL){
    printf("NO: %d str: %s val: %u  hex_val: 0x%x\n",wp->NO,wp->str,wp->val,wp->val);
    wp = wp->next;
  }
  return;
}


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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/paddr.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void new_wp(char *args);
void free_wp(int n);
void display_wp(void);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args){
  char *expression=strtok(NULL," ");
  uint64_t n=1;
  if(expression!=NULL){
    n=atoll(expression);
  }
  cpu_exec(n);
  return 0;
}

static int cmd_info(char *args){
  char *expression=strtok(NULL," ");
  if(expression==NULL)return -1;
  if(expression[0]=='r'){
    isa_reg_display();
  }
  if(expression[0]=='w'){
    display_wp();
  }
  return 0;
}

static int cmd_x(char *args){
  char *expression=strtok(NULL," ");
  char *expression1=strtok(NULL," ");
  long int num;
  uint64_t addr;
  sscanf(expression,"%ld",&num);
  sscanf(expression1,"%lx",&addr);
  int step=num>0 ? 4 : -4;
  num=num>0?num:-num;
  uint64_t N_T=0;
  for(;num>0;num--){
    printf("%10lx:   ",addr+N_T*step);
    printf("0x%-10x\n",paddr_read(addr+N_T*step,4));
    N_T++;
  }
  return 0;
}

static int cmd_p(char *args){
  bool flag=1;
  bool *flag1=&flag;
  char *expression=strtok(NULL,"");
  expr(expression,flag1);
  if(!flag){
    printf("%s","WRONG!");
    assert(0);
  }
  return 0;
}

static int cmd_w(char *args){
  char *expression=strtok(NULL,"");
  new_wp(expression);
  //
  return 0;
}

static int cmd_d(char *args){
  int n;
  char *expression=strtok(NULL,"");
  sscanf(expression,"%d",&n);
  free_wp(n);
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","Single step work",cmd_si},
  {"info","information",cmd_info},
  {"x","Xray",cmd_x},
  {"p","Program",cmd_p},
  {"w","set watchpoint",cmd_w},
  {"d","delete watchpoint",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}

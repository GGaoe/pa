cmd_/home/ggaoe/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o := unused

source_/home/ggaoe/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o := src/nemu-main.c

deps_/home/ggaoe/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o := \
    $(wildcard include/config/target/am.h) \
  /home/ggaoe/ics2023/nemu/include/common.h \
    $(wildcard include/config/mbase.h) \
    $(wildcard include/config/msize.h) \
    $(wildcard include/config/isa64.h) \
  /home/ggaoe/ics2023/nemu/include/macro.h \
  /home/ggaoe/ics2023/nemu/include/debug.h \
  /home/ggaoe/ics2023/nemu/include/utils.h \
    $(wildcard include/config/target/native/elf.h) \

/home/ggaoe/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o: $(deps_/home/ggaoe/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o)

$(deps_/home/ggaoe/ics2023/nemu/build/obj-riscv32-nemu-interpreter/src/nemu-main.o):
#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  
  int i;
  AM_GPU_CONFIG_T info = io_read(AM_GPU_CONFIG);
  int w = info.width, h = info.height;
  uint32_t *fb=(uint32_t *)(uintptr_t)FB_ADDR;
  for(i=0;i<w*h;i++)fb[i]=i;
  outl(SYNC_ADDR,1);
  
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t tmp=inl(VGACTL_ADDR);
  uint16_t h=(uint16_t)(tmp & 0x0000ffff);
  uint16_t w=(uint16_t)((tmp & 0xffff0000)>>16);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x=ctl->x;
  int y=ctl->y;
  int w=ctl->w;
  int h=ctl->h;
  uint32_t tmp=inl(VGACTL_ADDR);
  //uint32_t vga_h=(tmp & 0x0000ffff);
  uint32_t vga_w=((tmp & 0xffff0000)>>16);
  uint32_t *dst=(uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *p=ctl->pixels;
  for(int i=0;i<h;i++){
    for(int j=0;j<w;j++){
      dst[vga_w*(i+y)+(j+x)]=p[w*i+j];
    }
  }
  
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

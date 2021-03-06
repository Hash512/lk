#include <dev/gpio.h>
#include <kernel/timer.h>
#include <lk/console_cmd.h>
#include <lk/reg.h>
#include <platform/bcm28xx/dpi.h>
#include <platform/bcm28xx/gpio.h>
#include <platform/bcm28xx/hvs.h>
#include <platform/bcm28xx/pll_read.h>
#include <platform/bcm28xx/pv.h>
#include <stdio.h>
#include <stdlib.h>

static int cmd_dpi_start(int argc, const cmd_args *argv);
static int cmd_dpi_count(int argc, const cmd_args *argv);
static int cmd_dpi_move(int argc, const cmd_args *argv);

STATIC_COMMAND_START
STATIC_COMMAND("dpi_start", "start DPI interface", &cmd_dpi_start)
STATIC_COMMAND("dpi_count", "begin counting on framebuffer", &cmd_dpi_count)
STATIC_COMMAND("dpi_move", "move a pixel on the frame", &cmd_dpi_move)
STATIC_COMMAND_END(dpi);

uint32_t *framebuffer;
int width;
int height;
int stride;
timer_t updater;

static int cmd_dpi_start(int argc, const cmd_args *argv) {
  hvs_initialize();

  if (!framebuffer) {
    framebuffer = malloc(4*12*10);
    width = 10;
    height = 10;
    stride = 12;
  }
  for (int x=0; x< 10; x++) {
    for (int y=0; y<10; y++) {
      framebuffer[(x*stride) + y] = (0xff<<24) | (y << 16) | (y << 8) | y;
    }
  }

  int list_start = display_slot;
  hvs_add_plane(framebuffer);
  hvs_terminate_list();

  *REG32(SCALER_DISPLIST0) = list_start;
  *REG32(SCALER_DISPLIST1) = list_start;
  *REG32(SCALER_DISPLIST2) = list_start;


  // 0x200 means clock/2
  *REG32(CM_DPIDIV) = CM_PASSWORD | (0xf00 << 4);
  *REG32(CM_DPICTL) = CM_PASSWORD | CM_DPICTL_KILL_SET | CM_SRC_OSC;
  while (*REG32(CM_DPICTL) & CM_DPICTL_BUSY_SET) {};
  *REG32(CM_DPICTL) = CM_PASSWORD | CM_DPICTL_ENAB_SET | CM_SRC_OSC;
  while (*REG32(CM_DPICTL) & CM_DPICTL_BUSY_SET) {};
  printf("DPI clock set\n");
  int rate = measure_clock(17);
  printf("DPI clock measured at %d\n", rate);
  struct pv_timings t;
  t.vfp = 0;
  t.vsync = 1;
  t.vbp = 0;
  t.vactive = 10;

  t.hfp = 0;
  t.hsync = 1;
  t.hbp = 0;
  t.hactive = 10;

  setup_pixelvalve(&t, 0);

#define FORMAT(n) ((n & 0x7) << 11)

  *REG32(DPI_C) = DPI_ENABLE | FORMAT(6) | DPI_OUTPUT_ENABLE_MODE;
  //gpio_config(0, kBCM2708Pinmux_ALT2); // pixel-clock
  gpio_config(2, kBCM2708Pinmux_ALT2); // vsync
  gpio_config(3, kBCM2708Pinmux_ALT2); // hsync
  gpio_config(4, kBCM2708Pinmux_ALT2); // D0
  gpio_config(5, kBCM2708Pinmux_ALT2); // D1
  return 0;
}

uint8_t count;

static enum handler_return updater_entry(struct timer *t, lk_time_t now, void *arg) {
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      framebuffer[(y * stride) + x] = 0xff000000 | (count << 16);
    }
  }
  count++;
  return INT_NO_RESCHEDULE;
}

static int cmd_dpi_count(int argc, const cmd_args *argv) {
  timer_initialize(&updater);
  timer_set_periodic(&updater, 1000, updater_entry, NULL);
  return 0;
}

static enum handler_return mover_entry(struct timer *t, lk_time_t now, void *arg) {
  int y = count / width;
  int x = count % width;
  framebuffer[(y * stride) + x] = 0xff000000;
  count = (count+1) % (width*height);
  y = count / width;
  x = count % width;
  framebuffer[(y * stride) + x] = 0xffffffff;
  return INT_NO_RESCHEDULE;
}

static int cmd_dpi_move(int argc, const cmd_args *argv) {
  timer_initialize(&updater);
  timer_set_periodic(&updater, 100, mover_entry, NULL);
  return 0;
}

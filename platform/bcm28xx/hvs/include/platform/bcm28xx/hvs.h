#pragma once

#include <platform/bcm28xx.h>

#define SCALER_BASE (BCM_PERIPH_BASE_VIRT + 0x400000)

#define SCALER_DISPCTRL     (SCALER_BASE + 0x00)
#define SCALER_DISPCTRL_ENABLE  (1<<31)
#define SCALER_DISPEOLN     (SCALER_BASE + 0x18)
#define SCALER_DISPLIST0    (SCALER_BASE + 0x20)
#define SCALER_DISPLIST1    (SCALER_BASE + 0x24)
#define SCALER_DISPLIST2    (SCALER_BASE + 0x28)

struct hvs_channel {
  volatile uint32_t dispctrl;
  volatile uint32_t dispbkgnd;
  volatile uint32_t dispstat;
  // 31:30  mode
  // 29     full
  // 28     empty
  // 17:12  frame count
  // 11:0   line
  volatile uint32_t dispbase;
};

extern volatile struct hvs_channel *hvs_channels;

#define SCALER_DISPCTRL0    (SCALER_BASE + 0x40)
#define SCALER_DISPCTRLX_ENABLE (1<<31)
#define SCALER_DISPCTRLX_RESET  (1<<30)
#define SCALER_DISPCTRL_W(n)    ((n & 0xfff) << 12)
#define SCALER_DISPCTRL_H(n)    (n & 0xfff)
#define SCALER_DISPBKGND_AUTOHS (1<<31)
#define SCALER_DISPBKGND_GAMMA  (1<<29)

#define BASE_BASE(n) (n & 0xffff)
#define BASE_TOP(n) ((n & 0xffff) << 16)


#define SCALER_LIST_MEMORY  (BCM_PERIPH_BASE_VIRT + 0x402000)


#define CONTROL_FORMAT(n)       (n & 0xf)
#define CONTROL_END             (1<<31)
#define CONTROL_VALID           (1<<30)
#define CONTROL_WORDS(n)        ((n & 0x3f) << 24)
#define CONTROL_PIXEL_ORDER(n)  ((n & 3) << 13)
#define CONTROL_UNITY           (1<<4)

#define HVS_PIXEL_ORDER_ARGB                    2

enum hvs_pixel_format {
	/* 8bpp */
	HVS_PIXEL_FORMAT_RGB332 = 0,
	/* 16bpp */
	HVS_PIXEL_FORMAT_RGBA4444 = 1,
	HVS_PIXEL_FORMAT_RGB555 = 2,
	HVS_PIXEL_FORMAT_RGBA5551 = 3,
	HVS_PIXEL_FORMAT_RGB565 = 4,
	/* 24bpp */
	HVS_PIXEL_FORMAT_RGB888 = 5,
	HVS_PIXEL_FORMAT_RGBA6666 = 6,
	/* 32bpp */
	HVS_PIXEL_FORMAT_RGBA8888 = 7,

	HVS_PIXEL_FORMAT_YCBCR_YUV420_3PLANE = 8,
	HVS_PIXEL_FORMAT_YCBCR_YUV420_2PLANE = 9,
	HVS_PIXEL_FORMAT_YCBCR_YUV422_3PLANE = 10,
	HVS_PIXEL_FORMAT_YCBCR_YUV422_2PLANE = 11,
	HVS_PIXEL_FORMAT_H264 = 12,
	HVS_PIXEL_FORMAT_PALETTE = 13,
	HVS_PIXEL_FORMAT_YUV444_RGB = 14,
	HVS_PIXEL_FORMAT_AYUV444_RGB = 15,
	HVS_PIXEL_FORMAT_RGBA1010102 = 16,
	HVS_PIXEL_FORMAT_YCBCR_10BIT = 17,
};

#define CONTROL0_X(n) (n & 0xfff)
#define CONTROL0_Y(n) ((n & 0xfff) << 12)

#define CONTROL2_W(n) (n & 0xffff)
#define CONTROL2_H(n) ((n & 0xffff) << 16)

extern int display_slot;
extern volatile uint32_t* dlist_memory;

void hvs_add_plane(void *framebuffer);
void hvs_terminate_list(void);
void hvs_wipe_displaylist(void);
void hvs_initialize(void);
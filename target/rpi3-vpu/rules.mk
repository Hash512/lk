LOCAL_DIR := $(GET_LOCAL_DIR)

GLOBAL_INCLUDES += \
	$(LOCAL_DIR)/include

GLOBAL_DEFINES += TARGET_HAS_DEBUG_LED=1 VPU=1 MODEL=3

PLATFORM := bcm28xx

#include make/module.mk


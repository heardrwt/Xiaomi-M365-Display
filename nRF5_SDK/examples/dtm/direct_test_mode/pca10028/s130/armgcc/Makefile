PROJECT_NAME     := direct_test_mode_pca10028_s130
TARGETS          := nrf51422_xxac
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../..
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/nrf51422_xxac.out: \
  LINKER_SCRIPT  := direct_test_mode_gcc_nrf51.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_ROOT)/components/toolchain/system_nrf51.c \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/ble/ble_dtm/ble_dtm.c \
  $(SDK_ROOT)/components/ble/ble_dtm/ble_dtm_hw_nrf51.c \
  $(SDK_ROOT)/components/boards/boards.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/softdevice/s130/headers/nrf51 \
  $(PROJ_DIR) \
  $(SDK_ROOT)/components/libraries/bsp \
  ../config \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/softdevice/s130/headers \
  $(SDK_ROOT)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -DBOARD_PCA10028
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF51
CFLAGS += -DS130
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -DNRF51422
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -O3 -g3
CFLAGS += -mfloat-abi=soft
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DBOARD_PCA10028
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF51
ASMFLAGS += -DS130
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DBSP_DEFINES_ONLY
ASMFLAGS += -DNRF51422
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash flash_softdevice

# Default target - first one defined
default: nrf51422_xxac

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/nrf51422_xxac.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

# Flash softdevice
flash_softdevice:
	@echo Flashing: s130_nrf51_2.0.1_softdevice.hex
	nrfjprog --program $(SDK_ROOT)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex -f nrf51 --sectorerase 
	nrfjprog --reset -f nrf51

erase:
	nrfjprog --eraseall -f nrf52

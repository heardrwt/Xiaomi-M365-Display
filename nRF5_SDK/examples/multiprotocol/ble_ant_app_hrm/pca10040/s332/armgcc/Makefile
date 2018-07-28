PROJECT_NAME     := ble_ant_app_hrm_pca10040_s332
TARGETS          := nrf52832_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../..
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/nrf52832_xxaa.out: \
  LINKER_SCRIPT  := ble_ant_app_hrm_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/uart/app_uart_fifo.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/fstorage/fstorage.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/libraries/sensorsim/sensorsim.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/ant/ant_channel_config/ant_channel_config.c \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/ant_hrm.c \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/pages/ant_hrm_page_0.c \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/pages/ant_hrm_page_1.c \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/pages/ant_hrm_page_2.c \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/pages/ant_hrm_page_3.c \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/pages/ant_hrm_page_4.c \
  $(SDK_ROOT)/components/ant/ant_key_manager/ant_key_manager.c \
  $(SDK_ROOT)/components/ant/ant_stack_config/ant_stack_config.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp_nfc.c \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_mutex.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas/ble_bas.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs/ble_hrs.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/softdevice/s332/headers/nrf52 \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/fifo \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/libraries/sensorsim \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/utils \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/ant/ant_channel_config \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/ant/ant_stack_config \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/external/segger_rtt \
  ../config \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/ant/ant_key_manager \
  $(SDK_ROOT)/components/ant/ant_key_manager/config \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/ant/ant_profiles/ant_hrm/pages \
  $(SDK_ROOT)/components/softdevice/s332/headers \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -DNRF52_PAN_55
CFLAGS += -DNRF52_PAN_12
CFLAGS += -DNRF52_PAN_15
CFLAGS += -DNRF52_PAN_58
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DNRF52_PAN_54
CFLAGS += -DNRF52
CFLAGS += -DNRF52_PAN_51
CFLAGS += -DNRF52_PAN_36
CFLAGS += -DANT_STACK_SUPPORT_REQD
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DS332
CFLAGS += -DNRF52832
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DNRF52_PAN_31
CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror -O3 -g3
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DNRF52_PAN_55
ASMFLAGS += -DNRF52_PAN_12
ASMFLAGS += -DNRF52_PAN_15
ASMFLAGS += -DNRF52_PAN_58
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DNRF52_PAN_54
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52_PAN_51
ASMFLAGS += -DNRF52_PAN_36
ASMFLAGS += -DANT_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DS332
ASMFLAGS += -DNRF52832
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52_PAN_31
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash 

# Default target - first one defined
default: nrf52832_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	nrf52832_xxaa

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex
	@echo Flashing: $<
	nrfjprog --program $< -f nrf52 --sectorerase
	nrfjprog --reset -f nrf52

erase:
	nrfjprog --eraseall -f nrf52

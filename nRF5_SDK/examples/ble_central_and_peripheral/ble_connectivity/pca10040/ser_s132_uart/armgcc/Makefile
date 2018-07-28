PROJECT_NAME     := ble_connectivity_s132_uart_pca10040
TARGETS          := nrf52832_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../..
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/nrf52832_xxaa.out: \
  LINKER_SCRIPT  := ble_connectivity_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/uart/app_uart.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/common/ble_dtm_init.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_event_enc.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_evt_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_gap_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_gap_evt_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gap_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gatt_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_gattc_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_gattc_evt_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gattc_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_gatts_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_gatts_evt_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gatts_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_l2cap_conn.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/ble_l2cap_evt_conn.c \
  $(SDK_ROOT)/components/serialization/common/ble_serialization.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/common/cond_field_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/conn_ble_gap_sec_keys.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/conn_ble_user_mem.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/common/conn_mw.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/middleware/conn_mw_ble.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/middleware/conn_mw_ble_gap.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/middleware/conn_mw_ble_gattc.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/middleware/conn_mw_ble_gatts.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/middleware/conn_mw_ble_l2cap.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/middleware/conn_mw_nrf_soc.c \
  $(SDK_ROOT)/components/serialization/connectivity/hal/dtm_uart.c \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers/nrf_soc_conn.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/nrf_soc_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_cmd_decoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_dtm_cmd_decoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_error_handling.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_event_encoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_handlers.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_pkt_decoder.c \
  $(SDK_ROOT)/components/serialization/connectivity/ser_conn_reset_cmd_decoder.c \
  $(SDK_ROOT)/components/serialization/common/ser_dbg_sd_str.c \
  $(SDK_ROOT)/components/serialization/common/transport/ser_hal_transport.c \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy/ser_phy_uart.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/ble_dtm/ble_dtm.c \
  $(SDK_ROOT)/components/ble/ble_dtm/ble_dtm_hw_nrf52.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/serialization/connectivity/hal \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy/config \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/middleware \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/common \
  $(SDK_ROOT)/components/serialization/connectivity \
  $(SDK_ROOT)/components/serialization/connectivity/codecs/s132/serializers \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/serialization/common \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/scheduler \
  ../config \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/serialization/common/transport \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132 \
  $(SDK_ROOT)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -D__STACK_SIZE=2048
CFLAGS += -DNRF52
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSER_CONNECTIVITY
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DNRF52832
CFLAGS += -D__HEAP_SIZE=1024
CFLAGS += -DNRF52_PAN_12
CFLAGS += -DNRF52_PAN_58
CFLAGS += -DNRF52_PAN_55
CFLAGS += -DNRF52_PAN_54
CFLAGS += -DNRF52_PAN_31
CFLAGS += -DNRF52_PAN_51
CFLAGS += -DNRF52_PAN_36
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF52_PAN_15
CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DS132
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
ASMFLAGS += -D__STACK_SIZE=2048
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSER_CONNECTIVITY
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52832
ASMFLAGS += -D__HEAP_SIZE=1024
ASMFLAGS += -DNRF52_PAN_12
ASMFLAGS += -DNRF52_PAN_58
ASMFLAGS += -DNRF52_PAN_55
ASMFLAGS += -DNRF52_PAN_54
ASMFLAGS += -DNRF52_PAN_31
ASMFLAGS += -DNRF52_PAN_51
ASMFLAGS += -DNRF52_PAN_36
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF52_PAN_15
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3
ASMFLAGS += -DBSP_DEFINES_ONLY
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DS132

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash flash_softdevice

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

# Flash softdevice
flash_softdevice:
	@echo Flashing: s132_nrf52_3.0.0_softdevice.hex
	nrfjprog --program $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_3.0.0_softdevice.hex -f nrf52 --sectorerase 
	nrfjprog --reset -f nrf52

erase:
	nrfjprog --eraseall -f nrf52

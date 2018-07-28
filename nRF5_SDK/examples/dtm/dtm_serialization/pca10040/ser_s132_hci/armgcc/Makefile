PROJECT_NAME     := ble_dtm_pca10040_s132_hci
TARGETS          := nrf52832_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../..
PROJ_DIR := ../../..

$(OUTPUT_DIRECTORY)/nrf52832_xxaa.out: \
  LINKER_SCRIPT  := ble_dtm_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/app_ble_gap_sec_keys.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/app_ble_user_mem.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/middleware/app_mw_ble.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/middleware/app_mw_ble_gap.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/middleware/app_mw_ble_gattc.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/middleware/app_mw_ble_gatts.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/middleware/app_mw_ble_l2cap.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/middleware/app_mw_nrf_soc.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_app.c \
  $(SDK_ROOT)/components/serialization/application/codecs/common/ble_dtm_app.c \
  $(SDK_ROOT)/components/serialization/application/codecs/common/ble_dtm_init.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_event.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_evt_app.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_gap_app.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_gap_evt_app.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gap_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gatt_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_gattc_app.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_gattc_evt_app.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gattc_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_gatts_app.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_gatts_evt_app.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_gatts_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_l2cap_app.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/ble_l2cap_evt_app.c \
  $(SDK_ROOT)/components/serialization/common/ble_serialization.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/ble_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/common/cond_field_serialization.c \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers/nrf_soc_app.c \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132/nrf_soc_struct_serialization.c \
  $(SDK_ROOT)/components/serialization/application/hal/ser_app_hal_nrf5x.c \
  $(SDK_ROOT)/components/serialization/application/hal/ser_app_power_system_off.c \
  $(SDK_ROOT)/components/serialization/common/ser_dbg_sd_str.c \
  $(SDK_ROOT)/components/serialization/common/transport/ser_hal_transport.c \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy/ser_phy_hci.c \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy/ser_phy_hci_slip.c \
  $(SDK_ROOT)/components/serialization/application/transport/ser_sd_transport.c \
  $(SDK_ROOT)/components/serialization/application/transport/ser_softdevice_handler.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/uart/app_uart.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/drivers_nrf/hal/nrf_nvmc.c \
  $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp_nfc.c \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components \
  ../config \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/serialization/common \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/serialization/application/codecs/s132/serializers \
  $(SDK_ROOT)/components/serialization/common/struct_ser/s132 \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/serialization/application/codecs/common \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/serialization/common/transport \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/serialization/common/transport/ser_phy/config \
  $(SDK_ROOT)/components/serialization/application/hal \
  $(SDK_ROOT)/components/serialization/application/transport \
  $(SDK_ROOT)/components/libraries/log/src \

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -DNRF52_PAN_12
CFLAGS += -DNRF52_PAN_15
CFLAGS += -DNRF52_PAN_58
CFLAGS += -DSWI_DISABLE0
CFLAGS += -DNRF52_PAN_20
CFLAGS += -DNRF52_PAN_54
CFLAGS += -DNRF52_PAN_31
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DNRF52_PAN_51
CFLAGS += -DNRF52_PAN_36
CFLAGS += -DNRF52832
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DNRF52_PAN_64
CFLAGS += -DNRF52_PAN_55
CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -DS132
CFLAGS += -DHCI_TIMER2
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DNRF52
CFLAGS += -DSVCALL_AS_NORMAL_FUNCTION
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
ASMFLAGS += -DNRF52_PAN_12
ASMFLAGS += -DNRF52_PAN_15
ASMFLAGS += -DNRF52_PAN_58
ASMFLAGS += -DSWI_DISABLE0
ASMFLAGS += -DNRF52_PAN_20
ASMFLAGS += -DNRF52_PAN_54
ASMFLAGS += -DNRF52_PAN_31
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DNRF52_PAN_51
ASMFLAGS += -DNRF52_PAN_36
ASMFLAGS += -DNRF52832
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DNRF52_PAN_64
ASMFLAGS += -DNRF52_PAN_55
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3
ASMFLAGS += -DS132
ASMFLAGS += -DHCI_TIMER2
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52
ASMFLAGS += -DSVCALL_AS_NORMAL_FUNCTION

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

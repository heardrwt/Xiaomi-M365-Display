

#ifndef SDK_CONFIG_H
#define SDK_CONFIG_H
// <<< Use Configuration Wizard in Context Menu >>>\n
#ifdef USE_APP_CONFIG
#include "app_config.h"
#endif
// <h> Application 

//==========================================================
// <o> CHAN_ID_DEV_NUM - Channel ID: Device Number. 
#ifndef CHAN_ID_DEV_NUM
#define CHAN_ID_DEV_NUM 1
#endif

// <o> CHAN_ID_TRANS_TYPE - Channel ID: Transmission type. 
#ifndef CHAN_ID_TRANS_TYPE
#define CHAN_ID_TRANS_TYPE 5
#endif

// <h> MANUFACTURER_IDENTIFICATION_COMMON_PAGE 

//==========================================================
// <o> SDM_HW_REVISION - Hardware revision for manufacturer's identification common page 
#ifndef SDM_HW_REVISION
#define SDM_HW_REVISION 127
#endif

// <o> SDM_MANUFACTURER_ID - Manufacturer ID for manufacturer's identification common page 
#ifndef SDM_MANUFACTURER_ID
#define SDM_MANUFACTURER_ID 43690
#endif

// <o> SDM_MODEL_NUMBER - Model number for manufacturer's identification common page 
#ifndef SDM_MODEL_NUMBER
#define SDM_MODEL_NUMBER 21845
#endif

// </h> 
//==========================================================

// <o> MODIFICATION_TYPE  - Type of cadence values update
 

// <i> It can be updated by buttons or periodically rise and fall in auto mode
// <0=> MODIFICATION_TYPE_BUTTON 
// <1=> MODIFICATION_TYPE_AUTO 

#ifndef MODIFICATION_TYPE
#define MODIFICATION_TYPE 0
#endif

// <h> PRODUCT_INFORMATION_COMMON_PAGE 

//==========================================================
// <o> SDM_SW_REVISION_MAJOR - Software revision major number for product information common page 
#ifndef SDM_SW_REVISION_MAJOR
#define SDM_SW_REVISION_MAJOR 170
#endif

// <o> SDM_SW_REVISION_MINOR - Software revision minor number for product information common page, unused value 
#ifndef SDM_SW_REVISION_MINOR
#define SDM_SW_REVISION_MINOR 255
#endif

// <o> SDM_SERIAL_NUMBER - Serial number for product information common page 
#ifndef SDM_SERIAL_NUMBER
#define SDM_SERIAL_NUMBER 1234567
#endif

// </h> 
//==========================================================

// <h> SIMULATOR 

//==========================================================
// <o> SIMULATOR_STRIDE_LEN - Length of one stride 
#ifndef SIMULATOR_STRIDE_LEN
#define SIMULATOR_STRIDE_LEN 75
#endif

// <o> SIMULATOR_BURN_RATE - Calories burn rate 
#ifndef SIMULATOR_BURN_RATE
#define SIMULATOR_BURN_RATE 62
#endif

// <o> SIMULATOR_MIN - Minimal cadence 
#ifndef SIMULATOR_MIN
#define SIMULATOR_MIN 60
#endif

// <o> SIMULATOR_MAX - Maximal cadence 
#ifndef SIMULATOR_MAX
#define SIMULATOR_MAX 120
#endif

// <o> SIMULATOR_INCR - Cadence increment 
#ifndef SIMULATOR_INCR
#define SIMULATOR_INCR 1
#endif

// </h> 
//==========================================================

// </h> 
//==========================================================

// <h> nRF_ANT 

//==========================================================
// <q> ANT_CHANNEL_CONFIG_ENABLED  - ant_channel_config - ANT common channel configuration
 

#ifndef ANT_CHANNEL_CONFIG_ENABLED
#define ANT_CHANNEL_CONFIG_ENABLED 1
#endif

// <e> ANT_COMMON_PAGE_70_ENABLED - ant_common_page_70 - ANT+ common page 70
//==========================================================
#ifndef ANT_COMMON_PAGE_70_ENABLED
#define ANT_COMMON_PAGE_70_ENABLED 1
#endif
#if  ANT_COMMON_PAGE_70_ENABLED
// <e> ANT_COMMON_PAGE_70_LOG_ENABLED - Enables logging of common page 70 in the module.
//==========================================================
#ifndef ANT_COMMON_PAGE_70_LOG_ENABLED
#define ANT_COMMON_PAGE_70_LOG_ENABLED 1
#endif
#if  ANT_COMMON_PAGE_70_LOG_ENABLED
// <o> ANT_COMMON_PAGE_70_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_COMMON_PAGE_70_LOG_LEVEL
#define ANT_COMMON_PAGE_70_LOG_LEVEL 3
#endif

// <o> ANT_COMMON_PAGE_70_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_COMMON_PAGE_70_INFO_COLOR
#define ANT_COMMON_PAGE_70_INFO_COLOR 0
#endif

#endif //ANT_COMMON_PAGE_70_LOG_ENABLED
// </e>

#endif //ANT_COMMON_PAGE_70_ENABLED
// </e>

// <e> ANT_COMMON_PAGE_80_ENABLED - ant_common_page_80 - ANT+ common page 80
//==========================================================
#ifndef ANT_COMMON_PAGE_80_ENABLED
#define ANT_COMMON_PAGE_80_ENABLED 1
#endif
#if  ANT_COMMON_PAGE_80_ENABLED
// <e> ANT_COMMON_PAGE_80_LOG_ENABLED - Enables logging of common page 80 in the module.
//==========================================================
#ifndef ANT_COMMON_PAGE_80_LOG_ENABLED
#define ANT_COMMON_PAGE_80_LOG_ENABLED 1
#endif
#if  ANT_COMMON_PAGE_80_LOG_ENABLED
// <o> ANT_COMMON_PAGE_80_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_COMMON_PAGE_80_LOG_LEVEL
#define ANT_COMMON_PAGE_80_LOG_LEVEL 3
#endif

// <o> ANT_COMMON_PAGE_80_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_COMMON_PAGE_80_INFO_COLOR
#define ANT_COMMON_PAGE_80_INFO_COLOR 0
#endif

#endif //ANT_COMMON_PAGE_80_LOG_ENABLED
// </e>

#endif //ANT_COMMON_PAGE_80_ENABLED
// </e>

// <e> ANT_COMMON_PAGE_81_ENABLED - ant_common_page_81 - ANT+ common page 81
//==========================================================
#ifndef ANT_COMMON_PAGE_81_ENABLED
#define ANT_COMMON_PAGE_81_ENABLED 1
#endif
#if  ANT_COMMON_PAGE_81_ENABLED
// <e> ANT_COMMON_PAGE_81_LOG_ENABLED - Enables logging of common page 81 in the module.
//==========================================================
#ifndef ANT_COMMON_PAGE_81_LOG_ENABLED
#define ANT_COMMON_PAGE_81_LOG_ENABLED 1
#endif
#if  ANT_COMMON_PAGE_81_LOG_ENABLED
// <o> ANT_COMMON_PAGE_81_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_COMMON_PAGE_81_LOG_LEVEL
#define ANT_COMMON_PAGE_81_LOG_LEVEL 3
#endif

// <o> ANT_COMMON_PAGE_81_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_COMMON_PAGE_81_INFO_COLOR
#define ANT_COMMON_PAGE_81_INFO_COLOR 0
#endif

#endif //ANT_COMMON_PAGE_81_LOG_ENABLED
// </e>

#endif //ANT_COMMON_PAGE_81_ENABLED
// </e>

// <q> ANT_KEY_MANAGER_ENABLED  - ant_key_manager - Software Component
 

#ifndef ANT_KEY_MANAGER_ENABLED
#define ANT_KEY_MANAGER_ENABLED 1
#endif

// <q> ANT_REQUEST_CONTROLLER_ENABLED  - ant_request_controller - ANT+ request controller
 

#ifndef ANT_REQUEST_CONTROLLER_ENABLED
#define ANT_REQUEST_CONTROLLER_ENABLED 1
#endif

// <e> ANT_SDM_ENABLED - ant_sdm - Stride Based Speed and Distance Monitor Profile
//==========================================================
#ifndef ANT_SDM_ENABLED
#define ANT_SDM_ENABLED 1
#endif
#if  ANT_SDM_ENABLED
// <e> ANT_SDM_LOG_ENABLED - Enables general logging in the module.
//==========================================================
#ifndef ANT_SDM_LOG_ENABLED
#define ANT_SDM_LOG_ENABLED 1
#endif
#if  ANT_SDM_LOG_ENABLED
// <o> ANT_SDM_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_SDM_LOG_LEVEL
#define ANT_SDM_LOG_LEVEL 3
#endif

// <o> ANT_SDM_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_SDM_INFO_COLOR
#define ANT_SDM_INFO_COLOR 0
#endif

#endif //ANT_SDM_LOG_ENABLED
// </e>

// <e> ANT_SDM_PAGE_1_LOG_ENABLED - Enables logging of SDM page 1 in the module.
//==========================================================
#ifndef ANT_SDM_PAGE_1_LOG_ENABLED
#define ANT_SDM_PAGE_1_LOG_ENABLED 1
#endif
#if  ANT_SDM_PAGE_1_LOG_ENABLED
// <o> ANT_SDM_PAGE_1_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_SDM_PAGE_1_LOG_LEVEL
#define ANT_SDM_PAGE_1_LOG_LEVEL 3
#endif

// <o> ANT_SDM_PAGE_1_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_SDM_PAGE_1_INFO_COLOR
#define ANT_SDM_PAGE_1_INFO_COLOR 0
#endif

#endif //ANT_SDM_PAGE_1_LOG_ENABLED
// </e>

// <e> ANT_SDM_PAGE_2_LOG_ENABLED - Enables logging of SDM page 2 in the module.
//==========================================================
#ifndef ANT_SDM_PAGE_2_LOG_ENABLED
#define ANT_SDM_PAGE_2_LOG_ENABLED 1
#endif
#if  ANT_SDM_PAGE_2_LOG_ENABLED
// <o> ANT_SDM_PAGE_2_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_SDM_PAGE_2_LOG_LEVEL
#define ANT_SDM_PAGE_2_LOG_LEVEL 3
#endif

// <o> ANT_SDM_PAGE_2_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_SDM_PAGE_2_INFO_COLOR
#define ANT_SDM_PAGE_2_INFO_COLOR 0
#endif

#endif //ANT_SDM_PAGE_2_LOG_ENABLED
// </e>

// <e> ANT_SDM_PAGE_3_LOG_ENABLED - Enables logging of SDM page 3 in the module.
//==========================================================
#ifndef ANT_SDM_PAGE_3_LOG_ENABLED
#define ANT_SDM_PAGE_3_LOG_ENABLED 1
#endif
#if  ANT_SDM_PAGE_3_LOG_ENABLED
// <o> ANT_SDM_PAGE_3_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_SDM_PAGE_3_LOG_LEVEL
#define ANT_SDM_PAGE_3_LOG_LEVEL 3
#endif

// <o> ANT_SDM_PAGE_3_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_SDM_PAGE_3_INFO_COLOR
#define ANT_SDM_PAGE_3_INFO_COLOR 0
#endif

#endif //ANT_SDM_PAGE_3_LOG_ENABLED
// </e>

// <e> ANT_SDM_PAGE_16_LOG_ENABLED - Enables logging of SDM page 16 in the module.
//==========================================================
#ifndef ANT_SDM_PAGE_16_LOG_ENABLED
#define ANT_SDM_PAGE_16_LOG_ENABLED 1
#endif
#if  ANT_SDM_PAGE_16_LOG_ENABLED
// <o> ANT_SDM_PAGE_16_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_SDM_PAGE_16_LOG_LEVEL
#define ANT_SDM_PAGE_16_LOG_LEVEL 3
#endif

// <o> ANT_SDM_PAGE_16_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_SDM_PAGE_16_INFO_COLOR
#define ANT_SDM_PAGE_16_INFO_COLOR 0
#endif

#endif //ANT_SDM_PAGE_16_LOG_ENABLED
// </e>

// <e> ANT_SDM_PAGE_22_LOG_ENABLED - Enables logging of SDM page 22 in the module.
//==========================================================
#ifndef ANT_SDM_PAGE_22_LOG_ENABLED
#define ANT_SDM_PAGE_22_LOG_ENABLED 1
#endif
#if  ANT_SDM_PAGE_22_LOG_ENABLED
// <o> ANT_SDM_PAGE_22_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ANT_SDM_PAGE_22_LOG_LEVEL
#define ANT_SDM_PAGE_22_LOG_LEVEL 3
#endif

// <o> ANT_SDM_PAGE_22_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ANT_SDM_PAGE_22_INFO_COLOR
#define ANT_SDM_PAGE_22_INFO_COLOR 0
#endif

#endif //ANT_SDM_PAGE_22_LOG_ENABLED
// </e>

#endif //ANT_SDM_ENABLED
// </e>

// <e> ANT_STACK_CONFIG_ENABLED - ant_stack_config - Common ANT stack configuration
//==========================================================
#ifndef ANT_STACK_CONFIG_ENABLED
#define ANT_STACK_CONFIG_ENABLED 1
#endif
#if  ANT_STACK_CONFIG_ENABLED
// <o> ANT_CONFIG_TOTAL_CHANNELS_ALLOCATED - Allocated ANT channels 
#ifndef ANT_CONFIG_TOTAL_CHANNELS_ALLOCATED
#define ANT_CONFIG_TOTAL_CHANNELS_ALLOCATED 1
#endif

// <o> ANT_CONFIG_ENCRYPTED_CHANNELS - Encrypted ANT channels 
#ifndef ANT_CONFIG_ENCRYPTED_CHANNELS
#define ANT_CONFIG_ENCRYPTED_CHANNELS 0
#endif

// <o> ANT_CONFIG_EVENT_QUEUE_SIZE - Event queue size 
#ifndef ANT_CONFIG_EVENT_QUEUE_SIZE
#define ANT_CONFIG_EVENT_QUEUE_SIZE 32
#endif

// <o> ANT_CONFIG_BURST_QUEUE_SIZE - ANT burst queue size 
#ifndef ANT_CONFIG_BURST_QUEUE_SIZE
#define ANT_CONFIG_BURST_QUEUE_SIZE 128
#endif

#endif //ANT_STACK_CONFIG_ENABLED
// </e>

// <q> ANT_STATE_INDICATOR_ENABLED  - ant_state_indicator - ANT state indicator using BSP
 

#ifndef ANT_STATE_INDICATOR_ENABLED
#define ANT_STATE_INDICATOR_ENABLED 1
#endif

// </h> 
//==========================================================

// <h> nRF_Drivers 

//==========================================================
// <e> CLOCK_ENABLED - nrf_drv_clock - CLOCK peripheral driver
//==========================================================
#ifndef CLOCK_ENABLED
#define CLOCK_ENABLED 1
#endif
#if  CLOCK_ENABLED
// <o> CLOCK_CONFIG_XTAL_FREQ  - HF XTAL Frequency
 
// <0=> Default (64 MHz) 

#ifndef CLOCK_CONFIG_XTAL_FREQ
#define CLOCK_CONFIG_XTAL_FREQ 0
#endif

// <o> CLOCK_CONFIG_LF_SRC  - LF Clock Source
 
// <0=> RC 
// <1=> XTAL 
// <2=> Synth 

#ifndef CLOCK_CONFIG_LF_SRC
#define CLOCK_CONFIG_LF_SRC 1
#endif

// <o> CLOCK_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef CLOCK_CONFIG_IRQ_PRIORITY
#define CLOCK_CONFIG_IRQ_PRIORITY 7
#endif

// <e> CLOCK_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef CLOCK_CONFIG_LOG_ENABLED
#define CLOCK_CONFIG_LOG_ENABLED 0
#endif
#if  CLOCK_CONFIG_LOG_ENABLED
// <o> CLOCK_CONFIG_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef CLOCK_CONFIG_LOG_LEVEL
#define CLOCK_CONFIG_LOG_LEVEL 3
#endif

// <o> CLOCK_CONFIG_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef CLOCK_CONFIG_INFO_COLOR
#define CLOCK_CONFIG_INFO_COLOR 0
#endif

// <o> CLOCK_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef CLOCK_CONFIG_DEBUG_COLOR
#define CLOCK_CONFIG_DEBUG_COLOR 0
#endif

#endif //CLOCK_CONFIG_LOG_ENABLED
// </e>

#endif //CLOCK_ENABLED
// </e>

// <e> GPIOTE_ENABLED - nrf_drv_gpiote - GPIOTE peripheral driver
//==========================================================
#ifndef GPIOTE_ENABLED
#define GPIOTE_ENABLED 1
#endif
#if  GPIOTE_ENABLED
// <o> GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS - Number of lower power input pins 
#ifndef GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS
#define GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 4
#endif

// <o> GPIOTE_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef GPIOTE_CONFIG_IRQ_PRIORITY
#define GPIOTE_CONFIG_IRQ_PRIORITY 7
#endif

// <e> GPIOTE_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef GPIOTE_CONFIG_LOG_ENABLED
#define GPIOTE_CONFIG_LOG_ENABLED 0
#endif
#if  GPIOTE_CONFIG_LOG_ENABLED
// <o> GPIOTE_CONFIG_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef GPIOTE_CONFIG_LOG_LEVEL
#define GPIOTE_CONFIG_LOG_LEVEL 3
#endif

// <o> GPIOTE_CONFIG_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef GPIOTE_CONFIG_INFO_COLOR
#define GPIOTE_CONFIG_INFO_COLOR 0
#endif

// <o> GPIOTE_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef GPIOTE_CONFIG_DEBUG_COLOR
#define GPIOTE_CONFIG_DEBUG_COLOR 0
#endif

#endif //GPIOTE_CONFIG_LOG_ENABLED
// </e>

#endif //GPIOTE_ENABLED
// </e>

// <e> PERIPHERAL_RESOURCE_SHARING_ENABLED - nrf_drv_common - Peripheral drivers common module
//==========================================================
#ifndef PERIPHERAL_RESOURCE_SHARING_ENABLED
#define PERIPHERAL_RESOURCE_SHARING_ENABLED 0
#endif
#if  PERIPHERAL_RESOURCE_SHARING_ENABLED
// <e> COMMON_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef COMMON_CONFIG_LOG_ENABLED
#define COMMON_CONFIG_LOG_ENABLED 0
#endif
#if  COMMON_CONFIG_LOG_ENABLED
// <o> COMMON_CONFIG_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef COMMON_CONFIG_LOG_LEVEL
#define COMMON_CONFIG_LOG_LEVEL 3
#endif

// <o> COMMON_CONFIG_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef COMMON_CONFIG_INFO_COLOR
#define COMMON_CONFIG_INFO_COLOR 0
#endif

// <o> COMMON_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef COMMON_CONFIG_DEBUG_COLOR
#define COMMON_CONFIG_DEBUG_COLOR 0
#endif

#endif //COMMON_CONFIG_LOG_ENABLED
// </e>

#endif //PERIPHERAL_RESOURCE_SHARING_ENABLED
// </e>

// <e> UART_ENABLED - nrf_drv_uart - UART/UARTE peripheral driver
//==========================================================
#ifndef UART_ENABLED
#define UART_ENABLED 1
#endif
#if  UART_ENABLED
// <o> UART_DEFAULT_CONFIG_HWFC  - Hardware Flow Control
 
// <0=> Disabled 
// <1=> Enabled 

#ifndef UART_DEFAULT_CONFIG_HWFC
#define UART_DEFAULT_CONFIG_HWFC 0
#endif

// <o> UART_DEFAULT_CONFIG_PARITY  - Parity
 
// <0=> Excluded 
// <14=> Included 

#ifndef UART_DEFAULT_CONFIG_PARITY
#define UART_DEFAULT_CONFIG_PARITY 0
#endif

// <o> UART_DEFAULT_CONFIG_BAUDRATE  - Default Baudrate
 
// <323584=> 1200 baud 
// <643072=> 2400 baud 
// <1290240=> 4800 baud 
// <2576384=> 9600 baud 
// <3862528=> 14400 baud 
// <5152768=> 19200 baud 
// <7716864=> 28800 baud 
// <10289152=> 38400 baud 
// <15400960=> 57600 baud 
// <20615168=> 76800 baud 
// <30801920=> 115200 baud 
// <61865984=> 230400 baud 
// <67108864=> 250000 baud 
// <121634816=> 460800 baud 
// <251658240=> 921600 baud 
// <268435456=> 57600 baud 

#ifndef UART_DEFAULT_CONFIG_BAUDRATE
#define UART_DEFAULT_CONFIG_BAUDRATE 30801920
#endif

// <o> UART_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef UART_DEFAULT_CONFIG_IRQ_PRIORITY
#define UART_DEFAULT_CONFIG_IRQ_PRIORITY 7
#endif

// <q> UART_EASY_DMA_SUPPORT  - Driver supporting EasyDMA
 

#ifndef UART_EASY_DMA_SUPPORT
#define UART_EASY_DMA_SUPPORT 1
#endif

// <q> UART_LEGACY_SUPPORT  - Driver supporting Legacy mode
 

#ifndef UART_LEGACY_SUPPORT
#define UART_LEGACY_SUPPORT 1
#endif

// <e> UART0_ENABLED - Enable UART0 instance
//==========================================================
#ifndef UART0_ENABLED
#define UART0_ENABLED 1
#endif
#if  UART0_ENABLED
// <q> UART0_CONFIG_USE_EASY_DMA  - Default setting for using EasyDMA
 

#ifndef UART0_CONFIG_USE_EASY_DMA
#define UART0_CONFIG_USE_EASY_DMA 1
#endif

#endif //UART0_ENABLED
// </e>

// <e> UART_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef UART_CONFIG_LOG_ENABLED
#define UART_CONFIG_LOG_ENABLED 0
#endif
#if  UART_CONFIG_LOG_ENABLED
// <o> UART_CONFIG_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef UART_CONFIG_LOG_LEVEL
#define UART_CONFIG_LOG_LEVEL 3
#endif

// <o> UART_CONFIG_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef UART_CONFIG_INFO_COLOR
#define UART_CONFIG_INFO_COLOR 0
#endif

// <o> UART_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef UART_CONFIG_DEBUG_COLOR
#define UART_CONFIG_DEBUG_COLOR 0
#endif

#endif //UART_CONFIG_LOG_ENABLED
// </e>

#endif //UART_ENABLED
// </e>

// </h> 
//==========================================================

// <h> nRF_Libraries 

//==========================================================
// <e> APP_TIMER_ENABLED - app_timer - Application timer functionality
//==========================================================
#ifndef APP_TIMER_ENABLED
#define APP_TIMER_ENABLED 1
#endif
#if  APP_TIMER_ENABLED
// <q> APP_TIMER_WITH_PROFILER  - Enable app_timer profiling
 

#ifndef APP_TIMER_WITH_PROFILER
#define APP_TIMER_WITH_PROFILER 0
#endif

// <q> APP_TIMER_KEEPS_RTC_ACTIVE  - Enable RTC always on
 

// <i> If option is enabled RTC is kept running even if there is no active timers.
// <i> This option can be used when app_timer is used for timestamping.

#ifndef APP_TIMER_KEEPS_RTC_ACTIVE
#define APP_TIMER_KEEPS_RTC_ACTIVE 0
#endif

#endif //APP_TIMER_ENABLED
// </e>

// <q> BUTTON_ENABLED  - app_button - buttons handling module
 

#ifndef BUTTON_ENABLED
#define BUTTON_ENABLED 1
#endif

// <q> HARDFAULT_HANDLER_ENABLED  - hardfault_default - HardFault default handler for debugging and release
 

#ifndef HARDFAULT_HANDLER_ENABLED
#define HARDFAULT_HANDLER_ENABLED 1
#endif

// </h> 
//==========================================================

// <h> nRF_Log 

//==========================================================
// <e> NRF_LOG_ENABLED - nrf_log - Logging
//==========================================================
#ifndef NRF_LOG_ENABLED
#define NRF_LOG_ENABLED 1
#endif
#if  NRF_LOG_ENABLED
// <e> NRF_LOG_USES_COLORS - If enabled then ANSI escape code for colors is prefixed to every string
//==========================================================
#ifndef NRF_LOG_USES_COLORS
#define NRF_LOG_USES_COLORS 0
#endif
#if  NRF_LOG_USES_COLORS
// <o> NRF_LOG_COLOR_DEFAULT  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NRF_LOG_COLOR_DEFAULT
#define NRF_LOG_COLOR_DEFAULT 0
#endif

// <o> NRF_LOG_ERROR_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NRF_LOG_ERROR_COLOR
#define NRF_LOG_ERROR_COLOR 0
#endif

// <o> NRF_LOG_WARNING_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NRF_LOG_WARNING_COLOR
#define NRF_LOG_WARNING_COLOR 0
#endif

#endif //NRF_LOG_USES_COLORS
// </e>

// <o> NRF_LOG_DEFAULT_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NRF_LOG_DEFAULT_LEVEL
#define NRF_LOG_DEFAULT_LEVEL 3
#endif

// <e> NRF_LOG_DEFERRED - Enable deffered logger.

// <i> Log data is buffered and can be processed in idle.
//==========================================================
#ifndef NRF_LOG_DEFERRED
#define NRF_LOG_DEFERRED 1
#endif
#if  NRF_LOG_DEFERRED
// <o> NRF_LOG_DEFERRED_BUFSIZE - Size of the buffer for logs in words. 
// <i> Must be power of 2

#ifndef NRF_LOG_DEFERRED_BUFSIZE
#define NRF_LOG_DEFERRED_BUFSIZE 256
#endif

#endif //NRF_LOG_DEFERRED
// </e>

// <q> NRF_LOG_USES_TIMESTAMP  - Enable timestamping
 

// <i> Function for getting the timestamp is provided by the user

#ifndef NRF_LOG_USES_TIMESTAMP
#define NRF_LOG_USES_TIMESTAMP 0
#endif

#endif //NRF_LOG_ENABLED
// </e>

// <h> nrf_log_backend - Logging sink

//==========================================================
// <o> NRF_LOG_BACKEND_MAX_STRING_LENGTH - Buffer for storing single output string 
// <i> Logger backend RAM usage is determined by this value.

#ifndef NRF_LOG_BACKEND_MAX_STRING_LENGTH
#define NRF_LOG_BACKEND_MAX_STRING_LENGTH 256
#endif

// <o> NRF_LOG_TIMESTAMP_DIGITS - Number of digits for timestamp 
// <i> If higher resolution timestamp source is used it might be needed to increase that

#ifndef NRF_LOG_TIMESTAMP_DIGITS
#define NRF_LOG_TIMESTAMP_DIGITS 8
#endif

// <e> NRF_LOG_BACKEND_SERIAL_USES_UART - If enabled data is printed over UART
//==========================================================
#ifndef NRF_LOG_BACKEND_SERIAL_USES_UART
#define NRF_LOG_BACKEND_SERIAL_USES_UART 1
#endif
#if  NRF_LOG_BACKEND_SERIAL_USES_UART
// <o> NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE  - Default Baudrate
 
// <323584=> 1200 baud 
// <643072=> 2400 baud 
// <1290240=> 4800 baud 
// <2576384=> 9600 baud 
// <3862528=> 14400 baud 
// <5152768=> 19200 baud 
// <7716864=> 28800 baud 
// <10289152=> 38400 baud 
// <15400960=> 57600 baud 
// <20615168=> 76800 baud 
// <30801920=> 115200 baud 
// <61865984=> 230400 baud 
// <67108864=> 250000 baud 
// <121634816=> 460800 baud 
// <251658240=> 921600 baud 
// <268435456=> 57600 baud 

#ifndef NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE
#define NRF_LOG_BACKEND_SERIAL_UART_BAUDRATE 30801920
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_TX_PIN - UART TX pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_TX_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_TX_PIN 6
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_RX_PIN - UART RX pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_RX_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_RX_PIN 8
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_RTS_PIN - UART RTS pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_RTS_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_RTS_PIN 5
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_CTS_PIN - UART CTS pin 
#ifndef NRF_LOG_BACKEND_SERIAL_UART_CTS_PIN
#define NRF_LOG_BACKEND_SERIAL_UART_CTS_PIN 7
#endif

// <o> NRF_LOG_BACKEND_SERIAL_UART_FLOW_CONTROL  - Hardware Flow Control
 
// <0=> Disabled 
// <1=> Enabled 

#ifndef NRF_LOG_BACKEND_SERIAL_UART_FLOW_CONTROL
#define NRF_LOG_BACKEND_SERIAL_UART_FLOW_CONTROL 0
#endif

// <o> NRF_LOG_BACKEND_UART_INSTANCE  - UART instance used
 
// <0=> 0 

#ifndef NRF_LOG_BACKEND_UART_INSTANCE
#define NRF_LOG_BACKEND_UART_INSTANCE 0
#endif

#endif //NRF_LOG_BACKEND_SERIAL_USES_UART
// </e>

// <e> NRF_LOG_BACKEND_SERIAL_USES_RTT - If enabled data is printed using RTT
//==========================================================
#ifndef NRF_LOG_BACKEND_SERIAL_USES_RTT
#define NRF_LOG_BACKEND_SERIAL_USES_RTT 0
#endif
#if  NRF_LOG_BACKEND_SERIAL_USES_RTT
// <o> NRF_LOG_BACKEND_RTT_OUTPUT_BUFFER_SIZE - RTT output buffer size. 
// <i> Should be equal or bigger than \ref NRF_LOG_BACKEND_MAX_STRING_LENGTH.
// <i> This value is used in Segger RTT configuration to set the buffer size
// <i> if it is bigger than default RTT buffer size.

#ifndef NRF_LOG_BACKEND_RTT_OUTPUT_BUFFER_SIZE
#define NRF_LOG_BACKEND_RTT_OUTPUT_BUFFER_SIZE 512
#endif

#endif //NRF_LOG_BACKEND_SERIAL_USES_RTT
// </e>

// </h> 
//==========================================================

// </h> 
//==========================================================

// <h> nRF_Segger_RTT 

//==========================================================
// <h> segger_rtt - SEGGER RTT

//==========================================================
// <o> SEGGER_RTT_CONFIG_BUFFER_SIZE_UP - Size of upstream buffer. 
#ifndef SEGGER_RTT_CONFIG_BUFFER_SIZE_UP
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_UP 64
#endif

// <o> SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS - Size of upstream buffer. 
#ifndef SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS
#define SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS 2
#endif

// <o> SEGGER_RTT_CONFIG_BUFFER_SIZE_DOWN - Size of upstream buffer. 
#ifndef SEGGER_RTT_CONFIG_BUFFER_SIZE_DOWN
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_DOWN 16
#endif

// <o> SEGGER_RTT_CONFIG_MAX_NUM_DOWN_BUFFERS - Size of upstream buffer. 
#ifndef SEGGER_RTT_CONFIG_MAX_NUM_DOWN_BUFFERS
#define SEGGER_RTT_CONFIG_MAX_NUM_DOWN_BUFFERS 2
#endif

// </h> 
//==========================================================

// </h> 
//==========================================================

// <<< end of configuration section >>>
#endif //SDK_CONFIG_H


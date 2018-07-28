

#ifndef SDK_CONFIG_H
#define SDK_CONFIG_H
// <<< Use Configuration Wizard in Context Menu >>>\n
#ifdef USE_APP_CONFIG
#include "app_config.h"
#endif
// <h> Application 

//==========================================================
// <o> MAX_NDEF_RECORDS - Maximal number of NDEF records that can be stored in NDEF message descriptor. 
#ifndef MAX_NDEF_RECORDS
#define MAX_NDEF_RECORDS 10
#endif

// <o> MAX_TLV_BLOCKS - Maximal number of TLV blocks that can be stored in Type 2/4 Tag descriptor. 
#ifndef MAX_TLV_BLOCKS
#define MAX_TLV_BLOCKS 10
#endif

// <o> TAG_AFTER_READ_DELAY - Delay (in ms) after Tag read. 
#ifndef TAG_AFTER_READ_DELAY
#define TAG_AFTER_READ_DELAY 2000
#endif

// <o> TAG_DETECT_TIMEOUT - Timeout (in ms) for function which searches for a Tag. 
#ifndef TAG_DETECT_TIMEOUT
#define TAG_DETECT_TIMEOUT 5000
#endif

// <o> TAG_TYPE_2_DATA_BUFFER_SIZE - Buffer size for data from a Type 2 Tag Platform. 
#ifndef TAG_TYPE_2_DATA_BUFFER_SIZE
#define TAG_TYPE_2_DATA_BUFFER_SIZE 1024
#endif

// </h> 
//==========================================================

// <h> Third Parties 

//==========================================================
// <e> ADAFRUIT_PN532_ENABLED - adafruit_pn532 - Adafruit PN532 implementation for nRF5x
//==========================================================
#ifndef ADAFRUIT_PN532_ENABLED
#define ADAFRUIT_PN532_ENABLED 1
#endif
#if  ADAFRUIT_PN532_ENABLED
// <o> PN532_IRQ - Pin number  <0-31> 


#ifndef PN532_IRQ
#define PN532_IRQ 13
#endif

// <o> PN532_RESET - Pin number  <0-31> 


#ifndef PN532_RESET
#define PN532_RESET 14
#endif

// <o> PN532_CONFIG_SCL - Pin number  <0-31> 


#ifndef PN532_CONFIG_SCL
#define PN532_CONFIG_SCL 27
#endif

// <o> PN532_CONFIG_SDA - Pin number  <0-31> 


#ifndef PN532_CONFIG_SDA
#define PN532_CONFIG_SDA 26
#endif

// <o> PN532_CONFIG_TWI_INSTANCE  - TWI instance to be used
 
// <0=> 0 
// <1=> 1 
// <2=> 2 

#ifndef PN532_CONFIG_TWI_INSTANCE
#define PN532_CONFIG_TWI_INSTANCE 0
#endif

// <e> ADAFRUIT_PN532_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef ADAFRUIT_PN532_LOG_ENABLED
#define ADAFRUIT_PN532_LOG_ENABLED 0
#endif
#if  ADAFRUIT_PN532_LOG_ENABLED
// <o> ADAFRUIT_PN532_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef ADAFRUIT_PN532_LOG_LEVEL
#define ADAFRUIT_PN532_LOG_LEVEL 3
#endif

// <o> ADAFRUIT_PN532_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef ADAFRUIT_PN532_INFO_COLOR
#define ADAFRUIT_PN532_INFO_COLOR 0
#endif

#endif //ADAFRUIT_PN532_LOG_ENABLED
// </e>

// <o> PN532_PACKBUFF_SIZE - Size of the buffer used for sending commands and storing responses. 
#ifndef PN532_PACKBUFF_SIZE
#define PN532_PACKBUFF_SIZE 256
#endif

#endif //ADAFRUIT_PN532_ENABLED
// </e>

// </h> 
//==========================================================

// <h> nRF_Drivers 

//==========================================================
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

// <e> TWI_ENABLED - nrf_drv_twi - TWI/TWIM peripheral driver
//==========================================================
#ifndef TWI_ENABLED
#define TWI_ENABLED 1
#endif
#if  TWI_ENABLED
// <o> TWI_DEFAULT_CONFIG_FREQUENCY  - Frequency
 
// <26738688=> 100k 
// <67108864=> 250k 
// <104857600=> 400k 

#ifndef TWI_DEFAULT_CONFIG_FREQUENCY
#define TWI_DEFAULT_CONFIG_FREQUENCY 104857600
#endif

// <q> TWI_DEFAULT_CONFIG_CLR_BUS_INIT  - Enables bus clearing procedure during init
 

#ifndef TWI_DEFAULT_CONFIG_CLR_BUS_INIT
#define TWI_DEFAULT_CONFIG_CLR_BUS_INIT 0
#endif

// <q> TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT  - Enables bus holding after uninit
 

#ifndef TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT
#define TWI_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0
#endif

// <o> TWI_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef TWI_DEFAULT_CONFIG_IRQ_PRIORITY
#define TWI_DEFAULT_CONFIG_IRQ_PRIORITY 7
#endif

// <e> TWI0_ENABLED - Enable TWI0 instance
//==========================================================
#ifndef TWI0_ENABLED
#define TWI0_ENABLED 1
#endif
#if  TWI0_ENABLED
// <q> TWI0_USE_EASY_DMA  - Use EasyDMA (if present)
 

#ifndef TWI0_USE_EASY_DMA
#define TWI0_USE_EASY_DMA 0
#endif

#endif //TWI0_ENABLED
// </e>

// <e> TWI1_ENABLED - Enable TWI1 instance
//==========================================================
#ifndef TWI1_ENABLED
#define TWI1_ENABLED 0
#endif
#if  TWI1_ENABLED
// <q> TWI1_USE_EASY_DMA  - Use EasyDMA (if present)
 

#ifndef TWI1_USE_EASY_DMA
#define TWI1_USE_EASY_DMA 0
#endif

#endif //TWI1_ENABLED
// </e>

// <e> TWI_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef TWI_CONFIG_LOG_ENABLED
#define TWI_CONFIG_LOG_ENABLED 0
#endif
#if  TWI_CONFIG_LOG_ENABLED
// <o> TWI_CONFIG_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef TWI_CONFIG_LOG_LEVEL
#define TWI_CONFIG_LOG_LEVEL 3
#endif

// <o> TWI_CONFIG_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef TWI_CONFIG_INFO_COLOR
#define TWI_CONFIG_INFO_COLOR 0
#endif

// <o> TWI_CONFIG_DEBUG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef TWI_CONFIG_DEBUG_COLOR
#define TWI_CONFIG_DEBUG_COLOR 0
#endif

#endif //TWI_CONFIG_LOG_ENABLED
// </e>

#endif //TWI_ENABLED
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
// <q> APP_FIFO_ENABLED  - app_fifo - Software FIFO implementation
 

#ifndef APP_FIFO_ENABLED
#define APP_FIFO_ENABLED 1
#endif

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

// <e> APP_UART_ENABLED - app_uart - UART driver
//==========================================================
#ifndef APP_UART_ENABLED
#define APP_UART_ENABLED 1
#endif
#if  APP_UART_ENABLED
// <o> APP_UART_DRIVER_INSTANCE  - UART instance used
 
// <0=> 0 

#ifndef APP_UART_DRIVER_INSTANCE
#define APP_UART_DRIVER_INSTANCE 0
#endif

#endif //APP_UART_ENABLED
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

// <h> nRF_NFC 

//==========================================================
// <e> NFC_NDEF_MSG_ENABLED - nfc_ndef_msg - NFC NDEF Message generator module
//==========================================================
#ifndef NFC_NDEF_MSG_ENABLED
#define NFC_NDEF_MSG_ENABLED 1
#endif
#if  NFC_NDEF_MSG_ENABLED
// <o> NFC_NDEF_MSG_TAG_TYPE  - NFC Tag Type
 
// <2=> Type 2 Tag 
// <4=> Type 4 Tag 

#ifndef NFC_NDEF_MSG_TAG_TYPE
#define NFC_NDEF_MSG_TAG_TYPE 2
#endif

#endif //NFC_NDEF_MSG_ENABLED
// </e>

// <e> NFC_NDEF_MSG_PARSER_ENABLED - nfc_ndef_msg_parser - NFC NDEF message parser module
//==========================================================
#ifndef NFC_NDEF_MSG_PARSER_ENABLED
#define NFC_NDEF_MSG_PARSER_ENABLED 1
#endif
#if  NFC_NDEF_MSG_PARSER_ENABLED
// <e> NFC_NDEF_MSG_PARSER_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NFC_NDEF_MSG_PARSER_LOG_ENABLED
#define NFC_NDEF_MSG_PARSER_LOG_ENABLED 1
#endif
#if  NFC_NDEF_MSG_PARSER_LOG_ENABLED
// <o> NFC_NDEF_MSG_PARSER_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NFC_NDEF_MSG_PARSER_LOG_LEVEL
#define NFC_NDEF_MSG_PARSER_LOG_LEVEL 3
#endif

// <o> NFC_NDEF_MSG_PARSER_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NFC_NDEF_MSG_PARSER_INFO_COLOR
#define NFC_NDEF_MSG_PARSER_INFO_COLOR 0
#endif

#endif //NFC_NDEF_MSG_PARSER_LOG_ENABLED
// </e>

#endif //NFC_NDEF_MSG_PARSER_ENABLED
// </e>

// <e> NFC_NDEF_RECORD_PARSER_ENABLED - nfc_ndef_record_parser - NFC NDEF Record parser module
//==========================================================
#ifndef NFC_NDEF_RECORD_PARSER_ENABLED
#define NFC_NDEF_RECORD_PARSER_ENABLED 1
#endif
#if  NFC_NDEF_RECORD_PARSER_ENABLED
// <e> NFC_NDEF_RECORD_PARSER_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NFC_NDEF_RECORD_PARSER_LOG_ENABLED
#define NFC_NDEF_RECORD_PARSER_LOG_ENABLED 1
#endif
#if  NFC_NDEF_RECORD_PARSER_LOG_ENABLED
// <o> NFC_NDEF_RECORD_PARSER_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NFC_NDEF_RECORD_PARSER_LOG_LEVEL
#define NFC_NDEF_RECORD_PARSER_LOG_LEVEL 3
#endif

// <o> NFC_NDEF_RECORD_PARSER_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NFC_NDEF_RECORD_PARSER_INFO_COLOR
#define NFC_NDEF_RECORD_PARSER_INFO_COLOR 0
#endif

#endif //NFC_NDEF_RECORD_PARSER_LOG_ENABLED
// </e>

#endif //NFC_NDEF_RECORD_PARSER_ENABLED
// </e>

// <e> NFC_T2T_PARSER_ENABLED - nfc_type_2_tag_parser - Parser for decoding Type 2 Tag data
//==========================================================
#ifndef NFC_T2T_PARSER_ENABLED
#define NFC_T2T_PARSER_ENABLED 1
#endif
#if  NFC_T2T_PARSER_ENABLED
// <e> NFC_T2T_PARSER_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NFC_T2T_PARSER_LOG_ENABLED
#define NFC_T2T_PARSER_LOG_ENABLED 1
#endif
#if  NFC_T2T_PARSER_LOG_ENABLED
// <o> NFC_T2T_PARSER_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NFC_T2T_PARSER_LOG_LEVEL
#define NFC_T2T_PARSER_LOG_LEVEL 3
#endif

// <o> NFC_T2T_PARSER_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NFC_T2T_PARSER_INFO_COLOR
#define NFC_T2T_PARSER_INFO_COLOR 0
#endif

#endif //NFC_T2T_PARSER_LOG_ENABLED
// </e>

#endif //NFC_T2T_PARSER_ENABLED
// </e>

// <e> NFC_T4T_APDU_ENABLED - nfc_t4t_apdu - APDU encoder/decoder for Type 4 Tag
//==========================================================
#ifndef NFC_T4T_APDU_ENABLED
#define NFC_T4T_APDU_ENABLED 1
#endif
#if  NFC_T4T_APDU_ENABLED
// <e> NFC_T4T_APDU_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NFC_T4T_APDU_LOG_ENABLED
#define NFC_T4T_APDU_LOG_ENABLED 1
#endif
#if  NFC_T4T_APDU_LOG_ENABLED
// <o> NFC_T4T_APDU_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NFC_T4T_APDU_LOG_LEVEL
#define NFC_T4T_APDU_LOG_LEVEL 3
#endif

// <o> NFC_T4T_APDU_LOG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NFC_T4T_APDU_LOG_COLOR
#define NFC_T4T_APDU_LOG_COLOR 0
#endif

#endif //NFC_T4T_APDU_LOG_ENABLED
// </e>

#endif //NFC_T4T_APDU_ENABLED
// </e>

// <e> NFC_T4T_CC_FILE_PARSER_ENABLED - nfc_t4t_cc_file - Capability Container file for Type 4 Tag
//==========================================================
#ifndef NFC_T4T_CC_FILE_PARSER_ENABLED
#define NFC_T4T_CC_FILE_PARSER_ENABLED 1
#endif
#if  NFC_T4T_CC_FILE_PARSER_ENABLED
// <e> NFC_T4T_CC_FILE_PARSER_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NFC_T4T_CC_FILE_PARSER_LOG_ENABLED
#define NFC_T4T_CC_FILE_PARSER_LOG_ENABLED 1
#endif
#if  NFC_T4T_CC_FILE_PARSER_LOG_ENABLED
// <o> NFC_T4T_CC_FILE_PARSER_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NFC_T4T_CC_FILE_PARSER_LOG_LEVEL
#define NFC_T4T_CC_FILE_PARSER_LOG_LEVEL 3
#endif

// <o> NFC_T4T_CC_FILE_PARSER_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NFC_T4T_CC_FILE_PARSER_INFO_COLOR
#define NFC_T4T_CC_FILE_PARSER_INFO_COLOR 0
#endif

#endif //NFC_T4T_CC_FILE_PARSER_LOG_ENABLED
// </e>

#endif //NFC_T4T_CC_FILE_PARSER_ENABLED
// </e>

// <e> NFC_T4T_HL_DETECTION_PROCEDURES_ENABLED - nfc_t4t_hl_detection_procedures - NDEF Detection Procedure for Type 4 Tag
//==========================================================
#ifndef NFC_T4T_HL_DETECTION_PROCEDURES_ENABLED
#define NFC_T4T_HL_DETECTION_PROCEDURES_ENABLED 1
#endif
#if  NFC_T4T_HL_DETECTION_PROCEDURES_ENABLED
// <e> NFC_T4T_HL_DETECTION_PROCEDURES_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NFC_T4T_HL_DETECTION_PROCEDURES_LOG_ENABLED
#define NFC_T4T_HL_DETECTION_PROCEDURES_LOG_ENABLED 1
#endif
#if  NFC_T4T_HL_DETECTION_PROCEDURES_LOG_ENABLED
// <o> NFC_T4T_HL_DETECTION_PROCEDURES_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NFC_T4T_HL_DETECTION_PROCEDURES_LOG_LEVEL
#define NFC_T4T_HL_DETECTION_PROCEDURES_LOG_LEVEL 3
#endif

// <o> NFC_T4T_HL_DETECTION_PROCEDURES_LOG_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NFC_T4T_HL_DETECTION_PROCEDURES_LOG_COLOR
#define NFC_T4T_HL_DETECTION_PROCEDURES_LOG_COLOR 0
#endif

#endif //NFC_T4T_HL_DETECTION_PROCEDURES_LOG_ENABLED
// </e>

// <o> APDU_BUFF_SIZE - Size (in bytes) of the buffer for APDU storage 
#ifndef APDU_BUFF_SIZE
#define APDU_BUFF_SIZE 250
#endif

// <o> CC_STORAGE_BUFF_SIZE - Size (in bytes) of the buffer for CC file storage 
#ifndef CC_STORAGE_BUFF_SIZE
#define CC_STORAGE_BUFF_SIZE 64
#endif

#endif //NFC_T4T_HL_DETECTION_PROCEDURES_ENABLED
// </e>

// <e> NFC_T4T_TLV_BLOCK_PARSER_ENABLED - nfc_t4t_tlv_block - TLV block for Type 4 Tag
//==========================================================
#ifndef NFC_T4T_TLV_BLOCK_PARSER_ENABLED
#define NFC_T4T_TLV_BLOCK_PARSER_ENABLED 1
#endif
#if  NFC_T4T_TLV_BLOCK_PARSER_ENABLED
// <e> NFC_T4T_TLV_BLOCK_PARSER_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NFC_T4T_TLV_BLOCK_PARSER_LOG_ENABLED
#define NFC_T4T_TLV_BLOCK_PARSER_LOG_ENABLED 1
#endif
#if  NFC_T4T_TLV_BLOCK_PARSER_LOG_ENABLED
// <o> NFC_T4T_TLV_BLOCK_PARSER_LOG_LEVEL  - Default Severity level
 
// <0=> Off 
// <1=> Error 
// <2=> Warning 
// <3=> Info 
// <4=> Debug 

#ifndef NFC_T4T_TLV_BLOCK_PARSER_LOG_LEVEL
#define NFC_T4T_TLV_BLOCK_PARSER_LOG_LEVEL 3
#endif

// <o> NFC_T4T_TLV_BLOCK_PARSER_INFO_COLOR  - ANSI escape code prefix.
 
// <0=> Default 
// <1=> Black 
// <2=> Red 
// <3=> Green 
// <4=> Yellow 
// <5=> Blue 
// <6=> Magenta 
// <7=> Cyan 
// <8=> White 

#ifndef NFC_T4T_TLV_BLOCK_PARSER_INFO_COLOR
#define NFC_T4T_TLV_BLOCK_PARSER_INFO_COLOR 0
#endif

#endif //NFC_T4T_TLV_BLOCK_PARSER_LOG_ENABLED
// </e>

#endif //NFC_T4T_TLV_BLOCK_PARSER_ENABLED
// </e>

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


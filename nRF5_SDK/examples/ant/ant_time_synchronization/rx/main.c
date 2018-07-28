/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Dynastream Innovations, Inc. 2016
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 * 1) Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 
 * 2) Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 * 
 * 3) Neither the name of Dynastream nor the names of its
 *    contributors may be used to endorse or promote products
 *    derived from this software without specific prior
 *    written permission.
 * 
 * The following actions are prohibited:
 * 1) Redistribution of source code containing the ANT+ Network
 *    Key. The ANT+ Network Key is available to ANT+ Adopters.
 *    Please refer to http://thisisant.com to become an ANT+
 *    Adopter and access the key.
 * 
 * 2) Reverse engineering, decompilation, and/or disassembly of
 *    software provided in binary form under this license.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE HEREBY
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; DAMAGE TO ANY DEVICE, LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. SOME STATES DO NOT ALLOW
 * THE EXCLUSION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO THE
 * ABOVE LIMITATIONS MAY NOT APPLY TO YOU.
 * 
 */

/**@file
 * @defgroup nrf_ant_time_sync_rx_example ANT Time Sync RX Example
 * @{
 * @ingroup nrf_ant_time_sync
 *
 * @brief Example of ANT Time Synchronization RX.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S332 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S332 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s332/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include "app_error.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "ant_stack_config.h"
#include "ant_channel_config.h"
#include "ant_search_config.h"
#include "boards.h"
#include "nrf_drv_rtc.h"
#include "softdevice_handler.h"

// Channel configuration.
#define ANT_BROADCAST_CHANNEL_NUMBER    0x00 /**< ANT Channel 0. */
#define EXT_ASSIGN_NONE                 0x00 /**< ANT Ext Assign. */

// Arbitrary Page Numbers
#define TIME_SYNC_PAGE                  0x01
#define INVALID_PAGE                    0xFF

// Miscellaneous defines.
#define ANT_NETWORK_NUMBER              0x00  /**< Default public network number. */
#define ANT_RTC_CHANNEL                 0

const nrf_drv_rtc_t m_rtc = NRF_DRV_RTC_INSTANCE(1); /**< Declaring an instance of nrf_drv_rtc for RTC1. */

static uint8_t m_led_invert_next = 0; /**< Index of the next LED to be inverted. */

/**@brief Function for dispatching an ANT stack event to all modules with an ANT stack event handler.
 *
 * @details This function is called from the ANT Stack event interrupt handler after an ANT stack
 *          event has been received.
 *
 * @param[in] p_ant_evt  ANT stack event.
 */
void ant_evt_dispatch(ant_evt_t * p_ant_evt)
{
    ANT_MESSAGE * pstEventMessage = (ANT_MESSAGE *) p_ant_evt;

    if ((pstEventMessage->ANT_MESSAGE_ucMesgID == MESG_BROADCAST_DATA_ID)  // Check if this is a broadcast message
        && (pstEventMessage->ANT_MESSAGE_aucPayload[0] == TIME_SYNC_PAGE)) // Check if this is the time synchronization page
    {
        uint32_t time_stamp;
        uint32_t offset;
        uint32_t comp_chann_val;
        uint32_t led_status;

        // Get 4-byte RTC1 time stamp from extended data
        time_stamp = (pstEventMessage->ANT_MESSAGE_aucExtData[3] << 24) +
                     (pstEventMessage->ANT_MESSAGE_aucExtData[2] << 16) +
                     (pstEventMessage->ANT_MESSAGE_aucExtData[1] << 8) +
                     pstEventMessage->ANT_MESSAGE_aucExtData[0];

        // Get time sync offset from time sync packet
        offset = (pstEventMessage->ANT_MESSAGE_aucPayload[7] << 8) +
                 pstEventMessage->ANT_MESSAGE_aucPayload[6];

        // Calculate the next event time
        comp_chann_val = time_stamp - offset + LED_INVERT_PERIOD;

        // Setup RTC to fire during the next period
        ret_code_t err_code = nrf_drv_rtc_cc_set(&m_rtc, ANT_RTC_CHANNEL, comp_chann_val, true);
        APP_ERROR_CHECK(err_code);

        // Get LED status from the payload
        led_status = pstEventMessage->ANT_MESSAGE_aucPayload[5];

        // Get the next LED to invert at the next RTC interrupt
        m_led_invert_next = pstEventMessage->ANT_MESSAGE_aucPayload[4];

        // Set the LEDs based on the status from the transmitter
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            if ((1 << i) & led_status)
            {
                bsp_board_led_on(i);
            }
            else
            {
                bsp_board_led_off(i);
            }
        }
    }
}


/**@brief Function for ANT stack initialization.
 *
 * @details Initializes the SoftDevice and the ANT event interrupt.
 */
static void softdevice_setup(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    err_code = softdevice_ant_evt_handler_set(ant_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    err_code = softdevice_handler_init(&clock_lf_cfg, NULL, 0, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = ant_stack_static_config();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the LED's when RTC interrupt occurs.
 *
 */
static void led_event(void)
{
    uint32_t led_to_invert = ((m_led_invert_next++) % LEDS_NUMBER);

    bsp_board_led_invert(led_to_invert);
}


/** @brief: Function for handling the RTC1 interrupts.
 * Triggered on TICK and COMPARE0 match.
 */
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        led_event();

        // Get the current RTC counter value
        uint32_t count = nrf_drv_rtc_counter_get(&m_rtc);

        // Set the RTC channel to interrupt again after the LED_INVERT_PERIOD
        ret_code_t err_code = nrf_drv_rtc_cc_set(&m_rtc,
                                                 ANT_RTC_CHANNEL,
                                                 count + LED_INVERT_PERIOD,
                                                 true);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for setting up the ANT module to be ready for TX broadcast.
 */
static void ant_channel_rx_broadcast_setup(void)
{
    uint32_t              err_code;
    ANT_TIME_STAMP_CONFIG stampConfig;

    // Configure ANT Channel
    ant_channel_config_t broadcast_channel_config =
    {
        .channel_number    = ANT_BROADCAST_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_SLAVE,
        .ext_assign        = EXT_ASSIGN_NONE,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = CHAN_ID_DEV_NUM,
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUMBER,
    };

    // Configure received message timestamp to use RTC1 (4 byte counter)
    stampConfig.ucTimeBase        = ANT_TIME_BASE_ALT1;
    stampConfig.bTimeStampEnabled = true;
    err_code                      = sd_ant_time_stamp_config_set(&stampConfig);
    APP_ERROR_CHECK(err_code);

    // Initialize channel configuration
    err_code = ant_channel_init(&broadcast_channel_config);
    APP_ERROR_CHECK(err_code);

    // Disable high priority search timeout and set the proper channel number
    ant_search_config_t ant_search_config   = DEFAULT_ANT_SEARCH_CONFIG(ANT_BROADCAST_CHANNEL_NUMBER);
    ant_search_config.high_priority_timeout = ANT_HIGH_PRIORITY_TIMEOUT_DISABLE;

    // Set search timeout
    err_code = ant_search_init(&ant_search_config);
    APP_ERROR_CHECK(err_code);

    // Open channel.
    err_code = sd_ant_channel_open(ANT_BROADCAST_CHANNEL_NUMBER);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function initialization and configuration of RTC driver instance.
 */
static void rtc_config(void)
{
    uint32_t err_code;

    // Initialize RTC instance
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;

    err_code = nrf_drv_rtc_init(&m_rtc, &config, rtc_handler);
    APP_ERROR_CHECK(err_code);

    // Enable tick event & interrupt
    nrf_drv_rtc_tick_enable(&m_rtc, true);

    // Set compare channel to trigger interrupt after LED_INVERT_PERIOD
    err_code = nrf_drv_rtc_cc_set(&m_rtc, ANT_RTC_CHANNEL, LED_INVERT_PERIOD, true);
    APP_ERROR_CHECK(err_code);

    // Power on RTC instance
    nrf_drv_rtc_enable(&m_rtc);
}


/** @brief Function configuring GPIO for pin toggling.
 */
static void leds_config(void)
{
    // Configure all LED's on board.
    bsp_board_leds_init();
}


/**@brief Function for application main entry. Does not return.
 */
int main(void)
{
    softdevice_setup();
    rtc_config();
    leds_config();
    ant_channel_rx_broadcast_setup();

    // Main loop.
    for (;;)
    {
        // Put CPU in sleep if possible.
        uint32_t err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}



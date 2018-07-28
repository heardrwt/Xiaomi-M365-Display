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
 * @defgroup nrf_ant_time_sync_tx_example ANT Time Sync TX Example
 * @{
 * @ingroup nrf_ant_time_sync
 *
 * @brief Example of ANT Time Synchronization TX.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S332 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S332 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s332/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <string.h>
#include "app_error.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "ant_stack_config.h"
#include "ant_channel_config.h"
#include "boards.h"
#include "nordic_common.h"
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

static uint8_t m_led_status      = 0; /**< Current status of LEDs. */
static uint8_t m_led_invert_next = 0; /**< Index of the next LED to be inverted. */

/**@brief Function for setting payload for ANT message and sending it.
 */
void ant_time_sync_message_send()
{
    uint32_t err_code;
    uint8_t  message_payload[ANT_STANDARD_DATA_PAYLOAD_SIZE];
    uint32_t counter;

    // Get the current RTC counter value
    counter = nrf_drv_rtc_counter_get(&m_rtc);

    // Set the RTC channel to interrupt again after the LED_INVERT_PERIOD
    err_code = nrf_drv_rtc_cc_set(&m_rtc, ANT_RTC_CHANNEL, counter + LED_INVERT_PERIOD, true);
    APP_ERROR_CHECK(err_code);

    // Assign a new value to the broadcast data.
    memset(message_payload, 0, ANT_STANDARD_DATA_PAYLOAD_SIZE);
    message_payload[0] = TIME_SYNC_PAGE;
    message_payload[4] = m_led_invert_next;
    message_payload[5] = m_led_status;
    message_payload[6] = (uint8_t) LSB_32(counter);        // 2-byte counter LSB
    message_payload[7] = (uint8_t) LSB_32(counter >> 8);   // 2-byte counter MSB

    // Broadcast the data.
    err_code = sd_ant_time_sync_broadcast_tx(ANT_BROADCAST_CHANNEL_NUMBER,
                                             ANT_STANDARD_DATA_PAYLOAD_SIZE,
                                             message_payload);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for dispatching an ANT stack event to all modules with an ANT stack event handler.
 *
 * @details This function is called from the ANT Stack event interrupt handler after an ANT stack
 *          event has been received.
 *
 * @param[in] p_ant_evt  ANT stack event.
 */
void ant_evt_dispatch(ant_evt_t * p_ant_evt)
{
    if (p_ant_evt->channel == ANT_BROADCAST_CHANNEL_NUMBER)
    {
        switch (p_ant_evt->event)
        {
            case EVENT_TX:
                // Event called when message is sent
                break;

            default:
                break;
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
    bsp_board_led_invert(m_led_invert_next);

    m_led_status      ^= 1 << (m_led_invert_next++); // Update current status of LEDs
    m_led_invert_next %= LEDS_NUMBER;                // Update next LED to be inverted
}


/** @brief: Function for handling the RTC1 interrupts.
 * Triggered on COMPARE0 match.
 */
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        led_event();
        ant_time_sync_message_send();
    }
}


/**@brief Function for setting up the ANT module to be ready for TX broadcast.
 */
static void ant_channel_tx_broadcast_setup(void)
{
    uint32_t             err_code;
    ANT_TIME_SYNC_CONFIG syncConfig;

    // Configure ANT Time Synchronization using RTC1
    syncConfig.ucTimeBase           = ANT_TIME_BASE_ALT1; // RTC1
    syncConfig.bInvalidationEnabled = true;
    syncConfig.ucInvalidationByte   = INVALID_PAGE;       // Invalid page number set to 0xFF
    err_code                        = sd_ant_time_sync_config_set(&syncConfig);
    APP_ERROR_CHECK(err_code);

    // Configure ANT Channel
    ant_channel_config_t broadcast_channel_config =
    {
        .channel_number    = ANT_BROADCAST_CHANNEL_NUMBER,
        .channel_type      = CHANNEL_TYPE_MASTER,
        .ext_assign        = EXT_ASSIGN_NONE,
        .rf_freq           = RF_FREQ,
        .transmission_type = CHAN_ID_TRANS_TYPE,
        .device_type       = CHAN_ID_DEV_TYPE,
        .device_number     = CHAN_ID_DEV_NUM,
        .channel_period    = CHAN_PERIOD,
        .network_number    = ANT_NETWORK_NUMBER,
    };

    err_code = ant_channel_init(&broadcast_channel_config);
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
    ant_channel_tx_broadcast_setup();

    // Main loop.
    for (;;)
    {
        // Put CPU in sleep if possible.
        uint32_t err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}



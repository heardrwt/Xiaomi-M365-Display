/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Dynastream Innovations, Inc. 2012
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
 * @defgroup ant_sdm_rx_example ANT SDM RX example
 * @{
 * @ingroup nrf_ant_sdm
 *
 * @brief Example of ANT SDM RX Profile.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"
#include "bsp.h"
#include "hardfault.h"
#include "app_timer.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ant_sdm.h"
#include "ant_key_manager.h"
#include "ant_state_indicator.h"
#include "bsp_btn_ant.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define APP_TIMER_PRESCALER         0x00 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE     0x04 /**< Size of timer operation queues. */

#define SDM_CHANNEL_NUMBER          0x00 /**< Channel number assigned to SDM Profile. */

#define WILDCARD_TRANSMISSION_TYPE  0x00 /**< Wildcard transmission type. */
#define WILDCARD_DEVICE_NUMBER      0x00 /**< Wildcard device number. */

#define ANTPLUS_NETWORK_NUMBER      0x00 /**< Network number. */

/** @snippet [ANT SDM RX Instance] */
void ant_sdm_evt_handler(ant_sdm_profile_t * p_profile, ant_sdm_evt_t event);

SDM_DISP_CHANNEL_CONFIG_DEF(m_ant_sdm,
                            SDM_CHANNEL_NUMBER,
                            WILDCARD_TRANSMISSION_TYPE,
                            WILDCARD_DEVICE_NUMBER,
                            ANTPLUS_NETWORK_NUMBER,
                            SDM_MSG_PERIOD_4Hz);
SDM_DISP_PROFILE_CONFIG_DEF(m_ant_sdm,
                            ant_sdm_evt_handler);

static ant_sdm_profile_t m_ant_sdm;
/** @snippet [ANT SDM RX Instance] */


/**@brief Function for handling bsp events.
 */
void bsp_evt_handler(bsp_event_t evt)
{
    uint32_t                 err_code;
    ant_common_page70_data_t page70;

    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            page70   = ANT_COMMON_PAGE_DATA_REQUEST(ANT_SDM_PAGE_16);
            err_code = ant_sdm_page_request(&m_ant_sdm, &page70);
            APP_ERROR_CHECK(err_code);
            break;

        case BSP_EVENT_KEY_1:
            page70   = ANT_COMMON_PAGE_DATA_REQUEST(ANT_SDM_PAGE_22);
            err_code = ant_sdm_page_request(&m_ant_sdm, &page70);
            APP_ERROR_CHECK(err_code);
            break;

        case BSP_EVENT_SLEEP:
            ant_state_indicator_sleep_mode_enter();
            break;

        default:
            return; // no implementation needed
    }
}


/**@brief Function for dispatching an ANT stack event to all modules with an ANT stack event handler.
 *
 * @details This function is called from the ANT stack event interrupt handler after an ANT stack
 *          event has been received.
 *
 * @param[in] p_ant_evt  ANT stack event.
 */
void ant_evt_dispatch(ant_evt_t * p_ant_evt)
{
    ant_sdm_disp_evt_handler(&m_ant_sdm, p_ant_evt);
    ant_state_indicator_evt_handler(p_ant_evt);
    bsp_btn_ant_on_ant_evt(p_ant_evt);
}


/**@brief Function for the timer, tracer, and BSP initialization.
 */
static void utils_setup(void)
{
    uint32_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                        APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                        bsp_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ant_init();
    APP_ERROR_CHECK(err_code);
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

    err_code = ant_plus_key_set(ANTPLUS_NETWORK_NUMBER);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling ANT SDM events.
 */
void ant_sdm_evt_handler(ant_sdm_profile_t * p_profile, ant_sdm_evt_t event)
{
    switch (event)
    {
        case ANT_SDM_PAGE_1_UPDATED:
            /* fall through */
        case ANT_SDM_PAGE_2_UPDATED:
            /* fall through */
        case ANT_SDM_PAGE_3_UPDATED:
            /* fall through */
        case ANT_SDM_PAGE_16_UPDATED:
            /* fall through */
        case ANT_SDM_PAGE_22_UPDATED:
            /* fall through */
        case ANT_SDM_PAGE_80_UPDATED:
            /* fall through */
        case ANT_SDM_PAGE_81_UPDATED:
            NRF_LOG_INFO("Page was updated\r\n\r\n");
            break;

        case ANT_SDM_PAGE_REQUEST_SUCCESS:
            NRF_LOG_INFO("ANT_SDM_PAGE_REQUEST_SUCCESS\r\n\r\n");
            break;

        case ANT_SDM_PAGE_REQUEST_FAILED:
            NRF_LOG_INFO("ANT_SDM_PAGE_REQUEST_FAILED\r\n\r\n");
            break;

        default:
            break;
    }
}


/**@brief Function for SDM Profile initialization.
 *
 * @details Initializes the SDM Profile and opens the ANT channel.
 */
static void profile_setup(void)
{
/** @snippet [ANT SDM RX Profile Setup] */
    uint32_t err_code;

    err_code = ant_sdm_disp_init(&m_ant_sdm,
                                 SDM_DISP_CHANNEL_CONFIG(m_ant_sdm),
                                 SDM_DISP_PROFILE_CONFIG(m_ant_sdm));
    APP_ERROR_CHECK(err_code);

    err_code = ant_sdm_disp_open(&m_ant_sdm);
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_channel_opened();
    APP_ERROR_CHECK(err_code);
/** @snippet [ANT SDM RX Profile Setup] */
}


/**@brief Function for application main entry, does not return.
 */
int main(void)
{
    uint32_t err_code;

    utils_setup();
    softdevice_setup();
    ant_state_indicator_init(m_ant_sdm.channel_number, SDM_DISP_CHANNEL_TYPE);
    profile_setup();

    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            err_code = sd_app_evt_wait();
            APP_ERROR_CHECK(err_code);
        }
    }
}


/**
 *@}
 **/

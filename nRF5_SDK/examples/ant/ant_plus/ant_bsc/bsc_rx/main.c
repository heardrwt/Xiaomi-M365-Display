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
 * @defgroup ant_bsc_rx_example ANT BSC RX example
 * @{
 * @ingroup nrf_ant_bsc
 *
 * @brief Example of ANT BSC RX profile.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_error.h"
#include "bsp.h"
#include "hardfault.h"
#include "app_timer.h"
#include "ant_stack_config.h"
#include "softdevice_handler.h"
#include "ant_bsc.h"
#include "ant_key_manager.h"
#include "ant_state_indicator.h"
#include "bsp_btn_ant.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define APP_TIMER_PRESCALER         0x00                                                            /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE     0x04                                                            /**< Size of timer operation queues. */

#define BSC_CHANNEL_NUMBER          0x00                                                            /**< Channel number assigned to BSC profile. */

#define WILDCARD_TRANSMISSION_TYPE  0x00                                                            /**< Wildcard transmission type. */
#define WILDCARD_DEVICE_NUMBER      0x00                                                            /**< Wildcard device number. */

#define ANTPLUS_NETWORK_NUMBER      0x00                                                            /**< Network number. */

#define WHEEL_CIRCUMFERENCE         2070                                                            /**< Bike wheel circumference [mm] */
#define BSC_EVT_TIME_FACTOR         1024                                                            /**< Time unit factor for BSC events */
#define BSC_RPM_TIME_FACTOR         60                                                              /**< Time unit factor for RPM unit */
#define BSC_MS_TO_KPH_NUM           36                                                              /**< Numerator of [m/s] to [kph] ratio */
#define BSC_MS_TO_KPH_DEN           10                                                              /**< Denominator of [m/s] to [kph] ratio */
#define BSC_MM_TO_M_FACTOR          1000                                                            /**< Unit factor [m/s] to [mm/s] */
#define SPEED_COEFFICIENT           (WHEEL_CIRCUMFERENCE * BSC_EVT_TIME_FACTOR * BSC_MS_TO_KPH_NUM \
                                     / BSC_MS_TO_KPH_DEN / BSC_MM_TO_M_FACTOR)                      /**< Coefficient for speed value calculation */
#define CADENCE_COEFFICIENT         (BSC_EVT_TIME_FACTOR * BSC_RPM_TIME_FACTOR)                     /**< Coefficient for cadence value calculation */

typedef struct
{
    int32_t acc_rev_cnt;
    int32_t prev_rev_cnt;
    int32_t prev_acc_rev_cnt;
    int32_t acc_evt_time;
    int32_t prev_evt_time;
    int32_t prev_acc_evt_time;
} bsc_disp_calc_data_t;

static bsc_disp_calc_data_t m_speed_calc_data   = {0};
static bsc_disp_calc_data_t m_cadence_calc_data = {0};

/**@brief Application ANT event handler.
 *
 * @details This function is used to detect disconnection from the sensor device.
 *
 * @param[in] p_ant_evt  Pointer to ANT stack event structure.
 */
static void on_ant_evt(ant_evt_t * p_ant_event);

/** @snippet [ANT BSC RX Instance] */
void ant_bsc_evt_handler(ant_bsc_profile_t * p_profile, ant_bsc_evt_t event);

BSC_DISP_CHANNEL_CONFIG_DEF(m_ant_bsc,
                            BSC_CHANNEL_NUMBER,
                            WILDCARD_TRANSMISSION_TYPE,
                            DISPLAY_TYPE,
                            WILDCARD_DEVICE_NUMBER,
                            ANTPLUS_NETWORK_NUMBER,
                            BSC_MSG_PERIOD_4Hz);
BSC_DISP_PROFILE_CONFIG_DEF(m_ant_bsc,
                            ant_bsc_evt_handler);
ant_bsc_profile_t m_ant_bsc;
/** @snippet [ANT BSC RX Instance] */

/**@brief Function for dispatching a ANT stack event to all modules with a ANT stack event handler.
 *
 * @details This function is called from the ANT Stack event interrupt handler after a ANT stack
 *          event has been received.
 *
 * @param[in] p_ant_evt  ANT stack event.
 */
void ant_evt_dispatch(ant_evt_t * p_ant_evt)
{
    ant_bsc_disp_evt_handler(&m_ant_bsc, p_ant_evt);
    ant_state_indicator_evt_handler(p_ant_evt);
    bsp_btn_ant_on_ant_evt(p_ant_evt);
    on_ant_evt(p_ant_evt);
}

__STATIC_INLINE uint32_t calculate_speed(int32_t rev_cnt, int32_t evt_time)
{
    static uint32_t computed_speed   = 0;

    if (rev_cnt != m_speed_calc_data.prev_rev_cnt)
    {
        m_speed_calc_data.acc_rev_cnt  += rev_cnt - m_speed_calc_data.prev_rev_cnt;
        m_speed_calc_data.acc_evt_time += evt_time - m_speed_calc_data.prev_evt_time;

        /* Process rollover */
        if (m_speed_calc_data.prev_rev_cnt > rev_cnt)
        {
            m_speed_calc_data.acc_rev_cnt += UINT16_MAX + 1;
        }
        if (m_speed_calc_data.prev_evt_time > evt_time)
        {
            m_speed_calc_data.acc_evt_time += UINT16_MAX + 1;
        }

        m_speed_calc_data.prev_rev_cnt  = rev_cnt;
        m_speed_calc_data.prev_evt_time = evt_time;

        computed_speed = SPEED_COEFFICIENT *
                         (m_speed_calc_data.acc_rev_cnt  - m_speed_calc_data.prev_acc_rev_cnt) /
                         (m_speed_calc_data.acc_evt_time - m_speed_calc_data.prev_acc_evt_time);

        m_speed_calc_data.prev_acc_rev_cnt  = m_speed_calc_data.acc_rev_cnt;
        m_speed_calc_data.prev_acc_evt_time = m_speed_calc_data.acc_evt_time;
    }

    return (uint32_t) computed_speed;
}

static uint32_t calculate_cadence(int32_t rev_cnt, int32_t evt_time)
{
    static uint32_t computed_cadence = 0;

    if (rev_cnt != m_cadence_calc_data.prev_rev_cnt)
    {
        m_cadence_calc_data.acc_rev_cnt  += rev_cnt - m_cadence_calc_data.prev_rev_cnt;
        m_cadence_calc_data.acc_evt_time += evt_time - m_cadence_calc_data.prev_evt_time;

        /* Process rollover */
        if (m_cadence_calc_data.prev_rev_cnt > rev_cnt)
        {
            m_cadence_calc_data.acc_rev_cnt += UINT16_MAX + 1;
        }
        if (m_cadence_calc_data.prev_evt_time > evt_time)
        {
            m_cadence_calc_data.acc_evt_time += UINT16_MAX + 1;
        }

        m_cadence_calc_data.prev_rev_cnt  = rev_cnt;
        m_cadence_calc_data.prev_evt_time = evt_time;

        computed_cadence = CADENCE_COEFFICIENT *
                        (m_cadence_calc_data.acc_rev_cnt  - m_cadence_calc_data.prev_acc_rev_cnt) /
                        (m_cadence_calc_data.acc_evt_time - m_cadence_calc_data.prev_acc_evt_time);

        m_cadence_calc_data.prev_acc_rev_cnt  = m_cadence_calc_data.acc_rev_cnt;
        m_cadence_calc_data.prev_acc_evt_time = m_cadence_calc_data.acc_evt_time;
    }

    return (uint32_t) computed_cadence;
}

static void on_ant_evt(ant_evt_t * p_ant_event)
{
    switch (p_ant_event->event)
    {
        case EVENT_RX_FAIL_GO_TO_SEARCH:
            /* Reset speed and cadence values */
            memset(&m_speed_calc_data, 0, sizeof(m_speed_calc_data));
            memset(&m_cadence_calc_data, 0, sizeof(m_cadence_calc_data));
            break;

        default:
            // No implementation needed
            break;
    }
}

void ant_bsc_evt_handler(ant_bsc_profile_t * p_profile, ant_bsc_evt_t event)
{

    switch (event)
    {
        case ANT_BSC_PAGE_0_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_1_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_2_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_3_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_4_UPDATED:
            /* fall through */
        case ANT_BSC_PAGE_5_UPDATED:
            /* Log computed value */
            NRF_LOG_RAW_INFO("\r\n");

            if (DISPLAY_TYPE == BSC_SPEED_DEVICE_TYPE)
            {
                NRF_LOG_INFO("Computed speed value:                 %u kph\r\n\n",
                              (unsigned int) calculate_speed(p_profile->BSC_PROFILE_rev_count,
                                                             p_profile->BSC_PROFILE_event_time));
            }
            else if (DISPLAY_TYPE == BSC_CADENCE_DEVICE_TYPE)
            {
                NRF_LOG_INFO("Computed cadence value:               %u rpm\r\n\n",
                              (unsigned int) calculate_cadence(p_profile->BSC_PROFILE_rev_count,
                                                               p_profile->BSC_PROFILE_event_time));
            }
            break;

        case ANT_BSC_COMB_PAGE_0_UPDATED:
            NRF_LOG_INFO("Computed speed value:                         %u kph\r\n",
                          (unsigned int) calculate_speed(p_profile->BSC_PROFILE_speed_rev_count,
                                                         p_profile->BSC_PROFILE_speed_event_time));
            NRF_LOG_INFO("Computed cadence value:                       %u rpm\r\n\n",
                          (unsigned int) calculate_cadence(p_profile->BSC_PROFILE_cadence_rev_count,
                                                           p_profile->BSC_PROFILE_cadence_event_time));
            break;

        default:
            break;
    }
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by BSP.
 */
void bsp_event_handler(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            ant_state_indicator_sleep_mode_enter();
            break;

        default:
            break;
    }
}

/**@brief Function for the Timer, Tracer and BSP initialization.
 */
static void utils_setup(void)
{
    uint32_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                        APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                        bsp_event_handler);
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

    err_code = ant_stack_static_config();
    APP_ERROR_CHECK(err_code);

    err_code = ant_plus_key_set(ANTPLUS_NETWORK_NUMBER);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for BSC profile initialization.
 *
 * @details Initializes the BSC profile and open ANT channel.
 */
static void profile_setup(void)
{
/** @snippet [ANT BSC RX Profile Setup] */
    uint32_t err_code;

    err_code = ant_bsc_disp_init(&m_ant_bsc,
                                 BSC_DISP_CHANNEL_CONFIG(m_ant_bsc),
                                 BSC_DISP_PROFILE_CONFIG(m_ant_bsc));
    APP_ERROR_CHECK(err_code);

    err_code = ant_bsc_disp_open(&m_ant_bsc);
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_channel_opened();
    APP_ERROR_CHECK(err_code);
/** @snippet [ANT BSC RX Profile Setup] */
}


/**@brief Function for application main entry, does not return.
 */
int main(void)
{
    uint32_t err_code;

    utils_setup();
    softdevice_setup();
    ant_state_indicator_init(m_ant_bsc.channel_number, BSC_DISP_CHANNEL_TYPE);
    profile_setup();

    for (;; )
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

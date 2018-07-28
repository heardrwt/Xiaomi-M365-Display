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
 * @defgroup ant_bsc_tx_main ANT BSC TX example
 * @{
 * @ingroup nrf_ant_bsc
 *
 * @brief Example of ANT BSC TX profile.
 *
 * Before compiling this example for NRF52, complete the following steps:
 * - Download the S212 SoftDevice from <a href="https://www.thisisant.com/developer/components/nrf52832" target="_blank">thisisant.com</a>.
 * - Extract the downloaded zip file and copy the S212 SoftDevice headers to <tt>\<InstallFolder\>/components/softdevice/s212/headers</tt>.
 * If you are using Keil packs, copy the files into a @c headers folder in your example folder.
 * - Make sure that @ref ANT_LICENSE_KEY in @c nrf_sdm.h is uncommented.
 */

#include <stdio.h>
#include "nrf.h"
#include "nrf_soc.h"
#include "bsp.h"
#include "hardfault.h"
#include "app_error.h"
#include "nordic_common.h"
#include "ant_stack_config.h"
#include "softdevice_handler.h"
#include "ant_bsc.h"
#include "ant_state_indicator.h"
#include "ant_key_manager.h"
#include "app_timer.h"
#include "ant_bsc_simulator.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define MODIFICATION_TYPE_BUTTON 0 /* predefined value, MUST REMAIN UNCHANGED */
#define MODIFICATION_TYPE_AUTO   1 /* predefined value, MUST REMAIN UNCHANGED */

#if (MODIFICATION_TYPE != MODIFICATION_TYPE_BUTTON) \
    && (MODIFICATION_TYPE != MODIFICATION_TYPE_AUTO)

    #error Unsupported value of MODIFICATION_TYPE.
#endif

#define APP_TIMER_PRESCALER     0x00    /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE 0x04    /**< Size of timer operation queues. */
#define APP_TICK_EVENT_INTERVAL APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER) /**< 2 second's tick event interval in timer tick units. */
#define BSC_CHANNEL_NUMBER      0x00    /**< Channel number assigned to BSC profile. */
#define ANTPLUS_NETWORK_NUMBER  0       /**< Network number. */

/** @snippet [ANT BSC TX Instance] */
void ant_bsc_evt_handler(ant_bsc_profile_t * p_profile, ant_bsc_evt_t event);

BSC_SENS_CHANNEL_CONFIG_DEF(m_ant_bsc,
                            BSC_CHANNEL_NUMBER,
                            CHAN_ID_TRANS_TYPE,
                            SENSOR_TYPE,
                            CHAN_ID_DEV_NUM,
                            ANTPLUS_NETWORK_NUMBER);
BSC_SENS_PROFILE_CONFIG_DEF(m_ant_bsc,
                            true,
                            true,
                            ANT_BSC_PAGE_5,
                            ant_bsc_evt_handler);

static ant_bsc_profile_t m_ant_bsc;
/** @snippet [ANT BSC TX Instance] */

static ant_bsc_simulator_t  m_ant_bsc_simulator;    /**< Simulator used to simulate profile data. */


#if MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON
/**@brief Function for handling bsp events.
 */
/** @snippet [ANT BSC simulator button] */
void bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_KEY_0:
            ant_bsc_simulator_increment(&m_ant_bsc_simulator);
            break;

        case BSP_EVENT_KEY_1:
            ant_bsc_simulator_decrement(&m_ant_bsc_simulator);
            break;

        default:
            return; // no implementation needed
    }
}


/** @snippet [ANT BSC simulator button] */
#endif // MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON


/**@brief Function for dispatching a ANT stack event to all modules with a ANT stack event handler.
 *
 * @details This function is called from the ANT Stack event interrupt handler after a ANT stack
 *          event has been received.
 *
 * @param[in] p_ant_evt  ANT stack event.
 */
void ant_evt_dispatch(ant_evt_t * p_ant_evt)
{
    ant_bsc_sens_evt_handler(&m_ant_bsc, p_ant_evt);
    ant_state_indicator_evt_handler(p_ant_evt);
}


/**
 * @brief Function for setup all thinks not directly associated witch ANT stack/protocol.
 * @desc Initialization of: @n
 *         - app_tarce for debug.
 *         - app_timer, presetup for bsp and ant pulse simulation.
 *         - bsp for signaling leds and user buttons (if use button is enabled in example).
 *         - ant pulse simulate for task of filling bsc profile data.
 */
static void utils_setup(void)
{
    uint32_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize and start a single continuous mode timer, which is used to update the event time
    // on the main data page.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);

    #if (MODIFICATION_TYPE == MODIFICATION_TYPE_BUTTON)
    /** @snippet [ANT Pulse simulator button init] */
    err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                        APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                        bsp_evt_handler);
    APP_ERROR_CHECK(err_code);
    /** @snippet [ANT Pulse simulator button init] */
    #else
    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
    #endif
}


/**@brief Function for the BSC simulator initialization.
 */
static void simulator_setup(void)
{
    /** @snippet [ANT BSC simulator init] */
    const ant_bsc_simulator_cfg_t simulator_cfg =
    {
        .p_profile      = &m_ant_bsc,
        .device_type    = SENSOR_TYPE,
    };
    /** @snippet [ANT BSC simulator init] */

#if MODIFICATION_TYPE == MODIFICATION_TYPE_AUTO
    /** @snippet [ANT BSC simulator auto init] */
    ant_bsc_simulator_init(&m_ant_bsc_simulator, &simulator_cfg, true);
    /** @snippet [ANT BSC simulator auto init] */
#else
    /** @snippet [ANT BSC simulator button init] */
    ant_bsc_simulator_init(&m_ant_bsc_simulator, &simulator_cfg, false);
    /** @snippet [ANT BSC simulator button init] */
#endif
}


/**
 * @brief Function for ANT stack initialization.
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

    err_code = ant_stack_static_config(); // set ant resource
    APP_ERROR_CHECK(err_code);

    err_code = ant_plus_key_set(ANTPLUS_NETWORK_NUMBER);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling ANT BSC events.
 */
/** @snippet [ANT BSC simulator call] */
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
            /* fall through */
        case ANT_BSC_COMB_PAGE_0_UPDATED:
            ant_bsc_simulator_one_iteration(&m_ant_bsc_simulator);
            break;

        default:
            break;
    }
}


/** @snippet [ANT BSC simulator call] */

/**
 * @brief Function for BSC profile initialization.
 *
 * @details Initializes the BSC profile and open ANT channel.
 */
static void profile_setup(void)
{
/** @snippet [ANT BSC TX Profile Setup] */
    uint32_t err_code;

    err_code = ant_bsc_sens_init(&m_ant_bsc,
                                 BSC_SENS_CHANNEL_CONFIG(m_ant_bsc),
                                 BSC_SENS_PROFILE_CONFIG(m_ant_bsc));
    APP_ERROR_CHECK(err_code);

    m_ant_bsc.BSC_PROFILE_manuf_id     = BSC_MF_ID;
    m_ant_bsc.BSC_PROFILE_serial_num   = BSC_SERIAL_NUMBER;
    m_ant_bsc.BSC_PROFILE_hw_version   = BSC_HW_VERSION;
    m_ant_bsc.BSC_PROFILE_sw_version   = BSC_SW_VERSION;
    m_ant_bsc.BSC_PROFILE_model_num    = BSC_MODEL_NUMBER;

    err_code = ant_bsc_sens_open(&m_ant_bsc);
    APP_ERROR_CHECK(err_code);

    err_code = ant_state_indicator_channel_opened();
    APP_ERROR_CHECK(err_code);
/** @snippet [ANT BSC TX Profile Setup] */
}


/**@brief Function for application main entry, does not return.
 */
int main(void)
{
    uint32_t err_code;

    utils_setup();
    softdevice_setup();
    ant_state_indicator_init(m_ant_bsc.channel_number, BSC_SENS_CHANNEL_TYPE);
    simulator_setup();
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

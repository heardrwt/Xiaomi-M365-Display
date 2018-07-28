/**
 * This software is subject to the ANT+ Shared Source License
 * www.thisisant.com/swlicenses
 * Copyright (c) Dynastream Innovations, Inc. 2015
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

#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "app_error.h"
#include "app_timer.h"
#include "bsp.h"
#include "boards.h"
#include "hardfault.h"
#include "softdevice_handler.h"
#include "ant_stack_config.h"
#include "application.h"
#include "debug.h"

#define APP_TIMER_PRESCALER             0                   /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         2u                  /**< Size of timer operation queues. */
#define BTN_ID_ERROR                    0                   /**< ID of button used to test error handling. */


/**@brief Function for error handling, which demonstrates usage of the ANT debug
 * module to output debug information over the debug channel on an assertion
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
    #if DEBUG_CHANNEL_INCLUDED
    error_info_t * p_error_info = (error_info_t *)info;
    char     file_name[2];
    uint32_t file_name_len = strlen((char *)p_error_info->p_file_name);

    file_name[0] = '\0';
    file_name[1] = '\0';

    for (uint32_t i = 0; i < file_name_len; i++)
    {
        if ((p_error_info->p_file_name[i] != '.')
         && (p_error_info->p_file_name[i] != '/')
         && (p_error_info->p_file_name[i] != '\\'))
        {
            file_name[0] = p_error_info->p_file_name[i + 1];
            file_name[1] = p_error_info->p_file_name[i];
            break;
        }
    }

    ad_error_page_force((uint8_t)p_error_info->err_code,
                        (uint16_t)p_error_info->line_num,
                        file_name);
    #endif // DEBUG_CHANNEL_INCLUDED

    bsp_board_leds_on();

    for (;;)
    {
        // No implementation needed.
    }
}


/**@brief Function for handling bsp events.
 */
void bsp_evt_handler(bsp_event_t evt)
{
    switch (evt)
    {
        case BSP_EVENT_RESET:
            APP_ERROR_HANDLER(NRF_ERROR_INTERNAL);
            break;

        default:
            break; // No implementation needed
    }
}


/**@brief Function for dispatching a ANT stack event to all modules with a ANT stack event handler.
 *
 * @details This function is called from the ANT Stack event interrupt handler after a ANT stack
 *          event has been received.
 *
 * @param[in] p_ant_evt  ANT stack event.
 */
void ant_evt_dispatch(ant_evt_t * p_ant_evt)
{
    switch (p_ant_evt->channel)
    {
        case APPLICATION_CHANNEL:
            app_channel_event_handler(p_ant_evt);
            break;

    #if DEBUG_CHANNEL_INCLUDED
        case DEBUG_CHANNEL:
            // Transmit GPIO register values on debug channel
            ad_debug_field_set(ANT_DEBUG_FIELD_GPIO_REGISTER_LOW, (uint16_t)(NRF_GPIO->IN));
            ad_debug_field_set(ANT_DEBUG_FIELD_GPIO_REGISTER_HIGH, (uint16_t)(NRF_GPIO->IN >> 16));

            // If event is from debug channel, let the debug handler handle it
            ad_ant_event_process(p_ant_evt);
            break;
    #endif // DEBUG_CHANNEL_INCLUDED

        default:
            break; // No implementation needed
    }
}

/**@brief Function for the Timer and BSP initialization.
 */
static void utils_setup(void)
{
    uint32_t err_code;

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                        APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                        bsp_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_event_to_button_action_assign(BTN_ID_ERROR,
                                                 BSP_BUTTON_ACTION_LONG_PUSH,
                                                 BSP_EVENT_RESET);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for application main entry. Does not return.
 */
int main(void)
{
    uint32_t err_code;
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    utils_setup();

    // Enable SoftDevice.
    err_code = softdevice_ant_evt_handler_set(ant_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    err_code = softdevice_handler_init(&clock_lf_cfg, NULL, 0, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = ant_stack_static_config();
    APP_ERROR_CHECK(err_code);

    // Setup and open Channel_0 as a Bidirectional Master.
    app_channel_setup();

    #if DEBUG_CHANNEL_INCLUDED
    // Setup and open Debug Channel
    ad_init();
    ad_custom_command_callback_register(app_custom_debug_command_handler);
    #endif // DEBUG_CHANNEL_INCLUDED

    // Enter main loop
    for (;;)
    {
        err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
    }
}


/**
 *@}
 **/

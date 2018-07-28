/**
 * Copyright (c) 2012 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

 /**@file
 *
 * @defgroup XXXX
 * @{
 * @ingroup  YYYY
 *
 * @brief    ZZZZZ.
 */

#ifndef CLIENT_HANDLING_H__
#define CLIENT_HANDLING_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "device_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CLIENTS  DEVICE_MANAGER_MAX_CONNECTIONS  /**< Max number of clients. */

/**@brief Funtion for initializing the module.
 */
void client_handling_init(void);

/**@brief Funtion for returning the current number of clients.
 *
 * @return  The current number of clients.
 */
uint8_t client_handling_count(void);

/**@brief Funtion for creating a new client.
 *
 * @param[in] p_handle    Device Manager Handle. For link related events, this parameter
 *                        identifies the peer.
 *
 * @param[in] conn_handle Identifies link for which client is created.
 * @return NRF_SUCCESS on success, any other on failure.
 */
uint32_t client_handling_create(const dm_handle_t * p_handle, uint16_t conn_handle);

/**@brief Funtion for freeing up a client by setting its state to idle.
 *
 * @param[in] p_handle  Device Manager Handle. For link related events, this parameter
 *                      identifies the peer.
 *
 * @return NRF_SUCCESS on success, any other on failure.
 */
uint32_t client_handling_destroy(const dm_handle_t * p_handle);

/**@brief Funtion for handling client events.
 *
 * @param[in] p_ble_evt  Event to be handled.
 */
void client_handling_ble_evt_handler(ble_evt_t * p_ble_evt);


/**@brief Funtion for handling device manager events.
 *
 * @param[in] p_handle       Identifies device with which the event is associated.
 * @param[in] p_event        Event to be handled.
 * @param[in] event_result   Event result indicating whether a procedure was successful or not.
 */
ret_code_t client_handling_dm_event_handler(const dm_handle_t    * p_handle,
                                              const dm_event_t     * p_event,
                                              const ret_code_t     event_result);


#ifdef __cplusplus
}
#endif

#endif // CLIENT_HANDLING_H__

/** @} */

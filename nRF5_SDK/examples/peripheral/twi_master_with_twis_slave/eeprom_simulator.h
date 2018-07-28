/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
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

#ifndef EEPROM_SIMULATOR_H__
#define EEPROM_SIMULATOR_H__

#include "sdk_errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup twi_master_with_twis_slave_example
 * @defgroup eeprom_simulator EEPROM simulator
 *
 * This module simulates the behavior of TWI EEPROM.
 * There are no functions to access internal memory array.
 * Use TWI interface to read or write any data.
 *
 * @attention
 * During initialization EEPROM memory is filled by pattern that is
 * values from 127 downto 0.
 * @{
 */

ret_code_t eeprom_simulator_init(void);

/**
 * @brief Check if there was any error detected
 *
 * This function returns internal error flag.
 * Internal error flag is set if any error was detected during transmission.
 * To clear this flag use @ref eeprom_simulator_error_get
 * @retval true There is error detected.
 * @retval false There is no error detected.
 */
bool eeprom_simulator_error_check(void);

/**
 * @brief Get and clear transmission error
 *
 * Function returns transmission error data and clears internal error flag
 * @return Error that comes directly from @ref nrf_drv_twis_error_get
 */
uint32_t eeprom_simulator_error_get_and_clear(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // EEPROM_SIMULATOR_H__

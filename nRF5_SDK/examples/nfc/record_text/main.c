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

#include <stdint.h>
#include "nfc_t2t_lib.h"
#include "nfc_ndef_msg.h"
#include "nfc_text_rec.h"
#include "boards.h"
#include "app_error.h"
#include "hardfault.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define MAX_REC_COUNT      3     /**< Maximum records count. */

uint8_t m_ndef_msg_buf[256];

/**
 * @brief Callback function for handling NFC events.
 */
static void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
    (void)p_context;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            bsp_board_led_on(BSP_BOARD_LED_0);
            break;
        case NFC_T2T_EVENT_FIELD_OFF:
            bsp_board_led_off(BSP_BOARD_LED_0);
            break;
        default:
            break;
    }
}


/**
 * @brief Function for creating a record in English.
 */
static void en_record_add(nfc_ndef_msg_desc_t * p_ndef_msg_desc)
{
    /** @snippet [NFC text usage_1] */
    uint32_t             err_code;
    static const uint8_t en_payload[] =
                  {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    static const uint8_t en_code[] = {'e', 'n'};

    NFC_NDEF_TEXT_RECORD_DESC_DEF(en_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  en_payload,
                                  sizeof(en_payload));
   /** @snippet [NFC text usage_1] */

    err_code = nfc_ndef_msg_record_add(p_ndef_msg_desc,
                                       &NFC_NDEF_TEXT_RECORD_DESC(en_text_rec));
    APP_ERROR_CHECK(err_code);

}


/**
 * @brief Function for creating a record in Norwegian.
 */
static void no_record_add(nfc_ndef_msg_desc_t * p_ndef_msg_desc)
{
    uint32_t             err_code;
    static const uint8_t no_payload[] =
                          {'H', 'a', 'l', 'l', 'o', ' ', 'V', 'e', 'r', 'd', 'e', 'n', '!'};
    static const uint8_t no_code[] = {'N', 'O'};

    NFC_NDEF_TEXT_RECORD_DESC_DEF(no_text_rec,
                                  UTF_8,
                                  no_code,
                                  sizeof(no_code),
                                  no_payload,
                                  sizeof(no_payload));

    err_code = nfc_ndef_msg_record_add(p_ndef_msg_desc,
                                       &NFC_NDEF_TEXT_RECORD_DESC(no_text_rec));
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for creating a record in Polish.
 */
static void pl_record_add(nfc_ndef_msg_desc_t * p_ndef_msg_desc)
{
    uint32_t             err_code;
    static const uint8_t pl_payload[] =
                      {'W', 'i', 't', 'a', 'j', ' ', 0xc5, 0x9a, 'w', 'i', 'e', 'c', 'i', 'e', '!'};
    static const uint8_t pl_code[] = {'P', 'L'};

    NFC_NDEF_TEXT_RECORD_DESC_DEF(pl_text_rec,
                                  UTF_8,
                                  pl_code,
                                  sizeof(pl_code),
                                  pl_payload,
                                  sizeof(pl_payload));

    err_code = nfc_ndef_msg_record_add(p_ndef_msg_desc,
                                       &NFC_NDEF_TEXT_RECORD_DESC(pl_text_rec));
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for encoding the welcome message.
 */
static void welcome_msg_encode(uint8_t * p_buffer, uint32_t * p_len)
{
    NFC_NDEF_MSG_DEF(welcome_msg, MAX_REC_COUNT);

    en_record_add(&NFC_NDEF_MSG(welcome_msg));
    no_record_add(&NFC_NDEF_MSG(welcome_msg));
    pl_record_add(&NFC_NDEF_MSG(welcome_msg));

    /** @snippet [NFC text usage_2] */
    uint32_t err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(welcome_msg),
                                            p_buffer,
                                            p_len);
    APP_ERROR_CHECK(err_code);
    /** @snippet [NFC text usage_2] */
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t  len = sizeof(m_ndef_msg_buf);
    uint32_t  err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    /* Configure LED-pins as outputs */
    bsp_board_leds_init();

    /* Set up NFC */
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    /* Encode welcome message */
    welcome_msg_encode(m_ndef_msg_buf, &len);

    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    err_code = nfc_t2t_emulation_start();
    APP_ERROR_CHECK(err_code);

    while (1)
    {
        __WFE();
    }
}


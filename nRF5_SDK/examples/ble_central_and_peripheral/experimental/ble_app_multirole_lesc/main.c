/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
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

/**
 * @brief BLE multirole LESC example application main file.
 *
 * @detail This application demonstrates bonding with LE Secure Connections both as a peripheral and as a central.
 *
 * Led layout:
 * LED 1: Central side is scanning       LED 2: Central side is connected to a peripheral
 * LED 3: Peripheral side is advertising LED 4: Peripheral side is connected to a central
 *
 * @note: This application requires the use of an external ECC library for public key and shared secret calculation.
 *        Refer to the application's documentation for more details.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "peer_manager.h"
#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "app_uart.h"
#include "app_util.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_hrs.h"
#include "ble_hrs_c.h"
#include "ble_conn_state.h"
#include "fstorage.h"
#include "fds.h"
#include "nrf_crypto.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define LESC_DEBUG_MODE 0 /**< Set to 1 to use LESC debug keys, allows you to use a sniffer to inspect traffic. */
#define LESC_MITM_NC 1    /**< Use MITM (Numeric Comparison). */

#define APP_FEATURE_NOT_SUPPORTED   BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2          /**< Reply when unsupported features are requested. */

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE        GATT_MTU_SIZE_DEFAULT                         /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define CENTRAL_LINK_COUNT          1                                             /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT       1                                             /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

/* Central related. */

#define CENTRAL_SCANNING_LED        BSP_BOARD_LED_0
#define CENTRAL_CONNECTED_LED       BSP_BOARD_LED_1

#define APP_TIMER_PRESCALER         0                                             /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS        (2 + BSP_APP_TIMERS_NUMBER)                   /**< Maximum number of timers used by the application. */
#define APP_TIMER_OP_QUEUE_SIZE     2                                             /**< Size of timer operation queues. */

#define SEC_PARAMS_BOND             1                                             /**< Perform bonding. */
#if LESC_MITM_NC
#define SEC_PARAMS_MITM             1                                             /**< Man In The Middle protection required. */
#define SEC_PARAMS_IO_CAPABILITIES  BLE_GAP_IO_CAPS_DISPLAY_YESNO                 /**< Display Yes/No to force Numeric Comparison. */
#else
#define SEC_PARAMS_MITM             0                                             /**< Man In The Middle protection required. */
#define SEC_PARAMS_IO_CAPABILITIES  BLE_GAP_IO_CAPS_NONE                          /**< No I/O caps. */
#endif
#define SEC_PARAMS_LESC             1                                             /**< LE Secure Connections pairing required. */
#define SEC_PARAMS_KEYPRESS         0                                             /**< Keypress notifications not required. */
#define SEC_PARAMS_OOB              0                                             /**< Out Of Band data not available. */
#define SEC_PARAMS_MIN_KEY_SIZE     7                                             /**< Minimum encryption key size in octets. */
#define SEC_PARAMS_MAX_KEY_SIZE     16                                            /**< Maximum encryption key size in octets. */

#define SCAN_INTERVAL               0x00A0                                        /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW                 0x0050                                        /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_TIMEOUT                0

#define MIN_CONNECTION_INTERVAL     (uint16_t) MSEC_TO_UNITS(7.5, UNIT_1_25_MS)   /**< Determines minimum connection interval in milliseconds. */
#define MAX_CONNECTION_INTERVAL     (uint16_t) MSEC_TO_UNITS(30, UNIT_1_25_MS)    /**< Determines maximum connection interval in milliseconds. */
#define SLAVE_LATENCY               0                                             /**< Determines slave latency in terms of connection events. */
#define SUPERVISION_TIMEOUT         (uint16_t) MSEC_TO_UNITS(4000, UNIT_10_MS)    /**< Determines supervision time-out in units of 10 milliseconds. */

/**@brief Variable length data encapsulation in terms of length and pointer to data. */
typedef struct
{
    uint8_t     * p_data;    /**< Pointer to data. */
    uint16_t      data_len;  /**< Length of data. */
} data_t;

/**
 * @brief Parameters used when scanning.
 */
static const ble_gap_scan_params_t m_scan_params =
{
    .active   = 1,
    .interval = SCAN_INTERVAL,
    .window   = SCAN_WINDOW,
    .timeout  = SCAN_TIMEOUT,
    #if (NRF_SD_BLE_API_VERSION == 2)
        .selective   = 0,
        .p_whitelist = NULL,
    #endif
    #if (NRF_SD_BLE_API_VERSION == 3)
        .use_whitelist = 0,
    #endif
};

/**@brief Connection parameters requested for connection. */
static const ble_gap_conn_params_t m_connection_param =
{
    (uint16_t)MIN_CONNECTION_INTERVAL,
    (uint16_t)MAX_CONNECTION_INTERVAL,
    0,
    (uint16_t)SUPERVISION_TIMEOUT
};

static ble_hrs_c_t        m_ble_hrs_c;                                                    /**< Main structure used by the Heart rate client module. */
static uint16_t           m_conn_handle_hrs_c            = BLE_CONN_HANDLE_INVALID;       /**< Connection handle for the HRS central application */

static ble_db_discovery_t m_ble_db_discovery[CENTRAL_LINK_COUNT + PERIPHERAL_LINK_COUNT]; /**< list of DB structures used by the database discovery module. */

/* Peripheral related. */

#define PERIPHERAL_ADVERTISING_LED       BSP_BOARD_LED_2
#define PERIPHERAL_CONNECTED_LED         BSP_BOARD_LED_3

#define DEVICE_NAME                      "NordicLESCApp"                            /**< Name of device used for advertising. */
#define MANUFACTURER_NAME                "NordicSemiconductor"                      /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                 300                                        /**< The advertising interval (in units of 0.625 ms). This value corresponds to 187.5 ms. */
#define APP_ADV_TIMEOUT_IN_SECONDS       180                                        /**< The advertising timeout in units of seconds. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY    APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)/**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT     3                                          /**< Number of attempts before giving up the connection parameter negotiation. */

static ble_hrs_t    m_hrs;                                                          /**< Main structure for the Heart rate server module. */

#define BLE_GAP_LESC_P256_SK_LEN 32
/**@brief GAP LE Secure Connections P-256 Private Key. */
typedef struct
{
  uint8_t   sk[BLE_GAP_LESC_P256_SK_LEN];        /**< LE Secure Connections Elliptic Curve Diffie-Hellman P-256 Private Key in little-endian. */
} ble_gap_lesc_p256_sk_t;

static bool     m_numneric_match_requested = false;
static uint16_t m_num_comp_conn_handle;    
static char invalid_role_str[] = "INVALID_ROLE";
static char central_str[] = "CENTRAL";
static char periph_str[] = "PERIPHERAL";
static char *roles_str[BLE_GAP_ROLE_CENTRAL+1];


#if LESC_DEBUG_MODE
/**@brief Bluetooth SIG debug mode Private Key */
__ALIGN(4) static const ble_gap_lesc_p256_sk_t m_debug_lesc_sk = {{0xbd,0x1a,0x3c,0xcd,0xa6,0xb8,0x99,0x58,0x99,0xb7,0x40,0xeb,0x7b,0x60,0xff,0x4a, \
                                                 0x50,0x3f,0x10,0xd2,0xe3,0xb3,0xc9,0x74,0x38,0x5f,0xc5,0xa3,0xd4,0xf6,0x49,0x3f}};
#else
/**@brief Private Key: Replace with your own one, can be a random number or generated using a tool like OpenSSL */
__ALIGN(4) static const ble_gap_lesc_p256_sk_t m_lesc_sk = {{0x4a,0xa3,0x74,0x4b,0xa9,0xfc,0x2a,0x3a,0x8e,0x5a,0xd5,0x15,0xf8,0xe3,0x3f,0xb0, \
                                                 0x82,0x6c,0x98,0xf0,0x32,0x2a,0x51,0xee,0xb5,0x35,0x57,0x12,0xac,0x73,0x25,0x04}};
#endif
__ALIGN(4) static ble_gap_lesc_p256_pk_t m_lesc_pk;    /**< LESC ECC Public Key */
__ALIGN(4) static ble_gap_lesc_dhkey_t m_lesc_dhkey;   /**< LESC ECC DH Key*/

static nrf_crypto_key_t m_crypto_key_sk =
{
#if LESC_DEBUG_MODE
    .p_le_data = (uint8_t *) m_debug_lesc_sk.sk,
#else
    .p_le_data = (uint8_t *) m_lesc_sk.sk,
#endif
    .len = sizeof(m_lesc_sk.sk)
};
static nrf_crypto_key_t m_crypto_key_pk =
{
    .p_le_data = (uint8_t *) m_lesc_pk.pk,
    .len = sizeof(m_lesc_pk.pk)
};
static nrf_crypto_key_t m_crypto_key_dhkey =
{
    .p_le_data = (uint8_t *) m_lesc_dhkey.key,
    .len = sizeof(m_lesc_dhkey.key)
};

/* IAR has no support for the __ALIGN macro */
#if defined(__ICCARM__)
#pragma data_alignment = 1
#endif

/**@brief names which the central applications will scan for, and which will be advertised by the peripherals.
 *  if these are set to empty strings, the UUIDs defined below will be used
 */
static const char m_target_periph_name[] = "";

/**@brief UUIDs which the central applications will scan for if the name above is set to an empty string,
 * and which will be advertised by the peripherals.
 */
static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_HEART_RATE_SERVICE,         BLE_UUID_TYPE_BLE},
                                   {BLE_UUID_RUNNING_SPEED_AND_CADENCE,  BLE_UUID_TYPE_BLE}};


/**@brief Function to handle asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing ASSERT call.
 * @param[in] p_file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**
 * @brief Parses advertisement data, providing length and location of the field in case
 *        matching data is found.
 *
 * @param[in]  Type of data to be looked for in advertisement data.
 * @param[in]  Advertisement report length and pointer to report.
 * @param[out] If data type requested is found in the data report, type data length and
 *             pointer to data will be populated here.
 *
 * @retval NRF_SUCCESS if the data type is found in the report.
 * @retval NRF_ERROR_NOT_FOUND if the data type could not be found.
 */
static uint32_t adv_report_parse(uint8_t type, data_t * p_advdata, data_t * p_typedata)
{
    uint32_t  index = 0;
    uint8_t * p_data;

    p_data = p_advdata->p_data;

    while (index < p_advdata->data_len)
    {
        uint8_t field_length = p_data[index];
        uint8_t field_type   = p_data[index + 1];

        if (field_type == type)
        {
            p_typedata->p_data   = &p_data[index + 2];
            p_typedata->data_len = field_length - 1;
            return NRF_SUCCESS;
        }
        index += field_length + 1;
    }
    return NRF_ERROR_NOT_FOUND;
}


/**@brief Function for initiating scanning.
 */
static void scan_start(void)
{
    ret_code_t err_code;

    (void) sd_ble_gap_scan_stop();

    err_code = sd_ble_gap_scan_start(&m_scan_params);
    // It is okay to ignore this error since we are stopping the scan anyway.
    if (err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_CHECK(err_code);
    }
    NRF_LOG_INFO("Scanning\r\n");        
}


/**@brief Function for initiating advertising and scanning.
 */
static void adv_scan_start(void)
{
    ret_code_t err_code;
    uint32_t count;

    //check if there are no flash operations in progress
    err_code = fs_queued_op_count_get(&count);
    APP_ERROR_CHECK(err_code);

    if (count == 0)
    {
        // Start scanning for peripherals and initiate connection to devices which
        // advertise Heart Rate or Running speed and cadence UUIDs.
        scan_start();

        // Turn on the LED to signal scanning.
        bsp_board_led_on(CENTRAL_SCANNING_LED);

        // Start advertising.
        err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("Advertising\r\n");    
    }
}


/**@brief Function for handling File Data Storage events.
 *
 * @param[in] p_evt  Peer Manager event.
 * @param[in] cmd
 */
static void fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
    if (p_fds_evt->id == FDS_EVT_GC)
    {
        NRF_LOG_DEBUG("GC completed\r\n");
    }
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;
    uint16_t role = ble_conn_state_role(p_evt->conn_handle);

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_DEBUG("%s : PM_EVT_BONDED_PEER_CONNECTED: peer_id=%d\r\n",
                           nrf_log_push(roles_str[role]),
                           p_evt->peer_id);
        } break;

        case PM_EVT_CONN_SEC_START:
        {
            NRF_LOG_DEBUG("%s : PM_EVT_CONN_SEC_START: peer_id=%d\r\n",
                           nrf_log_push(roles_str[role]), 
                           p_evt->peer_id);
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_INFO("%s : PM_EVT_CONN_SEC_SUCCEEDED conn_handle: %d, Procedure: %d\r\n",
                           nrf_log_push(roles_str[role]),
                           p_evt->conn_handle,
                           p_evt->params.conn_sec_succeeded.procedure);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            NRF_LOG_DEBUG("%s: PM_EVT_CONN_SEC_FAILED: peer_id=%d, error=%d\r\n", 
                          nrf_log_push(roles_str[role]),
                          p_evt->peer_id, 
                          p_evt->params.conn_sec_failed.error);

        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
        } break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        {
            NRF_LOG_DEBUG("%s: PM_EVT_PEER_DATA_UPDATE_SUCCEEDED: peer_id=%d data_id=0x%x action=0x%x\r\n",
                           nrf_log_push(roles_str[role]), 
                           p_evt->peer_id, 
                           p_evt->params.peer_data_update_succeeded.data_id, 
                           p_evt->params.peer_data_update_succeeded.action);
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            adv_scan_start();
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}


/**@brief Handles events coming from the Heart Rate central module.
 */
static void hrs_c_evt_handler(ble_hrs_c_t * p_hrs_c, ble_hrs_c_evt_t * p_hrs_c_evt)
{
    switch (p_hrs_c_evt->evt_type)
    {
        case BLE_HRS_C_EVT_DISCOVERY_COMPLETE:
        {
            if (m_conn_handle_hrs_c == BLE_CONN_HANDLE_INVALID)
            {
                ret_code_t err_code;

                m_conn_handle_hrs_c = p_hrs_c_evt->conn_handle;
                NRF_LOG_INFO("CENTRAL: HRS discovered on conn_handle 0x%x\r\n",
                                m_conn_handle_hrs_c);

                err_code = ble_hrs_c_handles_assign(p_hrs_c,
                                                    m_conn_handle_hrs_c,
                                                    &p_hrs_c_evt->params.peer_db);
                APP_ERROR_CHECK(err_code);
                
                // Initiate bonding.
                NRF_LOG_INFO("CENTRAL: initiate security");
                err_code = pm_conn_secure(m_conn_handle_hrs_c, false);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }

                // Heart rate service discovered. Enable notification of Heart Rate Measurement.
                err_code = ble_hrs_c_hrm_notif_enable(p_hrs_c);
                APP_ERROR_CHECK(err_code);
            }
        } break; // BLE_HRS_C_EVT_DISCOVERY_COMPLETE

        case BLE_HRS_C_EVT_HRM_NOTIFICATION:
        {
            NRF_LOG_INFO("CENTRAL: Heart Rate = %d\r\n", p_hrs_c_evt->params.hrm.hr_value);
        } break; // BLE_HRS_C_EVT_HRM_NOTIFICATION

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for searching a given name in the advertisement packets.
 *
 * @details Use this function to parse received advertising data and to find a given
 * name in them either as 'complete_local_name' or as 'short_local_name'.
 *
 * @param[in]   p_adv_report   advertising data to parse.
 * @param[in]   name_to_find   name to search.
 * @return   true if the given name was found, false otherwise.
 */
static bool find_adv_name(const ble_gap_evt_adv_report_t *p_adv_report, const char * name_to_find)
{
    uint32_t err_code;
    data_t   adv_data;
    data_t   dev_name;

    // Initialize advertisement report for parsing
    adv_data.p_data     = (uint8_t *)p_adv_report->data;
    adv_data.data_len   = p_adv_report->dlen;

    //search for advertising names
    err_code = adv_report_parse(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
                                &adv_data,
                                &dev_name);
    if (err_code == NRF_SUCCESS)
    {
        if (memcmp(name_to_find, dev_name.p_data, dev_name.data_len )== 0)
        {
            return true;
        }
    }
    else
    {
        // Look for the short local name if it was not found as complete
        err_code = adv_report_parse(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME,
                                    &adv_data,
                                    &dev_name);
        if (err_code != NRF_SUCCESS)
        {
            return false;
        }
        if (memcmp(m_target_periph_name, dev_name.p_data, dev_name.data_len )== 0)
        {
            return true;
        }
    }
    return false;
}


/**@brief Function for searching a UUID in the advertisement packets.
 *
 * @details Use this function to parse received advertising data and to find a given
 * UUID in them.
 *
 * @param[in]   p_adv_report   advertising data to parse.
 * @param[in]   uuid_to_find   UUIID to search.
 * @return   true if the given UUID was found, false otherwise.
 */
static bool find_adv_uuid(const ble_gap_evt_adv_report_t *p_adv_report, const uint16_t uuid_to_find)
{
    uint32_t err_code;
    data_t   adv_data;
    data_t   type_data;

    // Initialize advertisement report for parsing.
    adv_data.p_data     = (uint8_t *)p_adv_report->data;
    adv_data.data_len   = p_adv_report->dlen;

    err_code = adv_report_parse(BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE,
                                &adv_data,
                                &type_data);

    if (err_code != NRF_SUCCESS)
    {
        // Look for the services in 'complete' if it was not found in 'more available'.
        err_code = adv_report_parse(BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE,
                                    &adv_data,
                                    &type_data);

        if (err_code != NRF_SUCCESS)
        {
            // If we can't parse the data, then exit.
            return false;
        }
    }

    // Verify if any UUID match the given UUID.
    for (uint32_t u_index = 0; u_index < (type_data.data_len / sizeof(uint16_t)); u_index++)
    {
        uint16_t    extracted_uuid;

        extracted_uuid = uint16_decode(&type_data.p_data[u_index * sizeof(uint16_t)]);

        if (extracted_uuid == uuid_to_find)
        {
            return true;
        }
    }
    return false;
}

/**@brief Function for handling BLE Stack events common to both the central and peripheral roles.
 * @param[in] conn_handle Connection Handle.
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(uint16_t conn_handle, ble_evt_t * p_ble_evt)
{
    ret_code_t err_code;
    char passkey[BLE_GAP_PASSKEY_LEN + 1];
    uint16_t role = ble_conn_state_role(conn_handle);

    static nrf_crypto_key_t peer_pk;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            NRF_LOG_INFO("%s: BLE_GAP_EVT_SEC_PARAMS_REQUEST\r\n", nrf_log_push(roles_str[role]));
            break;
        case BLE_GAP_EVT_PASSKEY_DISPLAY:
            memcpy(passkey, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, BLE_GAP_PASSKEY_LEN);
            passkey[BLE_GAP_PASSKEY_LEN] = 0x00;
            NRF_LOG_INFO("%s: BLE_GAP_EVT_PASSKEY_DISPLAY: passkey=%s match_req=%d\r\n", 
                         nrf_log_push(roles_str[role]),
                         nrf_log_push(passkey), 
                         p_ble_evt->evt.gap_evt.params.passkey_display.match_request);
            
            if (p_ble_evt->evt.gap_evt.params.passkey_display.match_request)
            {
                NRF_LOG_INFO("Press Button 1 to confirm, Button 2 to reject\r\n");
                m_num_comp_conn_handle = conn_handle;
                m_numneric_match_requested = true;
            }
            break;
        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            NRF_LOG_INFO("%s: BLE_GAP_EVT_AUTH_KEY_REQUEST\r\n", nrf_log_push(roles_str[role]));
            break;
        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            NRF_LOG_INFO("%s: BLE_GAP_EVT_LESC_DHKEY_REQUEST\r\n", nrf_log_push(roles_str[role]));
            peer_pk.p_le_data = &p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.p_pk_peer->pk[0];
            peer_pk.len = BLE_GAP_LESC_P256_PK_LEN;
            err_code = nrf_crypto_shared_secret_compute(NRF_CRYPTO_CURVE_SECP256R1, &m_crypto_key_sk, &peer_pk, &m_crypto_key_dhkey);
            APP_ERROR_CHECK(err_code);
            err_code = sd_ble_gap_lesc_dhkey_reply(conn_handle, &m_lesc_dhkey);
            APP_ERROR_CHECK(err_code);
            break;
         case BLE_GAP_EVT_AUTH_STATUS:
             NRF_LOG_INFO("%s: BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x\r\n",
                          nrf_log_push(roles_str[role]), 
                          p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                          p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                          p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
            break;
        default:
            // No implementation needed.
            break;
    }
}



/**@brief Function for handling BLE Stack events concerning central applications.
 *
 * @details This function keeps the connection handles of central applications up-to-date. It
 * parses scanning reports, initiating a connection attempt to peripherals when a target UUID
 * is found, and manages connection parameter update requests. Additionally, it updates the status
 * of LEDs used to report central applications activity.
 *
 * @note        Since this function updates connection handles, @ref BLE_GAP_EVT_DISCONNECTED events
 *              should be dispatched to the target application before invoking this function.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_central_evt(const ble_evt_t * const p_ble_evt)
{
    const ble_gap_evt_t   * const p_gap_evt = &p_ble_evt->evt.gap_evt;
    ret_code_t                    err_code;

    switch (p_ble_evt->header.evt_id)
    {
        /** Upon connection, check which peripheral has connected (HR or RSC), initiate DB
         *  discovery, update LEDs status and resume scanning if necessary. */
        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("CENTRAL: connected\r\n");
            /** If no Heart Rate sensor is currently connected, try to find them on this peripheral*/
            if (m_conn_handle_hrs_c == BLE_CONN_HANDLE_INVALID)
            {
                NRF_LOG_INFO("CENTRAL: try to find HRS on conn_handle 0x%x\r\n", p_gap_evt->conn_handle);

                APP_ERROR_CHECK_BOOL(p_gap_evt->conn_handle < CENTRAL_LINK_COUNT + PERIPHERAL_LINK_COUNT);
                err_code = ble_db_discovery_start(&m_ble_db_discovery[p_gap_evt->conn_handle], p_gap_evt->conn_handle);
                APP_ERROR_CHECK(err_code);
            }
            /** Update LEDs status, and check if we should be looking for more
             *  peripherals to connect to. */
            bsp_board_led_on(CENTRAL_CONNECTED_LED);
            if (ble_conn_state_n_centrals() == CENTRAL_LINK_COUNT)
            {
                bsp_board_led_off(CENTRAL_SCANNING_LED);
            }
            else
            {
                // Resume scanning.
                bsp_board_led_on(CENTRAL_SCANNING_LED);
                scan_start();
            }
            
        } break; // BLE_GAP_EVT_CONNECTED

        /** Upon disconnection, reset the connection handle of the peer which disconnected, update
         * the LEDs status and start scanning again. */
        case BLE_GAP_EVT_DISCONNECTED:
        {
            uint8_t n_centrals;
            NRF_LOG_INFO("CENTRAL: disconnected (reason: %d)\r\n",
                       p_gap_evt->params.disconnected.reason);
            if (p_gap_evt->conn_handle == m_conn_handle_hrs_c)
            {
                m_conn_handle_hrs_c = BLE_CONN_HANDLE_INVALID;
            }
            
            if (m_conn_handle_hrs_c == BLE_CONN_HANDLE_INVALID)
            {
                // Start scanning
                scan_start();

                // Update LEDs status.
                bsp_board_led_on(CENTRAL_SCANNING_LED);
            }
            n_centrals = ble_conn_state_n_centrals();

            if (n_centrals == 0)
            {
                bsp_board_led_off(CENTRAL_CONNECTED_LED);
            }
        } break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_ADV_REPORT:
        {
            if (strlen(m_target_periph_name) != 0)
            {
                if (find_adv_name(&p_gap_evt->params.adv_report, m_target_periph_name))
                {
                    // Initiate connection.
                    NRF_LOG_INFO("central connecting ...\r\n");
                    err_code = sd_ble_gap_connect(&p_gap_evt->params.adv_report.peer_addr,
                                                  &m_scan_params,
                                                  &m_connection_param);
                    if (err_code != NRF_SUCCESS)
                    {
                        NRF_LOG_DEBUG("Connection Request Failed, reason %d\r\n", err_code);
                    }
                }
            }
            else
            {
               /** We do not want to connect to two peripherals offering the same service, so when
                *  a UUID is matched, we check that we are not already connected to a peer which
                *  offers the same service. */
                if (find_adv_uuid(&p_gap_evt->params.adv_report, BLE_UUID_HEART_RATE_SERVICE)&&
                     (m_conn_handle_hrs_c == BLE_CONN_HANDLE_INVALID))
                {
                    // Initiate connection.
                    NRF_LOG_INFO("CENTRAL: connecting ...\r\n");
                    err_code = sd_ble_gap_connect(&p_gap_evt->params.adv_report.peer_addr,
                                                  &m_scan_params,
                                                  &m_connection_param);
                    if (err_code != NRF_SUCCESS)
                    {
                        NRF_LOG_DEBUG("CENTRAL: Connection Request Failed, reason %d\r\n", err_code);
                    }
                }
            }
        } break; // BLE_GAP_ADV_REPORT

        case BLE_GAP_EVT_TIMEOUT:
        {
            // We have not specified a timeout for scanning, so only connection attemps can timeout.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_DEBUG("CENTRAL: Connection Request timed out.\r\n");
            }
        } break; // BLE_GAP_EVT_TIMEOUT

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            // Accept parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                        &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        } break; // BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("CENTRAL: GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("CENTRAL: GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling BLE Stack events involving peripheral applications. Manages the
 * LEDs used to report the status of the peripheral applications.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_peripheral_evt(ble_evt_t * p_ble_evt)
{
    ret_code_t err_code;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("PERIPHERAL: connected\r\n");
            bsp_board_led_off(PERIPHERAL_ADVERTISING_LED);
            bsp_board_led_on(PERIPHERAL_CONNECTED_LED);
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("PERIPHERAL: disconnected, reason %d\r\n", 
                          p_ble_evt->evt.gap_evt.params.disconnected.reason);
            bsp_board_led_off(PERIPHERAL_CONNECTED_LED);
        break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("PERIPHERAL: GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("PERIPHERAL: GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gap_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling advertising events.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            bsp_board_led_on(PERIPHERAL_ADVERTISING_LED);
            break;

        case BLE_ADV_EVT_IDLE:
        {
            ret_code_t err_code;
            err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack event has
 * been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    uint16_t conn_handle;
    uint16_t role;

    /** The Connection state module has to be fed BLE events in order to function correctly
     * Remember to call ble_conn_state_on_ble_evt before calling any ble_conns_state_* functions. */
    ble_conn_state_on_ble_evt(p_ble_evt);

    pm_on_ble_evt(p_ble_evt);

    // The connection handle should really be retrievable for any event type.
    conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    role        = ble_conn_state_role(conn_handle);

    on_ble_evt(conn_handle, p_ble_evt);

    // Based on the role this device plays in the connection, dispatch to the right applications.
    if (role == BLE_GAP_ROLE_PERIPH)
    {
        // Manages peripheral LEDs.
        on_ble_peripheral_evt(p_ble_evt);

        ble_advertising_on_ble_evt(p_ble_evt);
        ble_conn_params_on_ble_evt(p_ble_evt);

        // Dispatch to peripheral applications.
        ble_hrs_on_ble_evt (&m_hrs, p_ble_evt);
    }
    else if ((role == BLE_GAP_ROLE_CENTRAL) || (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_REPORT))
    {
        /** on_ble_central_evt will update the connection handles, so we want to execute it
         * after dispatching to the central applications upon disconnection. */
        if (p_ble_evt->header.evt_id != BLE_GAP_EVT_DISCONNECTED)
        {
            on_ble_central_evt(p_ble_evt);
        }

        if (conn_handle < CENTRAL_LINK_COUNT + PERIPHERAL_LINK_COUNT)
        {
            ble_db_discovery_on_ble_evt(&m_ble_db_discovery[conn_handle], p_ble_evt);
        }
        ble_hrs_c_on_ble_evt(&m_ble_hrs_c, p_ble_evt);

        // If the peer disconnected, we update the connection handles last.
        if (p_ble_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED)
        {
            on_ble_central_evt(p_ble_evt);
        }
    }
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    ble_advertising_on_sys_evt(sys_evt);
    /** Dispatch the system event to the Flash Storage module, where it will be
     *  dispatched to the Flash Data Storage module and from there to the Peer Manager. */
    fs_sys_event_handler(sys_evt);
}


/**@brief Heart rate collector initialization.
 */
static void hrs_c_init(void)
{
    uint32_t         err_code;
    ble_hrs_c_init_t hrs_c_init_obj;

    hrs_c_init_obj.evt_handler = hrs_c_evt_handler;

    err_code = ble_hrs_c_init(&m_ble_hrs_c, &hrs_c_init_obj);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupts.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    // Initialize the SoftDevice handler module.
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for System events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
static void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_params;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        (void) pm_peers_delete();
    }

    memset(&sec_params, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_params.bond           = SEC_PARAMS_BOND;
    sec_params.mitm           = SEC_PARAMS_MITM;
    sec_params.lesc           = SEC_PARAMS_LESC;
    sec_params.keypress       = SEC_PARAMS_KEYPRESS;
    sec_params.io_caps        = SEC_PARAMS_IO_CAPABILITIES;
    sec_params.oob            = SEC_PARAMS_OOB;
    sec_params.min_key_size   = SEC_PARAMS_MIN_KEY_SIZE;
    sec_params.max_key_size   = SEC_PARAMS_MAX_KEY_SIZE;
    sec_params.kdist_own.enc  = 1;
    sec_params.kdist_own.id   = 1;
    sec_params.kdist_peer.enc = 1;
    sec_params.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_params);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = fds_register(fds_evt_handler);
    APP_ERROR_CHECK(err_code);

    nrf_crypto_init();

    err_code = nrf_crypto_public_key_compute(NRF_CRYPTO_CURVE_SECP256R1, &m_crypto_key_sk, &m_crypto_key_pk);
    APP_ERROR_CHECK(err_code);

    /* Set the public key */
    err_code = pm_lesc_public_key_set(&m_lesc_pk);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
static void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;
    switch (event)
    {
        case BSP_EVENT_KEY_0:
            NRF_LOG_DEBUG("BSP_EVENT_KEY_0\r\n");
            err_code = ble_hrs_heart_rate_measurement_send(&m_hrs, 87);
            if ((err_code != NRF_SUCCESS) &&
                (err_code != NRF_ERROR_INVALID_STATE) &&
                (err_code != BLE_ERROR_NO_TX_PACKETS) &&
                (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
                )
            {
                APP_ERROR_HANDLER(err_code);
            }
            if (m_numneric_match_requested)
            {
                NRF_LOG_INFO("Numeric Match\r\n");
                err_code = sd_ble_gap_auth_key_reply(m_num_comp_conn_handle, BLE_GAP_AUTH_KEY_TYPE_PASSKEY, NULL);
                APP_ERROR_CHECK(err_code);
                m_numneric_match_requested = false;
            }
            break;

      case BSP_EVENT_KEY_1:
            NRF_LOG_DEBUG("BSP_EVENT_KEY_1\r\n");
            if (m_numneric_match_requested)
            {
                NRF_LOG_INFO("Numeric REJECT\r\n");
                err_code = sd_ble_gap_auth_key_reply(m_num_comp_conn_handle, BLE_GAP_AUTH_KEY_TYPE_NONE, NULL);//Reject
                APP_ERROR_CHECK(err_code);
                m_numneric_match_requested = false;
            }
            break;

        default:
            break;
    }
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to
 *                            wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    ret_code_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                                 bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONNECTION_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONNECTION_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = SUPERVISION_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_CONN_HANDLE_INVALID; // Start upon connection.
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;  // Ignore events.
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_hrs_on_db_disc_evt(&m_ble_hrs_c, p_evt);
}


/**
 * @brief Database discovery initialization.
 */
static void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void services_init(void)
{
    uint32_t        err_code;
    ble_hrs_init_t  hrs_init;
    uint8_t         body_sensor_location;

    // Initialize the Heart Rate Service.
    body_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_FINGER;

    memset(&hrs_init, 0, sizeof(hrs_init));

    hrs_init.evt_handler                 = NULL;
    hrs_init.is_sensor_contact_supported = true;
    hrs_init.p_body_sensor_location      = &body_sensor_location;

    // Require LESC with MITM (Numeric Comparison)
    BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(&hrs_init.hrs_hrm_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_hrm_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_hrm_attr_md.write_perm);

    // Require LESC with MITM (Numeric Comparison)
    BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(&hrs_init.hrs_bsl_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_bsl_attr_md.write_perm);

    err_code = ble_hrs_init(&m_hrs, &hrs_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advdata_t          advdata;
    ble_adv_modes_config_t options;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);

}


/** @brief Function to sleep until a BLE event is received by the application.
 */
static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


int main(void)
{
    ret_code_t err_code;
    bool       erase_bonds;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    roles_str[BLE_GAP_ROLE_INVALID] = invalid_role_str;
    roles_str[BLE_GAP_ROLE_PERIPH] = periph_str;
    roles_str[BLE_GAP_ROLE_CENTRAL] = central_str;
    
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);
    buttons_leds_init(&erase_bonds);

    if (erase_bonds)
    {
        NRF_LOG_INFO("Bonds erased!\r\n");
    }
    ble_stack_init();

    peer_manager_init(erase_bonds);

    db_discovery_init();
    hrs_c_init();

    NRF_LOG_INFO("LE Secure Connections Example\r\n");
    
    gap_params_init();
    conn_params_init();
    services_init();
    advertising_init();

    adv_scan_start();

    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            // Wait for BLE events.
            power_manage();
        }
    }
}

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

/** @file
 *
 * @defgroup ble_sdk_apple_notification_main main.c
 * @{
 * @ingroup ble_sdk_app_apple_notification
 * @brief Apple Notification Client Sample Application main file. Disclaimer: 
 * This client implementation of the Apple Notification Center Service can and 
 * will be changed at any time by Nordic Semiconductor ASA.
 *
 * Server implementations such as the ones found in iOS can be changed at any 
 * time by Apple and may cause this client implementation to stop working.
 *
 * This file contains the source code for a sample application using the Apple 
 * Notification Center Service Client.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "nrf_ble_ancs_c.h"
#include "ble_db_discovery.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble_hci.h"
#include "ble_gap.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "peer_manager.h"
#include "app_timer.h"
#include "nrf_soc.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "softdevice_handler.h"
#include "fds.h"
#include "fstorage.h"
#include "nrf_delay.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "ble_conn_state.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


#if BUTTONS_NUMBER < 2
#error "Not enough resources on board"
#endif

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE           GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define CENTRAL_LINK_COUNT             0                                           /**< The number of central links used by the application. When changing this number remember to adjust the RAM settings. */
#define PERIPHERAL_LINK_COUNT          1                                           /**< The number of peripheral links used by the application. When changing this number remember to adjust the RAM settings. */
#define VENDOR_SPECIFIC_UUID_COUNT     10                                          /**< The number of vendor specific UUIDs used by this example. */

#define ATTR_DATA_SIZE                 BLE_ANCS_ATTR_DATA_MAX                      /**< Allocated size for attribute data. */

#define DISPLAY_MESSAGE_BUTTON_ID      1                                           /**< Button used to request notification attributes. */

#define DEVICE_NAME                    "ANCS"                                      /**< Name of the device. Will be included in the advertising data. */
#define APP_ADV_FAST_INTERVAL          40                                          /**< The advertising interval (in units of 0.625 ms). The default value corresponds to 25 ms. */
#define APP_ADV_SLOW_INTERVAL          3200                                        /**< Slow advertising interval (in units of 0.625 ms). The default value corresponds to 2 seconds. */
#define APP_ADV_FAST_TIMEOUT           180                                         /**< The advertising time-out in units of seconds. */
#define APP_ADV_SLOW_TIMEOUT           180                                         /**< The advertising time-out in units of seconds. */
#define ADV_INTERVAL_FAST_PERIOD       30                                          /**< The duration of the fast advertising period (in seconds). */

#define APP_TIMER_PRESCALER            0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE        5                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL              MSEC_TO_UNITS(500, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL              MSEC_TO_UNITS(1000, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                  0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT               MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating an event (connect or start of notification) to the first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT   3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define MESSAGE_BUFFER_SIZE            18                                          /**< Size of buffer holding optional messages in notifications. */

#define SECURITY_REQUEST_DELAY         APP_TIMER_TICKS(1500, APP_TIMER_PRESCALER)  /**< Delay after connection until security request is sent, if necessary (ticks). */

#define SEC_PARAM_BOND                 1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                 0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                 0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS             0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES      BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                  0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE         7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE         16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                      0xDEADBEEF                                  /**< Value used as error code on stack dump. Can be used to identify stack location on stack unwind. */

#define SCHED_MAX_EVENT_DATA_SIZE      MAX(APP_TIMER_SCHED_EVT_SIZE, \
                                           BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE               20                                          /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE               10                                          /**< Maximum number of events in the scheduler queue. */
#endif

static pm_peer_id_t   m_whitelist_peers[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];  /**< List of peers currently in the whitelist. */
static uint32_t       m_whitelist_peer_cnt;                                 /**< Number of peers currently in the whitelist. */
static bool           m_is_wl_changed;                                      /**< Indicates if the whitelist has been changed since last time it has been updated in the Peer Manager. */


/**@brief String literals for the iOS notification categories. used then printing to UART. */
static const char * lit_catid[BLE_ANCS_NB_OF_CATEGORY_ID] =
{
    "Other",
    "Incoming Call",
    "Missed Call",
    "Voice Mail",
    "Social",
    "Schedule",
    "Email",
    "News",
    "Health And Fitness",
    "Business And Finance",
    "Location",
    "Entertainment"
};


/**@brief String literals for the iOS notification event types. Used then printing to UART. */
static const char * lit_eventid[BLE_ANCS_NB_OF_EVT_ID] =
{
    "Added",
    "Modified",
    "Removed"
};


/**@brief String literals for the iOS notification attribute types. Used when printing to UART. */
static const char * lit_attrid[BLE_ANCS_NB_OF_NOTIF_ATTR] =
{
    "App Identifier",
    "Title",
    "Subtitle",
    "Message",
    "Message Size",
    "Date",
    "Positive Action Label",
    "Negative Action Label"
};


/**@brief String literals for the iOS notification attribute types. Used When printing to UART. */
static const char * lit_appid[BLE_ANCS_NB_OF_APP_ATTR] =
{
    "Display Name"
};


static ble_ancs_c_t       m_ancs_c;                                    /**< Structure used to identify the Apple Notification Service Client. */
static ble_db_discovery_t m_ble_db_discovery;                          /**< Structure used to identify the DB Discovery module. */
static pm_peer_id_t       m_peer_id;                                   /**< Device reference handle to the current bonded central. */
static uint16_t           m_cur_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */

APP_TIMER_DEF(m_sec_req_timer_id);                                     /**< Security request timer. The timer lets us start pairing request if one does not arrive from the Central. */

static ble_ancs_c_evt_notif_t m_notification_latest;                   /**< Local copy to keep track of the newest arriving notifications. */
static ble_ancs_c_attr_t      m_notif_attr_latest;                     /**< Local copy of the newest notification attribute. */
static ble_ancs_c_attr_t      m_notif_attr_app_id_latest;              /**< Local copy of the newest app attribute. */

static uint8_t m_attr_appid[ATTR_DATA_SIZE];                           /**< Buffer to store attribute data. */

static uint8_t m_attr_title[ATTR_DATA_SIZE];                           /**< Buffer to store attribute data. */
static uint8_t m_attr_subtitle[ATTR_DATA_SIZE];                        /**< Buffer to store attribute data. */
static uint8_t m_attr_message[ATTR_DATA_SIZE];                         /**< Buffer to store attribute data. */
static uint8_t m_attr_message_size[ATTR_DATA_SIZE];                    /**< Buffer to store attribute data. */
static uint8_t m_attr_date[ATTR_DATA_SIZE];                            /**< Buffer to store attribute data. */
static uint8_t m_attr_posaction[ATTR_DATA_SIZE];                       /**< Buffer to store attribute data. */
static uint8_t m_attr_negaction[ATTR_DATA_SIZE];                       /**< Buffer to store attribute data. */

static uint8_t m_attr_disp_name[ATTR_DATA_SIZE];                       /**< Buffer to store attribute data. */

/**@brief Callback function for handling asserts in the SoftDevice.
 *
 * @details This function is called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. 
 *          You must analyze how your product should react to asserts.
 * @warning On assert from the SoftDevice, the system can recover only on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Fetch the list of peer manager peer IDs.
 *
 * @param[inout] p_peers   The buffer where to store the list of peer IDs.
 * @param[inout] p_size    In: The size of the @p p_peers buffer.
 *                         Out: The number of peers copied in the buffer.
 */
static void peer_list_get(pm_peer_id_t * p_peers, uint32_t * p_size)
{
    pm_peer_id_t peer_id;
    uint32_t     peers_to_copy;

    peers_to_copy = (*p_size < BLE_GAP_WHITELIST_ADDR_MAX_COUNT) ?
                     *p_size : BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

    peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    *p_size = 0;

    while ((peer_id != PM_PEER_ID_INVALID) && (peers_to_copy--))
    {
        p_peers[(*p_size)++] = peer_id;
        peer_id = pm_next_peer_id_get(peer_id);
    }
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    ret_code_t ret;

    memset(m_whitelist_peers, PM_PEER_ID_INVALID, sizeof(m_whitelist_peers));
    m_whitelist_peer_cnt = (sizeof(m_whitelist_peers) / sizeof(pm_peer_id_t));

    peer_list_get(m_whitelist_peers, &m_whitelist_peer_cnt);

    ret = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
    APP_ERROR_CHECK(ret);

    // Setup the device identies list.
    // Some SoftDevices do not support this feature.
    ret = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);
    if (ret != NRF_ERROR_NOT_SUPPORTED)
    {
        APP_ERROR_CHECK(ret);
    }

    m_is_wl_changed = false;

    ret = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t ret;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_DEBUG("Connected to previously bonded device\r\n");
            m_peer_id = p_evt->peer_id;
        } break; // PM_EVT_BONDED_PEER_CONNECTED

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_INFO("Connection secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);

            m_peer_id = p_evt->peer_id;


            // Note: You should check on what kind of white list policy your application should use.
            if (p_evt->params.conn_sec_succeeded.procedure == PM_LINK_SECURED_PROCEDURE_BONDING)
            {
                NRF_LOG_DEBUG("New Bond, add the peer to the whitelist if possible\r\n");
                NRF_LOG_DEBUG("\tm_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d\r\n",
                               m_whitelist_peer_cnt + 1,
                               BLE_GAP_WHITELIST_ADDR_MAX_COUNT);

                if (m_whitelist_peer_cnt < BLE_GAP_WHITELIST_ADDR_MAX_COUNT)
                {
                    // Bonded to a new peer, add it to the whitelist.
                    m_whitelist_peers[m_whitelist_peer_cnt++] = m_peer_id;
                    m_is_wl_changed = true;
                }
            }
            ret  = ble_db_discovery_start(&m_ble_db_discovery, p_evt->conn_handle);
            APP_ERROR_CHECK(ret);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
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
            ret = fds_gc();
            if (ret == FDS_ERR_BUSY || ret == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(ret);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            advertising_start();
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

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}


/**@brief Function for handling the security request timer time-out.
 *
 * @details This function is called each time the security request timer expires.
 *
 * @param[in] p_context  Pointer used for passing context information from the
 *                       app_start_timer() call to the time-out handler.
 */
static void sec_req_timeout_handler(void * p_context)
{
    ret_code_t           ret;
    pm_conn_sec_status_t status;

    if (m_cur_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        ret = pm_conn_sec_status_get(m_cur_conn_handle, &status);
        APP_ERROR_CHECK(ret);

        // If the link is still not secured by the peer, initiate security procedure.
        if (!status.encrypted)
        {
            ret = pm_conn_secure(m_cur_conn_handle, false);
            if (ret != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(ret);
            }
        }
    }
}


/**@brief Function for setting up GATTC notifications from the Notification Provider.
 *
 * @details This function is called when a successful connection has been established.
 */
static void apple_notification_setup(void)
{
    ret_code_t ret;

    nrf_delay_ms(100); // Delay because we cannot add a CCCD to close to starting encryption. iOS specific.

    ret = ble_ancs_c_notif_source_notif_enable(&m_ancs_c);
    APP_ERROR_CHECK(ret);

    ret = ble_ancs_c_data_source_notif_enable(&m_ancs_c);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEBUG("Notifications Enabled.\r\n");
}


/**@brief Function for printing an iOS notification.
 *
 * @param[in] p_notif  Pointer to the iOS notification.
 */
static void notif_print(ble_ancs_c_evt_notif_t * p_notif)
{
    NRF_LOG_INFO("\r\nNotification\r\n");
    NRF_LOG_INFO("Event:       %s\r\n", (uint32_t)lit_eventid[p_notif->evt_id]);
    NRF_LOG_INFO("Category ID: %s\r\n", (uint32_t)lit_catid[p_notif->category_id]);
    NRF_LOG_INFO("Category Cnt:%u\r\n", (unsigned int) p_notif->category_count);
    NRF_LOG_INFO("UID:         %u\r\n", (unsigned int) p_notif->notif_uid);

    NRF_LOG_INFO("Flags:\r\n");
    if(p_notif->evt_flags.silent == 1)
    {
        NRF_LOG_INFO(" Silent\r\n");
    }
    if(p_notif->evt_flags.important == 1)
    {
        NRF_LOG_INFO(" Important\r\n");
    }
    if(p_notif->evt_flags.pre_existing == 1)
    {
        NRF_LOG_INFO(" Pre-existing\r\n");
    }
    if(p_notif->evt_flags.positive_action == 1)
    {
        NRF_LOG_INFO(" Positive Action\r\n");
    }
    if(p_notif->evt_flags.negative_action == 1)
    {
        NRF_LOG_INFO(" Negative Action\r\n");
    }
}


/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] p_attr Pointer to an iOS notification attribute.
 */
static void notif_attr_print(ble_ancs_c_attr_t * p_attr)
{
    if (p_attr->attr_len != 0)
    {
        NRF_LOG_INFO("%s: %s\r\n", (uint32_t)lit_attrid[p_attr->attr_id], nrf_log_push((char *)p_attr->p_attr_data));
    }
    else if (p_attr->attr_len == 0)
    {
        NRF_LOG_INFO("%s: (N/A)\r\n", (uint32_t)lit_attrid[p_attr->attr_id]);
    }
}


/**@brief Function for printing iOS notification attribute data.
 *
 * @param[in] p_attr Pointer to an iOS App attribute.
 */
static void app_attr_print(ble_ancs_c_attr_t * p_attr)
{
    if (p_attr->attr_len != 0)
    {
        NRF_LOG_INFO("%s: %s\r\n", (uint32_t)lit_appid[p_attr->attr_id], (uint32_t)p_attr->p_attr_data);
    }
    else if (p_attr->attr_len == 0)
    {
        NRF_LOG_INFO("%s: (N/A)\r\n", (uint32_t) lit_appid[p_attr->attr_id]);
    }
}


/**@brief Function for printing out errors that originated from the Notification Provider (iOS).
 *
 * @param[in] err_code_np Error code received from NP.
 */
static void err_code_print(uint16_t err_code_np)
{
    switch (err_code_np)
    {
        case BLE_ANCS_NP_UNKNOWN_COMMAND:
            NRF_LOG_INFO("Error: Command ID was not recognized by the Notification Provider. \r\n");
            break;

        case BLE_ANCS_NP_INVALID_COMMAND:
            NRF_LOG_INFO("Error: Command failed to be parsed on the Notification Provider. \r\n");
            break;

        case BLE_ANCS_NP_INVALID_PARAMETER:
            NRF_LOG_INFO("Error: Parameter does not refer to an existing object on the Notification Provider. \r\n");
            break;

        case BLE_ANCS_NP_ACTION_FAILED:
            NRF_LOG_INFO("Error: Perform Notification Action Failed on the Notification Provider. \r\n");
            break;

        default:
            break;
    }
}
/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{
    ret_code_t ret;

    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);

    // Create security request timer.
    ret = app_timer_create(&m_sec_req_timer_id,
                           APP_TIMER_MODE_SINGLE_SHOT,
                           sec_req_timeout_handler);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling the Apple Notification Service client.
 *
 * @details This function is called for all events in the Apple Notification client that
 *          are passed to the application.
 *
 * @param[in] p_evt  Event received from the Apple Notification Service client.
 */
static void on_ancs_c_evt(ble_ancs_c_evt_t * p_evt)
{
    ret_code_t ret = NRF_SUCCESS;

    switch (p_evt->evt_type)
    {
        case BLE_ANCS_C_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_DEBUG("Apple Notification Center Service discovered on the server.\r\n");
            ret = nrf_ble_ancs_c_handles_assign(&m_ancs_c, p_evt->conn_handle, &p_evt->service);
            APP_ERROR_CHECK(ret);
            apple_notification_setup();
            break;

        case BLE_ANCS_C_EVT_NOTIF:
            m_notification_latest = p_evt->notif;
            notif_print(&m_notification_latest);
            break;

        case BLE_ANCS_C_EVT_NOTIF_ATTRIBUTE:
            m_notif_attr_latest = p_evt->attr;
            notif_attr_print(&m_notif_attr_latest);
            if(p_evt->attr.attr_id == BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER)
            {
                m_notif_attr_app_id_latest = p_evt->attr;
            }
            break;
        case BLE_ANCS_C_EVT_DISCOVERY_FAILED:
            NRF_LOG_DEBUG("Apple Notification Center Service not discovered on the server.\r\n");
            break;

        case BLE_ANCS_C_EVT_APP_ATTRIBUTE:
            app_attr_print(&p_evt->attr);
            break;
        case BLE_ANCS_C_EVT_NP_ERROR:
            err_code_print(p_evt->err_code_np);
            break;
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the GAP.
 *
 * @details Use this function to set up all necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    ret_code_t              ret;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    ret = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(ret);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    ret = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling the Apple Notification Service client errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void apple_notification_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             ret;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    ret = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling Database Discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective service instances.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_ancs_c_on_db_disc_evt(&m_ancs_c, p_evt);
}


/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
static void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           ret;

    ret = pm_init();
    APP_ERROR_CHECK(ret);

    if (erase_bonds)
    {
        ret = pm_peers_delete();
        APP_ERROR_CHECK(ret);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;
    
    ret = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(ret);

    ret = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t ret = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(ret);

    // Prepare wakeup buttons.
    ret = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(ret);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    ret = sd_power_system_off();
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling advertising events.
 *
 * @details This function is called for advertising events that are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t ret;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising\r\n");
            ret = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(ret);
            break;

        case BLE_ADV_EVT_SLOW:
            NRF_LOG_INFO("Slow advertising\r\n");
            ret = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW);
            APP_ERROR_CHECK(ret);
            break;

        case BLE_ADV_EVT_FAST_WHITELIST:
            NRF_LOG_INFO("Fast advertising with Whitelist\r\n");
            ret = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST);
            APP_ERROR_CHECK(ret);
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        case BLE_ADV_EVT_WHITELIST_REQUEST:
        {
            ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            uint32_t       addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
            uint32_t       irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

            ret = pm_whitelist_get(whitelist_addrs, &addr_cnt, whitelist_irks, &irk_cnt);
            APP_ERROR_CHECK(ret);
            NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist\r\n",
                           addr_cnt,
                           irk_cnt);

            // Apply the whitelist.
            ret = ble_advertising_whitelist_reply(whitelist_addrs,
                                                  addr_cnt,
                                                  whitelist_irks,
                                                  irk_cnt);
            APP_ERROR_CHECK(ret);
        }
        break;

        default:
            break;
    }
}


/**@brief Function for handling the application's BLE stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    ret_code_t ret = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.\r\n");
            ret = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(ret);

            m_cur_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            ret               = app_timer_start(m_sec_req_timer_id, SECURITY_REQUEST_DELAY, NULL);
            APP_ERROR_CHECK(ret);
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.\r\n");
            m_cur_conn_handle = BLE_CONN_HANDLE_INVALID;
            ret               = app_timer_stop(m_sec_req_timer_id);
            APP_ERROR_CHECK(ret);

            if (p_ble_evt->evt.gap_evt.conn_handle == m_ancs_c.conn_handle)
            {
                m_ancs_c.conn_handle = BLE_CONN_HANDLE_INVALID;
            }
            if (m_is_wl_changed)
            {
                // The whitelist has been modified, update it in the Peer Manager.
                ret = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
                APP_ERROR_CHECK(ret);

                ret = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);
                if (ret != NRF_ERROR_NOT_SUPPORTED)
                {
                    APP_ERROR_CHECK(ret);
                }

                m_is_wl_changed = false;
            }
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            ret = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(ret);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
            ret = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(ret);
            break; // BLE_GATTS_EVT_TIMEOUT

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            ret = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                  NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(ret);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            // No implementation needed.
            break;
    }
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in] event  Event generated by button press.
 */
static void bsp_event_handler(bsp_event_t event)
{
    ret_code_t ret;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            ret = sd_ble_gap_disconnect(m_cur_conn_handle,
                                        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (ret != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(ret);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_ancs_c.conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                ret = ble_advertising_restart_without_whitelist();
                if (ret != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(ret);
                }
            }
            break;

        case BSP_EVENT_KEY_0:
            ret = nrf_ble_ancs_c_request_attrs(&m_ancs_c, &m_notification_latest);
            APP_ERROR_CHECK(ret);
            break;

        case BSP_EVENT_KEY_1:
            if(m_notif_attr_app_id_latest.attr_id == BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER
                && m_notif_attr_app_id_latest.attr_len != 0)
            {
                NRF_LOG_INFO("Request for %s: \r\n", (uint32_t)m_notif_attr_app_id_latest.p_attr_data);
                ret = nrf_ble_ancs_c_app_attr_request(&m_ancs_c,
                                                      m_notif_attr_app_id_latest.p_attr_data,
                                                      m_notif_attr_app_id_latest.attr_len);
                APP_ERROR_CHECK(ret);
            }
            break;

        case BSP_EVENT_KEY_2:
            if(m_notification_latest.evt_flags.positive_action == true)
            {
                NRF_LOG_INFO("Performing Positive Action.\r\n");
                ret = nrf_ancs_perform_notif_action(&m_ancs_c,
                                                    m_notification_latest.notif_uid,
                                                    ACTION_ID_POSITIVE);
                APP_ERROR_CHECK(ret);
            }
            break;

        case BSP_EVENT_KEY_3:
            if(m_notification_latest.evt_flags.negative_action == true)
            {
                NRF_LOG_INFO("Performing Negative Action.\r\n");
                ret = nrf_ancs_perform_notif_action(&m_ancs_c,
                                                    m_notification_latest.notif_uid,
                                                    ACTION_ID_NEGATIVE);
                APP_ERROR_CHECK(ret);
            }
            break;

        default:
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    /** The Connection state module has to be fed BLE events in order to function correctly
     * Remember to call ble_conn_state_on_ble_evt before calling any ble_conns_state_* functions. */
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);
    ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_ancs_c_on_ble_evt(&m_ancs_c, p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the system event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    // Dispatch the system event to the fstorage module, where it will be
    // dispatched to the Flash Data Storage (FDS) module.
    fs_sys_event_handler(sys_evt);

    // Dispatch to the Advertising module last, since it will check if there are any
    // pending flash operations in fstorage. Let fstorage process system events first,
    // so that it can report correctly to the Advertising module.
    ble_advertising_on_sys_evt(sys_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t ret;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    
    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
    
    ble_enable_params_t ble_enable_params;
    ret = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                               PERIPHERAL_LINK_COUNT,
                                               &ble_enable_params);
    APP_ERROR_CHECK(ret);

    ble_enable_params.common_enable_params.vs_uuid_count = VENDOR_SPECIFIC_UUID_COUNT;

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);
    
    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    ret = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(ret);

    // Register with the SoftDevice handler module for BLE events.
    ret = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(ret);

    // Register with the SoftDevice handler module for System events.
    ret = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for initializing the Apple Notification Center Service.
 */
static void services_init(void)
{
    ble_ancs_c_init_t ancs_init_obj;
    ret_code_t        ret;

    memset(&ancs_init_obj, 0, sizeof(ancs_init_obj));

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_APP_IDENTIFIER,
                                  m_attr_appid,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);
    
    ret = nrf_ble_ancs_c_app_attr_add(&m_ancs_c,
                                      BLE_ANCS_APP_ATTR_ID_DISPLAY_NAME,
                                      m_attr_disp_name,
                                      sizeof(m_attr_disp_name));
    APP_ERROR_CHECK(ret);

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_TITLE,
                                  m_attr_title,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_MESSAGE,
                                  m_attr_message,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_SUBTITLE,
                                  m_attr_subtitle,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_MESSAGE_SIZE,
                                  m_attr_message_size,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_DATE,
                                  m_attr_date,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_POSITIVE_ACTION_LABEL,
                                  m_attr_posaction,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);

    ret = nrf_ble_ancs_c_attr_add(&m_ancs_c,
                                  BLE_ANCS_NOTIF_ATTR_ID_NEGATIVE_ACTION_LABEL,
                                  m_attr_negaction,
                                  ATTR_DATA_SIZE);
    APP_ERROR_CHECK(ret);

    ancs_init_obj.evt_handler   = on_ancs_c_evt;
    ancs_init_obj.error_handler = apple_notification_error_handler;

    ret = ble_ancs_c_init(&m_ancs_c, &ancs_init_obj);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for initializing the advertising functionality.
 */
static void advertising_init(void)
{
    ret_code_t             ret;
    ble_advdata_t          advdata;
    ble_adv_modes_config_t options;

    static ble_uuid_t m_adv_uuids[1]; /**< Universally unique service identifiers. */

    m_adv_uuids[0].uuid = ANCS_UUID_SERVICE;
    m_adv_uuids[0].type = m_ancs_c.service.service.uuid.type;

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type                = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance       = true;
    advdata.flags                    = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    advdata.uuids_complete.uuid_cnt  = 0;
    advdata.uuids_complete.p_uuids   = NULL;
    advdata.uuids_solicited.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_solicited.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_whitelist_enabled = true;
    options.ble_adv_fast_enabled      = true;
    options.ble_adv_fast_interval     = APP_ADV_FAST_INTERVAL;
    options.ble_adv_fast_timeout      = APP_ADV_FAST_TIMEOUT;
    options.ble_adv_slow_enabled      = true;
    options.ble_adv_slow_interval     = APP_ADV_SLOW_INTERVAL;
    options.ble_adv_slow_timeout      = APP_ADV_SLOW_TIMEOUT;

    ret = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for initializing buttons and LEDs.
 *
 * @param[out] p_erase_bonds  True if the clear bonds button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t ret = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                            APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                            bsp_event_handler);
    APP_ERROR_CHECK(ret);

    ret = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(ret);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for initializing the Event Scheduler.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Function for initializing the database discovery module.
 */
static void db_discovery_init(void)
{
    ret_code_t ret = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    ret_code_t ret = sd_app_evt_wait();
    APP_ERROR_CHECK(ret);
}


/**@brief Function for application main entry.
 */
int main(void)
{
    bool erase_bonds;

    // Initialize.
    timers_init();
    log_init();
    buttons_leds_init(&erase_bonds);
    ble_stack_init();
    peer_manager_init(erase_bonds);
    if (erase_bonds == true)
    {
        NRF_LOG_INFO("Bonds erased!\r\n");
    }
    db_discovery_init();
    scheduler_init();
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("BLE ANCS Started\r\n");
    advertising_start();


    // Enter main loop.
    for (;;)
    {
        app_sched_execute();
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }

}


/**
 * @}
 */


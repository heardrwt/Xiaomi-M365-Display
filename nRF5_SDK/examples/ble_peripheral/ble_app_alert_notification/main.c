/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
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
 * @defgroup ble_sdk_alert_notification_main main.c
 * @{
 * @ingroup ble_sdk_app_alert_notification
 * @brief Alert Notification Client Sample Application main file.
 *
 * This file contains the source code for a sample application using the Alert Notification Profile
 * Client. This application uses the @ref srvlib_conn_params module.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ble_ans_c.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble_hci.h"
#include "ble_gap.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "nrf_gpio.h"
#include "softdevice_handler.h"
#include "ble_srv_common.h"
#include "ble_conn_params.h"
#include "boards.h"
#include "ble_db_discovery.h"
#include "app_timer.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "peer_manager.h"
#include "fds.h"
#include "fstorage.h"
#include "ble_conn_state.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define DEVICE_NAME                     "Nordic_Alert_Notif."                       /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_FAST_INTERVAL           40                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_SLOW_INTERVAL           3200                                        /**< Slow advertising interval (in units of 0.625 ms. This value corresponds to 2 seconds). */
#define APP_ADV_FAST_TIMEOUT            30                                          /**< The duration of the fast advertising period (in seconds). */
#define APP_ADV_SLOW_TIMEOUT            180                                         /**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define SECURITY_REQUEST_DELAY          APP_TIMER_TICKS(1500, APP_TIMER_PRESCALER)  /**< Delay after connection until security request is sent (1.5 seconds). */

#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define MESSAGE_BUFFER_SIZE             18                                          /**< Size of buffer holding optional messages in notifications. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

typedef enum
{
    ALERT_NOTIFICATION_DISABLED, /**< Alert Notifications has been disabled. */
    ALERT_NOTIFICATION_ENABLED,  /**< Alert Notifications has been enabled. */
    ALERT_NOTIFICATION_ON,       /**< Alert State is on. */
} ble_ans_c_alert_state_t;

static ble_ans_c_t        m_ans_c;                                                 /**< Structure used to identify the Alert Notification Service Client. */
static uint8_t            m_alert_message_buffer[MESSAGE_BUFFER_SIZE];             /**< Message buffer for optional notify messages. */
static ble_db_discovery_t m_ble_db_discovery;                                      /**< Structure used to identify the DB Discovery module. */
static uint16_t           m_cur_conn_handle = BLE_CONN_HANDLE_INVALID;              /**< Handle of the current connection. */

static ble_ans_c_alert_state_t m_new_alert_state    = ALERT_NOTIFICATION_DISABLED; /**< State that holds the current state of New Alert Notifications, i.e. Enabled, Alert On, Disabled. */
static ble_ans_c_alert_state_t m_unread_alert_state = ALERT_NOTIFICATION_DISABLED; /**< State that holds the current state of Unread Alert Notifications, i.e. Enabled, Alert On, Disabled. */
APP_TIMER_DEF(m_sec_req_timer_id);                                                 /**< Security request timer. The timer lets us start pairing request if one does not arrive from the Central. */

static void advertising_start(void);

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            NRF_LOG_INFO("Connected to a previously bonded device.\r\n");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            NRF_LOG_INFO("Connection secured. Role: %d. conn_handle: %d, Procedure: %d\r\n",
                         ble_conn_state_role(p_evt->conn_handle),
                         p_evt->conn_handle,
                         p_evt->params.conn_sec_succeeded.procedure);
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
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
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
    uint32_t             err_code;
    pm_conn_sec_status_t status;

    if (m_cur_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        err_code = pm_conn_sec_status_get(m_cur_conn_handle, &status);
        APP_ERROR_CHECK(err_code);

        // If the link is still not secured by the peer, initiate security procedure.
        if (!status.encrypted)
        {
            NRF_LOG_INFO("Start encryption\r\n");
            err_code = pm_conn_secure(m_cur_conn_handle, false);
            APP_ERROR_CHECK(err_code);
        }
    }
}


/**@brief Function for setup of alert notifications in central.
 *
 * @details This function will be called when a successful connection has been established.
 */
static void alert_notification_setup(void)
{
    uint32_t err_code;

    err_code = ble_ans_c_enable_notif_new_alert(&m_ans_c);
    APP_ERROR_CHECK(err_code);

    m_new_alert_state = ALERT_NOTIFICATION_ENABLED;

    err_code = ble_ans_c_enable_notif_unread_alert(&m_ans_c);
    APP_ERROR_CHECK(err_code);

    m_unread_alert_state = ALERT_NOTIFICATION_ENABLED;
}


/**@brief Function for setup of alert notifications in central.
 *
 * @details This function will be called when supported alert notification and
 *          supported unread alert notifications has been fetched.
 *
 * @param[in] p_evt  Event containing the response with supported alert types.
 */
static void control_point_setup(ble_ans_c_evt_t * p_evt)
{
    uint32_t                err_code;
    ble_ans_control_point_t setting;

    if (p_evt->uuid.uuid == BLE_UUID_SUPPORTED_UNREAD_ALERT_CATEGORY_CHAR)
    {
        if (p_evt->data.settings.ans_notification_call_support)
        {
            setting.command  = ANS_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION;
            setting.category = ANS_TYPE_NOTIFICATION_CALL;
        }
        else if (p_evt->data.settings.ans_missed_call_support)
        {
            setting.command  = ANS_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION;
            setting.category = ANS_TYPE_MISSED_CALL;
        }
        else
        {
            // Don't configure the control point if the above alerts types are not supported.
            return;
        }
    }
    else if (p_evt->uuid.uuid == BLE_UUID_SUPPORTED_NEW_ALERT_CATEGORY_CHAR)
    {
        if (p_evt->data.settings.ans_notification_call_support)
        {
            setting.command  = ANS_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION;
            setting.category = ANS_TYPE_NOTIFICATION_CALL;
        }
        else if (p_evt->data.settings.ans_missed_call_support)
        {
            setting.command  = ANS_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION;
            setting.category = ANS_TYPE_MISSED_CALL;
        }
        else
        {
            // Don't configure the control point if the above alerts types are not supported.
            return;
        }
    }
    else
    {
        return;
    }

    err_code = ble_ans_c_control_point_write(&m_ans_c, &setting);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for reading supported alert notifications in central.
 *
 * @details This function will be called when a connection has been established.
 */
static void supported_alert_notification_read(void)
{
    uint32_t err_code;

    err_code = ble_ans_c_new_alert_read(&m_ans_c);
    APP_ERROR_CHECK(err_code);

    err_code = ble_ans_c_unread_alert_read(&m_ans_c);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for handling the key of the New Alert Notification button.
 *
 * @details This function check the current state of new alert and then, enable, disable or clear
 *          the alert depending on the state.
 */
static void new_alert_state_toggle(void)
{
    uint32_t err_code = NRF_SUCCESS;

    if (m_new_alert_state == ALERT_NOTIFICATION_ON)
    {
        m_new_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code          = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
    }
    else if (m_new_alert_state == ALERT_NOTIFICATION_ENABLED)
    {
        m_new_alert_state = ALERT_NOTIFICATION_DISABLED;
        err_code          = ble_ans_c_disable_notif_new_alert(&m_ans_c);
    }
    else
    {
        m_new_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code          = ble_ans_c_enable_notif_new_alert(&m_ans_c);
    }

    // If the user presses the button while we are not connected,
    // we will have NRF_ERROR_INVALID_STATE, thus we just ignore the error code.
    if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_HANDLER(err_code);
    }
}


/**@brief Function for handling the key of the Unread Alert button.
 *
 * @details This function check the current state of unread alert and then, enable, disable or
 *          clear the alert depending on the state.
 */
static void unread_alert_state_toggle(void)
{
    uint32_t err_code = NRF_SUCCESS;

    if (m_unread_alert_state == ALERT_NOTIFICATION_ON)
    {
        m_unread_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code             = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
    }
    else if (m_unread_alert_state == ALERT_NOTIFICATION_ENABLED)
    {
        m_unread_alert_state = ALERT_NOTIFICATION_DISABLED;
        err_code             = ble_ans_c_disable_notif_unread_alert(&m_ans_c);
    }
    else
    {
        m_unread_alert_state = ALERT_NOTIFICATION_ENABLED;
        err_code             = ble_ans_c_enable_notif_unread_alert(&m_ans_c);
    }

    // If the user presses the button while we are not connected,
    // we will have NRF_ERROR_INVALID_STATE, thus we just ignore the error code.
    if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
    {
        APP_ERROR_HANDLER(err_code);
    }
}


/**@brief Function for handling key presses of the All Alert Notify button.
 *
 * @details This function check the current state of the alert notifications and based on the state
 *          it will request the central to resend current alert counters.
 */
static void all_alert_notify_request(void)
{
    uint32_t err_code = NRF_SUCCESS;

    if (m_unread_alert_state == ALERT_NOTIFICATION_ON ||
        m_unread_alert_state == ALERT_NOTIFICATION_ENABLED
       )
    {
        err_code = ble_ans_c_unread_alert_notify(&m_ans_c, ANS_TYPE_ALL_ALERTS);

        if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_HANDLER(err_code);
        }
    }

    if (m_new_alert_state == ALERT_NOTIFICATION_ON ||
        m_new_alert_state == ALERT_NOTIFICATION_ENABLED
       )
    {
        err_code = ble_ans_c_new_alert_notify(&m_ans_c, ANS_TYPE_ALL_ALERTS);

        if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_HANDLER(err_code);
        }
    }
}


/**@brief Function for lighting up the LED corresponding to the notification received.
 *
 * @param[in]   p_evt   Event containing the notification.
 */
static void handle_alert_notification(ble_ans_c_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->uuid.uuid == BLE_UUID_UNREAD_ALERT_CHAR)
    {
        if (m_unread_alert_state == ALERT_NOTIFICATION_ENABLED)
        {
            err_code = bsp_indication_set(BSP_INDICATE_ALERT_1);
            APP_ERROR_CHECK(err_code);
            m_unread_alert_state = ALERT_NOTIFICATION_ON;
        }
    }
    else if (p_evt->uuid.uuid == BLE_UUID_NEW_ALERT_CHAR)
    {
        if (m_new_alert_state == ALERT_NOTIFICATION_ENABLED)
        {
            err_code = bsp_indication_set(BSP_INDICATE_ALERT_0);
            APP_ERROR_CHECK(err_code);
            m_new_alert_state = ALERT_NOTIFICATION_ON;
        }
    }
    else
    {
        // Only Unread and New Alerts exists, thus do nothing.
    }
}


/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module, making it use the scheduler.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create security request timer.
    err_code = app_timer_create(&m_sec_req_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                sec_req_timeout_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Alert Notification Service Client.
 *
 * @details This function will be called for all events in the Alert Notification Client which
 *          are passed to the application.
 *
 * @param[in]   p_evt   Event received from the Alert Notification Service Client.
 */
static void on_ans_c_evt(ble_ans_c_evt_t * p_evt)
{
    uint32_t err_code;

    switch (p_evt->evt_type)
    {
        case BLE_ANS_C_EVT_NOTIFICATION:
            handle_alert_notification(p_evt);
            break; // BLE_ANS_C_EVT_NOTIFICATION

        case BLE_ANS_C_EVT_DISCOVERY_COMPLETE:
            err_code = ble_ans_c_handles_assign(&m_ans_c,
                                                p_evt->conn_handle,
                                                &p_evt->data.service);
            APP_ERROR_CHECK(err_code);
            supported_alert_notification_read();
            alert_notification_setup();
            break; // BLE_ANS_C_EVT_DISCOVERY_COMPLETE

        case BLE_ANS_C_EVT_READ_RESP:
            control_point_setup(p_evt);
            break; // BLE_ANS_C_EVT_READ_RESP

        case BLE_ANS_C_EVT_DISCONN_COMPLETE:
            m_new_alert_state    = ALERT_NOTIFICATION_DISABLED;
            m_unread_alert_state = ALERT_NOTIFICATION_DISABLED;

            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_ANS_C_EVT_DISCONN_COMPLETE

        default:
            // No implementation needed.
            break;
    }
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

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Alert Notification Service Client errors.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void alert_notification_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initialization of the Alert Notification Service Client.
 */
static void alert_notification_init()
{
    uint32_t         err_code;
    ble_ans_c_init_t ans_init_obj;

    memset(&ans_init_obj, 0, sizeof(ans_init_obj));
    memset(m_alert_message_buffer, 0, MESSAGE_BUFFER_SIZE);

    ans_init_obj.evt_handler         = on_ans_c_evt;
    ans_init_obj.message_buffer_size = MESSAGE_BUFFER_SIZE;
    ans_init_obj.p_message_buffer    = m_alert_message_buffer;
    ans_init_obj.error_handler       = alert_notification_error_handler;

    err_code = ble_ans_c_init(&m_ans_c, &ans_init_obj);
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
    ble_ans_c_on_db_disc_evt(&m_ans_c, p_evt);
}


/** @brief Database discovery module initialization.
 */
static void db_discovery_init(void)
{
    uint32_t err_code = ble_db_discovery_init(db_disc_handler);

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
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
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);

    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_DIRECTED:
            NRF_LOG_INFO("Directed advertising\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED);
            APP_ERROR_CHECK(err_code);
            break; // BLE_ADV_EVT_DIRECTED

        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break; // BLE_ADV_EVT_FAST

        case BLE_ADV_EVT_SLOW:
            NRF_LOG_INFO("Slow advertising\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW);
            APP_ERROR_CHECK(err_code);
            break; // BLE_ADV_EVT_SLOW

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break; // BLE_ADV_EVT_IDLE

        default:
            break;
    }
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_cur_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = ble_db_discovery_start(&m_ble_db_discovery, m_cur_conn_handle);
            APP_ERROR_CHECK(err_code);
            err_code = app_timer_start(m_sec_req_timer_id, SECURITY_REQUEST_DELAY, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            m_cur_conn_handle = BLE_CONN_HANDLE_INVALID;
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
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


/**@brief Function for handling events from the BSP module.
 *
 * @param[in] event  Event generated by button press.
 */
static void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break; // BSP_EVENT_SLEEP

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_cur_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break; // BSP_EVENT_DISCONNECT

        case BSP_EVENT_KEY_0:
            if (m_ans_c.conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                new_alert_state_toggle();
            }
            break; // BSP_EVENT_KEY_0

        case BSP_EVENT_KEY_1:
            if (m_ans_c.conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                unread_alert_state_toggle();
            }
            break; // BSP_EVENT_KEY_1

        case BSP_EVENT_KEY_2:
            if (m_ans_c.conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                all_alert_notify_request();
            }
            break; // BSP_EVENT_KEY_2

        default:
            // No implementation needed.
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
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);
    ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_ans_c_on_ble_evt(&m_ans_c, p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
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
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

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
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
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

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = 0;
    advdata.uuids_complete.p_uuids  = NULL;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_FAST_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_FAST_TIMEOUT;
    options.ble_adv_slow_enabled  = true;
    options.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
    options.ble_adv_slow_timeout  = APP_ADV_SLOW_TIMEOUT;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the
 *                            application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                                 bsp_event_handler);

    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    bool     erase_bonds;

    // Initialize.
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    timers_init();
    buttons_leds_init(&erase_bonds);
    ble_stack_init();
    peer_manager_init(erase_bonds);
    if (erase_bonds == true)
    {
        NRF_LOG_INFO("Bonds erased!\r\n");
    }
    gap_params_init();
    advertising_init();
    db_discovery_init();
    alert_notification_init();
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("Alert Notification started\r\n");
    advertising_start();

    // Enter main loop.
    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }
}


/**
 * @}
 */

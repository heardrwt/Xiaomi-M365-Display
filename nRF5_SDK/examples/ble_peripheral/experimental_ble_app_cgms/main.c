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

/** @file
 *
 * @defgroup ble_sdk_app_cgms_main main.c
 * @{
 * @ingroup ble_sdk_app_cgms
 * @brief Continuous Glucose Monitoring Profile Sample Application
 *
 * This file contains the source code for a sample application using the Continuous Glucose Monitoring service.
 * Bond Management Service, Battery service and Device Information service is also present.
 *
 */

#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "sensorsim.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "peer_manager.h"
#include "fds.h"
#include "fstorage.h"
#include "ble_conn_state.h"
#include "nrf_ble_qwr.h"

//Services
#include "ble_bas.h"
#include "nrf_ble_bms.h"
#include "nrf_ble_cgms.h"
#include "ble_dis.h"
#include "ble_racp.h"



#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                           /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE            GATT_MTU_SIZE_DEFAULT                       /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define DEVICE_NAME                     "Nordic_CGMS"                               /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */

#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         6                                           /**< Size of timer operation queues. */

#define SECURITY_REQUEST_DELAY          APP_TIMER_TICKS(4000, APP_TIMER_PRESCALER)  /**< Delay after connection until Security Request is sent, if necessary (ticks). */

#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(10000, APP_TIMER_PRESCALER) /**< Battery level measurement interval (ticks). */
#define MIN_BATTERY_LEVEL               81                                          /**< Minimum battery level as returned by the simulated measurement function. */
#define MAX_BATTERY_LEVEL               100                                         /**< Maximum battery level as returned by the simulated measurement function. */
#define BATTERY_LEVEL_INCREMENT         1                                           /**< Value by which the battery level is incremented/decremented for each call to the simulated measurement function. */

#define GLUCOSE_MEAS_INTERVAL           1                                           /**< Fastest possible communication interval*/
#define GL_CONCENTRATION_INC            10                                          /**< How much the simulated glucose concentration will increase upon a button press. */
#define GL_CONCENTRATION_DEC            5                                           /**< How much the simulated glucose concentration will decrease upon a button press. */
#define MAX_GLUCOSE_CONCENTRATION       800
#define MIN_GLUCOSE_CONCENTRATION       5                                           /**< Below 40 is hypo */

#define SECOND_10_MS_UNITS              100                                         /**< Definition of 1 second, when 1 unit is 10 ms. */
#define MIN_CONN_INTERVAL               7                                           /**< Minimum acceptable connection interval (0.25 seconds), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               400                                         /**< Maximum acceptable connection interval (0.5 second), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                (4 * SECOND_10_MS_UNITS)                    /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(15000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define APP_ADV_FAST_INTERVAL           0x0028                                      /**< Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.). */
#define APP_ADV_SLOW_INTERVAL           0x0C80                                      /**< Slow advertising interval (in units of 0.625 ms. This value corrsponds to 2 seconds). */
#define APP_ADV_FAST_TIMEOUT            30                                          /**< The duration of the fast advertising period (in seconds). */
#define APP_ADV_SLOW_TIMEOUT            180                                         /**< The duration of the slow advertising period (in seconds). */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static pm_peer_id_t                     m_peer_id;                                  /**< Device reference handle to the current bonded central. */

typedef enum
{
    BLE_NO_ADV,                                                                     /**< No advertising running. */
    BLE_DIRECTED_ADV,                                                               /**< Direct advertising to the latest central. */
    BLE_FAST_ADV_WHITELIST,                                                         /**< Advertising with whitelist. */
    BLE_FAST_ADV,                                                                   /**< Fast advertising running. */
    BLE_SLOW_ADV,                                                                   /**< Slow advertising running. */
    BLE_SLEEP,                                                                      /**< Go to system-off. */
} ble_advertising_mode_t;


static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static ble_bas_t                        m_bas;                                      /**< Structure used to identify the battery service. */
static nrf_ble_cgms_t                   m_cgms;                                     /**< Structure used to identify the glucose service. */
static nrf_ble_bms_t                    m_bms;                                      /**< Structure used to identify the Bond Management service. */
static ble_conn_state_user_flag_id_t    m_bms_bonds_to_delete;                      //!< Flags used to identify bonds that should be deleted. */

static sensorsim_cfg_t                  m_battery_sim_cfg;                          /**< Battery Level sensor simulator configuration. */
static sensorsim_state_t                m_battery_sim_state;                        /**< Battery Level sensor simulator state. */

static uint16_t                         m_current_offset        = 0;
static uint16_t                         m_glucose_concentration = MIN_GLUCOSE_CONCENTRATION;

static pm_peer_id_t   m_whitelist_peers[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];  /**< List of peers currently in the whitelist. */
static uint32_t       m_whitelist_peer_cnt;                                 /**< Number of peers currently in the whitelist. */

APP_TIMER_DEF(m_battery_timer_id);                                                  /**< Battery timer. */
APP_TIMER_DEF(m_glucose_meas_timer_id);                                             /**< Glucose measurement timer. */
APP_TIMER_DEF(m_sec_req_timer_id);                                                  /**< Security request timer. The timer lets us start pairing request if one does not arrive from the Central. */

static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_CGM_SERVICE,  BLE_UUID_TYPE_BLE},};    /**< Universally unique service identifiers. */

// Persistent storage system event handler.
void pstorage_sys_event_handler(uint32_t sys_evt);

#define USE_AUTHORIZATION_CODE 1

#if USE_AUTHORIZATION_CODE
static uint8_t m_auth_code[] = {'A', 'B', 'C', 'D'}; //0x41, 0x42, 0x43, 0x44
static int m_auth_code_len = sizeof(m_auth_code);
#endif

uint16_t m_last_connected_central;

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling advertising errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void ble_advertising_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for performing battery measurement and updating the Battery Level characteristic
 *        in Battery Service.
 */
static void battery_level_update(void)
{
    uint32_t err_code;
    uint8_t  battery_level;

    battery_level = (uint8_t)sensorsim_measure(&m_battery_sim_state, &m_battery_sim_cfg);

    err_code = ble_bas_battery_level_update(&m_bas, battery_level);
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != BLE_ERROR_NO_TX_PACKETS) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    )
    {
        APP_ERROR_HANDLER(err_code);
    }
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

    ret = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    battery_level_update();
}


/**@brief Function for handling the Security Request timer timeout.
 *
 * @details This function will be called each time the Security Request timer expires.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void sec_req_timeout_handler(void * p_context)
{
    uint32_t                err_code;
    uint16_t                conn_handle;
    pm_conn_sec_status_t    status;

    if (m_peer_id != PM_PEER_ID_INVALID)
    {
        err_code = pm_conn_handle_get(m_peer_id, &conn_handle);
        APP_ERROR_CHECK(err_code);

        err_code = pm_conn_sec_status_get(conn_handle, &status);
        APP_ERROR_CHECK(err_code);

        // If the link is still not secured by the peer, initiate security procedure.
        if (!status.encrypted)
        {
            err_code = pm_conn_secure(conn_handle, false);
            APP_ERROR_CHECK(err_code);
        }
    }
}


/**@brief Function for updating the glucose measurement and updating the glucose characteristic in Continuous Glucose Monitoring Service.
 */
static void read_glucose_measurement(void)
{
    ble_cgms_rec_t rec;
    uint32_t       err_code;

    memset(&rec, 0, sizeof(ble_cgms_rec_t));

    rec.meas.glucose_concentration = m_glucose_concentration;
    rec.meas.sensor_status_annunciation.warning    = 0;
    rec.meas.sensor_status_annunciation.calib_temp = 0;
    rec.meas.sensor_status_annunciation.status     = 0;
    rec.meas.flags                                 = 0;
    rec.meas.time_offset = m_current_offset;

    err_code = nrf_ble_cgms_meas_create(&m_cgms, &rec);
    if (err_code != NRF_SUCCESS)
    {
        // Ignore
    }
}

static void cgms_update_status(void)
{
    ret_code_t err_code;

    m_cgms.sensor_status.time_offset = m_current_offset;
    err_code = nrf_ble_cgms_update_status(&m_cgms, &m_cgms.sensor_status);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the glucose measurement timer expires.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void glucose_meas_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    if (m_cgms.comm_interval != 0)
    {
        m_current_offset += m_cgms.comm_interval;
    }
    else
    {
       m_current_offset += GLUCOSE_MEAS_INTERVAL;
    }
    read_glucose_measurement();
    cgms_update_status();
}


/**@brief Function for the Timer initialization.
 *
* @details Initializes the timer module. This creates and starts application timers.
*/
static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module, making it use the scheduler.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create battery timers.
    err_code = app_timer_create(&m_battery_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_glucose_meas_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                glucose_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    // Create Security Request timer.
    err_code = app_timer_create(&m_sec_req_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                sec_req_timeout_handler);
    APP_ERROR_CHECK(err_code);
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

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_GLUCOSE_METER);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


static void cgms_evt_handler(nrf_ble_cgms_t * p_cgms, nrf_ble_cgms_evt_t * p_evt)
{
    uint32_t err_code;

    switch (p_evt->evt_type)
    {
        case NRF_BLE_CGMS_EVT_NOTIFICATION_ENABLED:
        case NRF_BLE_CGMS_EVT_NOTIFICATION_DISABLED:
            break;

        case NRF_BLE_CGMS_EVT_START_SESSION:
            NRF_LOG_INFO("CGM Start Session\r\n");
            err_code = app_timer_start(m_glucose_meas_timer_id,
                                       APP_TIMER_TICKS(p_cgms->comm_interval * 60000,
                                                       APP_TIMER_PRESCALER),
                                       NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case NRF_BLE_CGMS_EVT_STOP_SESSION:
            NRF_LOG_INFO("CGM Stop Session\r\n");
            err_code = app_timer_stop(m_glucose_meas_timer_id);
            APP_ERROR_CHECK(err_code);
            break;

        case NRF_BLE_CGMS_EVT_WRITE_COMM_INTERVAL:
            NRF_LOG_INFO("CGM change communication interval\r\n");
            if (p_cgms->comm_interval == 0xFF)
            {
                p_cgms->comm_interval = GLUCOSE_MEAS_INTERVAL;
            }
            err_code = app_timer_stop(m_glucose_meas_timer_id);
            APP_ERROR_CHECK(err_code);

            if (p_cgms->comm_interval != 0)
            {
                err_code = app_timer_start(m_glucose_meas_timer_id,
                                           APP_TIMER_TICKS(p_cgms->comm_interval * 60000,
                                                           APP_TIMER_PRESCALER),
                                           NULL);
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }
}

typedef enum
{
    NO_DELETE,
    DELETE_ALL_BONDS,
    DELETE_CUR_BOND,
    DELETE_ALL_BUT_CUR_BOND,
}delete_bonds_t;
delete_bonds_t delete_operation = NO_DELETE;

#define MEM_BUFF_SIZE 512
nrf_ble_qwr_t        m_qwr;
uint8_t              mem[MEM_BUFF_SIZE];

/**@brief Function for handling events from bond management service.
 */
void bms_evt_handler(nrf_ble_bms_t * p_ess, nrf_ble_bms_evt_t * p_evt)
{
    ret_code_t err_code;

    switch (p_evt->evt_type)
    {
        case NRF_BLE_BMS_EVT_AUTH:
        {
            bool is_authorized = true;
#if USE_AUTHORIZATION_CODE
            if ((p_evt->auth_code.len != m_auth_code_len) ||
                (memcmp(m_auth_code, p_evt->auth_code.code, m_auth_code_len) != 0))
            {
                is_authorized = false;
            }
#endif
            err_code = nrf_ble_bms_auth_response(&m_bms, is_authorized);
            APP_ERROR_CHECK(err_code);
        } break; //NRF_BLE_BMS_EVT_AUTH

        default:
        break;
    }
}


uint16_t qwr_evt_handler(nrf_ble_qwr_t * p_qwr, nrf_ble_qwr_evt_t * p_evt)
{
    return nrf_ble_bms_on_qwr_evt(&m_bms, &m_qwr, p_evt);
}


static void delete_disconnected_bonds(void)
{
    uint32_t err_code;
    sdk_mapped_flags_key_list_t conn_handle_list = ble_conn_state_conn_handles();

    for (uint32_t i = 0; i < conn_handle_list.len; i++)
    {
        pm_peer_id_t peer_id;
        uint16_t conn_handle = conn_handle_list.flag_keys[i];
        bool pending         = ble_conn_state_user_flag_get(conn_handle, m_bms_bonds_to_delete);

        if (pending)
        {
            NRF_LOG_DEBUG("Attempting to delete bond.\r\n");
            err_code = pm_peer_id_get(conn_handle, &peer_id);
            if (err_code == NRF_SUCCESS)
            {
                err_code = pm_peer_delete(peer_id);
                APP_ERROR_CHECK(err_code);
            }
        }
    }
}


/**@brief Function for deleting the current bond
*/
static void delete_requesting_bond(nrf_ble_bms_t const * p_bms)
{
    ble_conn_state_user_flag_set(p_bms->conn_handle, m_bms_bonds_to_delete, true);
}


/**@brief Function for deleting all bonds
*/
static void delete_all_bonds(nrf_ble_bms_t const * p_bms)
{
    uint32_t err_code;
    uint16_t conn_handle;

    NRF_LOG_INFO("Client requested that all bonds be deleted\r\n");

    pm_peer_id_t peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
    while (peer_id != PM_PEER_ID_INVALID)
    {
        err_code = pm_conn_handle_get(peer_id, &conn_handle);
        APP_ERROR_CHECK(err_code);

        if (conn_handle != BLE_CONN_HANDLE_INVALID)
        {
            /* Defer the deletion since this connection is active. */
            NRF_LOG_DEBUG("Defer the deletion\r\n");
            ble_conn_state_user_flag_set(conn_handle, m_bms_bonds_to_delete, true);
        }
        else
        {
            err_code = pm_peer_delete(peer_id);
            APP_ERROR_CHECK(err_code);
        }

        peer_id = pm_next_peer_id_get(peer_id);
    }
}


/**@brief Function for initializing the services that will be used by the application.
 *
 * @details Initialize the Glucose, Battery and Device Information services.
 */
static void services_init(void)
{
    uint32_t             err_code;
    nrf_ble_bms_init_t   bms_init;
    nrf_ble_cgms_init_t  cgms_init;
    ble_dis_init_t       dis_init;
    ble_bas_init_t       bas_init;
    nrf_ble_qwr_init_t   qwr_init;

    // Initialize Queued Write Module
    memset(&qwr_init, 0, sizeof(qwr_init));
    qwr_init.mem_buffer.len   = MEM_BUFF_SIZE;
    qwr_init.mem_buffer.p_mem = mem;
    qwr_init.callback         = qwr_evt_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Bond Management Service
    memset(&bms_init, 0, sizeof(bms_init));

    m_bms_bonds_to_delete        = ble_conn_state_user_flag_acquire();
    bms_init.evt_handler         = bms_evt_handler;
    bms_init.error_handler       = service_error_handler;
#if USE_AUTHORIZATION_CODE
    bms_init.feature.delete_requesting_auth         = true;
    bms_init.feature.delete_all_auth                = true;
    bms_init.feature.delete_all_but_requesting_auth = false;
#else
    bms_init.feature.delete_requesting              = true;
    bms_init.feature.delete_all                     = true;
    bms_init.feature.delete_all_but_requesting      = false;
#endif
    bms_init.bms_feature_sec_req = SEC_JUST_WORKS;
    bms_init.bms_ctrlpt_sec_req  = SEC_JUST_WORKS;

    bms_init.p_qwr                                       = &m_qwr;
    bms_init.bond_callbacks.delete_requesting            = delete_requesting_bond;
    bms_init.bond_callbacks.delete_all                   = delete_all_bonds;

    err_code = nrf_ble_bms_init(&m_bms, &bms_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Glucose Service - sample selection of feature bits

    m_cgms.comm_interval = GLUCOSE_MEAS_INTERVAL;

    memset(&cgms_init, 0, sizeof(cgms_init));

    cgms_init.evt_handler           = cgms_evt_handler;
    cgms_init.error_handler         = service_error_handler;

    cgms_init.initial_run_time     = 20;

    cgms_init.initial_sensor_status.time_offset    = 0x00;
    cgms_init.initial_sensor_status.status.status |= NRF_BLE_CGMS_STATUS_SESSION_STOPPED;

    err_code = nrf_ble_cgms_init(&m_cgms, &cgms_init);
    APP_ERROR_CHECK(err_code);

    m_cgms.comm_interval = GLUCOSE_MEAS_INTERVAL;

    //add a basic measurement with only mandatory fields

    // Initialize Battery Service
    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the sensor simulators.
 */
static void sensor_simulator_init(void)
{
    m_battery_sim_cfg.min          = MIN_BATTERY_LEVEL;
    m_battery_sim_cfg.max          = MAX_BATTERY_LEVEL;
    m_battery_sim_cfg.incr         = BATTERY_LEVEL_INCREMENT;
    m_battery_sim_cfg.start_at_max = true;

    sensorsim_init(&m_battery_sim_state, &m_battery_sim_cfg);
}


/**@brief Function for starting application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameter events.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail configuration parameter, but instead we use the
 *                event handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
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
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
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
            NRF_LOG_INFO("BLE_ADV_EVT_DIRECTED\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_DIRECTED);
            APP_ERROR_CHECK(err_code);
            break; //BLE_ADV_EVT_DIRECTED

        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("BLE_ADV_EVT_FAST\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break; //BLE_ADV_EVT_FAST

        case BLE_ADV_EVT_SLOW:
            NRF_LOG_INFO("BLE_ADV_EVT_SLOW\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_SLOW);
            APP_ERROR_CHECK(err_code);
            break; //BLE_ADV_EVT_SLOW

        case BLE_ADV_EVT_FAST_WHITELIST:
            NRF_LOG_INFO("BLE_ADV_EVT_FAST_WHITELIST\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST);
            APP_ERROR_CHECK(err_code);
            break; //BLE_ADV_EVT_FAST

        case BLE_ADV_EVT_SLOW_WHITELIST:
            NRF_LOG_INFO("BLE_ADV_EVT_SLOW_WHITELIST\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING_WHITELIST);
            APP_ERROR_CHECK(err_code);
            break; //BLE_ADV_EVT_SLOW_WHITELIST

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break; //BLE_ADV_EVT_IDLE

        case BLE_ADV_EVT_WHITELIST_REQUEST:
        {
            NRF_LOG_DEBUG("BLE_ADV_EVT_WHITELIST_REQUEST\r\n");
            ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
            uint32_t addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
            uint32_t irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

            m_whitelist_peer_cnt = (sizeof(m_whitelist_peers) / sizeof(pm_peer_id_t));

            // Fetch a list of peer IDs and whitelist them.
            peer_list_get(m_whitelist_peers, &m_whitelist_peer_cnt);
            err_code = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
            APP_ERROR_CHECK(err_code);
            err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt,
                                        whitelist_irks,  &irk_cnt);
            APP_ERROR_CHECK(err_code);

            NRF_LOG_DEBUG("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist\r\n",
                           addr_cnt,
                           irk_cnt);

            // Apply the whitelist.
            err_code = ble_advertising_whitelist_reply(whitelist_addrs, addr_cnt,
                                                       whitelist_irks,  irk_cnt);
            APP_ERROR_CHECK(err_code);
        } break; //BLE_ADV_EVT_WHITELIST_REQUEST

        case BLE_ADV_EVT_PEER_ADDR_REQUEST:
        {
            pm_peer_data_bonding_t peer_bonding_data;

            // Only Give peer address if we have a handle to the bonded peer.
            if (m_peer_id != PM_PEER_ID_INVALID)
            {
                err_code = pm_peer_data_bonding_load(m_peer_id, &peer_bonding_data);
                if (err_code != NRF_ERROR_NOT_FOUND)
                {
                    APP_ERROR_CHECK(err_code);

                    ble_gap_addr_t * p_peer_addr = &(peer_bonding_data.peer_ble_id.id_addr_info);
                    err_code = ble_advertising_peer_addr_reply(p_peer_addr);
                    APP_ERROR_CHECK(err_code);
                }
            }
        } break; //BLE_ADV_EVT_PEER_ADDR_REQUEST

        default:
            break;
    }
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_bms_set_conn_handle(&m_bms, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            err_code = nrf_ble_cgms_conn_handle_assign(&m_cgms, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected\r\n");
            delete_disconnected_bonds();
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
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


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);
    nrf_ble_bms_on_ble_evt(&m_bms, p_ble_evt);
    nrf_ble_qwr_on_ble_evt(&m_qwr, p_ble_evt);
    nrf_ble_cgms_on_ble_evt(&m_cgms, p_ble_evt);
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
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

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
static void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist();
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;
        case BSP_EVENT_KEY_2:
            NRF_LOG_INFO("Increase GL Concentration\r\n");
            m_glucose_concentration += GL_CONCENTRATION_INC;
            if (m_glucose_concentration > MAX_GLUCOSE_CONCENTRATION)
            {
                m_glucose_concentration = MIN_GLUCOSE_CONCENTRATION;
            }
            break;
        case BSP_EVENT_KEY_3:
            NRF_LOG_INFO("Decrease GL Concentration\r\n");
            m_glucose_concentration -= GL_CONCENTRATION_DEC;
            if (m_glucose_concentration < MIN_GLUCOSE_CONCENTRATION)
            {
                m_glucose_concentration = MAX_GLUCOSE_CONCENTRATION;
            }
            break;

        default:
            break;
    }
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
            err_code = app_timer_start(m_sec_req_timer_id, SECURITY_REQUEST_DELAY, NULL);
            APP_ERROR_CHECK(err_code);
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
            NRF_LOG_INFO("Failed to secure connection. Disconnecting.\r\n");
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
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


/**@brief Function for the Peer Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Peer Manager.
 */
static void peer_manager_init(bool erase_bonds)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    if (erase_bonds)
    {
        err_code = pm_peers_delete();
        APP_ERROR_CHECK(err_code);
    }

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond              = SEC_PARAM_BOND;
    sec_param.mitm              = SEC_PARAM_MITM;
    sec_param.lesc              = SEC_PARAM_LESC;
    sec_param.keypress          = SEC_PARAM_KEYPRESS;
    sec_param.io_caps           = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob               = SEC_PARAM_OOB;
    sec_param.min_key_size      = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size      = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc     = 1;
    sec_param.kdist_own.id      = 1;
    sec_param.kdist_peer.enc    = 1;
    sec_param.kdist_peer.id     = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    uint8_t                adv_flags;
    ble_advdata_t          advdata;
    ble_adv_modes_config_t options;

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    adv_flags                       = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = adv_flags;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options));
    options.ble_adv_whitelist_enabled      = true;
    options.ble_adv_directed_enabled       = true;
    options.ble_adv_directed_slow_enabled  = false;
    options.ble_adv_directed_slow_interval = 0;
    options.ble_adv_directed_slow_timeout  = 0;
    options.ble_adv_fast_enabled           = true;
    options.ble_adv_fast_interval          = APP_ADV_FAST_INTERVAL;
    options.ble_adv_fast_timeout           = APP_ADV_FAST_TIMEOUT;
    options.ble_adv_slow_enabled           = true;
    options.ble_adv_slow_interval          = APP_ADV_SLOW_INTERVAL;
    options.ble_adv_slow_timeout           = APP_ADV_SLOW_TIMEOUT;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, ble_advertising_error_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
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


/**@brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;
    bool erase_bonds;

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
    services_init();
    sensor_simulator_init();
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("Continuous Glucose Monitoring Sensor Start!\r\n");
    application_timers_start();
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

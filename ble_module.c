/*
  ble_module.c
  
  Xiaomi M365 Display
  Copyright (c) 2018 Richard Heard. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Based on Nordic SDK Examples


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ble_module.h"

#include "nordic_common.h"
#include "app_error.h"
#include "app_uart.h"

#include "nrf.h"
#include "ble_db_discovery.h"

#include "app_timer.h"
#include "app_util.h"
#include "app_error.h"

#include "bsp.h"
#include "bsp_btn_ble.h"
#include "boards.h"

#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"

#include "softdevice_handler.h"

#include "ble_advdata.h"
#include "ble_nus_c.h"
#include "nrf_delay.h"

#include "ninebot_module.h"

#define NRF_LOG_MODULE_NAME "BLM"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define CENTRAL_LINK_COUNT      1                               /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT   0                               /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE    GATT_MTU_SIZE_DEFAULT           /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

//#define UART_TX_BUF_SIZE        256                             /**< UART TX buffer size. */
//#define UART_RX_BUF_SIZE        256                             /**< UART RX buffer size. */
                                                                
#define NUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN      /**< UUID type for the Nordic UART Service (vendor specific). */
                                                                
#define APP_TIMER_PRESCALER     0                               /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE 2                               /**< Size of timer operation queues. */
                                                                
#define SCAN_INTERVAL           0x00A0                          /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW             0x0050                          /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_ACTIVE             1                               /**< If 1, performe active scanning (scan requests). */
#define SCAN_SELECTIVE          0                               /**< If 1, ignore unknown devices (non whitelisted). */
#define SCAN_TIMEOUT            0x0000                          /**< */
                                                                
#define MIN_CONNECTION_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS) /**< Determines minimum connection interval in millisecond. */
#define MAX_CONNECTION_INTERVAL MSEC_TO_UNITS(75, UNIT_1_25_MS) /**< Determines maximum connection interval in millisecond. */
#define SLAVE_LATENCY           0                               /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT     MSEC_TO_UNITS(4000, UNIT_10_MS) /**< Determines supervision time-out in units of 10 millisecond. */
                                                                
#define UUID16_SIZE             2                               /**< Size of 16 bit UUID */
#define UUID32_SIZE	            4                               /**< Size of 32 bit UUID */
#define UUID128_SIZE            16                              /**< Size of 128 bit UUID */

static ble_nus_c_t              m_ble_nus_c;                    /* Nordic UART Service */
static ble_db_discovery_t       m_ble_db_discovery;

/**
 * @brief Connection parameters requested for connection.
 */
static const ble_gap_conn_params_t m_connection_param =
  {
    (uint16_t)MIN_CONNECTION_INTERVAL,  // Minimum connection
    (uint16_t)MAX_CONNECTION_INTERVAL,  // Maximum connection
    (uint16_t)SLAVE_LATENCY,            // Slave latency
    (uint16_t)SUPERVISION_TIMEOUT       // Supervision time-out
  };


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

/**
 * @brief NUS uuid
 */
static const ble_uuid_t m_nus_uuid = 
  {
    .uuid = BLE_UUID_NUS_SERVICE,
    .type = NUS_SERVICE_UUID_TYPE	
  };


/**@brief Function for asserts in the SoftDevice.
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
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
  NRF_LOG_ERROR("assert_nrf_callback!");
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

/**@brief Function to start scanning.
 */
void scan_start(void) {
    uint32_t err_code;
    
    err_code = sd_ble_gap_scan_start(&m_scan_params);
    APP_ERROR_CHECK(err_code);
    
    err_code = bsp_indication_set(BSP_INDICATE_SCANNING);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("Scan started.\r\n");
}

/**@brief Callback handling NUS Client events.
 *
 * @details This function is called to notify the application of NUS client events.
 *
 * @param[in]   p_ble_nus_c   NUS Client Handle. This identifies the NUS client
 * @param[in]   p_ble_nus_evt Pointer to the NUS Client event.
 */

/**@snippet [Handling events from the ble_nus_c module] */ 
static void ble_nus_c_evt_handler(ble_nus_c_t *p_ble_nus_c, const ble_nus_c_evt_t *p_ble_nus_evt) {
    uint32_t err_code;
  switch (p_ble_nus_evt->evt_type) {
        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
            err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
            APP_ERROR_CHECK(err_code);

    nrf_delay_ms(30);
            err_code = ble_nus_c_rx_notif_enable(p_ble_nus_c);
            APP_ERROR_CHECK(err_code);
            NRF_LOG_INFO("The device has the Nordic UART Service\r\n");
            ninebot_nus_start_polling(p_ble_nus_c);
            break;
        
        case BLE_NUS_C_EVT_NUS_RX_EVT: {
            // NRF_LOG_INFO("Received nus data\r\n");
            ninebot_nus_received_data(p_ble_nus_evt->p_data, p_ble_nus_evt->data_len);
  } break;
        
        case BLE_NUS_C_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected\r\n");
            ninebot_nus_stop_polling();
            scan_start();
            break;
    }
}

/**@brief Reads an advertising report and checks if a uuid is present in the service list.
 *
 * @details The function is able to search for 16-bit, 32-bit and 128-bit service uuids. 
 *          To see the format of a advertisement packet, see 
 *          https://www.bluetooth.org/Technical/AssignedNumbers/generic_access_profile.htm
 *
 * @param[in]   p_target_uuid The uuid to search fir
 * @param[in]   p_adv_report  Pointer to the advertisement report.
 *
 * @retval      true if the UUID is present in the advertisement report. Otherwise false  
 */
static bool is_uuid_present(const ble_uuid_t *p_target_uuid,
    const ble_gap_evt_adv_report_t *p_adv_report) {
  uint32_t err_code;
  uint32_t index = 0;
  uint8_t *p_data = (uint8_t *)p_adv_report->data;
  ble_uuid_t extracted_uuid;

  while (index < p_adv_report->dlen) {
    uint8_t field_length = p_data[index];
    uint8_t field_type = p_data[index + 1];

    if ((field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE) || (field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE)) {
      for (uint32_t u_index = 0; u_index < (field_length / UUID16_SIZE); u_index++) {
        err_code = sd_ble_uuid_decode(UUID16_SIZE,
            &p_data[u_index * UUID16_SIZE + index + 2],
            &extracted_uuid);
        if (err_code == NRF_SUCCESS) {
          if ((extracted_uuid.uuid == p_target_uuid->uuid) && (extracted_uuid.type == p_target_uuid->type)) {
            return true;
          }
        }
      }
    }

    else if ((field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE) || (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE)) {
      for (uint32_t u_index = 0; u_index < (field_length / UUID32_SIZE); u_index++) {
        err_code = sd_ble_uuid_decode(UUID16_SIZE,
            &p_data[u_index * UUID32_SIZE + index + 2],
            &extracted_uuid);
        if (err_code == NRF_SUCCESS) {
          if ((extracted_uuid.uuid == p_target_uuid->uuid) && (extracted_uuid.type == p_target_uuid->type)) {
            return true;
          }
        }
      }
    }

    else if ((field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE) || (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE)) {
      err_code = sd_ble_uuid_decode(UUID128_SIZE,
          &p_data[index + 2],
          &extracted_uuid);
      if (err_code == NRF_SUCCESS) {
        if ((extracted_uuid.uuid == p_target_uuid->uuid) && (extracted_uuid.type == p_target_uuid->type)) {
          return true;
        }
      }
    }
    index += field_length + 1;
  }
  return false;
}

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t *p_ble_evt) {
  uint32_t err_code;
  const ble_gap_evt_t *p_gap_evt = &p_ble_evt->evt.gap_evt;

  switch (p_ble_evt->header.evt_id) {
  case BLE_GAP_EVT_ADV_REPORT: {
    const ble_gap_evt_adv_report_t *p_adv_report = &p_gap_evt->params.adv_report;

    if (is_uuid_present(&m_nus_uuid, p_adv_report)) {

      err_code = sd_ble_gap_connect(&p_adv_report->peer_addr,
          &m_scan_params,
          &m_connection_param);

      if (err_code == NRF_SUCCESS) {
        // scan is automatically stopped by the connect
        err_code = bsp_indication_set(BSP_INDICATE_IDLE);
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("Connecting to target: ");
        NRF_LOG_HEXDUMP_INFO(p_adv_report->peer_addr.addr, BLE_GAP_ADDR_LEN);
      }
    }
  } break; // BLE_GAP_EVT_ADV_REPORT

  case BLE_GAP_EVT_CONNECTED:
    NRF_LOG_INFO("Connected to target\r\n");
    err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
    APP_ERROR_CHECK(err_code);

    // start discovery of services. The NUS Client waits for a discovery result
    err_code = ble_db_discovery_start(&m_ble_db_discovery, p_ble_evt->evt.gap_evt.conn_handle);
    APP_ERROR_CHECK(err_code);
    break; // BLE_GAP_EVT_CONNECTED

  case BLE_GAP_EVT_TIMEOUT:
    if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_SCAN) {
      NRF_LOG_DEBUG("Scan timed out.\r\n");
      scan_start();
    } else if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN) {
      NRF_LOG_INFO("Connection Request timed out.\r\n");
    }
    break; // BLE_GAP_EVT_TIMEOUT

  case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
    // Pairing not supported
    NRF_LOG_INFO("Pairing Not Supported.\r\n");
    err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

  case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
    // Accepting parameters requested by peer.
    NRF_LOG_DEBUG("GATT Update Request - Accept.\r\n");
    err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
        &p_gap_evt->params.conn_param_update_request.conn_params);
    APP_ERROR_CHECK(err_code);
    break; // BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST

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
    NRF_LOG_DEBUG("GATT Exchange MTU.\r\n");
    err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
        NRF_BLE_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
    break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif
  case BLE_EVT_TX_COMPLETE: {
    NRF_LOG_DEBUG("GATT TX Complete.\r\n");
  } break;
  default:
    break;
  }
}
/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack event has
 *  been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t *p_ble_evt) {
  //    NRF_LOG_DEBUG("ble_evt_dispatch - event\r\n");
  on_ble_evt(p_ble_evt);
//  bsp_btn_ble_on_ble_evt(p_ble_evt);
  ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
  ble_nus_c_on_ble_evt(&m_ble_nus_c, p_ble_evt);
}

/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t *p_evt) {
  NRF_LOG_DEBUG("db_disc_handler called\r\n");
  ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
}

/** @brief Function for initializing the Database Discovery Module.
 */
static void db_discovery_init(void) {
  uint32_t err_code = ble_db_discovery_init(db_disc_handler);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEBUG("db_discovery_init finished\r\n");
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void) {
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
  NRF_LOG_DEBUG("ble_stack_init finished.\r\n");

  nrf_delay_ms(100);
  db_discovery_init();
}

/**@brief Function for initializing the NUS Client.
 */
void nus_c_init(void) {
  uint32_t err_code;
  ble_nus_c_init_t nus_c_init_t;

  nus_c_init_t.evt_handler = ble_nus_c_evt_handler;

  err_code = ble_nus_c_init(&m_ble_nus_c, &nus_c_init_t);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEBUG("nus_c_init finished.\r\n");
}
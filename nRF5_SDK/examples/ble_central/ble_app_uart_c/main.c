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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "app_error.h"
#include "app_uart.h"
#include "ble_db_discovery.h"
#include "app_timer.h"
#include "app_util.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "boards.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "softdevice_handler.h"
#include "ble_advdata.h"
#include "ble_nus_c.h"

#define CENTRAL_LINK_COUNT      1                               /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT   0                               /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE    GATT_MTU_SIZE_DEFAULT           /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define UART_TX_BUF_SIZE        256                             /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE        256                             /**< UART RX buffer size. */

#define NUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN      /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_TIMER_PRESCALER     0                               /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE 2                               /**< Size of timer operation queues. */

#define SCAN_INTERVAL           0x00A0                          /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW             0x0050                          /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_ACTIVE             1                               /**< If 1, performe active scanning (scan requests). */
#define SCAN_SELECTIVE          0                               /**< If 1, ignore unknown devices (non whitelisted). */
#define SCAN_TIMEOUT            0x0000                          /**< Timout when scanning. 0x0000 disables timeout. */

#define MIN_CONNECTION_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS) /**< Determines minimum connection interval in millisecond. */
#define MAX_CONNECTION_INTERVAL MSEC_TO_UNITS(75, UNIT_1_25_MS) /**< Determines maximum connection interval in millisecond. */
#define SLAVE_LATENCY           0                               /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT     MSEC_TO_UNITS(4000, UNIT_10_MS) /**< Determines supervision time-out in units of 10 millisecond. */

#define UUID16_SIZE             2                               /**< Size of 16 bit UUID */
#define UUID32_SIZE             4                               /**< Size of 32 bit UUID */
#define UUID128_SIZE            16                              /**< Size of 128 bit UUID */

static ble_nus_c_t              m_ble_nus_c;                    /**< Instance of NUS service. Must be passed to all NUS_C API calls. */
static ble_db_discovery_t       m_ble_db_discovery;             /**< Instance of database discovery module. Must be passed to all db_discovert API calls */

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
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function to start scanning.
 */
static void scan_start(void)
{
    ret_code_t ret;

    ret = sd_ble_gap_scan_start(&m_scan_params);
    APP_ERROR_CHECK(ret);

    ret = bsp_indication_set(BSP_INDICATE_SCANNING);
    APP_ERROR_CHECK(ret);
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
    ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
}


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' i.e '\r\n' (hex 0x0D) or if the string has reached a length of
 *          @ref NUS_MAX_DATA_LENGTH.
 */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;

    switch (p_event->evt_type)
    {
        /**@snippet [Handling data from UART] */
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
            {
                while (ble_nus_c_string_send(&m_ble_nus_c, data_array, index) != NRF_SUCCESS)
                {
                    // repeat until sent.
                }
                index = 0;
            }
            break;
        /**@snippet [Handling data from UART] */
        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}


/**@brief Callback handling NUS Client events.
 *
 * @details This function is called to notify the application of NUS client events.
 *
 * @param[in]   p_ble_nus_c   NUS Client Handle. This identifies the NUS client
 * @param[in]   p_ble_nus_evt Pointer to the NUS Client event.
 */

/**@snippet [Handling events from the ble_nus_c module] */
static void ble_nus_c_evt_handler(ble_nus_c_t * p_ble_nus_c, const ble_nus_c_evt_t * p_ble_nus_evt)
{
    uint32_t err_code;
    switch (p_ble_nus_evt->evt_type)
    {
        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
            err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
            APP_ERROR_CHECK(err_code);

            err_code = ble_nus_c_rx_notif_enable(p_ble_nus_c);
            APP_ERROR_CHECK(err_code);
            printf("The device has the Nordic UART Service\r\n");
            break;

        case BLE_NUS_C_EVT_NUS_RX_EVT:
            for (uint32_t i = 0; i < p_ble_nus_evt->data_len; i++)
            {
                while (app_uart_put( p_ble_nus_evt->p_data[i]) != NRF_SUCCESS);
            }
            break;

        case BLE_NUS_C_EVT_DISCONNECTED:
            printf("Disconnected\r\n");
            scan_start();
            break;
    }
}
/**@snippet [Handling events from the ble_nus_c module] */

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
                            const ble_gap_evt_adv_report_t *p_adv_report)
{
    uint32_t err_code;
    uint32_t index = 0;
    uint8_t *p_data = (uint8_t *)p_adv_report->data;
    ble_uuid_t extracted_uuid;

    while (index < p_adv_report->dlen)
    {
        uint8_t field_length = p_data[index];
        uint8_t field_type   = p_data[index + 1];

        if ( (field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE)
           || (field_type == BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE)
           )
        {
            for (uint32_t u_index = 0; u_index < (field_length / UUID16_SIZE); u_index++)
            {
                err_code = sd_ble_uuid_decode(  UUID16_SIZE,
                                                &p_data[u_index * UUID16_SIZE + index + 2],
                                                &extracted_uuid);
                if (err_code == NRF_SUCCESS)
                {
                    if ((extracted_uuid.uuid == p_target_uuid->uuid)
                        && (extracted_uuid.type == p_target_uuid->type))
                    {
                        return true;
                    }
                }
            }
        }

        else if ( (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE)
                || (field_type == BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE)
                )
        {
            for (uint32_t u_index = 0; u_index < (field_length / UUID32_SIZE); u_index++)
            {
                err_code = sd_ble_uuid_decode(UUID16_SIZE,
                &p_data[u_index * UUID32_SIZE + index + 2],
                &extracted_uuid);
                if (err_code == NRF_SUCCESS)
                {
                    if ((extracted_uuid.uuid == p_target_uuid->uuid)
                        && (extracted_uuid.type == p_target_uuid->type))
                    {
                        return true;
                    }
                }
            }
        }

        else if ( (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE)
                || (field_type == BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE)
                )
        {
            err_code = sd_ble_uuid_decode(UUID128_SIZE,
                                          &p_data[index + 2],
                                          &extracted_uuid);
            if (err_code == NRF_SUCCESS)
            {
                if ((extracted_uuid.uuid == p_target_uuid->uuid)
                    && (extracted_uuid.type == p_target_uuid->type))
                {
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
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t              err_code;
    const ble_gap_evt_t * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_ADV_REPORT:
        {
            const ble_gap_evt_adv_report_t * p_adv_report = &p_gap_evt->params.adv_report;

            if (is_uuid_present(&m_nus_uuid, p_adv_report))
            {

                err_code = sd_ble_gap_connect(&p_adv_report->peer_addr,
                                              &m_scan_params,
                                              &m_connection_param);

                if (err_code == NRF_SUCCESS)
                {
                    // scan is automatically stopped by the connect
                    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
                    APP_ERROR_CHECK(err_code);
                    printf("Connecting to target %02x%02x%02x%02x%02x%02x\r\n",
                             p_adv_report->peer_addr.addr[0],
                             p_adv_report->peer_addr.addr[1],
                             p_adv_report->peer_addr.addr[2],
                             p_adv_report->peer_addr.addr[3],
                             p_adv_report->peer_addr.addr[4],
                             p_adv_report->peer_addr.addr[5]
                             );
                }
            }
        }break; // BLE_GAP_EVT_ADV_REPORT

        case BLE_GAP_EVT_CONNECTED:
            //NRF_LOG_DEBUG("Connected to target\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);

            // start discovery of services. The NUS Client waits for a discovery result
            err_code = ble_db_discovery_start(&m_ble_db_discovery, p_ble_evt->evt.gap_evt.conn_handle);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_TIMEOUT:
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_SCAN)
            {
                //NRF_LOG_DEBUG("Scan timed out.\r\n");
                scan_start();
            }
            else if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                printf("Connection Request timed out.\r\n");
            }
            break; // BLE_GAP_EVT_TIMEOUT

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            // Accepting parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            //NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            //NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
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
            break;
    }
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack event has
 *          been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
    ble_nus_c_on_ble_evt(&m_ble_nus_c,p_ble_evt);
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
}

/**@brief Function for handling events from the BSP module.
 *
 * @param[in] event  Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_ble_nus_c.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }
}

/**@brief Function for initializing the UART.
 */
static void uart_init(void)
{
    uint32_t err_code;

    const app_uart_comm_params_t comm_params =
      {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_ENABLED,
        .use_parity   = false,
        .baud_rate    = UART_BAUDRATE_BAUDRATE_Baud115200
      };

    APP_UART_FIFO_INIT(&comm_params,
                        UART_RX_BUF_SIZE,
                        UART_TX_BUF_SIZE,
                        uart_event_handle,
                        APP_IRQ_PRIORITY_LOWEST,
                        err_code);

    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the NUS Client.
 */
static void nus_c_init(void)
{
    uint32_t         err_code;
    ble_nus_c_init_t nus_c_init_t;

    nus_c_init_t.evt_handler = ble_nus_c_evt_handler;

    err_code = ble_nus_c_init(&m_ble_nus_c, &nus_c_init_t);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing buttons and leds.
 */
static void buttons_leds_init(void)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                                 bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for initializing the Database Discovery Module.
 */
static void db_discovery_init(void)
{
    uint32_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
}

/** @brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


int main(void)
{

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);

    uart_init();
    buttons_leds_init();
    db_discovery_init();
    ble_stack_init();
    nus_c_init();

    // Start scanning for peripherals and initiate connection
    // with devices that advertise NUS UUID.
    printf("Uart_c Scan started\r\n");
    scan_start();

    for (;;)
    {
        power_manage();
    }
}

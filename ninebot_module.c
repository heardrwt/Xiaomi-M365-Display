/*
  ninebot_module.c
  
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "ble_module.h"

#include "app_error.h"
#include "app_uart.h"
#include "nordic_common.h"

#include "ble_db_discovery.h"
#include "nrf.h"

#include "app_error.h"
#include "app_timer.h"
#include "app_util.h"

#include "boards.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "nrf_delay.h"

#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"

#include "softdevice_handler.h"

#include "ble_advdata.h"
#include "ble_nus_c.h"

#include "ninebot.h"
#include "ninebot_module.h"

#define NRF_LOG_MODULE_NAME "NBM"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define APP_TIMER_PRESCALER     0           /**< Value of the RTC1 PRESCALER register. */

// vars
APP_TIMER_DEF(m_ninebot_polling_timer_id); /** ninebot polling timer id. */
static ble_nus_c_t m_ninebot_nus_c;        /** Nordic UART Service */
static ninebot_data_callback_t m_data_callback;
static ninebot_data_t m_ninebot_data;

typedef enum {ninebot_expect_none, ninebot_expect_batt, ninebot_expect_speed} ninebot_expect_t; 
static ninebot_expect_t m_ninebot_expect;

// internal defs
void polling_timer_handler(void *p_context);
void handle_ninebot_pack(NinebotPack *pack);

// Ninebot

uint32_t ninebot_init(ninebot_data_callback_t data_callback) {
  uint32_t error_code = NRF_SUCCESS;
  if (!data_callback) {
    error_code = NRF_ERROR_INVALID_PARAM;
  } else {
    m_data_callback = data_callback;
    error_code = app_timer_create(&m_ninebot_polling_timer_id, APP_TIMER_MODE_REPEATED, polling_timer_handler);
    m_ninebot_data.connected = false;
    m_ninebot_data.battery_percentage = 1.0;
    m_ninebot_data.speed_kph = 0;
    m_ninebot_data.speed_mph = 0;
    m_data_callback(&m_ninebot_data);
  }

  NRF_LOG_DEBUG("ninebot_init finished.\r\n");
  return error_code;
}

uint32_t ninebot_uninit(void) {
  uint32_t error_code = NRF_SUCCESS;
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEBUG("ninebot_uninit finished.\r\n");
  return error_code;
}

uint32_t ninebot_get_current_data(ninebot_data_t *data_out) {
  uint32_t error_code = NRF_SUCCESS;
  if (data_out) {
    data_out = &m_ninebot_data;
  } else {
    error_code = NRF_ERROR_INVALID_PARAM;
  }
  return error_code;
}

// nus handlers
uint32_t ninebot_nus_start_polling(ble_nus_c_t *nus_c) {
  uint32_t error_code = NRF_SUCCESS;
  if (nus_c) {
    m_ninebot_nus_c = *nus_c;
    m_ninebot_data.connected = true;
    m_data_callback(&m_ninebot_data);
    // 3 times a second
    uint32_t ticks = APP_TIMER_TICKS(333, APP_TIMER_PRESCALER);
    uint32_t error_code = app_timer_start(m_ninebot_polling_timer_id, ticks, NULL);
    APP_ERROR_CHECK(error_code);
  } else {
    error_code = NRF_ERROR_INVALID_PARAM;
  }

  NRF_LOG_DEBUG("ninebot_nus_start_polling finished.\r\n");
  return error_code;
}

void ninebot_nus_received_data(uint8_t *p_data, uint8_t data_len) {
//  NRF_LOG_HEXDUMP_DEBUG(p_data, data_len);
  NinebotPack *pack = calloc(sizeof(NinebotPack), 1);
  ninebot_parse(p_data, data_len, pack);
  handle_ninebot_pack(pack);
  free(pack);
//  NRF_LOG_DEBUG("ninebot_nus_received_data finished.\r\n");
}

uint32_t ninebot_nus_stop_polling(void) {
  m_ninebot_data.connected = false;
  m_data_callback(&m_ninebot_data);

  uint32_t error_code = app_timer_stop(m_ninebot_polling_timer_id);
  APP_ERROR_CHECK(error_code);
  NRF_LOG_DEBUG("ninebot_nus_stop_polling finished.\r\n");
  return error_code;
}

// internal

void polling_timer_handler(void *p_context) {
  static uint8_t counter = 100;

  request_speed();
  // Only request these properties every 100 times.
  if (counter++ == 100) {
    counter = 0;
    nrf_delay_ms(100);
    request_battery_percentage();
    nrf_delay_ms(100);
    request_distance_remaining();
  }
}

void request_speed(void) {
  if (m_ninebot_data.connected) {
    NRF_LOG_DEBUG("request_speed\r\n");
    m_ninebot_expect = ninebot_expect_speed;
    NinebotPack message;
    static uint8_t send_data_array[NinebotMaxPayload] = {0x0};
    static uint16_t length = 0;

    if (length == 0) {
      if (ninebot_create_request(MastertoM365, Ninebotread, M365speedREG, M365speedLEN + 2, &message) == 0) {
        length = ninebot_serialyze(&message, send_data_array);
        NRF_LOG_HEXDUMP_DEBUG(send_data_array, length);
      }
    }

    uint8_t retries = 3;
    while (ble_nus_c_string_send(&m_ninebot_nus_c, (uint8_t *)&send_data_array, length) != NRF_SUCCESS && m_ninebot_nus_c.conn_handle != BLE_CONN_HANDLE_INVALID && retries-- > 0) {
      // repeat until sent
      NRF_LOG_DEBUG("Looping...\r\n");
    }
//    NRF_LOG_DEBUG("request_speed done\r\n");
  }
}

void request_battery_percentage(void) {
  if (m_ninebot_data.connected) {
    NRF_LOG_DEBUG("request_battery_percentage\r\n");
    NinebotPack message;
    static uint8_t send_data_array[NinebotMaxPayload] = {0x0};
    static uint16_t length = 0;

    if (length == 0) {
      if (ninebot_create_request(MastertoM365, Ninebotread, M365battREG, M365battLEN + 2, &message) == 0) {
        length = ninebot_serialyze(&message, send_data_array);
        NRF_LOG_HEXDUMP_DEBUG(send_data_array, length);
      }
    }

    uint8_t retries = 3;
    while (ble_nus_c_string_send(&m_ninebot_nus_c, (uint8_t *)&send_data_array, length) != NRF_SUCCESS && m_ninebot_nus_c.conn_handle != BLE_CONN_HANDLE_INVALID && retries-- > 0) {
      // repeat until sent
      NRF_LOG_DEBUG("Looping...\r\n");
    }
//    NRF_LOG_DEBUG("request_battery_percentage done\r\n");
  }
}

void request_distance_remaining(void) {
  if (m_ninebot_data.connected) {
    NRF_LOG_DEBUG("request_distance_remaining\r\n");
    NinebotPack message;
    static uint8_t send_data_array[NinebotMaxPayload] = {0x0};
    static uint16_t length = 0;

    if (length == 0) {
      if (ninebot_create_request(MastertoM365, Ninebotread, M365kmremainREG, M365kmremainLEN + 2, &message) == 0) {
        length = ninebot_serialyze(&message, send_data_array);
        NRF_LOG_HEXDUMP_DEBUG(send_data_array, length);
      }
    }
    uint8_t retries = 3;
    while (ble_nus_c_string_send(&m_ninebot_nus_c, (uint8_t *)&send_data_array, length) != NRF_SUCCESS && m_ninebot_nus_c.conn_handle != BLE_CONN_HANDLE_INVALID && retries-- > 0) {
      // repeat until sent
      NRF_LOG_DEBUG("Looping...\r\n");
    }
//    NRF_LOG_DEBUG("request_distance_remaining done\r\n");
  }
}




void handle_ninebot_pack(NinebotPack *pack) {
  bool update = false;
  if (pack->command == M365battREG) {
    NRF_LOG_DEBUG("handle_attribute_data - battery_percentage\r\n");
    uint16_t batt = ((uint16_t)pack->data[1] << 8) + pack->data[0];
    m_ninebot_data.battery_percentage = batt == 0 ? 0.0 : (double)batt /100.0;
    update = true;
  } else if (pack->command == M365speedREG) {
    int16_t speed = ((uint16_t)pack->data[1] << 8) + pack->data[0];
    m_ninebot_data.speed_kph = (double)((double)speed / 1000.0);      
    m_ninebot_data.speed_mph = m_ninebot_data.speed_kph * 0.621371;
    update = true;
    NRF_LOG_DEBUG("handle_attribute_data - speed = %d \r\n", speed);
  } else if (pack->command == M365kmremainREG) {
    uint16_t distance = ((uint16_t)pack->data[1] << 8) + pack->data[0]; //km restantes, ex 123= 1.23km
    m_ninebot_data.distance_remaining_km = (double)distance / 100.0;
    m_ninebot_data.distance_remaining_mi = m_ninebot_data.distance_remaining_km * 0.621371;
    update = true;
    NRF_LOG_DEBUG("handle_attribute_data - distance_remaining\r\n");
  } else {
    NRF_LOG_INFO("handle_attribute_data - other\r\n");
    NRF_LOG_HEXDUMP_INFO(pack->data, pack->len);
  }

  if (update) {
    m_data_callback(&m_ninebot_data);
  }
}


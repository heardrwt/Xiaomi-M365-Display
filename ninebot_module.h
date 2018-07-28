/*
  ninebot_module.h
  
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

#ifndef __NINEBOT_MODULE_H
#define __NINEBOT_MODULE_H

#include <stdint.h>
#include "ble_nus_c.h"

#ifdef __splusplus
extern "C" {
#endif

typedef struct {
  bool connected;
  double speed_kph;             // 0.0 - 32.0
  double speed_mph;             // 0.0 - 20.0
  double battery_percentage;    // 0.0 - 1.0
  double distance_remaining_km; // 0.0 - 30.0
  double distance_remaining_mi; // 0.0 - 22.0
} ninebot_data_t;

typedef void (*ninebot_data_callback_t)(ninebot_data_t *data);

uint32_t ninebot_init(ninebot_data_callback_t data_callback);
uint32_t ninebot_uninit(void);

uint32_t ninebot_get_current_data(ninebot_data_t *data_out);

// nus handlers
uint32_t ninebot_nus_start_polling(ble_nus_c_t *nus_c);
void ninebot_nus_received_data(uint8_t *p_data, uint8_t data_len);
uint32_t ninebot_nus_stop_polling(void);

// fetch updated data
void request_distance_remaining(void);
void request_speed(void);
void request_battery_percentage(void);

#ifdef __splusplus
}
#endif

#endif /* __NINEBOT_MODULE_H */
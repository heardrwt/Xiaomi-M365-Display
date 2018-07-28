#!/usr/local/bin/python2
# -*- mode: python; coding: utf-8 -*-
#
# Read data from Xiaomi M365 Scooter
from gattlib import GATTRequester, GATTResponse
from struct import *
from time import sleep
from binascii import hexlify

MASTER_TO_M365 = 0x20
M365_TO_MASTER = 0x23
BATTERY_TO_MASTER = 0x25

DISTANCE_INFO = 0x25
TRIP_INFO = 0xb0
BATTERY_INFO = 0x31


class Requester(GATTRequester):
    def on_notification(self, handle, data):
        payload = data[3:]
        # print hexlify(payload)
        if len(payload) > 8:
            data_type, address = unpack('<xxxBxB', payload[:6])
            if data_type == M365_TO_MASTER:
                if address == DISTANCE_INFO:
                    distance_left = unpack('<H', payload[6:8])
                    print 'Distance left:', distance_left[0] / 100., 'km'
                elif address == TRIP_INFO:
                    error, warning, flags, workmode, battery, speed, speed_average = unpack('<HHHHHHH', payload[6:20])
                    print 'Current speed:', speed / 100., 'km/h'
                    print 'Average speed:', speed_average / 100., 'km/h'

            elif data_type == BATTERY_TO_MASTER:
                if address == BATTERY_INFO:
                    capacity_left, battery_percent, current, voltage, battery_temperature1, battery_temperature2 \
                        = unpack('<HHhHBB', payload[6:16])
                    print 'Battery capacity:', capacity_left, 'mAh'
                    print 'Battery percentage:', battery_percent, '%'
                    print 'Battery current:', current / 100., 'A'
                    print 'Battery voltage:', voltage / 100., 'V'
                    print 'Battery temperature 1:', battery_temperature1 - 20, 'C'
                    print 'Battery temperature 2:', battery_temperature2 - 20, 'C'

            else:
                # this is a second packet from the 0xb0 request
                total_m, temperature = unpack('<Ixxxxh', payload[:10])
                print 'Total distance:', total_m / 1000., 'km'
                print 'Frame temperature', temperature / 10., 'C'


address = "AA:BB:CC:DD:EE:FF"
requester = Requester(address, False)

try:
    requester.connect(True, 'random')
except RuntimeError as e:
    # gattlib.connect's `wait=True` requires elevated permission
    # or modified capabilities.
    # It still connects, but a RuntimeError is raised. Check if
    # `self.gatt` is connected, and rethrow exception otherwise.
    if not requester.is_connected():
        raise e

# subscribe to the UART service
requester.write_by_handle(0x000c, str(bytearray([0x01, 0x00])))
sleep(0.1)
# speed, distance and lots of other stuff
requester.write_by_handle(0x000e, str(bytearray.fromhex("55aa 03 2001 b0 20 0bff")))
sleep(0.1)
# km left
requester.write_by_handle(0x000e, str(bytearray.fromhex("55aa 03 2001 25 02 b4ff")))
sleep(0.1)
# battery info
requester.write_by_handle(0x000e, str(bytearray.fromhex("55aa 03 2201 31 0a 9eff")))
sleep(0.1)

sleep(1)
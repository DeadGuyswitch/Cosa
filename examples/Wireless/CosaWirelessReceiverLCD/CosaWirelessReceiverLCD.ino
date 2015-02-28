/**
 * @file CosaWirelessReceiverLCD.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * Cosa Wireless interface demo; receiver messages from CosaWirelessSender
 * and others. Display message on LCD.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"

// Configuration; network and device addresses
#define NETWORK 0xC05A
#define DEVICE 0x01

// Select Wireless device driver
// #define USE_CC1101
// #define USE_NRF24L01P
// #define USE_RFM69
#define USE_VWI

#if defined(USE_CC1101)
#include "Cosa/Wireless/Driver/CC1101.hh"
CC1101 rf(NETWORK, DEVICE);

#elif defined(USE_NRF24L01P)
#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(NETWORK, DEVICE);

#elif defined(USE_RFM69)
#include "Cosa/Wireless/Driver/RFM69.hh"
RFM69 rf(NETWORK, DEVICE);

#elif defined(USE_VWI)
#include "Cosa/Wireless/Driver/VWI.hh"
// #include "Cosa/Wireless/Driver/VWI/Codec/BitstuffingCodec.hh"
// BitstuffingCodec codec;
// #include "Cosa/Wireless/Driver/VWI/Codec/Block4B4BCodec.hh"
// BitstuffingCodec codec;
#include "Cosa/Wireless/Driver/VWI/Codec/HammingCodec.hh"
HammingCodec codec;
// #include "Cosa/Wireless/Driver/VWI/Codec/ManchesterCodec.hh"
// ManchesterCodec codec;
// #include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
// VirtualWireCodec codec;
#define BPS 4000
#if defined(BAORD_TINY)
VWI rf(NETWORK, DEVICE, BPS, Board::D1, Board::D0, &codec);
#else
VWI rf(NETWORK, DEVICE, BPS, Board::D7, Board::D8, &codec);
#endif
#endif

// LCD, port and iostream configuration
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
// HD44780::MJKDZ port(0);
// HD44780::MJKDZ port;
// HD44780::GYIICLCD port;
HD44780::DFRobot port;
// HD44780::SainSmart port;
// HD44780::ERM1602_5 port;
// HD44780 lcd(&port, 20, 4);
// HD44780 lcd(&port, 16, 4);
HD44780 lcd(&port);
IOStream cout(&lcd);

void setup()
{
  Watchdog::begin();
  RTC::begin();
  lcd.begin();
  cout << clear << PSTR("CosaWirelessReceiver: started");
  rf.begin();
  sleep(2);
}

static const uint8_t IOSTREAM_TYPE = 0x00;

static const uint8_t PAYLOAD_MAX = 16;
struct payload_msg_t {
  uint8_t nr;
  uint8_t payload[PAYLOAD_MAX];
};
static const uint8_t PAYLOAD_TYPE = 0x01;

IOStream& operator<<(IOStream& outs, payload_msg_t* msg)
{
  outs.print(0L, msg->payload, PAYLOAD_MAX, IOStream::hex);
  return (outs);
}

struct dt_msg_t {
  uint8_t nr;
  int16_t indoors;
  int16_t outdoors;
  uint16_t battery;
};
static const uint8_t DIGITAL_TEMPERATURE_TYPE = 0x02;

IOStream& operator<<(IOStream& outs, dt_msg_t* msg)
{
  DS18B20::print(outs, msg->indoors);
  outs << PSTR(" C,");
  DS18B20::print(outs, msg->outdoors);
  outs << PSTR(" C,");
  outs << msg->battery << PSTR(" mV");
  return (outs);
}

struct dht_msg_t {
  uint8_t nr;
  int16_t humidity;
  int16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_HUMIDITY_TEMPERATURE_TYPE = 0x03;

IOStream& operator<<(IOStream& outs, dht_msg_t* msg)
{
  outs << msg->humidity << PSTR(" %,")
       << msg->temperature << PSTR(" C,")
       << msg->battery << PSTR(" mV");
  return (outs);
}

struct dlt_msg_t {
  uint32_t timestamp;
  uint16_t luminance;
  uint16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_LUMINANCE_TEMPERATURE_TYPE = 0x04;

IOStream& operator<<(IOStream& outs, dlt_msg_t* msg)
{
  outs << msg->luminance << ','
       << msg->temperature << ','
       << msg->battery << PSTR(" mV");
  return (outs);
}

void loop()
{
  // Receive a message
  static uint32_t recs = 0;
  static uint32_t errs = 0;
  const uint32_t TIMEOUT = 10000;
  const uint8_t MSG_MAX = 32;
  uint8_t msg[MSG_MAX];
  uint8_t src;
  uint8_t port;
  int count = rf.recv(src, port, msg, sizeof(msg), TIMEOUT);
  recs += 1;

  // Print the message header
  cout << clear;
  if (count >= 0) {
    // Print the message payload according to port/message type
    switch (port) {
    case IOSTREAM_TYPE:
      for (uint8_t i = 0; i < count; i++)
	if (msg[i] != '\f' && msg[i] != '\n')
	  cout << (char) msg[i];
      break;
    case PAYLOAD_TYPE:
      cout << (payload_msg_t*) msg;
      break;
    case DIGITAL_TEMPERATURE_TYPE:
      cout << (dt_msg_t*) msg;
      break;
    case DIGITAL_HUMIDITY_TEMPERATURE_TYPE:
      cout << (dht_msg_t*) msg;
      break;
    case DIGITAL_LUMINANCE_TEMPERATURE_TYPE:
      cout << (dlt_msg_t*) msg;
      break;
    default:
      cout.print(0L, msg, count, IOStream::hex);
    }
    cout << ' ' << errs << '/' << recs;
    return;
  }

  // Check error codes
  errs += 1;
  if (count == EMSGSIZE) {
    cout << PSTR("error:illegal frame size(-1)");
  }
  else if (count == ETIME) {
    cout << PSTR("error:timeout(-2)");
  }
  else if (count < 0) {
    cout << PSTR("error(") << count << PSTR(")");
  }
}

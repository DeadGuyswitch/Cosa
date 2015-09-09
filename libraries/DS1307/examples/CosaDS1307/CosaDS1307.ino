/**
 * @file CosaDS1307.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Cosa demonstration of the DS1307 I2C/Two-Wire Realtime clock device.
 *
 * @section Circuit
 * @code
 *                       TinyRTC(DS1307)
 *                       +------------+
 * (D2)----------------1-|SQ          |
 *                     2-|DS        DS|-1
 * (A5/SCL)------------3-|SCL      SCL|-2
 * (A4/SDA)------------4-|SDA      SDA|-3
 * (VCC)---------------5-|VCC      VCC|-4
 * (GND)---------------6-|GND      GND|-5
 *                     7-|BAT         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DS1307.h>

#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Set the real-time clock
// #define SET_TIME

// The real-time device, latest start and sample time in ram
DS1307 rtc;

// Data structure stored in device ram; last set and run time
struct latest_t {
  time_t set;
  time_t run;
};

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// Clock pin
InputPin clkPin(Board::D2);

// Watchdog clock
Clock clock;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaDS1307: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog and internal real-time clock
  Watchdog::begin();
  Watchdog::alarm(&clock);
  RTC::begin();

  // Read the latest set and run time
  latest_t latest;
  int count = rtc.read(&latest, sizeof(latest), DS1307::RAM_START);
  ASSERT(count == sizeof(latest));
  latest.set.to_binary();
  latest.run.to_binary();

  // Print latest set and the latest run time
  trace.print(PSTR("set on "));
  trace << latest.set << endl;
  trace.print(PSTR("run on "));
  trace << latest.run << endl;

  // Set the time. Adjust below to your current time
  time_t now;
#if defined(SET_TIME)
  now.year = 0x14;
  now.month = 0x09;
  now.date = 0x07;
  now.day = 0x00;
  now.hours = 0x22;
  now.minutes = 0x52;
  now.seconds = 0x00;
  rtc.set_time(now);
  latest.set = now;
#else
  rtc.get_time(now);
#endif

  // Update the run time with the current time and update ram
  latest.run = now;
  count = rtc.write(&latest, sizeof(latest), DS1307::RAM_START);
  ASSERT(count == sizeof(latest));

  // Print the control register
  ASSERT(rtc.enable());
  DS1307::control_t control;
  uint8_t pos = offsetof(DS1307::timekeeper_t, control);
  count = rtc.read(&control, sizeof(control), pos);
  ASSERT(count == sizeof(control));
  trace << PSTR("control = ") << bin << control << endl;
  trace.flush();

  // Synchronize clocks
  ASSERT(rtc.get_time(now));
  now.to_binary();
  RTC::time(now);
  clock.time(now);
  clock.calibrate(25);
}

void loop()
{
  static int32_t cycle = 1;

  // Wait for rising edge on clock pin
  while (clkPin.is_clear()) yield();
  ledPin.set();

  // Read the time from the rtc device and print
  time_t now;
  ASSERT(rtc.get_time(now));
  now.to_binary();
  clock_t seconds = now;
  int32_t rtc = RTC::time() - seconds;
  int32_t wdg = Watchdog::clock()->time() - seconds;
  trace << cycle << ':' << now
	<< PSTR(":RTC:")
	<< (rtc < 0 ? PSTR("T") : PSTR("T+")) << rtc
	<< PSTR(",err=") << (100.0 * rtc) / cycle << '%'
	<< PSTR(":Watchdog:")
	<< (wdg < 0 ? PSTR("T") : PSTR("T+")) << wdg
	<< PSTR(",err=") << (100.0 * wdg) / cycle << '%'
	<< endl;
  cycle += 1;

  // Wait for falling edge on clock pin
  while (clkPin.is_set()) yield();
  ledPin.clear();
}

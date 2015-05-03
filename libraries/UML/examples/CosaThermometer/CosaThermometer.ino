/**
 * @file CosaThermometer.ino
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
 * Demonstration of the Thermometer (DS18B20) periodic sampling
 * and display on LCD together with simple Clock.
 *
 * @section Diagram
 *
 *    Clock              Display<tick,0,0>
 *  +--------+            +---------+
 *  | clock  |            | display |
 *  |        |---[tick]-->|  tick   |
 *  |        |            |         |
 *  +--------+            +---------+
 *  [1024ms]
 *
 *  Thermometer           Display<temp,0,1>
 *  +--------+            +---------+
 *  | sensor |            | display |
 *  |        |---[temp]-->|  temp   |
 *  |        |            |         |
 *  +--------+            +---------+
 *  [OWI/2048ms]
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <UML.h>
#include <DS18B20.h>
#include <UML/Thermometer.h>

#include "Cosa/LCD/Driver/HD44780.hh"
// HD44780::Port4b port;
// HD44780::SR3W port;
HD44780::SR3WSPI port;
// HD44780::SR4W port;
// HD44780::ERM1602_5 port;
// HD44780::MJKDZ port;
// HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780::SainSmart port;
HD44780 lcd(&port);

using namespace UML;

// Forward declaration of the connectors
extern Clock::Tick tick;
extern Thermometer::Temperature temp;

// One-wire pin
OWI owi(Board::D4);

// The capsules with data dependencies (connectors)
Clock clock(tick, 1024);
Thermometer sensor(&owi, temp);

const char display_tick_prefix[] __PROGMEM = "Clock: ";
const char display_tick_suffix[] __PROGMEM = "";
Display<Clock::Tick, 0, 0>
display_tick(tick, &lcd, (str_P) display_tick_prefix, (str_P) display_tick_suffix);

const char display_temp_prefix[] __PROGMEM = "Indoor:";
const char display_temp_suffix[] __PROGMEM = " C";
Display<Thermometer::Temperature, 0, 1>
display_temp(temp, &lcd, (str_P) display_temp_prefix, (str_P) display_temp_suffix);

// The wiring; control dependencies (capsules)
Capsule* const tick_listeners[] __PROGMEM = { &display_tick, NULL };
Clock::Tick tick(tick_listeners, 0);

Capsule* const temp_listeners[] __PROGMEM = { &display_temp, NULL };
Thermometer::Temperature temp(temp_listeners, 0);

void setup()
{
  // Start the UML run-time
  UML::begin();

  // Start the lcd
  lcd.begin();

  // Connect and start the sensor
  sensor.connect(0);
  sensor.begin();

  // Start the clock
  clock.begin();
}

void loop()
{
  // Service Events and scheduled Capsules
  UML::service();
}

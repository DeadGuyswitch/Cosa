/**
 * @file CosaTrigger.ino
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
 * Count trigger signals.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "UML.hh"

/**
 * @section Diagram
 * 
 *    Trigger              Counter           TimedProbe
 *  +---------+          +---------+         +---------+
 *  | trigger |          | counter |         | probe   | 
 *  |         |---[s1]-->|         |--[c1]-->|         |
 *  |         |          |         |         |         |
 *  +---------+          +---------+         +---------+
 *       [EXT0]
 */

using namespace UML;

// Define Counter type
typedef Counter<Trigger::Signal> Triggers;

// Forward declaration of the connectors
extern Trigger::Signal s1;
extern Triggers::Count c1;

// The capsules with data dependencies (connectors)
Trigger trigger(Board::EXT0, s1);
Triggers counter(s1, c1);

const char probe_name[] __PROGMEM = "probe";
TimedProbe<Triggers::Count> probe((str_P) probe_name, c1);

// The wiring; control dependencies
Capsule* const s1_listeners[] __PROGMEM = { &counter, NULL };
Trigger::Signal s1(s1_listeners, 0);

Capsule* const c1_listeners[] __PROGMEM = { NULL };
Triggers::Count c1(c1_listeners, 0L);

void setup()
{
 // Start trace on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTrigger: started"));

  // Use the watchdog for timeout events
  Watchdog::begin(16, Watchdog::push_timeout_events);

  // Start the timed probe
  probe.begin();
}

void loop()
{
  trigger.enable();
  Event::service();
  controller.run();
}

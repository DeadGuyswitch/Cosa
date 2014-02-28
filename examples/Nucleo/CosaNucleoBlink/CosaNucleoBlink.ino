/**
 * @file CosaNucleoBlink.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Demonstration of Cosa Nucleo Threads; Blink LED with thread
 * delay and use an additional thread to control the LED blink
 * frequency.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Nucleo/Thread.hh"

class LED : public Nucleo::Thread {
private:
  OutputPin m_pin;
  uint16_t m_delay;
public:
  LED(Board::DigitalPin pin) : Thread(), m_pin(pin, 1), m_delay(200) {}
  void set_delay(uint16_t ms) { m_delay = ms; }
  virtual void run();
};

void 
LED::run() 
{ 
  m_pin.toggle(); 
  delay(m_delay);
}

template <uint16_t LOW, uint16_t HIGH, uint16_t INC>
class LEDController : public Nucleo::Thread {
private:
  LED* m_led;
public:
  LEDController(LED* led) : Thread(), m_led(led) {}
  virtual void run();
};

template <uint16_t LOW, uint16_t HIGH, uint16_t INC>
void
LEDController<LOW, HIGH, INC>::run()
{
  for (uint16_t ms = LOW; ms < HIGH; ms += INC) {
    m_led->set_delay(ms);
    delay(1000);
  }
  for (uint16_t ms = HIGH; ms > LOW; ms -= INC) {
    m_led->set_delay(ms);
    delay(1000);
  }
}

LED buildin(Board::LED);
LEDController<20,200,20> controller(&buildin);

void setup()
{
  Watchdog::begin();
  Nucleo::Thread::begin(&buildin, 32);
  Nucleo::Thread::begin(&controller, 32);
}

void loop()
{
  Nucleo::Thread::begin();
}

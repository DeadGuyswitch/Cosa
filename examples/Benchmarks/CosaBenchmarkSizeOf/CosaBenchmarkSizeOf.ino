/**
 * @file CosaBenchmarkSizeOf.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Cosa sizeof benchmark; the size of class instances. Static member
 * data are not included. Note that this is also a list of the classes
 * available in Cosa.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#define TRACE_NO_VERBOSE

#include "Cosa/Memory.h"

#include "Cosa/BitSet.hh"
#include "Cosa/Button.hh"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Driver/ST7735.hh"
#include "Cosa/Canvas/Element/Textbox.hh"
#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/OffScreen.hh"
#include "Cosa/EEPROM.hh"
#include "Cosa/Event.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/FSM.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/INET.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/CDC.hh"
#include "Cosa/IOStream/Driver/RS485.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/IOStream/Driver/WIO.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/Keypad.hh"
#include "Cosa/LCD.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/LCD/Driver/PCD8544.hh"
#include "Cosa/LCD/Driver/ST7565.hh"
#include "Cosa/LCD/Driver/VLCD.hh"
#include "Cosa/LED.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Listener.hh"
#include "Cosa/Menu.hh"
#include "Cosa/OWI.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/Pin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/PWMPin.hh"
#include "Cosa/IOPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/AnalogPins.hh"
#include "Cosa/AnalogComparator.hh"
#include "Cosa/Queue.hh"
#include "Cosa/Registry.hh"
#include "Cosa/Rotary.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Servo.hh"
#include "Cosa/Socket.hh"
#include "Cosa/SPI.hh"
#include "Cosa/SPI/Driver/SD.hh"
#include "Cosa/ProtoThread.hh"
#include "Cosa/Time.hh"
#include "Cosa/Timer.hh"
#include "Cosa/Touch.hh"
#include "Cosa/Trace.hh"
#include "Cosa/TWI.hh"
#include "Cosa/Wireless.hh"
#include "Cosa/Wireless/Driver/CC1101.hh"
#include "Cosa/Wireless/Driver/NRF24L01P.hh"
#include "Cosa/Wireless/Driver/RFM69.hh"
#include "Cosa/Wireless/Driver/VWI.hh"

void setup()
{
  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkSizeOf: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(BitSet<64>));
  TRACE(sizeof(Button));
  TRACE(sizeof(Canvas));
  TRACE(sizeof(ST7735));
  TRACE(sizeof(Textbox));
  TRACE(sizeof(OffScreen<64,128>));
#if defined(USBCON)
  TRACE(sizeof(CDC));
#endif
  TRACE(sizeof(EEPROM));
  TRACE(sizeof(Event));
  TRACE(sizeof(Event::queue));
  TRACE(sizeof(ExternalInterrupt));
  TRACE(sizeof(FSM));
  TRACE(sizeof(Interrupt::Handler));
  TRACE(sizeof(IOBuffer<64>));
  TRACE(sizeof(IOStream));
#if !defined(BOARD_ATTINY)
  TRACE(sizeof(RS485));
  TRACE(sizeof(UART));
#endif
  TRACE(sizeof(WIO));
  TRACE(sizeof(Keypad));
  TRACE(sizeof(LCDKeypad));
  TRACE(sizeof(LCD));
  TRACE(sizeof(HD44780));
  TRACE(sizeof(PCD8544));
  TRACE(sizeof(ST7565));
  TRACE(sizeof(VLCD));
  TRACE(sizeof(LED));
  TRACE(sizeof(Linkage));
  TRACE(sizeof(Link));
  TRACE(sizeof(Head));
  TRACE(sizeof(Listener<int>));
  TRACE(sizeof(Menu));
  TRACE(sizeof(Menu::Action));
  TRACE(sizeof(Menu::Walker));
  TRACE(sizeof(Menu::KeypadController));
  TRACE(sizeof(Menu::RotaryController));
  TRACE(sizeof(OWI));
  TRACE(sizeof(OWI::Driver));
  TRACE(sizeof(OWI::Search));
  TRACE(sizeof(Periodic));
  TRACE(sizeof(Pin));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(IOPin));
  TRACE(sizeof(PWMPin));
  TRACE(sizeof(AnalogPin));
  TRACE(sizeof(AnalogPins));
  TRACE(sizeof(AnalogComparator));
  TRACE(sizeof(Queue<int,32>));
  TRACE(sizeof(Registry));
  TRACE(sizeof(Rotary::Encoder));
  TRACE(sizeof(Rotary::Dial<int>));
  TRACE(sizeof(Rotary::AcceleratedDial<int, 900>));
  TRACE(sizeof(RTC));
  TRACE(sizeof(Servo));
  TRACE(sizeof(Socket));
  TRACE(sizeof(SPI::Driver));
  TRACE(sizeof(SPI::Slave));
  TRACE(sizeof(SD));
  TRACE(sizeof(ProtoThread));
  TRACE(sizeof(clock_t));
  TRACE(sizeof(time_t));
  TRACE(sizeof(Timer));
  TRACE(sizeof(Touch));
  TRACE(sizeof(Trace));
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(TWI::Slave));
  TRACE(sizeof(Watchdog));
  TRACE(sizeof(Wireless::Driver));
#if !defined(BOARD_ATTINY)
  TRACE(sizeof(CC1101));
  TRACE(sizeof(NRF24L01P));
  TRACE(sizeof(RFM69));
#endif
  TRACE(sizeof(VWI));
}

void loop()
{
  ASSERT(true == false);
}

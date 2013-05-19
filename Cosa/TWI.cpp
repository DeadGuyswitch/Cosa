/**
 * @file Cosa/TWI.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#if !defined(__ARDUINO_TINY__)
#include "Cosa/TWI.hh"
#include "Cosa/Bits.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"

/**
 * Default Two-Wire Interface clock: 100 KHz
 */
#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

TWI twi;

bool 
TWI::begin(Event::Handler* target, uint8_t addr)
{
  m_target = target;
  m_addr = addr;

  // Check for slave mode and set device address
  if (addr != 0) {
    if (target == 0) return (false);
    TWAR = m_addr;
  } 
  else {
    // Enable internal pullup
    synchronized {
      bit_set(PORTC, Board::SDA);
      bit_set(PORTC, Board::SCL);
    }
  }
  
  // Set clock prescale and bit rate
  bit_clear(TWSR, TWPS0);
  bit_clear(TWSR, TWPS1);
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
  TWCR = IDLE_CMD;
  return (true);
}

bool 
TWI::end()
{
  m_target = 0;
  TWCR = 0;
  TWAR = 0;
  return (true);
}

bool
TWI::request(uint8_t addr)
{
  m_addr = addr;
  m_state = (addr & WRITE_OP) ? MT_STATE : MR_STATE;
  m_status = NO_INFO;
  m_next = (uint8_t*) m_vec[0].buf;
  m_last = m_next + m_vec[0].size;
  m_ix = 0;
  m_count = 0;
  TWCR = START_CMD;
  return (true);
}

bool
TWI::write_request(uint8_t addr, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | WRITE_OP));
}

bool
TWI::write_request(uint8_t addr, uint8_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | WRITE_OP));
}

bool
TWI::write_request(uint8_t addr, uint16_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = (header >> 8);
  m_header[1] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | WRITE_OP));
}

bool
TWI::read_request(uint8_t addr, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | READ_OP));
}

int
TWI::await_completed(uint8_t mode)
{
  while (m_state > IDLE_STATE) Power::sleep(mode);
  return (m_count);
}

ISR(TWI_vect) 
{
  twi.m_status = TWI_STATUS(TWSR);
  switch (twi.m_status) {
    /**
     * Transaction Start Mode
     */
  case TWI::START:
  case TWI::REP_START:
    TWDR = twi.m_addr;
    TWCR = TWI::DATA_CMD;
    break;
  case TWI::ARB_LOST:
    TWCR = TWI::IDLE_CMD;
    twi.m_state = TWI::ERROR_STATE;
    twi.m_count = -1;
    break;
    
    /**
     * Master Transmitter Mode
     */
  case TWI::MT_SLA_ACK:
  case TWI::MT_DATA_ACK:
    if (twi.m_next == twi.m_last) {
      twi.m_ix += 1;
      twi.m_next = (uint8_t*) twi.m_vec[twi.m_ix].buf;
      twi.m_last = twi.m_next + twi.m_vec[twi.m_ix].size;
    }
    if (twi.m_next < twi.m_last) {
      TWDR = *twi.m_next++;
      TWCR = TWI::DATA_CMD;
      twi.m_count += 1;
      break;
    } 
  case TWI::MT_DATA_NACK: 
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_state = TWI::IDLE_STATE;
    break;
  case TWI::MT_SLA_NACK: 
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_count = -1;
    twi.m_state = TWI::ERROR_STATE;
    break;

    /**
     * Master Receiver Mode
     */
  case TWI::MR_DATA_ACK:
    *twi.m_next++ = TWDR;
    twi.m_count += 1;
  case TWI::MR_SLA_ACK:
    TWCR = (twi.m_next < (twi.m_last - 1)) ? TWI::ACK_CMD : TWI::NACK_CMD;
    break; 
  case TWI::MR_DATA_NACK:
    *twi.m_next++ = TWDR;
    twi.m_count += 1;
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_state = TWI::IDLE_STATE;
    break;
  case TWI::MR_SLA_NACK: 
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_count = -1;
    twi.m_state = TWI::ERROR_STATE;
    break;

    /**
     * Slave Transmitter Mode
     */
  case TWI::ST_SLA_ACK:
  case TWI::ST_ARB_LOST_SLA_ACK:
    twi.m_next = (uint8_t*) twi.m_vec[TWI::Device::READ_IX].buf;
    twi.m_last = twi.m_next + twi.m_vec[TWI::Device::READ_IX].size;
    twi.m_count = 0;
    twi.m_state = TWI::ST_STATE;
  case TWI::ST_DATA_ACK:
    if (twi.m_next < twi.m_last) {
      TWDR = *twi.m_next++;
      TWCR = TWI::ACK_CMD;
      twi.m_count += 1;
    } else {
      TWDR = 255;
      TWCR = TWI::NACK_CMD;
    }
    break;
  case TWI::ST_DATA_NACK:
  case TWI::ST_LAST_DATA:
    TWCR = TWI::ACK_CMD;
    twi.m_state = TWI::IDLE_STATE;
    break;

    /**
     * Slave Receiver Mode
     */
  case TWI::SR_SLA_ACK:
  case TWI::SR_GCALL_ACK:
  case TWI::SR_ARB_LOST_SLA_ACK:
  case TWI::SR_ARB_LOST_GCALL_ACK:
    twi.m_next = (uint8_t*) twi.m_vec[TWI::Device::READ_IX].buf;
    twi.m_last = twi.m_next + twi.m_vec[TWI::Device::READ_IX].size;
    twi.m_count = 0;
    TWCR = TWI::ACK_CMD;
    twi.m_state = TWI::SR_STATE;
    break;
  case TWI::SR_DATA_ACK:
  case TWI::SR_GCALL_DATA_ACK:
    if (twi.m_next < twi.m_last) {
      *twi.m_next++ = TWDR;
      twi.m_count += 1;
      TWCR = TWI::ACK_CMD;
    }
    else {
      TWCR = TWI::NACK_CMD;
    }
    break;
  case TWI::SR_STOP:
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_state = TWI::IDLE_STATE;
    TWAR = 0;
    Event::push(Event::WRITE_COMPLETED_TYPE, twi.m_target, twi.m_count);
    break;
  case TWI::SR_DATA_NACK:
  case TWI::SR_GCALL_DATA_NACK:
    TWCR = TWI::NACK_CMD;
    break;

  case TWI::NO_INFO:
    break;

  case TWI::BUS_ERROR: 
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_count = -1;
    twi.m_state = TWI::ERROR_STATE;
    break;
    
  default:     
    TWCR = TWI::IDLE_CMD; 
  }
}

void 
TWI::Device::on_event(uint8_t type, uint16_t value)
{
  if (type != Event::WRITE_COMPLETED_TYPE) return;
  void* buf = twi.m_vec[WRITE_IX].buf;
  size_t size = value;
  on_request(buf, size);
  TWAR = twi.m_addr;
}

void 
TWI::Device::set_write_buf(void* buf, size_t size)
{
  twi.m_vec[WRITE_IX].buf = buf;
  twi.m_vec[WRITE_IX].size = size;
}

void 
TWI::Device::set_read_buf(void* buf, size_t size)
{
  twi.m_vec[READ_IX].buf = buf;
  twi.m_vec[READ_IX].size = size;
}

#endif

/**
 * @file Cosa/Canvas/Driver/ILI9341.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_CANVAS_DRIVER_ILI9341_HH
#define COSA_CANVAS_DRIVER_ILI9341_HH

#include "Cosa/Canvas.hh"
#include "Cosa/SPI.hh"
#include "Cosa/OutputPin.hh"

/**
 * Device driver for ILI9341, TFT LCD Single Chip Driver,
 * 240x320 Resolution and max 262K color. The device driver uses
 * 16-bit color. See Canvas.
 * 
 * @section Circuit
 * Please note that 3V3 level signals are required. The reset signal
 * is optional.
 *
 * @code
 *                           ILI9341
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (SS/D10)------------3-|CS          |
 * (RST*)--------------4-|RST         |
 * (D9)----------------5-|DC          |
 * (MOSI/D11)----------6-|SDI         |
 * (SCK/D13)-----------7-|SCK         |
 * (VCC)------[330]----8-|LED         |
 * (MISO/D12)----------9-|SDO         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. ILITEK. ILI9341 specification, V1.13, 2011-07-20.
 *
 * @section Acknowledgements
 * Inspired by graphics library by ladyada/adafruit.
 *
 */
class ILI9341 : public Canvas, SPI::Driver {
public:
  /**
   * Construct ILI9341 canvas object with given control pins.
   * @param[in] cs slave selection pin (default pin 10).
   * @param[in] dc data/command selection pin (default pin 9).
   */
#if defined(BOARD_ATTINYX4)
  ILI9341(Board::DigitalPin cs = Board::D3, 
	 Board::DigitalPin dc = Board::D7);
#elif defined(BOARD_ATTINYX5)
  ILI9341(Board::DigitalPin cs = Board::D3, 
	 Board::DigitalPin dc = Board::D4);
#else
  ILI9341(Board::DigitalPin cs = Board::D10, 
	 Board::DigitalPin dc = Board::D9);
#endif

  /**
   * @override Canvas
   * Start interaction with device.
   * @return true(1) if successful otherwise false(0).
   */
  virtual bool begin();

  /**
   * @override Canvas
   * Set screen orientation.
   * @param[in] direction.
   */
  virtual uint8_t set_orientation(uint8_t direction);

  /**
   * @override Canvas
   * Set pixel with current color.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void draw_pixel(uint16_t x, uint16_t y);

  /**
   * @override Canvas
   * Draw vertical line with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length.
   */
  virtual void draw_vertical_line(uint16_t x, uint16_t y, uint16_t length);

  /**
   * @override Canvas
   * Draw horizontal line with current color.
   * @param[in] x.
   * @param[in] y.
   * @param[in] length.
   */
  virtual void draw_horizontal_line(uint16_t x, uint16_t y, uint16_t length);

  /**
   * @override Canvas
   * Fill rectangle with current color.
   * @param[in] x.
   * @param[in] y.
   * @param[in] width.
   * @param[in] height.
   */
  virtual void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
  
  /**
   * @override Canvas
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0).
   */
  virtual bool end();

protected:
  /**
   * Data/Command select pin (default is pin 9).
   */
  OutputPin m_dc;

  /**
   * Initialization script (in program memory).
   */
  static const uint8_t script[] PROGMEM;
  bool m_initiated;

  /**
   * SPI commands (ch. 8 Command, pp. 83-88).
   */
  enum Command {
    NOP = 0x0,			// No Operation
    SWRESET = 0x01,		// Software Reset
    SWDELAY = 0x02,		// Software Delay
    SCRIPTEND = 0x03,		// Script End
    RDDIDIF = 0x04,		// Read Display Identification Information
    RDDST = 0x09,		// Read Display Status
    RDDPM = 0x0A,		// Read Display Power Mode
    RDDMADCTL = 0x0B,		// Read Display MADCTL
    RDDCOLMOD = 0x0C,		// Read Display Pixel Format
    RDDIM = 0x0D,		// Read Display Image Mode
    RDDSM = 0x0E,		// Read Display Signal Mode
    RDDSDR = 0x0F,		// Read Display Self-Diagnostic Result
    SLPIN = 0x10,		// Enter Sleep Mode
    SLPOUT = 0x11,		// Exit Sleep Mode
    PTLON = 0x12,		// Partial Mode On
    NORON = 0x13,		// Normal Display Mode On
    DINVOFF = 0x20,		// Display Inversion Off
    DINVON = 0x21,		// Display Inversion On
    GAMSET = 0x26,		// Gamma Set
    DISPOFF = 0x28,		// Display Off
    DISPON = 0x29,		// Display On
    CASET = 0x2A,		// Column Address Set
    PASET = 0x2B,		// Page Address Set
    RAMWR = 0x2C,		// Memory Write
    RGBSET = 0x2D,		// Color Setting for 4K, 65K and 262K
    RAMRD = 0x2E,		// Memory Read
    PTLAR = 0x30,		// Partial Area
    VSCRDEF = 0x33,		// Vertical Scrolling Definition
    TEOFF = 0x34,		// Tearing Effect Line Off
    TEON = 0x35,		// Tearing Effect Line On
    MADCTL = 0x36,		// Memory Data Access Control
    VSCRSADD = 0x37,		// Vertical Scrolling Start Address
    IDMOFF = 0x38,		// Idle Mode Off
    IDMON = 0x39,		// Idle Mode On
    PIXSET = 0x3A,		// Pixel Format Set
    RAMWRC = 0x3C,		// Write Memory Continue
    RAMRDC = 0x3E,		// Read Memory Continue
    SETSCANLINE = 0x44,		// Set Tear Scanline
    GETSCANLINE = 0x45,		// Get Scanline
    WRDISBV = 0x51,		// Write Display Brightness Value
    RDDISBV = 0x52,		// Read Display Brightness Value
    WRCTRLD = 0x53,		// Write CTRL Display
    RDCTRLD = 0x54,		// Read CTRL Display
    WRCABC = 0x55,		// Write Content Adaptive Brightness Control
    RDCABC = 0x56,		// Read Content Adaptive Brightness Control
    WRCABCMIN = 0x5E,		// Write CABC Minimum Brightness
    RDCABCMIN = 0x5F,		// Read CABC Minimum Brightness
    RDID1 = 0xDA,		// Read ID1 Value
    RDID2 = 0xDB,		// Read ID2 Value
    RDID3 = 0xDC,		// Read ID3 Value
    IFMODE = 0xB0,		// Interface Mode Control
    FRMCTR1 = 0xB1,		// Frame Rate Control, normal mode
    FRMCTR2 = 0xB2,		// Frame Rate Control, idle mode
    FRMCTR3 = 0xB3,		// Frame Rate Control, partial mode
    INVTR = 0xB4,		// Display Inversion Control
    PRCTR = 0xB5,		// Blanking Porch Control
    DISCTRL = 0xB6,		// Display Function Control
    ETMOD = 0xB7,		// Entry Mode Set
    BLCTRL1 = 0xB8,		// Backlight Control 1
    BLCTRL2 = 0xB9,		// Backlight Control 2
    BLCTRL3 = 0xBA,		// Backlight Control 3
    BLCTRL4 = 0xBB,		// Backlight Control 4
    BLCTRL5 = 0xBC,		// Backlight Control 5
    BLCTRL6 = 0xBD,		// Backlight Control 6
    BLCTRL7 = 0xBE,		// Backlight Control 7
    BLCTRL8 = 0xBF,		// Backlight Control 8
    PWCTRL1 = 0xC0,		// Power Control 1
    PWCTRL2 = 0xC1,		// Power Control 2
    VMCTRL1 = 0xC5,		// VCOM Control 1
    VMCTRL2 = 0xC7,		// VCOM Control 2
    NVMWR = 0xD0,		// NV Memory Write
    NVMPKEY = 0xD1,		// NV Memory Protection Key
    RDNVM = 0xD2,		// NV Memory Status Read
    RDID4 = 0xD3,		// Read ID4
    PGAMCTRL = 0xE0,		// Positive Gamma Correction
    NGAMCTRL = 0xE1,		// Negative Gamma Correction
    DGAMCTRL1 = 0xE2,		// Digital Gamma Control 1
    DGAMCTRL2 = 0xE3,		// Digital Gamma Control 2
    IFCTRL = 0xF6,		// Interface Control
    PWCTRLA = 0xCB,		// Power Control A
    PWCTRLB = 0xCF,		// Power Control B
    DTCTRLA = 0xE8,		// Driver Timing Control A
    DTCTRLB = 0xEA,		// Driver Timing Control B
    PWONCTRL = 0xED,		// Power On Sequence Control
    EN3GAM = 0xF2,		// Enable/Disable 3-Gamma
    PRCTRL = 0xF7		// Pump Ratio Control
  } __attribute__((packed));

  /**
   *  Memory Data Access Control (bits).
   */
  enum {
    MADCTL_MH = 0x04,		// Horizontal Refresh order
    MADCTL_ML = 0x10,		// Vertical Refresh order
    MADCTL_MV = 0x20,		// Row / Column Exchange
    MADCTL_MX = 0x40,		// Column Address order
    MADCTL_MY = 0x80,		// Row Address order
    MADCTL_BGR = 0x08,		// BGR order
    MADCTL_RGB = 0x00		// RGB order
  } __attribute__((packed));

  /**
   * Screen size.
   */
  static const uint16_t SCREEN_WIDTH = 240;
  static const uint16_t SCREEN_HEIGHT = 320;

  /**
   * Write 16-bit data to device, MSB first.
   * @param[in] data to write.
   */
  void write(uint16_t data)
    __attribute__((always_inline))
  {
    spi.transfer(data >> 8);
    spi.transfer(data);
  }

  /**
   * Write command to device.
   * @param[in] cmd command to write.
   */
  void write(Command cmd)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
  }

  /**
   * Write command and 8-bit data to device.
   * @param[in] cmd command to write.
   * @param[in] data to write.
   */
  void write(Command cmd, uint8_t data)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer(data);
  }

  /**
   * Write command and 16-bit data to device.
   * @param[in] cmd command to write.
   * @param[in] data to write.
   */
  void write(Command cmd, uint16_t data)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer(data >> 8);
    spi.transfer(data);
  }

  /**
   * Write command and 2x16-bit data to device.
   * @param[in] cmd command to write.
   * @param[in] x data to write.
   * @param[in] y data to write.
   */
  void write(Command cmd, uint16_t x, uint16_t y)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer(x >> 8);
    spi.transfer(x);
    spi.transfer(y >> 8);
    spi.transfer(y);
  }
};

#endif


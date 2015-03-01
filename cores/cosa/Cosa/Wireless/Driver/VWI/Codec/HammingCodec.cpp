/**
 * @file Cosa/VWI/Codec/HammingCodec.cpp
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

#include "Cosa/Wireless/Driver/VWI/Codec/HammingCodec.hh"

// Encoding table; 4-bit code to 8-bit symbol.
const uint8_t HammingCodec::symbols[] __PROGMEM = {
  0x0f, 0x18, 0x24, 0x33, 0x42, 0x55, 0x69, 0x7e, 0x81, 0x96, 0xaa, 0xbd, 0xcc, 0xdb, 0xe7, 0xf0
};

#if defined(HAMMING_SYNDROME)

// Decoding table with syndrome; 8-bit symbol to 4-bit code (LSB) and 4-bit syndrome (MSB)
const uint8_t HammingCodec::codes[256] __PROGMEM = {
  0xf0, 0xe8, 0xd4, 0xc0, 0xb2, 0xa0, 0x90, 0x80, 0x71, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00,
  0x81, 0x91, 0xa1, 0xb3, 0xc1, 0xd5, 0xe9, 0xf1, 0x01, 0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x70,
  0x42, 0x52, 0x62, 0x73, 0x02, 0x12, 0x22, 0x32, 0xc2, 0xd6, 0xea, 0xf2, 0x82, 0x92, 0xa2, 0xb0,
  0x33, 0x23, 0x13, 0x03, 0x72, 0x63, 0x53, 0x43, 0xb1, 0xa3, 0x93, 0x83, 0xf3, 0xeb, 0xd7, 0xc3,
  0x24, 0x34, 0x04, 0x14, 0x64, 0x75, 0x44, 0x54, 0xa4, 0xb6, 0x84, 0x94, 0xec, 0xf4, 0xc4, 0xd0,
  0x55, 0x45, 0x74, 0x65, 0x15, 0x05, 0x35, 0x25, 0xd1, 0xc5, 0xf5, 0xed, 0x95, 0x85, 0xb7, 0xa5,
  0x96, 0x86, 0xb4, 0xa6, 0xd2, 0xc6, 0xf6, 0xee, 0x16, 0x06, 0x36, 0x26, 0x56, 0x46, 0x77, 0x66,
  0xef, 0xf7, 0xc7, 0xd3, 0xa7, 0xb5, 0x87, 0x97, 0x67, 0x76, 0x47, 0x57, 0x27, 0x37, 0x07, 0x17,
  0x18, 0x08, 0x38, 0x28, 0x58, 0x48, 0x79, 0x68, 0x98, 0x88, 0xba, 0xa8, 0xdc, 0xc8, 0xf8, 0xe0,
  0x69, 0x78, 0x49, 0x59, 0x29, 0x39, 0x09, 0x19, 0xe1, 0xf9, 0xc9, 0xdd, 0xa9, 0xbb, 0x89, 0x99,
  0xaa, 0xb8, 0x8a, 0x9a, 0xe2, 0xfa, 0xca, 0xde, 0x2a, 0x3a, 0x0a, 0x1a, 0x6a, 0x7b, 0x4a, 0x5a,
  0xdf, 0xcb, 0xfb, 0xe3, 0x9b, 0x8b, 0xb9, 0xab, 0x5b, 0x4b, 0x7a, 0x6b, 0x1b, 0x0b, 0x3b, 0x2b,
  0xcc, 0xd8, 0xe4, 0xfc, 0x8c, 0x9c, 0xac, 0xbe, 0x4c, 0x5c, 0x6c, 0x7d, 0x0c, 0x1c, 0x2c, 0x3c,
  0xbf, 0xad, 0x9d, 0x8d, 0xfd, 0xe5, 0xd9, 0xcd, 0x3d, 0x2d, 0x1d, 0x0d, 0x7c, 0x6d, 0x5d, 0x4d,
  0x7f, 0x6e, 0x5e, 0x4e, 0x3e, 0x2e, 0x1e, 0x0e, 0xfe, 0xe6, 0xda, 0xce, 0xbc, 0xae, 0x9e, 0x8e,
  0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7e, 0x8f, 0x9f, 0xaf, 0xbd, 0xcf, 0xdb, 0xe7, 0xff
};

#else

// Decoding table without syndrome; 8-bit symbol to 4-bit code; nibble vector.
const uint8_t HammingCodec::codes[128] __PROGMEM = {
  0x08, 0x40, 0x20, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x11, 0x13, 0x15, 0x91, 0x11, 0x11, 0x11, 0x10,
  0x22, 0x23, 0x22, 0x22, 0x26, 0xa2, 0x22, 0x20,
  0x33, 0x33, 0x23, 0x33, 0x13, 0x33, 0x3b, 0x73,
  0x44, 0x44, 0x45, 0x44, 0x46, 0x44, 0xc4, 0x40,
  0x55, 0x45, 0x55, 0x55, 0x15, 0x5d, 0x55, 0x75,
  0x66, 0x46, 0x26, 0x6e, 0x66, 0x66, 0x66, 0x76,
  0xf7, 0x73, 0x75, 0x77, 0x76, 0x77, 0x77, 0x77,
  0x88, 0x88, 0x88, 0x98, 0x88, 0xa8, 0xc8, 0x80,
  0x98, 0x99, 0x99, 0x99, 0x19, 0x9d, 0x9b, 0x99,
  0xa8, 0xaa, 0x2a, 0xae, 0xaa, 0xaa, 0xab, 0xaa,
  0xfb, 0xb3, 0xbb, 0x9b, 0xbb, 0xab, 0xbb, 0xbb,
  0xc8, 0x4c, 0xcc, 0xce, 0xcc, 0xcd, 0xcc, 0xcc,
  0xfd, 0xdd, 0xd5, 0x9d, 0xdd, 0xdd, 0xcd, 0xdd,
  0xfe, 0xee, 0xee, 0xee, 0xe6, 0xae, 0xce, 0xee,
  0xff, 0xff, 0xff, 0xfe, 0xff, 0xfd, 0xfb, 0x7f
};

#endif

const uint8_t HammingCodec::preamble[] __PROGMEM = {
  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x5a
};

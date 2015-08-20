/**
 * @file Debug.cpp
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

#include "Debug.hh"

Debug debug __attribute__((weak));

extern int __heap_start, *__brkval;

bool
Debug::begin(IOStream::Device* dev,
	     const char* file,
	     int line,
	     const char* func)
{
  if (m_dev != NULL) return (false);

  DATAEND =(int) &__heap_start;
  DATASIZE = DATAEND - RAMSTART;

  set_device(dev);
  print(PSTR("Cosa/Debug 1.0 Copyright (C) 2015\n"));
  print(PSTR("Debug::begin:"));
  run(file, line, func);
  return (true);
}

bool
Debug::end()
{
  if (m_dev == NULL) return (false);
  set_device(NULL);
  return (true);
}

void
Debug::run(const char* file, int line, const char* func, str_P expr)
{
  if (func != NULL) printf(PSTR(":%s:%d"), func, line);
  if (expr != NULL) printf(PSTR(":%S"), expr);
  if (func != NULL) println();

  uint16_t marker = 0xA5A5;
  str_P prompt = PSTR("Debug> ");

  while (1) {
    const size_t BUF_MAX = 32;
    char buf[BUF_MAX];
    memset(buf, 0, sizeof(buf));

    // Read command line and parse
    print(prompt);
    while (readline(buf, sizeof(buf)) == NULL) yield();
    size_t len = strlen(buf) - 1;
    if (buf[len] != '\n') continue;
    if (len == 0) continue;
    buf[len] = 0;

    if (!strcmp_P(buf, PSTR("go"))) return;

#if !defined(COSA_DEBUG_NO_DUMP_VARIABLES)
    if (!strcmp_P(buf, PSTR("variables"))) {
      do_dump_variables();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_DUMP_DATA)
    if (!strcmp_P(buf, PSTR("data"))) {
      do_dump_data();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_DUMP_HEAP)
    if (!strcmp_P(buf, PSTR("heap"))) {
      do_dump_heap();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_DUMP_STACK)
    if (!strcmp_P(buf, PSTR("stack"))) {
      do_dump_stack((int) &marker);
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
    if (!strcmp_P(buf, PSTR("memory"))) {
      do_memory_usage((int) &marker);
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_HELP)
    if (!strcmp_P(buf, PSTR("help"))) {
      do_help();
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_EXIT)
    if (!strcmp_P(buf, PSTR("exit"))) {
      print(EXITCHARACTER);
      exit(0);
    }
#endif

#if !defined(COSA_DEBUG_NO_WHERE)
    if (!strcmp_P(buf, PSTR("where"))) {
      printf(PSTR("%s:%d:%s\n"), file, line, func);
      continue;
    }
#endif

#if !defined(COSA_DEBUG_NO_LOOKUP_VARIABLES)
    if (!do_lookup_variables(buf)) {
      printf(PSTR("%s: unknown command or variable\n"), buf);
    }
#endif
  }
}

#if !defined(COSA_DEBUG_NO_LOOKUP_VARIABLES)
bool
Debug::do_lookup_variables(const char* name)
{
  bool found = false;
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) {
    if (strcmp_P(name, vp->m_name) == 0) {
      vp->print();
      found = true;
    }
  }
  return (found);
}
#endif

#if !defined(COSA_DEBUG_NO_DUMP_VARIABLES)
void
Debug::do_dump_variables()
{
  for (Variable* vp = m_var; vp != NULL; vp = vp->m_next) vp->print();
}
#endif

#if !defined(COSA_DEBUG_NO_DUMP_DATA)
void
Debug::do_dump_data()
{
  print(RAMSTART, (void*) RAMSTART, DATASIZE, IOStream::hex);
}
#endif

#if !defined(COSA_DEBUG_NO_DUMP_HEAP)
void
Debug::do_dump_heap()
{
  int HEAPSTART = (int) &__heap_start;
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  int size = HEAPEND - HEAPSTART;
  if (size == 0) return;
  print(HEAPSTART, (void*) HEAPSTART, size, IOStream::hex);
}
#endif

#if !defined(COSA_DEBUG_NO_DUMP_STACK)
void
Debug::do_dump_stack(int marker)
{
  int size = RAMEND - marker;
  if (size == 0) return;
  print(marker, (void*) marker, size, IOStream::hex);
}
#endif

#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
void
Debug::do_memory_usage(int marker)
{
  int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  int heap = HEAPEND - (int) &__heap_start;
  int stack = RAMEND - marker;
  int free = marker - HEAPEND;
  printf(PSTR("data=%d,"), DATASIZE);
  printf(PSTR("heap=%d,"), heap);
  printf(PSTR("stack=%d,"), stack);
  printf(PSTR("free=%d\n"), free);
}
#endif

#if !defined(COSA_DEBUG_NO_HELP)
void
Debug::do_help()
{
  static const char help[] PROGMEM =
    "go -- return to sketch\n"
#if !defined(COSA_DEBUG_NO_EXIT)
    "exit -- exit monitor\n"
#endif
#if !defined(COSA_DEBUG_NO_WHERE)
    "where -- location in source code\n"
#endif
#if !defined(COSA_DEBUG_NO_DUMP_REGISTER)
    "variables -- list variables\n"
#endif
#if !defined(COSA_DEBUG_NO_DUMP_DATA)
    "data -- print data\n"
#endif
#if !defined(COSA_DEBUG_NO_DUMP_HEAP)
    "heap -- print heap\n"
#endif
#if !defined(COSA_DEBUG_NO_DUMP_STACK)
    "stack -- print stack\n"
#endif
#if !defined(COSA_DEBUG_NO_MEMORY_USAGE)
    "memory -- print memory usage\n"
#endif
#if !defined(COSA_DEBUG_NO_LOOKUP_VARIABLES)
    "VARIABLE -- print variable(s)\n";
#endif
  print((str_P) help);
}

void
Debug::Variable::print()
{
  debug.printf(PSTR("%s:%S@%p"), m_func, m_name, m_ref);
  if (m_size == 1) {
    debug.printf(PSTR("=%d\n"), *((uint8_t*) m_ref));
  }
  else if (m_size == 2) {
    debug.printf(PSTR("=%d\n"), *((int*) m_ref));
  }
  else {
    debug.printf(PSTR("[%d]:"), m_size);
    debug.print((uint32_t) m_ref, m_ref, m_size, IOStream::hex);
  }
}

#endif

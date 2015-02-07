/**
 * @file Cosa/UML/Connector.hh
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

#ifndef COSA_UML_CONNECTOR_HH
#define COSA_UML_CONNECTOR_HH

#include "Cosa/UML/Controller.hh"

namespace UML {

/**
 * The Connector class for a given type. Implements value set and
 * get. The setting of the connector value will force the scheduling
 * of capsules that are listening for value change. The template
 * parameter ON_CHANGE can be used to filter on new values. The
 * default is scheduling on any setting. By giving ON_CHANGE the value
 * true the connector will only schedule if the value setting is
 * different than the previous value. 

 * @param[in] T connector value type.
 * @param[in] ON_CHANGE flag (Default false).
 *
 * @section Diagram
 *
 *     Capsule                             Capsule
 *   +---------+                         +---------+
 *   |   c1    |                         |   c2    |
 *   |         |                         |         |
 *   |      [Port]---[Connector<T>]--->[Port]      |
 *   |         |                         |         |
 *   +---------+                         +---------+
 *
 */
template<typename T, bool ON_CHANGE = false>
class Connector {
public:
  /**
   * Construct Connector with given null terminated vector of
   * listeners, and initial value.
   * @param[in] listeners null terminated vector of capsule references.
   * @param[in] value initial value of connector.
   */
  Connector(Capsule* const* listeners, T value) : 
    m_value(value),
    m_listeners(listeners)
  {}

  /**
   * Set the connector with given value. Schedule listener
   * capsules. 
   * @param[in] value to update with.
   * @return value.
   */
  T operator=(T value)
  {
    if (ON_CHANGE && (m_value == value)) return (value);
    m_value = value;
    controller.schedule(m_listeners);
    return (value);
  }

  /**
   * Set the connector with value from given connector. Schedule
   * listener capsules. 
   * @param[in] connector value to assign.
   * @return value.
   */
  T operator=(Connector<T> &connector)
  {
    return (*this = connector.m_value);
  }

  /**
   * Get the connector value.
   * @return value.
   */
  operator T()
  {
    return (m_value);
  }
  
protected:
  T m_value;
  Capsule* const* m_listeners;
};

/**
 * Boolean Connector with value filtering, i.e. on value change
 * scheduling.
 */
typedef Connector<bool,true> Signal;

};
#endif

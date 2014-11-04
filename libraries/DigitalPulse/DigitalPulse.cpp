/**
 * @file    DigitalPulse.cpp
 * @author  Mike Aiello N2HTT <n2htt@arrl.net>
 * @version 1.0
 *
 * Copyright (C) 2014 Michael Aiello N2HTT
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the 
 * Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section DESCRIPTION
 *
 * This file contains implementation for DigitalPulse. This struct
 * holds information describing a pulse read from an input pin.
 */
 
#include <Arduino.h> 
#include <DigitalPulse.h>

/**
 * returns pulse description text
 *
 * @return pointer to pulse description text
 */
const char * DigitalPulse::getDescription() {
   memset(pulseDescription, 0, PULSE_DESCRIPTION_MAX);
   
   if (isValid) {
      char buffer[PULSE_VALUE_BUFFER_MAX];
      memset(buffer, 0, sizeof(buffer));
      
      ltoa(startTime, buffer, 10);
      strncat(pulseDescription
            , buffer
            , PULSE_DESCRIPTION_MAX - 1);
      
      pulseDescription[strlen(pulseDescription)]
         = PULSE_DESCRIPTION_VALUE_DELIMITER;
      
      ltoa(endTime, buffer, 10);
      strncat(pulseDescription
            , buffer
            , PULSE_DESCRIPTION_MAX - 1);   
   }

   return pulseDescription;
}


/**
 * @file    ModeSelector.cpp
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
 * This file contains the class implementation for ModeSelector.
 * This class makes use of an input pin and the pulses detected on 
 * that pin to select between three operational modes: IDLE, SHORT_PULSE
 * and LONG_PULSE mode. The allows short or long button presses to
 * access different actions.
 */
 
#include <Arduino.h>
#include <ModeSelector.h>
   
/**
 * sets indication on output pins, 
 * based on pulse mode of input pin
 */
void ModeSelector::assertOutputPin() const
{
   switch (inputPin.getCurrentPinMode()) {
      case PIN_MODE_SHORT_PULSE:
         longPulseOutputPin.writeValue(LOW);
         shortPulseOutputPin.writeValue(HIGH);
         break;
         
      case PIN_MODE_LONG_PULSE:
         shortPulseOutputPin.writeValue(LOW);
         longPulseOutputPin.writeValue(HIGH);
         break;
         
      default:
      case PIN_MODE_IDLE:
         shortPulseOutputPin.writeValue(LOW);
         longPulseOutputPin.writeValue(LOW);
         break;
   };
}
   
/**
 * sets pulse mode on input pin
 * 
 * @param  newMode  the pulse mode to set, 
 *                  value in {IDLE, SHORT_PULSE, LONG_PULSE}
 */
void ModeSelector::forceMode(InputPinMode newMode)
{
    inputPin.setCurrentPinMode(newMode);
    assertOutputPin();
}

/**
 * reads and stores pulse mode of input pin
 * 
 * @return  true if mode of pin has changed
 */
bool ModeSelector::readInputPulseMode()
{
   return inputPin.readInputPulseMode();
}
    

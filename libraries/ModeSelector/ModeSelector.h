#ifndef _MODESELECTOR_H_
#define _MODESELECTOR_H_

/**
 * @file    ModeSelector.h
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
 * This file contains the class definition for ModeSelector.
 * This class makes use of an input pin and the pulses detected on 
 * that pin to select between three operational modes: IDLE, SHORT_PULSE
 * and LONG_PULSE mode. The allows short or long button presses to
 * access different actions.
 */
 
#include <Arduino.h> 
#include <DigitalPulse.h>
#include <DigitalPin.h>

/**
 * The Mode Selector uses one digital input pin, and two digital
 * output pins.
 * 
 * Based on the pulse mode of the input pin (see digitalpin.h 
 * for details) the mode selector sets the state of the output
 * pins. If the mode is IDLE, both output pins are LOW.
 * If the mode is SHORT_PULSE, the short pulse output pin is
 * set HIGH, and long pulse output pin is set LOW.
 * If the mode is LONG_PULSE, the long pulse output pin is
 * set HIGH, and short pulse output pin is set LOW.
 * 
 * The MS can also report the current mode to any using the
 * gettCurrentPinMode() method.
 * 
 * It can also force the input pin to any valid mode, using
 * forceMode() method.
 * 
 * The MS can also be used without output pins. The input pin
 * mode will still be read and can be forced or interrogated, 
 * but no output display will be provided.
 * 
 * This version of MS requires that the input and output
 * pins be declared and intialized outside of the class.
 * This allows MS to share its output pins with other
 * objects.
 */
 class ModeSelector {
 protected:
   
  /**
   * reference to mode selector button input pin
   */
   DigitalInputPin   &inputPin;
   
  /**
   * reference to short pulse mode indicator output pin
   */
   DigitalOutputPin  &shortPulseOutputPin;
   
  /**
   * reference to long pulse mode indicator output pin
   */
   DigitalOutputPin  &longPulseOutputPin;
   
public:
  /**
   * ModeSelector constructor
   * 
   * @param  dip  the input pin used for selector button 
   * @param  spop the output pin showing short mode indication 
   * @param  lpop the output pin showing long mode indication 
   */
   ModeSelector(DigitalInputPin  &dip
               ,DigitalOutputPin &spop
               ,DigitalOutputPin &lpop
   ) 
   : inputPin(dip)
   , shortPulseOutputPin(spop)
   , longPulseOutputPin(lpop)
   {}
   
  /**
   * ModeSelector destructor
   */
    ~ModeSelector() {}
   
  /**
   * reads and stores pulse mode of input pin
   * 
   * @return  true if mode of pin has changed
   */
   bool readInputPulseMode();
   
  /**
   * sets indication on output pins, 
   * based on pulse mode of input pin
   */
   void assertOutputPin() const;
   
  /**
   * sets pulse mode on input pin
   * 
   * @param  newMode  the pulse mode to set, 
   *                  value in {IDLE, SHORT_PULSE, LONG_PULSE}
   */
   void forceMode(InputPinMode newMode);
   
  /**
   * returns current mode of input pin 
   *
   * @return current mode of input pin, value in {LOW,HIGH}
   */
   int getCurrentPinMode() {
      return inputPin.getCurrentPinMode();
   }
};

#endif // _MODESELECTOR_H_

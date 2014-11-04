/**
 * @file    ModeSelectorTest.ino
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
 * This an example sketch using the ModeSelector object.
 * ModeSelector makes use of an input pin and the pulses detected on 
 * that pin to select between three operational modes: IDLE, SHORT_PULSE
 * and LONG_PULSE mode. The allows short or long button presses to
 * access different actions.
 */
 
#include <DigitalPulse.h>
#include <DigitalPin.h>
#include <ModeSelector.h>

/**
 * local constants
 */
#define MODE_SELECTOR_PIN     1
#define MODE_SHORT_PIN        5
#define MODE_LONG_PIN         6
#define DEBOUNCE_WAIT_MILS    50
#define STARTUP_WAIT_MILS     5
#define LOOP_DELAY_MILS       1
#define SERIAL_BAUD_RATE   9600
#define AUTO_RESET_MILS 3000

/**
 * pin representations
 */
DigitalInputPin ModeSelectPin( MODE_SELECTOR_PIN
                             , INPUT_PULLUP
                             , DEBOUNCE_WAIT_MILS
                             , DIGITAL_PIN_INIT_STATE_HIGH
                             , DIGITAL_PIN_INVERTING);

DigitalOutputPin ShortModePin( MODE_SHORT_PIN, DIGITAL_PIN_INIT_STATE_LOW );
DigitalOutputPin LongModePin( MODE_LONG_PIN, DIGITAL_PIN_INIT_STATE_LOW );

/**
 * ModeSelector object
 */
ModeSelector ModeSelect( ModeSelectPin
                       , ShortModePin
                       , LongModePin); 
                       
/**
 * file scoped global variables
 */
int loopPinMode = PIN_MODE_IDLE;
int x = 0;

/**
 * initialization performed at reset
 */
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(SERIAL_BAUD_RATE);
  
  // initialize digital pin hardware
  ModeSelectPin.initialize();
  ShortModePin.initialize();
  LongModePin.initialize();
  
  delay(STARTUP_WAIT_MILS);
}

/**
 * main loop
 */
void loop() {
   // note our current state
   loopPinMode = ModeSelect.getCurrentPinMode();
   
   // test for new state
   if (ModeSelect.readInputPulseMode()) {
      // state has changed - change output pin to match
      ModeSelect.assertOutputPin();
      
      // do something based on change of state
      loopPinMode = ModeSelect.getCurrentPinMode();
      switch (loopPinMode) {
         case PIN_MODE_SHORT_PULSE:
            Serial.print("\nNow in SHORT_PULSE mode");
            break;
            
         case PIN_MODE_LONG_PULSE:
            Serial.print("\nNow in LONG_PULSE mode");
            break;
            
         case PIN_MODE_IDLE:
            Serial.print("\nNow in IDLE mode");
            break;
            
         default:
            Serial.print("\nNow in UNKNOWN mode");
            break;
      };
     // reset watchdog count
     x = 0;
   }

   // watchdog state reset
   
   ++x;
   if (x >= AUTO_RESET_MILS) {
     x = 0;
     ModeSelect.forceMode(PIN_MODE_IDLE);
     Serial.print("\nNow forced IDLE mode");
   }
   
   // delay in between reads for stability
   delay(LOOP_DELAY_MILS);      
}










/**
 * @file    ChannelSelectorTest.ino
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
 * This an example sketch using the ChannelSelector object.
 * ChannelSelector makes use of an input pin and the pulses detected on 
 * that pin to either report the currently selected channel (on a short
 * press) or select the channel on a long press.
 *
 * Channel selection is via a "menu" presented on the long pulse output
 * pin. Each channel number is flashed in turn on the input pin, followed
 * by a long delay. Any pulse on the input pin will select the last 
 * channel number flashed. The menu loop continues until a selection is
 * made or the mode times out.
 */
 
#include <DigitalPulse.h>
#include <DigitalPin.h>
#include <ChannelSelector.h>

#define CHANNEL_SELECTOR_PIN  2
#define MODE_SHORT_PIN        5
#define MODE_LONG_PIN         6
#define DEBOUNCE_WAIT_MILS    50
#define STARTUP_WAIT_MILS     5
#define LOOP_DELAY_MILS       1
#define SERIAL_BAUD_RATE   9600

/**
 * channel selection input pin
 */
DigitalInputPin ChannelSelectPin( CHANNEL_SELECTOR_PIN
                                , INPUT_PULLUP
                                , DEBOUNCE_WAIT_MILS
                                , DIGITAL_PIN_INIT_STATE_HIGH
                                , DIGITAL_PIN_INVERTING);

/**
 * short mode output pin
 */
DigitalOutputPin ShortModePin( MODE_SHORT_PIN, DIGITAL_PIN_INIT_STATE_LOW );

/**
 * long mode output pin
 */
DigitalOutputPin LongModePin( MODE_LONG_PIN, DIGITAL_PIN_INIT_STATE_LOW );

/**
 * ChannelSelector object
 */
ChannelSelector ChannelSelect(ChannelSelectPin
                             ,ShortModePin
                             ,LongModePin); 
                       
/**
 * initialization performed at reset
 */
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(SERIAL_BAUD_RATE);
  
  // initialize digital pin hardware
  ChannelSelectPin.initialize();
  ShortModePin.initialize();
  LongModePin.initialize();
    
  delay(STARTUP_WAIT_MILS);
}

/**
 * main loop
 */
void loop() {
    
    if ( ChannelSelect.readInputPulseMode()) {
        ChannelSelect.processInputPulseMode();
        
        Serial.println(ChannelSelect.getCurrentChannelName());
    }
   
   
   // delay in between reads for stability
   delay(LOOP_DELAY_MILS);      
}










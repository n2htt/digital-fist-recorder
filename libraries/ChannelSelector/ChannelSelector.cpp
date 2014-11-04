#include <Arduino.h>
#include <ChannelSelector.h>

/**
 * @file    ChannelSelector.h
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
 * This file contains class definitions for ChannelSelector. This class
 * provides short press/long press selection and reporting of recording
 * channels, each channel holding an independent message.
 */
 
#define INNER_LOOP_DELAY_MILS  50
#define INNER_LOOP_COUNT       20

/**
 * flashes the channel number on the specified output pin
 * 
 * @param  channel  the channel number to flash 
 * @param  channel  the output pin to flash on
 */
void ChannelSelector::reportChannel(int channel, DigitalOutputPin &outputPin) const
{
     if ((channel > 0)&&(channel <= RECORDING_CHANNELS)){
         outputPin.writeValue(LOW);
         for (int ii=0; ii<channel; ++ii) {
            outputPin.outputPulse(CSELCT_DISPLAY_CHANNEL_PULSE_WIDTH_MILS
                                 ,CSELCT_DISPLAY_CHANNEL_SPACING_WIDTH_MILS
                                 ,CSELCT_DISPLAY_CHANNEL_LEAD_MILS);
         }
     }
}

/**
 * Invokes readInputPulseMode method of DigitalInputPin
 * on the input pin.
 * 
 * @see DigitalInputPin::readInputPulseMode() 
 * 
 * @return  true if mode of input pin mode has changed
 */
bool ChannelSelector::readInputPulseMode()
{
   return inputPin.readInputPulseMode();
}

/**
 * updates pulse mode
 */
void ChannelSelector::processInputPulseMode()
{
    int pinMode = PIN_MODE_IDLE;
    int newChannel = currentChannel;
    bool keep_reporting = true;
    
    // test for new state
    if (inputPin.hasChanged()) {

      // do something based on change of mode
      pinMode = inputPin.getCurrentPinMode();
      
      switch (pinMode) {
         case  PIN_MODE_SHORT_PULSE:
            delay(CSELCT_PAUSE_BEFORE_REPORT_MILS);
            reportChannel(currentChannel, shortPulseOutputPin);
            inputPin.setCurrentPinMode(PIN_MODE_IDLE);
            break;
            
         case PIN_MODE_LONG_PULSE:
            newChannel = currentChannel;
            keep_reporting = true;
            
            // reset pin mode so we can detect user selection
            inputPin.setCurrentPinMode(PIN_MODE_IDLE);
          
            while(keep_reporting) {
                // report proposed new channel
                reportChannel(newChannel, longPulseOutputPin);
                
                // give user chance to select it
                for (int ii=0; ii<INNER_LOOP_COUNT; ii++) {
                   delay(INNER_LOOP_DELAY_MILS);
                   
                   // select last proposed channel if pulse detected on pin
                   keep_reporting = !inputPin.readInputPulseMode();
                   if (!keep_reporting) {
                      currentChannel = newChannel;
                      
                      // report new channel on report pin
                      delay(CSELCT_PAUSE_BEFORE_REPORT_MILS);
                      reportChannel(currentChannel, shortPulseOutputPin);
                      break;
                   }
                }
                
                // set up next proposed channel
                newChannel = (++newChannel<=RECORDING_CHANNELS)?newChannel:1;
            }
            
            inputPin.setCurrentPinMode(PIN_MODE_IDLE);
            break;
            
         case PIN_MODE_IDLE:
            break;
            
         default:
            inputPin.setCurrentPinMode(PIN_MODE_IDLE);
            break;
      }; 
   } 
}
   
/**
 * Gets the text string associated with the specified channel
 * These strings correspond to the channel file names on the 
 * storage card.
 * 
 * @param  ch       the channel number
 * 
 * @return  text string associated with current channel
 */
char * ChannelSelector::getChannelName(unsigned int ch) const {
    if ((1 <= ch ) && (ch <= RECORDING_CHANNELS)) {
        return channelName[ch - 1];
    }
    else {
        return "";
    }
}
 

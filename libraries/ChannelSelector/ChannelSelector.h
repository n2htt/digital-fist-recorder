#ifndef _CHANNELSELECTOR_H_
#define _CHANNELSELECTOR_H_

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
 
#include <Arduino.h> 
#include <DigitalPin.h>
#include <DigitalPulse.h>

#define RECORDING_CHANNELS                           4 
#define CSELCT_DISPLAY_CHANNEL_PULSE_WIDTH_MILS    200
#define CSELCT_DISPLAY_CHANNEL_SPACING_WIDTH_MILS   80
#define CSELCT_DISPLAY_CHANNEL_LEAD_MILS            20
#define CSELCT_PAUSE_BEFORE_REPORT_MILS            400

/**
  The Channel Selector uses one digital input pin, and two digital
  output pins.

  The CS maintains the currently selected channel, an integer
  value from 1 to N inclusive, set in the constructor. The CS
  offers two functions to maintain the current channel: reporting 
  and selecting. It uses the pulse mode functionality of 
  DigitalInputPin to alternate between these functions.

  When CS detects entry into short pulse mode, it reports the
  currently selected channel by blinking the short pulse 
  output pin for the number of times corresponding to the
  current channel number. For example, it will blink once
  for channel 1, twice for channel 2 etc. After reporting
  the current channel, CS returns to the idle mode.

  When CS enters the long pulse mode, it begin a loop where it
  reports to the long pulse output pin, starting with the currently
  selected channel, and counting up. It wraps around to channel
  1 after reaching the highest defined channel. There is a long
  pause betwen channel reports, to allow for user input. When the
  channel selection input pin sees at least a short pulse, the 
  last reported channel is made the new currently selected 
  channel, and the CS returns to an idle mode.

  The CS can also report the current mode to any caller using 
  the gettCurrentChannel() method.

  It can also force the input pin to any valid mode, using
  forceMode() method.

  This version of CS requires that the input and output
  pins be declared and intialized outside of the class.
  This allows CS to share its output pins with other
  objects.

 */
class ChannelSelector {
 protected:
   DigitalInputPin   &inputPin;  /** reference to input pin */
   DigitalOutputPin  &shortPulseOutputPin;  /** reference to short mode output pin */
   DigitalOutputPin  &longPulseOutputPin; /** reference to long mode output pin */
   
   /** 
    * stores the currently selected channel number 
    */
   char *channelName[RECORDING_CHANNELS];
    
   int currentChannel; /** stores the currently selected channel number */
   
  /**
   * flashes the channel number on the specified output pin
   * 
   * @param  channel  the channel number to flash 
   * @param  channel  the output pin to flash on
   */
   void reportChannel(int channel, DigitalOutputPin &outputPin) const;
    
public:
  /**
   * ChannelSelector constructor
   * 
   * @param  dip  the input pin used for selector button 
   * @param  sp   the output pin showing short mode indication 
   * @param  lp   the output pin showing long mode indication 
   */
   ChannelSelector ( DigitalInputPin &dip
                   , DigitalOutputPin &sp
                   , DigitalOutputPin &lp) 
   : inputPin(dip)
   , shortPulseOutputPin(sp)
   , longPulseOutputPin(lp)
   , currentChannel(1)
   {
      channelName[0] = "chnl1.txt";
      channelName[1] = "chnl2.txt";
      channelName[2] = "chnl3.txt";
      channelName[3] = "chnl4.txt";
   }
 
  /**
   * ChannelSelector destructor
   */
   ~ChannelSelector () {}
   
  /**
   * Gets the text string associated with the specified channel
   * These strings correspond to the channel file names on the 
   * storage card.
   * 
   * @param  ch       the channel number
   * 
   * @return  text string associated with current channel
   */
    char * getChannelName(unsigned int ch) const;  
 
  /**
   * Gets the text string associated with the current channel
   * These strings correspond to the channel file names on the 
   * storage card.
   * 
   * @return  text string associated with current channel
   */
    char * getCurrentChannelName()  const {
        return getChannelName(currentChannel);
    }
    
 
  /**
   * Invokes readInputPulseMode method of DigitalInputPin
   * on the input pin.
   * 
   * @see DigitalInputPin::readInputPulseMode() 
   * 
   * @return  true if mode of input pin mode has changed
   */
    bool readInputPulseMode();
    
 
  /**
   * updates pulse mode
   */
    void processInputPulseMode();
    

   /**
    * returns integer value of currently selected channel
    *
    * @return currently selected channel
    */
   int getCurrentChannel() {
    return currentChannel;
   }
};

#endif // _CHANNELSELECTOR_H_

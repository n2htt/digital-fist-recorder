#ifndef _PULSE_TRAIN_RECORDER_H_
#define _PULSE_TRAIN_RECORDER_H_

/**
 * @file    PulseTrainRecorder.h
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
 * This file contains class definitions for PulseTrainRecorder
 * This class reads and writes pulse train descriptions to text
 * files on an SD card.
 */
 

#include <Arduino.h>
#include <SD.h>

#include <DigitalPulse.h>
#include <DigitalPin.h>

#define CHANNEL_FILENAME_MAX   16
#define PULSE_VALUE_BUFFER_CT   2
#define PLAYBACK_DELAY_MILS   100


/* -----------------------------------------------------------
 * 
 * -----------------------------------------------------------*/

/**
* SD implementation of Pulse Train Recorder
* 
* This class is written specifically for SD card implementation
* because of space considerations on the Arduino UNO. If more
* variable and code space were available, it could easily be 
* generalized to a base class, with child classes specific to 
* different media.
*/
class PulseTrainRecorder {
protected:
   char currentFileName[CHANNEL_FILENAME_MAX];

  /** time playback started, milliseconds since reset  */
   long playbackStartTime; 
   
  /** pulse train start time, normalized to time playback started, 
   *  milliseconds since reset  
   */
   long pulseTrainStartTime; 
   
  /** current pulse start time, normalized to time playback started, 
   *  milliseconds since reset  
   */
   long currentPulseStartTime;
   
  /** current pulse end time, normalized to time playback started, 
   *  milliseconds since reset  
   */
   long currentPulseEndTime;

  /** flag indicating SD card file is open for reading  */
   bool isOpenForRead; 
   
  /** flag indicating SD card file is open for writing  */
   bool isOpenForWrite;  

  /** flag indicating if playback is active  */
   bool isPlaybackActive;  

  /** file object on SD card  */
   File PTRFile;  

  /**
   * determines state of keying output by comparing 
   * time since playback started to 
   * offset from beginning and ending of the current pulse description
   *
   * @return keying output state value in {LOW,HIGH}
   */
   int getPlaybackLogicalState();
    
public:
  /**
   * PulseTrainRecorder Constructor
   */
   PulseTrainRecorder()
   : isOpenForRead(false)
   , isOpenForWrite(false)
   , playbackStartTime(0)
   , pulseTrainStartTime(0)
   , currentPulseStartTime(0)
   , currentPulseEndTime(0)
   , isPlaybackActive(false)
   {
    // empty text fields
    currentFileName[0] = 0;
   }
    
  /**
   * initializes SD card hardware
   *
   * @param  sd_reserved_pin       SD reserved output pin number
   * @param  sd_cs_pin             SD CS pin number
   * 
   * @return true if SD card reports successful initialization
   */
   bool initialize(int sd_reserved_pin, int sd_cs_pin);
    
  /**
   * PulseTrainRecorder Constructor
   */
   ~PulseTrainRecorder()
    {}
    
  /**
   * opens file for recording to SD card
   *
   * @return true if file successfully opened for writing
   */
   bool openForRecording(char *fn);
    
  /**
   * writes pulse description to SD card
   *
   * @return true if pulse successfully read and playback active
   */
   bool recordPulse(DigitalPulse dp);
    
  /**
   * opens file for playback from SD card
   *
   * @return true if file successfully opened and playback active
   */
   bool openForPlayback(char *fn);
    
  /**
   * reads next pulse description from SD card
   *
   * @return true if pulse successfully read and playback active
   */
   bool readNextPulse();
    
  /**
   * closes any file open on SD card
   */
   void close();

  /**
   * gets value of playback active flag
   *
   * @return true if playback active
   */
   bool playbackActive()  const {
      return isPlaybackActive;
   }

  /**
   * plays back pulses to keying pin and sidetone pin
   *
   * @return true if keying state has changed
   */
   bool playBackKeying( DigitalOutputPin &keyingPin
                      , DigitalOutputPin &sideTonePin); 

};

#endif // _PULSE_TRAIN_RECORDER_H_

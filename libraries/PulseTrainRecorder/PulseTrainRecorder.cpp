
/**
 * @file    PulseTrainRecorder.cpp
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

#include <PulseTrainRecorder.h>

/**
 * local overrides of file open modes
 */
#define FILE_READ O_READ
#define FILE_WRITE (O_WRITE |O_CREAT | O_TRUNC)

/**
 * initializes SD card hardware
 *
 * @param  sd_reserved_pin       SD reserved output pin number
 * @param  sd_cs_pin             SD CS pin number
 * 
 * @return true if SD card reports successful initialization
 */
bool PulseTrainRecorder::initialize(int sd_reserved_pin, int sd_cs_pin) {
   // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
   // Note that even if it's not used as the CS pin, the hardware SS pin 
   // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
   // or the SD library functions will not work. 
   pinMode(sd_reserved_pin, OUTPUT);
   
   // initialize card, return status 
   bool sd_okay = SD.begin(sd_cs_pin);
   return sd_okay;
}

/**
 * opens file for recording to SD card
 *
 * @return true if file successfully opened for writing
 */
bool PulseTrainRecorder::openForRecording(char * fn) {
   // close any open files and reset is open flags
   this->close();
   
   // copy to char buffer for use with file rtns
   memset(currentFileName, 0, CHANNEL_FILENAME_MAX);
   strncpy(currentFileName, fn, CHANNEL_FILENAME_MAX - 1);

   // attempt open for write
   PTRFile = SD.open(currentFileName, FILE_WRITE);
   
   if (PTRFile) {
      isOpenForWrite = true;
      //Serial.print(currentFileName);
      //Serial.println(" open for recording.");
   }
   else {
      isOpenForWrite = false;
      Serial.print(currentFileName);
      Serial.println(" failed to open for recording.");
  }
   return isOpenForWrite;
}

/**
 * opens file for playback from SD card
 *
 * @return true if file successfully opened and playback active
 */
bool PulseTrainRecorder::openForPlayback(char * fn) {
   // close any open files and reset is open flags
   this->close();
   
   // copy to char buffer for use with file rtns
   memset(currentFileName, 0, CHANNEL_FILENAME_MAX);
   strncpy(currentFileName, fn, CHANNEL_FILENAME_MAX - 1);
   
   // attempt open for read
   PTRFile = SD.open(currentFileName, FILE_READ);
   
   if (PTRFile) {
      isOpenForRead = true;
      //Serial.print(currentFileName);
      //Serial.println(" open for playback.");
   
      // load first record
      if (readNextPulse()) {
         // set playback time
          playbackStartTime = millis() + PLAYBACK_DELAY_MILS;

		 // save first pulse start time as pulse train offset
         pulseTrainStartTime = currentPulseStartTime;

         // ready to start playback
         isPlaybackActive = true;
      }
      else {
         Serial.print(currentFileName);
         Serial.println(" couldn't read first pulse.");
         isPlaybackActive = false;
      }
   }
   else {
      isOpenForRead = false;
      Serial.print(currentFileName);
      Serial.println(" failed to open for playback.");
   }

   return isOpenForRead && isPlaybackActive;
}

/**
 * closes any file open on SD card
 */
void PulseTrainRecorder::close() {
   // close any file already open
   if ((isOpenForWrite) || (isOpenForRead)){
      PTRFile.close();
      //Serial.print(currentFileName);
      //Serial.println("  closed.");  
   }
   
   // clear name and flags
   currentFileName[0]=0;
   isOpenForWrite = false;
   isOpenForRead = false;
   isPlaybackActive = false;
}
    
/**
 * writes pulse description to SD card
 *
 * @return false if writing is not possible, otherwise true
 */
bool PulseTrainRecorder::recordPulse(DigitalPulse dp) {
   bool rtn = false;
   if ((isOpenForWrite) && PTRFile) {
      if (dp.isValid) {
         // write to card and commit immediately
         PTRFile.println(dp.getDescription());
         PTRFile.flush();
      }

      rtn = true;
   }

   return rtn;
}

/**
 * reads next pulse description from SD card
 *
 * @return true if pulse successfully read and playback active
 */
bool PulseTrainRecorder::readNextPulse(){
   // set up parsing buffers
   int buf_idx[] = {0,0};
   int buf_sel = 0;
   char input_char = 0;
   
   char nextPulseBuffer[PULSE_VALUE_BUFFER_CT][PULSE_VALUE_BUFFER_MAX];
   memset(nextPulseBuffer, 0, PULSE_VALUE_BUFFER_CT*PULSE_VALUE_BUFFER_MAX);
   
   if (isOpenForRead && PTRFile && PTRFile.available()) {
      while (    PTRFile.available() 
              && (         buf_sel < PULSE_VALUE_BUFFER_CT)
              && (buf_idx[buf_sel] < PULSE_VALUE_BUFFER_MAX)) {
         input_char = PTRFile.read();
         
         if ('\n' == input_char) {
            // end of line we are done
            break;
         }
         else if (PULSE_DESCRIPTION_VALUE_DELIMITER == input_char) {
            // hit the value delimiter - switch buffers
            ++buf_sel;
         }
         else {
            // put character into correct buffer
            // and increment buffer index
            nextPulseBuffer[buf_sel][buf_idx[buf_sel]++] = input_char;
         }
      }
      
      currentPulseStartTime = atol(nextPulseBuffer[0]);
      currentPulseEndTime   = atol(nextPulseBuffer[1]);
      
      // reading a bad pulse description cancels playback
       isPlaybackActive = (   (currentPulseEndTime>currentPulseStartTime) 
                           && (currentPulseStartTime >= 0));

      /* 
      Serial.print("pulse: ");
      Serial.print(currentPulseStartTime);
      Serial.print(" ");
      Serial.print(currentPulseEndTime);
      Serial.println("!");
      */
    }
    else {
       // failure to read the next pulse cancels playback
       isPlaybackActive = false;
    }
   
   return isPlaybackActive;
}

/**
 * determines state of keying output by comparing 
 * time since playback started to 
 * offset from beginning and ending of the current pulse description
 *
 * @return keying output state value in {LOW,HIGH}
 */
int PulseTrainRecorder::getPlaybackLogicalState() {
   int rtnState = LOW;

   if (isPlaybackActive) {
      // key down for this pulse is the playback start time
      // plus the offset from the start of the first pulse
      // to the start of the current pulse
      long keyStartTime = playbackStartTime 
                        + currentPulseStartTime - pulseTrainStartTime;


      // key up for this pulse is the playback start time
      // plus the offset from the start of the first pulse
      // to the start of the current pulse
      long keyEndTime = playbackStartTime 
                      + currentPulseEndTime - pulseTrainStartTime;
                      
      long timeNow = millis();
                      
      // past the end of the current pulse?
      // get the next pulse
      if (timeNow >= keyEndTime){
         rtnState = LOW;
         readNextPulse();
      }
      // after the start of the current pulse
      // key should be high
      else if ((timeNow >= keyStartTime)) {
        rtnState = HIGH;
      }
      // current pulse hasn't started yet
      else {
          rtnState = LOW;
      }
   }

   return rtnState;
}

/**
 * plays back pulses to keying pin and sidetone pin
 *
 * @return true if keying state has changed
 */
bool PulseTrainRecorder::playBackKeying( DigitalOutputPin &keyingPin
                                       , DigitalOutputPin &sideTonePin) {

   // assume no change of keying state
   bool rtn = false;
   
   // get current state of output keying pin                                      
   int currentKeyingState =  keyingPin.getLogicalState();   
   
   int nextKeyingState = getPlaybackLogicalState();
   
   if (currentKeyingState != nextKeyingState) {
      keyingPin.writeLogicalValue(nextKeyingState);
      sideTonePin.writeLogicalValue(nextKeyingState);
      //Serial.println(millis());
      rtn = true;
   }
   
   return rtn;
}
    

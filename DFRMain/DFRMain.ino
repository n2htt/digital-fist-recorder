/*! \mainpage Digital Fist Recorder
 *
 * DFRMain is an Arduino sketch that implements the Digital Fist Recorder,
 * a device which can record Morse Code messages and play them back 
 * reproducing the exact timing of the input message.
 * This allows replaying messages that capture the "fist" of the sender -
 * all the idiosyncracies of timing that make hand-sent code as unique as
 * handwriting.
 * 
 */
 
/**
 * @file    DFRMain.ino
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
  * This sketch implements the "Digital Fist Recorder" [DFR] on the
 * Arduino UNO R3. The DFR stores four separate CW messages by 
 * recording the actual timing of the input pulse train, and reproducing
 * the exact timing on playback, thus preserving the operator's "fist".
 */
 
#include <Arduino.h>
#include <SD.h>

#include <DigitalPulse.h>
#include <DigitalPin.h>
#include <ModeSelector.h>
#include <ChannelSelector.h>
#include <PulseTrainRecorder.h>
#include "dfrconstants.h"

/**
 * local constants
 * <p>
 * AUTO_RESET_CT is the maximum number of loop iterations that are
 * allowed in record or playback mode without any activity. This 
 * value is used to implement a "watchdog" timer to prevent the 
 * possibility of a stuck key condition. Since each loop iteration
 * is approximately one millisecond, this value may be considered to 
 * represent the operation timeout in milliseconds.
 * <p>
 * ERROR_RPT_PULSE_WIDTH is the duration in milliseconds of the flash
 * used in the error report routine. (flash three times on error).
 * <p>
 * ERROR_RPT_SPACING is the duration in milliseconds of the wait between
 * flashes used in the error report routine.
 */
#define AUTO_RESET_CT 5000
#define ERROR_RPT_PULSE_WIDTH  300
#define ERROR_RPT_SPACING       75

#define WELCOME_PULSE_WIDTH  150
#define WELCOME_SPACING       50
                          
/**
 * These objects represent the pinouts on the Arduino board
 * that are used for input in this sketch
 * <p>
 * KeyingInput is connected to the external keying device (straight 
 * key, bug, cootie etc.). Bringing this pin LOW (grounding it)
 * corresponds to a "key down" or transmitting condition. Its rest
 * state is HIGH.
 * <p>
 * ModeSelectPin is connected to a button used to select playback or
 * record mode based on a short/long press. Bringing this pin LOW 
 * (grounding it) constitutes a buttton press. Its rest
 * state is HIGH.
 * <p>
 * ChannelSelectPin is connected to a button used to either interrogate
 * or set the current recording channel based on a short/long press. 
 * Bringing this pin LOW (grounding it) constitutes a buttton press. 
 * Its rest state is HIGH.
 */
DigitalInputPin KeyingInput( KEY_INPUT_PIN
                           , INPUT_PULLUP
                           , DEBOUNCE_WAIT_MILS
                           , DIGITAL_PIN_INIT_STATE_HIGH
                           , DIGITAL_PIN_INVERTING
                           , DIGITAL_PIN_WRITE_TO_SERIAL);
                         
DigitalInputPin ModeSelectPin( MODE_SELECTOR_PIN
                             , INPUT_PULLUP
                             , DEBOUNCE_WAIT_MILS
                             , DIGITAL_PIN_INIT_STATE_HIGH
                             , DIGITAL_PIN_INVERTING);

DigitalInputPin ChannelSelectPin( CHANNEL_SELECTOR_PIN
                                , INPUT_PULLUP
                                , DEBOUNCE_WAIT_MILS
                                , DIGITAL_PIN_INIT_STATE_HIGH
                                , DIGITAL_PIN_INVERTING);

                          
/**
 * These objects represent the pinouts on the Arduino board
 * that are used for output in this sketch
 * <p>
 * SpeakerOutput is toggled HIGH when an audio tone should be heard
 * The expectation is that a piezo buzzer or some other noise producing
 * device is attached to this pin
 * <p>
 * ShortModePin is used as an indicator for operations selected by 
 * short mode. These include playback, and channel reporting.
 * <p>
 * LongModePin is used as an indicator for operations selected by 
 * long mode. These include recording, and channel selection.
 * <p>
 * KeyingOutput is used to control keying of the radio. It is set
 * HIGH corresponding to a "key down" or transmitting condition; its
 * rest condition is LOW.
 */
DigitalOutputPin SpeakerOutput(SPEAKER_OUTPUT_PIN
                             , DIGITAL_PIN_INIT_STATE_LOW);
                          
DigitalOutputPin ShortModePin( MODE_SHORT_PIN
                             , DIGITAL_PIN_INIT_STATE_LOW);

DigitalOutputPin LongModePin( MODE_LONG_PIN
                            , DIGITAL_PIN_INIT_STATE_LOW);

DigitalOutputPin KeyingOutput( KEYING_OUTPUT_PIN
                             , DIGITAL_PIN_INIT_STATE_HIGH
                             , DIGITAL_PIN_INVERTING);
                          
/**
 * This object manages selection playback or record mode
 */
ModeSelector ModeSelect( ModeSelectPin
                       , ShortModePin
                       , LongModePin); 

                       

/**
 * This object manages selection of the current recording channel
 */
ChannelSelector ChannelSelect(ChannelSelectPin
                             ,ShortModePin
                             ,LongModePin); 

/**
 * This object manages recording and playback of pulses
 */
PulseTrainRecorder PulseTrain;
                       
/**
 * file scoped global variables
 * <p>
 * loopWatchdog counts iterations since the last action taken
 * If it reaches a preset threshold, the operating mode is 
 * forced back to idle, and any current activity ceases. This
 * is basically to prevent a "stuck key" situation from arising
 * under unforseen circumstances.
 * <p>
 * currentMode holds the current operating mode set in the 
 * execution loop. priorMode holds the operating mode carried
 * forward from the last execution loop. We determine if the 
 * mode has changed by comparing current and prior modes
 * <p>
 * modeChanged is set by the comparing of current and prior modes 
 */
static int loopWatchdog  = 0;
static int currentMode   = PIN_MODE_IDLE;
static int priorMode     = currentMode;
static bool modeChanged  = false;
                       
/**
 * This function performs an error indication (three flashes)
 * on an output pin
 */
void flashErrorIndication(DigitalOutputPin &op) {
   op.outputPulse(ERROR_RPT_PULSE_WIDTH, ERROR_RPT_SPACING);
   op.outputPulse(ERROR_RPT_PULSE_WIDTH, ERROR_RPT_SPACING);
   op.outputPulse(ERROR_RPT_PULSE_WIDTH, ERROR_RPT_SPACING);
}

/**
 * This function performs the initialization at reset
 */
void setup() {
   #ifdef ALLOW_SERIAL_IO
      // initialize serial communication at 9600 bits per second:
      Serial.begin(SERIAL_BAUD_RATE);
   #endif

   // enable serial debug messages for DigitalPin
   DigitalPin::writePulsesToSerialEnabled = false;

   // initialize digital pin hardware
   KeyingInput.initialize();
   ModeSelectPin.initialize();
   ChannelSelectPin.initialize();

   SpeakerOutput.initialize();
   ShortModePin.initialize();
   LongModePin.initialize();
   KeyingOutput.initialize();

   // initialize SD card
   if (PulseTrain.initialize(SD_RESERVED_PIN, SD_CS_PIN)) {
      //Serial.println("SD initialized.");
   }
   else {
      #ifdef ALLOW_SERIAL_IO
         Serial.print("SD initialization ");
         Serial.print(SD_RESERVED_PIN);
         Serial.print(" ");
         Serial.print(SD_CS_PIN);
         Serial.println(" failed!");
      #endif
      
      flashErrorIndication(ShortModePin);
   }
   
   delay(STARTUP_WAIT_MILS);
   
   // flash "welcome" indication
   SpeakerOutput.outputPulse(WELCOME_PULSE_WIDTH, WELCOME_SPACING);
   SpeakerOutput.outputPulse(WELCOME_PULSE_WIDTH, WELCOME_SPACING);
   delay(STARTUP_WAIT_MILS);
   LongModePin.outputPulse(WELCOME_PULSE_WIDTH,  WELCOME_SPACING);
   LongModePin.outputPulse(WELCOME_PULSE_WIDTH,  WELCOME_SPACING);
   delay(STARTUP_WAIT_MILS);
   ShortModePin.outputPulse(WELCOME_PULSE_WIDTH, WELCOME_SPACING);
   ShortModePin.outputPulse(WELCOME_PULSE_WIDTH, WELCOME_SPACING);
   delay(STARTUP_WAIT_MILS);
}
                       
/**
 * This function transitions to operation in the IDLE mode
 */
void transitToIdleMode() {
   // prior mode does not affect transition to idle
   //Serial.println("IDLE");           
   loopWatchdog = 0;

   // close any open recorder file
   PulseTrain.close();
      
   // re-activate all pins 
   KeyingInput.resume();
   ModeSelectPin.resume();
   ChannelSelectPin.resume();
   SpeakerOutput.resume();
   ShortModePin.resume();
   LongModePin.resume();
   KeyingOutput.resume();
   
   // reset all pins to logical "off" condition
   KeyingInput.setLogicalState(LOW);
   ModeSelectPin.setLogicalState(LOW);
   ChannelSelectPin.setLogicalState(LOW);

   SpeakerOutput.writeLogicalValue(LOW);
   ShortModePin.writeLogicalValue(LOW);
   LongModePin.writeLogicalValue(LOW);
   KeyingOutput.writeLogicalValue(LOW);
}

/**
 * This function continues operation in the IDLE mode
 */
void continueIdleMode() {
   // check for channel selection request
   if (ChannelSelect.readInputPulseMode()) {
      // activity on channel select input
      // suppress keying pass thru until processed
      KeyingOutput.suspend();
      KeyingInput.suspend();      
      
      // process channel request
      ChannelSelect.processInputPulseMode();
      
      // restore keying pass-thru 
      KeyingOutput.resume();
      KeyingInput.resume();      
   }

   // continuing idle mode  
   // check keying input
   KeyingInput.determinePinState();

   // set output pins based on current keying input state
   KeyingInput.indicate(KeyingOutput);
   KeyingInput.indicate(SpeakerOutput);
}

                       
/**
 * This function transitions to operation in the PLAYBACK mode
 */
void transitToPlaybackMode() {
   // only commence recording 
   // if coming from some state other than record
   if (PIN_MODE_SHORT_PULSE != priorMode) {
      //Serial.print("PLAYBACK ");  
      //Serial.println(ChannelSelect.getCurrentChannel());  
      loopWatchdog = 0;
      
      // attempt to start recording pulses to file
      if (PulseTrain.openForPlayback(ChannelSelect.getCurrentChannelName())) {
         // turn off keying pass-thru 
         KeyingInput.suspend();
      
         // restart watchdog 
         loopWatchdog = 0;
      }
      else {
         // attempt to open for playback was not successful
         flashErrorIndication(LongModePin);
         transitToIdleMode();
      }
   }
}

/**
 * This function continues operation in the PLAYBACK mode
 */
void continuePlaybackMode() {
   // continuing playback mode 
   if (PulseTrain.playbackActive()) {
      if (PulseTrain.playBackKeying( KeyingOutput
                                   , SpeakerOutput)) {
         // restart watchdog 
         loopWatchdog = 0;
      }
      
      // count for auto reset 
      ++loopWatchdog;
   }
   else {
      ModeSelect.forceMode(PIN_MODE_IDLE);
      transitToIdleMode(); 
   }
}

/**
 * This function transitions to operation in the RECORD mode
 */
void transitToRecordMode() {
   // only commence recording 
   // if coming from some state other than record
   if (PIN_MODE_LONG_PULSE != priorMode) {
      // entering record mode
      //Serial.print("RECORD ");     
      //Serial.println(ChannelSelect.getCurrentChannel());  
      
      // attempt to start recording pulses to file
      if (PulseTrain.openForRecording(ChannelSelect.getCurrentChannelName())) {
         // turn off keying pass-thru 
         KeyingOutput.suspend();
         
         // restart watchdog 
         loopWatchdog = 0;
      }
      else {
         // attempt to open for recording was not successful
         flashErrorIndication(ShortModePin);
         transitToIdleMode();
      }
   }
}

/**
 * This function continues operation in the RECORD mode
 */
void continueRecordMode() {
   // check keying input
   KeyingInput.determinePinState();

   // side tone only while recording
   KeyingInput.indicate(SpeakerOutput);

   // write to file as pulses complete
   if (KeyingInput.hasChanged()&&(LOW == KeyingInput.getLogicalState())) {
      loopWatchdog = 0;
      
      // exit to idle mode if pulse write fails
      if (!PulseTrain.recordPulse(KeyingInput.getLastPulse())) {
         flashErrorIndication(ShortModePin);
         transitToIdleMode();
      }
   }
   else {
      // count for auto reset 
      ++loopWatchdog;
   }
}

/**
 * This function selects the correct transition to a new mode 
 * when the mode has changed in the current loop execution
 */
void selectModeTransition() {
   // what we do depends on what we were doing
   switch (priorMode) {
      // any mode change while we are busy
      // cancels what was in progress and
      // reverts to IDLE mode
      case PIN_MODE_SHORT_PULSE:
      case PIN_MODE_LONG_PULSE: 
         transitToIdleMode();
         break;
      
      default:
         // force unknown mode to idle
         //Serial.println("UNKNOWN");
         ModeSelect.forceMode(PIN_MODE_IDLE);
         //Serial.println("forced IDLE");
         // note deliberate fall-through to idle mode  
         // no break here 
           
      case PIN_MODE_IDLE:
         // from IDLE we can start to record or playback
         switch (currentMode) {
            case PIN_MODE_SHORT_PULSE:
               transitToPlaybackMode();
               break;

            case PIN_MODE_LONG_PULSE:
               transitToRecordMode();
               break;

            case PIN_MODE_IDLE:
               // already idle, just keep going
               continueIdleMode();
               break;
         };
         break;
   };
}

/**
 * This function selects the correct action by mode 
 * when the mode has not changed in this loop execution
 */
void selectModeContinuation() {
   switch (currentMode) {
      case PIN_MODE_SHORT_PULSE:
         continuePlaybackMode();
         break;

      case PIN_MODE_LONG_PULSE:
         continueRecordMode();
         break;

      default:
      case PIN_MODE_IDLE:
         continueIdleMode();
         break;
   };
}

/**
 * Main execution loop
 */
 void loop() {
   // setting operational mode has highest priority
   if (modeChanged = ModeSelect.readInputPulseMode()) {
      // state has changed - change output pin to match
      ModeSelect.assertOutputPin();  
   }
   
   // note our current mode   
   currentMode = ModeSelect.getCurrentPinMode();

   if (currentMode != priorMode) {
      // handle mode changed event
      selectModeTransition();
   }
   else {
      // continuing existing mode
      selectModeContinuation();
   }

   // check watchdog counter to reset 
   // after long idle time in playback or record mode
   if (loopWatchdog >= AUTO_RESET_CT) {
      transitToIdleMode();
   }

   // save prior mode
   priorMode = currentMode;

   // loop delay
   delay(LOOP_DELAY_MILS);      
}




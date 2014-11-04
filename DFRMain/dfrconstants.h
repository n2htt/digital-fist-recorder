#ifndef _DFR_CONSTANTS_
#define _DFR_CONSTANTS_

/**
 * @file    dfrconstants.h
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
 * This file contains macros defining digital pins used by DFR
 * and some timing constants.
 */
 
/**
 * digital pin definitions
 */
#define KEY_INPUT_PIN         0
#define MODE_SELECTOR_PIN     1
#define CHANNEL_SELECTOR_PIN  2
#define SPEAKER_OUTPUT_PIN    3
#define SD_CS_PIN             4 // used by SD shield
#define MODE_SHORT_PIN        5
#define MODE_LONG_PIN         6
#define KEYING_OUTPUT_PIN     7
#define NOT_USED_PIN          8
#define NOT_USED_PIN          9
#define SD_RESERVED_PIN      10
#define SD_MOSI_PIN          11 // used by SD shield
#define SD_MISO_PIN          12 // used by SD shield
#define SD_CLK_PIN           13 // used by SD shield

/**
 * wait time definitions
 */
#define DEBOUNCE_WAIT_MILS    10
#define STARTUP_WAIT_MILS     5
#define LOOP_DELAY_MILS       1

/**
 * misc constant definitions
 */
#define SERIAL_BAUD_RATE   9600


#endif // _DFR_CONSTANTS_

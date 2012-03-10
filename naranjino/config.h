/* naranjino copyright (C) 2012  Nacho mas
 * based on trackuino. copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__


// --------------------------------------------------------------------------
// THIS IS THE TRACKUINO FIRMWARE CONFIGURATION FILE. YOUR CALLSIGN AND
// OTHER SETTINGS GO HERE.
//
// NOTE: all pins are Arduino based, not the Atmega chip. Mapping:
// http://www.arduino.cc/en/Hacking/PinMapping
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
// APRS config (aprs.c)
// --------------------------------------------------------------------------

// Set your callsign and SSID here. Common values for the SSID are
// (from http://zlhams.wikidot.com/aprs-ssidguide):
//
// - Balloons:  11
// - Cars:       9
// - Home:       0
// - IGate:      5
#define S_CALLSIGN      "ALMAAK"
#define S_CALLSIGN_ID   11

// Destination callsign: APRS (with SSID=0) is usually okay.
#define D_CALLSIGN      "APRS"
#define D_CALLSIGN_ID   0

// Digipeating paths:
// (read more about digipeating paths here: http://wa8lmf.net/DigiPaths/ )
// The recommended digi path for a balloon is WIDE2-1 or pathless. The default
// is to use WIDE2-1. Comment out the following two lines for pathless:
#define DIGI_PATH1      "WIDE2"
#define DIGI_PATH1_TTL  1

// APRS comment: this goes in the comment portion of the APRS message. You
// might want to keep this short. The longer the packet, the more vulnerable
// it is to noise. 
#define APRS_COMMENT    "NARANJO-FJN"

//UNCOMMNET if you want extra aprs msg using telemetry format.
#define SEND_TELEMETRY
//TELEMETRY MESSAGE DEFINITION
#ifdef SEND_TELEMETRY
#define TELEMETRY_PARM "PARM.Temp,Bat,Bat2,Alt,Vel,UP,DOWN,STP,FIX,B1,B2,bit7,bit8"
#define TELEMETRY_UNIT "UNIT.Celsiu,Volts,Volts,meters,km/h,up,down,on,fix,ok,ok,on,on"
#define TELEMETRY_EQNS "EQNS.0,1,-273,0,0.1,0,0,0.1,0,0,50,0,0,1,0"
#define TELEMETRY_BIT  "BIT.1,1,1,1,0,0,1,1,NARANJO-2012 BALLOON"
#define TELEMETRY_DEFINITION_DELAY 5000   //TIME BETWEEN DEFINITION MSG
#endif

// --------------------------------------------------------------------------
// AX.25 config (ax25.cpp)
// --------------------------------------------------------------------------

// TX delay in milliseconds
#define TX_DELAY      300

// --------------------------------------------------------------------------
// Tracker config (trackuino.cpp)
// --------------------------------------------------------------------------

// APRS_PERIOD is the period between transmissions. Since we're not listening
// before transmitting, it may be wise to choose a "random" value here JUST
// in case someone else is transmitting at fixed intervals like us. 61000 ms
// is the default (1 minute and 1 second).
//
// Low-power transmissions on occasional events (such as a balloon launch)
// might be okay at lower-than-standard APRS periods (< 10m). Check with/ask
// permision to local digipeaters beforehand.
#define APRS_PERIOD   61000UL           //1 minute
//Aditional power reduce for long lasting rescue. Slow aprs period after 
//APRS_REDUCE_CYCLE_START  seconds from boot and do not send telemetry
//You must set the reduce period
#define APRS_SLOW_CYCLE_START 18000   //5 hours
#define APRS_SLOW_PERIOD   301000UL   //5 minutes
// Set any value here (in ms) if you want to delay the first transmission
// after resetting the device.
#define APRS_DELAY    0UL

// GPS baud rate (in bits per second)
#define GPS_BAUDRATE  4800
// If GPS_ON_PIN is defined it it used to switch on/off the GPS module for power
// saving. If not, GPS module will be always on. 
#define GPS_ON_PIN    4
//Do not switch off GPS. Continue execution if GPS_FIX_TIMEOUT  is exceded.
#define GPS_FIX_TIMEOUT   (30000UL)
//Wait N_FIX fixed prior to switch off the GPS to increase precision.(Get the  lower DOP)
#define N_FIX 1
// Set if you wan to connect your GPS module in other port diferent of hardware RX pin
#define GPS_PORT_SOFTSERIAL_RX_PORT   2
#define GPS_PORT_SOFTSERIAL_TX_PORT   -1
// If you use SOFTSERIAL you can also choose if it is inverted or not. This is not
// posible when using hardware serial RX pin (always not-inverted)
// BYONICS GPS mandatory option!
#define GPS_PORT_SOFTSERIAL_INVERTED true


// --------------------------------------------------------------------------
// Modem config (modem.cpp)
// --------------------------------------------------------------------------

// AUDIO_PIN is the audio-out pin. The audio is generated by timer 2 using
// PWM, so the only two options are pins 3 and 11.
// Pin 11 doubles as MOSI, so I suggest using pin 3 for PWM and leave 11 free
// in case you ever want to interface with an SPI device.
#define AUDIO_PIN       3

// Radio: I've tested trackuino with two different radios:
// Radiometrix HX1 and SRB MX146. The interface with these devices
// is implemented in their respective radio_*.cpp files, and here
// you can choose which one will be hooked up to the tracker.
// The tracker will behave differently depending on the radio used:
//
// RadioHx1 (Radiometrix HX1):
// - Time from PTT-on to transmit: 5ms (per datasheet)
// - PTT is TTL-level driven (on=high) and audio input is 5v pkpk
//   filtered and internally DC-coupled by the HX1, so both PTT
//   and audio can be wired directly. Very few external components
//   are needed for this radio, indeed.
//
// RadioMx146 (SRB MX146):
// - Time from PTT-on to transmit: signaled by MX146 (pin RDY)
// - Uses I2C to set freq (144.8 MHz) on start
// - I2C requires wiring analog pins 4/5 (SDA/SCL) via two level
//   converters (one for each, SDA and SCL). DO NOT WIRE A 5V ARDUINO
//   DIRECTLY TO THE 3.3V MX146, YOU WILL DESTROY IT!!!
//
//   I2C 5-3.3v LEVEL TRANSLATOR:
//
//    +3.3v o--------+-----+      +---------o +5v
//                   /     |      /
//                R  \     |      \ R
//                   /    G|      /
//              3K3  \   _ L _    \ 4K7
//                   |   T T T    |
//   3.3v device o---+--+|_| |+---+---o 5v device
//                     S|     |D
//                      +-|>|-+
//                             N-channel MOSFET
//                           (BS170, for instance)
//
//   (Explanation of the lever translator:
//   http://www.neoteo.com/adaptador-de-niveles-para-bus-i2c-3-3v-5v.neo)
//
// - Audio needs a low-pass filter (R=8k2 C=0.1u) plus DC coupling
//   (Cc=10u). This also lowers audio to 500mV peak-peak required by
//   the MX146.
//
//                   8k2        10uF
//   Arduino out o--/\/\/\---+---||---o
//                     R     |     Cc
//                          ===
//                     0.1uF | C
//                           v
//
// - PTT is pulled internally to 3.3v (off) or shorted (on). Use
//   an open-collector BJT to drive it:
//        
//                             o MX146 PTT
//                             |
//                    4k7    b |c
//   Arduino PTT o--/\/\/\----K  (Any NPN will do)
//                     R       |e
//                             |
//                             v GND
// 
// - Beware of keying the MX146 for too long, you will BURN it.
//
// So, summing up. Options are:
//
// - RadioMx146
// - RadioHx1
#define RADIO_CLASS   RadioHx1

// --------------------------------------------------------------------------
// Radio config (radio_*.cpp)
// --------------------------------------------------------------------------

// This is the PTT pin
#define PTT_PIN           8

// This is the pin used by the MX146 radio to signal full RF
// #define MX146_READY_PIN   8

// --------------------------------------------------------------------------
// Sensors config (sensors.cpp)
// --------------------------------------------------------------------------

//#define INTERNAL_LM135_VOUT_PIN   0
#define INTERNAL_LM60_VOUT_PIN   0
// Units for temperature sensors (Added by: Kyle Crockett)
// 1 = Celsius, 2 = Kelvin, 3 = Fahrenheit
#define TEMP_UNIT 1

// Calibration value in the units selected. Use integer only.
#define CALIBRATION_VAL 0


#define INTERNAL_BATTERY_VOUT_PIN   2
#define EXTERNAL_BATTERY_VOUT_PIN   1


// --------------------------------------------------------------------------
// Buzzer config (buzzer.cpp)
// --------------------------------------------------------------------------

// This is the buzzer frequency. Choose one that maximizes the output volume
// according to your buzzer's datasheet. It must be between L and 65535,
// where L = F_CPU / 65535 and F_CPU is the clock frequency in hertzs. For
// Arduinos (16 MHz), that gives a low limit of 245 Hz.
//#define BUZZER_FREQ             895     // Hz
#define BUZZER_FREQ            2048     // Hz

// These are the number of seconds the buzzer will stay on/off alternately
// Initial values. Could be changed calling buzzer_time( ontime,float offtime)
#define BUZZER_ON_TIME          0.01       // secs
#define BUZZER_OFF_TIME         1.       // secs

// This option disables the buzzer above BUZZER_ALTITUDE meters. This is a
// float value, so make it really high (eg. 1000000.0 = 1 million meters)
// if you want it to never stop buzzing.
#define BUZZER_ALTITUDE         3000.0  // meters (divide by 0.3048 for ft.)

// The buzzer is driven by timer 1, so the options here are pin 9 or 10
#define BUZZER_PIN              9

// --------------------------------------------------------------------------
// Debug
// --------------------------------------------------------------------------

// This is the LED pin (13 on Arduinos). The LED will be on while the AVR is
// running and off while it's sleeping, so its brightness gives an indication
// of the activity.
#define LED_PIN                 13

// Debug info includes printouts from different modules to aid in testing and
// debugging.
// 
// 1. To properly receive debug information, only connect the Arduino RX pin 
//    to the GPS TX pin, and leave the Arduino TX pin disconnected. 
//
// 2. On the serial monitor, set the baudrate to GPS_BAUDRATE (above),
//    usually 9600.
//
// 3. When flashing the firmware, disconnect the GPS from the RX pin or you
//    will get errors.
#define DEBUG_GPS    // GPS sentence dump and checksum validation
#define DEBUG_AX25   // AX.25 frame dump
//
#define DEBUG_MODEM  // Modem ISR overrun and profiling
#define DEBUG_LOGIC  // logic flow control
#define DEBUG_RESET  // AVR reset


#endif


#include <icrmacros.h>

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

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation.
#if ARDUINO < 22
#error "Oops! We need Arduino 22 or later"
#endif

// Trackuino custom libs
#include "aprs.h"
#include "ax25.h"
#include "buzzer.h"
#include "config.h"
#include "debug.h"
#include "gps.h"
#include "modem.h"
#include "radio.h"
#include "radio_hx1.h"
#include "sensors.h"
#include "logic.h"

// Arduino/AVR libs
#include <WProgram.h>
#include <avr/power.h>
#include <avr/sleep.h>


//GPS connected to serial soft. Only alternative if serial signal is inverted
#ifdef GPS_PORT_SOFTSERIAL_RX_PORT
#include <NewSoftSerial.h>
NewSoftSerial gpsSerial =  NewSoftSerial(GPS_PORT_SOFTSERIAL_RX_PORT, GPS_PORT_SOFTSERIAL_TX_PORT,GPS_PORT_SOFTSERIAL_INVERTED);
#endif

unsigned long next_tx_millis;



void disable_bod_and_sleep()
{
  /* This will turn off brown-out detection while
   * sleeping. Unfortunately this won't work in IDLE mode.
   * Relevant info about BOD disabling: datasheet p.44
   *
   * Procedure to disable the BOD:
   *
   * 1. BODSE and BODS must be set to 1
   * 2. Turn BODSE to 0
   * 3. BODS will automatically turn 0 after 4 cycles
   *
   * The catch is that we *must* go to sleep between 2
   * and 3, ie. just before BODS turns 0.
   */
  unsigned char mcucr;

  cli();
  mcucr = MCUCR | (_BV(BODS) | _BV(BODSE));
  MCUCR = mcucr;
  MCUCR = mcucr & (~_BV(BODSE));
  sei();
  sleep_mode();    // Go to sleep
}

void power_save()
{
  /* Enter power saving mode. SLEEP_MODE_IDLE is the least saving
   * mode, but it's the only one that will keep the UART running.
   * In addition, we need timer0 to keep track of time, timer 1
   * to drive the buzzer and timer2 to keep pwm output at its rest
   * voltage.
   */

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();
 
  digitalWrite(LED_PIN, LOW);
  sleep_mode();    // Go to sleep
  digitalWrite(LED_PIN, HIGH);
  
  sleep_disable();  // Resume after wake up
  power_all_enable();

}

int get_gps_fix(int not_timeout=0) {
  int c,decode_flag=0,n_fix=1;
  unsigned long gps_fix_timeout;
  int timeout_flag=0;
  static int n_timeouts=0;
  
//SET GPS TIMEOUT  
  gps_fix_timeout=millis()+GPS_FIX_TIMEOUT;
  
// SWITCH ON GPS
#ifdef GPS_ON_PIN
  digitalWrite(GPS_ON_PIN,LOW);
#endif 
  
// SWITCH ON SERIAL PORT
while(modem_busy()) {}; //Asure modem is off. Double check
#ifdef GPS_PORT_SOFTSERIAL_RX_PORT
  gpsSerial.begin(GPS_BAUDRATE);
  delay(50);
  gpsSerial.flush();
#endif 

//WAIT FOR A FIX (or timeout)
#ifdef GPS_ON_PIN
// Get N_FIX fix to improve GPS acuracy if GPS is switching on/off
while (!(decode_flag=gps_decode(c)) || (n_fix<N_FIX)) { 
      if (decode_flag==1) {
          n_fix++; 
          decode_flag=0;
      }    
#else      
while (!gps_decode(c)) { 
#endif  
      if ((millis()>=gps_fix_timeout) && !not_timeout)  {
         timeout_flag=1;
         n_timeouts++;
#ifdef DEBUG_GPS
          Serial.print("TIMEOUT!");
          Serial.println(n_timeouts);
#endif  
        break;
      }  
#ifdef GPS_PORT_SOFTSERIAL_RX_PORT
      if (gpsSerial.available()) {
        c = gpsSerial.read();
#else
      if (Serial.available()) {
        c = Serial.read();
#endif    
      } 
}

//SWITCH OFF GPS. DO NOT DO IT IF THERE IS NOT FIX.
#ifdef GPS_ON_PIN  
    if (!timeout_flag) {
          digitalWrite(GPS_ON_PIN,HIGH);
          n_timeouts=0;
    }  
#endif

// Flush and switch off serialport
#ifdef GPS_PORT_SOFTSERIAL_RX_PORT
    gpsSerial.flush();
    gpsSerial.end();
    delay(100);
#endif        

return n_timeouts;
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.begin(GPS_BAUDRATE);

#ifdef DEBUG_RESET
  Serial.println("RESET");
  Serial.println(S_CALLSIGN);
#endif
  modem_setup();
  buzzer_setup();
  sensors_setup();
  gps_setup();



//Buzzer test. Reset sound
          buzzer_on();
          buzzer_power(500);
          buzzer_time(0.02,.2);
          delay(2000);
          buzzer_off();

#ifdef SEND_TELEMETRY
     aprs_send_telemetry_definition();
#endif

get_gps_fix(1); //try to get first fix from gps. Wait until that.
                //BEWARE TO SET 1 BEFORE LAUNCH DAY!!!!

 
//Send first APRS msg
          aprs_send("OK");
#ifdef SEND_TELEMETRY
          delay(5000);
          aprs_send_telemetry("OK");
#endif  


// Schedule the next transmission within APRS_DELAY ms
  next_tx_millis = millis() + APRS_DELAY;

//Start CAMS circuit  
delay(500);
//Serial.println("CAMs Countdown");
shutter_countdown();  
//Serial.println("CAMs started");
}

void loop()
{
  unsigned int timeouts;
  char temp[]="BAD:     ";   
  if (millis() >= next_tx_millis) {  
    //write_eeprom((byte) sensors_int_bat());
    // Show modem ISR stats from the previous transmission
#ifdef DEBUG_MODEM
    modem_debug();
#endif
    buzzer_off();  //Switch off the buzzer to avoid interferences with serialsoft and minimize simultaneos power demand.
    if((timeouts=get_gps_fix())==0) {
          buzzer_power(600);
          buzzer_time(0.05,1.);
          aprs_send("OK");
#ifdef SEND_TELEMETRY
          delay(5000);
          aprs_send_telemetry("OK");
#endif               
    } else {
          snprintf(&temp[5], 4, "%03u", (int) timeouts);
          buzzer_power(400);
          buzzer_time(0.01,0.1);
          aprs_send(temp); 
#ifdef SEND_TELEMETRY
          delay(5000);
          aprs_send_telemetry(temp);
#endif          
                       
    }  
      if (gps_altitude > BUZZER_ALTITUDE) {
        //In space reduce the buzzer lasting to 5s every APRS message
              buzzer_power(600);
              buzzer_time(0.9,0.1);
              buzzer_on();
              delay(5000);                         //5 seconds
              buzzer_off();
       } else {
          buzzer_on();
      }
      // Schedule the next transmission
      if (millis()/1000 < APRS_SLOW_CYCLE_START)
          next_tx_millis = millis() + APRS_PERIOD;
      else
          next_tx_millis = millis() + APRS_SLOW_PERIOD;
  } else {
    power_save();
  }
}




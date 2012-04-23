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

#include "config.h"
#include "ax25.h"
#include "gps.h"
#include "aprs.h"
#include "sensors.h"
#include <stdio.h>
#include <stdlib.h>
#include <WProgram.h>
#include <avr/pgmspace.h>

// Module functions
float meters_to_feet(float m)
{
  // 10000 ft = 3048 m
  return m / 0.3048;
}

#define MAX_TEMP_SIZE 72
char temp[MAX_TEMP_SIZE]; 

const struct s_address addresses[] = { 
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
};
  
// Exported functions
void aprs_send(char* msg)
{

  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte('/');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
  // ax25_send_string("021709z");     // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
  ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  ax25_send_byte('h');
  ax25_send_string(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte('/');                // Symbol table
  ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
  ax25_send_byte('O');                // Symbol: O=balloon, -=QTH
  snprintf(temp, 4, "%03d", (int)(gps_course + 0.5)); 
  ax25_send_string(temp);             // Course (degrees)
  ax25_send_byte('/');                // and
  snprintf(temp, 4, "%03d", (int)(gps_speed + 0.5));
  ax25_send_string(temp);             // speed (knots)
  ax25_send_string("/A=");            // Altitude (feet). Goes anywhere in the comment area
  snprintf(temp, 7, "%06ld", (long)(meters_to_feet(gps_altitude) + 0.5));
  ax25_send_string(temp);
  ax25_send_string("/Ti=");
  ax25_send_string(itoa(sensors_int_lm60(), temp, 10));  //Temperature
  ax25_send_string("/Vi=");
  ax25_send_string(itoa(sensors_int_bat(), temp, 10));   //Internal Battery
  ax25_send_string("/Ve=");
  ax25_send_string(itoa(sensors_ext_bat(), temp, 10));   //External Battery
  ax25_send_string("/UV=");
  ax25_send_string(itoa(sensors_uva(), temp, 10));       //UV sensor readding
  ax25_send_string("/DOP=");
  ax25_send_string(itoa((int)(gps_dop*10), temp, 10));  //Dilution of Precision x 10. Values near 10 or lower are the best.
  ax25_send_byte(' ');
  ax25_send_string(APRS_COMMENT);     // Comment
  ax25_send_byte(' ');
  ax25_send_string(msg);     // Comment
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
}

#ifdef SEND_TELEMETRY

void aprs_send_telemetry(char* msg) {
  static int sequence_n=0;

  //Do not send if time is longer than APRS_SLOW_CYCLE_START from boot
  if (millis()/1000 >= APRS_SLOW_CYCLE_START)
      return;
  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_string("T#");
  sequence_n++;
  snprintf(temp, 4, "%03u", (int) sequence_n);
  ax25_send_string(temp);             // telemetry sequence number
  ax25_send_byte(',');
  snprintf(temp, 4, "%03u", (int) sensors_int_lm60()+273);
  ax25_send_string(temp);             // telemetry internal temperature
  ax25_send_byte(',');  
  snprintf(temp, 4, "%03u", (int) sensors_int_bat());
  ax25_send_string(temp);             // telemetry internal bat
  ax25_send_byte(',');  
  snprintf(temp, 4, "%03u", (int) sensors_ext_bat());
  ax25_send_string(temp);             // telemetry external bat
  ax25_send_byte(',');  
  snprintf(temp, 4, "%03u", (int) gps_altitude/50);
  ax25_send_string(temp);             // telemetry altitude (meters)
  ax25_send_byte(',');  
  snprintf(temp, 4, "%03u", (int) (gps_speed*1.852)); 
  ax25_send_string(temp);             // telemetry speed (km/h)
  ax25_send_string(",00100000");  
  ax25_send_byte(' ');
  ax25_send_string(APRS_COMMENT);     // Comment
  ax25_send_byte(' ');
  ax25_send_string(msg);     // Comment
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
 
}

prog_char string_0[MAX_TEMP_SIZE] PROGMEM ="PARM.Temp,Bat,Bat2,Alt,Vel,UP,DOWN,STP,FIX,B1,B2,bit7,bit8\0";
prog_char string_1[MAX_TEMP_SIZE] PROGMEM ="UNIT.Celsiu,Volts,Volts,meters,km/h,up,down,on,fix,ok,ok,on,on\0";
prog_char string_2[MAX_TEMP_SIZE] PROGMEM ="EQNS.0,1,-273,0,0.1,0,0,0.1,0,0,50,0,0,1,0\0";
prog_char string_3[MAX_TEMP_SIZE] PROGMEM ="BIT.1,1,1,1,0,0,1,1,NARANJO-2012 BALLOON\0";
PROGMEM const char *telemetry_table[4]= {
  string_0,
  string_1,
  string_2,
  string_3,
};



void aprs_send_telemetry_definition_msg(int msg_n) {


  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte(':');
  ax25_send_string( S_CALLSIGN);
  ax25_send_byte('-');
  ax25_send_string(itoa(S_CALLSIGN_ID, temp, 10));
  ax25_send_byte(':');
  strcpy_P(temp, (char*)pgm_read_word(&(telemetry_table[msg_n])));
  ax25_send_string(temp);
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
  
}

void aprs_send_telemetry_definition()
{
  for (int i=0;i<4;i++) {
    aprs_send_telemetry_definition_msg(i); 
    delay(TELEMETRY_DEFINITION_DELAY);
  }
}

#endif

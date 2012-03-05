/* trackuino copyright (C) 2010  EA5HAV Javi
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

/* Credit to:
 *
 * cathedrow for this idea on using the ADC as a volt meter:
 * http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */

#include "config.h"
#include "sensors.h"
#include <WProgram.h>

/*
 * sensors_aref: measure an external voltage hooked up to the AREF pin,
 * optionally (and recommendably) through a pull-up resistor. This is
 * incompatible with all other functions that use internal references
 * (see config.h)
 */

void sensors_setup()
{

}

#ifdef INTERNAL_LM60_VOUT_PIN 
/*  Temp Sensor LM60 */
int sensors_int_lm60()
{
  analogReference(DEFAULT);      // Ref=1.1V. Okay up to 108 degC (424 + 6.25*108 = 1100mV)
  analogRead(INTERNAL_LM60_VOUT_PIN);            // Disregard the 1st conversion after changing ref (p.256)
  int adc = analogRead(INTERNAL_LM60_VOUT_PIN);  // Real read
  //int mV = 1100L * adc / 1024L;   // Millivolts
  int mV = 5000L * adc / 1024L;   // Millivolts
  switch (TEMP_UNIT)//Added by: Kyle Crockett
  {
	case 1://C
		return (4L * (mV - 424) / 25)+ CALIBRATION_VAL ;    // Vo(mV) = (6.25*T) + 424 -> T = (Vo - 424) * 100 / 625
	break;
	case 2://K
		return (4L * (mV - 424) / 25) + 273 + CALIBRATION_VAL; //C + 273 = K
	break;
	case 3://F
		return (36L * (mV - 424) / 125) + 32+ CALIBRATION_VAL; // (9/5)C + 32 = F
	break;
  };
}
#endif

#ifdef INTERNAL_LM135_VOUT_PIN 
/*  Temp Sensor LM135 */
int sensors_int_lm135() {
  analogReference(DEFAULT);
  analogRead(INTERNAL_LM35_VOUT_PIN);            // Disregard the 1st conversion after changing ref (p.256)
  int adc = analogRead(INTERNAL_LM135_VOUT_PIN );  // Real read
  return (5.0 * adc * 100.0)/1024.0-273.15;
}
#endif

/* Internal Battery sensor */
int sensors_int_bat() {
  analogReference(DEFAULT);
  analogRead(INTERNAL_BATTERY_VOUT_PIN);            // Disregard the 1st conversion after changing ref (p.256)
  int adc = analogRead(INTERNAL_BATTERY_VOUT_PIN);  // Real read
  return (5.0 * adc * 3. * 10.) /1024.0;            // 3 -> resistor are chossen to get 1/3 of Vbat. In my case 20k+10k 
}

/* External Battery sensor */
int sensors_ext_bat() {
  analogReference(DEFAULT);
  analogRead(EXTERNAL_BATTERY_VOUT_PIN);            // Disregard the 1st conversion after changing ref (p.256)
  int adc = analogRead(EXTERNAL_BATTERY_VOUT_PIN);  // Real read
  return (5.0 * adc * 3. * 10.) /1024.0;            // 3 -> resistor are chossen to get 1/3 of Vbat. In my case 20k+10k 
}

int sensors_acelerometer()
{
  // TO DO
  return 0;
}

int sensors_uva()
{
  // TO DO
  return 0;
}


int sensors_humidity()
{
  // TO DO
  return 0;
}

int sensors_pressure()
{
  // TO DO
  return 0;
}

int sensors_uv_ray()
{
  // Nice to have at 40 km altitude
  return 0;
}

int sensors_gamma_ray()
{
  // http://www.cooking-hacks.com/index.php/documentation/tutorials/geiger-counter-arduino-radiation-sensor-board
  return 0;
}

int sensors_graviton()
{
  // Wait, what?
  return 0;
}



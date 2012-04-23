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

/* Credit to:
 *
 * cathedrow for this idea on using the ADC as a volt meter:
 * http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */

#include "config.h"
#include "sensors.h"
#include <WProgram.h>

/* UV sensor definitions */
#define UVnumSensors 1    // Number of LED sensors
byte UVanodePin[] =  { UV_ANODE };   // UV,GR,IR Leds
byte UVcathodePin[] = { UV_CATHODE }; // the analogue input pins 0 to 5 when used digitally
int UVresults[3];


/*
 * sensors_aref: measure an external voltage hooked up to the AREF pin,
 * optionally (and recommendably) through a pull-up resistor. This is
 * incompatible with all other functions that use internal references
 * (see config.h)
 */

void sensors_setup()
{
  UVsensor_setup();
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
  int v;
  UVsensor_read_n(UV_SENSOR_N_SAMPLES); 
  v=999-UVresults[0]*999./1024.;
  return v;
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

/* UV LED Sensors subroutines
Nacho Mas 2012
Base on the work of - Mike Cook
*/


void UVsensor_setup() {
//  _SFR_IO8(0x35) |= 0x10;   // global disable pull up resistors
 
  for(int i=0; i<UVnumSensors; i++){
  digitalWrite(UVanodePin[i],LOW); // ensure pins go low immediatly after initilising them to outputs
  pinMode(UVanodePin[i],OUTPUT);   // declare the anode pins as outputs
  pinMode(UVcathodePin[i],INPUT); // declare the cathode pins as inputs
  }
}

//Mean value. Take n samples
void UVsensor_read_n(int n){
    long int dummy[UVnumSensors]={0};
    for(int j=0; j<n; j++){
        UVsensor_read();
        for(int i=0; i<UVnumSensors; i++){
            dummy[i]+=UVresults[i];
        }  
    }    
    for(int i=0; i<UVnumSensors; i++){
        UVresults[i] = dummy[i]/n;
    }  
    
}

//read sensosrs
void UVsensor_read() {
 // Discharge: turn all LEDs on
  for(int i=0; i<UVnumSensors; i++){
    digitalWrite(UVanodePin[i],HIGH);
    pinMode(UVcathodePin[i],OUTPUT);    // Enable cathode pins as outputs
    digitalWrite(UVcathodePin[i],LOW);  // Turn ON LED
  } 
   // charge up LEDs cathode = HIGH, anode = LOW
  for(int i=0; i<UVnumSensors; i++){
    digitalWrite(UVanodePin[i],LOW); 
    digitalWrite(UVcathodePin[i],HIGH);  
  }
  
  delay(1);
    // Put cathode pins into measuring state (analogue input)
  for(int i=0; i<UVnumSensors; i++){
    pinMode(UVcathodePin[i],INPUT);
    digitalWrite(UVcathodePin[i],LOW);
  }
  // Take a reading of the voltage level on the inputs to get a referance level before discharge
   /*for(int i=0; i<UVnumSensors; i++){
     analogRead(UVcathodePin[i]-14);//Discard
     UVresults[i] = analogRead(UVcathodePin[i]-14);  // Store the referance level
   }*/
  //**********************************************************
  delay(UV_SENSOR_PHOTON_INTEGRATION_TIME);  // LED discharge time or photon intergration time
              // The larger this is, the more sensitave is the system
  //**********************************************************

 // Read the sensors after discharge to measure the incedent light
    // for(int i=UVnumSensors-1; i>-1; i--){  // reverse order reduces start to finish cross talk
     for(int i=0; i<UVnumSensors; i++){  // reverse order reduces start to finish cross talk
     analogRead(UVcathodePin[i]-14);; //Discard
     //UVresults[i] -= analogRead(UVcathodePin[i]-14);     // subtract current reading from the referance to give the drop
     UVresults[i] = analogRead(UVcathodePin[i]-14);     // subtract current reading from the referance to give the drop
     pinMode(UVcathodePin[i],OUTPUT);   // by discharging the LED immediatly the charge on the A/D input is removed and
     digitalWrite(UVcathodePin[i],LOW); // the cross talk between sensors is reduce
     
  }

}


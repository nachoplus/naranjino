#include "logic.h"
#include "config.h"
#include "buzzer.h"
#include <WProgram.h>

/*
int get_free_memory() {
  int size = 2048; // Use 2048 with ATmega328
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
}*/

//Camara shutter routine

void shutter_countdown() {    
          pinMode(CAM_SHUTTER_PIN,OUTPUT);
          buzzer_on();
          buzzer_power(500);
          //buzzer_time(0.5,0.5);
          //delay(CAM_COUNTDOWN*1000);
          for (int i=1;i<=CAM_COUNTDOWN;i++) {             
              buzzer_time(CAM_COUNTDOWN*0.01/(float)i,CAM_COUNTDOWN*0.1/(float)i);
              delay(1000);
          }
          buzzer_time(0.9,0.1);
          digitalWrite(CAM_SHUTTER_PIN, HIGH);
          delay(CAM_PIC_ON_TIME*1000);
          digitalWrite(CAM_SHUTTER_PIN, LOW);
          buzzer_off();
}


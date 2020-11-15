/*
 * main.c
 *
 *  Created on: Nov 14, 2020
 *      Author: steveec9
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
//#include <pigpio.h>

#define MSG_OUT_PIN 15
#define PWM_FREQ  1000

int main(int argc, char *argv[]){
/*	  if (gpioInitialise() < 0){
	        printf("Could not initialize pigpio !\n");
	        return 1;
	      }

	  int res = gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, PWM_FREQ);
	  printf("Result %d\n", res);

	  for (int i = 0; i < 10000; i++) {
		  usleep(1000);
	  }

	  gpioTerminate();*/
	printf("Hello world");
}


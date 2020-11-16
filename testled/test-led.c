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
#include <pigpio.h>

#define MSG_OUT_PIN 12
#define PWM_FREQ  38000

int main(int argc, char *argv[]){
	  if (gpioInitialise() < 0){
	        printf("Could not initialize pigpio !\n");
	        return 1;
	      }
	int res = gpioSetMode(MSG_OUT_PIN, PI_ALT0);

	printf("Set Mode Result %d\n", res);	
	res = gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, 500000);
	printf("PWM Result %d\n", res);

	  for (int i = 0; i < 300; i++) {
		  usleep(1000);
	  }
	res = gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, 0);
	printf("PWM End Result %d\n", res);  
	gpioTerminate();
	  printf("Hello world");
}


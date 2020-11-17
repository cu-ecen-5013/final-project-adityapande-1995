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

	  for (int i = 0; i < 100; i++) {
		  res = gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, 500000);
		  printf("PWM On Result %d\n", res);
		  usleep(500*1000);
		  res = gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, 0);
		  printf("PWM Off Result %d\n", res);
		  usleep(500*1000);
	  }
	 gpioTerminate();
	 printf("gpioTerminate result:  %d\n", res);
}


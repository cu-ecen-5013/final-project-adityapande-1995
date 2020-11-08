#include <stdio.h>
#include <pigpio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "alphabet.c"

#define CLOCK_IN_PIN 20
#define MSG_PIN 21
#define MAX_SAMPLES 1000

char state[MAX_SAMPLES];
int state_index_to_write = 0;

void clock_callback(int gpio, int level, uint32_t tick){
  /*  If rising edge is detected in clock */
  if (level == 1){
    int pin_state = gpioRead(MSG_PIN);
    printf("\n Msg pin state : %d", pin_state);
  }
}

int main(){

  if (gpioInitialise() < 0){
    printf("Could not initialize pigio !\n");
    return 1;
  }
  gpioSetMode(CLOCK_IN_PIN, PI_INPUT);
  gpioSetMode(MSG_PIN, PI_INPUT);

  /*  Set up callback funtion for rising edge of clock */
  gpioSetAlertFunc(CLOCK_IN_PIN, clock_callback);


  printf("\nReady to receive data !");
  while (1){
    sleep(2);
  }

  return 0;
}


#include <stdio.h>
#include <pigpio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "alphabet.c"

#define CLOCK 0.5
#define CLOCK_FLASH_TIME 0.01
#define TOTAL_TIME 60.0

bool DONE = false;
int clock_state = 0;
void send_morse(char text[], int len){

  char dot_or_dash;
  alphabet current_code;

  for (int i = 0; i < len; i++){
    printf("\n\n** Sending new character !");
    /*  Get morse code for current alphabet */
    current_code = get_morse_code(text[i]);
    printf("\n Current char = %c, code : %s, %d", text[i], current_code.code, current_code.len);
    /*  Iterate over code and send it */
    for (int j=0; j< current_code.len ; j++){
      printf("\n Sending new dot/dash..");
      dot_or_dash = current_code.code[j];
      printf("\n Writing %c", dot_or_dash);
      gpioWrite(15, 1);
      if (dot_or_dash == 'o'){
        printf("\nSending dot..");
        time_sleep(CLOCK);
      }
      if (dot_or_dash == 'd'){
        printf("\nSending dash..");
        time_sleep(3*CLOCK);
      }
      printf("\nDot/Dash sending complete..");
      gpioWrite(15,0);
      time_sleep(CLOCK);
    }
  }

  printf("\n Message sent !");
  DONE = true;
}

void* clock_thread(void* args){
  int* pin = (int*)args;
  /* Set GPIO modes -- physical 8 is 14 for pi 3b */
  double start = time_time();

  while ( ((time_time() - start) < TOTAL_TIME) && !DONE ){
      gpioWrite(*pin, 1);
      clock_state = 1;
      time_sleep(CLOCK_FLASH_TIME);
      gpioWrite(*pin, 0);
      clock_state = 0;
      time_sleep(CLOCK-CLOCK_FLASH_TIME);
    }
}


int main(int argc, char *argv[])
{
  if (gpioInitialise() < 0){
      printf("Could not initialize pigio !\n");
      return 1;
    }

  gpioSetMode(15, PI_OUTPUT);
  gpioSetMode(14, PI_OUTPUT);

  /*  Clock thread */
  pthread_t clock_id;
  int clock_pin = 14;
  pthread_create(&clock_id, NULL, clock_thread, &clock_pin);

  /*  Morse code thread */
  char to_send[] = "SOS";
  send_morse(to_send, 3);

  pthread_join(clock_id, NULL);

  /* Stop DMA, release resources */
  gpioTerminate();

  return 0;
}

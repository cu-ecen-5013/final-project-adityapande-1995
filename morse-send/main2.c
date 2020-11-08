#include <stdio.h>
#include <pigpio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include "alphabet.c"

#define CLOCK 0.1
#define CLOCK_FLASH_TIME 0.05
#define TOTAL_TIME 60.0

#define CLOCK_OUT_PIN 14
#define MSG_OUT_PIN 15

#define CLOCK_IN_PIN 20
#define MSG_IN_PIN 21
#define RECEIVE_BUFFER_SIZE 1000

#define SEND_VERBOSE false


bool DONE = false;
int clock_state = 0;

int receiver_buffer[RECEIVE_BUFFER_SIZE];
int receive_buffer_index = 0;

void send_morse(char* text){

  int len = strlen(text);
  printf("\nLength of string to send : %d", len);
  char dot_or_dash;
  alphabet current_code;

  for (int i = 0; i < len; i++){
      if(SEND_VERBOSE) printf("\n\n** Sending new character !");
      /*  Get morse code for current alphabet */
      current_code = get_morse_code(text[i]);

      if(SEND_VERBOSE) printf("\n Current char = %c, code : %s, %d", text[i], current_code.code, current_code.len);
      /*  Iterate over code and send it */
      for (int j=0; j< current_code.len ; j++){
            if(SEND_VERBOSE) printf("\n Sending new dot/dash..");
            dot_or_dash = current_code.code[j];
            if(SEND_VERBOSE) printf("\n Writing %c", dot_or_dash);

            gpioWrite(MSG_OUT_PIN, 1);

            if (dot_or_dash == 'o'){
                    if(SEND_VERBOSE) printf("\nSending dot..");
                    time_sleep(CLOCK);
                  }
            if (dot_or_dash == 'd'){
                    if(SEND_VERBOSE) printf("\nSending dash..");
                    time_sleep(3*CLOCK);
                  }

            if(SEND_VERBOSE) printf("\nDot/Dash sending complete..");
            gpioWrite(MSG_OUT_PIN,0);
            time_sleep(CLOCK);
          }
      /* One alphabet sent*/
      gpioWrite(MSG_OUT_PIN,0);
      time_sleep(3*CLOCK);
    }

  printf("\n Message sent !\n");
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

void clock_in_callback(int gpio, int level, uint32_t tick){
  /*  If rising edge is detected in clock */
  if (level == 1){
      int pin_state = gpioRead(MSG_IN_PIN);
      //printf("\n Msg pin state : %d", pin_state);
      receiver_buffer[receive_buffer_index] = pin_state;
      receive_buffer_index++;
    }
}

void* sender_thread(void* arg){
  char* mytext = (char*)arg;
  send_morse(mytext);
  return NULL;
}

int main(int argc, char *argv[]){
  if (gpioInitialise() < 0){
        printf("Could not initialize pigio !\n");
        return 1;
      }

  // --------  Sending morse-code ----------
  gpioSetMode(MSG_OUT_PIN, PI_OUTPUT);
  gpioSetMode(CLOCK_OUT_PIN, PI_OUTPUT);

  /*  Clock out thread */
  pthread_t clock_id, sender_id;
  int clock_pin = CLOCK_OUT_PIN;
  pthread_create(&clock_id, NULL, clock_thread, &clock_pin);

  /*  Morse code output thread */
  char* to_send;
  if (argc < 2) to_send = "HELLOWORLD";
  else to_send = argv[1];
  printf("\nSending %s", to_send);
  pthread_create(&sender_id, NULL, sender_thread, (void*)to_send);
  
  /* **********  Receiver code  ************\/ */
  gpioSetMode(CLOCK_IN_PIN, PI_INPUT); 
  gpioSetMode(MSG_IN_PIN, PI_INPUT); 
  
  /*   Set up callback funtion for rising edge of clock */
  gpioSetAlertFunc(CLOCK_IN_PIN, clock_in_callback); 
  
  /*  Cleanup */
  pthread_join(clock_id, NULL);
  pthread_join(sender_id, NULL);
  gpioTerminate();

  /* Print results */
  printf("\n\n Receiver buffer :");
  for (int i = 0; i < receive_buffer_index; i++ ) printf("%d",receiver_buffer[i]);
  printf("\n Length of buffer: %d\n", receive_buffer_index);
  
  return 0;
}


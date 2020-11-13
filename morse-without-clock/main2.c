#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include "alphabet.c"

pthread_mutex_t lock;
double CLOCK_MS; // CLOCK time in milliseconds
bool PWM_ENABLE;

#define TOTAL_TIME 60.0
#define MSG_OUT_PIN 15
#define MSG_IN_PIN 21

#define RECEIVE_BUFFER_SIZE 1000
#define SEND_VERBOSE false

bool DONE = false;

int receiver_buffer[RECEIVE_BUFFER_SIZE];
int receive_buffer_index = 0;

int UF_buffer[RECEIVE_BUFFER_SIZE*2] = {0};
uint32_t UF_timestamps[RECEIVE_BUFFER_SIZE*2] = {0};
int UFB_index = 1;

void send_morse(char* text){

  int len = strlen(text);
  printf("\nLength of string to send : %d", len);
  char dot_or_dash;
  alphabet current_code;

  for (int i = 0; i < len; i++){
      if(SEND_VERBOSE) printf("\n\n** Sending new character , CLOCK_MS = %f !", CLOCK_MS);
      /*  Get morse code for current alphabet */
      current_code = get_morse_code(text[i]);

      if(SEND_VERBOSE) printf("\n Current char = %c, code : %s, %d", text[i], current_code.code, current_code.len);
      /*  Iterate over code and send it */
      for (int j=0; j< current_code.len ; j++){
            if(SEND_VERBOSE) printf("\n Sending new dot/dash..");
            dot_or_dash = current_code.code[j];
            if(SEND_VERBOSE) printf("\n Writing %c", dot_or_dash);

	    if (dot_or_dash == '-') continue;

            if(!PWM_ENABLE){ gpioWrite(MSG_OUT_PIN, 1); }
	    else{ gpioPWM(MSG_OUT_PIN, 128);  }

            if (dot_or_dash == 'o'){
                    if(SEND_VERBOSE) printf("\nSending dot..");
                    usleep(CLOCK_MS*1000);
                  }
            if (dot_or_dash == 'd'){
                    if(SEND_VERBOSE) printf("\nSending dash..");
                    usleep(CLOCK_MS*3*1000);
                  }

            if(SEND_VERBOSE) printf("\nDot/Dash sending complete..");

	    if(!PWM_ENABLE){ gpioWrite(MSG_OUT_PIN, 0); }
	    else{ gpioPWM(MSG_OUT_PIN, 0);  }

            usleep(CLOCK_MS*1000);
          }
      /* One alphabet sent*/
      if(!PWM_ENABLE){ gpioWrite(MSG_OUT_PIN, 0); }
      else{ gpioPWM(MSG_OUT_PIN, 0);  }

      usleep(CLOCK_MS*3*1000);
    }
  printf("\n Message sent !\n");
  DONE = true;
}

void msg_in_callback(int gpio, int level, uint32_t tick){
  pthread_mutex_lock(&lock);
  /*  If rising edge is detected in clock */
  if (level == 1){
    uint32_t timestamp = tick;
    UF_buffer[UFB_index] = 1; UF_timestamps[UFB_index] = timestamp;
    UFB_index++;
  }
  /*  Falling edge */
  if (level == 0){
    uint32_t timestamp = tick;
    UF_buffer[UFB_index] = -1; UF_timestamps[UFB_index] = timestamp;
    UFB_index++;
  }
  pthread_mutex_unlock(&lock);
}

void generate_receiver_buffer(){
  double fraction;
  for(int i = 1 ; i < UFB_index; i++){
    /*  Rising edge */
    if(UF_buffer[i] == 1){
      fraction = (UF_timestamps[i+1] - UF_timestamps[i])/(CLOCK_MS*1000);
      if (fraction > 0 && fraction < 1000){
        for (int j = 0 ; j < round(fraction); j++) receiver_buffer[j + receive_buffer_index] = 1;
        receive_buffer_index += round(fraction);
      }
    }
    if(UF_buffer[i] == -1){
      fraction = (UF_timestamps[i+1] - UF_timestamps[i])/(CLOCK_MS*1000);

      if (fraction > 0 && fraction < 1000){
        for (int j = 0 ; j < round(fraction); j++) receiver_buffer[j + receive_buffer_index] = 0;
        receive_buffer_index += round(fraction);
      }
    }
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
  pthread_mutex_init(&lock, NULL);

  // --------  Loopback init ----------
  gpioSetMode(MSG_OUT_PIN, PI_OUTPUT);
  gpioSetPWMfrequency(MSG_OUT_PIN, 10); 
  gpioSetMode(MSG_IN_PIN, PI_INPUT);  
  gpioSetISRFunc(MSG_IN_PIN,EITHER_EDGE,0,msg_in_callback);
  // Set PWM mode
  if (argc == 4){
	  gpioSetPWMfrequency(MSG_OUT_PIN, atof(argv[3]));
	  PWM_ENABLE = true;
	  printf("\n Running in PWM mode with freq %f!", atof(argv[3]));
  }else{
	  PWM_ENABLE = false;
	  printf("\n Running without PWM !");
  }
  // Set the clock
  if (argc >= 3)CLOCK_MS = atof(argv[2]);
  else CLOCK_MS = 3.0;
  /*  Morse code output thread */
  pthread_t sender_id;
  char* to_send;
  char padded_msg[1000];
  strcpy(padded_msg, "--");
  if (argc < 2) to_send = "-SOS";
  else{ 
	  strcat(padded_msg, argv[1]);
	  to_send = padded_msg;
  }
  printf("\nSending %s", to_send);
  pthread_create(&sender_id, NULL, sender_thread, (void*)to_send);
  
  /*  Cleanup */
  pthread_join(sender_id, NULL);
  gpioTerminate();

  /* Print results */
  //for(int i = 1; i < UFB_index; i++)printf("\n Edge %d at timestamp %u, frac with prev: %f", UF_buffer[i], UF_timestamps[i], (UF_timestamps[i] - UF_timestamps[i-1])/(CLOCK_MS*1000));
  generate_receiver_buffer();
  receive_buffer_index += 4; // Add EOW char
  printf("\n\n Receiver buffer :");
  for (int i = 0; i < receive_buffer_index; i++ ) printf("%d",receiver_buffer[i]);
  printf("\n Length of buffer: %d\n", receive_buffer_index);
  /*  Convert back to text */
  decode(receiver_buffer, receive_buffer_index);

  return 0;
}


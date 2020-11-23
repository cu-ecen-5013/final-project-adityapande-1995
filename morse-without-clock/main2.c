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
#include <time.h>
#include <signal.h>

pthread_mutex_t lock;
double CLOCK_MS; // CLOCK time in milliseconds
bool PWM_ENABLE;

#define TOTAL_TIME 			60.0
#define MSG_OUT_PIN 		12
#define MSG_IN_PIN 			18
#define PWM_FREQ			38000
#define DUTY_CYCLE			500000

#define HIGH			1
#define LOW				0


#define RECEIVE_BUFFER_SIZE 1000

#ifndef VERBOSE
#define VERBOSE 1
#endif

#if VERBOSE
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);
#else
#define DEBUG_PRINT(fmt, ...)
#endif

bool DONE = false;

int receiver_buffer[RECEIVE_BUFFER_SIZE];
int rx_buf_idx = 0;

struct timespec start;
struct timespec stop;

void send_morse(char *text) {

	int len = strlen(text);
	printf("\nLength of string to send : %d", len);
	char dot_or_dash;
	alphabet current_code;

	for (int i = 0; i < len; i++) {
		DEBUG_PRINT("\n\n** Sending new character , CLOCK_MS = %f !", CLOCK_MS);
		/*  Get morse code for current alphabet */
		current_code = get_morse_code(text[i]);

		DEBUG_PRINT("\n Current char = %c, code : %s, %d", text[i], current_code.code, current_code.len);
		/*  Iterate over code and send it */
		for (int j = 0; j < current_code.len; j++) {
			DEBUG_PRINT("\n Sending new dot/dash..");
			dot_or_dash = current_code.code[j];
			DEBUG_PRINT("\n Writing %c", dot_or_dash);

			if (dot_or_dash == '-'){
				continue;
			}

			if (!PWM_ENABLE) {gpioWrite(MSG_OUT_PIN, 1);} 
			else {gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, DUTY_CYCLE);}

			int delay_us;
			if (dot_or_dash == 'o') {
				DEBUG_PRINT("\nSending dot..");
				delay_us = CLOCK_MS * 1000;
			}
			else if (dot_or_dash == 'd') {
				DEBUG_PRINT("\nSending dash..");
				delay_us = CLOCK_MS * 3 * 1000;
			}

			usleep(delay_us);

			DEBUG_PRINT("\nDot/Dash sending complete..");

			if (!PWM_ENABLE) {gpioWrite(MSG_OUT_PIN, 0);} 
			else{gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, 0);}

			usleep(CLOCK_MS * 1000);
		}
		/* Send end of character */
		if (!PWM_ENABLE) {gpioWrite(MSG_OUT_PIN, 1);} 
		else {gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, DUTY_CYCLE);}
		usleep(CLOCK_MS*5*1000);

		if (!PWM_ENABLE) {gpioWrite(MSG_OUT_PIN, 0);} 
		else{gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, 0);}

		usleep(CLOCK_MS * 1000);

	}
	DEBUG_PRINT("\n Message sent !\n");
	DONE = true;
}

bool valid_edge = false;
void msg_in_callback(int gpio, int level, uint32_t tick) {
	pthread_mutex_lock(&lock);
	DEBUG_PRINT("\n Callback received at %u\n", tick);
	

	/*  If rising edge is detected in clock */
	if (level == HIGH && valid_edge) {
		double elapsed_us;

		valid_edge = false;

		int ret = clock_gettime(CLOCK_MONOTONIC, &stop);
		if (ret) {
			DEBUG_PRINT("Error: stop clock_gettime: %d\n", ret);
		}

	    elapsed_us = (stop.tv_sec - start.tv_sec) * 1000000 +
	    		(stop.tv_nsec - start.tv_nsec) / 1000;

	    if (elapsed_us > (CLOCK_MS * 4 * 1000)) {
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
		receiver_buffer[rx_buf_idx++] = 0;
	    	DEBUG_PRINT("Logic EOC received %f\n", elapsed_us);
		pthread_mutex_unlock(&lock);
		return;
	    }


	    if (elapsed_us > (CLOCK_MS * 2 * 1000)) {
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 0;
	    	DEBUG_PRINT("Logic 1 received %f\n", elapsed_us);
	    }
	    else {
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 0;
	    	DEBUG_PRINT("Logic 0 received %f\n", elapsed_us);
	    }

	}
	/*  Falling edge */
	else if (level == LOW) {
		valid_edge = true;
		int ret = clock_gettime(CLOCK_MONOTONIC, &start);
		if (ret) {
			DEBUG_PRINT("Error: start clock_gettime: %d\n", ret);
		}

		DEBUG_PRINT("Falling edge\n");
	}

	pthread_mutex_unlock(&lock);
}


void* sender_thread(void *arg) {
	char *mytext = (char*) arg;
	send_morse(mytext);
	DEBUG_PRINT("\n Returning from sender thread !");
	return NULL;
}

int main(int argc, char *argv[]) {
	if (gpioInitialise() < 0) {
		printf("Could not initialize pigio !\n");
		return 1;
	}
	pthread_mutex_init(&lock, NULL);

	// --------  Loopback init ----------
	gpioSetMode(MSG_OUT_PIN, PI_ALT0);
	gpioSetMode(MSG_IN_PIN, PI_INPUT);

	/* configure input as pull up since TSOP3082 is active low */
	gpioSetPullUpDown(MSG_IN_PIN, PI_PUD_UP);
	/* let I/O settle before configuring ISR*/
	usleep(100000);
	gpioSetISRFunc(MSG_IN_PIN, EITHER_EDGE, 0, msg_in_callback);

	// Set PWM mode
	if (argc == 4) {
		//gpioSetPWMfrequency(MSG_OUT_PIN, atof(argv[3]));
		// TODO: DOn't need to take frequency as argument. always 38 kHz
		PWM_ENABLE = true;
		printf("\n Running in PWM mode with freq %f!", atof(argv[3]));
	} else {
		PWM_ENABLE = false;
		printf("\n Running without PWM !");
	}
	// Set the clock
	if (argc >= 3)
		CLOCK_MS = atof(argv[2]);
	else
		CLOCK_MS = 3.0;
	/*  Morse code output thread */
	pthread_t sender_id;
	char *to_send;
	char padded_msg[1000];
	strcpy(padded_msg, "--");
	if (argc < 2)
		to_send = "-SOS";
	else {
		strcat(padded_msg, argv[1]);
		to_send = padded_msg;
	}
	printf("\nSending %s", to_send);


	pthread_create(&sender_id, NULL, sender_thread, (void*) to_send);
	DEBUG_PRINT("\n Sender thread spawned !");

	/*  Cleanup */
	pthread_join(sender_id, NULL);
	DEBUG_PRINT("\n Sender thread joined !");

	//generate_receiver_buffer();
	//rx_buf_idx += 4; // Add EOW char
	printf("\n\n Receiver buffer :");
	for (int i = 0; i < rx_buf_idx; i++)
		printf("%d", receiver_buffer[i]);
	printf("\n Length of buffer: %d\n", rx_buf_idx);
	/*  Convert back to text */
	decode(receiver_buffer, rx_buf_idx);

	//gpioTerminate();
	//DEBUG_PRINT(("\n Gpio terminated !"));
	//kill(getpid(), SIGKILL);

	return 0;
}


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
bool MSG_COMPLETE;
int EOC_COUNT;

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


int receiver_buffer[RECEIVE_BUFFER_SIZE];
int rx_buf_idx = 0;

struct timespec start;
struct timespec stop;

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

	    DEBUG_PRINT("\nclock ratio = %f\n", elapsed_us/(CLOCK_MS*1000));

	    // Separating character received 
	    if (elapsed_us > (CLOCK_MS * 4 * 1000)) {
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
		receiver_buffer[rx_buf_idx++] = 0;
	    	DEBUG_PRINT("Logic EOC received %f\n", elapsed_us);
		EOC_COUNT++;
		if (EOC_COUNT == 3)MSG_COMPLETE = true;
		pthread_mutex_unlock(&lock);
		return;
	    }


	    if (elapsed_us > (CLOCK_MS * 2 * 1000)) {
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 0;
		EOC_COUNT = 0;
	    	DEBUG_PRINT("Logic 1 received %f\n", elapsed_us);
	    }
	    else if(elapsed_us > (CLOCK_MS * 1 * 1000)) {
	    	receiver_buffer[rx_buf_idx++] = 1;
	    	receiver_buffer[rx_buf_idx++] = 0;
		EOC_COUNT = 0;
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

int main(int argc, char *argv[]) {
	if (gpioInitialise() < 0) {
		printf("Could not initialize pigio !\n");
		return 1;
	}
	pthread_mutex_init(&lock, NULL);

	EOC_COUNT = 0;
	MSG_COMPLETE = false;

	// --------  Input pin setup ----------
	gpioSetMode(MSG_IN_PIN, PI_INPUT);

	/* configure input as pull up since TSOP3082 is active low */
	gpioSetPullUpDown(MSG_IN_PIN, PI_PUD_UP);
	/* let I/O settle before configuring ISR*/
	usleep(100000);
	gpioSetISRFunc(MSG_IN_PIN, EITHER_EDGE, 0, msg_in_callback);

	// Set the clock
	if (argc >= 2)
		CLOCK_MS = atof(argv[1]);
	else{
		DEBUG_PRINT("\nClock rate needs to be provided !\n");
		exit(-1);
	}

	DEBUG_PRINT("\nSetup done with clock of %f ms ,waiting for signal !", CLOCK_MS);
	/* Wait till entire message is received */
	while(!MSG_COMPLETE)sleep(1.0);

	printf("\n\n Receiver buffer :");
	for (int i = 0; i < rx_buf_idx; i++)
		printf("%d", receiver_buffer[i]);
	printf("\n Length of buffer: %d\n", rx_buf_idx);
	/*  Convert back to text */
	decode(receiver_buffer, rx_buf_idx);

	return 0;
}


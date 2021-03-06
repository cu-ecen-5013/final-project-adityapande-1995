/*
 * file-transfer.c
 *
 *  Created on: Nov 14, 2020
 *      Author: Stephen Cool
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

struct send_thread_data {
	char *tx_data;
	int length;
};

struct send_thread_data send_thread_data;
pthread_mutex_t lock;
double CLOCK_MS; // CLOCK time in milliseconds

#define MSG_OUT_PIN 		12
#define MSG_IN_PIN 			18
#define PWM_FREQ			38000
#define DUTY_CYCLE			500000

#define HIGH			1
#define LOW				0

#define TX_BUF_SZ	1000

#define WRITEFILE "/var/tmp/IR_data.txt"


#ifndef VERBOSE
#define VERBOSE 1
#endif
#if VERBOSE
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);
#else
#define DEBUG_PRINT(fmt, ...)
#endif

int rx_bit_idx = 0;
char rx_byte = 0;

char tx_buf[TX_BUF_SZ] = { 0 };
int tx_buf_idx = 0;

struct timespec start;
struct timespec stop;

pthread_t sender_id;

bool valid_edge = false;

void msg_in_callback(int gpio, int level, uint32_t tick);
void* sender_thread(void *arg);
void send_data(struct send_thread_data *data);

/**
 * @brief: sender thread
 * @param pointer to send_thread_data struct containing data to send
 */
void* sender_thread(void *arg) {
	struct send_thread_data *data = (struct send_thread_data*) arg;
	send_data(data);
	DEBUG_PRINT("\n Returning from sender thread !");
	return 0;
}

/**
 * @brief: send data over IR link using hardware PWM
 * @param: pointer send_thread_data struct for data to send
 */
void send_data(struct send_thread_data *data) {

	printf("Length of string to send : %d\n", data->length);

	for (int i = 0; i < data->length; i++) {
		DEBUG_PRINT("\n** Sending new byte: %c **\n", (char ) data->tx_data[i]);
		/*  Iterate over code and send it */

		for (int j = 0; j < 8; j++) {

			/* send one bit at a time */
			char bit_to_send = (data->tx_data[i] >> j) & 0x01;

			int delay_us;
			if (bit_to_send == 0) {
				DEBUG_PRINT("Sending 0..\n");
				delay_us = CLOCK_MS * 1000;
			} else if (bit_to_send == 1) {
				DEBUG_PRINT("Sending 1..\n");
				delay_us = CLOCK_MS * 3 * 1000;
			}

			gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, DUTY_CYCLE);

			usleep(delay_us);

			gpioHardwarePWM(MSG_OUT_PIN, PWM_FREQ, 0);

			usleep(CLOCK_MS * 1000);
		}
	}
	DEBUG_PRINT("\n Message sent !\n");
}

/**
 * @brief: receive data callback on every I/O interrupt
 * @param: gpio pin on which ISR received
 * @param: level - pin level (high or low)
 * @param: tick - not used
 */
void msg_in_callback(int gpio, int level, uint32_t tick) {
	pthread_mutex_lock(&lock);

	/*  If rising edge is detected in clock */
	if (level == HIGH && valid_edge) {
		double elapsed_us;

		valid_edge = false;

		int ret = clock_gettime(CLOCK_MONOTONIC, &stop);
		if (ret) {
			DEBUG_PRINT("Error: stop clock_gettime: %d\n", ret);
		}

		elapsed_us = (stop.tv_sec - start.tv_sec) * 1000000
				+ (stop.tv_nsec - start.tv_nsec) / 1000;

		if (elapsed_us > (CLOCK_MS * 2 * 1000)) {
			rx_byte |= 1 << rx_bit_idx;
			rx_bit_idx++;
			DEBUG_PRINT("Logic 1 received %c\n", rx_bit_idx);
		} else {
			rx_byte &= ~(1 << rx_bit_idx);
			rx_bit_idx++;
			DEBUG_PRINT("Logic 0 received %c\n", rx_bit_idx);
		}

	}
	/*  Falling edge */
	else if (level == LOW) {
		valid_edge = true;
		int ret = clock_gettime(CLOCK_MONOTONIC, &start);
		if (ret) {
			DEBUG_PRINT("Error: start clock_gettime: %d\n", ret);
		}
	}

	if (rx_bit_idx == 8) {
		DEBUG_PRINT("Writing byte to file: %c", rx_byte);

		int fd = open(WRITEFILE, O_WRONLY | O_CREAT | O_APPEND,
		S_IRWXU | S_IRGRP | S_IROTH);

		if (fd < 0) {
			int err = errno;
			DEBUG_PRINT("Error opening file: %d.\n", err);
		}

		char *buf = &rx_byte;
		int ret;
		int len = sizeof(rx_byte);
		while (len != 0 && (ret = write(fd, buf, len)) != 0) {

			if (ret == -1) {
				const int err = errno;
				if (err == EINTR)
					continue;
				DEBUG_PRINT(
						"Error writing. File descriptor: <%d>. Errno: <%d>\n",
						fd, err);
				break;
			}

			len -= ret;
			buf += ret;
		}
		rx_bit_idx = 0;
		rx_byte = 0;
		if (close(fd) != 0) {
			DEBUG_PRINT("Error receive file\n");
		}
	}
	pthread_mutex_unlock(&lock);
}

/**
 * @brief: transfer input file and save
 * @param: arg1 - filepath to data to send
 * @param: arg2 - transmission clock speed
 */
int main(int argc, char *argv[]) {
	DEBUG_PRINT("Starting stress test\n");

	char *readFile;
	if (argc == 3) {
		readFile = argv[1];
		CLOCK_MS = atof(argv[2]);
		DEBUG_PRINT("Read File: %s, Clock (ms): %f", readFile, CLOCK_MS);
	} else {
		DEBUG_PRINT("Incorrect parameters given: %d\n", argc);
		return -1;
	}

	if (gpioInitialise() < 0) {
		printf("Could not initialize pigio !\n");
		return 1;
	}
	pthread_mutex_init(&lock, NULL);

	/* configure I/O */
	gpioSetMode(MSG_OUT_PIN, PI_ALT0);
	gpioSetMode(MSG_IN_PIN, PI_INPUT);

	/* configure input as pull up since TSOP3082 is active low */
	gpioSetPullUpDown(MSG_IN_PIN, PI_PUD_UP);
	/* let I/O settle before configuring ISR*/
	usleep(100000);
	gpioSetISRFunc(MSG_IN_PIN, EITHER_EDGE, 0, msg_in_callback);

	DEBUG_PRINT("Reading from %s", readFile);

	int fd = open(readFile, O_RDONLY,
	        S_IRWXU | S_IRGRP | S_IROTH);

	if (fd < 0) {
		int err = errno;
		DEBUG_PRINT("Error opening file: %d.\n", err);
		return -1;
	}

	int ret;
	int bytesRead;

	while ((ret = read(fd, tx_buf, TX_BUF_SZ)) != 0) {
		if (ret == -1) {
			int err = errno;
			if (err == EINTR)
				continue;
			DEBUG_PRINT("****Error reading from file: %d***\n", err);
			break;
		}

		bytesRead += ret;

		send_thread_data.tx_data = tx_buf;
		send_thread_data.length = bytesRead;

		pthread_create(&sender_id, NULL, sender_thread,
				(void*) &send_thread_data);
		DEBUG_PRINT("\n Sender thread spawned !");

		/*  Cleanup */
		pthread_join(sender_id, NULL);
		DEBUG_PRINT("\n Sender thread joined !");

	}

	DEBUG_PRINT("Closing fd\n");
	if (close(fd) != 0) {
		int err = errno;
		DEBUG_PRINT("Failed to close file: %d\n", err);
	}

	gpioTerminate();
}

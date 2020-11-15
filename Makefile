# Makefile for building morse-without-clock
ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror
endif

LDFLAGS+=-lpigpio -lrt -lpthread -lm

all:
	$(CC) $(CFLAGS) morse-without-clock/main2.c -o morse-loopback $(INCLUDES) $(LDFLAGS);
	$(CC) $(CFLAGS) testled/test-led.c -o test-led $(INCLUDES) $(LDFLAGS);

clean :
	rm -f morse-loopback test-led;

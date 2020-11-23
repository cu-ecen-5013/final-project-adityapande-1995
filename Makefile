# Makefile for building morse-without-clock
#ifeq ($(CC),)
#	CC = $(CROSS_COMPILE)gcc
#endif
#ifeq ($(CFLAGS),)
#	CFLAGS = -g -Wall -Werror
#endif
#
#LDFLAGS+=-lpigpio -lrt -lpthread -lm
#
#all:
#	$(CC) $(CFLAGS) morse-without-clock/main2.c -o morse-loopback $(INCLUDES) $(LDFLAGS);
#	$(CC) $(CFLAGS) testled/test-led.c -o test-led $(INCLUDES) $(LDFLAGS);
#
#clean :
#	rm -f morse-loopback test-led;

#CC=gcc
#CFLAGS +=-g -Wall -Werror

all: morse-loopback stress-test-app

morse-loopback: ./morse-without-clock/main2.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./morse-without-clock/main2.o -o morse-loopback -lpigpio -lrt -lpthread -lm

main2.o: ./morse-without-clock/main2.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./morse-without-clock/main2.c -o ./morse-without-clock/main2.o

stress-test-app: ./stress-test/stress-test.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./stress-test/stress-test.o -o stress-test-app -lpigpio -lrt -lpthread -lm

stress-test.o: ./stress-test/stress-test.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./stress-test/stress-test.c -o ./stress-test/stress-test.o


clean:
	rm -f *.o morse-loopback *.elf *.map ./morse-without-clock/*.o ./stress-test/*.o

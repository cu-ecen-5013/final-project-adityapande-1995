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

all: test-led

test-led: ./testled/test-led.o ./morse-without-clock/main2.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./testled/test-led.o -o test-led -lpigpio
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./morse-without-clock/main2.o -o morse-loopback -lpigpio -lrt -lpthread -lm

test-led.o: ./testled/test-led.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./testled/test-led.c -o ./testled/test-led.o

main2.o: ./morse-without-clock/main2.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./morse-without-clock/main2.c -o ./morse-without-clock/main2.o

clean:
	rm -f *.o test-led morse-loopback *.elf *.map ./testled/*.o ./morse-without-clock/*.o

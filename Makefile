#CC=gcc
#CFLAGS +=-g -Wall -Werror

all: test-led

test-led: ./testled/test-led.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./testled/test-led.o -o test-led -lpigpio

test-led.o: ./testled/test-led.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./testled/test-led.c -o ./testled/test-led.o

clean:
	rm -f *.o test-led *.elf *.map ./testled/*.o

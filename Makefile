
all: morse-loopback file-transfer-app receiver

morse-loopback: ./morse-without-clock/main2.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./morse-without-clock/main2.o -o morse-loopback -lpigpio -lrt -lpthread -lm

main2.o: ./morse-without-clock/main2.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./morse-without-clock/main2.c -o ./morse-without-clock/main2.o

file-transfer-app: ./file-transfer/file-transfer.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./file-transfer/file-transfer.o -o file-transfer-app -lpigpio -lrt -lpthread -lm

file-transfer.o: ./file-transfer/file-transfer.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./file-transfer/file-transfer.c -o ./file-transfer/file-transfer.o

receiver: ./morse-without-clock/receiver.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -g -Wall -Werror -I/ ./morse-without-clock/receiver.o -o receiver -lpigpio -lrt -lpthread -lm

receiver.o: ./morse-without-clock/receiver.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) -c ./morse-without-clock/receiver.c -o ./morse-without-clock/receiver.o


clean:
	rm -f *.o receiver morse-loopback file-transfer-app *.elf *.map ./morse-without-clock/*.o ./file-transfer/*.o

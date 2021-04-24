CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -Wno-unused-variable -pthread
OBJS = chksum.o queue.o stopwatch.o packetsource.o crc32.o generators.o fingerprint.o
TARGETS = serial parallel serial_queue
LDLIBS  = -lm

all: $(TARGETS)

serial : serial.o $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS) serial.o -o serial $(LDLIBS)

serial_queue : serial_queue.o $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS) serial_queue.o -o serial_queue $(LDLIBS)

parallel : parallel.o $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) parallel.o -o parallel $(LDLIBS)

serial.o: serial.c
	$(CC) $(CFLAGS) -c serial.c

serial_queue.o: serial_queue.c
	$(CC) $(CFLAGS) -c serial_queue.c

parallel.o: parallel.c
	$(CC) $(CFLAGS) -c parallel.c

chksum.o: chksum.c lib/chksum.h
	$(CC) $(CFLAGS) -c chksum.c

queue.o: queue.c lib/queue.h
	$(CC) $(CFLAGS) -c queue.c

stopwatch.o: stopwatch.c lib/stopwatch.h
	$(CC) $(CFLAGS) -c stopwatch.c

packetsource.o: packetsource.c lib/packetsource.h
	$(CC) $(CFLAGS) -c packetsource.c

crc32.o: crc32.c lib/crc32.h
	$(CC) $(CFLAGS) -c crc32.c

fingerprint.o: fingerprint.c lib/fingerprint.h
	$(CC) $(CFLAGS) -c fingerprint.c

generators.o: generators.c lib/generators.h
	$(CC) $(CFLAGS) -c generators.c

clean:
	- rm -f *~ *.o  $(TARGETS)
	- rm res/*

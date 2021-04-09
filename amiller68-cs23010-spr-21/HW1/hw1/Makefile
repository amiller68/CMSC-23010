CC = gcc
CFLAGS = -Wall -Werror -O3 -Wno-unused-variable
OBJS = floyd.o graph.o stopwatch.o
TARGETS = fw_serial fw_parallel

all: $(TARGETS)

fw_serial : fw_serial.o $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) fw_serial.o -o  fw_serial

fw_parallel : fw_parallel.o $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) fw_parallel.o -o  fw_parallel

fw_serial.o: fw_serial.c
	$(CC) $(CFLAGS) -c fw_serial.c

fw_parallel.o: fw_parallel.c
	$(CC) $(CFLAGS) -c fw_parallel.c

floyd.o: floyd.c lib/floyd.h
	$(CC) $(CFLAGS) -c floyd.c

stopwatch.o: stopwatch.c lib/stopwatch.h
	$(CC) $(CFLAGS) -c stopwatch.c

graph.o: graph.c lib/graph.h
	$(CC) $(CFLAGS) -c graph.c

clean:
	rm -f *~ *.o  $(TARGETS)
	rm tests/*
	rm res/*

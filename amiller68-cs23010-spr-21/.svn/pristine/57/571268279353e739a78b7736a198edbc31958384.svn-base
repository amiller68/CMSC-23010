CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -Wno-unused-variable -pthread
OBJS = lock.o stopwatch.o
TARGETS = serial parallel my_test_serial my_test_parallel
LDLIBS  = -lm -dlopen

all: $(TARGETS)

serial : serial.o $(OBJS)
	$(CC) $(CFLAGS)  $(OBJS) serial.o -o serial $(LDLIBS)

parallel : parallel.o $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) parallel.o -o parallel $(LDLIBS)

my_test_serial : my_test_serial.o $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) my_test_serial.o -o my_test_serial $(LDLIBS)

my_test_parallel : my_test_parallel.o $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) my_test_parallel.o -o my_test_parallel $(LDLIBS)

serial.o: serial.c
	$(CC) $(CFLAGS) -c serial.c

parallel.o: parallel.c
	$(CC) $(CFLAGS) -c parallel.c

my_test_serial.o: my_test_serial.c
	$(CC) $(CFLAGS) -c my_test_serial.c

my_test_parallel.o: my_test_parallel.c
	$(CC) $(CFLAGS) -c my_test_parallel.c

lock.o: lock.c lib/lock.h
	$(CC) $(CFLAGS) -c lock.c

stopwatch.o: stopwatch.c lib/stopwatch.h
	$(CC) $(CFLAGS) -c stopwatch.c

clean:
	- rm -f *~ *.o  $(TARGETS)
	- rm -r exp_data

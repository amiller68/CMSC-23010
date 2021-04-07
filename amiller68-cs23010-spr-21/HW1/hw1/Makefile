CC = gcc
CFLAGS = -Wall -Werror -O3
OBJS = main.o floyd.o
TARGET = main

all: $(TARGET)

 $(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o  $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

floyd.o: floyd.c lib/floyd.h
	$(CC) $(CFLAGS) -c floyd.c

stopwatch.o: stopwatch.c lib/stopwatch.h
	$(CC) $(CFLAGS) -c stopwatch.c

graph.o: graph.c lib/graph.h
	$(CC) $(CFLAGS) -c graph.c

clean:
	rm -f *~ *.o  $(TARGET)

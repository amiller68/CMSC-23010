CC = gcc
CFLAGS = -Wall -Werror -O3
OBJS = main.o floyd.o
TARGET = fw

all: $(TARGET)

 $(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o  $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

floyd.o: floyd.c lib/floyd.h
	$(CC) $(CFLAGS) -c floyd.c

clean:
	rm -f *~ *.o  $(TARGET)

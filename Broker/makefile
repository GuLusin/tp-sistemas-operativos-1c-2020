CC=gcc
CFLAGS = #-Wall -g
BINS = broker
all: clean $(BINS)

broker: src/broker.c src/broker.h
	$(CC) $(CFLAGS) -o $@ $^ -L. -lcommons -lpthread

clean:
	rm $(BINS)

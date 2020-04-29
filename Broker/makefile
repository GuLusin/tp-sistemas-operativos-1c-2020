CC=gcc
CFLAGS = #-Wall -g
BINS = broker
all: $(BINS)

broker: src/broker.c src/broker.h
	$(CC) $(CFLAGS) -o $@ $^ -L. -lcommons -lpthread

clean:
	rm $(BINS)

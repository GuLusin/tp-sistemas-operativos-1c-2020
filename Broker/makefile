CC=gcc
CFLAGS = #-Wall -g
BINS = broker
all: $(BINS)

broker: src/broker.c src/broker.h src/back_end_broker.c src/back_end_broker.h
	$(CC) $(CFLAGS) -o $@ $^ -L. -lcommons -lpthread

clean:
	rm $(BINS)

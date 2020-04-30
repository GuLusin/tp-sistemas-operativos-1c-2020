CC=gcc
CFLAGS = -Wall -g
BINS = gameboy
all: $(BINS)

gameboy: src/gameboy.c src/gameboy.h
	$(CC) $(CFLAGS) -o $@ $^ -lcommons -lpthread

clean:
	rm $(BINS)

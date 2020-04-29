CC=gcc
CFLAGS = -Wall -g
BINS = team
all: $(BINS)

team: src/team.c src/team.h
	$(CC) $(CFLAGS) -o $@ $^ -lcommons -lpthread

clean:
	rm $(BINS)

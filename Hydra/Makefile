CC = g++
CFLAGS = -Wall -O2 -std=c++11
LINKS = -lpthread -ldl
HEADER = -I./source

all: hydra

hydra: source/lang1.hpp source/main.cpp source/lang1.cpp
	$(CC) $(CFLAGS) -o hydra source/main.cpp source/lang1.cpp $(HEADER) $(LINKS)

#debug: surface.h main.c cmdparser.c cmdparser.h
#	$(CC) $(CFLAGS) -g -o debug main.c cmdparser.c -ldl

#test: surface.h test.c
#	$(CC) $(CFLAGS) -o test test.c -ldl

#.PHONY: debug
#debug: surface.h main.c cmdparser.c cmdparser.h
#	$(CC) $(CFLAGS) -o hydra main.c cmdparser.c -ldl -lpthread -D"DEBUG"

#.PHONY: clean
#clean:
#	rm debug

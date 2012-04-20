CC=g++
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CFLAGS += -Wall -Wextra -g
endif

ifeq ($(UNAME), SunOS)
LDLIBS=-lsocket -lnsl
endif


EXECUTABLES=tcp_server

all: $(EXECUTABLES)

tcp_server: tcp_server.c tcp.cpp queueable.cpp

test: test_tcp

test_tcp:
	time ./tcp_server


.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLES)

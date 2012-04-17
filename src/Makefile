CC=g++
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CFLAGS += -Wall -Wextra
endif

ifeq ($(UNAME), SunOS)
LDLIBS=-lsocket -lnsl
endif


EXECUTABLES=tcp_client tcp_server

all: $(EXECUTABLES)

tcp_client: tcp_client.c tcp.cpp queueable.cpp
tcp_server: tcp_server.c tcp.cpp queueable.cpp

test: test_tcp

test_tcp:
	time ./tcp_server &
	time ./tcp_client


.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLES)

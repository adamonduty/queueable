CC=g++
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
CXXFLAGS += -Wall -Wextra -g
LDLIBS=-lrt
endif

ifeq ($(UNAME), SunOS)
LDLIBS=-lsocket -lnsl
endif


EXECUTABLES=run_queueable

all: $(EXECUTABLES)

run_queueable: run_queueable.cpp socket.cpp stream_socket.cpp queueable.cpp tcp.cpp udss.cpp udds.cpp mq.cpp md5.cpp

test: all
	time ./run_queueable


.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLES)

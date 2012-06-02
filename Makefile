all:
	cd src && make

test: all
	src/run_queueable

clean:
	cd src && make clean

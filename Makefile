all: logging database server client

logging:
	cd logging; make

database:
	cd database; make

server:
	cd server; make

client:
	cd client; make

clean:
	cd logging; make clean
	cd database; make clean
	cd server; make clean
	cd client; make clean

.PHONY: all clean logging database server client
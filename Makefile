all: 
	make -f client/Makefile all
	make -f server/Makefile all

client:
	make -f client/Makefile all

server:
	make -f server/Makefile all

clean:
	make -f client/Makefile clean
	make -f server/Makefile clean
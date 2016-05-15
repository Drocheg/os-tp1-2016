sockets: logging socketsdatabase socketsserver socketsclient socketsconfig

fifos: logging  fifosserver fifosclient fifosdatabase fifosconfig


socketsdatabase: 
	cd database; make sockets

socketsclient: 
	cd client; make sockets

socketsserver:
	cd server; make sockets

socketsconfig:
	cp ./configSkeleton/sockets.conf ./config.conf


fifosdatabase: 
	cd database; make fifos

fifosclient: 
	cd client; make fifos

fifosserver:
	cd server; make fifos

fifosconfig:
	cp ./configSkeleton/fifos.conf ./config.conf




logging:
	cd logging; make

#database:
#	cd database; make

#server:
#	cd server; make

#client:
#	cd client; make

clean:
	cd logging; make clean
	cd database; make clean
	cd server; make clean
	cd client; make clean
	rm -f config.conf

.PHONY: all clean logging database server client sockets fifos

sockets: logging database socketsserver socketsclient done socketsconfig 

fifos: logging database fifosserver fifosclient done fifosconfig

socketsclient: 
	cd client; make sockets

socketsserver:
	cd server; make sockets

socketsconfig:
	cp ./configSkeleton/sockets.conf ./dist/config.conf

fifosclient: 
	cd client; make fifos

fifosserver:
	cd server; make fifos

fifosconfig:
	cp ./configSkeleton/fifos.conf ./dist/config.conf

logging:
	cd logging; make

done:
	mkdir -p dist
	mv database/databaseServer.bin ./dist/
	mv client/client.bin ./dist/
	mv server/server.bin ./dist/
	mv logging/loggingDaemon.bin ./dist/

database:
	cd database; make

clean:
	cd logging; make clean
	cd database; make clean
	cd server; make clean
	cd client; make clean
	rm -rf ./dist

.PHONY: clean logging database sockets fifos

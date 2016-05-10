

struct servers_set_t {
	size_t size;
	pid_t *servers;
}

typedef servers_set_t * ServersSet;


static ServersSet createServersSet() {
	ServersSet serversSet = malloc(sizeof(*serversSet));
	serversSet->size = 0;
	serversSet->servers = NULL;
}

static void destroyServersSet(ServersSet serversSet) {
	free(serversSet->servers)
	free(serversSet);
}

static int addServer(ServersSet serversSet, pid_t serverPID) {
	pid_t *aux = realloc(serversSet->servers, (serversSet->size + 1)*sizeof(pid_t));
	if (aux == NULL) {
		return -1;
	}
	serversSet->servers = aux;
	serversSet->servers[size++] = serverPID;
	return 0;
}

static int searchPID(ServersSet serversSet, pid_t serverPID) {
	 int i = 0;
	 while (i < size && ServersSet->servers[i] != serverPID) {
		i++;
	}
	return (i == serversSet->size) ? -1 : i;
}

static int removeServer(ServersSet serversSet, pid_t serverPID) {
	int i = searchPID(serversSet, serverPID);
	if (i >= 0) {
		pid_t *aux = realloc(serversSet->servers, (serversSet->size - 1)*sizeof(pid_t));
		if (aux == NULL) {
			return -1;
		}
		while (i + 1 < serversSet->size) {
			aux[i] = serversSet->servers[i + 1];
		}
		serversSet->servers = aux;
		serversSet->size--;
		if (serversSet->size == 0) {
			free(serversSet->servers);
		}
	}
 	return 0;
}




static int createForkedServer(Connection c, ServersSet serversSet) {
	int pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Couldn't create a forked server\n");
		return -1;
	} else if (pid > 0) {
		/* Parent Process */
		serversSet->addServer(pid);
		return 0;
	} else {
		/* Child Process */
		return forkedServer(c);
	}
}


int main(int argc, char *argv[]) {

	//int isParentProcess = 1;
	int *servers = createServersSet();
	char *address = getServerAddress();
	ConnectionParams connParams = conn_listen(address);
	if (connParams == NULL) {
		fprintf(stderr, "Couldn't start server. Aborting.\n");
		return -1;
	}
	printf("Server started. Listening to connnection requests.\n");
	while(1) {
		Connection c = conn_accept(connParams);
		if (c == NULL) {
			fprintf(stderr, "Error accepting connection\n");
			exit(1);
		} else {
			int pid = createForkedServer();

		}
	}


}














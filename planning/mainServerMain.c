#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



static void signalHandler(int signum, siginfo_t *siginfo, void *context) {

	printf("Signal sender: %d\n", siginfo->si_pid); /* client's PID */
	if (signum == SIGUSR1) {
		int pid = fork();
		if (pid > 0) {

			printf("I'm main server\n");
		} else {

			printf("I'm forked server\n");
			 /*Here goes an execve with the bin file of the forked server */
		}
	}
}

/*
 * Configures main server in order to receive connections (i.e. configures signal handler)
 * Returns 0 if succesful; otherwise the value -1 is returned and the global variable errno
 * is set to indicate the error.
 */
static int configure() {

	struct sigaction act;

	act.sa_sigaction = &signalHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART | SA_SIGINFO;
	return sigaction(SIGUSR1, &act, NULL);

}


int main(void) {
	
	// Start logging server (maybe using a fork and execve)
	// Start SQLite
	if (configure()) {
		exit(1);
	}

	
	while (1) {
		/* Main Loop */
	}
	return 0;
}




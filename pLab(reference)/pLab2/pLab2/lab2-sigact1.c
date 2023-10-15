#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int n=0;

void sigint_handler1(int signum) {
	printf("signal %d is caught for %d times\n", signum, ++n);

	if (n == 3) {
		exit(0);
	}
}

int main() {
	struct sigaction sa;
	
	/* use sigaction to install a signal handler named sigint_handler1 */
	sigaction(SIGINT, NULL, &sa);
	sa.sa_handler = sigint_handler1;
	sigaction(SIGINT, &sa, NULL);
	
	printf("My process ID is %d\n", (int)getpid());
	printf("Press Ctrl-c 3 times to kill me\n");;
	while (1) {
		sleep(1);
	}
}

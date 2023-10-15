#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int n=0;

void sigint_handler2(int signum, siginfo_t *sig, void *v) {
	printf("signal %d (from %d) is caught for %d times\n", signum, sig->si_pid, ++n);
	if (sig->si_code == SI_USER) printf("It is sent by a user (kill command)\n");
	else if (sig->si_code == SI_KERNEL) printf("It is sent by the kernel\n");
	
	if (n == 3) {
		exit(0);
	}
}

int main() {
	struct sigaction sa;
	
	/* use sigaction to install a signal handler named sigint_handler1 */
	sigaction(SIGINT, NULL, &sa);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = sigint_handler2;
	sigaction(SIGINT, &sa, NULL);
	
	printf("My process ID is %d\n", (int)getpid());
	printf("Press Ctrl-c 3 times to kill me\n");;
	while (1) {
		sleep(1);
	}
}

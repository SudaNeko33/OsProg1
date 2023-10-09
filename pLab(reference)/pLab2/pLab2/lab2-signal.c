#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void sigint_handler(int signum) {
    printf("\nA signal SIGINT is caught\n\n");
}

int main() {
	printf("SIGINT can be CAUGHT once in the coming 10 seconds.\n");
	printf("Press Ctrl-c to try\n");
	signal(SIGINT, sigint_handler);	
	
	sleep(10);
	
	printf("SIGINT is ignored in the coming 10 seconds.\n");
	printf("Press Ctrl-c to try\n");
	signal(SIGINT, SIG_IGN);
	
	sleep(10);
	
	printf("\n\nSIGINT is set to default action.\n");
	printf("Press Ctrl-c to kill me\n");
	signal(SIGINT, SIG_DFL);
	
	while (1) {
		sleep(10);
	}
}

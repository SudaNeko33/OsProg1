/* Lab 1 - Process
*  lab1-waitpid.c
*/

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
	pid_t pid = fork();
	
	if (pid < 0) {
		printf("fork: error no = %s\n", strerror(errno));
		exit(-1);
	} else if (pid == 0) {
		printf("child: I am a child process, with pid: %d\n", (int) getpid());
		printf("child: exited\n");
		exit(0);
	} else {
		printf("I am a parent process, with pid: %d and my child's pid is %d\n", 
			(int) getpid(), (int) pid);
		
		pid_t child_pid;
		int status;
		child_pid = waitpid(pid, &status, 0);
		
		printf("Child process (%d) exited, with exit status %d\n", 
			(int) child_pid, WEXITSTATUS(status));
		
		printf("Press <enter> to continue\n");
		getchar();  
	}
	
	return 0;
}

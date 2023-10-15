/* Lab 1 - Process
*  lab1-getrusage.c
*/

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>


int main() {
  pid_t pid;
  struct rusage used;

  pid = fork();

  if (pid < 0) {
    printf("fork: error no = %s\n", strerror(errno));
    exit(-1);
  } else if (pid == 0) {
    printf("child: I am a child process, with pid %d\n", (int)getpid());
	printf("Use the kill system command to terminate me !!!\n");
    while (1);  // use the kill system command and send the SIGTERM signal to kill it!!!
  } else {
    int status;
    waitpid(pid, &status, 0);  // wait for child to terminate
    printf("Child process exited, with signal status: %d\n", WTERMSIG(status));
    getrusage(RUSAGE_CHILDREN, &used);
    printf("The child had experienced %ld involuntary context switches\n", used.ru_nivcsw);
 
  }

  return 0;
}

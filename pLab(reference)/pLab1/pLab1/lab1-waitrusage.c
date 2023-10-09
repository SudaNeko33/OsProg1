/* Lab 1 - Process
*  lab1-waitrusage.c
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
    wait4(pid, &status, 0, &used);  // wait for child to terminate
    printf("Child process exited, with exit status: %d\n", WTERMSIG(status));
    printf("The child had experienced %ld involuntary context switches\n", used.ru_nivcsw);
 
  }

  return 0;
}

/* Lab 1 - Process
*  lab1-waitid.c
*/

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>



int main() {
  pid_t pid;

  pid = fork();

  if (pid < 0) {
    printf("fork: error no = %s\n", strerror(errno));
    exit(-1);
  } else if (pid == 0) {
    printf("child: I am a child process, with pid %d\n", (int)getpid());
    printf("Use the kill system command to terminate me !!!\n");
    while (1);  // use the kill system command and send the SIGTERM signal to kill it!!!
  } else {
    siginfo_t info;
    int status;
    int ret = waitid(P_ALL, 0, &info, WNOWAIT | WEXITED);  // wait for child to terminate
    if (!ret) {
      printf("Child with process id: %d has exited\n", (int) info.si_pid);
      waitpid(info.si_pid, &status, 0);
      printf("Child process (%d) exited, with signal status %d\n", 
			(int) info.si_pid, WTERMSIG(status));
    } else {
      perror("waitid");
    }
  }

  return 0;
}

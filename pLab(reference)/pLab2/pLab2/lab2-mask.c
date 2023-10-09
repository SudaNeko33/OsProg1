#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void sigint_handler(int signum) {
    printf("\nA signal SIGINT is caught\n\n");
}

int main() {
  sigset_t blkset;
  
  printf("SIGINT can be CAUGHT once in the coming 10 seconds.\n");
  printf("Press Ctrl-c to try\n");
  signal(SIGINT, sigint_handler); 
  
  sleep(10);
  
  printf("SIGINT is masked in the coming 10 seconds.\n");
  printf("Press Ctrl-c to try\n");
  sigemptyset(&blkset);
  sigaddset(&blkset, SIGINT);
  sigprocmask(SIG_BLOCK, &blkset, NULL);
  
  sleep(10);
  
  sigprocmask(SIG_UNBLOCK, &blkset, NULL);
  printf("\n\nSIGINT is set to default action.\n");
  printf("Press Ctrl-c to kill me\n");
  signal(SIGINT, SIG_DFL);
  
  while (1) {
    sleep(10);
  }
}

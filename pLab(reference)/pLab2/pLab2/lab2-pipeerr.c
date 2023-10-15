/* Lab 2 - Pipe
*  lab2-pipeerr.c
*/

#include <stdio.h>
#include <unistd.h>

int main() {
	int pfd[2];
	
	pipe(pfd);

	if (fork() == 0) {	//in child
		close(pfd[1]);  //close the write end
		char buff[100];
		int res = read(pfd[0], buff, 100);
		if (res == 0)
			printf("Child: EOF is reached -- No one holds the write end!!\n");
	} else {  //in parent
		close(pfd[1]);  //close the write end
		printf("Press enter to close the program\n");
		getchar();
	}
	
	return 0;
}

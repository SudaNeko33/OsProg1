/* Lab 2 - Pipe
*  lab2-pipefull.c
*/

#include <stdio.h>
#include <unistd.h>

int main() {
	char pattern[] = "0123456789abcdef";
	int pfd[2];
	int num = 0, i = 0;
	
	pipe(pfd);
	
	while (1) {
		num = write(pfd[1], pattern, 16);
		i += num;
		printf("%d\n", i);
	}
	
	return 0;
}

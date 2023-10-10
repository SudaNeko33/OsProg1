#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<signal.h>

int main() {
    int i =6;
    char input[100] = {0}; // 用于存储用户输入的数据
    char prefix[100] = "/proc/";
    sprintf(prefix, "/proc/%d/stat\n", i);

    printf("%s", prefix);
    

    return 0;
}

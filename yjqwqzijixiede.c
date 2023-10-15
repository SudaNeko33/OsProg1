/*
Name: Xu Ziyin
No. 3036173372
Development platform: Ubuntu 22.04
Remark:
Finish all the requirements.(?))))))))))

*/


#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>
#include<signal.h>
#include <sys/mman.h>

pid_t pid = -1;

struct proc p;
struct proc {
    int PID;
    char* CMD;
    char STATE;
    int EXCODE;
    char EXSIG[20];
    int PPID;
    int USER;
    int SYS;
    int VCTX;
    int NVCTX;
};


char* buf[6] = {0};
int buf_count = 0;
pid_t pids[6] = {0};
int pid_cnt;

int *pipes[6];
int command_count = 0;
int child_count = 0;

int status = 0;

char* commands[6] = {0};
char* cmds[6][30] = {0};

int* shared_var = 0;



void sigint_handler(int signum){
    if(signum == SIGINT){
        if(pid == getpid()){
            printf("\n");
            fclose(stdin);
        }
    }
}


void sigintchild_handler(int signum){
    exit(0);
}

// receive SIGUSR1 and then execute the command
void sigusr1_handler(int sig){
    printf("Receive sigusr1, start\n");
    *shared_var = 1;
}

// print the process information
void print_proc(pid_t cpid){
    if(cpid == 0) return;
    memset(&p, 0, sizeof(struct proc));
    p.CMD = (char*)malloc(sizeof(char)*100);

    char prefix[100] ;

    sprintf(prefix, "/proc/%d/stat", cpid);
    FILE* fp = fopen(prefix, "r");
    if (fp == NULL){
        printf("JCshell: \'%d\': No such process\n", cpid);
        return;
    }
    char tmp[100]={0};
    int c = 0;
    
    fscanf(fp, "%d", &p.PID);
    fscanf(fp, "%s %c %d", p.CMD, &p.STATE, &p.PPID);

    for(int i=0 ;i<9;i++){
        fscanf(fp, "%s", tmp);
    }

    fscanf(fp, "%d %d", &p.USER, &p.SYS);

    for(int i=0; i<22;i++){
        fscanf(fp, "%s", tmp);
    }
    fscanf(fp, "%d", &p.EXCODE);
    fclose(fp);
}


void sigchild_handler(int signum){
    if(signum == SIGCHLD){
        print_proc(pids[pid_cnt]);
        pid_cnt++;
        if(child_count != command_count){
            close(pipes[child_count][0]);
            close(pipes[child_count][1]);
        }
        wait(&status);
        if(WIFEXITED(status)){
            p.EXCODE = WEXITSTATUS(status);
        }else{
            p.EXCODE = WTERMSIG(status);
        }

        child_count++;
        if(p.EXCODE == 0){
            strcpy(p.EXSIG, "\0");
        }else{
            strcpy(p.EXSIG, strsignal(WTERMSIG(status)));
        }
        char* proc_static = (char*)malloc(sizeof(char)*300);
        char main_cmd[] = "(main)";
        if(memcmp(p.CMD, main_cmd, 6) == 0){
            memset(proc_static, 0, sizeof(char)*300);
        }else if(p.EXCODE != 0)
            sprintf(proc_static,"(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (EXSIG)%s (PPID)%d (USER)%d (SYS)%d (VCTX)%d (NVCTX)%d\n", p.PID, p.CMD, p.STATE,p.EXCODE, p.EXSIG, p.PPID, p.USER, p.SYS, p.VCTX, p.NVCTX);
        else 
            sprintf(proc_static,"(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (PPID)%d (USER)%d (SYS)%d (VCTX)%d (NVCTX)%d\n", p.PID, p.CMD, p.STATE, p.EXCODE, p.PPID, p.USER, p.SYS, p.VCTX, p.NVCTX);
        buf[buf_count] = proc_static;
        buf_count++;
        status = 0;
    }
}

// analysis the input command with |
void split_multi_cmd(char* input, int* num_of_cmds){
    // check whether there are two | symbols without in-between commands
    int flag = 0;
    int i = 0;
    while (input[i] != '\0') {
        if (input[i] == '|') {
                // check if there are only spaces in between the two pipes
                int j = i + 1;
                if (input[j] == '|') {
                    flag = 1;
                    break;
                }
                while (input[j] != '\0' && input[j] == ' ') {
                    j++;
                }
                if (input[j] == '|') {
                    flag = 1;
                    break;
                }
        }
        if(flag) break;
        i++;
    }
    if (flag) {
        printf("JCshell: should not have two | symbols without in-between command\n");
        return ;
    }
    // start to split the input command
    char* cmds_remain = strtok(commands, "|");
    int cnt_cmds = 0;
    char * cmd_tmp;
    memset(commands, 0, sizeof(char)*6);
    while(cmds_remain){
        char* cmds_cur = cmds_remain;
        cmds_remain = strtok(NULL, "|");
        while(cmds_cur[0] == ' ') cmds_cur++;
        if(cnt_cmds == 0){
            if(cmds_remain){
                cmd_tmp = (char*)malloc(sizeof(char)*(cmds_remain-cmds_cur+1));
            }else{
                cmd_tmp = (char*)malloc(sizeof(char)*(input+1025-cmds_cur));
            }
            strcpy(cmd_tmp, cmds_cur);
            commands[0] = cmd_tmp;
        }else if(!cmds_remain){
            commands[cnt_cmds] = (char*)malloc(sizeof(char)*(input+1025-cmds_cur));
            strcpy(commands[cnt_cmds], cmds_cur);
        }else{
            commands[cnt_cmds] = (char*)malloc(sizeof(char)*(cmds_remain-cmds_cur+1));
            strcpy(commands[cnt_cmds], cmds_cur);
        }
        cnt_cmds++;
        if(cnt_cmds > 5){
            printf("JCshell: too many commands\n");
            return;
        }
    }
    // store the number of commands
    *num_of_cmds = cnt_cmds;
    return;
}

// analysis a single command
void split_single_cmd(char* command, int i){ // meigaiwan
    while(command[0] == ' ') command++;
    char* cmd_remain = strtok(command, " ");
    int cnt_str = 0;
    char* cmd_tmp;

    while(cmd_remain){ // while the command(not loaded) is not empty
        char* cmd_cur = cmd_remain; // cmd_cur points to cmd_remain
        cmd_remain = strtok(NULL, " "); // cmd_remain points to the next command
        while(cmd_cur[0] == ' ') cmd_cur++; // delete spaces before cmd_cur
        if (cnt_str == 0){
            if (!cmd_remain){ 
            // cmd_remain is empty
            // cmd_cur is the only command, no spaces after, e.g. ls
                cmd_tmp = (char*)malloc(sizeof(char) * (command + 1025 - cmd_cur));
                // dont know length of cmd_cur, so the longest is (command + 1025 - cmd_cur), which = 1025
            // quit the loop
            }
            else { // cmd_cur is the first command, and with spaces afterwards
                cmd_tmp = (char*)malloc(sizeof(char) * (cmd_remain - cmd_cur + 1));
                // the distance between two pointers is the length of this command
                
            }
            strcpy(cmd_tmp, cmd_cur);
            cmds[i][0] = cmd_tmp;
        }
        else{ // command string in the middle
            if (!cmd_remain){ // cmd_cur is the last command
                // dont know how long it is
                cmds[i][cnt_str] = (char*)malloc(sizeof(char) * (command + 1025 - cmd_cur));
                strcpy(cmds[i][cnt_str], cmd_cur);
            }
            else{ // cmd_cur is not the last command
                cmds[i][cnt_str] = (char*)malloc(sizeof(char) * (cmd_remain - cmd_cur + 1));
                strcpy(cmds[i][cnt_str], cmd_cur);
            }
        }
        cnt_str++;
        if (cnt_str > 30){
            printf("JCshell: too many commands\n");
            return NULL;
        }
        if(memcmp(cmd_tmp, "exit", 4) == 0){
            if(cnt_str > 1){
                printf("\"exit\" with too many arguments\n");
                return;
            }
            else{
                printf("JCshell: Terminated\n");
                exit(0);
            }
        }
    }
}

int main(int argc, char *argv[]){
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchild_handler);
    signal(SIGUSR1, sigusr1_handler);   

    size_t shared_var_size = sizeof(int);
    shared_var = (int *)mmap(NULL, shared_var_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid = getpid();
    while(1){
        for(int i=0;i<6;i++){
            if(commands[i]){
                free(commands[i]);
                commands[i] = NULL;
            }
        }
        printf("## JCshell [%d] ## ", pid);
        char input[1025] = {'\0'};

        for(int i=0; i<6;i++){
            if(buf[i]){
                free(buf[i]);
                buf[i] = NULL;
            }
        }
        buf_count = 0;

        freopen("/dev/tty", "r", stdin);
        gets(input, sizeof(input), stdin);
        
        if(input[0] == '\0'){ 
            continue;
        }

        command_count = 0;
        child_count = 0;
        pid_cnt = 0;
        split_multi_cmd(input, &command_count);
        
        if(commands == NULL){ 
            continue;
        }

        int valid = 1;
        for(int i=0;i<command_count;i++){
            split_single_cmd(commands[i],i);
            if(cmds[i][0] == NULL) {
                valid = 0;
                break;
            }
        }
        if(!valid) continue;
        for(int i=0; i<command_count;i++){
            pipes[i] = (int*)malloc(sizeof(int)*2);
            pipe(pipes[i]);
        }

        for(int i=0; i<command_count; i++){ 
            pid_t p = fork();
            pids[i] = p;
            if(p != 0){
                continue;
            }else{
                signal(SIGINT, sigintchild_handler);
                signal(SIGUSR1, sigusr1_handler);
                for(int j=0; j<command_count; j++){
                    if(j!=i-1){
                        close(pipes[j][0]);
                    }
                    if(j!=i){
                        close(pipes[j][1]);
                    }
                }

                if(i != 0){
                    if(dup2(pipes[i-1][0], STDIN_FILENO) ==-1){
                        printf("dup2 stdin error\n");
                    }
                }
                if(i != command_count-1){
                    if(dup2(pipes[i][1], STDOUT_FILENO) == -1){
                        printf("dup2 stdout error\n");
                    }
                    
                }
                printf("waiting for sigusr1 to start\n");
                while(*shared_var == 0);

                if(execvp(cmds[i][0], cmds[i]) == -1){
                    printf("JCshell: \'%s\': No such file or directory\n", cmds[i][0]);
                    if(buf[i]){
                        free(buf[i]);
                        buf[i] = NULL;
                    }
                    exit(0);
                }
            }
        }

        while(child_count != command_count);
        // 
        for(int i=0; buf[i];i++){
            if(buf[i]){
                printf("%s", buf[i]);
                free(buf[i]);
                buf[i] = NULL;
            }
        }
        for(int i=0;i<6;i++){
            if(cmds[i]){
                for(int j=0;j<30;j++){
                    if(cmds[i][j]){
                        free(cmds[i][j]);
                        cmds[i][j] = NULL;
                    }
                }

            }
        }
        for(int i=0; i<command_count;i++){
            close(pipes[i][0]);
            close(pipes[i][1]);
            free(pipes[i]);
        }
    }
}
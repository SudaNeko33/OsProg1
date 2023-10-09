#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>
#include<signal.h>
pid_t pid = -1;

struct proc {
    int PID;
    char CMD[100];
    char STATE;
    int EXCODE;
    char EXSIG[20];
    int PPID;
    char USER[6];
    char SYS[6];
    int VCTX;
    int NVCTX;
};

// signal in JCshell process
void sigint_handler(int signum){
    if(signum == SIGINT){
        if(pid == getpid()){
            printf("\n");
            fclose(stdin);
        }
    }
}

// signal in child process
void sigintchild_handler(int signum){
    printf("child sigint\n");
    exit(1);
}

struct proc print_proc(pid_t pid){
    if(pid == 0) return (struct proc){0};
    struct proc p_tmp;

}


void sigchild_handler(int signum){
    if(signum == SIGCHLD){
        struct proc 
    }
}

char** split_multi_cmd(char* commands, int* num_of_cmds){
    char* cmds_remain = strtok(commands, "|");
    int cnt_cmds = 0;
    char** args = (char**)malloc(sizeof(char)*6);
    memset(args, 0, sizeof(char)*6);
    while(cmds_remain){
        char* cmds_cur = cmds_remain;
        cmds_remain = strtok(NULL, "|");
        while(cmds_cur[0] == ' ') cmds_cur++;
        if(cnt_cmds == 0){
            if(cmds_remain){
                args[0] = (char*)malloc(sizeof(char)*(cmds_remain-cmds_cur+1));
            }else{
                args[0] = (char*)malloc(sizeof(char)*(commands+1024-cmds_cur));
            }
            strcpy(args[0], cmds_cur);
        }else if(!cmds_remain){
            args[cnt_cmds] = (char*)malloc(sizeof(char)*(commands+1024-cmds_cur));
            strcpy(args[cnt_cmds], cmds_cur);
        }else{
            args[cnt_cmds] = (char*)malloc(sizeof(char)*(cmds_remain-cmds_cur+1));
            strcpy(args[cnt_cmds], cmds_cur);
        }
        cnt_cmds++;
        if(cnt_cmds > 5){
            printf("JCshell: too many commands\n");
            return NULL;
        }
    }
    *num_of_cmds = cnt_cmds;
    return args;
}

char** split_single_cmd(char* command){
    while(command[0] == ' ') command++;
    char* cmd_remain = strtok(command, " ");
    int cnt_str = 0;
    char** args = (char**)malloc(sizeof(char*) * 30);
    memset(args, 0, sizeof(char*) * 30);
    while(cmd_remain){ // while the command(not loaded) is not empty
        char* cmd_cur = cmd_remain; // cmd_cur points to cmd_remain
        cmd_remain = strtok(NULL, " "); // cmd_remain points to the next command
        while(cmd_cur[0] == ' ') cmd_cur++; // delete spaces before cmd_cur
        if (cnt_str == 0){
            if (!cmd_remain){ 
            // cmd_remain is empty
            // cmd_cur is the only command, no spaces after, e.g. ls
                args[0] = (char*)malloc(sizeof(char) * (command + 1024 - cmd_cur));
                // dont know length of cmd_cur, so the longest is (command + 1024 - cmd_cur), which = 1024
                strcpy(args[0], cmd_cur);
            // quit the loop
            }
            else { // cmd_cur is the first command, and with spaces afterwards
                args[0] = (char*)malloc(sizeof(char) * (cmd_remain - cmd_cur + 1));
                // the distance between two pointers is the length of this command
                strcpy(args[0], cmd_cur);
            }
        }
        else{ // command string in the middle
            if (!cmd_remain){ // cmd_cur is the last command
                // dont know how long it is
                args[cnt_str] = (char*)malloc(sizeof(char) * (command + 1024 - cmd_cur));
                strcpy(args[cnt_str], cmd_cur);
            }
            else{ // cmd_cur is not the last command
                args[cnt_str] = (char*)malloc(sizeof(char) * (cmd_remain - cmd_cur + 1));
                strcpy(args[cnt_str], cmd_cur);
            }
        }
        cnt_str++;
        if (cnt_str > 30){
            printf("JCshell: too many commands\n");
            return NULL;
        }
        if(memcmp(args[0], "exit", 4) == 0){
            if(cnt_str > 1){
                printf("\"exit\" with too many arguments\n");
                return NULL;
            }
            else{
                printf("JCshell: Terminated\n");
                exit(0);
            }
        }
    }
    return args;
}

int command_count = 0;
int child_count = 0;
int main(int argc, char *argv[]){
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    pid = getpid();
    while(1){
        printf("## JCshell [%d] ## ", pid);
        command_count = 0;
        char input[1024] = {'\0'};
        char** commands = analysis_multi(input, &command_count);
        if(commands == NULL){ 
            continue;
        }
        pid_cnt = 0;
        
        char** cmds[6];
        int valid = 1;
        for(int i=0;i<command_count;i++){
            cmds[i] = analysis_single(commands[i]);
            if(cmds[i] == NULL) {
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
            // printf("cmd %d\n", i);
            pid_t p = fork();
            pids[i] = p;
            if(p != 0){
                continue;
            }else{
                signal(SIGINT, sigintchild_handler);
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

                if(execvp(cmds[i][0], cmds[i]) == -1){
                    printf("JCshell: \'%s\': No such file or directory\n", cmds[i][0]);
                    exit(0);
                }

                
            }
        }

        while(child_count != command_count);
        // 
        for(int i=0; buf[i];i++){
            printf("%s", buf[i]);
        }
    }

    
    
    
}
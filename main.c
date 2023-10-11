#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>
#include<signal.h>
#include <sys/mman.h>

pid_t pid = -1;

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

int* shared_var = 0  ;

struct proc p ;
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

void sigint_handler(int signum){
    if(signum == SIGINT ){
        if(pid == getpid()){
            printf("\n");
            fclose(stdin);
        }
    }
}

void sigintchild_handler(int sig){
    exit(0);
}

void sigusr1_handler(int sig){
    printf("Receive sigusr1, start\n");
    *shared_var = 1;
    
}


void print_proc(pid_t cpid){
        if(cpid == 0) return ;
        memset(&p, 0, sizeof(struct proc));
        p.CMD = (char*)malloc(sizeof(char)*100);
        // pid_t cpid = wait(NULL);
        // pid_t cpid = getpid();
        char prefix[100] ;
        

        sprintf(prefix, "/proc/%d/stat", cpid);
        
        FILE* fp = fopen(prefix, "r");
        if(fp == NULL){
            printf("JCshell: \'%d\': No such process\n", cpid);
            return ;
        }
        
        char waste[100]={0};
        int c=0;
     
        fscanf(fp, "%d", &p.PID);
        // printf("%d %c\n", c,p.PID);
        // printf("%d\n", strlen(waste));
        fscanf(fp, "%s %c %d", p.CMD, &p.STATE, &p.PPID);
        

        // printf("\nstatus %d\n", status);
        for(int i=0 ;i<9;i++){
            fscanf(fp, "%s", waste);
        }
        
        fscanf(fp, "%d %d", &p.USER, &p.SYS);

        for(int i=0; i<22;i++){
            fscanf(fp, "%s", waste);
        }
        fscanf(fp, "%d", &p.EXCODE);
        fclose(fp);

        
        sprintf(prefix, "/proc/%d/status", cpid);
        fp = fopen(prefix, "r");
        char title[20];
        if(fp == NULL){
            printf("JCshell: \'%d\': No such process\n", cpid);
            return ;
        }
        while(fscanf(fp, "%s", title) != EOF){
            if(strcmp(title, "voluntary_ctxt_switches:") == 0){
                fscanf(fp, "%d", &p.VCTX);
            }else if(strcmp(title, "nonvoluntary_ctxt_switches:") == 0){
                fscanf(fp, "%d", &p.NVCTX);
            }
        }

        fclose(fp);
        
        // return p;
        // exit(0);
    
}

void sigchld_handler(int sig){
    if(sig == SIGCHLD){
        
        print_proc(pids[pid_cnt]);

        pid_cnt++;
        if(child_count != command_count){
            close(pipes[child_count][0]);
            close(pipes[child_count][1]);
            
        }
        
        wait(NULL);
        // printf("status %d\n", status);
        if(WIFEXITED(status)){
            p.EXCODE = WEXITSTATUS(status);
        }else{
            p.EXCODE = WTERMSIG(status);
        }

        child_count++;
        // p.EXCODE /= 256;
        if(p.EXCODE == SIGINT){
            strcpy(p.EXSIG, "Interrupt");
        }else if(p.EXCODE == SIGKILL){
            strcpy(p.EXSIG, "Killed");
        }else if(p.EXCODE == 0){
            strcpy(p.EXSIG, "\0");
        }else{
            strcpy(p.EXSIG, "Other Signal");
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
        // printf("%s\n",p.CMD);
    }
}

void analysis_multi(char* input, int* cnt){
    char* temp2 = strtok(input, "|");
    int count = 0;
    char* cmd;
    // char** args = (char**)malloc(sizeof(char)*6);
    memset(commands, 0, sizeof(char*)*6);
    while(temp2){
        char* temp1 = temp2;
        temp2 = strtok(NULL, "|");
        while(temp1[0] == ' '){
            temp1++;
        }
        if(count == 0){
            if(temp2){
                cmd = (char*)malloc(sizeof(char)*(temp2-temp1+1));
            }else{
                cmd = (char*)malloc(sizeof(char)*(input+1025-temp1));
            }
            strcpy(cmd, temp1);
            // printf("%d\n", cmd[1024]);
            commands[0] = cmd;
        }else if(!temp2 && count !=0){
            commands[count] = (char*)malloc(sizeof(char)*(input+1025-temp1));
            strcpy(commands[count], temp1);
            
        }else{
            commands[count] = (char*)malloc(sizeof(char)*(temp2-temp1+1));
            strcpy(commands[count], temp1);
        }
        count ++;
        // printf("%s\n", args[count-1]);
        if(count > 5){
            printf("JCshell: too many commands\n");
            return ;
        }
    }
    // printf("%d\n", count);
    *cnt = count;
    return ;
}

// single command no pipe
void analysis_single(char* command, int i){
    while(command[0] == ' '){
        command++;
    }
    // printf("%s\n", command);
    // strtok(char s[], const char *delim)
    // 首次调用时s为要分解的字符串，delim为分隔符字符串。
    // 之后s为NULL
    char* temp2 = strtok(command, " ");
    int count = 0; // count string number
    char* cmd;
    // char** args = (char**)malloc(sizeof(char)*30);
    // memset(args, 0, sizeof(char)*30); // why 31?
    while(temp2){
        char* temp1 = temp2;
        temp2 = strtok(NULL, " ");
        if(temp1 == temp2){
            continue;
        }
        // 从这里开始没懂
                
        
        if(count == 0){
            if(temp2){ // 
                cmd = (char*)malloc(sizeof(char)*(temp2-temp1+1)); // temp2-temp1 是两个指针之间的距离
            }else{ // temp2 是 NULL 只有一条 没有后面空格什么的 eg ls
                cmd = (char*)malloc(sizeof(char)*(command+1025-temp1));
            }
            strcpy(cmd, temp1);
            cmds[i][0] = cmd;
            
        }else if(!temp2 && count !=0){ // temp2 is null 处理最后一个参数（前面有参数）的时候
            cmds[i][count] = (char*)malloc(sizeof(char)*(command+1025-temp1));
            strcpy(cmds[i][count], temp1);            
        }else{
            cmds[i][count] = (char*)malloc(sizeof(char)*(temp2-temp1+1));
            strcpy(cmds[i][count], temp1);
        }
        // printf("%d-%d\n",i, count);
        count ++;
        if(count > 30){
            printf("JCshell: too many arguments\n");
            return ;
        }
    }
    if(memcmp(cmd, "exit", 4) == 0){
        if(count > 1){
            printf("\"exit\" with other arguments\n");
            return ;
        }
        else{
            printf("JCshell: Terminated\n");
            exit(0);
        }
    }
    
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    signal(SIGUSR1, sigusr1_handler);   
    size_t shared_var_size = sizeof(int);
    shared_var = (int *)mmap(NULL, shared_var_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid = getpid();
    while (1){
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
        analysis_multi(input, &command_count);
        
        if(commands == NULL){ 
            continue;
        }
        
       
        // char** cmds[6];
        int valid = 1;
        for(int i=0;i<command_count;i++){
            analysis_single(commands[i], i);
            // printf("%s: command\n", commands[i]);
            if(cmds[i][0] == NULL) {
                valid = 0;
                break;
            }
            // printf("1");
        }
        
        if(!valid) {
            continue;
        }
        
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
                // while(*shared_var == 0);

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
        // printf("%s\n", input);
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
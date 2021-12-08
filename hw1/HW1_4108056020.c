#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
char *cmd, promp[100]={'\0'}, cwd[100]={'\0'}, buf[100]={'\0'};
void handler(int);
struct sigaction internal_c = { .sa_handler = handler, .sa_flags = 0 },
                 internal_z = { .sa_handler = handler, .sa_flags = 0 },
                 external_c = { .sa_handler = handler, .sa_flags = 0 },
                 external_z = { .sa_handler = handler, .sa_flags = 0 };
int excmd(){
    switch(fork()){
        case -1:{
            perror("Error");
            return EXIT_FAILURE;
        }
        case 0:{
            sigemptyset(&external_c.sa_mask);
            sigemptyset(&external_z.sa_mask);
            sigaction(SIGINT, &external_c, NULL);
            sigaction(SIGTSTP, &external_z, NULL);
            char *argv[100]; int argc = 0;
            for(char *t = strtok(cmd, " "), *arg; t; t = strtok(NULL, " ")){
                arg = malloc(strlen(t) + 1);
                if(arg) strcpy(arg, t);
                argv[argc++] = arg;
            }
            argv[argc] = NULL;
            execvp(argv[0], argv);
            return EXIT_SUCCESS;
        }
        default:
            wait(NULL);
            return EXIT_SUCCESS;
    }
}
void handler(int signum){
    puts("");
    switch(signum){
        case SIGINT:{ //puts("SIGINT");
            write_history(".bash_history");
            printf("kill(%d,%d)", getpid(), SIGTERM);
            kill(getpid(), SIGTERM);
            return;
        }
        case SIGTSTP:{ //puts("SIGTSTP"); //TO-DO
            register HIST_ENTRY **history = history_list();
            register int i;
            for(i = -1; history[++i];);
            cmd = history[--i]->line;
            printf(cmd);
            excmd();
        }
    }
}
int main(){
    printf("4108056020 shell ");
    sigemptyset(&internal_c.sa_mask);
    sigemptyset(&internal_z.sa_mask);
    sigaction(SIGINT, &internal_c, NULL);
    sigaction(SIGTSTP, &internal_z, NULL);
    using_history();
    read_history(".bash_history");
    puts("start");
    for(int h;;){
        sprintf(promp, "%s$ ", getcwd(cwd,sizeof(cwd)));
        cmd = readline(promp);
        if(cmd[0]){
            char *expansion;
            switch(history_expand(cmd, &expansion)){
                case -1: case 2:
                    free(expansion);
                    continue;
            }
            add_history(expansion);
            strncpy(cmd, expansion, sizeof(cmd) - 1);
            free(expansion);
        }
        if(sscanf(cmd,"export %[^=]=%[^\n]\n", cwd, buf)>0){
            setenv(cwd, buf, 1);
            printf("%s=%s\n",cwd,getenv(cwd));
            continue;
        }else if(sscanf(cmd,"echo %[^\n]\n", buf)>0){
            puts(buf);
            continue;
        }else if(!strcmp(cmd, "pwd")){
            puts(getcwd(cwd,sizeof(cwd)));
            continue;
        }else if(sscanf(cmd,"cd %[^\n]\n", buf)>0){
            chdir(buf);
            continue;
        }
        else if(!strcmp(cmd, "history")){
            register HIST_ENTRY **history = history_list();
            for(register int i = 0; history[i]; i++)
                printf("%d %s\n", i + history_base, history[i]->line);
            continue;
        }else if(sscanf(cmd,"history %d", &h)>0 && h>0){
            register HIST_ENTRY **history = history_list();
            register int current;
            for(current = 0; history[current++];);
            if(h > current--) continue;
            for(register int i = current - h; history[i]; i++)
                printf("%d %s\n", i + history_base, history[i]->line);
            continue;
        }
        else if(!strcmp(cmd, "exit")){
            write_history(".bash_history");
            return EXIT_SUCCESS;
        }else if(excmd() == EXIT_SUCCESS) continue;
        //else perror("Error");
    }
}
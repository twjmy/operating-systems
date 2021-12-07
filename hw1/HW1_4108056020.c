#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
void handler(int signum){
    write_history(".bash_history");
    if(signum == SIGINT){
        puts("");
        exit(1);
    }
}
int main(){
    printf("4108056020 shell ");
    struct sigaction sa={
        .sa_handler = handler,
        .sa_flags = 0
    };
    char *cmd, promp[100]={'\0'}, cwd[100]={'\0'}, buf[100]={'\0'};
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
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
        }else if(sscanf(cmd,"echo %[^\n]\n",buf)>0){
            puts(buf);
            continue;
        }else if(!strcmp(cmd, "pwd")){
            puts(getcwd(cwd,sizeof(cwd)));
            continue;
        }else if(sscanf(cmd,"cd %[^\n]\n",buf)>0){
            chdir(buf);
            continue;
        }else if(strcmp(cmd, "history") == 0){
            register HIST_ENTRY **the_list = history_list();
            for(register int i = 0; the_list[i]; i++)
                printf("%d %s\n", i + history_base, the_list[i]->line);
            continue;
        }else if(sscanf(cmd,"history %d", &h)>0 && h>0){
            register HIST_ENTRY **the_list = history_list();
            register int current;
            for(current = 0; the_list[current++];);
            if(h > current--) continue;
            for(register int i = current - h; the_list[i]; i++)
                printf("%d %s\n", i + history_base, the_list[i]->line);
            continue;
        }
        // for debug: else perror("Error");
        switch(fork()){
            case -1:{
                perror("Error");
                return EXIT_FAILURE;
            }
            case 0:{
                char *argv[100];
                int argc = 0;
                for(char *str = strtok(cmd, " "), *arg; str != NULL;){
                    arg = malloc(strlen(str) + 1);
                    if(arg != NULL) strcpy(arg, str);
                    argv[argc++] = arg;
                    str = strtok(NULL, " ");
                }
                argv[argc] = NULL;
                execvp(argv[0], argv);
                continue;
            }
            default:{
                wait(NULL);
                continue;
            }
        }
    }
}
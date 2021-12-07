#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
main(){
    printf("4108056020 shell start\n");
    char cmd[100]={'\0'}, cwd[100]={'\0'}, buf[100]={'\0'};
    for(;cmd;){
        printf("%s$",getcwd(cwd,sizeof(cwd)));
        scanf("%[^\n]",cmd); getchar();
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
        }// for debug: else perror("Error");
        switch(fork()){
            case -1:{
                perror("Error");
                return EXIT_FAILURE;
            }
            case 0:{
                char *argv[100];
                int argc = 0;
                for(char *str = strtok(cmd, " "), *arg; str != NULL;) {
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
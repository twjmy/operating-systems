#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
main(){
    printf("4108056020 shell start");
    char cmd[100]={'\0'}, cwd[100]={'\0'}, buf[100]={'\0'};
    for(;cmd;){
        printf("\n%s$",getcwd(cwd,sizeof(cwd)));
        scanf("%[^\n]",cmd); getchar();
        if(sscanf(cmd,"export %[^=]=%[^\n]\n", cwd, buf)>0){
            setenv(cwd, buf, 1);
            printf("%s=%s",cwd,getenv(cwd));
            continue;
        }else if(sscanf(cmd,"echo %[^\n]\n",buf)>0){
            printf(buf);
            continue;
        }else if(!strcmp(cmd, "pwd")){
            printf("%s",getcwd(cwd,sizeof(cwd)));
            continue;
        }else if(sscanf(cmd,"cd %[^\n]\n",buf)>0){
            chdir(buf);
            continue;
        }// for debug: else perror("Error");
    }
}
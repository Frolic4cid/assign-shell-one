#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BSIZE 256

void sigint_handler(int sig);
/***********************************************************************
 * This starter shell implements a simple shell that implements:
 * 
 * (1) An internal EXIT command that exits the shell.
 * (2) Executes an external program with command line arguments
 * 
 ***********************************************************************/
int main(int argc, char *argv[]){
    char buffer[BSIZE];
    pid_t child;
    int status;
    char *args[10];

    // Set up the SIGINT handler
    struct sigaction sa = {0};
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    while(1){
        printf("shell> ");
        fgets(buffer,BSIZE,stdin);
        buffer[(strlen(buffer)-1)]='\0';  //Remove NewLine

        char *token = strtok(buffer," \n");

        int i = 0;
        while (token != NULL) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        //** Internal Command **
        if (args[0] == NULL) {
            continue; // No command entered, prompt again
        }
        if(strcmp(args[0],"exit")==0){
            break;
        }
        if(strcmp(args[0],"cd")==0){
            char *directory;

            //I wanted it to return home like linux normally does when you type cd with no arguments or cd ~
            if(args[1]==NULL || strcmp(args[1],"~")==0){
                directory = getenv("HOME");
            } else {
                directory = args[1];
            }

            if (directory == NULL) {
                fprintf(stderr, "cd: HOME not set\n");
                continue;
            }

            if(chdir(directory)!=0){
                perror("cd failed");
            }
            continue;
        }
        //** External Command **
        child = fork();
        if(child==0){
            //*** Child Process ***
            // Make sure sigint works for the child process
            signal(SIGINT, SIG_DFL);

            execvp(args[0],args);

            // If execvp returns, it must have failed.
            fprintf(stderr, "Program execution failed\n");
            exit(1);
        }else{
            //*** Parent Process **
            waitpid(child,&status,0);
        }

    }
    return 0;
}

// Makes it so the shell won't terminate when SIGINT is received
void sigint_handler(int sig) {
    (void)sig;
    write(STDOUT_FILENO, "\n", 1);
}
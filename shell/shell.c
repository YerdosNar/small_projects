#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void signal_handle(int sig) {
    printf("\n\033[36m===================\n");
    printf("     Exiting...\n");
    printf("===================\033[0m\n");
    exit(sig);
}

int main() {
    signal(SIGINT, signal_handle);
    size_t str_max = 0;
    char pwd[1024];
    char *lineptr = NULL;
    while(1) {
        if(getcwd(pwd, sizeof(pwd)) == NULL) {
            perror("getcwd");
            return 1;
        }
        printf("\033[34m%s> \033[0m", pwd);

        ssize_t chars_read = getline(&lineptr, &str_max, stdin);
        if(chars_read == -1) break;

        if(lineptr[chars_read - 1] == '\n') lineptr[chars_read - 1] = '\0';
        if(chars_read == 1) continue;

        char *tokens[64];
        int i = 0;

        char *token = strtok(lineptr, " ");
        while(token != NULL && i < 63) {
            tokens[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        tokens[i] = NULL;

        if(!strncmp(tokens[0], "exit", 4)) signal_handle(0);
        if(!strncmp(tokens[0], "cd", 2)) {
            if(tokens[1] == NULL) {
                if(chdir("/home/yerdos/") != 0) perror("cd failed");
            } else {
                if(chdir(tokens[1]) != 0) perror("cd failed");
            }
            continue;
        }

        pid_t fork_pid = fork();
        if(fork_pid < 0) perror("fork failed");
        else if(fork_pid == 0) {
            execvp(tokens[0], tokens);
            perror("Command failed...");
            exit(1);
        }
        else {
            int status;
            waitpid(fork_pid, &status, 0);
        }
    }

    free(lineptr);
    return 0;
}

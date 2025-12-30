#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>

void signal_handle(int sig) {
    printf("\n\033[36m===================\n");
    printf("     Exiting...\n");
    printf("===================\033[0m\n");
    exit(sig);
}

int main() {
    signal(SIGINT, signal_handle);
    struct passwd *info = getpwuid(1000);
    char *home_dir = info->pw_dir;
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
                if(chdir(home_dir) != 0) perror("cd failed");
            } else {
                if(chdir(tokens[1]) != 0) perror("cd failed");
            }
            continue;
        }

        int pipe_idx = -1;
        for(int j = 0; j < i; j++) {
            if(!strcmp(tokens[j], "|")) {
                printf("PIPE FOUND: %d\n", j);
                pipe_idx = j;
                break;
            }
        }

        if(pipe_idx != -1) {
            tokens[pipe_idx] = NULL;
            char **cmd1 = tokens;
            char **cmd2 = &tokens[pipe_idx + 1];

            int fd[2];
            if(pipe(fd) == -1) {
                perror("pipe failed");
                continue;
            }

            pid_t pid1 = fork();
            if(pid1 == -1) {
                perror("pid1 fork failed");
                exit(1);
            }
            if(pid1 == 0) {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);

                execvp(cmd1[0], cmd1);
                perror("execvp cmd1");
                exit(1);
            }

            pid_t pid2 = fork();
            if(pid2 == -1) {
                perror("pid2 fork failed");
                exit(1);
            }
            if(pid2 == 0) {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);

                execvp(cmd2[0], cmd2);
                perror("execvp cmd2");
                exit(1);
            }

            close(fd[0]);
            close(fd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        } else {
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
    }

    free(lineptr);
    return 0;
}

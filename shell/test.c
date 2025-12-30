#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

// Parse a single command into arguments (e.g., "ls -l" → ["ls", "-l", NULL])
char** parse_cmd(char* cmd) {
    char** args = malloc(MAX_ARGS * sizeof(char*));
    char* token = strtok(cmd, " \t\n");
    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return args;
}

// Split input into pipe-separated commands (returns array of commands, sets num_cmds)
char** split_pipes(char* input, int* num_cmds) {
    *num_cmds = 1;
    for (int i = 0; input[i]; i++) { if (input[i] == '|') (*num_cmds)++; }

    char** cmds = malloc(*num_cmds * sizeof(char*));
    char* token = strtok(input, "|");
    int i = 0;
    while (token != NULL && i < *num_cmds) {
        // Trim whitespace
        char* trimmed = token;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        char* end = trimmed + strlen(trimmed) - 1;
        while (end > trimmed && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
        *(end + 1) = '\0';

        cmds[i++] = trimmed;
        token = strtok(NULL, "|");
    }
    return cmds;
}

int main() {
    char input[MAX_INPUT];
    while (1) {
        printf("minishell> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) break; // Handle Ctrl+D

        int num_cmds;
        char** cmds = split_pipes(input, &num_cmds);

        if (num_cmds == 1) { // No pipe: run single command (reuse original logic)
            char** args = parse_cmd(cmds[0]);
            if (!args[0]) { free(args); free(cmds); continue; }

            pid_t pid = fork();
            if (pid == -1) { perror("fork"); exit(1); }
            if (pid == 0) { execvp(args[0], args); perror("execvp"); exit(1); }
            else { waitpid(pid, NULL, 0); free(args); }
        } else if (num_cmds == 2) { // Two-command pipeline (e.g., cmd1 | cmd2)
            int fd[2];
            if (pipe(fd) == -1) { perror("pipe"); exit(1); } // Create pipe

            // Fork child for cmd1
            pid_t pid1 = fork();
            if (pid1 == -1) { perror("fork"); exit(1); }

            if (pid1 == 0) { // Child 1: cmd1 (write to pipe)
                close(fd[0]); // Close unused read end
                dup2(fd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
                close(fd[1]); // Close original write end (dup2 copies it)

                char** args = parse_cmd(cmds[0]);
                execvp(args[0], args);
                perror("execvp (cmd1)");
                exit(1);
            }

            // Fork child for cmd2
            pid_t pid2 = fork();
            if (pid2 == -1) { perror("fork"); exit(1); }

            if (pid2 == 0) { // Child 2: cmd2 (read from pipe)
                close(fd[1]); // Close unused write end
                dup2(fd[0], STDIN_FILENO); // Redirect stdin to pipe read end
                close(fd[0]); // Close original read end (dup2 copies it)

                char** args = parse_cmd(cmds[1]);
                execvp(args[0], args);
                perror("execvp (cmd2)");
                exit(1);
            }

            // Parent: close both pipe ends and wait for children
            close(fd[0]);
            close(fd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }

        free(cmds);
    }
    return 0;
}

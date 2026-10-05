// OSSP Skill 21 - Module Integration and Runtime Error Handling
// Demonstrates modular-style functions, interface validation,
// syntax checking, runtime errors, messages, logging, and recovery.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

static void log_error(const char *message) {
    FILE *log = fopen("skill21_errors.log", "a");

    if (log) {
        fprintf(log, "%s\n", message);
        fclose(log);
    }
}

static int validate_command(const char *cmd) {
    if (!cmd || *cmd == '\0') {
        fprintf(stderr, "Syntax error: empty command.\n");
        log_error("Empty command");
        return 0;
    }

    if (strstr(cmd, "||")) {
        fprintf(stderr, "Syntax error: invalid pipeline operator.\n");
        log_error("Invalid pipeline syntax");
        return 0;
    }

    return 1;
}

static int run_command(const char *cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        log_error("fork failed");
        return -1;
    }

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        perror("exec");
        _exit(127);
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        log_error("waitpid failed");
        return -1;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Command failed with status %d: %s",
                 WEXITSTATUS(status), cmd);
        fprintf(stderr, "%s\n", msg);
        log_error(msg);
    }

    return 0;
}

int main(void) {
    char line[512];

    printf("=== Skill 21: Integration + Error Handling ===\n");
    printf("Try: echo Hello | wc -c\n");
    printf("Try an invalid command to see recovery.\n");
    printf("Type 'exit' to quit.\n\n");

    while (1) {
        printf("integrated> ");

        if (!fgets(line, sizeof(line), stdin))
            break;

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0)
            break;

        if (!validate_command(line))
            continue;

        run_command(line);
    }

    printf("Program ended gracefully. Check skill21_errors.log for failures.\n");
    return 0;
}

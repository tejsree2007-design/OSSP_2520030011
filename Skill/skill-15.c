// OSSP Skill 15 - Combined Redirection
// Demonstrates stdout + stderr merged into one file using dup2().

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(void) {
    const char *file = "skill15_combined.txt";

    printf("=== Skill 15: Combined stdout + stderr Redirection ===\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            _exit(1);
        }

        // Both streams point to the same open file description.
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);

        printf("Normal output: stdout message.\n");
        fprintf(stderr, "Error output: stderr message.\n");

        fflush(stdout);
        fflush(stderr);
        _exit(0);
    }

    waitpid(pid, NULL, 0);

    printf("Combined output written to %s\n", file);
    printf("Reading the file:\n");

    FILE *f = fopen(file, "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), f))
        printf("%s", line);

    fclose(f);
    return 0;
}

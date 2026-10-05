// OSSP Skill 14 - Append and stderr Redirection
// Demonstrates >> append mode and 2> stderr redirection.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

static void run_append_demo(void) {
    pid_t pid = fork();

    if (pid == 0) {
        int fd = open("skill14_append.txt",
                      O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            perror("open append");
            _exit(1);
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);

        printf("This line was appended by the child.\n");
        fflush(stdout);
        _exit(0);
    }

    waitpid(pid, NULL, 0);
}

static void run_stderr_demo(void) {
    pid_t pid = fork();

    if (pid == 0) {
        int fd = open("skill14_error.txt",
                      O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
            _exit(1);

        dup2(fd, STDERR_FILENO);
        close(fd);

        fprintf(stderr, "This is an error message redirected to a file.\n");
        fflush(stderr);
        _exit(0);
    }

    waitpid(pid, NULL, 0);
}

int main(void) {
    printf("=== Skill 14: Append + stderr Redirection ===\n");

    run_append_demo();
    run_append_demo();
    run_stderr_demo();

    printf("Created skill14_append.txt using append mode.\n");
    printf("Created skill14_error.txt using stderr redirection.\n");

    return 0;
}

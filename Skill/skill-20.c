// OSSP Skill 20 - SIGTSTP and Process Groups
// Demonstrates process groups, SIGTSTP suspension,
// SIGCONT resume, and group-based signal delivery.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main(void) {
    printf("=== Skill 20: SIGTSTP + Process Groups ===\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        setpgid(0, 0);

        printf("Child pid=%d pgid=%d\n", getpid(), getpgrp());
        fflush(stdout);

        for (int i = 1; i <= 8; i++) {
            printf("Child running: %d\n", i);
            fflush(stdout);
            sleep(1);
        }

        _exit(0);
    }

    sleep(2);

    printf("Parent sends SIGTSTP to the child process group.\n");
    if (kill(-pid, SIGTSTP) == -1)
        perror("SIGTSTP");

    int status;
    waitpid(pid, &status, WUNTRACED);

    if (WIFSTOPPED(status))
        printf("Child stopped by signal %d.\n", WSTOPSIG(status));

    printf("Parent resumes the group using SIGCONT.\n");
    if (kill(-pid, SIGCONT) == -1)
        perror("SIGCONT");

    waitpid(pid, &status, 0);

    printf("Child completed after resume.\n");
    return 0;
}

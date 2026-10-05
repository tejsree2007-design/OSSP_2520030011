// OSSP Skill 18 - Signal Delivery and Resume Stopped Jobs
// Demonstrates process groups, SIGSTOP, SIGCONT, signal delivery,
// stopped state, and background recovery.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main(void) {
    printf("=== Skill 18: Signals + Resume Stopped Job ===\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        setpgid(0, 0);
        printf("Child pid=%d, pgid=%d running.\n", getpid(), getpgrp());
        fflush(stdout);

        for (int i = 1; i <= 10; i++) {
            printf("Child working... %d\n", i);
            sleep(1);
        }

        _exit(0);
    }

    sleep(2);

    printf("\nParent sends SIGSTOP to the child process group.\n");
    if (kill(-pid, SIGSTOP) == -1)
        perror("kill SIGSTOP");

    int status;
    waitpid(pid, &status, WUNTRACED);

    if (WIFSTOPPED(status))
        printf("Child is stopped by signal %d.\n", WSTOPSIG(status));

    printf("Parent sends SIGCONT to resume the stopped job.\n");
    if (kill(-pid, SIGCONT) == -1)
        perror("kill SIGCONT");

    waitpid(pid, &status, 0);

    printf("Child resumed and completed.\n");
    return 0;
}

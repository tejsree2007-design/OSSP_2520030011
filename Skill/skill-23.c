// OSSP Skill 23 - Large Pipelines and Multiple Jobs
// Demonstrates a longer pipeline, multiple child processes,
// stability/error monitoring, synchronization, and timing.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define STAGES 4

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void) {
    printf("=== Skill 23: Large Pipeline + Multiple Processes ===\n");
    printf("Pipeline: printf -> tr -> sort -> wc\n");

    int pipes[STAGES - 1][2];

    for (int i = 0; i < STAGES - 1; i++)
        if (pipe(pipes[i]) == -1)
            die("pipe");

    pid_t pids[STAGES];
    clock_t start = clock();

    for (int i = 0; i < STAGES; i++) {
        pids[i] = fork();

        if (pids[i] == -1)
            die("fork");

        if (pids[i] == 0) {
            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);

            if (i < STAGES - 1)
                dup2(pipes[i][1], STDOUT_FILENO);

            for (int j = 0; j < STAGES - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (i == 0)
                execlp("printf", "printf",
                       "banana\napple\norange\napple\n", NULL);
            else if (i == 1)
                execlp("tr", "tr", "a-z", "A-Z", NULL);
            else if (i == 2)
                execlp("sort", "sort", NULL);
            else
                execlp("wc", "wc", "-l", NULL);

            perror("exec");
            _exit(127);
        }
    }

    for (int i = 0; i < STAGES - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int failures = 0;

    for (int i = 0; i < STAGES; i++) {
        int status;

        if (waitpid(pids[i], &status, 0) == -1) {
            perror("waitpid");
            failures++;
            continue;
        }

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            failures++;
    }

    double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;

    printf("\nAll %d processes synchronized.\n", STAGES);
    printf("Failures detected: %d\n", failures);
    printf("Measured CPU time: %.6f seconds\n", elapsed);

    return failures ? 1 : 0;
}

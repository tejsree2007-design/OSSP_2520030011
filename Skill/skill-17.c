// OSSP Skill 17 - jobs and foreground switching
// Demonstrates job table, target job identification,
// terminal foreground concept, waiting for selected jobs,
// and state updates.
//
// This educational version simulates foreground switching
// without changing the user's terminal process group.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_JOBS 8

typedef struct {
    int id;
    pid_t pid;
    int active;
    int stopped;
} Job;

static Job jobs[MAX_JOBS];
static int next_id = 1;

static int add_job(pid_t pid) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (!jobs[i].active) {
            jobs[i] = (Job){next_id++, pid, 1, 0};
            return jobs[i].id;
        }
    }
    return -1;
}

static Job *find_job(int id) {
    for (int i = 0; i < MAX_JOBS; i++)
        if (jobs[i].active && jobs[i].id == id)
            return &jobs[i];
    return NULL;
}

static void list_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++)
        if (jobs[i].active)
            printf("[%d] pid=%d %s\n", jobs[i].id, jobs[i].pid,
                   jobs[i].stopped ? "Stopped" : "Running");
}

int main(void) {
    printf("=== Skill 17: jobs + foreground ===\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("Child pid=%d running for 5 seconds...\n", getpid());
        sleep(5);
        _exit(0);
    }

    int id = add_job(pid);
    printf("Started job [%d] pid=%d\n", id, pid);

    list_jobs();

    printf("\nSwitching job [%d] to foreground...\n", id);
    Job *job = find_job(id);

    if (!job) {
        printf("Job not found.\n");
        return 1;
    }

    int status;
    if (waitpid(job->pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    job->active = 0;
    printf("Foreground job [%d] completed. Terminal control returns to shell.\n", id);

    return 0;
}

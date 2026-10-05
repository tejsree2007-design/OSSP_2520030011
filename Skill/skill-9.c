// OSSP Skill 9 - Built-in Command Dispatch
// Demonstrates a dispatch table, in-process built-ins,
// cd/path validation, previous directory, and invalid commands.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define INPUT_SIZE 256

static char previous_dir[PATH_MAX] = "";

static void builtin_pwd(char **args) {
    (void)args;
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)))
        printf("%s\n", cwd);
    else
        perror("pwd");
}

static void builtin_cd(char **args) {
    char old[PATH_MAX];
    char target[PATH_MAX];

    if (!getcwd(old, sizeof(old))) {
        perror("getcwd");
        return;
    }

    if (!args[1] || strcmp(args[1], "~") == 0) {
        const char *home = getenv("HOME");
        snprintf(target, sizeof(target), "%s", home ? home : "/");
    } else if (strcmp(args[1], "-") == 0) {
        if (previous_dir[0] == '\0') {
            printf("cd: no previous directory\n");
            return;
        }
        snprintf(target, sizeof(target), "%s", previous_dir);
    } else {
        snprintf(target, sizeof(target), "%s", args[1]);
    }

    if (chdir(target) == -1) {
        fprintf(stderr, "cd: %s: %s\n", target, strerror(errno));
        return;
    }

    snprintf(previous_dir, sizeof(previous_dir), "%s", old);

    if (strcmp(args[1] ? args[1] : "", "-") == 0)
        builtin_pwd(NULL);
}

static void builtin_help(char **args) {
    (void)args;
    printf("Built-ins: pwd, cd, help, exit\n");
}

static void builtin_exit(char **args) {
    (void)args;
    printf("Exiting built-in shell...\n");
    exit(0);
}

typedef void (*builtin_fn)(char **);

struct builtin {
    const char *name;
    builtin_fn fn;
};

static struct builtin table[] = {
    {"pwd", builtin_pwd},
    {"cd", builtin_cd},
    {"help", builtin_help},
    {"exit", builtin_exit}
};

static int split(char *line, char **args, int max_args) {
    int count = 0;
    char *tok = strtok(line, " \t");

    while (tok && count < max_args - 1) {
        args[count++] = tok;
        tok = strtok(NULL, " \t");
    }

    args[count] = NULL;
    return count;
}

int main(void) {
    char line[MAX_INPUT];
    char *args[32];

    printf("=== Skill 9: Built-in Dispatch + cd ===\n");

    while (1) {
        printf("builtin> ");

        if (!fgets(line, sizeof(line), stdin))
            break;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0')
            continue;

        int argc = split(line, args, 32);
        int found = 0;

        for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
            if (strcmp(args[0], table[i].name) == 0) {
                table[i].fn(args);
                found = 1;
                break;
            }
        }

        if (!found)
            printf("Unknown built-in: %s\n", args[0]);

        (void)argc;
    }

    return 0;
}

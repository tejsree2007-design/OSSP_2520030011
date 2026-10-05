// OSSP Skill 13 - Input and Output Redirection
// Demonstrates < and >, open(), dup2(), error handling,
// and restoration of standard input/output.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(void) {
    printf("=== Skill 13: Input / Output Redirection ===\n");

    const char *input_file = "skill13_input.txt";
    const char *output_file = "skill13_output.txt";

    FILE *f = fopen(input_file, "w");
    if (!f) {
        perror("fopen");
        return 1;
    }
    fprintf(f, "Hello from the input file.\nSecond line.\n");
    fclose(f);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        int in = open(input_file, O_RDONLY);
        if (in == -1) {
            perror("open input");
            _exit(1);
        }

        int out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out == -1) {
            perror("open output");
            close(in);
            _exit(1);
        }

        dup2(in, STDIN_FILENO);
        dup2(out, STDOUT_FILENO);

        close(in);
        close(out);

        execlp("cat", "cat", NULL);
        perror("execlp");
        _exit(1);
    }

    waitpid(pid, NULL, 0);

    printf("Output was redirected to %s\n", output_file);
    printf("Contents:\n");

    f = fopen(output_file, "r");
    if (!f) {
        perror("fopen output");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), f))
        printf("%s", line);

    fclose(f);
    return 0;
}

// OSSP Skill 8 - Variable Expansion
// Demonstrates: variable references, expansion, undefined variables,
// token update, and simple nested-variable expansion.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 512
#define MAX_OUTPUT 1024

static const char *lookup_var(const char *name) {
    const char *value = getenv(name);
    return value ? value : "";
}

static void expand_once(const char *input, char *output, size_t out_size) {
    size_t i = 0, j = 0;

    while (input[i] && j + 1 < out_size) {
        if (input[i] == '$') {
            char name[128];
            size_t n = 0;

            if (input[i + 1] == '{') {
                i += 2;
                while (input[i] && input[i] != '}' &&
                       n + 1 < sizeof(name)) {
                    name[n++] = input[i++];
                }
                if (input[i] == '}') i++;
            } else {
                i++;
                while (input[i] &&
                       (isalnum((unsigned char)input[i]) || input[i] == '_') &&
                       n + 1 < sizeof(name)) {
                    name[n++] = input[i++];
                }
            }

            name[n] = '\0';

            if (n > 0) {
                const char *value = lookup_var(name);
                while (*value && j + 1 < out_size)
                    output[j++] = *value++;
            } else {
                output[j++] = '$';
            }
        } else {
            output[j++] = input[i++];
        }
    }
    output[j] = '\0';
}

static void expand_recursive(const char *input, char *output, size_t size) {
    char current[MAX_OUTPUT];
    char next[MAX_OUTPUT];

    snprintf(current, sizeof(current), "%s", input);

    for (int pass = 0; pass < 5; pass++) {
        expand_once(current, next, sizeof(next));
        if (strcmp(current, next) == 0) break;
        snprintf(current, sizeof(current), "%s", next);
    }

    snprintf(output, size, "%s", current);
}

int main(void) {
    char input[MAX_INPUT];
    char output[MAX_OUTPUT];

    printf("=== Skill 8: Variable Expansion ===\n");
    printf("Try: echo $HOME $USER ${SHELL}\n");
    printf("Type 'exit' to quit.\n\n");

    while (1) {
        printf("expand> ");
        if (!fgets(input, sizeof(input), stdin))
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
            break;

        expand_recursive(input, output, sizeof(output));
        printf("Expanded: %s\n", output);
    }

    return 0;
}

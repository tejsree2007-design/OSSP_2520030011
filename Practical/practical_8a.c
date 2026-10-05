#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *a, *b;

    a = (int *)malloc(5 * sizeof(int));
    printf("Memory allocated using malloc() for 5 integers.\n");

    b = (int *)calloc(5, sizeof(int));
    printf("Memory allocated using calloc() for 5 integers.\n");

    a = (int *)realloc(a, 10 * sizeof(int));
    printf("Memory reallocated using realloc() for 10 integers.\n");

    free(a);
    printf("Memory allocated to a freed using free().\n");

    free(b);
    printf("Memory allocated to b freed using free().\n");

    printf("Memory allocation demonstration completed successfully.\n");

    return 0;
}

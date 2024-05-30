#include <stdio.h>
#include <stdlib.h>

typedef struct mystruct {
    int zahl1;
    int zahl2;
    int zahl3;
} mystruct;

int main() {
    mystruct test1;
    mystruct *test2 = malloc(sizeof(mystruct));
    int t1 = sizeof(mystruct);
    int t2 = sizeof(test1);
    int t3 = sizeof(test2);
    printf("t1: %d\nt2: %d\nt3: %d\n", t1, t2, t3);
}

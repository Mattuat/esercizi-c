#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int **pptr;
    int *ptr;
    int val;

    val = 10;
    ptr = &val;
    *pptr = ptr;

    printf("val %d\n",val);
    printf("ptr val %d\n",*ptr);
    printf("pptr val %d\n",**pptr);

    return 0;
}
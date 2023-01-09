#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "mymalloc.h"

int main(int argc, char *argv[])
{
    myinit(2);
    
    
    int* testArray;
    testArray = mymalloc(4 * sizeof(long));
    testArray[0] = 1;
    printf("UTILIZATION: %lf", utilization());
    
    long* testArray2;
    printf("FIRST MALLOC CALL\n");
    testArray2 = mymalloc(4 * sizeof(long));
    testArray2[0] = 12;
    testArray2[1] = 22;
    testArray2[2] = 32;
    testArray2[3] = 42;
    printf("\n%ld %ld %ld %ld \n",testArray2[0], testArray2[1], testArray2[2], testArray2[3]);
    fflush(NULL);

    myfree((size_t*)((size_t)(testArray + 1)));
    myfree(testArray + 1000000);
    myfree(testArray + 1);
    myfree(testArray);
    myfree(testArray);

    
    // printf("REALLOC CALL\n");  
    // testArray = myrealloc(testArray, 100 * sizeof(long));
    // printf("%p", testArray2);
    // printf("\n%ld %ld %ld %ld \n",testArray2[0], testArray2[1], testArray2[2], testArray2[3]);

    testArray = mymalloc(3 * sizeof(long));
    // testArray2 = myrealloc(testArray2, 100 * sizeof(long));
    myfree(testArray2);
    testArray2 = mymalloc(1 * sizeof(long));
    
    testArray2 = mymalloc(8 * sizeof(long));
    testArray2 = mymalloc(1 * sizeof(int));
    testArray2 = mymalloc(1 * sizeof(int));
    myfree(testArray2);
    testArray2 = mymalloc(8 * sizeof(int));
    testArray2 = mymalloc(4 * sizeof(long));
    testArray2 = mymalloc(8 * sizeof(int));
    testArray2 = mymalloc(4 * sizeof(long));
    myfree(testArray2);
    testArray2 = mymalloc(4 * sizeof(long));
    testArray2 = mymalloc(8 * sizeof(int));
    testArray2 = mymalloc(4 * sizeof(long));


    for(int i = 0; i<10000; i++){
        printf("%d ", i);
        testArray2 = mymalloc(16 * sizeof(int));
        
    }

    printf("UTILIZATION: %lf", utilization());

    mycleanup();

}
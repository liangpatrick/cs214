#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mymalloc.h"

// 0 means that it is free and 1 means it is occupied

size_t *heap; // 8-byte array of size 1MB

size_t *freeHeadPtr = NULL; //pointer to start of free block linked list
size_t *heapHead;

const size_t SIZE = 1048576;
const size_t BLOCKS = SIZE / 8; //number of blocks(131072)

size_t method;

const size_t BOUNDS = 2;
const size_t MIN_REGION_SIZE = 32;
size_t *ptr;

size_t memStored = 0;
size_t spaceUsed = 0;
// also as far as malloc(stdio) is used in this whole project, as outlined in the pdf, we use malloc to initialize myinit

void myinit(int allocAlg)
{
    memStored = 0;
    spaceUsed = 0;
    method = allocAlg;
    heap = malloc(SIZE); //value is mb in bytes (131072 8-byte blocks)
    //printf("%p\n",heap);
    //one single free region of 131072
    heap[0] = SIZE;          //header
    heap[BLOCKS - 1] = SIZE; //footer
    heap[1] = 0;             //previous block ptr USE 0 for NULL since it will be casted
    heap[2] = 0;             //next block ptr
    freeHeadPtr = &heap[0];  //sets this as free block head
    ptr = freeHeadPtr;
    heapHead = ptr;
    //printf("HEAP TAIL: %p\n", heapHead + BLOCKS - 1);
}

void *mymalloc(size_t size)
{
    if (size == 0)
        return NULL;

    size_t payloadBlocks = (size % 8 == 0) ? (size / 8) : (int)(size / 8) + 1;
    if (payloadBlocks < 2)
    {
        payloadBlocks = 2;
    }

    payloadBlocks = payloadBlocks + 1; //adds one padding to hold size of payload

    size_t ptrSize = 0;
    size_t ptrBlocks = 0;
    size_t freeBlocks = 0;

    size_t *prevPtr = NULL;
    size_t *nextPtr = NULL;

    size_t *returnPtr = NULL;

    int remainingFreeBlocks = 0;

    size_t *freeBlockPtr = NULL;
    switch (method)
    {
        //  first fit
    case 0:;
        ptr = freeHeadPtr;
        //printf("\nSTARTING BLOCK: %p\n",ptr);
        //printf("FREE HEAD PTR: %p",ptr);
        //fflush(NULL);
        while (ptr != NULL &&
               ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) != 0) &&                  //checks if no free region if split occurs
               ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) <= (int)MIN_REGION_SIZE)) //checks if resulting free region if split occurs is big enough to store pointers

        {
            ptr = (size_t *)(*(ptr + 2)); //sets ptr to next ptr (stored in offset of 16) (pointer must be case from size_t to size_t*)
            //printf("\nNext Block: %p\n",ptr);
        }
        //printf("\nCHOSEN NEW FREE BLOCK: %p\n",ptr);

        if (ptr == NULL)
        {
            printf("Unable to allocate new blocks, no suitable free regions found");
            return NULL;
        }

        //printf("\nPTR VALUE: %ld",*ptr);
        ptrSize = (*ptr & -2);      //size in bytes of current region
        ptrBlocks = ptrSize / 8;    //size in blocks
        freeBlocks = ptrBlocks - 2; //free blocks in region excludes header and footer

        memStored = memStored + size;
        spaceUsed = spaceUsed + ((payloadBlocks + 2) * 8);

        prevPtr = (size_t *)*(ptr + 1);
        nextPtr = (size_t *)*(ptr + 2);

        returnPtr = ptr + 1;
        //printf("\nRETURN POINTER: %p\n",(returnPtr));
        //fflush(NULL);

        remainingFreeBlocks = freeBlocks - payloadBlocks - 2; //subtract 2 for new headers and footers and excludes current headers and footers

        //sets up headers and footers for new occupied block
        //printf("PTR: %p", ptr);
        *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1; //adds one to signify full
        //printf("\nSIZE OF PTR: %ld", *ptr );
        ptr = (ptr + (payloadBlocks) + 1);
        *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1;
        *(ptr - 1) = size;
        //printf("\nPTR TAIL: %p", ptr);
        /*
        printf("\nFREE BLOCK PTR HEAD: %p\n", freeHeadPtr);
        printf("Payload Blocks: %ld", payloadBlocks);
        printf("\nFree Blocks: %ld", freeBlocks);
        printf("\nPREV POINTER VALUE: %p", prevPtr);
        printf("\nNEXT POINTER VALUE: %p", nextPtr);
        printf(" SIZE OF SPACE BEING CHECKED: %ld BLOCKS, SIZE OF PAYLOAD: %ld BLOCKS, REMAINING FREE BLOCKS: %d",freeBlocks,payloadBlocks, remainingFreeBlocks);
            
        fflush(NULL);*/

        if (remainingFreeBlocks > 0 || remainingFreeBlocks == -2)
        {
            if (remainingFreeBlocks == -2)
            { //if there are 0 free blocks in the segment after the split
                size_t *prevPtrNext = NULL;
                size_t *nextPtrPrev = NULL;
                if (nextPtr != NULL && prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = (size_t)nextPtr;
                    *nextPtrPrev = (size_t)prevPtr;
                }
                else if (prevPtr != NULL)
                { //if only previous blocks exist in the free list
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = 0;
                }
                else if (nextPtr != NULL)
                { //if only next blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    *nextPtrPrev = 0;
                    freeHeadPtr = nextPtr;
                }
                else
                { //if neither exists
                    freeHeadPtr = NULL;
                }
            }
            else
            {                             //if there are atleast 2 free blocks in the segment after the split
                freeBlockPtr = (ptr + 1); //sets up headers, footers
                //printf("\nFREE BLOCK PTR: %p\n", freeBlockPtr);
                *freeBlockPtr = (remainingFreeBlocks * 8) + (BOUNDS * 8);
                size_t *freeBlockPtrTail;
                freeBlockPtrTail = freeBlockPtr + remainingFreeBlocks + 1;
                //printf("FREE BLOCK PTR TAIL: %p\n", freeBlockPtrTail);
                *freeBlockPtrTail = (remainingFreeBlocks * 8) + (BOUNDS * 8);

                size_t *prevPtrNext = NULL;
                size_t *nextPtrPrev = NULL;

                if (nextPtr != NULL && prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 2) = (size_t)nextPtr;
                    *nextPtrPrev = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 1) = (size_t)prevPtr;
                    //printf("FREE BLOCK PTR ####0: %p\n", freeBlockPtr);
                }
                else if (prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    prevPtrNext = (prevPtr + 2);
                    *prevPtrNext = (size_t)freeBlockPtr;
                    //printf("FREE BLOCK PTR ####1: %p\n", freeBlockPtr);
                    *(freeBlockPtr + 2) = 0;
                    *(freeBlockPtr + 1) = (size_t)prevPtr;
                }
                else if (nextPtr != NULL)
                { //if only next blocks exist in the free list
                    nextPtrPrev = (nextPtr + 1);
                    *nextPtrPrev = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 1) = 0;
                    *(freeBlockPtr + 2) = (size_t)nextPtr;
                    freeHeadPtr = freeBlockPtr;
                    //printf("FREE BLOCK PTR ####2: %p\n", freeBlockPtr);
                }
                else
                { //if neither exists
                    *(freeBlockPtr + 1) = 0;
                    *(freeBlockPtr + 2) = 0;
                    freeHeadPtr = freeBlockPtr;
                    //printf("FREE BLOCK PTR ####3: %p\n", freeBlockPtr);
                }
                ptr = freeBlockPtr;
            }
            return returnPtr;
        }
        else if (remainingFreeBlocks != 0)
        {
            printf("FATAL: ERROR HANDLING NEW FREE BLOCK.");
            fflush(stdout);
            return NULL;
        }
        break;

        //  next fit
    case 1:;
        size_t *startingPtr = ptr;

        if (((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) != 0) &&                  //checks if no free region if split occurs
            ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) <= (int)MIN_REGION_SIZE)) //checks if resulting free region if split occurs is big enough to store pointers
        {
            if (ptr == NULL)
            {
                ptr = freeHeadPtr;
            }
            else
            {
                ptr = (size_t *)(*(ptr + 2)); //sets ptr to next ptr (stored in offset of 16) (pointer must be case from size_t to size_t*)
            }
        }
        while (ptr != startingPtr &&
               ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) != 0) &&                  //checks if no free region if split occurs
               ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) <= (int)MIN_REGION_SIZE)) //checks if resulting free region if split occurs is big enough to store pointers

        {
            if (ptr == NULL)
            {
                ptr = freeHeadPtr;
            }
            else
            {
                ptr = (size_t *)(*(ptr + 2)); //sets ptr to next ptr (stored in offset of 16) (pointer must be case from size_t to size_t*)
            }
        }
        //printf("\nCHOSEN NEW FREE BLOCK: %p\n",ptr);

        if (ptr == NULL)
        {
            printf("Unable to allocate new blocks, no suitable free regions found");
            return NULL;
        }

        //printf("\nPTR VALUE: %ld",*ptr);
        ptrSize = (*ptr & -2);      //size in bytes of current region
        ptrBlocks = ptrSize / 8;    //size in blocks
        freeBlocks = ptrBlocks - 2; //free blocks in region excludes header and footer

        memStored = memStored + size;
        spaceUsed = spaceUsed + ((payloadBlocks + 2) * 8);

        prevPtr = (size_t *)*(ptr + 1);
        nextPtr = (size_t *)*(ptr + 2);

        returnPtr = ptr + 1;
        //printf("\nRETURN POINTER: %p\n",(returnPtr));
        //fflush(NULL);

        remainingFreeBlocks = freeBlocks - payloadBlocks - 2; //subtract 2 for new headers and footers and excludes current headers and footers

        //sets up headers and footers for new occupied block
        //printf("PTR: %p", ptr);
        *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1; //adds one to signify full
        //printf("\nSIZE OF PTR: %ld", *ptr );
        ptr = (ptr + (payloadBlocks) + 1);
        *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1;
        *(ptr - 1) = size;
        //printf("\nPTR TAIL: %p", ptr);

        /*
        printf("\nFREE BLOCK PTR HEAD: %p\n", freeHeadPtr);
        printf("Payload Blocks: %ld", payloadBlocks);
        printf("\nFree Blocks: %ld", freeBlocks);
        printf("\nPREV POINTER VALUE: %p", prevPtr);
        printf("\nNEXT POINTER VALUE: %p", nextPtr);
        printf(" SIZE OF SPACE BEING CHECKED: %ld BLOCKS, SIZE OF PAYLOAD: %ld BLOCKS, REMAINING FREE BLOCKS: %d\n\n",freeBlocks,payloadBlocks, remainingFreeBlocks);
            
        fflush(NULL);
        */
        if (remainingFreeBlocks > 0 || remainingFreeBlocks == -2)
        {
            if (remainingFreeBlocks == -2)
            { //if there are 0 free blocks in the segment after the split
                size_t *prevPtrNext = NULL;
                size_t *nextPtrPrev = NULL;
                if (nextPtr != NULL && prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = (size_t)nextPtr;
                    *nextPtrPrev = (size_t)prevPtr;
                }
                else if (prevPtr != NULL)
                { //if only previous blocks exist in the free list
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = 0;
                }
                else if (nextPtr != NULL)
                { //if only next blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    *nextPtrPrev = 0;
                    freeHeadPtr = nextPtr;
                }
                else
                { //if neither exists
                    freeHeadPtr = NULL;
                }
            }
            else
            {                             //if there are atleast 2 free blocks in the segment after the split
                freeBlockPtr = (ptr + 1); //sets up headers, footers
                //printf("\nFREE BLOCK PTR: %p\n", freeBlockPtr);
                *freeBlockPtr = (remainingFreeBlocks * 8) + (BOUNDS * 8);
                size_t *freeBlockPtrTail;
                freeBlockPtrTail = freeBlockPtr + remainingFreeBlocks + 1;
                //printf("FREE BLOCK PTR TAIL: %p\n", freeBlockPtrTail);
                *freeBlockPtrTail = (remainingFreeBlocks * 8) + (BOUNDS * 8);

                size_t *prevPtrNext = NULL;
                size_t *nextPtrPrev = NULL;

                if (nextPtr != NULL && prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 2) = (size_t)nextPtr;
                    *nextPtrPrev = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 1) = (size_t)prevPtr;
                    //printf("FREE BLOCK PTR ####0: %p\n", freeBlockPtr);
                }
                else if (prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    prevPtrNext = (prevPtr + 2);
                    *prevPtrNext = (size_t)freeBlockPtr;
                    //printf("FREE BLOCK PTR ####1: %p\n", freeBlockPtr);
                    *(freeBlockPtr + 2) = 0;
                    *(freeBlockPtr + 1) = (size_t)prevPtr;
                }
                else if (nextPtr != NULL)
                { //if only next blocks exist in the free list
                    nextPtrPrev = (nextPtr + 1);
                    *nextPtrPrev = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 1) = 0;
                    *(freeBlockPtr + 2) = (size_t)nextPtr;
                    freeHeadPtr = freeBlockPtr;
                    //printf("FREE BLOCK PTR ####2: %p\n", freeBlockPtr);
                }
                else
                { //if neither exists
                    *(freeBlockPtr + 1) = 0;
                    *(freeBlockPtr + 2) = 0;
                    freeHeadPtr = freeBlockPtr;
                    //printf("FREE BLOCK PTR ####3: %p\n", freeBlockPtr);
                }
                ptr = freeBlockPtr;
            }
            return returnPtr;
        }
        else if (remainingFreeBlocks != 0)
        {
            printf("FATAL: ERROR HANDLING NEW FREE BLOCK.");
            fflush(stdout);
            return NULL;
        }
        break;
        // best fit
    case 2:;
        size_t *bestPtr = NULL;
        ptr = freeHeadPtr;
        printf("%p FREE PTR HEAD\n", ptr);
        printf("%ld PAYLOAD SIZE\n", payloadBlocks);
        //printf("\nSTARTING BLOCK: %p\n",ptr);
        //printf("FREE HEAD PTR: %p",ptr);
        //fflush(NULL);
        while (ptr != NULL)
        {
            //printf("BEST PTR BEFORE %p \n", bestPtr);
            if ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) == 0 && (bestPtr == NULL || *bestPtr > *ptr)) //checks if no free region if split occurs
            {
                bestPtr = ptr;
                break;
            }

            //printf("CUR POINTER %p \n", ptr);
            //printf("CUR NEXT %p\n", (size_t *)(*(ptr + 2)));
            //printf("CUR PREV %p\n", (size_t *)(*(ptr + 1)));
            //printf("CUR SIZE %ld\n", *ptr);

            if ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) >= (int)MIN_REGION_SIZE && (bestPtr == NULL || *bestPtr > *ptr)) //checks if resulting free region if split occurs is big enough to store pointers
            {
                bestPtr = ptr;
            }
            printf("BEST PTR AFTER %p \n", bestPtr);

            ptr = (size_t *)(*(ptr + 2)); //sets ptr to next ptr (stored in offset of 16) (pointer must be case from size_t to size_t*)
            //printf("\nNext Block: %p\n",ptr);
        }
        //printf("\nCHOSEN NEW FREE BLOCK: %p\n",ptr);
        ptr = bestPtr;
        printf("\nCHOSEN POINTER %p \n", ptr);
        printf("CHOSEN POINTER PREV %p \n", (size_t *)(*(ptr + 1)));
        if (ptr == NULL)
        {
            printf("Unable to allocate new blocks, no suitable free regions found");
            return NULL;
        }

        //printf("\nPTR VALUE: %ld",*ptr);
        ptrSize = (*ptr & -2);      //size in bytes of current region
        ptrBlocks = ptrSize / 8;    //size in blocks
        freeBlocks = ptrBlocks - 2; //free blocks in region excludes header and footer

        memStored = memStored + size;
        spaceUsed = spaceUsed + ((payloadBlocks + 2) * 8);

        prevPtr = (size_t *)*(ptr + 1);
        nextPtr = (size_t *)*(ptr + 2);

        returnPtr = ptr + 1;
        //printf("\nRETURN POINTER: %p\n",(returnPtr));
        //fflush(NULL);

        remainingFreeBlocks = freeBlocks - payloadBlocks - 2; //subtract 2 for new headers and footers and excludes current headers and footers

        //sets up headers and footers for new occupied block
        //printf("PTR: %p", ptr);
        *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1; //adds one to signify full
        //printf("\nSIZE OF PTR: %ld", *ptr );
        ptr = (ptr + (payloadBlocks) + 1);
        *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1;
        *(ptr - 1) = size;
        //printf("\nPTR TAIL: %p", ptr);
        /*
        printf("\nFREE BLOCK PTR HEAD: %p\n", freeHeadPtr);
        printf("Payload Blocks: %ld", payloadBlocks);
        printf("\nFree Blocks: %ld", freeBlocks);
        printf("\nPREV POINTER VALUE: %p", prevPtr);
        printf("\nNEXT POINTER VALUE: %p", nextPtr);
        printf(" SIZE OF SPACE BEING CHECKED: %ld BLOCKS, SIZE OF PAYLOAD: %ld BLOCKS, REMAINING FREE BLOCKS: %d",freeBlocks,payloadBlocks, remainingFreeBlocks);
            
        fflush(NULL);*/

        if (remainingFreeBlocks > 0 || remainingFreeBlocks == -2)
        {
            if (remainingFreeBlocks == -2)
            { //if there are 0 free blocks in the segment after the split
                size_t *prevPtrNext = NULL;
                size_t *nextPtrPrev = NULL;
                if (nextPtr != NULL && prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = (size_t)nextPtr;
                    *nextPtrPrev = (size_t)prevPtr;
                }
                else if (prevPtr != NULL)
                { //if only previous blocks exist in the free list
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = 0;
                }
                else if (nextPtr != NULL)
                { //if only next blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    *nextPtrPrev = 0;
                    freeHeadPtr = nextPtr;
                }
                else
                { //if neither exists
                    freeHeadPtr = NULL;
                }
            }
            else
            {                             //if there are atleast 2 free blocks in the segment after the split
                freeBlockPtr = (ptr + 1); //sets up headers, footers
                //printf("\nFREE BLOCK PTR: %p\n", freeBlockPtr);
                *freeBlockPtr = (remainingFreeBlocks * 8) + (BOUNDS * 8);
                size_t *freeBlockPtrTail;
                freeBlockPtrTail = freeBlockPtr + remainingFreeBlocks + 1;
                //printf("FREE BLOCK PTR TAIL: %p\n", freeBlockPtrTail);
                *freeBlockPtrTail = (remainingFreeBlocks * 8) + (BOUNDS * 8);

                size_t *prevPtrNext = NULL;
                size_t *nextPtrPrev = NULL;

                if (nextPtr != NULL && prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    nextPtrPrev = nextPtr + 1;
                    prevPtrNext = prevPtr + 2;
                    *prevPtrNext = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 2) = (size_t)nextPtr;
                    *nextPtrPrev = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 1) = (size_t)prevPtr;
                    //printf("FREE BLOCK PTR ####0: %p\n", freeBlockPtr);
                }
                else if (prevPtr != NULL)
                { //if next and previous blocks exist in the free list
                    printf("PREV POINTER: %p", prevPtr);
                    prevPtrNext = (prevPtr + 2);
                    *prevPtrNext = (size_t)freeBlockPtr;
                    //printf("FREE BLOCK PTR ####1: %p\n", freeBlockPtr);
                    *(freeBlockPtr + 2) = 0;
                    *(freeBlockPtr + 1) = (size_t)prevPtr;
                }
                else if (nextPtr != NULL)
                { //if only next blocks exist in the free list
                    nextPtrPrev = (nextPtr + 1);
                    *nextPtrPrev = (size_t)freeBlockPtr;
                    *(freeBlockPtr + 1) = 0;
                    *(freeBlockPtr + 2) = (size_t)nextPtr;
                    freeHeadPtr = freeBlockPtr;
                    //printf("FREE BLOCK PTR ####2: %p\n", freeBlockPtr);
                }
                else
                { //if neither exists
                    *(freeBlockPtr + 1) = 0;
                    *(freeBlockPtr + 2) = 0;
                    freeHeadPtr = freeBlockPtr;
                    //printf("FREE BLOCK PTR ####3: %p\n", freeBlockPtr);
                }
                ptr = freeBlockPtr;
            }
            return returnPtr;
        }
        else if (remainingFreeBlocks != 0)
        {
            printf("FATAL: ERROR HANDLING NEW FREE BLOCK.");
            fflush(stdout);
            return NULL;
        }
        break;
    }

    return (void *)returnPtr; //returns address of the begining of payload
}

void myfree(void *free) //inserts new free at front of free blocks
{
    size_t *testPtr = freeHeadPtr;
    while (testPtr != NULL)
    {
        printf("TEST PTR BEFORE MYFREE: %p", testPtr);
        printf(" SIZE: %ld", *testPtr);
        printf(" NEXT: %p", (size_t *)*(testPtr + 2));
        printf(" PREV: %p\n", (size_t *)*(testPtr + 1));
        testPtr = (size_t *)(*(testPtr + 2));
    }

    //printf("FREE CALLED!\n");
    if (free == NULL)
    {
        return;
    }
    size_t *freeRegionHead;
    freeRegionHead = ((size_t *)free) - 1;
    printf("\nPOINTER BEING FREED: %p\n", freeRegionHead);
    printf("POINTER BEING FREED SIZE: %ld\n", (*freeRegionHead & -2));

    if (freeRegionHead == NULL || (size_t)freeRegionHead < (size_t)heapHead || (size_t)freeRegionHead > (size_t)(heapHead + BLOCKS))
    {
        printf("error: not a heap pointer\n");
        return;
    }

    size_t regionSize = (*freeRegionHead & -2);
    size_t numBlocks = regionSize / 8;
    size_t newNumBlocks = numBlocks;
    size_t *freeRegionTail;
    freeRegionTail = freeRegionHead + numBlocks - 1;

    if ((((size_t)freeRegionHead % 8) != 0) || ((((*freeRegionHead & 1) / 8) + freeRegionHead) > (heapHead + BLOCKS - 1)) || *freeRegionTail != *freeRegionHead || freeRegionHead == freeRegionTail || *freeRegionHead == 0)
    {
        printf("error: not a malloced addres\n");
        return;
    }

    if ((*freeRegionHead & 1) == 0)
    {
        printf("error: double free\n");
        return;
    }
    *freeRegionHead = regionSize;

    size_t *nextRegionHead;
    size_t *nextRegionTail;
    size_t *prevRegionHead;
    size_t *prevRegionTail;

    memStored = memStored - *(freeRegionTail - 1);
    spaceUsed = spaceUsed - (regionSize);

    size_t prevRegionSize;
    prevRegionTail = freeRegionHead - 1;
    if (prevRegionTail > heapHead)
    {
        prevRegionSize = *(prevRegionTail) & -2;
    }
    else
    {
        prevRegionSize = 0;
    }
    size_t prevNumBlocks = prevRegionSize / 8;
    prevRegionHead = freeRegionHead - prevNumBlocks;

    size_t nextRegionSize;
    nextRegionHead = freeRegionHead + numBlocks;
    if (nextRegionHead > heapHead)
    {
        nextRegionSize = *(nextRegionHead) & -2;
    }
    else
    {
        nextRegionSize = 0;
    }
    size_t nextNumBlocks = nextRegionSize / 8;
    nextRegionTail = nextRegionHead + nextNumBlocks - 1;

    //printf("PREV REGION HEAD: %p\n", prevRegionHead);
    //printf("NEXT REGION HEAD: %p\n", nextRegionHead);

    if ((nextRegionSize == 0 || (((*nextRegionHead) & 1) == 1)) && (prevRegionSize == 0 || (((*prevRegionHead) & 1) == 1)))
    { //both next and previous regions are full or out of bounds
        *freeRegionHead = regionSize;
        printf("CASE: 1!!!\n");
        *(freeRegionHead + 1) = 0;
        *(freeRegionHead + 2) = (size_t)freeHeadPtr;
        *freeRegionTail = regionSize;
        *(freeHeadPtr + 1) = (size_t)freeRegionHead;
        freeHeadPtr = freeRegionHead;
    }
    else if ((nextRegionSize == 0 || (((*nextRegionHead) & 1) == 1)))
    { //next region is full or out of bounds

        printf("CASE: 2!!!\n"); /*
        printf("Free Region HEAD: %p\n", freeRegionHead);
        printf("NEXT REGION HEAD VAL: %ld\n", (*nextRegionHead));
        printf("PREV REGION HEAD: %ld\n", (*prevRegionHead));*/
        size_t *prevOfPrevRegion = (size_t *)*(prevRegionHead + 1);
        size_t *nextOfPrevRegion = (size_t *)*(prevRegionHead + 2);
        if (nextOfPrevRegion != NULL && nextOfPrevRegion == nextRegionHead)
        {
            nextOfPrevRegion = (size_t *)*(nextOfPrevRegion + 2);
        }
        if (prevOfPrevRegion != NULL && prevOfPrevRegion == nextRegionHead)
        {
            prevOfPrevRegion = (size_t *)*(prevOfPrevRegion + 1);
        }
        if (prevOfPrevRegion != NULL)
        {
            *(prevOfPrevRegion + 2) = (size_t)nextOfPrevRegion; //sets previous pointer of previous region to next pointer of previous region
            if (nextOfPrevRegion != NULL)
            {
                *(nextOfPrevRegion + 1) = (size_t)prevOfPrevRegion; //sets next pointer of previous region to previous pointer of previous region
            }
        }
        else
        {
            if (nextOfPrevRegion != NULL)
            {
                *(nextOfPrevRegion + 1) = (size_t)freeHeadPtr;
            }
            //printf("nextOFPREVREGION: %p\n", nextOfPrevRegion);
            freeHeadPtr = nextOfPrevRegion;
        }

        newNumBlocks = prevNumBlocks + newNumBlocks; //sets up bounds of new free block
        *freeRegionTail = newNumBlocks * 8;
        freeRegionHead = prevRegionHead;
        *(freeRegionHead + 1) = 0;
        *(freeRegionHead + 2) = (size_t)freeHeadPtr;
        *freeRegionHead = newNumBlocks * 8;
        freeHeadPtr = freeRegionHead;
    }
    else if ((prevRegionSize == 0 || (((*prevRegionHead) & 1) == 1)))
    { //previous region is full or out of bounds
        printf("CASE: 3!!!\n");
        size_t *prevOfNextRegion = (size_t *)*(nextRegionHead + 1);
        size_t *nextOfNextRegion = (size_t *)*(nextRegionHead + 2);
        if (nextOfNextRegion != NULL && nextOfNextRegion == prevRegionHead)
        {
            nextOfNextRegion = (size_t *)*(nextOfNextRegion + 2);
        }
        if (prevOfNextRegion != NULL && prevOfNextRegion == nextRegionHead)
        {
            prevOfNextRegion = (size_t *)*(prevOfNextRegion + 1);
        }
        if (prevOfNextRegion != NULL)
        {
            *(prevOfNextRegion + 2) = (size_t)nextOfNextRegion; //sets previous pointer of next region to next pointer of next region
            if (nextOfNextRegion != NULL)
            {
                *(nextOfNextRegion + 1) = (size_t)prevOfNextRegion; //sets next pointer of next region to previous pointer of next region
            }
        }
        else
        {
            freeHeadPtr = nextOfNextRegion;
            if (nextOfNextRegion != NULL)
            {
                *(nextOfNextRegion + 1) = (size_t)freeHeadPtr;
            }
        }

        newNumBlocks = nextNumBlocks + newNumBlocks; //sets up bounds of new free block
        *nextRegionTail = newNumBlocks * 8;
        *(freeRegionHead + 1) = 0;
        *(freeRegionHead + 2) = (size_t)freeHeadPtr;
        *freeRegionHead = (size_t)(newNumBlocks * 8);
        freeHeadPtr = freeRegionHead;
    }
    else
    { //neither next and previous regions are full or out of bounds
        printf("CASE: 4!!!\n");
        size_t *prevOfPrevRegion = (size_t *)*(prevRegionHead + 1);
        size_t *nextOfPrevRegion = (size_t *)*(prevRegionHead + 2);
        if (nextOfPrevRegion != NULL && nextOfPrevRegion == nextRegionHead)
        {
            nextOfPrevRegion = (size_t *)*(nextOfPrevRegion + 2);
        }
        if (prevOfPrevRegion != NULL && prevOfPrevRegion == nextRegionHead)
        {
            prevOfPrevRegion = (size_t *)*(prevOfPrevRegion + 1);
        }
        if (prevOfPrevRegion != NULL)
        {
            *(prevOfPrevRegion + 2) = (size_t)nextOfPrevRegion; //sets previous pointer of previous region to next pointer of previous region
            if (nextOfPrevRegion != NULL)
            {
                *(nextOfPrevRegion + 1) = (size_t)prevOfPrevRegion; //sets next pointer of previous region to previous pointer of previous region
            }
        }
        else
        {
            if (nextOfPrevRegion != NULL)
            {
                *(nextOfPrevRegion + 1) = (size_t)freeHeadPtr;
            }
            freeHeadPtr = nextOfPrevRegion;
        }

        size_t *prevOfNextRegion = (size_t *)*(nextRegionHead + 1);
        size_t *nextOfNextRegion = (size_t *)*(nextRegionHead + 2);
        if (nextOfNextRegion != NULL && nextOfNextRegion == prevRegionHead)
        {
            nextOfNextRegion = (size_t *)*(nextOfNextRegion + 2);
        }
        if (prevOfNextRegion != NULL && prevOfNextRegion == nextRegionHead)
        {
            prevOfNextRegion = (size_t *)*(prevOfNextRegion + 1);
        }

        if (prevOfNextRegion != NULL)
        {
            *(prevOfNextRegion + 2) = (size_t)nextOfNextRegion; //sets previous pointer of next region to next pointer of next region
            if (nextOfNextRegion != NULL)
            {
                *(nextOfNextRegion + 1) = (size_t)prevOfNextRegion; //sets next pointer of next region to previous pointer of next region
            }
        }
        else
        {
            freeHeadPtr = nextOfNextRegion;
            if (nextOfNextRegion != NULL)
            {
                *(nextOfNextRegion + 1) = (size_t)freeHeadPtr;
            }
        }

        newNumBlocks = prevNumBlocks + newNumBlocks; //sets up bounds of new free block
        newNumBlocks = nextNumBlocks + newNumBlocks;
        freeRegionHead = prevRegionHead;
        *nextRegionTail = newNumBlocks * 8;
        *(freeRegionHead + 1) = 0;
        *(freeRegionHead + 2) = (size_t)freeHeadPtr;
        *freeRegionHead = newNumBlocks * 8;
        freeHeadPtr = freeRegionHead;
    }
    //printf("FREED BLOCK POINTER %p\n", freeRegionHead);
    //printf("FREED BLOCK POINTER SIZE %ld\n", *freeRegionHead);

    testPtr = freeHeadPtr;
    while (testPtr != NULL)
    {
        printf("TEST PTR AFTER MYFREE: %p", testPtr);
        printf(" SIZE: %ld", *testPtr);
        printf(" NEXT: %p", (size_t *)*(testPtr + 2));
        printf(" PREV: %p\n", (size_t *)*(testPtr + 1));
        testPtr = (size_t *)(*(testPtr + 2));
    }
}

//modded for realloc to keep in place
void *moddedmalloc(size_t size, size_t *pointer, size_t *prevOfNextRegion, size_t *nextOfNextRegion, size_t *nextRegionHead)
{
    if (size == 0)
        return NULL;

    size_t payloadBlocks = (size % 8 == 0) ? (size / 8) : (int)(size / 8) + 1;
    if (payloadBlocks < 2)
    {
        payloadBlocks = 2;
    }

    payloadBlocks = payloadBlocks + 1; //adds one padding to hold size of payload

    size_t ptrSize = 0;
    size_t ptrBlocks = 0;
    size_t freeBlocks = 0;

    size_t *prevPtr = NULL;
    size_t *nextPtr = NULL;

    size_t *returnPtr = NULL;

    int remainingFreeBlocks = 0;

    size_t *freeBlockPtr = NULL;

    ptr = pointer;

    if (((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) != 0) &&                 //checks if no free region if split occurs
        ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) < (int)MIN_REGION_SIZE)) //checks if resulting free region if split occurs is big enough to store pointers
    {
        return NULL; //sets ptr to next ptr (stored in offset of 16) (pointer must be case from size_t to size_t*)
        printf("NULL!!!!!!1??????");
    }
    printf("MODDED FREE BLOCK: %p\n", ptr);
    if (ptr == NULL)
    {
        printf("NULL!!!!!!2??????");
        return NULL;
    }

    //printf("\nPTR VALUE: %ld",*ptr);
    ptrSize = (*ptr & -2);      //size in bytes of current region
    ptrBlocks = ptrSize / 8;    //size in blocks
    freeBlocks = ptrBlocks - 2; //free blocks in region excludes header and footer

    memStored = memStored + size;
    spaceUsed = spaceUsed + ((payloadBlocks + 2) * 8);

    prevPtr = prevOfNextRegion; //does not work since blocks are full $$$$
    nextPtr = nextOfNextRegion;

    returnPtr = ptr + 1;
    //printf("\nRETURN POINTER: %p\n",(returnPtr));
    //fflush(NULL);

    remainingFreeBlocks = freeBlocks - payloadBlocks - 2; //subtract 2 for new headers and footers and excludes current headers and footers
    printf("REALLOC REMAINING FREE BLOCKS %d", remainingFreeBlocks);
    //sets up headers and footers for new occupied block
    //printf("PTR: %p", ptr);
    *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1; //adds one to signify full
    //printf("\nSIZE OF PTR: %ld", *ptr );
    ptr = (ptr + (payloadBlocks) + 1);
    *ptr = (payloadBlocks * 8) + (BOUNDS * 8) + 1;
    *(ptr - 1) = size;
    //printf("\nMODDED PTR TAIL: %p", ptr);

    if (remainingFreeBlocks > 0 || remainingFreeBlocks == -2)
    {
        if (remainingFreeBlocks == -2)
        { //if there are 0 free blocks in the segment after the split
            size_t *prevPtrNext = NULL;
            size_t *nextPtrPrev = NULL;

            if (nextPtr != NULL && prevPtr != NULL)
            { //if next and previous blocks exist in the free list
                nextPtrPrev = nextPtr + 1;
                prevPtrNext = prevPtr + 2;
                *prevPtrNext = (size_t)nextPtr;
                *nextPtrPrev = (size_t)prevPtr;
            }
            else if (prevPtr != NULL)
            { //if only previous blocks exist in the free list
                prevPtrNext = prevPtr + 2;
                *prevPtrNext = 0;
            }
            else if (nextPtr != NULL)
            { //if only next blocks exist in the free list
                nextPtrPrev = nextPtr + 1;
                *nextPtrPrev = 0;
                freeHeadPtr = nextPtr;
            }
            else
            {
                if (nextRegionHead != NULL)
                {
                    freeHeadPtr = NULL;
                }
            }
        }
        else
        {                             //if there are atleast 2 free blocks in the segment after the split
            freeBlockPtr = (ptr + 1); //sets up headers, footers
            //printf("\nFREE BLOCK PTR: %p\n", freeBlockPtr);
            *freeBlockPtr = (remainingFreeBlocks * 8) + (BOUNDS * 8);
            size_t *freeBlockPtrTail;
            freeBlockPtrTail = freeBlockPtr + remainingFreeBlocks + 1;
            //printf("FREE BLOCK PTR TAIL: %p\n", freeBlockPtrTail);
            *freeBlockPtrTail = (remainingFreeBlocks * 8) + (BOUNDS * 8);

            size_t *prevPtrNext = NULL;
            size_t *nextPtrPrev = NULL;

            if (nextPtr != NULL && prevPtr != NULL)
            { //if next and previous blocks exist in the free list
                nextPtrPrev = nextPtr + 1;
                prevPtrNext = prevPtr + 2;
                *prevPtrNext = (size_t)freeBlockPtr;
                *(freeBlockPtr + 2) = (size_t)nextPtr;
                *nextPtrPrev = (size_t)freeBlockPtr;
                *(freeBlockPtr + 1) = (size_t)prevPtr;
                //printf("FREE BLOCK PTR ####0: %p\n", freeBlockPtr);
            }
            else if (prevPtr != NULL)
            { //if next and previous blocks exist in the free list
                prevPtrNext = (prevPtr + 2);
                *prevPtrNext = (size_t)freeBlockPtr;
                //printf("FREE BLOCK PTR ####1: %p\n", freeBlockPtr);
                *(freeBlockPtr + 2) = 0;
                *(freeBlockPtr + 1) = (size_t)prevPtr;
            }
            else if (nextPtr != NULL)
            { //if only next blocks exist in the free list
                nextPtrPrev = (nextPtr + 1);
                *nextPtrPrev = (size_t)freeBlockPtr;
                *(freeBlockPtr + 1) = 0;
                *(freeBlockPtr + 2) = (size_t)nextPtr;
                freeHeadPtr = freeBlockPtr;
                //printf("FREE BLOCK PTR ####2: %p\n", freeBlockPtr);
            }
            else
            { //if neither exists
                freeHeadPtr = freeBlockPtr;
                *(freeBlockPtr + 1) = 0;
                if (nextRegionHead == NULL)
                {
                    *(freeBlockPtr + 2) = (size_t)freeBlockPtr;
                }
                else
                {
                    *(freeBlockPtr + 2) = 0;
                }
                //printf("FREE BLOCK PTR ####3: %p\n", freeBlockPtr);
            }
            ptr = freeBlockPtr;
        }
        return returnPtr;
    }
    else if (remainingFreeBlocks != 0)
    {
        printf("FATAL: ERROR HANDLING NEW FREE BLOCK.");
        fflush(stdout);
        return NULL;
    }
    return NULL;
}

void *myrealloc(void *ptrl, size_t size)
{
    printf("FREE HEAD POINTER: %p\n", freeHeadPtr);
    size_t *testPtr = freeHeadPtr;
    while (testPtr != NULL)
    {
        printf("TEST PTR BEFORE REALLOC: %p\n", testPtr);
        testPtr = (size_t *)(*(testPtr + 2));
    }

    if (ptrl == NULL && size == 0)
    {
        return NULL;
    }
    else if (ptrl == NULL)
    {
        return mymalloc(size);
    }
    else if (size == 0)
    {
        myfree(ptrl);
        return NULL;
    }

    size_t *curRegionHead = (size_t *)ptrl - 1;
    size_t curRegionSize = (*curRegionHead & -2);
    size_t curRegionBlocks = curRegionSize / 8;
    size_t *curRegionTail = curRegionHead + curRegionBlocks - 1;
    size_t curRegionPayloadSize = *(curRegionTail - 1);

    size_t newNumBlocks = curRegionBlocks;

    size_t *nextRegionHead;
    size_t *nextRegionTail;
    size_t nextRegionSize;
    nextRegionHead = curRegionHead + curRegionBlocks;
    if (nextRegionHead > heapHead && nextRegionHead < heapHead + BLOCKS - 1)
    {
        nextRegionSize = *(nextRegionHead) & -2;
    }
    else
    {
        nextRegionSize = 0;
    }
    size_t nextNumBlocks = nextRegionSize / 8;
    nextRegionTail = nextRegionHead + nextNumBlocks - 1;

    size_t *prevOfNextRegion = NULL;
    size_t *nextOfNextRegion = NULL;
    size_t *newRegion = NULL;

    if ((nextRegionHead != NULL && (((*nextRegionHead) & 1) != 1)))
    { //attempt to merge with next region
        printf("MERGED!");
        prevOfNextRegion = (size_t *)*(nextRegionHead + 1);
        nextOfNextRegion = (size_t *)*(nextRegionHead + 2);

        if (freeHeadPtr == nextRegionHead)
        {
            freeHeadPtr = nextOfNextRegion;
        }

        newNumBlocks = nextNumBlocks + newNumBlocks; //sets up bounds of new free block
        *nextRegionTail = (newNumBlocks * 8) + 1;
        *(nextRegionTail - 1) = curRegionPayloadSize;
        curRegionTail = nextRegionTail;
        *curRegionHead = (size_t)((newNumBlocks * 8) + 1);
        //newRegion = moddedmalloc(size,(size_t*)ptr - 1, prevOfNextRegion , nextOfNextRegion);   //attempts to keep in place
    }
    else
    {
        nextRegionHead = NULL;
    }

    newRegion = moddedmalloc(size, (size_t *)ptrl - 1, prevOfNextRegion, nextOfNextRegion, nextRegionHead); //attempts to keep in place

    printf("\nREALLOC FREE HEAD PTR: %p\n", freeHeadPtr);
    if (newRegion == NULL)
    {
        newRegion = mymalloc(size);
        printf("NOT IN PLACE!");

        if (newRegion != NULL)
        {
            size_t cpySize;
            if (size < curRegionPayloadSize)
            {
                cpySize = size;
            }
            else
            {
                cpySize = curRegionPayloadSize;
            }
            memcpy(newRegion, ptrl, cpySize);

            if (prevOfNextRegion != NULL && prevOfNextRegion == nextRegionHead)
            {
                prevOfNextRegion = (size_t *)*(prevOfNextRegion + 1);
            }

            if (prevOfNextRegion != NULL && nextOfNextRegion != NULL)
            {
                *(prevOfNextRegion + 2) = (size_t)nextOfNextRegion;
                *(nextOfNextRegion + 1) = (size_t)prevOfNextRegion;
            }
            else if (prevOfNextRegion != NULL)
            {
                *(prevOfNextRegion + 2) = 0;
            }
            else if (nextOfNextRegion != NULL)
            {
                *(nextOfNextRegion + 1) = 0;
                freeHeadPtr = nextOfNextRegion;
            }
            /*
        testPtr = freeHeadPtr;
        while(testPtr != NULL){
        printf("TEST PTR AFTER REALLOC BEFORE FREE: %p\n", testPtr);
        testPtr = (size_t *)(*(testPtr + 2));}
        */

            printf("PTRL SIZE!!!!!!!!!! %ld", *((size_t *)ptrl - 1));
            printf("PTRL - 1 SIZE!!!!!!!!!! %p", ((size_t *)ptrl - 1));
            myfree(ptrl);
            printf("PTRL SIZE AFTER FREE!!!!!!!!!! %ld\n", *((size_t *)ptrl - 1));

            size_t *testPtr = freeHeadPtr;
            while (testPtr != NULL)
            {
                printf("TEST PTR AFTER FREE REALLOC: %p", testPtr);
                printf(" SIZE: %ld", *testPtr);
                printf(" NEXT: %p", (size_t *)*(testPtr + 2));
                printf(" PREV: %p\n", (size_t *)*(testPtr + 1));
                testPtr = (size_t *)(*(testPtr + 2));
            }

            return (void *)newRegion;
        }
        else
        {
            printf("REALLOC FAILED!");
            return (void *)ptrl;
        }
    }
    else
    {
        //printf("MALLOCED IN PLACE!");
        return newRegion;
    }
}

void mycleanup()
{
    free(heapHead);
}

double utilization()
{
    return (double)((double)memStored / (double)spaceUsed);
}
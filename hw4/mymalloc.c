#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "mymalloc.h"

#define WSIZE 4             /* Word and header/footer size (bytes) */
#define DSIZE 8             /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Extend heap by this amount (bytes) */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

/* Private global variables */
static char *mem_heap;     /* Points to first byte of heap */
static char *mem_brk;      /* Points to last byte of heap plus 1 */
static char *mem_max_addr; /* Max legal heap addr plus 1*/

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
/*
 * mem_init - Initialize the memory system model
 */
void mem_init(void)
{
    mem_heap = (char *)malloc(SIZE);
    mem_brk = (char *)mem_heap;
    mem_max_addr = (char *)(mem_heap + SIZE);
}

void meminit(int allocAlg)
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

/*
 * mem_sbrk - Simple model of the sbrk function. Extends the heap
 * by incr bytes and returns the start address of the new area. In
 * this model, the heap cannot be shrunk.
 */
void *mem_sbrk(int incr)
{
    char *old_brk = mem_brk;

    if ((incr < 0) || ((mem_brk + incr) > mem_max_addr))
    {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
        return (void *)-1;
        mem_brk += incr;
        return (void *)old_brk;
    }
}
void myinit(int allocAlg)
{
    /* Create the initial empty heap */
    // if ((heap = mem_sbrk(4 * WSIZE)) == (void *)-1)
    //     return -1;
    PUT(heap, 0);                            /* Alignment padding */
    PUT(heap + (1 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap + (2 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap + (3 * WSIZE), PACK(0, 1));     /* Epilogue header */
    heap += (2 * WSIZE);
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
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    // if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
    //     return -1;
    // return 0;
}

void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    { /* Case 1 */
        return bp;
    }

    else if (prev_alloc && !next_alloc)
    { /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc)
    { /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    else
    { /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}
void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}
void myfree(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

char *find_fit(size_t asize)
{
    size_t payloadBlocks = (asize % 8 == 0) ? (asize / 8) : (int)(asize / 8) + 1;
    if (payloadBlocks < 2)
    {
        payloadBlocks = 2;
    }
    size_t* prePtr = NULL;
    while (ptr != NULL && ptr >= heapHead && ptr < heapHead + BLOCKS &&
           ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) != 0) &&                 //checks if no free region if split occurs
           ((int)(*ptr & -2) - (((int)payloadBlocks * 8) + ((int)BOUNDS * 8)) < (int)MIN_REGION_SIZE)) //checks if resulting free region if split occurs is big enough to store pointers

    {
        prePtr = ptr;
        ptr = (size_t *)(*(ptr + 2)); //sets ptr to next ptr (stored in offset of 16) (pointer must be case from size_t to size_t*)
        if (ptr == prePtr)
        {
            ptr = NULL;
            break;
        }
        //printf("\nNext Block: %p\n",ptr);
    }
    //printf("\nCHOSEN NEW FREE BLOCK: %p\n",ptr);

    if (ptr == NULL || !(ptr >= heapHead && ptr < heapHead + BLOCKS))
    {
        //printf("Unable to allocate new blocks, no suitable free regions found");
        return NULL;
    }
}

void *mymalloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

#include <stdio.h>
#include <stdlib.h>
#include "dumalloc.h"
#define HEAP_SIZE (128*8)

typedef struct memoryBlockHeader {
     int size; // size of the reserved block
     struct memoryBlockHeader* next;  // the next block in the integrated free list
} memoryBlockHeader;

memoryBlockHeader* freeListHead;
unsigned char heap[HEAP_SIZE];  // The memory pool

void duInitMalloc() {
    // Initialize the memory allocator
    for (int i = 0; i < HEAP_SIZE; i++) {
        heap[i] = 0;
    }
    //Stamp header into first 16 bytes
    freeListHead = (memoryBlockHeader*)heap; // Point to start of heap
    freeListHead->size = HEAP_SIZE - sizeof(memoryBlockHeader);
    freeListHead->next = NULL;
}

void* duMalloc(int size) {
    //if size not % 8, round up to nearest % 8
    if (size % 8 != 0) {
        size += 8 - (size % 8);
    }
    
    memoryBlockHeader* current = freeListHead;
    memoryBlockHeader* previous = NULL;
    int totalSize = size + sizeof(memoryBlockHeader);
    
    // walk down the list to find a suitable block
    while (current != NULL && current->size < totalSize) {
        previous = current;
        current = current->next;
    }
    
    // if no suitable block found
    if (current == NULL) {
        return NULL;
    }
    
    // found a suitable block now handle allocation
    if (current->size >= totalSize + sizeof(memoryBlockHeader) + 8) { 
        // split the block
        memoryBlockHeader* newFreeBlock = (memoryBlockHeader*)((unsigned char*)current + totalSize);
        newFreeBlock->size = current->size - totalSize;
        newFreeBlock->next = current->next;
        if (previous == NULL) {
            freeListHead = newFreeBlock;
        } else {
            previous->next = newFreeBlock;
        }
    } else {
        // use the entire block
        if (previous == NULL) {
            freeListHead = current->next;
        } else {
            previous->next = current->next;
        }
        totalSize = current->size + sizeof(memoryBlockHeader); // adjust size to full block size
    }
    
    current->size = size;
    return (void*)((unsigned char*)current + sizeof(memoryBlockHeader));
}

void duFree(void* ptr) {
    if (ptr == NULL) {
        return; // I dont think we need this but maybe for later
    }
    // calculate the start of the memory block header by subtracting header size from the user pointer
    memoryBlockHeader* blockToFree = (memoryBlockHeader*)((unsigned char*)ptr - sizeof(memoryBlockHeader));

    // handle at the beginning of the free list
    if (freeListHead == NULL || blockToFree < freeListHead) {
        // insert at the head of the free list
        blockToFree->next = freeListHead; // link to the previous head
        freeListHead = blockToFree;
        return;
    }

    // traverse the free list until we find where this block should be inserted
    memoryBlockHeader* current = freeListHead;
    while (current->next != NULL && current->next < blockToFree) {
        current = current->next;
    }
    // insert the block into the free list
    blockToFree->next = current->next;
    current->next = blockToFree;
}

void duMemoryDump() {
    printf("Memory Dump:\n");
    memoryBlockHeader* current = freeListHead;
    while (current != NULL) {
        int offset = (unsigned char*)current - heap;
        printf("Block at %p (offset %d): size %d\n", (void*)current, offset, current->size);
        current = current->next;
    }
}

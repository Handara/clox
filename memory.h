#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"
#include <stdio.h>

#define MIN_RANK 6ull
#define MAX_RANK 32ull
#define MIN_SIZE (1ull<<MIN_RANK)
#define MAX_SIZE (1ull<<MAX_RANK)
#define RANK_COUNT (MAX_RANK - MIN_RANK)

typedef struct buddyBlock buddyBlock;

struct buddyBlock{
    size_t rank;
    buddyBlock* next;
    buddyBlock* prev;
    bool isFree;
};

typedef struct{
    uint8_t* start;
    size_t maxRank;
    buddyBlock* freelist[RANK_COUNT];
}buddyAllocator;

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);
void initBuddyAllocator(size_t size);
void addToFreeList(buddyBlock* block, size_t rank);
void removeFromFreeList(buddyBlock* block);
buddyBlock* getBuddy(buddyBlock* block);

#endif

#include <stdlib.h>
#include <string.h>
#include "memory.h"

static buddyAllocator* allocator = NULL;

size_t nextPowerOfTwo(size_t input){
    size_t result = input - 1;
    
    result |= result >> 1;
    result |= result >> 2;
    result |= result >> 4;
    result |= result >> 8;
    result |= result >> 16;
    result |= result >> 32;
     
    return ++result;
}

size_t log2Floor(size_t n){
    return 63 - __builtin_clzll(n);
}

size_t sizeToRank(size_t size){
    return log2Floor(nextPowerOfTwo(size)) - 6;
}

void initBuddyAllocator(size_t size){
    size_t first_block_size = nextPowerOfTwo(size);
    allocator = malloc(sizeof(buddyAllocator) + first_block_size);
    allocator->start = ((uint8_t*) allocator) + sizeof(buddyAllocator);
    allocator->maxRank = sizeToRank(first_block_size);
    for (size_t i = 0; i < RANK_COUNT; i++){
        allocator->freelist[i] = NULL;
    }
    addToFreeList((buddyBlock*)allocator->start,sizeToRank(size));
}

void addToFreeList(buddyBlock* block, size_t rank){
    block->rank= rank;
    block->prev = NULL;
    block->isFree = true;
    block->next = allocator->freelist[rank]; 
    if (block->next){
        block->next->prev = block;
    }
    allocator->freelist[rank] = block;
}

void removeFromFreeList(buddyBlock* block){ 
    if (block->next){
        block->next->prev = block->prev;
    }
    if (block->prev){
        block->prev->next = block->next;
    }else{
        allocator->freelist[block->rank] = block->next;
    }
    block->next = NULL;
    block->prev = NULL;
    block->isFree = false;
}

buddyBlock* getBuddy(buddyBlock* block){ 
    size_t offset = (uint8_t*)block - allocator->start; 
    offset =  offset^(1<<(block->rank+6)); 
    return (buddyBlock*)(allocator->start + offset);
}


void* allocate(size_t size){
    size_t actual_size = size + sizeof(buddyBlock);
    
    if (size == 0 || actual_size > MAX_SIZE) return NULL;
    if (actual_size < MIN_SIZE) actual_size = MIN_SIZE;

    size_t rank = sizeToRank(actual_size);
    buddyBlock* block = NULL; 
    
    for (size_t i = rank; i < RANK_COUNT; i++){
        if (allocator->freelist[i]){
            block = allocator->freelist[i];
            removeFromFreeList(block);
            break;
        }
    }
    if (block == NULL)return NULL;
    while(block->rank > rank){
        block->rank--;
        buddyBlock* buddy = getBuddy(block);
        addToFreeList(buddy, block->rank);
    }   
    return (uint8_t*)block + sizeof(buddyBlock);
}

void freelocate(void* pointer){
    buddyBlock* block = (buddyBlock*)((uint8_t*)pointer - sizeof(buddyBlock));      
    
    while(block->rank < allocator->maxRank){
        buddyBlock* buddy = getBuddy(block);
        if(!buddy->isFree || buddy->rank != block->rank){
            break;
        }
        removeFromFreeList(buddy);
        if(buddy < block){
            block = buddy;
        }
        block->rank++;
    }
    addToFreeList(block, block->rank);
}

void* reallocate(void* pointer, size_t oldSize, size_t newSize){
    if (newSize == 0){
        //free(pointer);
        freelocate(pointer);
        return NULL;
    }
    void* result = allocate(newSize);
    if(pointer){
        memcpy(result,pointer,oldSize);
        freelocate(pointer);
    }
        
    if (result == NULL) exit(1);
    return result;
}

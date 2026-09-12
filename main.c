#include <stdio.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "memory.h"


int main(/* int argc, const char* argv[] */){
    initBuddyAllocator(100000);
    Chunk chunk;
    initChunk(&chunk);

    for (int i = 0; i < 300; i++){
        writeConstant(&chunk,i,i);
    }
    freeChunk(&chunk);
    return 0;
}

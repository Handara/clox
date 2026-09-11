#include <stdio.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "memory.h"


int main(/*int argc, const char* argv[]*/){
    initBuddyAllocator(1000000);
    Chunk chunk;
    initChunk(&chunk);

    for (int i = 0; i < 300; i++){
        writeConstant(&chunk,i,i);
    }
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}

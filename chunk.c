#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk){
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initValueArray(&chunk->constants);
    chunk->lines = NULL;
    chunk->lineCount = 0;
    chunk->lineCapacity = 0;
}

void freeChunk(Chunk* chunk){
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->lineCapacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line){
    if (chunk->capacity < chunk->count + 1){
        int oldCapacity =  chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    if(chunk->lineCapacity < chunk->lineCount +1){
        int oldLineCapacity = chunk->lineCapacity;
        chunk->lineCapacity = GROW_CAPACITY(oldLineCapacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldLineCapacity, chunk->lineCapacity);
    }

    chunk->code[chunk->count] = byte;

    if (line < chunk->lineCount){
        chunk->lines[line]++;
    }else{
        chunk->lines[line] = 1;
        chunk->lineCount = line + 1;
    }

    chunk->count++;
}

int getLine(Chunk* chunk, int instructionIndex){
    int accumulator = 0;

    for (int i = 0; i < chunk->lineCount; i++){
        accumulator += chunk->lines[i];
        if (accumulator > instructionIndex){
            return i;
        }
    }
    return -1;
}

int addConstant(Chunk* chunk, Value value){
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void writeConstant(Chunk* chunk, Value value, int line){
    int constant = addConstant(chunk, value);
    if (constant > UINT8_MAX){
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        writeChunk(chunk, constant, line);
        writeChunk(chunk, constant >> 8, line);
        writeChunk(chunk, constant >> 16, line);
    }else{
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, constant, line);
    }
}

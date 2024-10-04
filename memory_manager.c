#include "memory_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Structure representing a block of memory in the pool
typedef struct MemBlock {
    size_t offset;         // Offset from the start of the memory pool
    size_t size;           // Size of the memory block
    int is_free;           // Flag indicating if the block is free
    struct MemBlock* next; // Pointer to the next memory block
} MemBlock;

static char* memory_pool = NULL;    // Pointer to the memory pool
static size_t pool_size = 0;        // Total size of the memory pool
static MemBlock* block_list = NULL; // Linked list of memory blocks

// Initialize the memory pool with the given size
void mem_init(size_t size) {
    memory_pool = (char*)malloc(size);
    if (!memory_pool) {
        printf("Error: Could not allocate memory pool\n");
        exit(1);
    }
    pool_size = size;

    block_list = (MemBlock*)malloc(sizeof(MemBlock));
    if (!block_list) {
        printf("Error: Could not allocate block list\n");
        exit(1);
    }
    block_list->offset = 0;
    block_list->size = size;
    block_list->is_free = 1;
    block_list->next = NULL;
}

// Allocate a block of memory of the given size
void* mem_alloc(size_t size) {
    if (size == 0) {
        MemBlock* current = block_list;
        while (current) {
            if (current->is_free) {
                return memory_pool + current->offset;
            }
            current = current->next;
        }
        return NULL;
    }

    MemBlock* current = block_list;
    while (current) {
        if (current->is_free && current->size >= size) {
            if (current->size > size) {
                // Split the block if it's larger than needed
                MemBlock* new_block = (MemBlock*)malloc(sizeof(MemBlock));
                if (!new_block) {
                    return NULL;
                }
                new_block->offset = current->offset + size;
                new_block->size = current->size - size;
                new_block->is_free = 1;
                new_block->next = current->next;

                current->size = size;
                current->is_free = 0;
                current->next = new_block;
            } else {
                current->is_free = 0;
            }
            return memory_pool + current->offset;
        }
        current = current->next;
    }

    return NULL;
}

// Free a previously allocated block of memory
void mem_free(void* ptr) {
    if (!ptr) return;

    size_t offset = (char*)ptr - memory_pool;
    MemBlock* current = block_list;
    MemBlock* prev = NULL;

    while (current) {
        if (current->offset == offset) {
            if (current->is_free) {
                return;
            }
            current->is_free = 1;

            // Merge with next block if it's free
            if (current->next && current->next->is_free) {
                MemBlock* next_block = current->next;
                current->size += next_block->size;
                current->next = next_block->next;
                free(next_block);
            }

            // Merge with previous block if it's free
            if (prev && prev->is_free) {
                prev->size += current->size;
                prev->next = current->next;
                free(current);
            }
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Resize an allocated block of memory to a new size
void* mem_resize(void* ptr, size_t size) {
    if (!ptr) return mem_alloc(size);
    if (size == 0) {
        mem_free(ptr);
        return NULL;
    }

    size_t offset = (char*)ptr - memory_pool;
    MemBlock* current = block_list;

    while (current) {
        if (current->offset == offset) {
            if (current->size >= size) {
                if (current->size > size) {
                    // Split the block if it's larger than needed
                    MemBlock* new_block = (MemBlock*)malloc(sizeof(MemBlock));
                    if (!new_block) {
                        return NULL;
                    }
                    new_block->offset = current->offset + size;
                    new_block->size = current->size - size;
                    new_block->is_free = 1;
                    new_block->next = current->next;

                    current->size = size;
                    current->next = new_block;
                }
                return ptr;
            } else {
                // Attempt to merge with next block if possible
                if (current->next && current->next->is_free &&
                    current->size + current->next->size >= size) {
                    current->size += current->next->size;
                    MemBlock* next_block = current->next;
                    current->next = next_block->next;
                    free(next_block);

                    if (current->size > size) {
                        MemBlock* new_block = (MemBlock*)malloc(sizeof(MemBlock));
                        if (!new_block) {
                            return NULL;
                        }
                        new_block->offset = current->offset + size;
                        new_block->size = current->size - size;
                        new_block->is_free = 1;
                        new_block->next = current->next;

                        current->size = size;
                        current->next = new_block;
                    }
                    return ptr;
                } else {
                    // Allocate a new block and copy the data
                    void* new_ptr = mem_alloc(size);
                    if (new_ptr) {
                        memcpy(new_ptr, ptr, current->size);
                        mem_free(ptr);
                    }
                    return new_ptr;
                }
            }
        }
        current = current->next;
    }

    return NULL;
}

// Deinitialize the memory manager and free all resources
void mem_deinit() {
    if (memory_pool) {
        free(memory_pool);
        memory_pool = NULL;
        pool_size = 0;
    }

    MemBlock* current = block_list;
    while (current) {
        MemBlock* next = current->next;
        free(current);
        current = next;
    }
    block_list = NULL;
}

#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>

typedef struct Arena Arena;
struct Arena {
    size_t count;
    size_t capacity;
    uint8_t *items;
    Arena *next;
};

#ifndef ARDEF
#define ARDEF
#endif  // ARDEF

ARDEF Arena* arena_new(void);
ARDEF void* arena_alloc(Arena *arena, size_t nbytes);
ARDEF void arena_free(Arena *arena);

#ifdef ARENA_IMPLEMENTATION

#ifndef ARENA_INIT_CAP
#define ARENA_INIT_CAP (1024 * 8)
#endif  //ARENA_INIT_CAP

#ifndef ARENA_GROW_SIZE
#define ARENA_GROW_SIZE 1.5
#endif  // ARENA_GROW_SIZE

#ifndef ARENA_ALIGN
#define ARENA_ALIGN (sizeof(max_align_t))
static_assert((ARENA_ALIGN & (ARENA_ALIGN - 1)) == 0, "ARENA_ALIGN must be a power of two!");
#endif  // ARENA_ALIGN

#ifndef ARENA_ALLOC
#define ARENA_ALLOC malloc
#endif  // ARENA_ALLOC

#ifndef ARENA_FREE
#define ARENA_FREE free
#endif  // ARENA_FREE

ARDEF Arena* arena__new_cap(const size_t cap)
{
    Arena *arena = ARENA_ALLOC(sizeof(Arena));
    if(!arena) return NULL;
    arena->capacity = cap;
    arena->items = ARENA_ALLOC(cap);
    if(!arena->items)
    {
        ARENA_FREE(arena);
        return NULL;
    }
    arena->count = 0;
    arena->next  = NULL;

    return arena;
}

ARDEF Arena* arena_new(void)
{
    return arena__new_cap(ARENA_INIT_CAP); 
}

ARDEF uintptr_t arena__align_up(const uintptr_t addr)
{
    const uintptr_t mask = ARENA_ALIGN - 1;
    return (addr + mask) & ~mask;
}

ARDEF void* arena_alloc(Arena *arena, size_t nbytes)
{
    const uintptr_t base = (uintptr_t) arena->items;
    const uintptr_t current = base + arena->count;
    const uintptr_t aligned = arena__align_up(current + (uintptr_t) nbytes);    
    const uintptr_t aligned_offset = (aligned - base) + nbytes;
    if(aligned_offset > arena->capacity)
    {
        if(!arena->next)
        {
            size_t next_cap = arena->capacity * ARENA_GROW_SIZE;
            while(next_cap < nbytes) next_cap *= ARENA_GROW_SIZE;
            if(!(arena->next = arena__new_cap(next_cap))) return NULL;
        }

        return arena_alloc(arena->next, nbytes);
    }

    arena->count = aligned_offset;
    return (void*) aligned;
}

ARDEF void arena_free(Arena *arena)
{
    if(!arena) return;

    arena_free(arena->next);
    ARENA_FREE(arena->items);
    ARENA_FREE(arena);
}

#endif // ARENA_IMPLEMENTATION

#endif // ARENA_H

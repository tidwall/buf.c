//==============================================================================
// $ cc buf.c test.c && ./a.out
//==============================================================================

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include "buf.h"

static bool rand_alloc_fail = false;
static int rand_alloc_fail_odds = 3; // 1 in 3 chance malloc will fail.
static uintptr_t total_allocs = 0;
static uintptr_t total_mem = 0;

static void *xmalloc(size_t size) {
    if (rand_alloc_fail && rand()%rand_alloc_fail_odds == 0) {
        return NULL;
    }
    void *mem = malloc(sizeof(uintptr_t)+size);
    assert(mem);
    *(uintptr_t*)mem = size;
    total_allocs++;
    total_mem += size;
    return (char*)mem+sizeof(uintptr_t);
}

static void xfree(void *ptr) {
    if (ptr) {
        total_mem -= *(uintptr_t*)((char*)ptr-sizeof(uintptr_t));
        free((char*)ptr-sizeof(uintptr_t));
        total_allocs--;
    }
}

uint64_t rand_uint() {
     return (((uint64_t)rand()<<33) | 
            ((uint64_t)rand()<<1) | 
            ((uint64_t)rand()<<0)) >> (rand()&63);
}

int64_t rand_int() {
     return (int64_t)(rand_uint()) * ((rand()&1)?1:-1);
}


static void all() {
    static int run_counter = 0;
    int seed = getenv("SEED")?atoi(getenv("SEED")):time(NULL);
    int N = getenv("N")?atoi(getenv("N")):2000;
    printf("seed=%d, count=%d\n", seed, N);
again:
    run_counter++;
    if (run_counter == N) {
        return;
    }
    seed += run_counter;
    srand(seed);

    rand_alloc_fail = true;

    char *bytes;
    while (!(bytes = xmalloc(N))) {}
    for (int i = 0; i < N; i++) {
        bytes[i] = rand();
    }

    struct buf buf = {0};

    for (int i = 0; i < N; ) {
        switch (rand()%2) {
        case 0:
            while (!buf_append_byte(&buf, bytes[i])){};
            i++;
            break;
        case 1: {
                int n = rand()%10;
                if (n > N-i) n = N-i;
                while (!buf_append(&buf, bytes+i, n)){};
                i += n;
            }
            break;
        }
    }

    assert(buf.len == N);
    for (int i = 0; i < N; i++) {
        assert(buf.data[i] == bytes[i]);
    }
    assert(N == 0 || buf.data[N] == 0);

    buf_clear(&buf);
    xfree(bytes);

    
    if (total_allocs != 0) {
        fprintf(stderr, "total_allocs: expected 0, got %lu\n", total_allocs);
        exit(1);
    }
    goto again;
}


int main() {
    buf_set_allocator(xmalloc, xfree);
    printf("Running buf.c tests...\n");
    all();
    printf("PASSED\n");
}


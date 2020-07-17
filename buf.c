// Copyright 2020 Joshua J Baker. All rights reserved.

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "buf.h"

static void *(*_malloc)(size_t) = NULL;
static void (*_free)(void *) = NULL;

#define bmalloc (_malloc?_malloc:malloc)
#define bfree (_free?_free:free)

// buf_set_allocator allows for configuring a custom allocator for
// all buf library operations. This function, if needed, should be called
// only once at startup and a prior to calling buf_append*().
void buf_set_allocator(void *(malloc)(size_t), void (*free)(void*)) {
    _malloc = malloc;
    _free = free;
}

// buf_append appends data to buffer. To append a null-terminated c-string
// specify -1 for the len.
bool buf_append(struct buf *buf, const char *data, ssize_t len) {
    if (len < 0) {
        len = strlen(data);
    }
    if (buf->len+len >= buf->cap) {
        size_t cap = buf->cap ? buf->cap * 2 : 1;
        while (buf->len+len > cap) {
            cap *= 2;
        }
        char *data = bmalloc(cap+1);
        if (!data) {
            return false;
        }
        memcpy(data, buf->data, buf->len);
        bfree(buf->data);
        buf->data = data;
        buf->cap = cap;
    }
    memcpy(buf->data+buf->len, data, len);
    buf->len += len;
    buf->data[buf->len] = '\0';
    return true;
}

// buf_append_byte appends a single byte to buffer.
// Returns false if the 
bool buf_append_byte(struct buf *buf, char ch) {
    if (buf->len == buf->cap) {
        return buf_append(buf, &ch, 1);
    }
    buf->data[buf->len] = ch;
    buf->len++;
    buf->data[buf->len] = '\0';
    return true;
}

// buf_clear clears the buffer and frees all data
void buf_clear(struct buf *buf) {
    if (buf->data) {
        bfree(buf->data);
    }
    memset(buf, 0, sizeof(struct buf));
}

//==============================================================================
// TESTS AND BENCHMARKS
// $ cc -DBUF_TEST buf.c && ./a.out              # run tests
// $ cc -DBUF_TEST -O3 buf.c && BENCH=1 ./a.out  # run benchmarks
//==============================================================================
#ifdef BUF_TEST

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>

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
    while (!(bytes = bmalloc(N))) {}
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
    bfree(bytes);

    if (total_allocs != 0) {
        fprintf(stderr, "total_allocs: expected 0, got %lu\n", total_allocs);
        exit(1);
    }
    goto again;
}

static void benchmarks() {}

int main() {
    buf_set_allocator(xmalloc, xfree);
    if (getenv("BENCH")) {
        printf("Running buf.c benchmarks...\n");
        benchmarks();
    } else {
        printf("Running buf.c tests...\n");
        all();
        printf("PASSED\n");
    }
}

#endif

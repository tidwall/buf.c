// https://github.com/tidwall/buf.c
//
// Copyright 2024 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
//
// buf.c: Simple byte buffers

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

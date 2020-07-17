// Copyright 2020 Joshua J Baker. All rights reserved.

#ifndef BUF_H
#define BUF_H

#include <sys/types.h>
#include <stdbool.h>

struct buf {
    char *data;
    size_t len, cap;
};

bool buf_append(struct buf *buf, const char *data, ssize_t len);
bool buf_append_byte(struct buf *buf, char ch);
void buf_clear(struct buf *buf);
void buf_set_allocator(void *(malloc)(size_t), void (*free)(void*));

#endif

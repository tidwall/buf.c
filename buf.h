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

bool buf_append_uvarint(struct buf *buf, uint64_t x);
int buf_uvarint(struct buf *buf, size_t offset, uint64_t *x);
bool buf_append_varint(struct buf *buf, int64_t x);
int buf_varint(struct buf *buf, size_t offset, int64_t *x);

#endif

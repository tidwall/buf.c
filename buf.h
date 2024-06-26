// https://github.com/tidwall/buf.c
//
// Copyright 2024 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
//
// buf.c: Simple byte buffers

#ifndef BUF_H
#define BUF_H

#include <sys/types.h>
#include <stdbool.h>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

struct buf {
    char *data;
    size_t len, cap;
};

bool buf_append(struct buf *buf, const char *data, ssize_t len);
bool buf_append_byte(struct buf *buf, char ch);
void buf_clear(struct buf *buf);
void buf_set_allocator(void *(malloc)(size_t), void (*free)(void*));

#endif

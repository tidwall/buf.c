# buf.c

Simple byte buffers

## Example

```c
#include <stdio.h>
#include "buf.h"

int main() {
    struct buf buf = { 0 };
    buf_append(&buf, "hello", 5);    // append data
    buf_append(&buf, " world", -1);  // append c-string
    buf_append_byte(&buf, '!');      // append single byte

    printf("%s\n", buf.data);

    buf_clear(&buf);                 // clear and free the buffer
}
// output:
// hello world!
```

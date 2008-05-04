#include <stdlib.h>
#include "object.h"
#include "block.h"

object_t block_new(size_t size) {
    object_t self;
    self.tag = OBJECT_BLOCK_TAG;
    self.data.pointer = malloc(size);
    return self;
}

void *block_unbox(object_t self) {
    if (!object_is_block(self))
        exit(1);
    return self.data.pointer;
}

#include <stdlib.h>
#include "/usr/include/string.h"
#include "object.h"
#include "block.h"
#include "string.h"

object_t string_new(const char *str) {
    object_t self;
    size_t len;
    len = strlen(str);
    self = block_new(len + 1);
    strcpy((char *)block_unbox(self), str);
    return self;
}

char *string_unbox(object_t self) {
    return (char *)block_unbox(self);
}

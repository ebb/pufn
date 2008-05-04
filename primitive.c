#include <stdlib.h>
#include "object.h"
#include "primitive.h"
#include "block.h"
#include "word.h"
#include "string.h"

object_t primitive_new(const char *name, primitive_t c_function) {
    object_t self;
    object_t block;
    block = block_new(sizeof(primitive_t));
    *(primitive_t *)block_unbox(block) = c_function;
    self = word_new(string_new(name), block);
    return self;
}

/* public domain */

#include <stdlib.h>
#include "object.h"
#include "word.h"
#include "wrapper.h"
#include "fail.h"

object_t wrapper_new(object_t word) {
    object_t self;
    self.tag = OBJECT_WRAPPER_TAG;
    self.data.pointer = malloc(sizeof(object_t));
    *(object_t *)self.data.pointer = word;
    return self;
}

object_t wrapper_unbox(object_t self) {
    if (!object_is_wrapper(self))
        fail();
    return *(object_t *)self.data.pointer;
}

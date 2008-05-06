/* public domain */

#include "object.h"
#include "boolean.h"

object_t boolean_new(int value) {
    object_t self;
    self.tag = OBJECT_BOOLEAN_TAG;
    self.data.number = value;
    return self;
}

int boolean_unwrap(object_t self) {
    if (!object_is_boolean(self))
        fail();
    return self.data.number;
}

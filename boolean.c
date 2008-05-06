/* public domain */

#include "object.h"
#include "boolean.h"

object_t boolean_t;
object_t boolean_f;

static object_t boolean_new(int value) {
    object_t self;
    self.tag = OBJECT_BOOLEAN_TAG;
    self.data.number = value;
    return self;
}

void boolean_initialize() {
    boolean_t = boolean_new(1);
    boolean_f = boolean_new(0);
}

int boolean_unbox(object_t self) {
    if (!object_is_boolean(self))
        fail();
    return self.data.number;
}

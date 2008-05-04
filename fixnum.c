#include <stdlib.h>
#include "object.h"
#include "fixnum.h"

object_t fixnum_new(int n) {
    object_t self;
    self.tag = OBJECT_FIXNUM_TAG;
    self.data.number = n;
    return self;
}

int fixnum_unbox(object_t self) {
    if (!object_is_fixnum(self))
        exit(1);
    return self.data.number;
}

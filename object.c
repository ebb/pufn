/* public domain */

#include <stdlib.h>
#include "object.h"

int object_eq(object_t a, object_t b) {
    if (a.tag != b.tag)
        return 0;
    if (object_is_fixnum(a))
        return a.data.number == b.data.number;
    else
        return a.data.pointer == b.data.pointer;
}

void object_free(object_t self) {
    switch (self.tag) {
        case OBJECT_WORD_TAG:
        case OBJECT_LIST_TAG:
        case OBJECT_PRIMITIVE_TAG:
        case OBJECT_STRING_TAG:
        case OBJECT_WRAPPER_TAG:
        case OBJECT_CONTINUATION_TAG:
        case OBJECT_BLOCK_TAG:
            free(self.data.pointer);
    }
}

int object_is_fixnum(object_t self) {
    return self.tag == OBJECT_FIXNUM_TAG;
}

int object_is_word(object_t self) {
    return self.tag == OBJECT_WORD_TAG;
}

int object_is_list(object_t self) {
    return self.tag == OBJECT_LIST_TAG;
}

int object_is_primitive(object_t self) {
    return self.tag == OBJECT_PRIMITIVE_TAG;
}

int object_is_string(object_t self) {
    return self.tag == OBJECT_STRING_TAG;
}

int object_is_boolean(object_t self) {
    return self.tag == OBJECT_BOOLEAN_TAG;
}

int object_is_wrapper(object_t self) {
    return self.tag == OBJECT_WRAPPER_TAG;
}

int object_is_continuation(object_t self) {
    return self.tag == OBJECT_CONTINUATION_TAG;
}

int object_is_block(object_t self) {
    return self.tag == OBJECT_BLOCK_TAG;
}

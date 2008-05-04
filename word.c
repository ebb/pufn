#include <stdlib.h>
#include "object.h"
#include "word.h"

object_t word_new(object_t name, object_t definition) {
    object_t self;
    self.tag = OBJECT_WORD_TAG;
    self.data.pointer = malloc(2 * sizeof(object_t));
    ((object_t *)self.data.pointer)[0] = name;
    ((object_t *)self.data.pointer)[1] = definition;
    return self;
}

object_t *word_unbox(object_t self) {
    if (!object_is_word(self))
        exit(1);
    return (object_t *)self.data.pointer;
}

object_t word_name(object_t self) {
    return word_unbox(self)[0];
}

object_t word_definition(object_t self) {
    return word_unbox(self)[1];
}

void word_set_definition(object_t self, object_t definition) {
    word_unbox(self)[1] = definition;
}

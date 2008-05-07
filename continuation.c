/* public domain */

#include <stdlib.h>
#include "object.h"
#include "machine.h"
#include "continuation.h"

object_t continuation_new(machine_t *machine) {
    object_t self;
    self.tag = OBJECT_CONTINUATION_TAG;
    self.data.pointer = malloc(sizeof(machine_t));
    *(machine_t *)self.data.pointer = *machine;
    return self;
}

machine_t *continuation_unbox(object_t self) {
    return (machine_t *)self.data.pointer;
}

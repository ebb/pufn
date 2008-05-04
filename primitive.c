#include <stdlib.h>
#include "object.h"
#include "machine.h"
#include "primitive.h"

object_t primitive_new(primitive_t c_function) {
    object_t self;
    self.tag = OBJECT_PRIMITIVE_TAG;
    self.data.pointer = malloc(sizeof(primitive_t));
    *(primitive_t *)self.data.pointer = c_function;
    return self;
}

primitive_t *primitive_unbox(object_t self) {
    if (!object_is_primitive(self))
        exit(1);
    return (primitive_t *)self.data.pointer;
}

machine_t *primitive_execute(object_t self, machine_t *machine) {
    primitive_t c_function;
    c_function = *primitive_unbox(self);
    return c_function(machine);
}

#include <stdlib.h>
#include "/usr/include/string.h"
#include "object.h"
#include "word.h"
#include "list.h"
#include "block.h"
#include "machine.h"
#include "primitive.h"

static machine_t machine_prototype;

void machine_initialize() {
    machine_prototype.core.call = list_nil;
    machine_prototype.core.data = list_nil;
    machine_prototype.retain = list_nil;
    machine_prototype.dictionary = list_nil;
}

machine_t *machine_new(object_t dictionary) {
    machine_t *machine;
    machine = machine_copy(&machine_prototype);
    machine->dictionary = dictionary;
    return machine;
}

machine_t *machine_copy(machine_t *self) {
    machine_t *copy;
    copy = (machine_t *)malloc(sizeof(machine_t));
    memcpy(copy, self, sizeof(machine_t));
    return copy;
}

static machine_t *machine_primitive(object_t primitive, machine_t *machine) {
    primitive_t c_function;
    c_function = *(primitive_t *)block_unbox(primitive);
    return c_function(machine);
}

static machine_t *machine_loop(machine_t *machine, object_t r) {
    machine_core_t *core;
    machine = machine_copy(machine);
    core = &machine->core;
    if (object_is_word(r))
        goto exec;
call:
    core->call = list_append(r, core->call);
eval:
    if (list_empty(core->call))
        return machine;
    else {
        r = list_head(core->call);
        core->call = list_tail(core->call);
        if (object_is_word(r))
            goto exec;
        else {
            core->data = list_new(r, core->data);
            goto eval;
        }
    }
exec:
    r = word_definition(r);
    if (object_is_block(r)) {
        machine = machine_primitive(r, machine);
        goto eval;
    } else
        goto call;
}

machine_t *machine_call(machine_t *machine, object_t quote) {
    return machine_loop(machine, quote);
}

machine_t *machine_execute(machine_t *machine, object_t word) {
    return machine_loop(machine, word);
}

/* public domain */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "object.h"
#include "word.h"
#include "list.h"
#include "wrapper.h"
#include "machine.h"
#include "primitive.h"
#include "fail.h"
#include "print.h"

static machine_t machine_prototype;
static int machine_logging_p;

void machine_initialize() {
    machine_prototype.core.call = list_nil;
    machine_prototype.core.data = list_nil;
    machine_prototype.retain = list_nil;
    machine_prototype.dictionary = list_nil;
    machine_disable_logging();
}

void machine_enable_logging() {
    machine_logging_p = 1;
}

void machine_disable_logging() {
    machine_logging_p = 0;
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

static void on_exec(object_t r, machine_t *machine) {
    if (machine_logging_p) {
        printf("exec: "); print_object(r);
        printf("\n  call: ");
        print_object(list_take(3, machine->core.call));
        printf("\n  data: ");
        print_object(list_take(3, machine->core.data));
        printf("\n");
    }
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
            if (object_is_wrapper(r))
                r = wrapper_unbox(r);
            core->data = list_new(r, core->data);
            goto eval;
        }
    }
exec:
    on_exec(r, machine);
    r = word_definition(r);
    if (object_is_primitive(r)) {
        machine = primitive_execute(r, machine);
        core = &machine->core;
        goto eval;
    } else
        goto call;
}

machine_t *machine_call(machine_t *machine, object_t quote) {
    if (!object_is_list(quote))
        fail();
    return machine_loop(machine, quote);
}

machine_t *machine_execute(machine_t *machine, object_t word) {
    if (!object_is_word(word))
        fail();
    return machine_loop(machine, word);
}

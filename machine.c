#include <stdlib.h>
#include "object.h"
#include "word.h"
#include "list.h"
#include "block.h"
#include "machine.h"

object_t machine_new() {
    return list_new_4(list_nil, list_nil, list_nil, list_nil);
}

object_t machine_return_stack(object_t self) {
    return list_nth(self, 0);
}

object_t machine_data_stack(object_t self) {
    return list_nth(self, 1);
}

object_t machine_retain_stack(object_t self) {
    return list_nth(self, 2);
}

object_t machine_dictionary(object_t self) {
    return list_nth(self, 3);
}

object_t machine_replace_return_stack(object_t self, object_t stack) {
    return list_replace_nth(self, 0, stack);
}

object_t machine_replace_data_stack(object_t self, object_t stack) {
    return list_replace_nth(self, 1, stack);
}

object_t machine_replace_retain_stack(object_t self, object_t stack) {
    return list_replace_nth(self, 2, stack);
}

object_t machine_replace_dictionary(object_t self, object_t dict) {
    return list_replace_nth(self, 3, dict);
}

object_t machine_return_push(object_t self, object_t list) {
    if (object_eq(list, list_nil))
        return self;
    else {
        object_t stack;
        stack = list_new(list, machine_return_stack(self));
        return machine_replace_return_stack(self, stack);
    }
}

object_t machine_data_push(object_t self, object_t object) {
    object_t stack;
    stack = list_new(object, machine_data_stack(self));
    return machine_replace_data_stack(self, stack);
}

object_t machine_data_pop(object_t self, object_t *object) {
    object_t stack;
    stack = machine_data_stack(self);
    *object = list_head(stack);
    return machine_replace_data_stack(self, list_tail(stack));
}

void machine_return_pop(object_t self, object_t *machine, object_t *list) {
    object_t stack;
    stack = machine_return_stack(self);
    if (object_eq(stack, list_nil)) {
        *machine = self;
        *list = list_nil;
    } else {
        *machine = machine_replace_return_stack(self, list_tail(stack));
        *list = list_head(stack);
    }
}

enum {
    MACHINE_ENTRY_EXECUTE,
    MACHINE_ENTRY_CALL
};

static object_t machine_loop(int entry, object_t machine, object_t object) {
    if (entry == MACHINE_ENTRY_EXECUTE)
        goto label_execute;
label_call:
    if (object_eq(list_nil, object))
        goto label_return;
    else {
        machine = machine_return_push(machine, list_tail(object));
        object = list_head(object);
        goto label_evaluate;
    }
label_evaluate:
    if (object_is_word(object))
        goto label_execute;
    else {
        machine = machine_data_push(machine, object);
        goto label_return;
    }
label_execute:
    object = word_definition(object);
    if (object_is_block(object)) {
        machine = block_execute(object, machine);
        goto label_return;
    } else
        goto label_call;
label_return:
    machine_return_pop(machine, &machine, &object);
    if (object_eq(object, list_nil))
        return machine;
    else
        goto label_call;
}

object_t machine_execute(object_t machine, object_t word) {
    return machine_loop(MACHINE_ENTRY_EXECUTE, machine, word);
}

object_t machine_call(object_t machine, object_t list) {
    return machine_loop(MACHINE_ENTRY_CALL, machine, list);
}

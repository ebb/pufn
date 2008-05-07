/* public domain */

#include <stdio.h>
#include "object.h"
#include "fixnum.h"
#include "word.h"
#include "list.h"
#include "string.h"
#include "boolean.h"
#include "dictionary.h"
#include "machine.h"
#include "primitive.h"
#include "prelude.h"
#include "print.h"

object_t prelude;

static object_t prelude_pop(machine_t *machine) {
    object_t object;
    object_t stack;
    stack = machine->core.data;
    if (object_eq(list_nil, stack))
        fail();
    object = list_head(stack);
    machine->core.data = list_tail(stack);
    return object;
}

static void prelude_push(machine_t *machine, object_t object) {
    machine->core.data = list_new(object, machine->core.data);
}

machine_t *prelude__call(machine_t *machine) {
    object_t quote;
    quote = prelude_pop(machine);
    machine->core.call = list_append(quote, machine->core.call);
    return machine;
}

machine_t *prelude__execute(machine_t *machine) {
    object_t word;
    word = prelude_pop(machine);
    machine->core.call = list_new(word, machine->core.call);
    return machine;
}

machine_t *prelude__if(machine_t *machine) {
    object_t c, qt, qf;
    qf = prelude_pop(machine);
    qt = prelude_pop(machine);
    c = prelude_pop(machine);
    if (boolean_unbox(c))
        prelude_push(machine, qt);
    else
        prelude_push(machine, qf);
    return prelude__call(machine);
}

machine_t *prelude__dup(machine_t *machine) {
    object_t object;
    object = list_head(machine->core.data);
    machine->core.data = list_new(object, machine->core.data);
    return machine;
}

machine_t *prelude__drop(machine_t *machine) {
    prelude_pop(machine);
    return machine;
}

machine_t *prelude__swap(machine_t *machine) {
    object_t a, b;
    b = prelude_pop(machine);
    a = prelude_pop(machine);
    prelude_push(machine, b);
    prelude_push(machine, a);
    return machine;
}

machine_t *prelude__retain(machine_t *machine) {
    object_t x;
    x = prelude_pop(machine);
    machine->retain = list_new(x, machine->retain);
    return machine;
}

machine_t *prelude__release(machine_t *machine) {
    object_t x;
    x = list_head(machine->retain);
    machine->retain = list_tail(machine->retain);
    prelude_push(machine, x);
    return machine;
}

machine_t *prelude__eq(machine_t *machine) {
    object_t a, b;
    object_t eq;
    b = prelude_pop(machine);
    a = prelude_pop(machine);
    eq = object_eq(a, b) ? boolean_t : boolean_f;
    machine->core.data = list_new(eq, machine->core.data);
    return machine;
}

machine_t *prelude__add(machine_t *machine) {
    object_t a, b, d;
    b = prelude_pop(machine);
    a = prelude_pop(machine);
    d = fixnum_new(fixnum_unbox(a) + fixnum_unbox(b));
    machine->core.data = list_new(d, machine->core.data);
    return machine;
}

machine_t *prelude__subtract(machine_t *machine) {
    object_t a, b, d;
    b = prelude_pop(machine);
    a = prelude_pop(machine);
    d = fixnum_new(fixnum_unbox(a) - fixnum_unbox(b));
    machine->core.data = list_new(d, machine->core.data);
    return machine;
}

machine_t *prelude__multiply(machine_t *machine) {
    object_t a, b, d;
    b = prelude_pop(machine);
    a = prelude_pop(machine);
    d = fixnum_new(fixnum_unbox(a) * fixnum_unbox(b));
    machine->core.data = list_new(d, machine->core.data);
    return machine;
}

machine_t *prelude__divide(machine_t *machine) {
    object_t a, b, d;
    b = prelude_pop(machine);
    a = prelude_pop(machine);
    d = fixnum_new(fixnum_unbox(a) / fixnum_unbox(b));
    machine->core.data = list_new(d, machine->core.data);
    return machine;
}

machine_t *prelude__dot(machine_t *machine) {
    object_t object;
    object = prelude_pop(machine);
    print_object(object);
    return machine;
}

machine_t *prelude__newline(machine_t *machine) {
    printf("\n");
    return machine;
}

struct entry {
    const char *name;
    primitive_t definition;
    int parsing_p;
};
    
struct entry entries[] = {
    {
         "call", prelude__call, 0
    },
    {
        "execute", prelude__execute, 0
    },
    {
        "if", prelude__if, 0
    },
    {
        "dup", prelude__dup, 0
    },
    {
        "drop", prelude__drop, 0
    },
    {
        "eq", prelude__eq, 0
    },
    {
        "+", prelude__add, 0
    },
    {
        "-", prelude__subtract, 0
    },
    {
        "*", prelude__multiply, 0
    },
    {
        "/", prelude__divide, 0
    },
    {
        "swap", prelude__swap, 0
    },
    {
        ">r", prelude__retain, 0
    },
    {
        "r>", prelude__release, 0
    },
    {
        ".", prelude__dot, 0
    },
    {
        "newline", prelude__newline, 0
    }
};

void prelude_initialize() {
    int i, n;
    n = sizeof(entries) / sizeof(struct entry);
    prelude = list_nil;
    for (i = 0; i < n; ++i) {
        object_t word;
        word = word_new(string_new(entries[i].name),
                        primitive_new(entries[i].definition),
                        entries[i].parsing_p ? boolean_t : boolean_f);
        prelude = dictionary_insert(prelude, word);
    }
}

/* public domain */

#include <stdio.h>
#include <string.h>
#include "object.h"
#include "fixnum.h"
#include "word.h"
#include "list.h"
#include "string.h"
#include "boolean.h"
#include "dictionary.h"
#include "machine.h"
#include "primitive.h"
#include "continuation.h"
#include "prelude.h"
#include "print.h"
#include "parse.h"
#include "fail.h"

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

machine_t *prelude__t(machine_t *machine) {
    prelude_push(machine, boolean_t);
    return machine;
}

machine_t *prelude__f(machine_t *machine) {
    prelude_push(machine, boolean_f);
    return machine;
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

machine_t *prelude__curry(machine_t *machine) {
    object_t quote;
    object_t object;
    quote = prelude_pop(machine);
    object = prelude_pop(machine);
    quote = list_new(object, quote);
    prelude_push(machine, quote);
    return machine;
}

machine_t *prelude__compose(machine_t *machine) {
    object_t quote1, quote2;
    object_t composed;
    quote2 = prelude_pop(machine);
    quote1 = prelude_pop(machine);
    composed = list_append(quote1, quote2);
    prelude_push(machine, composed);
    return machine;
}

machine_t *prelude__callcc(machine_t *machine) {
    object_t quote;
    machine_t *copy;
    quote = prelude_pop(machine);
    copy = machine_copy(machine);
    quote = list_new(continuation_new(copy), quote);
    prelude_push(machine, quote);
    return prelude__call(machine);
}

machine_t *prelude__continue(machine_t *machine) {
    int count;
    object_t continuation;
    object_t values;
    continuation = prelude_pop(machine);
    count = fixnum_unbox(prelude_pop(machine));
    values = list_nil;
    while (count-- > 0)
        values = list_new(prelude_pop(machine), values);
    machine = machine_copy(continuation_unbox(continuation));
    machine->core.data = list_reverse_append(values, machine->core.data);
    return machine;
}

machine_t *prelude__if(machine_t *machine) {
    object_t c, qt, qf;
    qf = prelude_pop(machine);
    qt = prelude_pop(machine);
    c = prelude_pop(machine);
    prelude_push(machine, object_eq(c, boolean_f) ? qf : qt);
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

machine_t *prelude__parse_definition(machine_t *machine) {
    return parse_definition(machine);
}

machine_t *prelude__parse_quote(machine_t *machine) {
    return parse_quote(machine);
}

machine_t *prelude__parse_wrapper(machine_t *machine) {
    return parse_wrapper(machine);
}

machine_t *prelude__quote_delimiter(machine_t *machine) {
    fail();
    return machine;
}

machine_t *prelude__definition_delimiter(machine_t *machine) {
    fail();
    return machine;
}

machine_t *prelude__list_p(machine_t *machine) {
    object_t object, p;
    object = prelude_pop(machine);
    p = object_is_list(object) ? boolean_t : boolean_f;
    prelude_push(machine, p);
    return machine;
}

machine_t *prelude__string_p(machine_t *machine) {
    object_t object, p;
    object = prelude_pop(machine);
    p = object_is_string(object) ? boolean_t : boolean_f;
    prelude_push(machine, p);
    return machine;
}

machine_t *prelude__type_tag(machine_t *machine) {
    object_t object;
    object = prelude_pop(machine);
    prelude_push(machine, fixnum_new(object.tag));
    return machine;
}

machine_t *prelude__list_head(machine_t *machine) {
    object_t list;
    list = prelude_pop(machine);
    prelude_push(machine, list_head(list));
    return machine;
}

machine_t *prelude__list_tail(machine_t *machine) {
    object_t list;
    list = prelude_pop(machine);
    prelude_push(machine, list_tail(list));
    return machine;
}

machine_t *prelude__string_equal(machine_t *machine) {
    object_t a, b, p;
    b = prelude_pop(machine);
    a = prelude_pop(machine);
    p = (0 == strcmp(string_unbox(a), string_unbox(b)))
        ? boolean_t : boolean_f;
    prelude_push(machine, p);
    return machine;
}

machine_t *prelude__word_definition(machine_t *machine) {
    object_t word;
    word = prelude_pop(machine);
    prelude_push(machine, word_definition(word));
    return machine;
}

machine_t *prelude__word_name(machine_t *machine) {
    object_t word;
    word = prelude_pop(machine);
    prelude_push(machine, word_name(word));
    return machine;
}

machine_t *prelude__enable_logging(machine_t *machine) {
    machine_enable_logging();
    return machine;
}

machine_t *prelude__disable_logging(machine_t *machine) {
    machine_disable_logging();
    return machine;
}

struct entry {
    const char *name;
    primitive_t definition;
    int parsing_p;
};
    
struct entry entries[] = {
    {
        "t", prelude__t, 0
    },
    {
        "f", prelude__f, 0
    },
    {
         "call", prelude__call, 0
    },
    {
        "execute", prelude__execute, 0
    },
    {
        "curry", prelude__curry, 0
    },
    {
        "compose", prelude__compose, 0
    },
    {
        "callcc", prelude__callcc, 0
    },
    {
        "continue", prelude__continue, 0
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
    },
    {
        ":", prelude__parse_definition, 1
    },
    {
        "[", prelude__parse_quote, 1
    },
    {
        "\\", prelude__parse_wrapper, 1
    },
    {
        "]", prelude__quote_delimiter, 0
    },
    {
        ";", prelude__definition_delimiter, 0
    },
    {
        "type-tag", prelude__type_tag, 0
    },
    {
        "list?", prelude__list_p, 0
    },
    {
        "string?", prelude__string_p, 0
    },
    {
        "list-head", prelude__list_head, 0
    },
    {
        "list-tail", prelude__list_tail, 0
    },
    {
        "string=", prelude__string_equal, 0
    },
    {
        "enable-logging", prelude__enable_logging, 0
    },
    {
        "disable-logging", prelude__disable_logging, 0
    },
    {
        "word-definition", prelude__word_definition, 0
    },
    {
        "word-name", prelude__word_name, 0
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

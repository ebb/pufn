/* public domain */

#include <stdlib.h>
#include "fail.h"
#include "object.h"
#include "fixnum.h"
#include "word.h"
#include "list.h"
#include "string.h"
#include "boolean.h"
#include "block.h"
#include "dictionary.h"
#include "machine.h"
#include "primitive.h"
#include "parse.h"
#include "prelude.h"

machine_t *one_plus(machine_t *machine) {
    object_t *data_stack;
    object_t value;
    data_stack = &machine->core.data;
    value = list_head(*data_stack);
    value = fixnum_new(1 + fixnum_unbox(value));
    *data_stack = list_new(value, list_tail(*data_stack));
    return machine;
}

machine_t *primitive_fail(machine_t *machine) {
    fail();
    return 0;
}

machine_t *true(machine_t *machine) {
    machine->core.data = list_new(boolean_t, machine->core.data);
    return machine;
}

machine_t *false(machine_t *machine) {
    machine->core.data = list_new(boolean_f, machine->core.data);
    return machine;
}

static object_t main_primitive(const char *name, primitive_t c_function) {
    return word_new(string_new(name), primitive_new(c_function), boolean_f);
}

static object_t main_parsing_primitive(const char *name, 
                                       primitive_t c_function) {
    return word_new(string_new(name), primitive_new(c_function), boolean_t);
}

int main (int argc, const char *argv[]) {
    machine_t *machine;
    object_t quote;
    object_t primitive_true;
    object_t primitive_false;
    object_t primitive_one_plus;
    object_t primitive_parse_quote;
    object_t primitive_parse_definition;
    object_t quote_delimiter;
    object_t definition_delimiter;
    object_t primitive_parse_wrapper;
    object_t dictionary;
    boolean_initialize();
    list_initialize();
    machine_initialize();
    prelude_initialize();
    primitive_true = main_primitive("t", true);
    primitive_false = main_primitive("f", false);
    primitive_one_plus = main_primitive("one_plus", one_plus);
    primitive_parse_quote = main_parsing_primitive("[", parse_quote);
    primitive_parse_definition = main_parsing_primitive(":", parse_definition);
    quote_delimiter = main_primitive("]", primitive_fail);
    definition_delimiter = main_primitive(";", primitive_fail);
    primitive_parse_wrapper = main_parsing_primitive("\\", parse_wrapper);
    dictionary = prelude;
    dictionary = dictionary_insert(dictionary, primitive_true);
    dictionary = dictionary_insert(dictionary, primitive_false);
    dictionary = dictionary_insert(dictionary, primitive_one_plus);
    dictionary = dictionary_insert(dictionary, primitive_parse_quote);
    dictionary = dictionary_insert(dictionary, quote_delimiter);
    dictionary = dictionary_insert(dictionary, primitive_parse_definition);
    dictionary = dictionary_insert(dictionary, definition_delimiter);
    dictionary = dictionary_insert(dictionary, primitive_parse_wrapper);
    machine = machine_new(dictionary);
    quote = parse_file(machine, "/Users/eric/cufn/tests.stuf");
    machine = machine_call(machine, quote);
    print_object(machine->core.data);
    return 0;
}

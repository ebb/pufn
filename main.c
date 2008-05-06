/* public domain */

#include <stdlib.h>
#include "fail.h"
#include "object.h"
#include "fixnum.h"
#include "word.h"
#include "list.h"
#include "string.h"
#include "block.h"
#include "dictionary.h"
#include "machine.h"
#include "primitive.h"
#include "parse.h"

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

static object_t main_primitive(const char *name, primitive_t c_function) {
    return word_new(string_new(name), primitive_new(c_function));
}

int main (int argc, const char *argv[]) {
    machine_t *machine;
    object_t quote;
    object_t primitive_one_plus;
    object_t primitive_parse_quote;
    object_t primitive_parse_definition;
    object_t quote_delimiter;
    object_t definition_delimiter;
    object_t dictionary;
    list_initialize();
    machine_initialize();
    primitive_one_plus = main_primitive("one_plus", one_plus);
    primitive_parse_quote = main_primitive("[", parse_quote);
    primitive_parse_definition = main_primitive(":", parse_definition);
    quote_delimiter = main_primitive("]", primitive_fail);
    definition_delimiter = main_primitive(";", primitive_fail);
    dictionary = dictionary_insert(list_nil, primitive_one_plus);
    dictionary = dictionary_insert(dictionary, primitive_parse_quote);
    dictionary = dictionary_insert(dictionary, quote_delimiter);
    dictionary = dictionary_insert(dictionary, primitive_parse_definition);
    dictionary = dictionary_insert(dictionary, definition_delimiter);
    machine = machine_new(dictionary);
    quote = parse_file(machine, "/Users/eric/tmp/test.cufn");
    machine = machine_call(machine, quote);
    print_object(machine->core.data);
    return 0;
}

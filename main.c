/* initial interpreter written in C
 * used to write interpreter for more complete language
 * second language gives access to its own interpreter
 */

#include <stdlib.h>
#include "object.h"
#include "fixnum.h"
#include "word.h"
#include "list.h"
#include "block.h"
#include "primitive.h"
#include "machine.h"
#include "string.h"
#include "machine.h"
#include "parse.h"
#include "dictionary.h"

object_t one_plus(object_t machine) {
    object_t stack;
    object_t value;
    stack = machine_data_stack(machine);
    value = fixnum_new(1 + fixnum_unbox(list_head(stack)));
    stack = list_new(value, list_tail(stack));
    return machine_replace_data_stack(machine, stack);
}

object_t primitive_exit(object_t machine) {
    exit(1);
}

int main (int argc, const char *argv[]) {
    object_t machine;
    object_t quote;
    object_t primitive_one_plus;
    object_t primitive_parse_quote;
    object_t primitive_parse_definition;
    object_t quote_delimiter;
    object_t definition_delimiter;
    object_t dictionary;
    list_initialize();
    machine = machine_new();
    primitive_one_plus = primitive_new("one_plus", one_plus);
    primitive_parse_quote = primitive_new("[", parse_quote);
    primitive_parse_definition = primitive_new(":", parse_definition);
    quote_delimiter = primitive_new("]", primitive_exit);
    definition_delimiter = primitive_new(";", primitive_exit);
    dictionary = dictionary_insert(list_nil, primitive_one_plus);
    dictionary = dictionary_insert(dictionary, primitive_parse_quote);
    dictionary = dictionary_insert(dictionary, quote_delimiter);
    dictionary = dictionary_insert(dictionary, primitive_parse_definition);
    dictionary = dictionary_insert(dictionary, definition_delimiter);
    machine = machine_replace_dictionary(machine, dictionary);
    quote = parse_file(machine, "/Users/eric/tmp/test.cufn");
    machine = machine_call(machine, quote);
    print_object(machine_data_stack(machine));
    return 0;
}

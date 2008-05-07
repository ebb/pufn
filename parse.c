/* public domain */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fail.h"
#include "object.h"
#include "string.h"
#include "list.h"
#include "fixnum.h"
#include "word.h"
#include "boolean.h"
#include "wrapper.h"
#include "dictionary.h"
#include "machine.h"
#include "parse.h"
#include "print.h"

/* TODO: parse strings
 *  treat CHAR: " specially during tokenization
 *  pull out the characters delimited by the quotation marks
 *  hand the string to a handler as follows:
 *    "foo" goes to the normal string parser
 *    P"tmp" goes to a parsing word named "P"
 *  the dispatch is featuritis -- put it off
 */

#define PARSE_LINE_MAX 1024
#define PARSE_TEXT_MAX 1024

static char parse_line_buffer[PARSE_LINE_MAX];
static char *parse_text[PARSE_TEXT_MAX];
static int parse_line;
static int parse_column;
static int parse_line_count;

machine_t *parse_file(machine_t *machine) {
    FILE *file;
    object_t phony_word;
    object_t filename;
    filename = list_pop(&machine->core.data);
    file = fopen(string_unbox(filename), "r");
    if (file == 0)
        fail();
    parse_line_count = 0;
    while (fgets(parse_line_buffer, PARSE_LINE_MAX, file) != 0) {
        parse_text[parse_line_count] =
            (char *)malloc(strlen(parse_line_buffer) + 1);
        strcpy(parse_text[parse_line_count], parse_line_buffer);
        ++parse_line_count;
    }
    fclose(file);
    parse_line = 0;
    parse_column = 0;
    phony_word = word_new(string_new(""), list_nil, boolean_f);
    list_push(&machine->core.data, phony_word);
    return parse_until_word(machine);
}

int parse_is_parsing_word(object_t word) {
    return boolean_unbox(word_parsing_p(word));
}

machine_t *parse_wrapper(machine_t *machine) {
    object_t dictionary;
    object_t name;
    object_t word;
    dictionary = machine->dictionary;
    if (!parse_token(&name))
        fail();
    word = dictionary_find(dictionary, name);
    machine->core.data = list_new(list_new_1(wrapper_new(word)),
                                  machine->core.data);
    return machine;
}

machine_t *parse_quote(machine_t *machine) {
    object_t dictionary;
    object_t delimiter;
    object_t quote;
    dictionary = machine->dictionary;
    delimiter = dictionary_find(dictionary, string_new("]"));
    list_push(&machine->core.data, delimiter);
    machine = parse_until_word(machine);
    quote = list_pop(&machine->core.data);
    machine->core.data = list_new(list_new_1(quote), machine->core.data);
    return machine;
}

machine_t *parse_definition(machine_t *machine) {
    object_t dictionary;
    object_t delimiter;
    object_t name;
    object_t quote;
    dictionary = machine->dictionary;
    delimiter = dictionary_find(dictionary, string_new(";"));
    if (parse_token(&name)) {
        object_t word;
        word = word_new(name, list_nil, boolean_f);
        machine->dictionary = dictionary_insert(dictionary, word);
        list_push(&machine->core.data, delimiter);
        machine = parse_until_word(machine);
        quote = list_pop(&machine->core.data);
        word_set_definition(word, quote);
    } else
        fail();
    machine->core.data = list_new(list_nil, machine->core.data);
    return machine;
}

machine_t *parse_until_word(machine_t *machine) {
    object_t stop_word;
    object_t parse_stack;
    object_t object;
    stop_word = list_pop(&machine->core.data);
    parse_stack = list_nil;
label_scan_loop:
    if (parse_scan(machine, &object)) {
        if (object_is_word(object)) {
            if (object_eq(stop_word, object))
                goto label_done;
            if (parse_is_parsing_word(object)) {
                object_t sub;
                machine = machine_execute(machine, object);
                sub = list_head(machine->core.data);
                machine->core.data = list_tail(machine->core.data);
                parse_stack = list_append(sub, parse_stack);
                goto label_scan_loop;
            }
        }
        parse_stack = list_new(object, parse_stack);
        goto label_scan_loop;
    }
label_done:
    list_push(&machine->core.data, list_reverse(parse_stack));
    return machine;
}

int parse_still_parsing() {
    return parse_line < parse_line_count;
}

int parse_still_parsing_line() {
    return parse_column < strlen(parse_text[parse_line]);
}

int parse_column_is_blank() {
    char c;
    c = parse_text[parse_line][parse_column];
    switch (c) {
        case ' ': return 1;
        case '\n': return 1;
        default: return 0;
    }
}

void parse_skip_blank() {
    char *line;
    size_t len;
    line = parse_text[parse_line];
    len = strlen(line);
    while (parse_column < len && parse_column_is_blank())
        ++parse_column;
}

void parse_skip_word() {
    char *line;
    size_t len;
    line = parse_text[parse_line];
    len = strlen(line);
    while (parse_column < len && !parse_column_is_blank())
        ++parse_column;
}

void parse_next_line() {
    ++parse_line;
    parse_column = 0;
}

int parse_token(object_t *token) {
label_top:
    if (parse_still_parsing()) {
        parse_skip_blank();
        if (parse_still_parsing_line()) {
            int begin, end, i;
            char buffer[PARSE_LINE_MAX];
            begin = parse_column;
            parse_skip_word();
            end = parse_column;
            for (i = 0; begin + i < end; ++i)
                buffer[i] = parse_text[parse_line][begin + i];
            buffer[end - begin] = '\0';
            *token = string_new(buffer);
            return 1;
        } else {
            parse_next_line();
            goto label_top;
        }
    } else
        return 0;
}

int parse_scan(machine_t *machine, object_t *object) {
    object_t token;
    int number;
    char *end;
    if (!parse_token(&token))
        return 0;
    number = (int)strtol(string_unbox(token), &end, 10);
    if (end == string_unbox(token)) {
        object_t dictionary;
        dictionary = machine->dictionary;
        *object = dictionary_find(dictionary, token);
        return 1;
    } else {
        *object = fixnum_new(number);
        return 1;
    }
}

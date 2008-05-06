/* public domain */

#include <stdlib.h>
#include <stdio.h>
#include "/usr/include/string.h"
#include "fail.h"
#include "object.h"
#include "string.h"
#include "list.h"
#include "fixnum.h"
#include "word.h"
#include "dictionary.h"
#include "machine.h"
#include "parse.h"

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

object_t parse_file(machine_t *machine, const char *filename) {
    FILE *file;
    object_t phony_word;
    file = fopen(filename, "r");
    if (file == 0)
        fail();
    parse_line_count = 0;
    while (fgets(parse_line_buffer, PARSE_LINE_MAX, file) != 0) {
        parse_text[parse_line_count] =
            (char *)malloc(strlen(parse_line_buffer));
        strcpy(parse_text[parse_line_count], parse_line_buffer);
        ++parse_line_count;
    }
    parse_line = 0;
    parse_column = 0;
    phony_word = word_new(string_new(""), list_nil);
    return parse_until_word(machine, phony_word);
}

int parse_is_parsing_word(object_t word) {
    return (   0 == strcmp(string_unbox(word_name(word)), "[")
            || 0 == strcmp(string_unbox(word_name(word)), ":"));
}

machine_t *parse_quote(machine_t *machine) {
    object_t dictionary;
    object_t delimiter;
    object_t quote;
    dictionary = machine->dictionary;
    delimiter = dictionary_find(dictionary, string_new("]"));
    quote = parse_until_word(machine, delimiter);
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
        quote = parse_until_word(machine, delimiter);
        word = word_new(name, quote);
        machine->dictionary = dictionary_insert(dictionary, word);
        return machine;
    } else
        fail();
}

object_t parse_until_word(machine_t *machine, object_t stop_word) {
    object_t parse_stack;
    object_t object;
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
    return list_reverse(parse_stack);
}

void parse_define(object_t word, object_t quote) {
    word_set_definition(word, quote);
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

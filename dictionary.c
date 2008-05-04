#include <stdlib.h>
#include "/usr/include/string.h"
#include "object.h"
#include "dictionary.h"
#include "list.h"
#include "string.h"
#include "word.h"

object_t dictionary_new() {
    return list_nil;
}

object_t dictionary_insert(object_t self, object_t word) {
    return list_new(list_new_2(word_name(word), word), self);
}

object_t dictionary_find(object_t self, object_t name) {
    object_t entry;
    char *name_unboxed;
    name_unboxed = string_unbox(name);
label_top:
    if (object_eq(self, list_nil))
        exit(1);
    entry = list_head(self);
    if (!strcmp(name_unboxed, string_unbox(list_head(entry))))
        return list_nth(entry, 1);
    else {
        self = list_tail(self);
        goto label_top;
    }
}

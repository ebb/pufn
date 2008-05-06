/* public domain */

#include <stdlib.h>
#include "/usr/include/string.h"
#include "fail.h"
#include "object.h"
#include "string.h"

object_t string_new(const char *str) {
    object_t self;
    size_t len;
    len = strlen(str);
    self.tag = OBJECT_STRING_TAG;
    self.data.pointer = malloc(len + 1);
    strcpy((char *)self.data.pointer, str);
    return self;
}

char *string_unbox(object_t self) {
    if (!object_is_string(self))
        fail();
    return (char *)self.data.pointer;
}

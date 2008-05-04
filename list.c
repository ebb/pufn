#include <stdlib.h>
#include "object.h"
#include "list.h"

object_t list_new(object_t head, object_t tail) {
    object_t self;
    self.tag = OBJECT_LIST_TAG;
    self.data.pointer = malloc(2 * sizeof(object_t));
    ((object_t *)self.data.pointer)[0] = head;
    ((object_t *)self.data.pointer)[1] = tail;
    return self;
}

object_t *list_unbox(object_t self) {
    if (!object_is_list(self))
        exit(1);
    return (object_t *)self.data.pointer;
}

object_t list_nil;

int list_empty(object_t self) {
    return object_eq(list_nil, self);
}

object_t list_new_1(object_t a) {
    return list_new(a, list_nil);
}

object_t list_new_2(object_t a, object_t b) {
    return list_new(a, list_new_1(b));
}

object_t list_new_3(object_t a, object_t b, object_t c) {
    return list_new(a, list_new_2(b, c));
}

object_t list_new_4(object_t a, object_t b, object_t c, object_t d) {
    return list_new(a, list_new_3(b, c, d));
}

object_t list_head(object_t self) {
    if (!object_is_list(self))
        exit(1);
    return list_unbox(self)[0];
}

object_t list_tail(object_t self) {
    if (!object_is_list(self))
        exit(1);
    return list_unbox(self)[1];
}

object_t list_reverse_append(object_t src, object_t dst) {
label_top:
    if (object_eq(src, list_nil))
        return dst;
    else {
        dst = list_new(list_head(src), dst);
        src = list_tail(src);
        goto label_top;
    }
}

object_t list_reverse(object_t list) {
    return list_reverse_append(list, list_nil);
}

object_t list_append(object_t a, object_t b) {
    return list_reverse_append(list_reverse(a), b);
}

object_t list_nth(object_t self, int n) {
    if (n < 0)
        exit(1);
label_top:
    if (object_eq(self, list_nil))
        exit(1);
    else if (n == 0)
        return list_head(self);
    else {
        --n;
        self = list_tail(self);
        goto label_top;
    }
}

object_t list_replace_nth(object_t self, int n, object_t elt) {
    object_t acc;
    if (n < 0)
        exit(1);
    acc = list_nil;
label_top:
    if (object_eq(self, list_nil))
        exit(1);
    else if (n == 0)
        return list_reverse_append(acc, list_new(elt, list_tail(self)));
    else {
        --n;
        acc = list_new(list_head(self), acc);
        self = list_tail(self);
        goto label_top;
    }
}

void list_initialize() {
    object_t tmp;
    list_nil = list_new(tmp, tmp);
    list_unbox(list_nil)[0] = list_nil;
    list_unbox(list_nil)[1] = list_nil;
}

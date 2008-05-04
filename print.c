#include <stdlib.h>
#include <stdio.h>
#include "object.h"
#include "print.h"
#include "fixnum.h"
#include "word.h"
#include "list.h"
#include "block.h"
#include "string.h"

void print_object(object_t root) {
    switch (root.tag) {
        case OBJECT_FIXNUM_TAG:
            printf("%d", fixnum_unbox(root));
            break;
        case OBJECT_WORD_TAG:
            printf("%s", string_unbox(word_name(root)));
            break;
        case OBJECT_LIST_TAG:
            printf("[ ");
            while (!object_eq(root, list_nil)) {
                print_object(list_head(root));
                printf(" ");
                root = list_tail(root);
            }
            printf("]");
            break;
        case OBJECT_BLOCK_TAG:
            printf("0x%x", block_unbox(root));
            break;
    }
}

/* public domain */

#include <stdlib.h>
#include <stdio.h>
#include "object.h"
#include "print.h"
#include "fixnum.h"
#include "word.h"
#include "list.h"
#include "boolean.h"
#include "string.h"
#include "block.h"
#include "machine.h"
#include "primitive.h"
#include "wrapper.h"

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
        case OBJECT_STRING_TAG:
            printf("\"%s\"", string_unbox(root));
            break;
        case OBJECT_PRIMITIVE_TAG:
            printf("0x%x", (void *)primitive_unbox(root));
            break;
        case OBJECT_BOOLEAN_TAG:
            if (object_eq(root, boolean_t))
                printf("t");
            else
                printf("f");
            break;
        case OBJECT_WRAPPER_TAG:
            printf("\\ %s", string_unbox(word_name(wrapper_unbox(root))));
            break;
        case OBJECT_BLOCK_TAG:
            printf("0x%x", block_unbox(root));
            break;
        default:
            fail();
    }
}

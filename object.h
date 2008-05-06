/* public domain */

typedef struct {
    int tag;
    union {
        int number;
        void *pointer;
    } data;
} object_t;

enum {
    OBJECT_FIXNUM_TAG,
    OBJECT_WORD_TAG,
    OBJECT_LIST_TAG,
    OBJECT_PRIMITIVE_TAG,
    OBJECT_STRING_TAG,
    OBJECT_BOOLEAN_TAG,
    OBJECT_WRAPPER_TAG,
    OBJECT_BLOCK_TAG
};

int object_eq(object_t, object_t);

void object_free(object_t);

int object_is_fixnum(object_t);
int object_is_word(object_t);
int object_is_list(object_t);
int object_is_primitive(object_t);
int object_is_string(object_t);
int object_is_boolean(object_t);
int object_is_wrapper(object_t);
int object_is_block(object_t);

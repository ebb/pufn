/* public domain */

void list_initialize();
object_t list_new(object_t, object_t);
object_t *list_unbox(object_t);
object_t list_nil;
int list_empty(object_t);
object_t list_new_1(object_t);
object_t list_new_2(object_t, object_t);
object_t list_new_3(object_t, object_t, object_t);
object_t list_new_4(object_t, object_t, object_t, object_t);
object_t list_head(object_t);
object_t list_tail(object_t);
object_t list_reverse_append(object_t, object_t);
object_t list_reverse(object_t);
object_t list_append(object_t, object_t);
object_t list_nth(object_t, int);
object_t list_replace_nth(object_t, int, object_t);
object_t list_pop(object_t *);
void list_push(object_t *, object_t);
object_t list_take(int, object_t);

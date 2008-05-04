typedef machine_t *(*primitive_t)(machine_t *);
object_t primitive_new(const char *, primitive_t);

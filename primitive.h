typedef machine_t *(*primitive_t)(machine_t *);
object_t primitive_new(primitive_t);
primitive_t *primitive_unbox(object_t);
machine_t *primitive_execute(object_t, machine_t *);

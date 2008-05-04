typedef struct {
    object_t call, data;
} machine_core_t;

typedef struct {
    machine_core_t core;
    object_t retain;
    object_t dictionary;
} machine_t;

void machine_initialize();
machine_t *machine_new(object_t);
machine_t *machine_copy(machine_t *);
machine_t *machine_call(machine_t *, object_t);
machine_t *machine_execute(machine_t *, object_t);

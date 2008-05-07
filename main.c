/* public domain */

#include <stdlib.h>
#include <stdio.h>
#include "object.h"
#include "list.h"
#include "boolean.h"
#include "string.h"
#include "machine.h"
#include "parse.h"
#include "prelude.h"
#include "print.h"

int main (int argc, const char *argv[]) {
    machine_t *machine;
    object_t quote;
    int i;
    boolean_initialize();
    list_initialize();
    machine_initialize();
    prelude_initialize();
    machine = machine_new(prelude);
    for (i = 1; i < argc; ++i) {
        list_push(&machine->core.data, string_new(argv[i]));
        machine = parse_file(machine);
        quote = list_pop(&machine->core.data);
        machine = machine_call(machine, quote);
    }
    print_object(machine->core.data);
    printf("\n");
    return 0;
}

/* public domain */

#include <stdlib.h>
#include <stdio.h>
#include "object.h"
#include "list.h"
#include "boolean.h"
#include "machine.h"
#include "parse.h"
#include "prelude.h"
#include "print.h"

int main (int argc, const char *argv[]) {
    machine_t *machine;
    object_t quote;
    boolean_initialize();
    list_initialize();
    machine_initialize();
    prelude_initialize();
    machine = machine_new(prelude);
    while (--argc > 0) {
        quote = parse_file(machine, argv[argc]);
        machine = machine_call(machine, quote);
    }
    print_object(machine->core.data);
    printf("\n");
    return 0;
}

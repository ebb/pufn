#include <stdlib.h>
#include <assert.h>
#include "fail.h"

void fail() {
    assert(0);
    exit(1);
}

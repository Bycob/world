#include "common.h"

#include <cstdlib>

// TODO this is bad ! Dont do this !!!!!
PEACE_EXPORT void freePtrs(int size, void **array) {
    for (int i = 0; i < size; ++i) {
        free(array[i]);
    }
}
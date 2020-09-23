#include <stdlib.h>
#include "grain.h"

grain *grain_new(int start_sample, int end_sample) {
    grain *x = (grain *)malloc(sizeof(grain));
    x->start_sample = start_sample;
    x->end_sample = end_sample;
    x->current_pos = 0;
    
    return x;
}

void grain_free(grain *x) {
    free(x);
}

#include <stdlib.h>
#include "grain_scheduler.h"
#include "grain.h"

grain_scheduler *grain_scheduler_new(t_word *sample, int sample_length) {
    grain_scheduler *x = (grain_scheduler *)malloc(sizeof(grain_scheduler));
    x->sample = sample;
    x->sample_length = sample_length;
    x->offset = 0;
    x->grain_length = 0;
    x->num_grains = 0;
    
    return x;
}

void grain_scheduler_free(grain_scheduler *x) {
    free(x);
}

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length) {
    x->offset = offset;
    x->grain_length = num_grains;
    x->num_grains = grain_length;
}

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out) {
    *out = x->sample[sample_pos].w_float;
}

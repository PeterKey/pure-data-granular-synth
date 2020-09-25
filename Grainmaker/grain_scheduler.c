#include <stdlib.h>
#include "grain_scheduler.h"
#include "grain.h"

grain_scheduler *grain_scheduler_new(t_word *src_sample, int src_sample_length) {
    grain_scheduler *x = (grain_scheduler *)malloc(sizeof(grain_scheduler));
    x->src_sample = src_sample;
    x->src_sample_length = src_sample_length;
    x->offset = 0;
    x->grain_length = 0;
    x->num_grains = 0;
    
    return x;
}

void grain_scheduler_free(grain_scheduler *x) {
    if (x) {
        while (x->num_grains >= 0) {
            x->num_grains--;
            if ((x->grains+x->num_grains) >= 0) {
                grain_free((grain *) (x->grains+x->num_grains));
            }
        }
    }
    free(x);
}

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length) {
    x->offset = offset;
    x->grain_length = num_grains;
    x->num_grains = grain_length;
}

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out) {
    if (!x->grains) {
        x->grains = (t_int *) (calloc(x->num_grains, sizeof(t_int)));
    }

	for (int i = 0; i < x->num_grains; i++){
		if (*(x->grains+i) == 0){
			*(x->grains + i) = (t_int) construct_grain(sample_pos, x->src_sample_length, x->offset, x->grain_length);
		}

	}

	*out = x->src_sample[sample_pos].w_float;
}

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
    free(x);
}

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length) {
    x->offset = offset;
    x->grain_length = num_grains;
    x->num_grains = grain_length;
}

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out) {
	x->grains = (t_int*)(calloc(sizeof(t_int)*x->num_grains));

	for (int i = 0; i < x->num_grains; i++){
		if (*(x->grains+i) == 0){
			*(x->grains + i) = (t_int) construct_grain(sample_pos, x->src_sample_length, x->offset, x->grain_length);
		}

	}

	*out = x->src_sample[sample_pos].w_float;
}
grain *construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length){
	int range_start = sample_pos - offset;
	if (range_start < 0) range_start = 0;
	int range_end = sample_pos + offset;
	int max_range = src_sample_length - grain_length;
	if (range_end > max_range) range_end = max_range;

	int start_sample = range_start + rand() % (range_end - range_start);
	grain *new_grain = grain_new(start_sample, (start_sample + grain_length));
	return new_grain;

}

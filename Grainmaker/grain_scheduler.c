#include <stdlib.h>
#include <time.h>
#include "grain_scheduler.h"
#include "grain.h"

grain_scheduler *grain_scheduler_new(t_word *src_sample, int src_sample_length) {
    grain_scheduler *x = (grain_scheduler *)malloc(sizeof(grain_scheduler));
    x->src_sample = src_sample;
    x->src_sample_length = src_sample_length;
    x->offset = 0;
    x->grain_length = 0;
    x->num_grains = 0;
    x->grain_spread = 1;
    
    return x;
}

void grain_scheduler_free(grain_scheduler *x) {
    if (x) {
        free(x->grains);
    }
    free(x);
}

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length) {
    if (x->offset != offset) x->offset = offset;
    if (x->grain_length != grain_length) x->grain_length = grain_length;
    if (x->num_grains != num_grains) x->num_grains = num_grains;
    
    if (!x->grains) {
        x->grains = (grain *) (calloc(x->num_grains, sizeof(grain)));
        post("constructed grains");
        
        for (int i = 0; i < x->num_grains; i++) {
            x->grains[i] = construct_grain(0, x->src_sample_length, x->offset, x->grain_length);
        }

        x->grain_pauses = (int *) calloc(x->num_grains, sizeof(int));

        srand((unsigned int) time(NULL));
        for (int i = 1; i < x->num_grains; i++) {
            x->grain_pauses[i] = (int) (((float)(rand())/(float)(RAND_MAX)) * (float)(x->grain_length + x->grain_spread));
        }
    }
}

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out) {

//	for (int i = 0; i < x->num_grains; i++){
//        if (!x->grains[i]){
//			*(x->grains + i) = construct_grain(sample_pos, x->src_sample_length, x->offset, x->grain_length);
//		}
//	}

	*out = x->src_sample[sample_pos].w_float;
}



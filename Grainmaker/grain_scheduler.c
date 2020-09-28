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
            x->grains[i] = construct_grain(0, 0, 0, 0);
        }

        x->grain_pauses = (int *) calloc(x->num_grains, sizeof(int));

        srand((unsigned int) time(NULL));
        for (int i = 1; i < x->num_grains; i++) {
            x->grain_pauses[i] = (int) (((float)(rand())/(float)(RAND_MAX)) * (float)(x->grain_length + x->grain_spread));
        }
    }
}

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out) {

    // if grains smaller then num_grains check if you can resize
    int shrink_grains_array = (sizeof(x->grains) / x->num_grains) - x->num_grains;
    
    int current_num_grains = x->num_grains;
    for (int i = 0; i < current_num_grains; i++) {

        // If Grain waits
        if (x->grain_pauses[i] > 0) {
            if (shrink_grains_array > 0) {
                // TODO: Shrink Array
                shrink_grains_array--;
                current_num_grains--;
            } else {
                x->grain_pauses[i]--;
            }
        } else { // Grain does not wait
            // Check if grain is still running
            if (x->grains[i].current_sample <= x->grains[i].end_sample) {
                
                //Add Sample
                // TODO: Convolve correctly
                *out = *out + x->src_sample[x->grains[i].current_sample].w_float;
                x->grains[i].current_sample++;
                
                // If Grain has ended now pause it
                if (x->grains[i].current_sample > x->grains[i].end_sample) {
                    x->grain_pauses[i] = x->grain_spread;
                }
            } else { // If grain has finished and no more pause create new
                x->grains[i] = construct_grain(sample_pos, x->src_sample_length, x->offset, x->grain_length);
            }
        }
    }

	*out = x->src_sample[sample_pos].w_float;
}



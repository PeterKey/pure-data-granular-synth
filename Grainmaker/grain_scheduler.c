#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "grain_scheduler.h"
#include "grain.h"


float gauss(grain x){
    //creates a value to convolve with based on gaussian function f(x)=a*e^⁻(x-b)²/2c²
    //where a = max value (1 for us so not needed), b = position of max value, c = 6/length of values
    if (x.grain_size == 0) return 0.0;
    float p = ((x.current_sample - x.start_sample) - (x.grain_size / 2));
    float c = 6 / x.grain_size;
    float e = - pow(p, 2) / 2 * pow(c, 2);
    float g_val = expf(e);

    return g_val;
}

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
        free(x->grain_pauses);
    }
    free(x);
}

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length) {
    if (x->offset != offset) x->offset = offset;
    if (x->grain_length != grain_length) x->grain_length = grain_length;
    if (x->num_grains != num_grains) x->num_grains = num_grains;
    
    if (!x->grains) {
        x->grains = (grain *) (calloc(x->num_grains, sizeof(grain)));
        x->current_num_grains = x->num_grains;
        
        for (int i = 0; i < x->num_grains; i++) {
            x->grains[i] = construct_grain(0, 0, 0, 0);
        }

        x->grain_pauses = (int *) calloc(x->num_grains, sizeof(int));

        srand((unsigned int) time(NULL));
        x->grain_pauses[0] = 0;
        for (int i = 1; i < x->num_grains; i++) {
            x->grain_pauses[i] = (int) (((float)(rand())/(float)(RAND_MAX)) * (float)(x->grain_length + x->grain_spread));
        }
    }
    
    // Extend grains and grain_pauses if num_grains increases
    if (x->current_num_grains < num_grains) {
        grain *new_grains = (grain *) (calloc(num_grains, sizeof(grain)));
        int *new_grain_pauses = (int *) calloc(num_grains, sizeof(int));

        for (int i = 0; i < x->current_num_grains; i++) {
            new_grains[i] = x->grains[i];
            new_grain_pauses[i] = x->grain_pauses[i];
        }
        
        for (int i = x->current_num_grains; i < num_grains; i++) {
            new_grains[i] = construct_grain(0, 0, 0, 0);
            new_grain_pauses[i] = (int) (((float)(rand())/(float)(RAND_MAX)) * (float)(x->grain_length + x->grain_spread));
        }
        
        free(x->grains);
        free(x->grain_pauses);
        x->grains = new_grains;
        x->grain_pauses = new_grain_pauses;
        x->current_num_grains = num_grains;
    }
}

void kick_grain(grain_scheduler *x, int index) {
    grain *new_grains = (grain *) (calloc(x->current_num_grains - 1, sizeof(grain)));
    int *new_grain_pauses = (int *) calloc(x->current_num_grains - 1, sizeof(int));
    
    bool grain_kicked = false;
    for (int i = 0; i < x->current_num_grains; i++) {
        if (i == index) {
            grain_kicked = true;
            continue;
        } else {
            new_grains[i - (int) grain_kicked] = x->grains[i];
            new_grain_pauses[i - (int) grain_kicked] = x->grain_pauses[i];
        }
    }
    
    free(x->grains);
    free(x->grain_pauses);
    x->grains = new_grains;
    x->grain_pauses = new_grain_pauses;
    x->current_num_grains--;
}

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out) {
    // if grains smaller then num_grains check if you can resize
    int shrink_grains_array = x->current_num_grains - x->num_grains;
    
    float output = 0.0;

    for (int i = 0; i < x->current_num_grains; i++) {

        // If Grain waits
        if (x->grain_pauses[i] > 0) {
            if (shrink_grains_array > 0) {
                kick_grain(x, i);
                shrink_grains_array--;
            } else {
                x->grain_pauses[i]--;
            }
        }
        else { // Grain does not wait
            // Check if grain is still running
            if (x->grains[i].current_sample <= x->grains[i].end_sample) {

                //Add Sample
            	float conv_value = gauss(x->grains[i]);
                output = output + (float) ((x->src_sample[x->grains[i].current_sample].w_float * conv_value) / (float) (x->current_num_grains));
                x->grains[i].current_sample++;

                // If Grain has ended now pause it
                if (x->grains[i].current_sample > x->grains[i].end_sample) {
                    x->grain_pauses[i] = x->grain_spread;
                }
            }
            else { // If grain has finished and no more pause create new
                x->grains[i] = construct_grain(sample_pos, x->src_sample_length, x->offset, x->grain_length);
            }
        }
    }

    *out = output;
}


float gauss(grain x){
	//creates a value to convolve with based on gaussian function f(x)=a*e^⁻(x-b)²/2c²
	//where a = max value (1 for us so not needed), b = position of max value, c = 6/length of values
    if (x.grain_size == 0) return 0.0;
    float p = ((x.current_sample - x.start_sample) - (x.grain_size / 2));
	float c = 20 / x.grain_size;
	float e = - pow(p, 2) / 2 * pow(c, 2);
    float g_val = expf(e);

	return g_val;
}


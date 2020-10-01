/**
*@file grain.c
*@author Peter Gorzo, Jonas Koerwer, Claudio Albrecht, Roman Schweikert<br>
*Audiocommunication Group, Technical University Berlin <br>
*Real-time audio programming in C, SoSe2020 <br>
*A simple Grain sampler <br>
*<br>
*@brief Functions to create grains for the grain_scheduler object. <br>
*<br>
*Grain includes the methods to initialize and construct individual grains. <br>
**<br>
*/




#include <stdlib.h>
#include <time.h>
#include "m_pd.h"
#include "grain.h"
#include "grain_scheduler.h"

int minimumInt(int a, int b) {
    return a<b ? a : b;
}

/**
 *@related grain
 *@brief Creates a new grain object <br>
 *@param start_sample The position in the source sample where the grain starts <br>
 *@param end_sample The position in the source sample where the grain ends <br>
 *@return Returns a grain
 */

grain grain_new(int start_sample, int end_sample) {
    grain x;
    x.start_sample = start_sample;
    x.end_sample = end_sample;
    x.current_sample = start_sample;
    x.grain_size = end_sample - start_sample;
    
    return x;
}

grain construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length){
    //calculates a range in which to create the grain
	int range_start = sample_pos - offset;
    if (range_start < 0) range_start = 0;
    int range_end = sample_pos + offset;
    int max_range = src_sample_length - grain_length;
    if (range_end > max_range) range_end = max_range;
    //sets the sample start at a random position within the range
    int range_size = range_end - range_start;
    int start_sample;
    if (range_size > 0) {
        start_sample = (int) range_start + rand() % (range_end - range_start);
    } else {
        start_sample = 0;
    }
    int end_sample = minimumInt(src_sample_length, start_sample + grain_length);
    grain new_grain = grain_new(start_sample, end_sample);
    
    return new_grain;
}

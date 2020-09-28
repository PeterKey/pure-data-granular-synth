#include <stdlib.h>
#include "grain.h"
#include <m_pd.h>

grain grain_new(int start_sample, int end_sample) {
    grain x;
    x.start_sample = start_sample;
    x.end_sample = end_sample;
    x.current_pos = 0;
    
    return x;
}

grain construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length){
    int range_start = sample_pos - offset;
    if (range_start < 0) range_start = 0;
    int range_end = sample_pos + offset;
    int max_range = src_sample_length - grain_length;
    if (range_end > max_range) range_end = max_range;

    int start_sample = range_start + rand() % (range_end - range_start);
    grain new_grain = grain_new(start_sample, (start_sample + grain_length));
    
    post("new grain constructed");
    return new_grain;
}

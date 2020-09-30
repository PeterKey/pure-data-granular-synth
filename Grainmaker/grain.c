#include <stdlib.h>
#include <time.h>
#include "m_pd.h"
#include "grain.h"
#include "grain_scheduler.h"

int minimumInt(int a, int b) {
    return a<b ? a : b;
}

grain grain_new(int start_sample, int end_sample) {
    grain x;
    x.start_sample = start_sample;
    x.end_sample = end_sample;
    x.current_sample = start_sample;
    x.grain_size = end_sample - start_sample;
    
    return x;
}

grain construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length){
    int range_start = sample_pos - offset;
    if (range_start < 0) range_start = 0;
    int range_end = sample_pos + offset;
    int max_range = src_sample_length - grain_length;
    if (range_end > max_range) range_end = max_range;

    int range_size = range_end - range_start;
    int start_sample;
    if (range_size > 0) {
        srand((unsigned int) time(NULL));
        start_sample = (int) range_start + rand() % (range_end - range_start);
    } else {
        start_sample = 0;
    }
    int end_sample = minimumInt(src_sample_length, start_sample + grain_length);
    grain new_grain = grain_new(start_sample, end_sample);
    
    return new_grain;
}

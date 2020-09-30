#ifndef grain_h
#define grain_h

typedef struct grain {
    int start_sample,
        end_sample,
        current_sample,
        grain_size;
}grain;

grain construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length);

#endif /* grain_h */

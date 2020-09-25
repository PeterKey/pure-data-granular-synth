#ifndef grain_h
#define grain_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct grain {
    int start_sample,
        end_sample,
        current_pos;
}grain;

grain *grain_new(int start_sample, int end_sample);

void grain_free(grain *x);

grain *construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length);

#ifdef __cplusplus
}
#endif

#endif /* grain_h */

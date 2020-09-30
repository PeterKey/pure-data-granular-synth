#ifndef grain_h
#define grain_h

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef struct grain {
    int start_sample,
        end_sample,
        current_sample,
        grain_size;
}grain;

grain construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length);

//#ifdef __cplusplus
//}
//#endif

#endif /* grain_h */

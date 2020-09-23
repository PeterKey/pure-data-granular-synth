#ifndef grain_scheduler_h
#define grain_scheduler_h

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

#ifdef __cplusplus
}
#endif

#endif /* grain_scheduler_h */

#ifndef grain_scheduler_h
#define grain_scheduler_h

#include <stdio.h>
#include "m_pd.h"
#include "grain.h"

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef struct grain_scheduler
{
    t_word      *src_sample;
    grain       *grains;
    int         *grain_pauses,
                src_sample_length,
                offset,
                num_grains,
                grain_length,
                grain_spread;
    
}grain_scheduler;

grain_scheduler *grain_scheduler_new(t_word *src_sample, int src_sample_length);

void grain_scheduler_free(grain_scheduler *x);

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length);

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out);

float gauss(grain x);

void printInt(int toPrint);

//#ifdef __cplusplus
//}
//#endif

#endif /* grain_scheduler_h */

/**
*@file grain_scheduler.h
*@author Peter Gorszo, Jonas Koerwer, Claudio Albrecht, Roman <br>
*Audiocommunication Group, Technical University Berlin <br>
*Real-time audio programming in C, SoSe2020 <br>
*A simple Grain sampler <br>
*<br>
*@brief Object to handle the creation, deletion and scheduling of grains <br>
*<br>
*Grain_scheduler manages all grains for the grainmaker~.c pd external. It constructs grains, <br>
*sets the time between grains, and computes the output. <br>
*<br>
*/

#ifndef grain_scheduler_h
#define grain_scheduler_h

#include <stdio.h>
#include "m_pd.h"
#include "grain.h"

/**
 * @struct grain_scheduler
 * @brief A structure for a gain_scheduler object <br>
 * @var grain_scheduler::src_sample The source Sample from which the grains are to be constructed. <br>
 * @var grain_scheduler::grains A pointer to the array in which the grains are stored. <br>
 * @var grain_scheduler::grain_pauses a pointer to the array in which the pause time for each grain is stored. <br>
 * @var grain_scheduler::src_sample_length The length of the source sample in samples. <br>
 * @var grain_scheduler::offset The offset defines the range in which grains can be constructed. <br>
 * @var grain_scheduler::num_grains The number of grains as defined from outside the external by the user. <br>
 * @var grain_scheduler::current_num_grains The current number of grains that have been created and are <br>
 * being played or waiting to be played. <br>
 * @var grain_scheduler::grain_length The length of a grain in samples, as defined from outside the external by the user. <br>
 * @var grain_scheduler::grain_spread Controls the amount of time between the playing of grains.
 */

typedef struct grain_scheduler
{
    t_word      *src_sample;
    grain       *grains;
    int         *grain_pauses,
                src_sample_length,
                offset,
                num_grains,
                current_num_grains,
                grain_length,
                grain_spread;
    
}grain_scheduler;

grain_scheduler *grain_scheduler_new(t_word *src_sample, int src_sample_length);

void grain_scheduler_free(grain_scheduler *x);

void grain_scheduler_set_props(grain_scheduler *x, int offset, int num_grains, int grain_length);

void grain_scheduler_perform(grain_scheduler *x, int sample_pos, t_sample *out);

float gauss(grain x);

void printInt(int toPrint);

#endif /* grain_scheduler_h */

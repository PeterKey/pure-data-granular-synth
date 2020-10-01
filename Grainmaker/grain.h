/**
*@file grain.h
*@author Peter Gorzo, Jonas Koerwer, Claudio Albrecht, Roman Schweikert<br>
*Audiocommunication Group, Technical University Berlin <br>
*Real-time audio programming in C, SoSe2020 <br>
*A simple Grain sampler <br>
*<br>
*@brief Object to handle the individual grains of the grainmaker~ object. <br>
*<br>
*Grain includes the methods to initialize and construct individual grains. <br>
**<br>
*/

#ifndef grain_h
#define grain_h

/**
 * @struct grain
 * @brief The struct of a grain <br>
 * @var start_sample  <br>
 * @var end_sample   <br>
 * @var current_sample <br>
 * @var grain_size  <br>
 */

typedef struct grain {
    int start_sample, /**<The position in the source sample where the grain starts */
        end_sample, /**<The position in the source sample where the grain ends */
        current_sample, /**<The position in the source sample where the grain is currently playing  */
        grain_size; /**<The size of the grain, calculated by subtracting start_sample from end_sample */
}grain;

/**
 * @related grain
 * @brief Creates a grain to be used by grain_scheduler <br>
 * @param sample_pos The position of the playhead as defined from outside. <br>
 * @param src_sample_length The length of the source sample <br>
 * @þaram offset The offset around sample_pos in which grains can be constructed <br>
 * @param grain_length The length of the individual grains
 * The construct_grain function creates grains in an area of the source sample that is defined by sample_pos and offset. <br>
 * It sets all grain variables. <br>
 * @return A grain object <br>
 */

grain construct_grain(int sample_pos, int src_sample_length, int offset, int grain_length);

#endif /* grain_h */

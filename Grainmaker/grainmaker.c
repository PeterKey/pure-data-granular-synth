//
//  grainmaker.c
//  Grainmaker
//
//  Created by Peter K. Gorzo on 27.08.20.
//  Copyright © 2020 Realtime Audio. All rights reserved.
//

#include "m_pd.h"

static t_class *grainmaker_class;

typedef struct _grainmaker {
    t_object    x_obj;
    t_int       offset, grain_length;
    t_float     head_pos, playback_speed;
    // sample, grain_envelope, (grain_start), direction
    t_array     *sample;
    t_inlet     *in_sample, *in_offset, *in_grain_length, *in_head_pos;
//    t_outlet    *out_A, *out_B, *out_synch, *out_count;
}t_grainmaker;

void *grainmaker_new(t_floatarg f1, t_floatarg f2) {
    t_grainmaker *x = (t_grainmaker *)pd_new(grainmaker_class);
    
    x->in_sample = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_list, gensym("sample"));

//    x->out_A = outlet_new(&x->x_obj, &s_bang);
//    x->out_B = outlet_new(&x->x_obj, &s_bang);
//    x->out_synch = outlet_new(&x->x_obj, &s_bang);
//    x->out_count = outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;
}

void grainmaker_onBangMsg(t_grainmaker *x) {
    post("[grainmaker]:");
    if (sizeof(x->in_sample) > 0) {
        post("no array");
    } else {
        post("array");
    }
}

void grainmaker_free(t_grainmaker *x) {
    inlet_free(x->in_sample);

//    outlet_free(x->out_A);
//    outlet_free(x->out_B);
//    outlet_free(x->out_synch);
//    outlet_free(x->out_count);
}

void grainmaker_setup(void) {
    grainmaker_class = class_new(gensym("grainmaker"),
                                 (t_newmethod)grainmaker_new,
                                 (t_method)grainmaker_free,
                                 sizeof(t_grainmaker),
                                 CLASS_DEFAULT,
                                 0);
    
    class_addbang(grainmaker_class, (t_method)grainmaker_onBangMsg);
}

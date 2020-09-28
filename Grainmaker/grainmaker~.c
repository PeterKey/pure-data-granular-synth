//
//  grainmaker.c
//  Grainmaker
//
//  Created by Peter K. Gorzo on 27.08.20.
//  Copyright © 2020 Realtime Audio. All rights reserved.
//

#include "m_pd.h"
#include <stdlib.h>
#include "grain_scheduler.h"

static t_class *grainmaker_tilde_class;

typedef struct _grainmaker_tilde {
    t_object        x_obj;
    t_word          *x_sample;
    int             x_sample_length,
                    offset,
                    num_grains,
                    grain_length;

    t_symbol        *x_arrayname;
    t_float         f;
    
    grain_scheduler *x_scheduler;
    
    t_inlet         *in_offset,
                    *in_num_grains,
                    *in_grain_length;
    
    t_outlet        *out;
    
//    t_int       offset, grain_length;
//    t_float     head_pos, playback_speed;
//    // sample, grain_envelope, (grain_start), direction
//    t_array     *sample;
//    t_inlet     *in_sample, *in_offset, *in_grain_length, *in_head_pos;
//    t_outlet    *out_A, *out_B, *out_synch, *out_count;
    
}t_grainmaker_tilde;

void *grainmaker_tilde_new(t_symbol *arrayname) {
    t_grainmaker_tilde *x = (t_grainmaker_tilde *)pd_new(grainmaker_tilde_class);
    
    x->x_arrayname = arrayname;
    x->x_sample = 0;
    x->x_sample_length = 0;
    x->f = 0;
    
    
    x->in_offset = inlet_new(&x->x_obj,  &x->x_obj.ob_pd, &s_float, gensym("offset"));
    x->in_num_grains = inlet_new(&x->x_obj,  &x->x_obj.ob_pd, &s_float, gensym("num_grains"));
    x->in_grain_length = inlet_new(&x->x_obj,  &x->x_obj.ob_pd, &s_float, gensym("grain_length"));
    
    x->out = outlet_new(&x->x_obj, &s_signal);
    
    return (void *)x;
}

void grainmaker_tilde_free(t_grainmaker_tilde *x) {
    grain_scheduler_free(x->x_scheduler);

    inlet_free(x->in_offset);
    inlet_free(x->in_num_grains);
    inlet_free(x->in_grain_length);
    outlet_free(x->out);
}

static t_int *grainmaker_tilde_perform(t_int *w)
{
    t_grainmaker_tilde *x = (t_grainmaker_tilde *)(w[1]);
    t_sample *in = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    int sample_length = x->x_sample_length - 1;;

    if(sample_length <= 0) goto zero;
    if (!x->x_sample) goto zero;
    
    
    // Print all current values of inlets
    if(x->offset < 0) goto zero;
    if(x->num_grains <= 0) goto zero;
    if(x->grain_length <= 0) goto zero;
    
    grain_scheduler_set_props(x->x_scheduler, x->offset, x->num_grains, x->grain_length);


//    int length = snprintf( NULL, 0, "%d", offset );
//    char* str1 = malloc( length + 1 );
//    snprintf( str1, length + 1, "%d", offset );
//    post("offset: ");
//    post(str1);
//    free(str1);
//
//    length = snprintf( NULL, 0, "%d", num_grains );
//    char* str = malloc( length + 1 );
//    snprintf( str, length + 1, "%d", num_grains );
//    post("num_grains: ");
//    post(str);
//    free(str);
//
//    length = snprintf( NULL, 0, "%d", grain_length );
//    char* str2 = malloc( length + 1 );
//    snprintf( str2, length + 1, "%d", grain_length );
//    post("grain_length: ");
//    post(str2);
//    free(str2);

    int i;
    for (i = 0; i < n; i++)
    {

        int sample_pos = *in++;
        if (sample_pos < 0) {
            sample_pos = 0;
        }
        else if (sample_pos > sample_length) {
            sample_pos = sample_length;
        }
        
        grain_scheduler_perform(x->x_scheduler, sample_pos, out++);
    }
    return (w+5);
 zero:
    while (n--) *out++ = 0;

    return (w+5);
}

static void grainmaker_tilde_set(t_grainmaker_tilde *x)
{
    // Copied from tabread~ external
    t_garray *a;
    
    if (!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
    {
        if (x->x_arrayname->s_name)
            pd_error(x, "grainmaker~: %s: no such array", x->x_arrayname->s_name);
        x->x_sample = 0;
    }
    else if (!garray_getfloatwords(a, &x->x_sample_length, &x->x_sample))
    {
        pd_error(x, "%s: bad template for grainmaker~", x->x_arrayname->s_name);
        x->x_sample = 0;
    }
    else {
        garray_usedindsp(a);
        x->x_scheduler = grain_scheduler_new(x->x_sample, x->x_sample_length);
    }
}

static void grainmaker_tilde_dsp(t_grainmaker_tilde *x, t_signal **sp)
{
    grainmaker_tilde_set(x);

//    if(x->offset < 0)
//        post("grainmaker~: please define a positive offset.");
//    if(x->num_grains <= 0)
//        post("grainmaker~: please define a number of grains > 0.");
//    if(x->grain_length <= 0)
//        post("grainmaker~: please define a grain length > 0.");

    dsp_add(grainmaker_tilde_perform, 4, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[0]->s_n);
}

static void grainmaker_tilde_set_offset(t_grainmaker_tilde *x, t_floatarg f) {
    x->offset = (int)(f);
    if(x->offset < 0) x->offset = 0;
    if(x->offset > x->x_sample_length) x->offset = x->x_sample_length;
}

static void grainmaker_tilde_set_num_grains(t_grainmaker_tilde *x, t_floatarg f) {
    x->num_grains = (int)(f);
    if(x->num_grains < 0) x->num_grains = 0;
    if(x->num_grains > 50) x->num_grains = 50;
}

static void grainmaker_tilde_set_grain_length(t_grainmaker_tilde *x, t_floatarg f) {
    x->grain_length = (int)(f);
    if(x->grain_length <= 0) x->grain_length = 1;
    if(x->grain_length > x->x_sample_length) x->grain_length = x->x_sample_length;
}

void grainmaker_tilde_setup(void) {
    grainmaker_tilde_class = class_new(gensym("grainmaker~"),
                                 (t_newmethod)grainmaker_tilde_new,
                                 (t_method)grainmaker_tilde_free,
                                 sizeof(t_grainmaker_tilde),
                                 0,
                                 A_DEFSYM,
                                 0);
    CLASS_MAINSIGNALIN(grainmaker_tilde_class, t_grainmaker_tilde, f);
    class_addmethod(grainmaker_tilde_class, (t_method)grainmaker_tilde_dsp,
        gensym("dsp"), A_CANT, 0);

    class_addmethod(grainmaker_tilde_class,
                    (t_method)grainmaker_tilde_set_offset,
                    gensym("offset"),
                    A_DEFFLOAT,
                    0);
    
    class_addmethod(grainmaker_tilde_class,
                    (t_method)grainmaker_tilde_set_num_grains,
                    gensym("num_grains"),
                    A_DEFFLOAT,
                    0);
    
    class_addmethod(grainmaker_tilde_class,
                    (t_method)grainmaker_tilde_set_grain_length,
                    gensym("grain_length"),
                    A_DEFFLOAT,
                    0);
}

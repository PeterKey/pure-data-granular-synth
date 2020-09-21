//
//  grainmaker.c
//  Grainmaker
//
//  Created by Peter K. Gorzo on 27.08.20.
//  Copyright © 2020 Realtime Audio. All rights reserved.
//

#include "m_pd.h"
#include "stdlib.h"

static t_class *grainmaker_tilde_class;

typedef struct _grainmaker_tilde {
    t_object    x_obj;
    t_word      *x_vec;
    int         x_npoints;
    t_symbol    *x_arrayname;
    t_float     t_f;
    
    
//    t_int       offset, grain_length;
//    t_float     head_pos, playback_speed;
//    // sample, grain_envelope, (grain_start), direction
//    t_array     *sample;
//    t_inlet     *in_sample, *in_offset, *in_grain_length, *in_head_pos;
//    t_outlet    *out_A, *out_B, *out_synch, *out_count;
}t_grainmaker_tilde;

void *grainmaker_tilde_new(t_symbol *s) {
    t_grainmaker_tilde *x = (t_grainmaker_tilde *)pd_new(grainmaker_tilde_class);
    
    x->x_arrayname = s;
    x->x_vec = 0;
    x->t_f = 0;
    outlet_new(&x->x_obj, gensym("signal"));
    
    return (void *)x;
}

static t_int *grainmaker_tilde_perform(t_int *w)
{
    t_grainmaker_tilde *x = (t_grainmaker_tilde *)(w[1]);
    t_sample *in = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    int maxindex;
    t_word *buf = x->x_vec;
    int i;
    
    int length = snprintf( NULL, 0, "%d", n );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", n );
    
    post(str);
    
    free(str);

    maxindex = x->x_npoints - 1;
    if(maxindex<0) goto zero;
    if (!buf) goto zero;

    for (i = 0; i < n; i++)
    {
        int index = *in++;
        if (index < 0)
            index = 0;
        else if (index > maxindex)
            index = maxindex;
        *out++ = buf[index].w_float;
    }
    return (w+5);
 zero:
    while (n--) *out++ = 0;

    return (w+5);
}

void grainmaker_tilde_free(t_grainmaker_tilde *x) {

}

static void grainmaker_tilde_set(t_grainmaker_tilde *x, t_symbol *s)
{
    // Copied from tabread~ external
    t_garray *a;
    x->x_arrayname = s;
    
    if (!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
    {
        if (*s->s_name)
            pd_error(x, "grainmaker~: %s: no such array", x->x_arrayname->s_name);
        x->x_vec = 0;
    }
    else if (!garray_getfloatwords(a, &x->x_npoints, &x->x_vec))
    {
        pd_error(x, "%s: bad template for grainmaker~", x->x_arrayname->s_name);
        x->x_vec = 0;
    }
    else garray_usedindsp(a);
}

static void grainmaker_tilde_dsp(t_grainmaker_tilde *x, t_signal **sp)
{
    grainmaker_tilde_set(x, x->x_arrayname);
    dsp_add(grainmaker_tilde_perform, 4, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[0]->s_n);
}

void grainmaker_tilde_setup(void) {
    grainmaker_tilde_class = class_new(gensym("grainmaker~"),
                                 (t_newmethod)grainmaker_tilde_new,
                                 (t_method)grainmaker_tilde_free,
                                 sizeof(t_grainmaker_tilde),
                                 0,
                                 A_DEFSYM,
                                 0);
    CLASS_MAINSIGNALIN(grainmaker_tilde_class, t_grainmaker_tilde, t_f);
    class_addmethod(grainmaker_tilde_class, (t_method)grainmaker_tilde_dsp,
        gensym("dsp"), A_CANT, 0);
}

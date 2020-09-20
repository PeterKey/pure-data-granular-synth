//
//  grainmaker.c
//  Grainmaker
//
//  Created by Peter K. Gorzo on 27.08.20.
//  Copyright © 2020 Realtime Audio. All rights reserved.
//

#include "m_pd.h"

static t_class *grainmaker_tilde_class;

typedef struct _grainmaker_tilde {
    t_object    x_obj;
    t_int       offset, grain_length;
    t_float     head_pos, playback_speed;
    t_symbol   *x_arrayname;
    t_float     t_f;
    // sample, grain_envelope, (grain_start), direction
    t_array     *sample;
    t_inlet     *in_sample, *in_offset, *in_grain_length, *in_head_pos;
//    t_outlet    *out_A, *out_B, *out_synch, *out_count;
}t_grainmaker_tilde;

void *grainmaker_tilde_new(t_floatarg f1, t_floatarg f2) {
    t_grainmaker_tilde *x = (t_grainmaker_tilde *)pd_new(grainmaker_tilde_class);
    
    x->in_sample = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_list, gensym("sample"));

//    x->out_A = outlet_new(&x->x_obj, &s_bang);
//    x->out_B = outlet_new(&x->x_obj, &s_bang);
//    x->out_synch = outlet_new(&x->x_obj, &s_bang);
//    x->out_count = outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;
}

static void grainmaker_tilde_set(t_grainmaker_tilde *x, t_symbol *s)
{
    t_garray *a;

    x->x_arrayname = s;
    
    if (!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
    {
        if (*s->s_name)
            pd_error(x, "tabread~: %s: no such array", x->x_arrayname->s_name);
        x->x_vec = 0;
    }
    else if (!garray_getfloatwords(a, &x->x_npoints, &x->x_vec))
    {
        pd_error(x, "%s: bad template for tabread~", x->x_arrayname->s_name);
        x->x_vec = 0;
    }
    else garray_usedindsp(a);
}

static void grainmaker_tilde_dsp(t_grainmaker_tilde *x, t_signal **sp)
{
    grainmaker_tilde_set(x, x->x_arrayname);

    dsp_add(grainmaker_tilde_perform, 4, x,
        sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);

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
    inlet_free(x->in_sample);

//    outlet_free(x->out_A);
//    outlet_free(x->out_B);
//    outlet_free(x->out_synch);
//    outlet_free(x->out_count);
}

void grainmaker_setup(void) {
    grainmaker_class = class_new(gensym("grainmaker~"),
                                 (t_newmethod)grainmaker_new,
                                 (t_method)grainmaker_free,
                                 sizeof(t_grainmaker),
                                 CLASS_DEFAULT,
                                 0);
    
    class_addbang(grainmaker_class, (t_method)grainmaker_onBangMsg);
}

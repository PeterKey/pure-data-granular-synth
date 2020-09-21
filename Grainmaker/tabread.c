/* Copyright (c) 1997-1999 Miller Puckette and others.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.  */

/* sampling */

/* LATER make tabread4 and tabread~ */

#include "m_pd.h"


/******************** tabread~ ***********************/

static t_class *tabread_tilde_class;

typedef struct _tabread_tilde
{
    t_object x_obj;
    int x_npoints;
    t_word *x_vec;
    t_symbol *x_arrayname;
    t_float x_f;
} t_tabread_tilde;

static void *tabread_tilde_new(t_symbol *s)
{
    t_tabread_tilde *x = (t_tabread_tilde *)pd_new(tabread_tilde_class);
    x->x_arrayname = s;
    x->x_vec = 0;
    outlet_new(&x->x_obj, gensym("signal"));
    x->x_f = 0;
    return (x);
}

static t_int *tabread_tilde_perform(t_int *w)
{
    t_tabread_tilde *x = (t_tabread_tilde *)(w[1]);
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
        
        *out = buf[n].w_float
    }
    return (w+5);
 zero:
    while (n--) *out++ = 0;

    return (w+5);
}

static void tabread_tilde_set(t_tabread_tilde *x, t_symbol *s)
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

static void tabread_tilde_dsp(t_tabread_tilde *x, t_signal **sp)
{
    tabread_tilde_set(x, x->x_arrayname);

    dsp_add(tabread_tilde_perform, 4, x,
        sp[0]->s_vec, sp[1]->s_vec, (t_int)sp[0]->s_n);

}

static void tabread_tilde_free(t_tabread_tilde *x)
{
}

static void tabread_tilde_setup(void)
{
    tabread_tilde_class = class_new(gensym("tabread~"),
        (t_newmethod)tabread_tilde_new, (t_method)tabread_tilde_free,
        sizeof(t_tabread_tilde), 0, A_DEFSYM, 0);
    CLASS_MAINSIGNALIN(tabread_tilde_class, t_tabread_tilde, x_f);
    class_addmethod(tabread_tilde_class, (t_method)tabread_tilde_dsp,
        gensym("dsp"), A_CANT, 0);
    class_addmethod(tabread_tilde_class, (t_method)tabread_tilde_set,
        gensym("set"), A_SYMBOL, 0);
}


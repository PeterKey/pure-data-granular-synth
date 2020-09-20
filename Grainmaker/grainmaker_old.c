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
    t_int       init_count, current_count;
    t_int       mod_A, mod_B;
    t_inlet     *in_mod_A, *in_mod_B;
    t_outlet    *out_A, *out_B, *out_synch, *out_count;
}t_grainmaker;

void grainmaker_setMods(t_grainmaker *x, t_floatarg f1, t_floatarg f2) {
    x->mod_A = (f1 <= 0) ? 1 : f1;
    x->mod_B = (f1 <= 0) ? 1 : f2;
}

void grainmaker_resetCount(t_grainmaker *x) {
    x->init_count = 0;
    x->current_count = x->init_count;
}

void grainmaker_onResetMsg(t_grainmaker *x) {
    grainmaker_resetCount(x);
}

void grainmaker_onListMsg(t_grainmaker *x, t_symbol *s, t_int argc, t_atom *argv) {
    switch(argc) {
        case 2:
            grainmaker_setMods(x, atom_getfloat(argv), atom_getfloat(argv + 1));
            grainmaker_resetCount(x);
            break;
        default:
            error("[grainmaker ]: two arguments are needed to set a new ratio");
    }
}

void grainmaker_onBangMsg(t_grainmaker *x) {
    t_int mod_A = x->mod_A;
    t_int mod_B = x->mod_B;
    t_int mod_synch = mod_A * mod_B;
    t_int n = x->current_count;
    
    if (n % mod_synch == 0) {
        outlet_bang(x->out_A);
        outlet_bang(x->out_B);
        outlet_bang(x->out_synch);
        
        x->current_count = 0;
    } else {
        if (n % mod_A == 0) outlet_bang(x->out_A);
        if (n % mod_B == 0) outlet_bang(x->out_B);
    }
    
    outlet_float(x->out_count, x->current_count );
    x->current_count++;
}

void grainmaker_onSetA(t_grainmaker *x, t_floatarg f) {
    grainmaker_setMods(x, f, x->mod_B);
}

void grainmaker_onSetB(t_grainmaker *x, t_floatarg f) {
    grainmaker_setMods(x, x->mod_A, f);
}

void *grainmaker_new(t_floatarg f1, t_floatarg f2) {
    t_grainmaker *x = (t_grainmaker *)pd_new(grainmaker_class);
    
    grainmaker_resetCount(x);
    grainmaker_setMods(x, f1, f2);
    
    x->in_mod_A = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ratio_A"));
    x->in_mod_B = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ratio_B"));
    
    x->out_A = outlet_new(&x->x_obj, &s_bang);
    x->out_B = outlet_new(&x->x_obj, &s_bang);
    x->out_synch = outlet_new(&x->x_obj, &s_bang);
    x->out_count = outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;
}

void grainmaker_free(t_grainmaker *x) {
    inlet_free(x->in_mod_A);
    inlet_free(x->in_mod_B);
    outlet_free(x->out_A);
    outlet_free(x->out_B);
    outlet_free(x->out_synch);
    outlet_free(x->out_count);
}

void grainmaker_setup(void) {
    grainmaker_class = class_new(gensym("grainmaker"),
                                 (t_newmethod)grainmaker_new,
                                 (t_method)grainmaker_free,
                                 sizeof(t_grainmaker),
                                 CLASS_DEFAULT,
                                 A_DEFFLOAT,
                                 A_DEFFLOAT,
                                 0);
    class_addbang(grainmaker_class, (t_method)grainmaker_onBangMsg);
    class_addmethod(grainmaker_class, (t_method)grainmaker_onResetMsg, gensym("reset"), 0);
    class_addlist(grainmaker_class, (t_method)grainmaker_onListMsg);
    class_addmethod(grainmaker_class, (t_method)grainmaker_onSetA, gensym("ratioA"), A_DEFFLOAT, 0);
    class_addmethod(grainmaker_class, (t_method)grainmaker_onSetB, gensym("ratioB"), A_DEFFLOAT, 0);
}

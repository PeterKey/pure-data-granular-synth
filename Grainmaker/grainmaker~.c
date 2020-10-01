/**
*@file grainmaker~.c
*@author Peter Gorzo, Jonas Koerwer, Claudio Albrecht, Roman Schweikert<br>
*Audiocommunication Group, Technical University Berlin <br>
*Real-time audio programming in C, SoSe2020 <br>
*A simple Grain sampler <br>
*<br>
*@brief Entrypoint of the grainmaker~ external and pd interfacing <br>
*<br>
*Pure Data related external definitions. <br>
**<br>
*/

#include <stdlib.h>
#include "grain_scheduler.h"
#include "m_pd.h"

static t_class *grainmaker_tilde_class;

/**
* @struct _grainmaker_tilde
* @brief A structure for a grainmaker~ object <br>
* @var _grainmaker_tilde::x_obj The grainmaker object self reference <br>
* @var _grainmaker_tilde::x_sample Pointer to the sample array <br>
* @var _grainmaker_tilde::x_sample_length Length of x_sample <br>
* @var _grainmaker_tilde::offset Value of the offset inlet <br>
* @var _grainmaker_tilde::num_grains Value of the num_grains inlet <br>
* @var _grainmaker_tilde::grain_length Value of the grain_length inlet <br>
* @var _grainmaker_tilde::x_arrayname Name of the referenced sample array<br>
* @var _grainmaker_tilde::f Reference for pure data signal input <br>
* @var _grainmaker_tilde::x_scheduler Pointer to current grain_scheduler <br>
* @var _grainmaker_tilde::in_offset Pointer to the offset inlet <br>
* @var _grainmaker_tilde::in_num_grains Pointer to the num_grains inlet <br>
* @var _grainmaker_tilde::in_grain_length Pointer to the grain_length inlet <br>
* @var _grainmaker_tilde::out Pointer to the signal outlet <br>
*/
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
    
}t_grainmaker_tilde;

/**
* @related _grainmaker_tilde
* @brief Creates new grainmaker object <br>
* @param arrayname  Name of the provided array used as a source sample <br>
* Create new grainmaker_tilde object <br>
* @return Pointer to grainmaker object <br>
*/
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

/**
* @related _grainmaker_tilde
* @brief Frees grainmaker object <br>
* @param x  grainmaker object<br>
* Free the grainmaker object <br>
*/
void grainmaker_tilde_free(t_grainmaker_tilde *x) {
    grain_scheduler_free(x->x_scheduler);

    inlet_free(x->in_offset);
    inlet_free(x->in_num_grains);
    inlet_free(x->in_grain_length);
    outlet_free(x->out);
}

/**
* @related _grainmaker_tilde
* @brief Performs grainmaker tilde <br>
* @param w signal input<br>
* Performs grainmaker tilde <br>
@return returns output signal  <br>
*/
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
    if(x->num_grains < 0) goto zero;
    if(x->grain_length <= 0) goto zero;

    grain_scheduler_set_props(x->x_scheduler, x->offset, x->num_grains, x->grain_length);

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

/**
* @related _grainmaker_tilde
* @brief Sets the array to read from <br>
* @param x grainmaker tilde object<br>
* Sets the array to use as a sample  <br>
*/
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

/**
* @related _grainmaker_tilde
* @brief Sets up the grainmaker tilde for use as a dsp <br>
* @param x grainmaker tilde object<br>
* @param sp the input signal vector <br>
*/
static void grainmaker_tilde_dsp(t_grainmaker_tilde *x, t_signal **sp)
{
    grainmaker_tilde_set(x);
    if (x->offset > x->x_sample_length) x->offset = x->x_sample_length;
    if (x->grain_length > x->x_sample_length) x->grain_length = x->x_sample_length;
    dsp_add(grainmaker_tilde_perform, 4, x,
            sp[0]->s_vec,
            sp[1]->s_vec,
            sp[0]->s_n);
}

/**
* @related _grainmaker_tilde
* @brief Reacts to inlet changes of offset <br>
* @param x grainmaker tilde object<br>
* @param f new value of offset <br>
*/
static void grainmaker_tilde_set_offset(t_grainmaker_tilde *x, t_floatarg f) {
    int new_value = (int)(f);
    if(new_value < 0) new_value = 0;
    if(x->x_sample_length && new_value > x->x_sample_length) {
        new_value = x->x_sample_length;
    }
    x->offset = (int) new_value;
}

/**
* @related _grainmaker_tilde
* @brief Reacts to inlet changes of num_grains <br>
* @param x grainmaker tilde object<br>
* @param f new value of num_grains <br>
*/
static void grainmaker_tilde_set_num_grains(t_grainmaker_tilde *x, t_floatarg f) {
    int new_value = (int)(f);
    if(new_value < 0) new_value = 0;
    if(new_value > 50) new_value = 50;
    x->num_grains = (int) new_value;
}

/**
* @related _grainmaker_tilde
* @brief Reacts to inlet changes of grain_length <br>
* @param x grainmaker tilde object<br>
* @param f new value of grain_length <br>
*/
static void grainmaker_tilde_set_grain_length(t_grainmaker_tilde *x, t_floatarg f) {
    int new_value = (int)(f);
    if(new_value <= 0) new_value = 1;
    if(x->x_sample_length && new_value > x->x_sample_length) {
        new_value = x->x_sample_length;
    }
    x->grain_length = (int) new_value;
}

/**
* @related _grainmaker_tilde
* @brief Sets up grainmaker as a pd external <br>
*/
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
    
    class_sethelpsymbol(grainmaker_tilde_class, gensym("grainmaker~"));
}

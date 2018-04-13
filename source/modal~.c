//==============================================================================
//
//  @file modal~.c
//  @author Yves Candau <ycandau@gmail.com>
//  
//  @brief A Max external for modal synthesis. It is essentially a bank of two
//  pole resonators, with added functionality for manipulating the bank.
//  
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//==============================================================================
//  Header files
//==============================================================================

#include "modal~.h"

//==============================================================================
//  Global class pointer and static variables
//==============================================================================

static t_class *modal_class = NULL;

static t_symbol *sym_empty = NULL;
static t_symbol *sym_free = NULL;
static t_symbol *sym_ampl = NULL;
static t_symbol *sym_freq = NULL;
static t_symbol *sym_decay = NULL;

//==============================================================================
//  Function declarations
//==============================================================================

void modal_bang(t_modal *x) {

  POST("bang");
}

//******************************************************************************
//  Initialization function
//
void C74_EXPORT ext_main(void* r) {

  t_class *c = class_new(
    "y.modal~",
    (method)modal_new,
    (method)modal_free,
    (long)sizeof(t_modal),
    (method)NULL,
    A_GIMME, 0);

  class_addmethod(c, (method)modal_bang, "bang", 0);

  class_addmethod(c, (method)modal_dsp64,  "dsp64",  A_CANT, 0);
  class_addmethod(c, (method)modal_assist, "assist", A_CANT, 0);

  class_addmethod(c, (method)modal_out_type, "out_type", A_SYM, 0);
  class_addmethod(c, (method)modal_out_sort, "out_sort", A_SYM, 0);

  // ====  IO  ====

  class_addmethod(c, (method)io_dictionary, "dictionary", A_SYM, 0);
  class_addmethod(c, (method)io_import, "import", A_GIMME, 0);
  class_addmethod(c, (method)io_load,   "load",   A_GIMME, 0);
  class_addmethod(c, (method)io_save,   "save",   A_GIMME, 0);
  class_addmethod(c, (method)io_split,  "split",  A_GIMME, 0);
  class_addmethod(c, (method)io_join,   "join",   A_GIMME, 0);
  class_addmethod(c, (method)io_rename, "rename", A_GIMME, 0);
  class_addmethod(c, (method)io_delete, "delete", A_GIMME, 0);
  class_addmethod(c, (method)io_clear,  "clear",  A_GIMME, 0);

  class_addmethod(c, (method)modal_info,  "info",  A_GIMME, 0);
  class_addmethod(c, (method)modal_param, "param", A_GIMME, 0);
  class_addmethod(c, (method)modal_post,  "post",  A_GIMME, 0);
  class_addmethod(c, (method)modal_flush, "flush", A_GIMME, 0);

  // ====  PARAMETERS  ====

  class_addmethod(c, (method)modal_master, "master", A_FLOAT, 0);

  class_addmethod(c, (method)modal_is_on,      "is_on", A_GIMME, 0);
  class_addmethod(c, (method)modal_gain,       "gain",  A_GIMME, 0);
  class_addmethod(c, (method)modal_ampl_mult,  "ampl",  A_GIMME, 0);
  class_addmethod(c, (method)modal_freq_shift, "freq",  A_GIMME, 0);
  class_addmethod(c, (method)modal_decay_mult, "decay", A_GIMME, 0);

  // ====  MODES  ====

  class_addmethod(c, (method)mode_all_on,    "all_on",    A_GIMME, 0);
  class_addmethod(c, (method)mode_all_off,   "all_off",   A_GIMME, 0);
  class_addmethod(c, (method)mode_cycle,     "cycle",     A_GIMME, 0);
  class_addmethod(c, (method)mode_diffusion, "diffusion", A_GIMME, 0);
  class_addmethod(c, (method)mode_resonator, "resonator", A_GIMME, 0);

  // ====  STATES  ====

  class_addmethod(c, (method)state_state,        "state",        A_GIMME, 0);
  class_addmethod(c, (method)state_ramp_to,      "ramp_to",      A_GIMME, 0);
  class_addmethod(c, (method)state_ramp_between, "ramp_between", A_GIMME, 0);
  class_addmethod(c, (method)state_ramp_max,     "ramp_max",     A_GIMME, 0);
  class_addmethod(c, (method)state_velocity,     "velocity",     A_GIMME, 0);
  class_addmethod(c, (method)state_freeze,       "freeze",       A_GIMME, 0);
  
  // Ranges

  class_addmethod(c, (method)modal_get_ampl_rng,  "get_ampl_rng",  A_GIMME, 0);
  class_addmethod(c, (method)modal_get_freq_rng,  "get_freq_rng",  A_GIMME, 0);
  class_addmethod(c, (method)modal_get_decay_rng, "get_decay_rng", A_GIMME, 0);

  // Selections

  class_addmethod(c, (method)modal_sel_all,       "sel_all",       A_GIMME, 0);
  class_addmethod(c, (method)modal_sel_ampl_ind,  "sel_ampl_ind",  A_GIMME, 0);
  class_addmethod(c, (method)modal_sel_ampl_rng,  "sel_ampl_rng",  A_GIMME, 0);
  class_addmethod(c, (method)modal_sel_freq_ind,  "sel_freq_ind",  A_GIMME, 0);
  class_addmethod(c, (method)modal_sel_freq_rng,  "sel_freq_rng",  A_GIMME, 0);
  class_addmethod(c, (method)modal_sel_decay_ind, "sel_decay_ind", A_GIMME, 0);
  class_addmethod(c, (method)modal_sel_decay_rng, "sel_decay_rng", A_GIMME, 0);
  
  CLASS_ATTR_FLOAT(c, "smoothing", 0, t_modal, a_smoothing);
  CLASS_ATTR_LABEL(c, "smoothing", 0, "rms smoothing");
  // XXX CLASS_ATTR_ACCESSORS(c, "smoothing", NULL, NULL);
  //CLASS_ATTR_MIN      (c, "smoothing", 0, "0");
  //CLASS_ATTR_MAX      (c, "smoothing", 0, "1");
  //CLASS_ATTR_FILTER_CLIP(c, "smoothing", 0, 1);
  //CLASS_ATTR_SAVE(c, "smoothing", 0);

  class_dspinit(c);
  class_register(CLASS_BOX, c);
  modal_class = c;
  
  // Frequently used symbols
  sym_empty = gensym("");
  sym_free = gensym("free");
  sym_ampl = gensym("ampl");
  sym_freq = gensym("freq");
  sym_decay = gensym("decay");  
}

// ========  NEW INSTANCE ROUTINE: MODAL_NEW  ========
// Called when the object is created

void *modal_new(t_symbol *sym, t_int32 argc, t_atom *argv) {

  // ====  MAX initializations  ====

  t_modal *x = NULL;
  x = (t_modal *)object_alloc(modal_class);
  
  if (x == NULL) {
    MY_ERR("Object allocation failed.");
    return NULL;
  }
  TRACE("modal_new");

  dsp_setup((t_pxobject *)x, 1);                  // Creating one MSP inlet

  x->outl_float = floatout((t_object *)x);
  x->outl_mess = outlet_new((t_object*)x, NULL);  // Outlet 8: For messages

  outlet_new((t_object *)x, "signal");            // Outlet 7: For signals
  outlet_new((t_object *)x, "signal");            // Outlet 6: For signals
  outlet_new((t_object *)x, "signal");            // Outlet 5: For signals
  outlet_new((t_object *)x, "signal");            // Outlet 4: For signals
  outlet_new((t_object *)x, "signal");            // Outlet 3: For signals
  outlet_new((t_object *)x, "signal");            // Outlet 2: For signals
  outlet_new((t_object *)x, "signal");            // Outlet 1: For signals
  outlet_new((t_object *)x, "signal");            // Outlet 0: For signals

  x->obj.z_misc |= Z_NO_INPLACE;      // Separate input and output arrays

  // ====  Arguments  ====

  t_int32 state_cnt = 0;

  // If no arguments are provided, the default values are used
  if (argc == 0) {
    x->bank_cnt   = BANK_CNT_DEF;
    x->reson_max = RESON_MAX_DEF;
    state_cnt = STATE_CNT_DEF; 
  }
  // If one argument is provided, get: the number of banks
  else if ((argc == 1)
    && (atom_gettype(argv) == A_LONG) && (atom_getlong(argv) >= 1)) {

    x->bank_cnt   = (t_int32)atom_getlong(argv);
    x->reson_max = RESON_MAX_DEF;
    state_cnt = STATE_CNT_DEF;
  }
  // If two arguments are provided, get:
  // the number of banks and the maximum number of resonators
  else if ((argc == 2)
      && (atom_gettype(argv) == A_LONG) && (atom_getlong(argv) >= 1)
      && (atom_gettype(argv + 1) == A_LONG) && (atom_getlong(argv + 1) >= 1)) {

    x->bank_cnt   = (t_int32)atom_getlong(argv);
    x->reson_max = (t_int32)atom_getlong(argv + 1);
    state_cnt = STATE_CNT_DEF;
  }
  // If three arguments are provided, get:
  // the number of banks, the max number of resonators, and the number of states
  else if ((argc == 3)
      && (atom_gettype(argv) == A_LONG) && (atom_getlong(argv) >= 1)
      && (atom_gettype(argv + 1) == A_LONG) && (atom_getlong(argv + 1) >= 1)
      && (atom_gettype(argv + 2) == A_LONG) && (atom_getlong(argv + 2) >= 1)) {

    x->bank_cnt   = (t_int32)atom_getlong(argv);
    x->reson_max = (t_int32)atom_getlong(argv + 1);
    state_cnt = (t_int32)atom_getlong(argv + 2);
  }
  // Otherwise the arguments are invalid and the default values are used
  else {
    x->bank_cnt   = BANK_CNT_DEF;
    x->reson_max = RESON_MAX_DEF;
    state_cnt = STATE_CNT_DEF;

    MY_ERR("modal_new:  Invalid arguments.  The method expects:");
    MY_ERR2("  The arguments determine the number of banks, maximum resonators per bank, and number of states.");
    MY_ERR2("  The default values are:  Banks: %i - Max reson: %i - States: %i.", BANK_CNT_DEF, RESON_MAX_DEF, STATE_CNT_DEF);
    MY_ERR2("  Possible arguments are:");
    MY_ERR2("    No arguments:  All defaults");
    MY_ERR2("    One Int:    Banks: Arg 0");
    MY_ERR2("    Two Int:    Banks: Arg 0 - Max reson: Arg 1");
    MY_ERR2("    Three Int:  Banks: Arg 0 - Max reson: Arg 1 - States: Arg 2");
}

  POST("modal_new:  modal~ object created:  %i banks, %i resonators max, %i states.", x->bank_cnt, x->reson_max, state_cnt);
  POST("  You need to load modal models before using the object.");
  
  // Set pointers to NULL
  x->outp_mess_arr = NULL;

  // Initializing variables
  x->master      = MASTER_MULT;
  x->samplerate = sys_getsr();
  x->msr        = x->samplerate / 1000;
  x->output_ind = 0;

  // Allocating memory for the banks
  x->bank_arr = NULL;
  x->bank_arr = (t_bank *)sysmem_newptr(sizeof(t_bank) * x->bank_cnt);
  if (!x->bank_arr) {
    MY_ERR("modal_new:  Failed to allocate bank_arr.");
    return NULL;
  }
  // Constructors for the banks
  for (int i = 0; i < x->bank_cnt; i++) {
    if (bank_new(x, x->bank_arr + i, 1) == ERR_ALLOC) {
      return NULL;
    }
  }
  // Set the ramping function, inverse function, and parameter
  x->ramp_param     = 4;
  x->ramp_func     = ramp_exp;
  x->ramp_func_inv = ramp_exp_inv;

  // Allocating memory for the states
  x->state_arr = _state_arr_new(state_cnt, &(x->state_cnt));
  if (!x->state_arr) {
    MY_ERR("modal_new:  Failed to allocate state_arr.");
    return NULL;
  }
  // Initialize the temporary state used for calculations
  _state_init(x->state_tmp, x->reson_max, 0, 0);
  if ((!x->state_tmp->U_arr) || (!x->state_tmp->A_arr)) {
    MY_ERR("modal_new:  Failed to allocate state_tmp.");
    return NULL;
  }
  // Set the name of the dictionary to empty for now
  x->dict_sym = sym_empty;

  // Allocate memory for outputting messages
  x->outp_mess_arr = (t_atom *)sysmem_newptr(sizeof(t_atom) * 3 * x->reson_max);
  if (!x->outp_mess_arr) {
    MY_ERR("modal_new:  Failed to allocate mess_arr.");
    return NULL;
  }
  // Initialize random
  srand((unsigned int)time(NULL));

  // Initialize output
  x->out_type = OUT_TYPE_OUTP;
  x->sort_type = OUT_SORT_FREQ;
  x->a_smoothing = 0.1;

  x->reson_cur = x->bank_arr->reson_arr;

  return (x);
}

// ========  METHOD: MODAL_FREE  ========
// Called when the object is deleted

void modal_free(t_modal *x) {

  TRACE("modal_free");
  
  for (int i = 0; i < x->bank_cnt; i++) { bank_free(x, x->bank_arr + i); }
  if (x->bank_arr) { sysmem_freeptr(x->bank_arr); }

  if (x->state_arr) { _state_arr_free(&(x->state_arr), &(x->state_cnt)); }
  _state_free(x->state_tmp);

  if (x->outp_mess_arr) { sysmem_freeptr(x->outp_mess_arr); }

  dsp_free((t_pxobject *)x);
}

// ========  METHOD: MODAL_DSP64  ========
// Called when the DAC is enabled

void modal_dsp64(
  t_modal *x, t_object *dsp64, t_int32 *count, t_double samplerate,
  long maxvectorsize, long flags) {

  TRACE("modal_dsp64");
  POST("Samplerate = %.0f - Maxvectorsize = %i", samplerate, maxvectorsize);

  object_method(dsp64, gensym("dsp_add64"), x, modal_perform64, 0, NULL);

  // Recalculate everything that depends on the samplerate
  x->samplerate = samplerate;
  x->msr = x->samplerate / 1000;
  
  // Update the banks of resonators
  for (int i = 0; i < x->bank_cnt; i++) { bank_update(x, x->bank_arr + i); }
}

// ========  METHOD: MODAL_PERFORM64  ========

void modal_perform64(
  t_modal *x, t_object *dsp64, t_double **ins, long numins, t_double **outs,
  long numouts, long sampleframes, long flags, void *userparam) {

  // Input and output vectors
  t_double *in = ins[0];
  t_double *out0 = outs[0], *out1 = outs[1], *out2 = outs[2], *out3 = outs[3];
  t_double *out4 = outs[4], *out5 = outs[5], *out6 = outs[6], *out7 = outs[7];

  // Set all output vectors to 0
  for (int i = 0; i < sampleframes; i++) {
    out0[i] = 0; out1[i] = 0; out2[i] = 0; out3[i] = 0;
    out4[i] = 0; out5[i] = 0; out6[i] = 0; out7[i] = 0;
  }

  // Loop through all the banks
  for (t_int32 bnk = 0; bnk < x->bank_cnt; bnk++) {

    t_bank *bank = x->bank_arr + bnk;

    if (bank->is_on == true) {
      
      // Variables for the loop through all the resonators
      t_resonator *reson = bank->reson_arr;
      t_int32 chunk_len = -1;
      t_int32 counter = 0;
      t_int32 counter_x_vel = 0;
      t_int32 cntd_d_vel = 0;
      t_double gain_bank = x->master * bank->gain;
      t_double d_ampl = 0.0;
      t_double gain_res = 0.0;
      t_double sum_sqr = 0.0;
      t_double tmp = 0.0;
      t_double dA = 0.0;

      // Loop through all the resonators
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {

        // Set the resonator and initialize
        reson = bank->reson_arr + res;
        counter = sampleframes;
        in = ins[0];
        out0 = outs[0]; out1 = outs[1]; out2 = outs[2]; out3 = outs[3];
        out4 = outs[4]; out5 = outs[5]; out6 = outs[6]; out7 = outs[7];
        sum_sqr = 0.0;
        
        // Keep looping until all the chunks are processed
        while (counter) {

          // == Calculate:
          //   chunk_len:   the number of samples to process in this chunk loop - cannot be 0
          //   counter:     the number of samples left to process in this perform cycle
          //   reson->cntd: the total number of sampleframes left to process (unscaled by the velocity)

          // == Temporary variables
          counter_x_vel = (t_int32)(counter * bank->velocity);
          cntd_d_vel = (t_int32)(reson->cntd / bank->velocity);              // cannot be 0, unless cntd is 0
          if ((cntd_d_vel == 0) && (reson->cntd != 0)) { cntd_d_vel = 1; }  // correct for rounding down to 0 when cntd is not 0

          // == Five cases depending on the countdown
          
          // == If the bank is set to freeze
          // == process the whole audio vector with no ramping or countdown
          if (bank->is_frozen) { chunk_len = sampleframes; counter = 0; }

          // == Zero countdown:  Don't do anything and go straight to the mode change method
          else if (reson->cntd == 0) { goto MODAL_PEFORM64_MODE_CHANGE; }

          // == Indefinite countdown:  The chunk is the whole length of the perform cycle
          else if (reson->cntd == INDEFINITE) { chunk_len = counter; counter = 0; }

          // == Countdown extends beyond perform cycle:  The chunk is the whole length of the perform cycle
          else if (reson->cntd > counter_x_vel) { chunk_len = counter; counter = 0; reson->cntd -= counter_x_vel; }
          //else if (reson->cntd > counter) { chunk_len = counter; counter = 0; reson->cntd -= chunk_len; }

          // == Countdown shorter than perform cycle:  Keep processing chunks and mode changes
          else { chunk_len = cntd_d_vel; counter -= chunk_len; reson->cntd = 0; }    // counter never gets to -1 in spite of rounding
          //else { chunk_len = reson->cntd; counter -= chunk_len; reson->cntd = 0; }
          
          // ==== Process the chunk depending on the mode of the resonator

          // == RESONATOR IS OFF
          // == Iterate the input and output pointers so they will be ready for the next chunk
          if (reson->mode_type == MODE_TYPE_OFF) {
            out0 += chunk_len; out1 += chunk_len; out2 += chunk_len; out3 += chunk_len;
            out4 += chunk_len; out5 += chunk_len; out6 += chunk_len; out7 += chunk_len; in += chunk_len;
          }

          // == RESONATOR IS FIXED, FROZEN OR INDEFINITE
          // == Add values without ramping
          else if ((reson->mode_type == MODE_TYPE_FIX) || (bank->is_frozen) || (reson->cntd == INDEFINITE)) {

            // The output gain does not vary over the chunk
            gain_res = gain_bank * reson->out_A_cur;

            for (t_int32 smp = 0; smp < chunk_len; smp++) {

              // Calculate the next value of the resonator
              tmp = reson->a0 * (*in) * reson->in_A_cur + reson->b1 * reson->y_m1 + reson->b2 * reson->y_m2;
              reson->y_m2 = reson->y_m1;
              reson->y_m1 = tmp;

              // To calculate RMS. Does not include resonator gain
              sum_sqr += tmp * tmp;

              // Apply gain and iterate the input and output pointers
              tmp *= gain_res;
              *out0 += tmp * reson->diff_mult[0]; *out1 += tmp * reson->diff_mult[1];
              *out2 += tmp * reson->diff_mult[2]; *out3 += tmp * reson->diff_mult[3];
              *out4 += tmp * reson->diff_mult[4]; *out5 += tmp * reson->diff_mult[5];
              *out6 += tmp * reson->diff_mult[6]; *out7 += tmp * reson->diff_mult[7];
              out0++; out1++; out2++; out3++; out4++; out5++; out6++; out7++; in++;
            }
          }

          // == RESONATOR HAS AMPLITUDE RAMPING
          // == Add values with ramping
          else if (reson->mode_type == MODE_TYPE_VAR_A) {

            // Calculate dA: linear ramping of input amplitude over the chunk length

            // Increment the normalized ordinate value U by dU for the chunk length:
            // recalculated each chunk to avoid cumulative errors
            // alternative would be to calculate dU once when the ramp is created
            reson->in_U_cur += chunk_len * (reson->in_U_targ - reson->in_U_cur) / cntd_d_vel;    // cntd_d_vel cannot be 0

            // Calculate A(U + dU): the target amplitude value at the end of the chunk length
            tmp =  x->ramp_func(reson->in_U_cur, x->ramp_param);

            // Calculate dA
            dA = (tmp - reson->in_A_cur) / chunk_len;    // chunk_len cannot be 0

            // Loop over all the samples of the chunk
            for (t_int32 smp = 0; smp < chunk_len; smp++) {

              // Ramp current amplitude multipliers
              gain_res = gain_bank * reson->out_A_cur;

              // Calculate the next value of the resonator
              tmp = reson->a0 * (*in) * reson->in_A_cur + reson->b1 * reson->y_m1 + reson->b2 * reson->y_m2;
              reson->y_m2 = reson->y_m1;
              reson->y_m1 = tmp;

              // Ramp input gain
              reson->in_A_cur += dA;

              // To calculate RMS
              sum_sqr += tmp * tmp;

              // Apply gain and iterate the input and output pointers
              tmp *= gain_res;
              *out0 += tmp * reson->diff_mult[0]; *out1 += tmp * reson->diff_mult[1];
              *out2 += tmp * reson->diff_mult[2]; *out3 += tmp * reson->diff_mult[3];
              *out4 += tmp * reson->diff_mult[4]; *out5 += tmp * reson->diff_mult[5];
              *out6 += tmp * reson->diff_mult[6]; *out7 += tmp * reson->diff_mult[7];
              out0++; out1++; out2++; out3++; out4++; out5++; out6++; out7++; in++;
            }
          }

          // == RESONATOR HAS AMPLITUDE AND PARAMETER RAMPING
          // == Add values with ramping. It cannot be set on indefinite time.
          // XXX TO DO XXX

          /*else if (reson->mode_type == MODE_TYPE_VAR_S) {

            // The gain does not vary over the chunk
            gain_res = gain_bank * reson->out_A_cur;

            reson->freq_tmp *= exp(reson->param[0] * chunk_len / (12 * x->samplerate));
            t_double r = exp(-reson->decay / x->samplerate);
            t_double b1_targ = 2 * r * cos(TWOPI * reson->freq_tmp / x->samplerate);
            t_double d_b1 = (b1_targ - reson->b1) / chunk_len;

            for (t_int32 smp = 0; smp < chunk_len; smp++) {

              reson->b1 += d_b1;

              // Calculate the next value of the resonator
              tmp = reson->a0 * (*in) * reson->in_A_cur + reson->b1 * reson->y_m1 + reson->b2 * reson->y_m2;
              reson->y_m2 = reson->y_m1;
              reson->y_m1 = tmp;

              // To calculate RMS
              sum_sqr += tmp * tmp;

              // Apply gain and iterate the input and output pointers
              tmp *= gain_res;
              *out0 += tmp * reson->diff_mult[0]; *out1 += tmp * reson->diff_mult[1];
              *out2 += tmp * reson->diff_mult[2]; *out3 += tmp * reson->diff_mult[3];
              *out4 += tmp * reson->diff_mult[4]; *out5 += tmp * reson->diff_mult[5];
              *out6 += tmp * reson->diff_mult[6]; *out7 += tmp * reson->diff_mult[7];
              out0++; out1++; out2++; out3++; out4++; out5++; out6++; out7++; in++;
            }
          }*/

          // == OTHERWISE
          // == Post a message error
          else { MY_ERR("modal_perform64:  Invalid mode type."); }

          // If the countdown has reached 0, change the mode of the resonator
          // This happened either from outside the perform64 method, as a way to set an initial mode
          // Or within the chunk loop
  MODAL_PEFORM64_MODE_CHANGE:
          if (reson->cntd == 0) { _mode_iterate(x, bank, reson); }
        }

        // Smoothing parameter for rms output
        reson->rms = x->a_smoothing * sqrt(sum_sqr / sampleframes) + (1 - x->a_smoothing) * reson->rms;
      }
    }
  }

  // == Output information for all resonators of one bank, done once per perform cycle
  // ==   matrixctrl (row index) (column index) (parameter value, scaled 0-100) ... (resonator count) times

  // ==== If the object is setup to output information on the resonators
  if (x->out_type != OUT_TYPE_OFF) {

    t_bank *bank = x->bank_cur;
    t_atom *mess = x->outp_mess_arr;

    // == Choose the resonator parameter by which to sort the output: amplitude, frequence, or decay
    t_int32 *out_sort = NULL;
    if      (x->sort_type == OUT_SORT_AMPL)  { out_sort = bank->sort_ampl; }
    else if (x->sort_type == OUT_SORT_FREQ)  { out_sort = bank->sort_freq; }
    else if (x->sort_type == OUT_SORT_DECAY) { out_sort = bank->sort_decay; }

    // == Output output multiplier
    if (x->out_type == OUT_TYPE_OUTP) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        atom_setlong(mess++, res % 10);
        atom_setlong(mess++, (t_int32)(res / 10));
        atom_setlong(mess++, (t_int32)((bank->reson_arr + out_sort[res])->out_A_cur * 100));
      }
    }

    // == Output input multiplier
    else if (x->out_type == OUT_TYPE_INP) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        atom_setlong(mess++, res % 10);
        atom_setlong(mess++, (t_int32)(res / 10));
        atom_setlong(mess++, (t_int32)((bank->reson_arr + out_sort[res])->in_A_cur * 100)); }  }

    // == Output RMS
    else if (x->out_type == OUT_TYPE_RMS) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        atom_setlong(mess++, res % 10);
        atom_setlong(mess++, (t_int32)(res / 10));
        atom_setlong(mess++, (t_int32)((bank->reson_arr + out_sort[res])->rms * 100)); }  }

    // == Output channel index (highest channel index if there are more than one)
    else if (x->out_type == OUT_TYPE_CH_I) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        atom_setlong(mess++, res % 10);
        atom_setlong(mess++, (t_int32)(res / 10));
        atom_setlong(mess++, (t_int32)((((bank->reson_arr + out_sort[res])->diff_ind + 4) % 8) * 12.5)); }  }

    // == Output number of channels
    else if (x->out_type == OUT_TYPE_CH_N) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        atom_setlong(mess++, res % 10);
        atom_setlong(mess++, (t_int32)(res / 10));
        atom_setlong(mess++, (t_int32)((bank->reson_arr + out_sort[res])->diff_cnt * 12.5)); }  }
    
    // == Output the list for the matrixctrl object
    outlet_anything(x->outl_mess, gensym("matrixctrl"), bank->reson_cnt * 3, x->outp_mess_arr); }

  // ==== Output a float for the scrolling multislider object
  outlet_float(x->outl_float, x->reson_cur->rms);
}

// ========  METHOD: MODAL_ASSIST  ========

void modal_assist(t_modal *x, void *b, long msg, t_int32 arg, char *str) {

  //TRACE("modal_assist");
  
  if (msg == ASSIST_INLET) {
    switch (arg) {
    case 0: sprintf(str, "Inlet 0: All purpose (signal, list)"); break;
    default: break; } }

  else if (msg == ASSIST_OUTLET) {
    switch (arg) {
    case 0: sprintf(str, "Outlet 0: For signals - Channel 1 (signal)"); break;
    case 1: sprintf(str, "Outlet 1: For signals - Channel 2 (signal)"); break;
    case 2: sprintf(str, "Outlet 2: For signals - Channel 3 (signal)"); break;
    case 3: sprintf(str, "Outlet 3: For signals - Channel 4 (signal)"); break;
    case 4: sprintf(str, "Outlet 4: For signals - Channel 5 (signal)"); break;
    case 5: sprintf(str, "Outlet 5: For signals - Channel 6 (signal)"); break;
    case 6: sprintf(str, "Outlet 6: For signals - Channel 7 (signal)"); break;
    case 7: sprintf(str, "Outlet 7: For signals - Channel 8 (signal)"); break;
    case 8: sprintf(str, "Outlet 8: For messages (list)"); break;
    default: break; } }
}

// ====  METHOD: BANK_FIND  ====
// Looks for a bank of resonators using an atom that contains either:
// an index, or a symbol which could be free or the name of a bank
// Returns a pointer to the corresponding bank or NULL if no bank is found

t_bank *bank_find(t_modal *x, t_atom *argv, t_symbol *sym) {

  // If the atom contains an int
  if (atom_gettype(argv) == A_LONG) {

    // Test if the index is valid
    t_int32 index = (t_int32)atom_getlong(argv);
    if ((index >= 0) && (index < x->bank_cnt)) {
      return (x->bank_arr + index);
    }
  }
  // If the atom contains a symbol
  else if (atom_gettype(argv) == A_SYM) {

    // Test if the symbol is one of the bank names
    t_symbol *sym = atom_getsym(argv);
    for (int i = 0; i < x->bank_cnt; i++) {
      if ((x->bank_arr + i)->name == sym) {
        return (x->bank_arr + i);
      }
    }
  }
  // Otherwise return NULL
  MY_ERR("%s:  Invalid bank reference.", sym->s_name);
  return NULL;
}

// ====  METHOD: MODAL_FIND_RESON  ====

t_resonator *modal_find_reson(
  t_modal *x, t_bank *bank, t_atom *argv, t_symbol *sym) {

  // If the atom contains an int
  if (atom_gettype(argv) == A_LONG) {

    // Test if the index is valid
    t_int32 index = (t_int32)atom_getlong(argv);
    if ((index >= 0) && (index < bank->reson_cnt)) {
      
      // Determine the index depending on the sorting method
      if      (x->sort_type == OUT_SORT_AMPL)  { index = bank->sort_ampl[index]; }
      else if (x->sort_type == OUT_SORT_FREQ)  { index = bank->sort_freq[index]; }
      else if (x->sort_type == OUT_SORT_DECAY) { index = bank->sort_decay[index]; }

      return (bank->reson_arr + index); } }

  // Otherwise return NULL
  MY_ERR("%s:  Invalid resonator reference.", sym->s_name);
  return NULL;
}

// ====  METHOD: MODAL_OUT_TYPE  ====

void modal_out_type(t_modal *x, t_symbol *type) {

  if      (type == gensym("off"))    { x->out_type = OUT_TYPE_OFF; }
  else if (type == gensym("input"))  { x->out_type = OUT_TYPE_INP; }
  else if (type == gensym("output")) { x->out_type = OUT_TYPE_OUTP; }
  else if (type == gensym("rms"))    { x->out_type = OUT_TYPE_RMS; }
  else if (type == gensym("chan_i")) { x->out_type = OUT_TYPE_CH_I; }
  else if (type == gensym("chan_n")) { x->out_type = OUT_TYPE_CH_N; }
  else { MY_ERR("modal_out_type:  Wrong argument."); }
}

// ====  METHOD: MODAL_OUT_SORT  ====

void modal_out_sort(t_modal *x, t_symbol *type) {


  if      (type == gensym("ampl"))  { x->sort_type = OUT_SORT_AMPL; }
  else if (type == gensym("freq"))  { x->sort_type = OUT_SORT_FREQ; }
  else if (type == gensym("decay")) { x->sort_type = OUT_SORT_DECAY; }

  else { MY_ERR("modal_out_sort:  Wrong argument."); }
}

// ====  METHOD: IO_IMPORT  ====
// Import resonator data from a text file into a bank.
// Arguments:  int/sym, sym, [sym]
//   Arg 0:  The bank to import into (int/sym):  index / name / "free"
//   Arg 1:  The name of the new bank (sym)
//   Arg 2:  Optional file name [sym]

void io_import(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_import");

  // Pointers to structures that need cleanup
  t_filehandle file_handle = NULL;
  t_handle file_text = NULL;

  t_bool test_arg = true;
  t_bank *bank;
  t_symbol *name;

  // The number of arguments should be two or three
  if ((argc != 2) && (argc != 3)) { test_arg = false; }

  else {

    // The first argument should reference a bank of resonator
    bank = bank_find(x, argv, sym);
    if (bank == NULL) { test_arg = false; }

    // The second argument should be a symbol with the name of the new bank
    name = atom_getsym(argv + 1);
    if (name == sym_empty) { test_arg = false; } }

  // If any of the previous tests failed the arguments are invalid
  if (test_arg == false) {
    MY_ERR("%s:  Invalid arguments. The method expects:  int/sym, sym, [sym]", sym->s_name);
    MY_ERR2("  Arg 0:  The bank to import into (int/sym):  index / name / \"free\"");
    MY_ERR2("  Arg 1:  The name of the new bank (sym)");
    MY_ERR2("  Arg 2:  Optional file name [sym]");
    goto MODAL_IMPORT_END; }

  // Variables for modal file selection
  t_bool     test_file = false;
  t_symbol  *file_sym;
  char       file_name[MAX_FILENAME_CHARS];
  short       file_path;
  t_fileinfo file_info;
  t_fourcc   file_type = FOUR_CHAR_CODE('TEXT');

  // If 3 arguments, try opening the corresponding file
  if (argc == 3) {
    file_sym = atom_getsym(argv + 2);

    // Test that the third argument is a symbol
    if (file_sym == sym_empty) {
      MY_ERR("%s:  Arg 2:  Invalid argument type: symbol expected.", sym->s_name); }
  
    // Test that the third argument contains a valid path and name
    else if (path_frompathname(file_sym->s_name, &file_path, file_name)) {
      MY_ERR("%s:  Arg 2:  Invalid path and file name.", sym->s_name); }

    // Test that the file exists
    else if (path_fileinfo(file_name, file_path, &file_info)) {
      MY_ERR("%s:  Arg 2:  File not found.", sym->s_name); }

    else { test_file = true; } }
  
  // If 2 arguments or the file was not found, open a dialog box
  if ((argc == 2) || (test_file == false)) {
    open_promptset("Choose a text file with modal parameters.");
    if (open_dialog(file_name, &file_path, &file_type, &file_type, 1) != 0)
      { goto MODAL_IMPORT_END; } }    // If no file selected cancel
  
  // Open the file
  if (path_opensysfile(file_name, file_path, &file_handle, PATH_READ_PERM)) {
    MY_ERR("%s:  Arg 2:  Failed to open the file.", sym->s_name); goto MODAL_IMPORT_END; }

  // Read the file into text
  file_text = sysmem_newhandle(0);
  // TEXT_NULL_TERMINATE is important, otherwise the string has no end
  sysfile_readtextfile(file_handle, file_text, 0, TEXT_LB_NATIVE | TEXT_NULL_TERMINATE);
  
  // Test the validity of the file content
  int nb = 0, d_ptr = 0, valid = 0;
  char *ptr = *file_text;

  // The first token should be an int
  valid = sscanf_s(ptr, "%i %n", &nb, &d_ptr);

  // Followed by 3*n floats
  t_int32 cntd = 3 * nb;
  while ((valid == 1) && (cntd--)) {
    float f;
    ptr += d_ptr;
    valid = sscanf_s(ptr, "%f %n", &f, &d_ptr); }
  
  // If sscanf_s failed to read
  if (valid != 1) { MY_ERR("%s:  Invalid data in the modal file %s.", file_name, sym->s_name); }

  // ==== Otherwise load the values
  else {
    
    // Free the existing bank and create a new one
    bank_free(x, bank);
    if (bank_new(x, bank, nb) == ERR_ALLOC) { goto MODAL_IMPORT_END; }

    // Set the name and gain for the resonator
    bank->name = name;
    bank->gain = 1.0;

    // Read the data from the text file
    int tmp;
    t_resonator *reson;
    ptr = *file_text;

    // Read the number of resonators
    sscanf_s(ptr, "%i %n", &tmp, &d_ptr); ptr += d_ptr;
    
    // Read the resonator parameters
    float ampl, freq, decay;
    for (t_int32 i = 0; i < bank->reson_cnt; i++) {
      sscanf_s(ptr, "%f %n", &ampl, &d_ptr); ptr += d_ptr;
      sscanf_s(ptr, "%f %n", &freq, &d_ptr); ptr += d_ptr;
      sscanf_s(ptr, "%f %n", &decay, &d_ptr); ptr += d_ptr;

      reson = bank->reson_arr + i;
      reson->ampl_ref   = (t_double)ampl;
      reson->freq_ref  = (t_double)freq;
      reson->decay_ref = (t_double)decay; }

    // Update and sort the resonators by amplitude, frequency and decay
    bank_update(x, bank);
    bank_sort(x, bank); }
    
  // Send out a message to indicate completion of import
  // NB: Using mess_arr was not working, possibly because the function was deferred
  t_atom mess_arr[4];
  atom_setlong(mess_arr, bank - x->bank_arr);
  atom_setsym(mess_arr + 1, bank->name);
  atom_setlong(mess_arr + 2, bank->reson_cnt);
  atom_setfloat(mess_arr + 3, bank->gain);
  outlet_anything(x->outl_mess, gensym("import"), 4, mess_arr);

  // Close the file and handle
  MODAL_IMPORT_END:
  if (file_handle) { sysfile_close(file_handle); }
  if (file_text)   { sysmem_freehandle(file_text); }
  return;
}

// ====  METHOD: IO_DICTIONARY  ====

void io_dictionary(t_modal *x, t_symbol *dict_sym) {

  TRACE("io_dictionary");
  
  t_dictionary *dict = dictobj_findregistered_retain(dict_sym);

  if (!dict) { MY_ERR("io_dictionary:  There is no dictionary %s.", dict_sym->s_name); return; }

  x->dict_sym = dict_sym;
  dictobj_release(dict);
}

// ====  METHOD: IO_LOAD  ====
// Load a bank from the main dictionary.
// Arguments: int/sym sym sym
//   Arg 0:  The name of the bank to load in the dictionary (sym)
//   Arg 1:  The bank to load into (int/sym):  index / name / "free"

void io_load(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_load");

  // Pointers to structures that need cleanup
  t_dictionary *dict = NULL;

  // Get the name of the bank to look for in the dictionary
  t_symbol *bank_sym = atom_getsym(argv);
  if (bank_sym == sym_empty) { MY_ERR("io_load:  Arg 0:  A bank name to look for in the dictionary is required."); goto MODAL_LOAD_END; }

  // Get the bank to load into
  t_bank *bank = bank_find(x, argv + 1, sym);
  if (bank == NULL) { MY_ERR("io_load:  Arg 1:  The bank to load into was not found."); goto MODAL_LOAD_END; }

  // Test if the main dictionary is found
  dict = dictobj_findregistered_retain(x->dict_sym);
  if (!dict) { MY_ERR("io_load:  There is no main dictionary %s.", x->dict_sym->s_name); goto MODAL_LOAD_END; }

  // Open the subdictionary with all the banks
  t_dictionary *dict_all_banks = NULL;
  dictionary_getdictionary(dict, gensym("banks"), (t_object **)&dict_all_banks);
  if (dict_all_banks == NULL) {
    MY_ERR("io_load:  The main dictionary %s does not have a subdictionary for the banks.", x->dict_sym->s_name); goto MODAL_LOAD_END;  }

  // Look for the specific bank we want to load from the dictionary
  t_dictionary *dict_bank = NULL;
  dictionary_getdictionary(dict_all_banks, bank_sym, (t_object **)&dict_bank);
  if (dict_bank == NULL) {
    MY_ERR("io_load:  The bank %s was not found in the main dictionary %s.", bank_sym->s_name, x->dict_sym->s_name); goto MODAL_LOAD_END; }

  // Get the number of resonators
  t_atom_long a_al = 0;
  dictionary_getlong(dict_bank, gensym("reson_cnt"), &a_al);
  if ((a_al < 1) || (a_al > x->reson_max)) {
    MY_ERR("io_load:  Invalid number of resonators: %i. Expected: 1 to %i.", a_al, x->reson_max); goto MODAL_LOAD_END; }

  // Free the existing bank and create a new one
  bank_free(x, bank);
  if (bank_new(x, bank, (t_int32)a_al) == ERR_ALLOC) { goto MODAL_LOAD_END; }

  // Get the name and gain of the bank
  bank->name = bank_sym;
  double a_d;
  dictionary_getfloat(dict_bank, gensym("gain"), &a_d); bank->gain = (t_double)a_d;

  // Get the array of atoms
  t_atom *atom_arr = NULL;
  long a_l = 0;
  dictionary_getatoms(dict_bank, gensym("resonators"), &a_l, &atom_arr);

  // The number of atoms in the array should match the number of resonators previously retrieved
  if (a_l != bank->reson_cnt) {
    MY_ERR("io_load:  The number of resonators is inconsistent with the number of parameters provided."); goto MODAL_LOAD_END; }

  // Read the data for all the resonators
  t_resonator  *reson;
  t_dictionary *dict_reson;
  for (t_int32 i = 0; i < bank->reson_cnt; i++) {
    reson = bank->reson_arr + i;
    dict_reson = (t_dictionary *)atom_getobj(atom_arr + i);
    dictionary_getfloat(dict_reson, gensym("ampl"),  &a_d); reson->ampl_ref   = a_d;
    dictionary_getfloat(dict_reson, gensym("freq"),  &a_d); reson->freq_ref   = a_d;
    dictionary_getfloat(dict_reson, gensym("decay"), &a_d); reson->decay_ref = a_d; }

  // Update and sort the resonators by amplitude, frequency and decay
  bank_update(x, bank);
  bank_sort(x, bank);

  // Send out a message to indicate completion of load
  t_atom mess_arr[4];
  atom_setlong(mess_arr, bank - x->bank_arr);
  atom_setsym(mess_arr + 1, bank->name);
  atom_setlong(mess_arr + 2, bank->reson_cnt);
  atom_setfloat(mess_arr + 3, bank->gain);
  outlet_anything(x->outl_mess, gensym("load"), 4, mess_arr);
  
  // Release the main dictionary
  MODAL_LOAD_END:
  if (dict)   { dictobj_release(dict); }
  return;
}

// ====  METHOD: IO_SAVE  ====
// Save a bank into a dictionary.
// Arguments: int/sym sym [sym]
//   Arg 0:  The bank to save into the dictionary (int/sym):  index / name
//   Arg 1:  The name to save it under
//   Arg 2:  Optional argument "protect" or "override"

void io_save(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_save");

  // Pointers to structures that need cleanup
  t_atom       *atoms = NULL;
  t_dictionary *dict  = NULL;

  // Intercept notifications generated when the patcher is saved
  if (sym != gensym("save")) { goto MODAL_SAVE_END; }

  // Get the bank to save into the dictionary
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) {
    MY_ERR("io_save:  Arg 0:  The bank to save was not found."); goto MODAL_SAVE_END; }

  // Get the name under which to save it
  t_symbol *bank_sym = atom_getsym(argv + 1);
  if (bank_sym == sym_empty) {
    MY_ERR("io_save:  Arg 1:  A name to save the bank under is required."); goto MODAL_SAVE_END; }

  // Read the optional third argument
  t_symbol *arg2 = sym_empty;
  if (argc == 3) { arg2 = atom_getsym(argv + 2); }
  if ((arg2 != gensym("protect")) && (arg2 != gensym("override"))) { arg2 = sym_empty; }

  // Test if the main dictionary is found
  dict = dictobj_findregistered_retain(x->dict_sym);
  if (!dict) { MY_ERR("io_save:  There is no dictionary %s.", x->dict_sym->s_name); goto MODAL_SAVE_END; }

  // Test if a subdictionary for the banks already exists
  t_dictionary *dict_all_banks = NULL;
  dictionary_getdictionary(dict, gensym("banks"), (t_object **)&dict_all_banks);

  // If not create one and append it to the main dictionary
  // Do not free because ownership is passed on to the main dictionary dict
  if (dict_all_banks == NULL) {
    dict_all_banks = dictionary_new();
    dictionary_appenddictionary(dict, gensym("banks"), (t_object *)dict_all_banks);  }

  // Test if a subdictionary for the bank already exists
  t_dictionary *dict_bank = NULL;
  t_symbol *is_protect = sym_empty;

  dictionary_getdictionary(dict_all_banks, bank_sym, (t_object **)&dict_bank);
  if (dict_bank != NULL) {

    // Test if the bank is write protected
    dictionary_getsym(dict_bank, gensym("protected"), &is_protect);

    // Only save if the third argument is "protect" or "override"
    if ((is_protect == gensym("true")) && (arg2 == sym_empty)) {
      MY_ERR("io_save:  The dictionary bank %s is write protected. Use \"protect\" or \"override\" to save over it.", bank_sym->s_name);
      goto MODAL_SAVE_END; } }
    
  // Create a subdictionary for the bank and append it to dict_all_banks. Do not free.
  if (arg2 == gensym("protect")) { is_protect = gensym("true"); }
  else { is_protect = gensym("false"); }

  dict_bank = dictionary_sprintf("@from %s @reson_cnt %i @gain %f @protected %s \
    @ampl_min %f @ampl_max %f @freq_min %f @freq_max %f @decay_min %f @decay_max %f",
    bank->name->s_name, bank->reson_cnt, bank->gain, is_protect->s_name,
    bank->ampl_min, bank->ampl_max, bank->freq_min, bank->freq_max, bank->decay_min, bank->decay_max);
  dictionary_appenddictionary(dict_all_banks, bank_sym, (t_object *)dict_bank);
  
  // Create an array of atoms for the resonators
  atoms = (t_atom *)sysmem_newptr(sizeof(t_atom) * bank->reson_cnt);
  if (atoms == NULL) {
    MY_ERR("io_save:  Failed to allocate a temporary array of atoms."); goto MODAL_SAVE_END; }

  // Create a subdictionary for each resonator, put each in the array of atoms
  t_resonator  *reson = NULL;
  t_dictionary *dict_reson = NULL;
  for (t_int32 i = 0; i < bank->reson_cnt; i++) {
    reson = bank->reson_arr + i;
    dict_reson = dictionary_sprintf("@index %i @ampl %f @freq %f @decay %f @b1 %f @b2 %f",
      i, reson->a0, reson->freq, reson->decay, reson->b1, reson->b2);
    atom_setobj(atoms + i, dict_reson); }

  // Append the array of atoms to dict_bank. The atoms are copied.
  dictionary_appendatoms(dict_bank, gensym("resonators"), bank->reson_cnt, atoms);

  // Send out a message to indicate completion of load
  t_atom mess_arr[3];
  atom_setlong(mess_arr, bank - x->bank_arr);
  atom_setsym(mess_arr + 1, bank_sym);
  atom_setlong(mess_arr + 2, bank->reson_cnt);
  outlet_anything(x->outl_mess, gensym("save"), 3, mess_arr);
  
  // Free the array of atoms and release the main dictionary
  MODAL_SAVE_END:
  if (atoms) { sysmem_freeptr(atoms); }
  if (dict)   { dictobj_release(dict); }
  return;
}

// ====  METHOD: IO_SPLIT  ====
// Splits a bank into two banks in a dictionary.
// Arguments: int/sym sym
//   Arg 0:  The bank to split into the dictionary (int/sym):  index / name
//   Arg 1:  The name to save it under. The second bank will be ..._rem

void io_split(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_split");

  // Pointers to structures that need cleanup
  char         *name = NULL;
  t_atom       *atoms = NULL;
  t_atom       *atoms_rem = NULL;
  t_dictionary *dict  = NULL;

  // Get the bank to save into the dictionary
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) {
    MY_ERR("io_save:  Arg 0:  The bank to save was not found."); goto MODAL_SPLIT_END; }

  // Get the name under which to save it
  t_symbol *bank_sym = atom_getsym(argv + 1);
  if (bank_sym == sym_empty) {
    MY_ERR("io_save:  Arg 1:  A name to save the bank under is required."); goto MODAL_SPLIT_END; }

  // Set the name for the second bank with the removed resonators
  name = (char *)sysmem_newptr(sizeof(char) * (long)(strlen(bank_sym->s_name) + 4));
  if (name == NULL) { MY_ERR("io_save:  Failed to allocate a temporary string."); goto MODAL_SPLIT_END; }

  strcpy(name, bank_sym->s_name);
  strcat(name, "_rem");
  t_symbol *bank_rem_sym = gensym(name);
  
  POST("Split:  name: %s remainder: %s", bank_sym->s_name, bank_rem_sym->s_name);

  // Test if the main dictionary is found
  dict = dictobj_findregistered_retain(x->dict_sym);
  if (!dict) { MY_ERR("io_save:  There is no dictionary %s.", x->dict_sym->s_name); goto MODAL_SPLIT_END; }

  // Test if a subdictionary for the banks already exists
  t_dictionary *dict_all_banks = NULL;
  dictionary_getdictionary(dict, gensym("banks"), (t_object **)&dict_all_banks);

  // If not create one and append it to the main dictionary
  // Do not free because ownership is passed on to the main dictionary dict
  if (dict_all_banks == NULL) {
    dict_all_banks = dictionary_new();
    dictionary_appenddictionary(dict, gensym("banks"), (t_object *)dict_all_banks);  }

  // Create two arrays of atoms for the resonators
  atoms      = (t_atom *)sysmem_newptr(sizeof(t_atom) * bank->reson_cnt);
  atoms_rem = (t_atom *)sysmem_newptr(sizeof(t_atom) * bank->reson_cnt);

  if ((atoms == NULL) || (atoms_rem == NULL)) {
    MY_ERR("io_save:  Failed to allocate a temporary array of atoms."); goto MODAL_SPLIT_END; }

  // Create a subdictionary for each resonator, put each in one of the array of atoms
  t_resonator  *reson = NULL;
  t_dictionary *dict_reson = NULL;

  t_int32 ind;
  t_atom *p_atom;

  t_int32 cnt1 = 0, cnt2 = 0;
  t_double ampl1_min = bank->ampl_max, ampl2_min = bank->ampl_max;
  t_double ampl1_max = bank->ampl_min, ampl2_max = bank->ampl_min;
  t_double freq1_min = bank->freq_max, freq2_min = bank->freq_max;
  t_double freq1_max = bank->freq_min, freq2_max = bank->freq_min;
  t_double decay1_min = bank->decay_max, decay2_min = bank->decay_max;
  t_double decay1_max = bank->decay_min, decay2_max = bank->decay_min;
  
  for (t_int32 res = 0; res < bank->reson_cnt; res++) {
    reson = bank->reson_arr + res;

    if (reson->mode_ind != MODE_FIX_OFF) {
      ind = cnt1;
      p_atom = atoms + cnt1;
      cnt1++;
      if (reson->a0 < ampl1_min) { ampl1_min = reson->a0; }
      if (reson->a0 > ampl1_max) { ampl1_max = reson->a0; }
      if (reson->freq < freq1_min) { freq1_min = reson->freq; }
      if (reson->freq > freq1_max) { freq1_max = reson->freq; }
      if (reson->decay < decay1_min) { decay1_min = reson->decay; }
      if (reson->decay > decay1_max) { decay1_max = reson->decay; }  }

    else {
      ind = cnt2;
      p_atom = atoms_rem + cnt2;
      cnt2++;
      if (reson->a0 < ampl2_min) { ampl2_min = reson->a0; }
      if (reson->a0 > ampl2_max) { ampl2_max = reson->a0; }
      if (reson->freq < freq2_min) { freq2_min = reson->freq; }
      if (reson->freq > freq2_max) { freq2_max = reson->freq; }
      if (reson->decay < decay2_min) { decay2_min = reson->decay; }
      if (reson->decay > decay2_max) { decay2_max = reson->decay; } }

    dict_reson = dictionary_sprintf("@index %i @ampl %f @freq %f @decay %f @b1 %f @b2 %f",
      ind, reson->a0, reson->freq, reson->decay, reson->b1, reson->b2);
    atom_setobj(p_atom, dict_reson); }

  // If 

  if (cnt1 != 0) {

  // Create a subdictionary for the bank and append it to dict_all_banks. Do not free.
  t_dictionary *dict_bank = dictionary_sprintf("@from %s @reson_cnt %i @gain %f @protected %s \
    @ampl_min %f @ampl_max %f @freq_min %f @freq_max %f @decay_min %f @decay_max %f",
    bank->name->s_name, cnt1, bank->gain, "false",
    ampl1_min, ampl1_max, freq1_min, freq1_max, decay1_min, decay1_max);
  dictionary_appenddictionary(dict_all_banks, bank_sym, (t_object *)dict_bank);

  // Append the array of atoms to dict_bank. The atoms are copied.
  dictionary_appendatoms(dict_bank, gensym("resonators"), cnt1, atoms); }

  if (cnt2 != 0) {

  // Create a subdictionary for the bank and append it to dict_all_banks. Do not free.
  t_dictionary *dict_bank_rem = dictionary_sprintf("@from %s @reson_cnt %i @gain %f @protected %s \
    @ampl_min %f @ampl_max %f @freq_min %f @freq_max %f @decay_min %f @decay_max %f",
    bank->name->s_name, cnt2, bank->gain, "false",
    ampl2_min, ampl2_max, freq2_min, freq2_max, decay2_min, decay2_max);
  dictionary_appenddictionary(dict_all_banks, bank_rem_sym, (t_object *)dict_bank_rem);

  // Append the array of atoms to dict_bank_rem. The atoms are copied.
  dictionary_appendatoms(dict_bank_rem, gensym("resonators"), cnt2, atoms_rem); }
  
  // Send out a message to indicate completion of load
  t_atom mess_arr[5];
  atom_setlong(mess_arr, bank - x->bank_arr);
  atom_setsym(mess_arr + 1, bank_sym);
  atom_setlong(mess_arr + 2, cnt1);
  atom_setsym(mess_arr + 3, bank_rem_sym);
  atom_setlong(mess_arr + 4, cnt2);
  outlet_anything(x->outl_mess, gensym("split"), 5, mess_arr);
  
  // Free the array of atoms and release the main dictionary
MODAL_SPLIT_END:
  if (name)       { sysmem_freeptr(name); }
  if (atoms)     { sysmem_freeptr(atoms); }
  if (atoms_rem) { sysmem_freeptr(atoms_rem); }
  if (dict)       { dictobj_release(dict); }
  return;
}

// ====  METHOD: IO_JOIN  ====
// Join two loaded banks into one.

void io_join(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_join");

  // Get the first bank
  t_bank *bank1 = bank_find(x, argv, sym);
  if (bank1 == NULL) {
    MY_ERR("io_join:  Arg 0:  The first bank to join was not found."); return; }

  // Get the second bank to join with the first
  t_bank *bank2 = bank_find(x, argv + 1, sym);
  if (bank2 == NULL) {
    MY_ERR("io_join:  Arg 1:  The second bank to join was not found."); return; }

  // Reallocate the bank
  t_int32 res_cnt_1 = bank1->reson_cnt;
  if (bank_realloc(x, bank1, bank1->reson_cnt + bank2->reson_cnt) == ERR_ALLOC) { return; }

  // Copy the resonators into the second segment of the resonator array
  for (t_int32 res = 0; res < bank2->reson_cnt; res ++) {
    reson_copy(x, bank1, bank1->reson_arr + res_cnt_1 + res, bank2->reson_arr + res); }

  bank_update(x, bank1);
  bank_sort(x, bank1);

  // Send out a message to indicate completion of load
  t_atom mess_arr[3];
  atom_setlong(mess_arr, bank1 - x->bank_arr);
  atom_setsym(mess_arr + 1, bank1->name);
  atom_setlong(mess_arr + 2, bank1->reson_cnt);
  outlet_anything(x->outl_mess, gensym("join"), 3, mess_arr);
}

// ====  METHOD: IO_RENAME  ====

void io_rename(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_rename");

  // Pointers to structures that need cleanup
  t_dictionary *dict = NULL;

  // Get the name of the bank to look for in the dictionary
  t_symbol *bank_sym = atom_getsym(argv);
  if (bank_sym == sym_empty) { MY_ERR("%s:  Arg 0:  A bank name to look for in the dictionary is required.", sym->s_name); goto MODAL_RENAME_END; }

  // Get the name to rename the saved bank under
  t_symbol *bank_sym_new = atom_getsym(argv + 1);
  if (bank_sym_new == sym_empty) { MY_ERR("%s:  Arg 1:  A new name for the bank is required.", sym->s_name); goto MODAL_RENAME_END; }

  // Test if the main dictionary is found
  dict = dictobj_findregistered_retain(x->dict_sym);
  if (!dict) { MY_ERR("%s:  There is no main dictionary %s.", sym->s_name, x->dict_sym->s_name); goto MODAL_RENAME_END; }

  // Open the subdictionary with all the banks
  t_dictionary *dict_all_banks = NULL;
  dictionary_getdictionary(dict, gensym("banks"), (t_object **)&dict_all_banks);
  if (dict_all_banks == NULL) {
    MY_ERR("%s:  The main dictionary %s does not have a subdictionary for the banks.", sym->s_name, x->dict_sym->s_name); goto MODAL_RENAME_END; }

  // Look for the specific bank we want to rename in the dictionary
  t_dictionary *dict_bank = NULL;
  dictionary_getdictionary(dict_all_banks, bank_sym, (t_object **)&dict_bank);
  if (dict_bank == NULL) {
    MY_ERR("%s:  The bank %s was not found in the main dictionary %s.", sym->s_name, bank_sym->s_name, x->dict_sym->s_name); goto MODAL_RENAME_END; }

  // Chuck the entry and reappend it under a different key
  dictionary_chuckentry(dict_all_banks, bank_sym);
  dictionary_appenddictionary(dict_all_banks, bank_sym_new, (t_object *)dict_bank);

  // Send out a message to indicate completion of rename
  t_atom mess_arr[2];
  atom_setsym(mess_arr, bank_sym);
  atom_setsym(mess_arr + 1, bank_sym_new);
  outlet_anything(x->outl_mess, gensym("rename"), 2, mess_arr);
  
  // Release the main dictionary
  MODAL_RENAME_END:
  if (dict)   { dictobj_release(dict); }
  return;
}

// ====  METHOD: IO_DELETE  ====

void io_delete(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_delete");

  // Pointers to structures that need cleanup
  t_dictionary *dict = NULL;

  // Get the name of the bank to look for in the dictionary
  t_symbol *bank_sym = atom_getsym(argv);
  if (bank_sym == sym_empty) { MY_ERR("%s:  Arg 0:  A bank name to look for in the dictionary is required.", sym->s_name); goto MODAL_DELETE_END; }

  // Test if the main dictionary is found
  dict = dictobj_findregistered_retain(x->dict_sym);
  if (!dict) { MY_ERR("%s:  There is no main dictionary %s.", sym->s_name, x->dict_sym->s_name); goto MODAL_DELETE_END; }

  // Open the subdictionary with all the banks
  t_dictionary *dict_all_banks = NULL;
  dictionary_getdictionary(dict, gensym("banks"), (t_object **)&dict_all_banks);
  if (dict_all_banks == NULL) {
    MY_ERR("%s:  The main dictionary %s does not have a subdictionary for the banks.", sym->s_name, x->dict_sym->s_name); goto MODAL_DELETE_END; }

  // Look for the specific bank we want to delete in the dictionary
  t_dictionary *dict_bank = NULL;
  dictionary_getdictionary(dict_all_banks, bank_sym, (t_object **)&dict_bank);
  if (dict_bank == NULL) {
    MY_ERR("%s:  The bank %s was not found in the main dictionary %s.", sym->s_name, bank_sym->s_name, x->dict_sym->s_name); goto MODAL_DELETE_END; }

  // Chuck the entry and reappend it under a different key
  dictionary_deleteentry(dict_all_banks, bank_sym);

  // Send out a message to indicate completion of delete
  t_atom mess_arr[1];
  atom_setsym(mess_arr, bank_sym);
  outlet_anything(x->outl_mess, gensym("delete"), 1, mess_arr);
  
  // Release the main dictionary
  MODAL_DELETE_END:
  if (dict)   { dictobj_release(dict); }
  return;
}

// ====  METHOD: IO_CLEAR  ====
// Clear a bank of resonators, deallocating all the arrays.
// Arguments:  int/sym
//   Arg 0:  The bank to clear information from (int/sym):  index / name

void io_clear(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("io_clear");

  // The number of arguments should be one
  if (argc == 1) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      // Free the existing bank and create a new one
      bank_free(x, bank);
      if (bank_new(x, bank, 1) == ERR_ALLOC) { return; }

      // Send out a message to indicate completion of clearing
      t_atom mess_arr[3];
      atom_setlong(mess_arr, bank - x->bank_arr);
      atom_setsym(mess_arr + 1, bank->name);
      atom_setlong(mess_arr + 2, bank->reson_cnt);
      outlet_anything(x->outl_mess, gensym("clear"), 3, mess_arr);

      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym", sym->s_name);
  MY_ERR2("  Arg 0:  The bank to clear (int/sym):  index / name");
  return;
}

// ====  METHOD: MODAL_INFO  ====
// Send information from a bank of resonators into two messages.
// Arguments:  int/sym
//   Arg 0:  The bank to send information from (int/sym):  index / name
// Output:  a message
//   Mess 1:  "show1", index, name, number of modes

void modal_info(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_info");

  // The first argument should reference a bank of resonator
  t_bank *bank;
  if ((argc >= 1) && ((bank = bank_find(x, argv, sym)) != NULL)) {

    x->bank_cur = bank;

    // Send a first message out with the name and number of resonators
    t_atom mess_arr[8];

    atom_setlong(mess_arr, bank - x->bank_arr);
    atom_setsym(mess_arr + 1, bank->name);
    atom_setlong(mess_arr + 2, bank->reson_cnt);
    atom_setfloat(mess_arr + 3, bank->gain);
    atom_setfloat(mess_arr + 4, bank->velocity);
    atom_setfloat(mess_arr + 5, bank->ampl_mult);
    atom_setfloat(mess_arr + 6, bank->freq_shift);
    atom_setfloat(mess_arr + 7, bank->decay_mult);

    outlet_anything(x->outl_mess, gensym("info"), 8, mess_arr);

    return; }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym (sym)", sym->s_name);
  MY_ERR2("  Arg 0:  The bank to send information from (int/sym):  index / name");
  return;
}

// ====  METHOD: MODAL_PARAM  ====
// Send information from a bank of resonators into two messages.
// Arguments:  int/sym [sym]
//   Arg 0:  The bank to send information from (int/sym):  index / name
//   Arg 1:  Optional type of sorting to apply: "ampl" / "freq" / "decay"
// Output:  a message
//   "param", amplitudes, frequencies, decays (3*n)

void modal_param(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_param");
  
  t_atom *mess_arr = NULL;

  // The first argument should reference a bank of resonator
  t_bank *bank;
  if ((argc >= 1) && ((bank = bank_find(x, argv, sym)) != NULL)) {

      mess_arr = (t_atom *)sysmem_newptr(sizeof(t_atom) * 4 * bank->reson_cnt);
      if (mess_arr == NULL) {
        MY_ERR("modal_param:  Failed to allocate an array of messages."); goto MODAL_PARAM_END; }

    // The second argument should be empty...
    if (argc == 1) {

      t_atom *atom = mess_arr;
      for (int i = 0; i < bank->reson_cnt; i++) {

        atom_setlong(atom++, i);
        atom_setfloat(atom++, (bank->reson_arr + i)->a0);
        atom_setfloat(atom++, (bank->reson_arr + i)->freq);
        atom_setfloat(atom++, (bank->reson_arr + i)->decay); }

      outlet_anything(x->outl_mess, gensym("param"), 4 * bank->reson_cnt, mess_arr);
      goto MODAL_PARAM_END;  }

    // Or a symbol to indicate the type of sorting
    else if (argc == 2) {

      t_symbol *sort = atom_getsym(argv + 1);
      t_int32 *sort_arr = NULL;

      if      (sort == sym_freq)  { sort_arr = bank->sort_freq; }
      else if (sort == sym_decay) { sort_arr = bank->sort_decay; }
      else if (sort == sym_ampl)  { sort_arr = bank->sort_ampl; }

      if (sort_arr != NULL) {

        t_atom *atom = mess_arr;
        for (int i = 0; i < bank->reson_cnt; i++) {

          atom_setlong(atom++, i);
          atom_setfloat(atom++, (bank->reson_arr + sort_arr[i])->a0);
          atom_setfloat(atom++, (bank->reson_arr + sort_arr[i])->freq);
          atom_setfloat(atom++, (bank->reson_arr + sort_arr[i])->decay); }
    
      outlet_anything(x->outl_mess, gensym("param"), 4 * bank->reson_cnt, mess_arr);
      goto MODAL_PARAM_END; } } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym (sym)", sym->s_name);
  MY_ERR2("  Arg 0:  The bank to send information from (int/sym):  index / name");

  // Free the array of atoms and release the main dictionary
  MODAL_PARAM_END:
  if (mess_arr) { sysmem_freeptr(mess_arr); }
  return;
}

// ====  METHOD: MODAL_POST  ====
// Post information from a bank of resonators.
// Arguments:  int/sym
//   Arg 0:  The bank to send information from (int/sym):  index / name

void modal_post(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_post");
  
  // The number of arguments should be one
  if (argc == 1) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      // Post all the information
      POST("Resonator bank %i:", bank - x->bank_arr);
      POST("  Name: %s - Modes: %i - Gain: %f", bank->name->s_name, bank->reson_cnt, bank->gain);

      for (int i = 0; i < bank->reson_cnt; i++) {
        POST("  Res %i:  Ampl = %.2f  Freq = %.0f  Decay = %.2f  B1 = %.2f  B2 = %.2f  Y(n-1) = %.2f  Y(n-2) = %.2f",
          i, (bank->reson_arr + i)->a0, (bank->reson_arr + i)->freq, (bank->reson_arr + i)->decay,
          (bank->reson_arr + i)->b1, (bank->reson_arr + i)->b2,
          (bank->reson_arr + i)->y_m1, (bank->reson_arr + i)->y_m2); }

      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym", sym->s_name);
  MY_ERR2("  Arg 0:  The bank to post information from (int/sym):  index / name");
  return;
}

// ====  METHOD: MODAL_FLUSH  ====
// Flush a bank of resonators, setting the arrays of (n-1) and (n-2) values to zero.
// Arguments:  int/sym
//   Arg 0:  The bank to flush information from (int/sym):  index / name

void modal_flush(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_flush");

  // The number of arguments should be one
  if (argc == 1) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      // Reset the (n-1) and (n-2) values of the filters to zero
      for (int i = 0; i < bank->reson_cnt; i++) {
        (bank->reson_arr + i)->y_m1 = 0.0;
        (bank->reson_arr + i)->y_m2 = 0.0; }
      
      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym", sym->s_name);
  MY_ERR2("  Arg 0:  The bank to flush information from (int/sym):  index / name");
  return;
}

// ====  METHOD: MODAL_MASTER  ====
// Set the master gain for the whole object.
// Arguments:  Float
//   Arg 0:  Float - The master gain of the whole object

void modal_master(t_modal *x, t_double gain) {

  TRACE("modal_master");

  x->master = gain * MASTER_MULT;
}

// ====  METHOD: MODAL_GAIN  ====
// Set the gain of bank of resonator.
// Arguments:  Int Float
//   Arg 0:  Int - The index of the bank
//   Arg 1:  Float - The gain of the resonator bank

void modal_gain(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_gain");

  t_int32 index = (t_int32)atom_getlong(argv);

  (x->bank_arr + index)->gain = atom_getfloat(argv + 1);
}

// ====  METHOD: MODAL_IS_ON  ====
// Set the bank of resonators on or off.
// Arguments:  Int Int
//   Arg 0:  Int - The index of the bank
//   Arg 1:  Int - To set the bank on or off

void modal_is_on(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_is_on");

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  t_bool is_on = (t_bool)atom_getlong(argv + 1);
  bank->is_on = is_on;
}

// ====  METHOD: MODAL_AMPL_MULT  ====
// Set the amplitude multiplier of bank of resonator.
// Arguments:  Int Float
//   Arg 0:  Int - The index of the bank
//   Arg 1:  Float - The amplitude multiplier of the resonator bank

void modal_ampl_mult(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_ampl_mult");

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  bank->ampl_mult = atom_getfloat(argv + 1);

  /*t_resonator *reson = bank->reson_arr + bank->reson_cnt;

  while (reson-- != bank->reson_arr) {
    reson->a0 = reson->ampl_ref * bank->ampl_mult; }*/
}

// ====  METHOD: MODAL_FREQ_MULT  ====
// Set the frequency multiplier of bank of resonator.
// Arguments:  Int Float
//   Arg 0:  Int - The index of the bank
//   Arg 1:  Float - The frequency multiplier of the resonator bank

void modal_freq_shift(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_freq_shift");

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  bank->freq_shift = atom_getfloat(argv + 1);
  bank->freq_mult = pow(2, bank->freq_shift / 12);

  bank_update(x, bank);
}

// ====  METHOD: MODAL_DECAY_MULT  ====
// Set the decay multiplier of bank of resonator.
// Arguments:  Int Float
//   Arg 0:  Int - The index of the bank
//   Arg 1:  Float - The decay multiplier of the resonator bank

void modal_decay_mult(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("modal_decay_mult");

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  bank->decay_mult = atom_getfloat(argv + 1);

  bank_update(x, bank);
}

// ====  METHOD: MODAL_GET_AMPL_RNG  ====

void modal_get_ampl_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  t_atom mess_arr[2];
  atom_setfloat(mess_arr, bank->ampl_min);
  atom_setfloat(mess_arr + 1, bank->ampl_max);
  outlet_anything(x->outl_mess, gensym("ampl_rng"), 2, mess_arr);
}

// ====  METHOD: MODAL_GET_FREQ_RNG  ====

void modal_get_freq_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  t_atom mess_arr[2];
  atom_setfloat(mess_arr, bank->freq_min);
  atom_setfloat(mess_arr + 1, bank->freq_max);
  outlet_anything(x->outl_mess, gensym("freq_rng"), 2, mess_arr);
}

// ====  METHOD: MODAL_GET_DECAY_RNG  ====

void modal_get_decay_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  t_atom mess_arr[2];
  atom_setfloat(mess_arr, bank->decay_min);
  atom_setfloat(mess_arr + 1, bank->decay_max);
  outlet_anything(x->outl_mess, gensym("decay_rng"), 2, mess_arr);
}

// ====  METHOD: MODAL_SEL_ALL  ====

void modal_sel_all(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The number of arguments should be one
  if (argc == 1) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      for (t_int32 i = 0; i < bank->reson_cnt; i++) { ; }
      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym", sym->s_name);
  MY_ERR2("  Arg 0:  The bank for which to select all the resonators (int/sym):  index / name");
  return;
}

// ====  METHOD: MODAL_SEL_INIT  ====

__inline void modal_sel_init(t_modal *x, t_bank *bank) {

  bank->sel_ampl_min = bank->ampl_max;
  bank->sel_ampl_max = bank->ampl_min;
  bank->sel_freq_min = bank->freq_max;
  bank->sel_freq_max = bank->freq_min;
  bank->sel_decay_min = bank->decay_max;
  bank->sel_decay_max = bank->decay_min;
}

// ====  METHOD: MODAL_SEL_COMPARE  ====

__inline void modal_sel_compare(t_modal *x, t_bank *bank, t_resonator *reson) {

  bank->sel_ampl_min = MIN(bank->sel_ampl_min, reson->a0);
  bank->sel_ampl_max = MAX(bank->sel_ampl_max, reson->a0);
  bank->sel_freq_min = MIN(bank->sel_freq_min, reson->freq);
  bank->sel_freq_max = MAX(bank->sel_freq_max, reson->freq);
  bank->sel_decay_min = MIN(bank->sel_decay_min, reson->decay);
  bank->sel_decay_max = MAX(bank->sel_decay_max, reson->decay);
}

// ====  METHOD: MODAL_SEL_OUT  ====

__inline void modal_sel_out(t_modal *x, t_bank *bank, t_int32 cnt) {

  t_atom mess_arr[7];

  atom_setlong(mess_arr, cnt);

  if (cnt) {
    atom_setfloat(mess_arr + 1, bank->sel_ampl_min);
    atom_setfloat(mess_arr + 2, bank->sel_ampl_max);
    atom_setfloat(mess_arr + 3, bank->sel_freq_min);
    atom_setfloat(mess_arr + 4, bank->sel_freq_max);
    atom_setfloat(mess_arr + 5, bank->sel_decay_min);
    atom_setfloat(mess_arr + 6, bank->sel_decay_max);
    outlet_anything(x->outl_mess, gensym("select"), 7, mess_arr); }

  else { outlet_anything(x->outl_mess, gensym("select"), 1, mess_arr); }
}

// ====  METHOD: MODAL_SEL_AMPL_IND  ====

void modal_sel_ampl_ind(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The number of arguments should be three
  if (argc == 3) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      t_int32 min = (t_int32)atom_getlong(argv + 1);
      t_int32 max = (t_int32)atom_getlong(argv + 2);

      modal_sel_init(x, bank);

      for (t_int32 i = 0; i < min; i++)                { ; }
      for (t_int32 i = max; i < bank->reson_cnt; i++) {  ; }

      for (t_int32 i = min; i < max; i++) {
        t_resonator *reson = bank->reson_arr + bank->sort_ampl[i];
        ;
        modal_sel_compare(x, bank, reson); }

      modal_sel_out(x, bank, max - min);
      
      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym int int", sym->s_name);
  MY_ERR2("  Arg 0:  The bank for which to select a range of resonators (int/sym):  index / name");
  MY_ERR2("  Arg 1:  The lower index of the range ordered by amplitude:  int");
  MY_ERR2("  Arg 2:  The upper index of the range ordered by amplitude:  int");
  return;
}

// ====  METHOD: MODAL_SEL_AMPL_RNG  ====

void modal_sel_ampl_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  ;
}

// ====  METHOD: MODAL_SEL_FREQ_IND  ====

void modal_sel_freq_ind(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The number of arguments should be three
  if (argc == 3) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      t_int32 min = (t_int32)atom_getlong(argv + 1);
      t_int32 max = (t_int32)atom_getlong(argv + 2);

      modal_sel_init(x, bank);

      for (t_int32 i = 0; i < min; i++)                { ; }
      for (t_int32 i = max; i < bank->reson_cnt; i++) { ; }

      for (t_int32 i = min; i < max; i++) {
        t_resonator *reson = bank->reson_arr + bank->sort_freq[i];
        ;
        modal_sel_compare(x, bank, reson); }

      modal_sel_out(x, bank, max - min);

      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym int int", sym->s_name);
  MY_ERR2("  Arg 0:  The bank for which to select a range of resonators (int/sym):  index / name");
  MY_ERR2("  Arg 1:  The lower index of the range ordered by frequency:  int");
  MY_ERR2("  Arg 2:  The upper index of the range ordered by frequency:  int");
  return;
}

// ====  METHOD: MODAL_SEL_FREQ_RNG  ====

void modal_sel_freq_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The number of arguments should be three
  if (argc == 3) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      t_double min = (t_int32)atom_getfloat(argv + 1);
      t_double max = (t_int32)atom_getfloat(argv + 2);
  
      modal_sel_init(x, bank);

      t_int32 i = 0, cnt = 0;
      while (((bank->reson_arr + bank->sort_freq[i])->freq < min) && (i++ < bank->reson_cnt)) { ; }

      while (((bank->reson_arr + bank->sort_freq[i])->freq < max) && (i++ < bank->reson_cnt)) {
        t_resonator *reson = bank->reson_arr + bank->sort_freq[i];
        ;
        cnt++;
        modal_sel_compare(x, bank, reson); }
      
      while (i++ < bank->reson_cnt) { ; }

      modal_sel_out(x, bank, cnt);

      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym float float", sym->s_name);
  MY_ERR2("  Arg 0:  The bank for which to select a range of resonators (int/sym):  index / name");
  MY_ERR2("  Arg 1:  The lower value of the range ordered by frequency:  float");
  MY_ERR2("  Arg 2:  The upper value of the range ordered by frequency:  float");
  return;    
}

// ====  METHOD: MODAL_SEL_DECAY_IND  ====

void modal_sel_decay_ind(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  // The number of arguments should be three
  if (argc == 3) {

    // The first argument should reference a bank of resonator
    t_bank *bank = bank_find(x, argv, sym);
    if (bank != NULL) {

      t_int32 min = (t_int32)atom_getlong(argv + 1);
      t_int32 max = (t_int32)atom_getlong(argv + 2);
  
      modal_sel_init(x, bank);

      for (t_int32 i = 0; i < min; i++)                { ; }
      for (t_int32 i = max; i < bank->reson_cnt; i++)  { ; }

      for (t_int32 i = min; i < max; i++) {
        t_resonator *reson = bank->reson_arr + bank->sort_decay[i];
        ;
        modal_sel_compare(x, bank, reson); }

      modal_sel_out(x, bank, max - min);

      return; } }

  // Otherwise the arguments are invalid
  MY_ERR("%s:  Invalid arguments. The method expects:  int/sym int int", sym->s_name);
  MY_ERR2("  Arg 0:  The bank for which to select a range of resonators (int/sym):  index / name");
  MY_ERR2("  Arg 1:  The lower index of the range ordered by decay:  int");
  MY_ERR2("  Arg 2:  The upper index of the range ordered by decay:  int");
  return;    
}

// ====  METHOD: MODAL_SEL_DECAY_RNG  ====

void modal_sel_decay_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  ;
}

// ========  RESONATOR METHODS  ========

// ====  METHOD: RESON_NEW  ====

void reson_new(t_modal *x, t_bank *bank, t_resonator *reson) {

  TRACE("reson_new");

  reson->ampl_ref   = 1.0;
  reson->freq_ref   = 400;
  reson->decay_ref = 1000;

  reson_update(x, bank, reson);
  reson->y_m1 = 0.0;
  reson->y_m2 = 0.0;

  reson->in_U_cur  = 0.0;
  reson->in_A_cur  = 0.0;
  reson->in_U_targ = 0.0;
  reson->in_A_targ = 0.0;

  reson->out_A_cur  = 1.0;
  reson->out_A_targ  = 1.0;

  reson->mode_ind  = MODE_FIX_OFF;
  reson->mode_type  = (x->mode_arr + reson->mode_ind)->type;
  reson->cntd        = INDEFINITE;
  reson->cntd_type  = MODE_CNTD_BANK;

  for (t_int32 t = 0; t < 4; t++) {
    reson->times[t]      = random_int(bank->times[2 * t], bank->times[2 * t + 1]);
    reson->times[t + 4] = random_int(bank->times[2 * t], bank->times[2 * t + 1]); }

  reson->diff_type = MODE_DIFF_ONE_RR;
  for (t_int32 ch = 0; ch < 8; ch++) { reson->diff_mult[ch]  = 0.0; }
  reson->diff_ind = rand() % 8;
  reson->diff_mult[reson->diff_ind] = 1.0;
  reson->diff_cnt = 1;
  reson->diff_chg = false;

  reson->rms = 0;
}

// ====  METHOD: RESON_FREE  ====

void reson_free(t_modal *x, t_resonator *reson) {

  TRACE("reson_free");

  return;
}

// ====  METHOD: RESON_COPY  ====

void reson_copy(t_modal *x, t_bank *bank, t_resonator *reson, t_resonator *reson_src) {

  TRACE("reson_copy");

  reson->ampl_ref  = reson_src->ampl_ref;
  reson->freq_ref  = reson_src->freq_ref;
  reson->decay_ref = reson_src->decay_ref;

  reson_update(x, bank, reson);
}

// ====  METHOD: RESON_UPDATE  ====

void reson_update(t_modal *x, t_bank *bank, t_resonator *reson) {

  reson->a0     = reson->ampl_ref  * bank->ampl_mult;
  reson->freq  = reson->freq_ref  * bank->freq_mult;
  reson->decay = reson->decay_ref * bank->decay_mult;

  t_double r = exp(-reson->decay / x->samplerate);
  reson->b1 = 2 * r * cos(TWOPI * reson->freq / x->samplerate);
  reson->b2 = -r * r;
}

// ========  BANK METHODS  ========

// ====  METHOD: BANK_NEW  ====
// Allocate all the arrays for a bank of resonators.

t_int32 bank_new(t_modal *x, t_bank *bank, t_int32 nb) {

  TRACE("bank_new");

  // Set pointers to NULL
  bank->reson_arr   = NULL;
  bank->sort_ampl  = NULL;
  bank->sort_freq  = NULL;
  bank->sort_decay = NULL;

  // Check the validity of the number of resonators
  if (nb < 1) {
    MY_ERR("bank_new:  Invalid number of resonators: %i. Should be at least 1.", nb);
    return ERR_ALLOC; }
  if (nb > x->reson_max) {
    MY_ERR("bank_new:  Invalid number of resonators: %i. Should be at most %i.", x->reson_max, nb);
    return ERR_ALLOC; }

  // Variable initialization
  bank->is_on      = false;
  bank->is_frozen = false;
  bank->name      = sym_free;
  bank->gain      = 1.0;
  bank->reson_cnt = nb;
  bank->velocity  = 1.0;

  bank->ampl_mult   = 1.0;    // These need to be set before calling reson_new
  bank->freq_mult  = 1.0;
  bank->decay_mult = 1.0;

  // Set up mode tree (before calling reson_new)
  _mode_new(x, bank);

  // Memory allocation for the resonators
  bank->reson_arr  = (t_resonator *)sysmem_newptr(sizeof(t_resonator) * bank->reson_cnt);
  if (bank->reson_arr == NULL) { MY_ERR("bank_new:  Failed to allocate reson_arr."); return ERR_ALLOC; }

  // Resonator initialization
  for (int i = 0; i < bank->reson_cnt; i++) { reson_new(x, bank, bank->reson_arr + i); }

  // Memory allocation for sorting
  bank->sort_ampl   = (t_int32 *)sysmem_newptr(sizeof(t_int32) * bank->reson_cnt);
  if (bank->sort_ampl == NULL) { MY_ERR("bank_new:  Failed to allocate sort_ampl."); return ERR_ALLOC; }

  bank->sort_freq = (t_int32 *)sysmem_newptr(sizeof(t_int32) * bank->reson_cnt);
  if (bank->sort_freq == NULL) { MY_ERR("bank_new:  Failed to allocate sort_freq."); return ERR_ALLOC; }

  bank->sort_decay = (t_int32 *)sysmem_newptr(sizeof(t_int32) * bank->reson_cnt);
  if (bank->sort_decay == NULL) { MY_ERR("bank_new:  Failed to allocate sort_decay."); return ERR_ALLOC; }

  // Sort the resonators by amplitude, frequency and decay
  bank_sort(x, bank);

  return ERR_NONE;
}

// ====  METHOD: BANK_REALLOC  ====
// Reallocate all the arrays for a bank of resonators.

t_int32 bank_realloc(t_modal *x, t_bank *bank, t_int32 nb) {

  TRACE("bank_realloc");

  // Check the validity of the number of resonators
  if (nb < 1) {
    MY_ERR("bank_realloc:  Invalid number of resonators: %i. Should be at least 1.", nb);
    return ERR_ALLOC; }
  if (nb > x->reson_max) {
    MY_ERR("bank_realloc:  Invalid number of resonators: %i. Should be at most %i.", x->reson_max, nb);
    return ERR_ALLOC; }

  // Variable initialization
  bank->is_on      = false;
  //bank->is_frozen = false;
  //bank->gain      = 1.0;

  bank->ampl_mult   = 1.0;    // These need to be set before calling reson_new
  bank->freq_mult  = 1.0;
  bank->decay_mult = 1.0;

  // Memory allocation for new array of resonators
  t_resonator *new_reson_arr =  (t_resonator *)sysmem_newptr(sizeof(t_resonator) * nb);
  if (new_reson_arr == NULL) { MY_ERR("bank_realloc:  Failed to allocate new_reson_arr."); return ERR_ALLOC; }

  // Copy the current resonators
  for (t_int32 res = 0; res < nb; res++) { reson_new(x, bank, new_reson_arr + res); }
  for (t_int32 res = 0; res < min(nb, bank->reson_cnt); res++){
    reson_copy(x, bank, new_reson_arr + res, bank->reson_arr + res); }
  
  // Free the current array of resonators and set pointer to the new array
  if (bank->reson_arr) { sysmem_freeptr(bank->reson_arr); }
  bank->reson_arr = new_reson_arr;
  bank->reson_cnt = nb;

  // Memory reallocation for sorting
  bank->sort_ampl   = (t_int32 *)sysmem_resizeptrclear(bank->sort_ampl, sizeof(t_int32) * bank->reson_cnt);
  if (bank->sort_ampl == NULL) { MY_ERR("bank_realloc:  Failed to reallocate sort_ampl."); return ERR_ALLOC; }

  bank->sort_freq = (t_int32 *)sysmem_resizeptrclear(bank->sort_freq, sizeof(t_int32)* bank->reson_cnt);
  if (bank->sort_freq == NULL) { MY_ERR("bank_realloc:  Failed to reallocate sort_freq."); return ERR_ALLOC; }

  bank->sort_decay = (t_int32 *)sysmem_resizeptrclear(bank->sort_decay, sizeof(t_int32)* bank->reson_cnt);
  if (bank->sort_decay == NULL) { MY_ERR("bank_realloc:  Failed to reallocate sort_decay."); return ERR_ALLOC; }

  // Sort the resonators by amplitude, frequency and decay
  bank_sort(x, bank);

  return ERR_NONE;
}

// ====  METHOD: BANK_FREE  ====
// Free all the arrays of a bank of resonators.

void bank_free(t_modal *x, t_bank *bank) {

  TRACE("bank_free");

  if (bank->reson_arr)  { sysmem_freeptr(bank->reson_arr); }
  if (bank->sort_ampl)  { sysmem_freeptr(bank->sort_ampl); }
  if (bank->sort_freq)  { sysmem_freeptr(bank->sort_freq); }
  if (bank->sort_decay) { sysmem_freeptr(bank->sort_decay); }
}

// ====  METHOD: COMPARE_AMPL  ====
// Method used for sorting resonators by amplitude, used with qsort_s

int compare_ampl(void *bank, const t_int32 *index1, const t_int32 *index2) {

  if ((((t_bank*)bank)->reson_arr + *index1)->a0 < (((t_bank*)bank)->reson_arr + *index2)->a0) { return 1; }
  else { return -1; }
}

// ====  METHOD: COMPARE_FREQ  ====
// Method used for sorting resonators by frequency, used with qsort_s

int compare_freq(void *bank, const t_int32 *index1, const t_int32 *index2) {

  if ((((t_bank*)bank)->reson_arr + *index1)->freq > (((t_bank*)bank)->reson_arr + *index2)->freq) { return 1; }
  else { return -1; }
}

// ====  METHOD: COMPARE_DECAY  ====
// Method used for sorting resonators by decay, used with qsort_s

int compare_decay(void *bank, const t_int32 *index1, const t_int32 *index2) {

  if ((((t_bank*)bank)->reson_arr + *index1)->decay < (((t_bank*)bank)->reson_arr + *index2)->decay) { return 1; }
  else { return -1; }
}

// ====  METHOD: BANK_SORT  ====
// Sort the resonators by amplitude, frequency and decay

void bank_sort(t_modal *x, t_bank *bank) {

  TRACE("bank_sort");

  // Initializing the arrays for sorting
  for (int i = 0; i < bank->reson_cnt; i++){
    bank->sort_ampl[i]  = i;
    bank->sort_freq[i]  = i;
    bank->sort_decay[i]  = i; }

  // Sort the arrays of indexes for amplitude, frequency, and decay values
  qsort_s(bank->sort_ampl, bank->reson_cnt, sizeof(t_int32), compare_ampl, bank);
  qsort_s(bank->sort_freq, bank->reson_cnt, sizeof(t_int32), compare_freq, bank);
  qsort_s(bank->sort_decay, bank->reson_cnt, sizeof(t_int32), compare_decay, bank);

  // Set the ranges for amplitude, frequency, and decay values
  bank->ampl_min  = (bank->reson_arr + bank->sort_ampl[bank->reson_cnt - 1])->a0;
  bank->ampl_max  = (bank->reson_arr + bank->sort_ampl[0])->a0;
  bank->freq_min  = (bank->reson_arr + bank->sort_freq[0])->freq;
  bank->freq_max  = (bank->reson_arr + bank->sort_freq[bank->reson_cnt - 1])->freq;
  bank->decay_min = (bank->reson_arr + bank->sort_decay[bank->reson_cnt - 1])->decay;
  bank->decay_max = (bank->reson_arr + bank->sort_decay[0])->decay;

  bank->sel_ampl_min  = bank->ampl_min;
  bank->sel_ampl_max  = bank->ampl_max;
  bank->sel_freq_min  = bank->freq_min;
  bank->sel_freq_max  = bank->freq_max;
  bank->sel_decay_min = bank->decay_min;
  bank->sel_decay_max = bank->decay_max;
}

// ====  METHOD: BANK_UPDATE  ====
// Used to update the parameters of the resonators.
// Amplitude, frequency and decay have to be already defined.

__inline void bank_update(t_modal *x, t_bank *bank) {

  TRACE("bank_update");

  for (t_int32 i = 0; i < bank->reson_cnt; i++) { reson_update(x, bank, bank->reson_arr + i); }
}

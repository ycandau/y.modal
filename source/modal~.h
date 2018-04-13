#ifndef YC_MODAL_H_
#define YC_MODAL_H_

// ========  HEADER FILE FOR MISCELLANEOUS MAX UTILITIES  ========

#include "ext.h"
#include "max_util.h"
#include "envelopes.h"
#include "random.h"
#include "dict.h"
#include <time.h>

// ========  DEFINES  ========

#define BANK_CNT_DEF  10   // Default number of banks
#define RESON_MAX_DEF 150  // Default maximum number of modes
#define STATE_CNT_DEF 10   // Default number of states

#define INDEFINITE -1      // To bypass ramping

#define MASTER_MULT 0.01   // Default for master multiplier

// ========  STRUCTURES  ========

typedef struct _state     t_state;
typedef struct _mode      t_mode;
typedef struct _modal     t_modal;
typedef struct _bank      t_bank;
typedef struct _resonator t_resonator;

typedef void (*t_action)(t_modal *x, t_bank *bank, t_resonator *reson, t_mode *mode);

// ========  STRUCTURE:  STATE  ========
// Used to store a state

typedef struct _state {

  t_int32   cnt;
  t_double* U_arr;
  t_double* A_arr;
  t_symbol* name;
  t_symbol* from;
  
} t_state;

// ========  STRUCTURE:  MODE  ========
// Used to create mode graphs

typedef enum _mode_ind {

  MODE_FIX_CHG,   // Fixed modes: used in all_on, all_of, and 
  MODE_FIX_ON,    // individual resonator control: resonator on / off / toggle
  MODE_FIX_DOWN,
  MODE_FIX_OFF,
  MODE_CYC_WAIT,
  MODE_CYC_UP,    // Cycling mode: Ramping UP to 1
  MODE_CYC_ON,    // Cycling mode: Fixed at 1
  MODE_CYC_DOWN,
  MODE_CYC_OFF,
  MODE_LAST

} t_mode_ind;  // MODE_LAST is used to define a static array in t_modal

typedef enum _mode_type {

  MODE_TYPE_OFF,     // Resonator is off: no processing in the perform function
  MODE_TYPE_FIX,     // Resonator is fixed: no ramping
  MODE_TYPE_VAR_A,   // Resonator is variable: amplitude ramping
  MODE_TYPE_VAR_P,   // Resonator is variable: parameter ramping
  MODE_TYPE_VAR_AP,  // Resonator is variable: amplitude and parameter ramping
  MODE_TYPE_VAR_S    // Resonator is variable: pitch shifting

} t_mode_type;

typedef struct _mode {

  t_symbol*   name;
  t_mode_ind  index;
  t_mode_type type;
  t_mode_ind  next_ind;

  t_double ampl_out;
  t_double ampl_in;
  t_double time_min;
  t_double time_max;
  t_int32  time_ind;
  t_action func_open;
  t_action func_close;

} t_mode;

// ========  STRUCTURE:  RESONATOR  ========
// One resonator

typedef enum _diff_type {

  MODE_DIFF_ALL,
  MODE_DIFF_ONE_S,
  MODE_DIFF_ONE_R,
  MODE_DIFF_ONE_RR,
  MODE_DIFF_NUM_R,
  MODE_DIFF_NUM_RR,
  MODE_DIFF_MATR,
  MODE_DIFF_FUNC

} t_diff_type;

// Countdown type: used for cycling modes

typedef enum _cntd_type
{
  // MODE_CNTD_INDEF,    // No countdown, indefinite time
  MODE_CNTD_RESON,    // Get countdown values from resonator
  MODE_CNTD_BANK,      // Get countdown values from bank
  // MODE_CNTD_MODE      // Get countdown values from mode

} t_cntd_type;

typedef struct _resonator {

  t_double a0;     // Resonator coefficients for x(n)
  t_double b1;     // Resonator coefficients for y(n-1)
  t_double b2;     // Resonator coefficients for y(n-2)

  t_double y_m1;   // Stores previous values y(n-1)
  t_double y_m2;   // Stores previous values y(n-2)

  t_double freq;   // Resonator frequencies
  t_double decay;  // Resonator decays
  
  t_double ampl_ref;
  t_double freq_ref;
  t_double decay_ref;
  
  t_double freq_tmp;  // For pitch shifting, to state the initial value

  t_double in_U_cur;   // For input amplitude: current abscissa value: 0 to 1
  t_double in_A_cur;   // Current ordinate value: amplitude, 0 to 1
  t_double in_U_targ;  // For input amplitude: target abscissa value: 0 to 1
  t_double in_A_targ;  // Target ordinate value: amplitude, 0 to 1

  t_int32     cntd;       // Countdown remaining in samples
  t_cntd_type cntd_type;  // Indicate where to get time values from

  t_double out_A_cur;   // Current amplitude multiplier for cycling (ramped)
  t_double out_A_targ;  // Target amplitude multiplier for cycling (ramped)

  t_diff_type diff_type;
  t_double    diff_mult[8];
  t_int32     diff_sto;      // Store the diffusion channel index
  t_bool      diff_chg;      // Indicate diffusion channels have changed
  t_int32     diff_ind;      // For output: index of diffusion channel
  t_int32     diff_cnt;      // For output: number of diffusion channels
  
  t_mode_ind  mode_ind;
  t_mode_type mode_type;

  t_int32  times[8];
  t_double param[2];

  t_double rms;

} t_resonator;

// ========  STRUCTURE:  BANK  ========
// Bank of resonators

typedef struct _bank {

  t_resonator* reson_arr;  // Array of resonators
  t_int32      reson_cnt;  // Number of resonators in the bank

  t_bool    is_on;      // Whether the bank is on or off
  t_bool    is_frozen;
  t_symbol* name;       // Name of the bank
  t_double  gain;       // The gain of the bank

  t_double  ampl_mult;   // Amplitude multiplier for the bank
  t_double  freq_mult;   // Frequency multiplier for the bank
  t_double  freq_shift;
  t_double  decay_mult;  // Decay multiplier for the bank

  t_int32* sort_ampl;   // An array to sort the resonators by amplitude
  t_int32* sort_freq;   // An array to sort the resonators by frequency
  t_int32* sort_decay;  // An array to sort the resonators by decay

  t_double velocity;  // Velocity multiplier to affect rate of change

  t_double ampl_min;
  t_double ampl_max;
  t_double freq_min;
  t_double freq_max;
  t_double decay_min;
  t_double decay_max;

  t_double sel_ampl_min;
  t_double sel_ampl_max;
  t_double sel_freq_min;
  t_double sel_freq_max;
  t_double sel_decay_min;
  t_double sel_decay_max;

  t_int32 times[16];
  
} t_bank;

// ========  STRUCTURE:  MODAL OBJECT  ========

typedef enum _sort_type {

  OUT_SORT_NONE,
  OUT_SORT_AMPL,
  OUT_SORT_FREQ,
  OUT_SORT_DECAY,

} t_sort_type;

typedef  enum _out_type {

  OUT_TYPE_OFF,
  OUT_TYPE_OUTP,
  OUT_TYPE_INP,
  OUT_TYPE_RMS,
  OUT_TYPE_CH_I,
  OUT_TYPE_CH_N,

} t_out_type;

typedef struct _modal
{
  t_pxobject obj;       // Use t_pxobject for MSP objects

  void*   outl_mess;    // Outlet 2: messages
  void*   outl_float;
  t_int32 output_ind;
  
  t_bank* bank_arr;     // Array of banks
  t_int32 bank_cnt;     // Maximum number of banks

  t_int32 reson_max;    // Maximum number of modes

  t_double master;      // Amplitude multiplier for whole output
  t_double samplerate;  // Stores the samplerate
  t_double msr;         // Millisecondrate
  t_int32  sort_type;

  t_mode mode_arr[MODE_LAST];

  t_state* state_arr;
  t_int32  state_cnt;

  t_state state_tmp[1];

  t_double ramp_param;
  t_ramp   ramp_func;
  t_ramp   ramp_func_inv;

  t_symbol*    dict_sym;  // Name of a dictionary to state all data
  t_bank*      bank_cur;
  t_resonator* reson_cur;

  t_out_type out_type;

  t_double a_smoothing;
  t_atom*  outp_mess_arr;  // To output messages
  
} t_modal;

// ========  METHOD PROTOTYPES  ========

// ====  MAX MSP METHODS  ====

void* modal_new(t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_free(t_modal *x);
void modal_dsp64(t_modal *x, t_object *dsp64, t_int32 *count, t_double samplerate, long maxvectorsize, long flags);
void modal_perform64(t_modal *x, t_object *dsp64, t_double **ins, long numins, t_double **outs, long numouts, long sampleframes, long flags, void *userparam);
void modal_assist(t_modal *x, void *b, long msg, t_int32 arg, char *str);

// ====  INTERFACE METHODS  ====

void modal_out_type (t_modal *x, t_symbol *type);
void modal_out_sort (t_modal *x, t_symbol *sort);

void io_dictionary(t_modal *x, t_symbol *dict_sym);

void io_import(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void io_load  (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void io_save  (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void io_split (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void io_join  (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void io_rename(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void io_delete(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void io_clear (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);

void modal_info (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_param(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_post (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_flush(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);

void modal_master(t_modal *x, t_double gain);

void modal_is_on     (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_gain      (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_ampl_mult (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_freq_shift(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_decay_mult(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);

void modal_get_ampl_rng (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_get_freq_rng (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_get_decay_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);

void modal_sel_all      (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_sel_ampl_ind (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_sel_ampl_rng (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_sel_freq_ind (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_sel_freq_rng (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_sel_decay_ind(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void modal_sel_decay_rng(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);

// ====  MODES  ====

void _mode_new    (t_modal *x, t_bank *bank);
void _mode_iterate(t_modal *x, t_bank *bank, t_resonator *reson);

void mode_all_on   (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void mode_all_off  (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void mode_cycle    (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void mode_diffusion(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void mode_resonator(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);

// ====  STATES  ====
// Use storage slots to ramp to and in between

t_my_err _state_init(t_state *state, t_int32 ampl_cnt, t_double u, t_double a);
void     _state_free(t_state *state);

t_state* _state_arr_new   (t_int32 cnt, t_int32 *state_cnt);
void     _state_arr_free (t_state **state_arr, t_int32 *state_cnt);

t_state* _state_find(t_state *state_arr, t_int32 state_cnt, t_atom *atom);
t_my_err _state_store(t_modal *x, t_bank *bank, t_state *state, t_symbol *name);
void     _state_ramp(t_modal *x, t_bank *bank, t_state *state, t_int32 cntd);

t_my_err _state_dict_save(t_state *state, t_dictionary *dict_arr_states, t_symbol *state_sym, t_symbol *is_prot);
t_my_err _state_dict_load(t_dictionary *dict_state, t_state *state);

void state_state       (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void state_ramp_to     (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void state_ramp_between(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void state_ramp_max    (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void state_velocity    (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);
void state_freeze      (t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv);

// ====  RESONATOR METHODS  ====

t_resonator* modal_find_reson(t_modal *x, t_bank *bank, t_atom *argv, t_symbol *sym);

void reson_new   (t_modal *x, t_bank *bank, t_resonator *reson);
void reson_free  (t_modal *x, t_resonator *reson);
void reson_copy  (t_modal *x, t_bank *bank, t_resonator *reson, t_resonator *reson_src);
void reson_update(t_modal *x, t_bank *bank, t_resonator *reson);

// ====  BANK METHODS  ====

t_bank* bank_find  (t_modal *x, t_atom *argv, t_symbol *sym);

int compare_ampl (void *bank, const t_int32 *index1, const t_int32 *index2);
int compare_freq (void *bank, const t_int32 *index1, const t_int32 *index2);
int compare_decay(void *bank, const t_int32 *index1, const t_int32 *index2);

t_int32  bank_new    (t_modal *x, t_bank *bank, t_int32 nb);
t_int32  bank_realloc(t_modal *x, t_bank *bank, t_int32 nb);
void bank_free       (t_modal *x, t_bank *bank);
void bank_sort       (t_modal *x, t_bank *bank);
void bank_update     (t_modal *x, t_bank *bank);

// ========  END OF HEADER FILE  ========

#endif

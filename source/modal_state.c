#include "modal~.h"

// ====  _STATE_INIT  ====

/**
Initialize one state
state->cnt is set to -1 if there was an error
Returns:
  ERR_NONE:  Succesful initialization
  ERR_COUNT:  Invalid count argument, should be one at least
  ERR_ALLOC:  Failed allocation 
*/
t_my_err _state_init(t_state *state, t_int32 coord_cnt, t_double u, t_double a) {

  // Initialize the symbols to the empty symbol
  state->name = gensym("");
  state->from  = gensym("");

  // Initialize the array pointers to NULL
  state->U_arr = NULL;
  state->A_arr = NULL;

  // The count should not be negative
  if (coord_cnt < 0) {
    state->cnt = -1;
    return ERR_COUNT; }

  // ... if it is equal to 0:
  else if (coord_cnt == 0) {
    state->cnt = 0;
    return ERR_NONE; }

  // ... if it is at least 1:
  else {

    // Allocate the arrays
    state->U_arr = (t_double*)sysmem_newptr(sizeof(t_double) * coord_cnt);
    state->A_arr = (t_double*)sysmem_newptr(sizeof(t_double) * coord_cnt);

    // Then test the allocation
    if (state->U_arr && state->A_arr) {

      // Set the count
      state->cnt = coord_cnt;

      // Initialize the array values
      for (t_int32 res = 0; res < state->cnt; res++) {
        state->U_arr[res] = u;
        state->A_arr[res] = a; }

      return ERR_NONE; }
  
    // Otherwise there was an allocation error
    else {
      state->cnt = -1;
      if (state->U_arr) { sysmem_freeptr(state->U_arr); state->U_arr = NULL; }
      if (state->A_arr) { sysmem_freeptr(state->A_arr); state->A_arr = NULL; }  
      return ERR_ALLOC; } }
}

// ====  _STATE_FREE  ====

/**
Free one state
*/
void _state_free(t_state *state) {

  state->cnt = 0;

  if (state->U_arr) {
    sysmem_freeptr(state->U_arr);
    state->U_arr = NULL; }
  
  if (state->A_arr) {
    sysmem_freeptr(state->A_arr);
    state->A_arr = NULL; }
}

// ====  _STATE_ARR_NEW  ====

/**
Create an array of states
  cnt:  number of states in the array, at least 1
  state_cnt:  pointer to the element count of the array
Returns a pointer to an array of states or NULL
*/
t_state *_state_arr_new(t_int32 cnt, t_int32 *state_cnt) {

  // Test that the count for the array is at least one
  if (cnt < 1) { return NULL; }

  // Allocate the array of states and test the allocation
  t_state* state_arr = (t_state*)sysmem_newptr(sizeof(t_state) * cnt);
  if (!state_arr) { return NULL; }

  // Initialize the count and the states in the array
  *state_cnt = cnt;
  for (t_int32 st = 0; st < *state_cnt; st++) {  _state_init(state_arr + st, 0, 0, 0);  }

  // Return the allocated pointer
  return state_arr;
}

// ====  _STATE_ARR_FREE  ====

/**
Free an array of states
*/
void _state_arr_free(t_state **state_arr, t_int32 *state_cnt) {

  // Test if the array of storage slots is not yet allocated
  if (!*state_arr) { return; }

  // Otherwise proceed to free each state
  for (t_int32 st = 0; st < *state_cnt; st++) { _state_free(*state_arr + st); }

  // Free the array of states and set the count to 0
  sysmem_freeptr(*state_arr);
  *state_arr = NULL;
  *state_cnt = 0;
}

// ====  _STATE_FIND  ====

/**
Find a state within the array of states
Returns a pointer to the state or NULL
*/
t_state *_state_find(t_state *state_arr, t_int32 state_cnt, t_atom *atom) {

  // Test if the array of storage slots is not yet allocated
  // If the atom is not an integer
  if ((!state_arr) || (atom_gettype(atom) != A_LONG))  { return NULL; }
  
  // Test if the index of the slot is within range
  t_int32 state_ind = (t_int32)atom_getlong(atom);
  if ((state_ind < 0) || (state_ind >= state_cnt)) { return NULL; }

  return (state_arr + state_ind);
}
  
// ====  _STATE_STORE  ====

/**
Set a storage slot to the current values from a bank
Returns:
ERR_NONE:  Storing successful
ERR_COUNT:  Invalid count argument, should be one at least
ERR_ALLOC:  Failed allocation
*/
t_my_err _state_store(t_modal *x, t_bank *bank, t_state *state, t_symbol *name) {

  // Free and reallocate the state if the number of resonators does not match
  if (state->cnt != bank->reson_cnt) {
    _state_free(state);
    t_my_err err = _state_init(state, bank->reson_cnt, 0, 0);
    if (err != ERR_NONE) { return err; } }
  
  // Copy the current values from the bank into the storage slot
  for (t_int32 res = 0; res < state->cnt; res++) {
    state->A_arr[res] = bank->reson_arr[res].in_A_cur;
    state->U_arr[res] = x->ramp_func_inv(state->A_arr[res], x->ramp_param);  }

  // Set the name and the bank from which the state was defined
  state->name = name;
  state->from = bank->name;

  return ERR_NONE;
}

// ====  _STATE_DICT_SAVE  ====

/**
To save a state into a dictionary. Passed as a function pointer argument to dict_save
Returns ERR_NONE or ERR_ALLOC
*/
t_my_err _state_dict_save(t_state *state, t_dictionary *dict_arr_states, t_symbol *state_sym, t_symbol *is_prot) {

  t_dictionary *dict_state = dictionary_sprintf("@name %s @count %i @from %s", state_sym->s_name, state->cnt, state->from->s_name);
  dictionary_appenddictionary(dict_arr_states, state_sym, (t_object *)dict_state);

  // Create an array of atoms for temporary storage
  t_atom *atom_arr = (t_atom *)sysmem_newptr(sizeof(t_atom)* state->cnt);
  if (!atom_arr) { return ERR_ALLOC; }

  // Create subdictionaries for the abscissa and ordinate arrays, and append them to the state dictionary
  atom_setdouble_array(state->cnt, atom_arr, state->cnt, state->U_arr);
  dictionary_appendatoms(dict_state, gensym("abscissa"), state->cnt, atom_arr);

  atom_setdouble_array(state->cnt, atom_arr, state->cnt, state->A_arr);
  dictionary_appendatoms(dict_state, gensym("ordinate"), state->cnt, atom_arr);

  // Set the name of the state struct
  state->name = state_sym;

  sysmem_freeptr(atom_arr);
  return ERR_NONE;
}

// ====  _STATE_DICT_LOAD  ====

/**
Load a state from a dictionary. Passed as a function pointer argument to dict_load
Returns:
  ERR_NONE:  Succesful initialization
  ERR_COUNT:  Invalid count argument, should be one at least
  ERR_ALLOC:  Failed allocation (from _state_init)
*/
t_my_err _state_dict_load(t_dictionary *dict_state, t_state *state) {

  // Get the number of state values from the dictionary
  t_atom_long a_count = 0;
  dictionary_getlong(dict_state, gensym("count"), &a_count);
  if (a_count < 1) { return ERR_COUNT; }

  // Free the array of amplitude values if the number of resonators does not match
  if (state->cnt != a_count) { _state_free(state); }

  // Allocate the array of amplitude values if necessary 
  if ((!state->U_arr) || (!state->A_arr)) {
    t_my_err err = _state_init(state, (t_int32)a_count, 0, 0);
    if (err != ERR_NONE) { return err; } }

  // Get "name" and "from" from the dictionary
  dictionary_getsym(dict_state, gensym("name"), &(state->name));
  dictionary_getsym(dict_state, gensym("from"), &(state->from));

  // Get the "abscissa" and "ordinate" arrays from the dictionary
  t_atom *atom_arr = NULL;
  long a_long;
  dictionary_getatoms(dict_state, gensym("abscissa"), &a_long, &atom_arr);
  if (a_long != state->cnt) { return ERR_COUNT; }
  for (t_int32 res = 0; res < state->cnt; res++) { state->U_arr[res] = atom_getfloat(atom_arr + res); }

  dictionary_getatoms(dict_state, gensym("ordinate"), &a_long, &atom_arr);
  if (a_long != state->cnt) { return ERR_COUNT; }
  for (t_int32 res = 0; res < state->cnt; res++) { state->A_arr[res] = atom_getfloat(atom_arr + res); }
  
  // If all is successfull free the array of atoms and return
  return ERR_NONE;
}

// ====  STATE_STATE  ====

/**
Interface method to call:  new / free / resize / get / post / store / save / load / rename / delete
*/
void state_state(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("state_state");

  // Argument 0 should be a command: new, free, resize, post or store
  MY_ASSERT((argc < 1) || (atom_gettype(argv) != A_SYM),
    "state:  Arg 0:  Command expected: new / free / resize / get / post / store / save / load / rename / delete.");
  t_symbol *cmd = atom_getsym(argv);

  // ====  NEW:  Allocate a new array of states  ====
  // state new (int: state count)

  if (cmd == gensym("new")) {
    
    MY_ASSERT(x->state_arr, "state new:  An array of states already exists.");
    MY_ASSERT(argc != 2, "state new:  2 args expected:  state new (int: array size)");
    MY_ASSERT(atom_gettype(argv + 1) != A_LONG, "state new:  Arg 1:  Int expected for the number of states.");

    t_int32 state_cnt = (t_int32)atom_getlong(argv + 1);
    MY_ASSERT(state_cnt < 1, "state new:  Arg 1:  Value of at least 1 expected for the number of states.");

    x->state_arr = _state_arr_new(state_cnt, &(x->state_cnt));
    MY_ASSERT(!x->state_arr, "state new:  Failed to allocate an array of states.");

    POST("state new:  Array of %i states created.", x->state_cnt); }

  // ====  FREE:  Free the array of states  ====
  // state free

  else if (cmd == gensym("free")) {

    MY_ASSERT(!x->state_arr, "state free:  No array of states available to free.");
    MY_ASSERT(argc != 1, "state free:  1 args expected:  state free");

    _state_arr_free(&(x->state_arr), &(x->state_cnt));
    
    POST("state free:  Array of states freed."); }

  // ====  RESIZE:  Resize the array of storage slots  ====

  else if ((cmd == gensym("resize")) && (argc == 3) && (atom_gettype(argv + 2) == A_LONG)) {
    ; }

  // ====  GET:  get information on a state as a message  ====
  // state get (int: state index)

  else if (cmd == gensym("get")) {

    MY_ASSERT(!x->state_arr, "state get:  No array of states available to get information from.");
    MY_ASSERT(argc != 2, "state get:  2 args expected:  state get (int: state index)");

    // Argument 1 should reference a non empty state
    t_state *state = _state_find(x->state_arr, x->state_cnt, argv + 1);
    MY_ASSERT(!state, "state get:  Arg 1:  State not found.");

    // Output a message with information about the state
    t_atom mess_arr[4];
    atom_setlong(mess_arr, state - x->state_arr);
    atom_setsym(mess_arr + 1, state->name);
    atom_setlong(mess_arr + 2, state->cnt);
    atom_setsym(mess_arr + 3, state->from);
    outlet_anything(x->outl_mess, gensym("state"), 4, mess_arr); }

  // ====  POST:  Post information on the array of states  ====
  // state post (int: state index / sym: all)

  else if (cmd == gensym("post")) {

    MY_ASSERT(!x->state_arr, "state post:  No array of states available to post information on.");
    MY_ASSERT(argc != 2, "state post:  2 args expected:  state post (int: state index / sym: all)");

    // If Arg 1 is a symbol and equal to "all"
    if ((atom_gettype(argv + 1) == A_SYM) && (atom_getsym(argv + 1) == gensym("all"))) {

      // Post information on all the states
      POST("The array of states has %i elements.", x->state_cnt);
      t_state *state = NULL;
      for (t_int32 st = 0; st < x->state_cnt; st++) {
        state = x->state_arr + st;
        POST("  State %i:  Count: %i - Name: %s - From: %s", st, state->cnt, state->name->s_name, state->from->s_name); } }
  
    // ... If Arg 1 is an int
    else if (atom_gettype(argv + 1) == A_LONG) {

      // Find the state
      t_state *state = _state_find(x->state_arr, x->state_cnt, argv + 1);
      MY_ASSERT(!state, "state post:  Arg 1:  State not found.");
      
      // Post detailed information on one state
      POST("The array of states has %i elements.", x->state_cnt);
      POST("State %i:  Count: %i - Name: %s - From: %s", state - x->state_arr, state->cnt, state->name->s_name, state->from->s_name);

      for (t_int32 res = 0; res < state->cnt; res++) {
        POST("  Value %i:  U = %f A = %f", res, state->U_arr[res], state->A_arr[res]); } }

    // ... Otherwise the arguments are invalid
    else { MY_ASSERT(argc != 2, "state post:  Invalid args:  state post (int: state index / sym: all)"); } }
  
  // ====  STORE:  Store the values from a bank into a state  ====
  // state store (int: bank index) (int: state index) (sym: state name)

  else if (cmd == gensym("store")) {

    MY_ASSERT(!x->state_arr, "state store:  No array of states available to store into.");
    MY_ASSERT(argc != 4, "state store:  4 args expected:  state store (int: bank index) (int: state index) (sym: state name)");

    // Argument 1 should reference a bank
    t_bank *bank = bank_find(x, argv + 1, sym);
    MY_ASSERT(!bank, "state store:  Arg 1:  Bank not found");

    // Argument 2 should reference a state
    t_state *state = _state_find(x->state_arr, x->state_cnt, argv + 2);
    MY_ASSERT(!state, "state store:  Arg 2:  State not found");

    // Argument 3 should hold the name of the state as a symbol
    t_symbol *name = atom_getsym(argv + 3);
    MY_ASSERT(name == gensym(""), "state store:  Arg 3:  Symbol expected for the name of the state.")

    t_my_err err = _state_store(x, bank, state, name);
    MY_ASSERT(err != ERR_NONE, "Failed to allocate an array of states.");

    POST("state store:  Bank %i state stored in state %i: %s", bank - x->bank_arr, state - x->state_arr, state->name->s_name); }

  // ====  SAVE:  Save a state under a new name  ====
  // state save (int: state index) (sym: state name)

  else if (cmd == gensym("save")) {

    MY_ASSERT(!x->state_arr, "state save:  No array of states available to save from.");
    MY_ASSERT(argc != 3, "state save:  3 args expected:  state save (int: state index) (sym: state name)");

    // Argument 1 should reference a non empty state
    t_state *state = _state_find(x->state_arr, x->state_cnt, argv + 1);
    MY_ASSERT(!state, "state save:  Arg 1:  State not found.");
    MY_ASSERT(!state->cnt, "state save:  Arg 1:  The state is empty.");

    if (dict_save(x, x->dict_sym, gensym("states"), gensym("state save"), 1, state, argv + 2, _state_dict_save) == ERR_NONE) {
      POST("state save:  State %i saved as \"%s\" - Count: %i.", state - x->state_arr, atom_getsym(argv + 2)->s_name, state->cnt); } }

  // ====  LOAD:  Load a state  ====
  // state load (sym: state name) (int: state index)

  else if (cmd == gensym("load")) {

    MY_ASSERT(!x->state_arr, "state load:  No array of states available to load into.");
    MY_ASSERT(argc != 3, "state load:  3 args expected:  state load (sym: state name) (int: state index)");

    // Argument 2 should reference a state
    t_state *state = _state_find(x->state_arr, x->state_cnt, argv + 2);
    MY_ASSERT(!state, "state load:  Arg 2:  State not found.");

    if (dict_load(x, x->dict_sym, gensym("states"), gensym("state load"), 1, state, argv + 1, _state_dict_load) == ERR_NONE) {
      POST("state load:  State \"%s\" loaded into %i - Count: %i.", atom_getsym(argv + 1)->s_name, state - x->state_arr, state->cnt); } }

  // ====  DELETE:  Delete a state  ====
  // state delete (sym: state name)

  else if (cmd == gensym("delete")) {

    MY_ASSERT(argc != 2, "state delete:  2 args expected:  state delete (sym: state name)");

    if (dict_delete(x, x->dict_sym, gensym("states"), gensym("state delete"), 1, argv + 1) == ERR_NONE) {
      POST("state delete:  State \"%s\" deleted from the dictionary.", atom_getsym(argv + 1)->s_name); } }

  // ====  RENAME:  Rename a state  ====
  // state rename (sym: state1 name) (sym: state2 name)

  else if (cmd == gensym("rename")) {

    MY_ASSERT(argc != 3,
      "state rename:  3 args expected:  state rename (sym: state1 name) (sym: state2 name)");

    if (dict_rename(x, x->dict_sym, gensym("states"), gensym("state rename"), 1, argv + 1, argv + 2) == ERR_NONE) {
      POST("state rename:  State \"%s\" renamed to \"%s\".", atom_getsym(argv + 1)->s_name, atom_getsym(argv + 2)->s_name); } }

  // ====  Otherwise the argument is invalid  ====

  else {
    MY_ERR("state:  Arg 0:  Command expected: new / free / resize / get / post / store / save / load / rename / delete."); return; }
}

// ====  _STATE_RAMP  ====

/**
Ramp a bank to a state
Used by the interface ramping methods
*/
void _state_ramp(t_modal *x, t_bank *bank, t_state *state, t_int32 cntd) {

  t_resonator *reson = NULL;

  // Loop through all the resonators of the bank
  for (t_int32 res = 0; res < bank->reson_cnt; res++) {
    reson = bank->reson_arr + res;

    // Set mode, countdown and initialize target values
    reson->mode_ind   = MODE_FIX_CHG;
    reson->mode_type = MODE_TYPE_VAR_A;
    reson->cntd       = cntd;
    reson->in_A_targ = 0;
    reson->in_U_targ = 0; }

  // Take into account a difference in the number of resonators and state values
  t_int32 cnt = MIN(state->cnt, bank->reson_cnt);

  // Set all the target values to the state values
  for (t_int32 res = 0; res < cnt; res++) {
    reson = bank->reson_arr + res;
    reson->in_U_targ = state->U_arr[res];
    reson->in_A_targ = state->A_arr[res];  }
}

// ====  STATE_RAMP_TO  ====

/**
Ramp a bank to a state
  ramp_to (int: bank index) (int: state index) (float: time in ms)
*/
void state_ramp_to(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("state_ramp_to");

  // The method expects three arguments
  MY_ASSERT(argc != 3, "ramp_to:  3 args expected:  ramp_to (int: bank index) (int: state index) (float: time in ms)");
  
  // Argument 0 should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  MY_ASSERT(!bank, "ramp_to:  Arg 0:  Bank not found.");
  
  // The second argument should reference a state
  t_state *state = _state_find(x->state_arr, x->state_cnt, argv + 1);
  MY_ASSERT(!state, "ramp_to:  Arg 1:  State not found.");

  // Argument 2 should be a positive float: the time in ms
  MY_ASSERT((atom_gettype(argv + 2) != A_FLOAT) && (atom_gettype(argv + 2) != A_LONG),
    "ramp_to:  Arg 2:  Positive float expected: time in ms.");
  
  t_double time = atom_getfloat(argv + 2);
  MY_ASSERT(time <= 0, "ramp_to:  Arg 2:  Positive float expected: time in ms.");

  _state_ramp(x, bank, state, (t_int32)(time * x->msr));
}

// ====  STATE_RAMP_BETWEEN  ====

/**
Ramp a bank to an interpolated setting between two states
  ramp_betweeen (int: bank) (int: state 1) (int: state 2) (float: interpolation) (float: time in ms)
*/
void state_ramp_between(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("state_ramp_between");

  // The method expects five arguments
  MY_ASSERT(argc != 5, "ramp_between:  5 args expected:  ramp_betweeen (int: bank) (int: state 1) (int: state 2) (float: interpolation) (float: time in ms)");
  
  // Argument 0 should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  MY_ASSERT(!bank, "ramp_between:  Arg 0:  Bank not found.");
  
  // Argument 1 should reference a state
  t_state *state1 = _state_find(x->state_arr, x->state_cnt, argv + 1);
  MY_ASSERT(!state1, "ramp_between:  Arg 1:  State not found.");

  // Argument 2 should reference a state
  t_state *state2 = _state_find(x->state_arr, x->state_cnt, argv + 2);
  MY_ASSERT(!state2, "ramp_between:  Arg 2:  State not found.");

  // Argument 3 should be a float between 0 and 1: interpolation between the two states
  MY_ASSERT((atom_gettype(argv + 3) != A_FLOAT) && (atom_gettype(argv + 3) != A_LONG),
    "ramp_between:  Arg 3:  Float [0-1] expected: interpolation between the two states.");

  t_double interp = atom_getfloat(argv + 3);
  MY_ASSERT((interp < 0) || (interp > 1),
    "ramp_between:  Arg 3:  Float [0-1] expected: interpolation between the two states.");

  // Argument 4 should be the time in ms
  MY_ASSERT((atom_gettype(argv + 4) != A_FLOAT) && (atom_gettype(argv + 4) != A_LONG),
    "ramp_between:  Arg 4:  Positive float expected: time in ms.");
  
  t_double time = atom_getfloat(argv + 4);
  MY_ASSERT(time <= 0, "ramp_between:  Arg 2:  Positive float expected: time in ms.");

  // Get the minimum between the number of resonators in the bank and the state counts
  t_int32 cnt = MIN(state1->cnt, state2->cnt);
  cnt = MIN(bank->reson_cnt, cnt);
  x->state_tmp->cnt = cnt;

  // Calculate the interpolated values from the abscissa
  for (t_int32 res = 0; res < cnt; res++) {
    x->state_tmp->U_arr[res] = state1->U_arr[res] + interp * (state2->U_arr[res] - state1->U_arr[res]);
    x->state_tmp->A_arr[res] = x->ramp_func(x->state_tmp->U_arr[res], x->ramp_param);  }
  
  _state_ramp(x, bank, x->state_tmp, (t_int32)(time * x->msr));
}

// ====  STATE_RAMP_MAX  ====

/**
Ramp a bank to the maximum of a list of interpolated states 
  ramp_max (int: bank) [(int: state) (float: interpolation)] {x N} (float: time in ms)
*/
void state_ramp_max(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("state_ramp_max");

  // The method expects (2 + 2*n) arguments
  MY_ASSERT((((argc % 2) != 0) || (argc < 4)),
    "ramp_max:  Expects:  ramp_max (int: bank) [(int: state) (float: interpolation)] {x N} (float: time in ms)");
  
  // Argument 0 should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  MY_ASSERT(!bank, "ramp_max:  Arg 0:  Bank not found.");

  // The last argument should be the time in ms
  MY_ASSERT((atom_gettype(argv + argc - 1) != A_FLOAT) && (atom_gettype(argv + argc - 1) != A_LONG),
    "ramp_max:  Arg %i:  Positive float expected: time in ms.", argc - 1);
  
  t_double time = atom_getfloat(argv + argc - 1);
  MY_ASSERT(time <= 0, "ramp_max:  Arg %i:  Positive float expected: time in ms.", argc - 1);

  // The arguments from the second one should be [int, float] pairs
  t_int32 state_cnt = argc / 2 - 1;
  t_atom *atom = argv + 1;
  t_state *state = NULL;
  t_double interp;

  t_int32 cnt = bank->reson_cnt;
  for (t_int32 res = 0; res < cnt; res++) { x->state_tmp->U_arr[res] = 0; }

  while (state_cnt--) {

    // The first argument of the pair should reference a state
    state = _state_find(x->state_arr, x->state_cnt, atom++);
    MY_ASSERT(!state, "ramp_max:  Arg:  State not found.");

    // The second argument of the pair should be a float between 0 and 1: interpolation from 0
    MY_ASSERT((atom_gettype(atom) != A_FLOAT) && (atom_gettype(atom) != A_LONG),
      "ramp_max:  Arg:  Float [0-1] expected: interpolation between 0 and state");    

    interp = atom_getfloat(atom++);
    MY_ASSERT((interp < 0) || (interp > 1),
      "ramp_max:  Arg:  Float [0-1] expected: interpolation between 0 and state");

    // Get the minimum between the number of resonators in the bank and the state counts
    cnt = MIN(state->cnt, cnt);

    // Calculate the interpolated values and take the maximum
    for (t_int32 res = 0; res < cnt; res++) {
      x->state_tmp->U_arr[res] = MAX(interp * state->U_arr[res], x->state_tmp->U_arr[res]); }  }

  x->state_tmp->cnt = cnt;
  for (t_int32 res = 0; res < cnt; res++) {
    x->state_tmp->A_arr[res] = x->ramp_func(x->state_tmp->U_arr[res], x->ramp_param); }
  
  _state_ramp(x, bank, x->state_tmp, (t_int32)(time * x->msr));
}

// ====  STATE_VELOCITY  ====

/**
Set the ramping velocity for a bank
  velocity (int: bank index) (float: velocity)
*/
void state_velocity(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("state_velocity");

  // The method expects two arguments
  MY_ASSERT(argc != 2, "velocity:  2 args expected:  velocity (int: bank index) (float: velocity)");

  // Argument 0 should reference a bank
  t_bank *bank = bank_find(x, argv, sym);
  MY_ASSERT(!bank, "velocity:  Arg 0:  Bank not found.");

  // Argument 1 should be the velocity
  MY_ASSERT((atom_gettype(argv + 1) != A_FLOAT) && (atom_gettype(argv + 1) != A_LONG),
    "velocity:  Arg 1:  Positive float expected.");

  // The velocity argument should be positive
  t_double velocity = atom_getfloat(argv + 1);
  MY_ASSERT(velocity < 0, "velocity:  Arg 1:  Positive float expected.");

  bank->velocity = velocity;
}

// ====  METHOD: STATE_FREEZE  ====

/**
Freeze or unfreeze a bank of resonator
Works for all modes, whether the bank is cycling or ramping
  freeze (int: bank index) (int: 0 or 1)
*/
void state_freeze(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("state_freeze");

  // The method expects two arguments
  MY_ASSERT(argc != 2, "freeze:  2 args expected:  freeze (int: bank index) (int: 0 or 1)");

  // Argument 0 should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  MY_ASSERT(!bank, "freeze:  Arg 0:  Bank not found.");

  // Argument 1 should be 0 or 1
  MY_ASSERT(atom_gettype(argv + 1) != A_LONG, "freeze:  Arg 1:  0 or 1 expected to freeze or unfreeze the state.");
  t_int32 is_frozen = (t_int32)(atom_getlong(argv + 1));
  MY_ASSERT((is_frozen != 0) && (is_frozen != 1), "freeze:  Arg 1:  0 or 1 expected to freeze or unfreeze the state.");

  bank->is_frozen = (is_frozen == 1) ? true : false;
}

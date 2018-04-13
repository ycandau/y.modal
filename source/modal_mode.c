#include "modal~.h"

// ====  OPENING ACTIONS FOR MODES  ====

void st_act_open_shift(t_modal *x, t_bank *bank, t_resonator *reson, t_mode *mode) {

//  reson->freq_tmp = reson->freq;
}

void st_act_diff(t_modal *x, t_bank *bank, t_resonator *reson, t_mode *mode) {

  // If the diffusion has changed
  if ((reson->diff_chg) || (reson->diff_type == MODE_DIFF_ONE_RR) ||
    (reson->diff_type == MODE_DIFF_NUM_RR) || (reson->diff_type == MODE_DIFF_MATR)) {

    switch (reson->diff_type) {
    case MODE_DIFF_ALL: 
      for (t_int32 ch = 0; ch < 8; ch++) { reson->diff_mult[ch] = 1.0; };
      reson->diff_ind = 7;
      reson->diff_cnt = 8;
      break;

    // One channel set by cmd
    case MODE_DIFF_ONE_S:
      for (t_int32 ch = 0; ch < 8; ch++) { reson->diff_mult[ch] = 0.0; };
      reson->diff_ind = reson->diff_sto;
      reson->diff_mult[reson->diff_ind] = 1.0;
      reson->diff_cnt = 1;
      break;

    // One channel chosen at random once or repeatedly
    case MODE_DIFF_ONE_R:
    case MODE_DIFF_ONE_RR:
      for (t_int32 ch = 0; ch < 8; ch++) { reson->diff_mult[ch] = 0.0; };
      reson->diff_ind = rand() % 8;
      reson->diff_mult[reson->diff_ind] = 1.0;
      reson->diff_cnt = 1;
      break;
    
    // N channels chosen at random once or repeatedly
    case MODE_DIFF_NUM_R:
    case MODE_DIFF_NUM_RR: 
    { reson->diff_cnt = reson->diff_sto;
      t_int32 index_arr[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
      random_n_of_m(reson->diff_cnt, 8, index_arr);
      for (t_int32 ch = 0; ch < reson->diff_cnt; ch++) { reson->diff_mult[index_arr[ch]] = 1.0; };
      for (t_int32 ch = reson->diff_cnt; ch < 8; ch++) { reson->diff_mult[index_arr[ch]] = 0.0; };
      t_int32 ch = 8;
      while ((ch--) && (reson->diff_mult[ch] != 1.0)) { ; }
      reson->diff_ind = ch; }
      break;
    
    case MODE_DIFF_MATR: break;
    case MODE_DIFF_FUNC: break;  }

    reson->diff_chg = false; }
}

// ====  METHOD:  MODE_NEW  ====

void _mode_new(t_modal *x, t_bank *bank) {

  t_mode *modes = x->mode_arr;

  // Default for all modes, overridden if necessary below
  for (t_int32 st = 0; st < MODE_LAST; st++) {
    modes[st].index      = st;
    modes[st].ampl_in    = 0.0;
    modes[st].ampl_out  = 1.0;
    modes[st].func_open  = NULL;
    modes[st].func_close = NULL; }

  modes[MODE_FIX_CHG].name      = gensym("fix_chg");
  modes[MODE_FIX_CHG].type      = MODE_TYPE_VAR_A;
  modes[MODE_FIX_CHG].next_ind  = MODE_FIX_ON;
  modes[MODE_FIX_CHG].ampl_in    = 1.0;
  modes[MODE_FIX_CHG].ampl_out  = 1.0;
  modes[MODE_FIX_CHG].time_ind  = 4;

  modes[MODE_FIX_ON].name        = gensym("fix_on");
  modes[MODE_FIX_ON].type        = MODE_TYPE_FIX;
  modes[MODE_FIX_ON].next_ind    = MODE_FIX_DOWN;

  modes[MODE_FIX_DOWN].name      = gensym("fix_down");
  modes[MODE_FIX_DOWN].type      = MODE_TYPE_VAR_A;
  modes[MODE_FIX_DOWN].next_ind  = MODE_FIX_OFF;
  modes[MODE_FIX_DOWN].ampl_in  = 0.0;
  modes[MODE_FIX_DOWN].ampl_out  = 1.0;
  modes[MODE_FIX_DOWN].time_ind  = 4;    // Using the same slot as for MODE_FIX_CHG

  modes[MODE_FIX_OFF].name      = gensym("fix_off");
  modes[MODE_FIX_OFF].type      = MODE_TYPE_OFF;
  modes[MODE_FIX_OFF].next_ind  = MODE_FIX_CHG;
  modes[MODE_FIX_OFF].func_open  = &st_act_diff;

// Setup each mode
  modes[MODE_CYC_UP].name        = gensym("cyc_up");
  modes[MODE_CYC_UP].type        = MODE_TYPE_VAR_A;
  modes[MODE_CYC_UP].next_ind    = MODE_CYC_ON;
  modes[MODE_CYC_UP].ampl_in    = 1.0;
  modes[MODE_CYC_UP].ampl_out    = 1.0;
  modes[MODE_CYC_UP].time_ind    = 0;

  modes[MODE_CYC_ON].name        = gensym("cyc_on");
  modes[MODE_CYC_ON].type        = MODE_TYPE_FIX;
  modes[MODE_CYC_ON].next_ind    = MODE_CYC_DOWN;
  modes[MODE_CYC_ON].time_ind    = 1;

  modes[MODE_CYC_DOWN].name      = gensym("cyc_down");
  modes[MODE_CYC_DOWN].type      = MODE_TYPE_VAR_A;
  modes[MODE_CYC_DOWN].next_ind  = MODE_CYC_OFF;
  modes[MODE_CYC_DOWN].ampl_in  = 0.0;
  modes[MODE_CYC_DOWN].ampl_out  = 1.0;
  modes[MODE_CYC_DOWN].time_ind  = 2;

  modes[MODE_CYC_OFF].name      = gensym("cyc_off");
  modes[MODE_CYC_OFF].type      = MODE_TYPE_OFF;
  modes[MODE_CYC_OFF].next_ind  = MODE_CYC_UP;
  modes[MODE_CYC_OFF].func_open  = &st_act_diff;
  modes[MODE_CYC_OFF].time_ind  = 3;

  modes[MODE_CYC_WAIT].name      = gensym("wait");
  modes[MODE_CYC_WAIT].type      = MODE_TYPE_FIX;
  modes[MODE_CYC_WAIT].next_ind  = MODE_CYC_DOWN;

  /* XXX modes[MODE_SHIFT1].name        = gensym("shift1");
  modes[MODE_SHIFT1].type        = MODE_TYPE_VAR_A;
  modes[MODE_SHIFT1].next_ind    = MODE_SHIFT2;
  modes[MODE_SHIFT1].func_open   = &st_act_open_shift;
  modes[MODE_SHIFT1].ampl_in    = 1.0;
  modes[MODE_SHIFT1].ampl_out    = 1.0;
  
  modes[MODE_SHIFT2].name        = gensym("shift2");
  modes[MODE_SHIFT2].type        = MODE_TYPE_VAR_S;
  modes[MODE_SHIFT2].next_ind    = MODE_SHIFT3;
  modes[MODE_SHIFT2].time_ind    = 4;

  modes[MODE_SHIFT3].name        = gensym("shift3");
  modes[MODE_SHIFT3].type        = MODE_TYPE_VAR_A;
  modes[MODE_SHIFT3].next_ind    = MODE_CYC_OFF;
  modes[MODE_SHIFT3].ampl_in    = 1.0;
  modes[MODE_SHIFT3].ampl_out    = 0.0;*/

  bank->times[0] = (t_int32)(4500 * x->msr);
  bank->times[1] = (t_int32)(5000 * x->msr);
  bank->times[2] = (t_int32)(4500 * x->msr);
  bank->times[3] = (t_int32)(5000 * x->msr);
  bank->times[4] = (t_int32)(4500 * x->msr);
  bank->times[5] = (t_int32)(5000 * x->msr);
  bank->times[6] = (t_int32)(4500 * x->msr);
  bank->times[7] = (t_int32)(5000 * x->msr);

  bank->times[8] = (t_int32)(500 * x->msr);
  bank->times[9] = (t_int32)(1000 * x->msr);
  bank->times[10] = (t_int32)(500 * x->msr);
  bank->times[11] = (t_int32)(1000 * x->msr);
  bank->times[12] = (t_int32)(500 * x->msr);
  bank->times[13] = (t_int32)(1000 * x->msr);
  bank->times[14] = (t_int32)(500 * x->msr);
  bank->times[15] = (t_int32)(1000 * x->msr);
}

// ====  METHOD: MODAL_MODE_ITERATE  ====

void _mode_iterate(t_modal *x, t_bank *bank, t_resonator *reson) {

  //TRACE("mode_iterate");

  // Get the current mode
  t_mode *mode = x->mode_arr + reson->mode_ind;

  // Closing actions for the mode
  if (mode->func_close) { mode->func_close(x, bank, reson, mode); }

  // Get the next mode
  if (mode->index == MODE_CYC_WAIT) {

    switch (rand() % 4) {
    case 0: mode = x->mode_arr + MODE_CYC_UP; break;
    case 1: mode = x->mode_arr + MODE_CYC_ON; break;
    case 2: mode = x->mode_arr + MODE_CYC_DOWN; break;
    case 3: mode = x->mode_arr + MODE_CYC_OFF; break;  } }

  else {
    mode = x->mode_arr + mode->next_ind; }

  // Opening actions for the next mode
  //if (mode->func_open) { mode->func_open(x, bank, reson, mode); }

  // Get the index and type for the resonator
  reson->mode_ind  = mode->index;
  reson->mode_type = mode->type;

  // XXX To wipe resonant tail when turning off
  //if (mode->type == MODE_TYPE_OFF) { reson->y_m1 = 0; reson->y_m2 = 0; }

  // If the amplitude will change, set the target amplitude
  if ((mode->type == MODE_TYPE_VAR_A) || (mode->type == MODE_TYPE_VAR_AP)) {
    reson->in_U_targ = mode->ampl_in;
    reson->in_A_targ = mode->ampl_in;
    //reson->out_A_targ = mode->ampl_out;
  }

  // Set the countdown
  switch (mode->index) {

  // Cycling modes
  case MODE_CYC_UP: case MODE_CYC_ON: case MODE_CYC_DOWN: case MODE_CYC_OFF:
    switch (reson->cntd_type) {
    case MODE_CNTD_RESON:  reson->cntd = reson->times[mode->time_ind]; break;
    case MODE_CNTD_BANK:  reson->cntd = random_int(bank->times[2 * mode->time_ind], bank->times[2 * mode->time_ind + 1]); break;
    }
    break;

  // Fixed modes
  case MODE_FIX_CHG: case MODE_FIX_DOWN:
    reson->cntd = reson->times[4];
    break;

  case MODE_FIX_ON: case MODE_FIX_OFF:
    reson->cntd = INDEFINITE;
    break;

  // Shifting modes
  // XXX case MODE_SHIFT2: reson->cntd = reson->times[mode->time_ind]; break;
  //case MODE_SHIFT3: reson->cntd = (t_int32)(500 * x->msr); break;
  }

  if (reson == bank->reson_arr) {
    POST("New mode: %s, cntd = %i, Ucur = %f, Acur = %f, Utarg = %f, Atarg = %f", mode->name->s_name, reson->cntd, reson->in_U_cur, reson->in_A_cur, reson->in_U_targ, reson->in_A_targ);
  }
}

// ====  METHOD:  MODE_ALL_ON  ====

void mode_all_on(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("mode_all_on");
  
  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) {  return; }

  t_resonator *reson = NULL;

  // No time arguments: turn on resonators instantly
  if (argc == 1) {
    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_OFF;
      reson->cntd      = 0;
      reson->times[4] = (t_int32)(10 * x->msr); } }
  
  // One time argument: same ramping time for everybody
  else if (argc == 2) {
    t_int32 ramp = (t_int32)(atom_getfloat(argv + 1) * x->msr);

    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_OFF;
      reson->cntd      = 0;
      reson->times[4] = ramp; } }

  // Two time arguments: same ramping time, random delays
  else if (argc == 3) {
    t_int32 ramp = (t_int32)(atom_getfloat(argv + 1) * x->msr);
    t_double wait_max = atom_getfloat(argv + 2) - atom_getfloat(argv + 1);

    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_OFF;
      reson->cntd      = random_time_to_smp(0, wait_max, x->msr);
      reson->times[4] = ramp; } }

  // Three time arguments: random ramping times, random delays
  else if (argc == 4) {
    t_double ramp_f = 0;
    t_double ramp_min = atom_getfloat(argv + 1);
    t_double ramp_max = atom_getfloat(argv + 2);
    t_double wait_max = atom_getfloat(argv + 3);

    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_OFF;
      ramp_f          = random_float(ramp_min, ramp_max);
      reson->cntd      = random_time_to_smp(0, wait_max - ramp_f, x->msr);
      reson->times[4] = (t_int32)(ramp_f * x->msr); } }
}

// ====  METHOD: MODAL_ALL_OFF  ====

void mode_all_off(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("mode_all_off");
  
  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  t_resonator *reson = NULL;

  // No time arguments: turn on resonators instantly
  if (argc == 1) {
    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_ON;
      reson->cntd       = 0;
      reson->times[4] = (t_int32)(10 * x->msr); } }
  
  // One time argument: same ramping time for everybody
  else if (argc == 2) {
    t_int32 ramp = (t_int32)(atom_getfloat(argv + 1) * x->msr);

    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_ON;
      reson->cntd       = 0;
      reson->times[4] = ramp; } }

  // Two time arguments: same ramping time, random delays
  else if (argc == 3) {
    t_int32 ramp = (t_int32)(atom_getfloat(argv + 1) * x->msr);
    t_double wait_max = atom_getfloat(argv + 2) - atom_getfloat(argv + 1);
    
    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_ON;
      reson->cntd       = random_time_to_smp(0, wait_max, x->msr);
      reson->times[4] = ramp; } }

  // Three time arguments: random ramping times, random delays
  else if (argc == 4) {
    t_double ramp_f = 0;
    t_double ramp_min = atom_getfloat(argv + 1);
    t_double ramp_max = atom_getfloat(argv + 2);
    t_double wait_max = atom_getfloat(argv + 3);

    for (int res = 0; res < bank->reson_cnt; res++) {
      reson = bank->reson_arr + res;

      reson->mode_ind = MODE_FIX_ON;
      ramp_f           = random_float(ramp_min, ramp_max);
      reson->cntd       = random_time_to_smp(0, wait_max - ramp_f, x->msr);
      reson->times[4] = (t_int32)(ramp_f * x->msr); } }
}

// ====  METHOD: MODE_CYCLE  ====

void mode_cycle(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("mode_cycle");

  // There should be at least three arguments
  if (argc < 3) { MY_ERR("%s:  Invalid arguments:  At least 3 expected.", sym->s_name); return; }

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { MY_ERR("%s:  Invalid arguments:  Arg 0: bank not found.", sym->s_name); return; }

  // ==== Applying command to ALL resonators

  // The second argument can be the symbol "all"
  // The command applies then to all the resonators of the bank
  if ((atom_gettype(argv + 1) == A_SYM) && (atom_getsym(argv + 1) == gensym("all"))) {

    // The third argument should be a symbol indicating the command for the bank
    t_symbol *cmd = atom_getsym(argv + 2);

    // "resume": set all the resonators back to cycling
    if (cmd == gensym("resume")) {
      
      // Loop through the resonators
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {

        t_resonator *reson = bank->reson_arr + res;

        // Only cycle resonators that are not cycling yet
        if ((reson->mode_ind != MODE_CYC_UP) && (reson->mode_ind != MODE_CYC_ON) &&
          (reson->mode_ind != MODE_CYC_DOWN) && (reson->mode_ind != MODE_CYC_OFF)) {

          // Calculate the total cycle time for waiting
          t_int32 time = 0;
          if (reson->cntd_type == MODE_CNTD_BANK) {
            for (t_int32 t = 0; t < 4; t++) {  time += bank->times[2 * t + 1]; } }
          else if (reson->cntd_type == MODE_CNTD_RESON) {
            for (t_int32 t = 0; t < 4; t++) {  time += reson->times[t]; } }

          // Set mode to wait up to the total cycle length
          reson->mode_ind = MODE_CYC_WAIT;
          if (reson->mode_type != MODE_TYPE_OFF) { reson->mode_type = MODE_TYPE_FIX; }
          reson->cntd = random_int(0, time); } } }
    
    // "reson": set resonator time parameters in control
    else if(cmd == gensym("reson")) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        (bank->reson_arr + res)->cntd_type = MODE_CNTD_RESON; }  }

    // "rand": randomize resonator time parameters once, and set them in control
    else if(cmd == gensym("rand")) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        (bank->reson_arr + res)->cntd_type = MODE_CNTD_RESON;
        for (t_int32 t = 0; t < 4; t++){
          (bank->reson_arr + res)->times[t] = random_int(bank->times[2 * t], bank->times[2 * t + 1]); } } }

    // "randr": set bank time parameters in control, so time is randomized repeatedly
    else if (cmd == gensym("randr")) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        (bank->reson_arr + res)->cntd_type = MODE_CNTD_BANK; } }

    // "times": change time parameters, in the bank if 8 floats, in the resonators if 4 floats
    else if(cmd == gensym("times")) {
      if (argc == 7) {
        for (t_int32 res = 0; res < bank->reson_cnt; res++) {
          for (t_int32 t = 0; t < 4; t++) {
            (bank->reson_arr + res)->times[t] = (t_int32)(atom_getfloat(argv + t + 3) * x->msr); } } }
      else if (argc == 11) {
        for (t_int32 t = 0; t < 8; t++) {
          bank->times[t] = (t_int32)(atom_getfloat(argv + t + 3) * x->msr); } }
      else{
          MY_ERR("%s:  Invalid arguments for command \"%s\":  Either 4 or 8 floats expected.",
            sym->s_name, cmd->s_name); return; }  }

    // Otherwise the command is invalid
    else { MY_ERR("%s:  Invalid command.", sym->s_name); return; } }

  // ==== Applying command to ONE resonator

  // Or The second argument can be a resonator reference
  else {
    t_resonator *reson = modal_find_reson(x, bank, argv + 1, sym);
    if (reson == NULL) { MY_ERR("%s:  Invalid arguments:  Arg 1: resonator not found.", sym->s_name); return; }

    // The third argument should be a symbol indicating the command for the bank
    t_symbol *cmd = atom_getsym(argv + 2);

    // "resume": set all the resonators back to cycling
    if (cmd == gensym("resume")) {
      
      t_mode *mode = x->mode_arr + reson->mode_ind;

      // Only cycle resonators that are not cycling yet
      if ((reson->mode_ind != MODE_CYC_UP) && (reson->mode_ind != MODE_CYC_ON) &&
        (reson->mode_ind != MODE_CYC_DOWN) && (reson->mode_ind != MODE_CYC_OFF)) {

        t_int32 time = 0;

        // Calculate the total cycle time for waiting
        if (reson->cntd_type == MODE_CNTD_BANK) {
          for (t_int32 t = 0; t < 4; t++) {  time += bank->times[2 * t + 1]; } }
        else if (reson->cntd_type == MODE_CNTD_RESON) {
          for (t_int32 t = 0; t < 4; t++) {  time += reson->times[t]; } }

        // Set mode to wait up to the total cycle length
        reson->mode_ind = MODE_CYC_WAIT;
        if (reson->mode_type != MODE_TYPE_OFF) { reson->mode_type = MODE_TYPE_FIX; }
        reson->cntd = random_int(0, time); } }
    
    // "reson": set resonator time parameters in control
    else if(cmd == gensym("reson")) {
      reson->cntd_type = MODE_CNTD_RESON; }

    // "rand": randomize resonator time parameters once, and set them in control
    else if(cmd == gensym("rand")) {
      reson->cntd_type = MODE_CNTD_RESON;
      for (t_int32 t = 0; t < 4; t++){
        reson->times[t] = random_int(bank->times[2 * t], bank->times[2 * t + 1]); } }

    // "randr": set bank time parameters in control, so time is randomized repeatedly
    else if (cmd == gensym("randr")) {
      reson->cntd_type = MODE_CNTD_BANK; }

    // "times": change time parameters, in the bank if 8 floats, in the resonators if 4 floats
    else if(cmd == gensym("times")) {
      if (argc == 7) {
        for (t_int32 t = 0; t < 4; t++) {
          reson->times[t] = (t_int32)(atom_getfloat(argv + t + 3) * x->msr); } }
      else{
          MY_ERR("%s:  Invalid arguments for command \"%s\":  4 floats expected.",
            sym->s_name, cmd->s_name); return; }  }

    // Otherwise the command is invalid
    else { MY_ERR("%s:  Invalid command.", sym->s_name); return; } }
}

// ====  METHOD: MODE_DIFFUSION  ====

void mode_diffusion(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("mode_diffusion");

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { MY_ERR("%s:  Invalid arguments:  Arg 0: bank not found.", sym->s_name); return; }

  // ==== Applying command to ALL resonators

  // The second argument can be the symbol "all"
  // The command then applies to all the resonators of the bank
  if ((atom_gettype(argv + 1) == A_SYM) && (atom_getsym(argv + 1) == gensym("all"))) {
    t_resonator *reson = NULL;

    // The third argument should be a symbol indicating the command for the resonator
    t_symbol *cmd = atom_getsym(argv + 2);

    // Consider the possible commands

    // == Output to one set channel
    if (cmd == gensym("put")) {
      t_int32 ch = -1;

      if ((argc == 4) && (atom_gettype(argv + 3) == A_LONG)
          && ((ch = (t_int32)atom_getlong(argv + 3)) >= 0) && (ch <= 7)) {

        for (t_int32 res = 0; res < bank->reson_cnt; res++) {
          reson = bank->reson_arr + res;
          reson->diff_type = MODE_DIFF_ONE_S;
          reson->diff_ind = ch;
          reson->diff_cnt = 1;
          for (t_int32 ch2 = 0; ch2 < 8; ch2++) { reson->diff_mult[ch2] = 0; }
          reson->diff_mult[reson->diff_ind] = 1; } }
  
      else { MY_ERR("%s:  Invalid arguments for \"%s\" command.", sym->s_name, cmd->s_name); return; } }

    // == Output to one set channel
    if (cmd == gensym("1to1")) {

      for (t_int32 bk = 0; bk < 8; bk++) {
        t_bank *bank2 = x->bank_arr + bk;

        for (t_int32 res = 0; res < bank2->reson_cnt; res++) {
          reson = bank2->reson_arr + res;

          reson->diff_type = MODE_DIFF_ONE_S;
          reson->diff_ind = bk;
          reson->diff_cnt = 1;
          for (t_int32 ch2 = 0; ch2 < 8; ch2++) { reson->diff_mult[ch2] = 0; }
          reson->diff_mult[reson->diff_ind] = 1; } } }

    // == Output to all channels
    else if (cmd == gensym("all")) {
      for (t_int32 res = 0; res < bank->reson_cnt; res++) {
        reson = bank->reson_arr + res;
        reson->diff_type = MODE_DIFF_ALL;
        reson->diff_chg  = true; } }

    // == Output to one set channel
    else if (cmd == gensym("set")) {
      t_int32 ch = -1;

      if ((argc == 4) && (atom_gettype(argv + 3) == A_LONG)
          && ((ch = (t_int32)atom_getlong(argv + 3)) >= 0) && (ch <= 7)) {

        for (t_int32 res = 0; res < bank->reson_cnt; res++) {
          reson = bank->reson_arr + res;
          reson->diff_type = MODE_DIFF_ONE_S;
          reson->diff_sto  = ch;
          reson->diff_chg  = true; } }
  
      else { MY_ERR("%s:  Invalid arguments for \"%s\" command.", sym->s_name, cmd->s_name); return; } }

    // == Output to 1 or N channels chosen at random once or repeatedly
    else if ((cmd == gensym("rand")) || (cmd == gensym("randr"))) {
      t_int32 nb = -1;

      if ((argc == 4) && (atom_gettype(argv + 3) == A_LONG)
          && ((nb = (t_int32)atom_getlong(argv + 3)) >= 1) && (nb <= 8)) {
        for (t_int32 res = 0; res < bank->reson_cnt; res++) {
          reson = bank->reson_arr + res;

          if (nb == 1) {
            if (cmd == gensym("rand"))  { reson->diff_type = MODE_DIFF_ONE_R; }
            if (cmd == gensym("randr")) { reson->diff_type = MODE_DIFF_ONE_RR; } }
          else {
            if (cmd == gensym("rand"))  { reson->diff_type = MODE_DIFF_NUM_R; }
            if (cmd == gensym("randr")) { reson->diff_type = MODE_DIFF_NUM_RR; } }

          reson->diff_sto = nb;
          reson->diff_chg = true; } }

      else { MY_ERR("%s:  Invalid arguments for \"%s\" command.", sym->s_name, cmd->s_name); return; } }

    else { MY_ERR("%s:  Invalid command.", sym->s_name); return; } }

  // ==== Applying command to ONE resonator

  // Or The second argument can be a resonator reference
  else {
    t_resonator *reson = modal_find_reson(x, bank, argv + 1, sym);
    if (reson == NULL) { MY_ERR("%s:  Invalid arguments:  Arg 1: resonator not found.", sym->s_name); return; }

    // The third argument should be a symbol indicating the command for the resonator
    t_symbol *cmd = atom_getsym(argv + 2);

    // Consider the possible commands

    // == Output to all channels
    if (cmd == gensym("all")) {
      reson->diff_type = MODE_DIFF_ALL;
      reson->diff_chg  = true; }

    // == Output to one set channel
    else if (cmd == gensym("set")) {
      t_int32 ch = -1;

      if ((argc == 4) && (atom_gettype(argv + 3) == A_LONG)
          && ((ch = (t_int32)atom_getlong(argv + 3)) >= 0) && (ch <= 7)) {
        reson->diff_type = MODE_DIFF_ONE_S;
        reson->diff_sto  = ch;
        reson->diff_chg  = true; }
  
      else { MY_ERR("%s:  Invalid arguments for \"%s\" command.", sym->s_name, cmd->s_name); return; } }

    // == Output to 1 or N channels chosen at random once or repeatedly
    else if ((cmd == gensym("rand")) || (cmd == gensym("randr"))) {
      t_int32 nb = -1;

      if ((argc == 4) && (atom_gettype(argv + 3) == A_LONG)
          && ((nb = (t_int32)atom_getlong(argv + 3)) >= 1) && (nb <= 8)) {

        if (nb == 1) {
          if (cmd == gensym("rand"))  { reson->diff_type = MODE_DIFF_ONE_R; }
          if (cmd == gensym("randr")) { reson->diff_type = MODE_DIFF_ONE_RR; } }
        else {
          if (cmd == gensym("rand"))  { reson->diff_type = MODE_DIFF_NUM_R; }
          if (cmd == gensym("randr")) { reson->diff_type = MODE_DIFF_NUM_RR; } }

        reson->diff_sto = nb;
        reson->diff_chg = true; }

      else { MY_ERR("%s:  Invalid arguments for \"%s\" command.", sym->s_name, cmd->s_name); return; } }

    else { MY_ERR("%s:  Invalid command.", sym->s_name); return; }
  }
}

// ====  METHOD: MODE_RESONATOR  ====

void mode_resonator(t_modal *x, t_symbol *sym, t_int32 argc, t_atom *argv) {

  TRACE("mode_resonator");

  // The first argument should reference a bank of resonator
  t_bank *bank = bank_find(x, argv, sym);
  if (bank == NULL) { return; }

  // The second argument should reference a resonator
  t_resonator *reson = modal_find_reson(x, bank, argv + 1, sym);
  if (reson == NULL) { return; }

  // The third argument should be a symbol indicating the command for the resonator
  t_symbol *cmd = atom_getsym(argv + 2);

  // The fourth optional argument is a ramping time
  t_int32 ramp = 0;
  if (argc == 3) { ramp = (t_int32)(10 * x->msr); }
  else if ((argc >= 4) && ((atom_gettype(argv + 3) == A_LONG) || (atom_gettype(argv + 3) == A_FLOAT))) {
    ramp = (t_int32)(atom_getfloat(argv + 3) * x->msr); }
  else { MY_ERR("%s:  Invalid arguments.", sym->s_name); return; }

  // Consider the possible commands

  // ==== Turn the resonator on
  if (cmd == gensym("on")) {
    reson->mode_ind          = MODE_FIX_OFF;
    reson->cntd                = 0;
    reson->times[4] = ramp;  }

  // == Turn the resonator off
  else if (cmd == gensym("off")) {
    reson->mode_ind          = MODE_FIX_ON;
    reson->cntd                = 0;
    reson->times[4] = ramp;  }

  // == Toggle the resonator on or off
  else if (cmd == gensym("toggle")) {

    switch (reson->mode_ind) {
    case MODE_FIX_ON: case MODE_FIX_DOWN: case MODE_CYC_ON: case MODE_CYC_DOWN:
      reson->mode_ind = MODE_FIX_ON;
      reson->cntd = 0;
      reson->times[4] = ramp;
      break;
    case MODE_FIX_OFF: case MODE_FIX_CHG: case MODE_CYC_OFF: case MODE_CYC_UP:
      reson->mode_ind = MODE_FIX_OFF;
      reson->cntd = 0;
      reson->times[4] = ramp;
      break; }

    reson->cntd = 0; }

  // == Set the resonator back to cycle if it is fixed
  else if (cmd == gensym("cycle")) {
    
    switch (rand() % 4) {

    case 0:
      reson->mode_ind = MODE_CYC_OFF;
      reson->cntd = 0;
      break;

    case 1:
      reson->mode_ind = MODE_CYC_UP;
      reson->cntd = 0;
      break;

    case 2:
      reson->mode_ind = MODE_CYC_ON;
      reson->cntd = 0;
      break;

    case 3:
      reson->mode_ind = MODE_CYC_DOWN;
      reson->cntd = 0;
      break; } }

  // == Output rms for a given resonator and bank
  else if (cmd == gensym("rms")) {
    x->output_ind = (t_int32)(reson - bank->reson_arr);  }

  // == Start a pitch shift cycle
  /* XXX else if (cmd == gensym("shift")) {

    t_double shift = atom_getfloat(argv + 4);
    t_double ampl_mult = atom_getfloat(argv + 5);

    reson->mode_ind = MODE_SHIFT1;
    reson->mode_type = MODE_TYPE_VAR_A;
    reson->cntd = (t_int32)(500 * x->msr);
    reson->out_A_targ = 2;
    reson->param[0] = shift;
    reson->times[(x->mode_arr + MODE_SHIFT2)->time_ind] = ramp; }*/

  x->reson_cur = reson;
  
  // Send out a message to indicate resonator information
  t_atom mess_arr[4];
  atom_setlong(mess_arr, atom_getlong(argv + 1));
  atom_setfloat(mess_arr + 1, reson->ampl_ref);
  atom_setfloat(mess_arr + 2, reson->freq_ref);
  atom_setfloat(mess_arr + 3, reson->decay_ref);
  outlet_anything(x->outl_mess, gensym("reson"), 4, mess_arr);
}

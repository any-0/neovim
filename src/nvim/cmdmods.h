#pragma once

#include "nvim/api/private/helpers.h"
#include "nvim/ex_cmds_defs.h"
#include "nvim/window.h"

static inline Dict cmdmods_dict(const cmdmod_T *cmod, Arena *arena)
{
  // TODO(bfredl): nested keydict would be nice..
  Dict mods = arena_dict(arena, 20);

  Dict filter = arena_dict(arena, 2);
  PUT_C(filter, "pattern", CSTR_TO_ARENA_OBJ(arena, cmod->cmod_filter_pat));
  PUT_C(filter, "force", BOOLEAN_OBJ(cmod->cmod_filter_force));
  PUT_C(mods, "filter", DICT_OBJ(filter));

  PUT_C(mods, "silent", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_SILENT));
  PUT_C(mods, "emsg_silent", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_ERRSILENT));
  PUT_C(mods, "unsilent", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_UNSILENT));
  PUT_C(mods, "sandbox", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_SANDBOX));
  PUT_C(mods, "noautocmd", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_NOAUTOCMD));
  PUT_C(mods, "tab", INTEGER_OBJ(cmod->cmod_tab - 1));
  PUT_C(mods, "verbose", INTEGER_OBJ(cmod->cmod_verbose - 1));
  PUT_C(mods, "browse", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_BROWSE));
  PUT_C(mods, "confirm", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_CONFIRM));
  PUT_C(mods, "hide", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_HIDE));
  PUT_C(mods, "keepalt", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_KEEPALT));
  PUT_C(mods, "keepjumps", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_KEEPJUMPS));
  PUT_C(mods, "keepmarks", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_KEEPMARKS));
  PUT_C(mods, "keeppatterns", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_KEEPPATTERNS));
  PUT_C(mods, "lockmarks", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_LOCKMARKS));
  PUT_C(mods, "noswapfile", BOOLEAN_OBJ(cmod->cmod_flags & CMOD_NOSWAPFILE));
  PUT_C(mods, "vertical", BOOLEAN_OBJ(cmod->cmod_split & WSP_VERT));
  PUT_C(mods, "horizontal", BOOLEAN_OBJ(cmod->cmod_split & WSP_HOR));

  char *split;
  if (cmod->cmod_split & WSP_BOT) {
    split = "botright";
  } else if (cmod->cmod_split & WSP_TOP) {
    split = "topleft";
  } else if (cmod->cmod_split & WSP_BELOW) {
    split = "belowright";
  } else if (cmod->cmod_split & WSP_ABOVE) {
    split = "aboveleft";
  } else {
    split = "";
  }
  PUT_C(mods, "split", CSTR_AS_OBJ(split));

  return mods;
}

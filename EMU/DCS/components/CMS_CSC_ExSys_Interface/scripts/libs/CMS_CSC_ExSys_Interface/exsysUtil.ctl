/**@file

Contains some utility functions

@author Evaldas Juska (PH/UCM)
@date   October 2010
*/

#uses "CMS_CSC_common/emu_common.ctl"

/**
  * Does a mass dpGet on all dps + dpes pairs.
  * @return values of dpGet for all dps + all dpes.
  */
dyn_anytype exsys_massDpeGet(dyn_string dps, dyn_string dpes) {
  dyn_string fullDpes;
  for (int i=1; i <= dynlen(dps); i++) {
    for (int j=1; j <= dynlen(dpes); j++) {
      dynAppend(fullDpes, dps[i] + dpes[j]);
    }
  }
  dyn_anytype values = emu_dpGetMany(fullDpes);
  return values;
}

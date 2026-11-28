/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_12_MENUOPTIONS_H
#define GUARD_12_MENUOPTIONS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gKey_defns[18];

extern int gKey_count;

extern int gOrig_key_mapping[77];

extern char* gKey_names__options[153];

extern int gPending_entry;

extern int gCurrent_key;

extern tRadio_bastards gRadio_bastards__options[13];

extern int gLast_graph_sel__options;

extern tInterface_spec* gThe_interface_spec__options;

extern br_pixelmap* gDials_pix;

extern void C2_HOOK_FASTCALL DrawDial(int pWhich_one, int pWhich_stage);

extern void C2_HOOK_FASTCALL SoundOptionsStart(void);

extern void C2_HOOK_FASTCALL MoveDialFromTo(int pWhich_one, int pOld_stage, int pNew_stage);

extern int C2_HOOK_FASTCALL SoundOptionsDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out);

extern int C2_HOOK_FASTCALL SoundOptionsLeft(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL SoundOptionsRight(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL SoundClick(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern void C2_HOOK_FASTCALL PlayRadioOn2(int pIndex, int pValue);

extern void C2_HOOK_FASTCALL PlayRadioOff2(int pIndex, int pValue);

extern void C2_HOOK_FASTCALL PlayRadioOn__options(int pIndex, int pValue);

extern void C2_HOOK_FASTCALL PlayRadioOff__options(int pIndex, int pValue);

extern void C2_HOOK_FASTCALL DrawInitialRadios(void);

extern void C2_HOOK_FASTCALL RadioChanged(int pIndex, int pNew_value);

extern int C2_HOOK_FASTCALL GraphOptLeft(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL GraphOptRight(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL GraphOptUp(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL GraphOptDown(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL RadioClick(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL GraphOptGoAhead(int* pCurrent_choice, int* pCurrent_mode);

extern void C2_HOOK_FASTCALL PlotAGraphBox__options(int pIndex, int pColour_value);

extern void C2_HOOK_FASTCALL DrawAGraphBox__options(int pIndex);

extern void C2_HOOK_FASTCALL EraseAGraphBox__options(int pIndex);

extern void C2_HOOK_FASTCALL DrawGraphBox(int pCurrent_choice, int pCurrent_mode);

extern void C2_HOOK_FASTCALL SaveOrigKeyMapping(void);

extern void C2_HOOK_FASTCALL SaveKeyMapping__options(void);

extern void C2_HOOK_FASTCALL ChangeKeyMapIndex(int pNew_one);

extern void C2_HOOK_FASTCALL SetKeysToDefault(void);

extern void C2_HOOK_FASTCALL DrawKeyAssignments(int pCurrent_choice, int pCurrent_mode);

extern void C2_HOOK_FASTCALL GetKeyCoords(int pIndex, int* pY, int* pName_x, int* pKey_x, int* pEnd_box);

extern int C2_HOOK_FASTCALL KeyAssignLeft(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL KeyAssignRight(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL KeyAssignUp(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL KeyAssignDown(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL KeyAssignGoAhead(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL MouseyClickBastard(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern void C2_HOOK_FASTCALL DrawInitialKMRadios(void);

extern void C2_HOOK_FASTCALL LoadSoundOptionsData(void);

extern void C2_HOOK_FASTCALL FreeSoundOptionsData(void);

extern void C2_HOOK_FASTCALL DrawDisabledOptions(void);

extern tFlicette gSound_options_flicker_on[3];

extern tFlicette gSound_options_flicker_off[3];

extern tFlicette gSound_options_push[3];

extern tMouse_area gSound_options_mouse_areas[3];

extern tInterface_spec gSound_options_interface_spec;

extern tFlicette gGraphics_options_flicker_on[14];

extern tFlicette gGraphics_options_flicker_off[14];

extern tFlicette gGraphics_options_push[14];

extern tMouse_area gGraphics_options_mouse_areas[14];

extern tInterface_spec gGraphics_options_interface_spec;

extern tFlicette gControl_options_flicker_on[4];

extern tFlicette gControl_options_flicker_off[4];

extern tFlicette gControl_options_push[4];

extern tMouse_area gControl_options_mouse_areas[5];

extern tInterface_spec gControl_options_interface_spec;

extern tFlicette gOptions_flicker_on[4];

extern tFlicette gOptions_flicker_off[4];

extern tFlicette gOptions_push[4];

extern tMouse_area gOptions_mouse_areas[4];

extern tInterface_spec gOptions_interface_spec;

extern void C2_HOOK_FASTCALL DoSoundOptions(void);

extern void C2_HOOK_FASTCALL GetGraphicsOptions(void);

extern void C2_HOOK_FASTCALL DoGraphicsOptions(void);

extern void C2_HOOK_FASTCALL CalibrateJoysticks(void);

extern void C2_HOOK_FASTCALL LoadKeyNames__options(void);

extern void C2_HOOK_FASTCALL DoControlOptions(void);

extern void C2_HOOK_FASTCALL DoOptions(void);

extern void C2_HOOK_FASTCALL SetGraphicsOptions(void);

extern void C2_HOOK_FASTCALL DisposeKeyNames__options(void);

// SaveKeyMapping (see 72-interface)

extern void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend);

#endif // GUARD_12_MENUOPTIONS_H

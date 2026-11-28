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

extern void C2_HOOK_FASTCALL LoadSoundOptionsData(void);

extern void C2_HOOK_FASTCALL FreeSoundOptionsData(void);

extern void C2_HOOK_FASTCALL DrawDisabledOptions(void);

extern void C2_HOOK_FASTCALL SetGraphicsOptions(void);

// SaveOrigKeyMapping

// SaveKeyMapping (see 72-interface)

// ChangeKeyMapIndex

extern void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend);

#endif // GUARD_12_MENUOPTIONS_H

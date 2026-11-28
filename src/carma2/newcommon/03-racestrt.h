/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_03_RACESTRT_H
#define GUARD_03_RACESTRT_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gOur_starting_position;
extern tU32 gLast_host_query;

extern int gChange_race_net_mode;

extern int C2_HOOK_FASTCALL UpRace(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL DownRace(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL ClickOnRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL UpClickRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL DownClickRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL ChangeCarGoAhead(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one);

extern int C2_HOOK_FASTCALL SortOpponents(void);

// DoGridPosition

extern int C2_HOOK_FASTCALL NetSynchStartDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out);

extern int C2_HOOK_FASTCALL NetSynchStartGoAhead(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL ExitWhenReady(int* pCurrent_choice, int* pCurrent_mode);

// ReceivedViewCar

#endif // GUARD_03_RACESTRT_H

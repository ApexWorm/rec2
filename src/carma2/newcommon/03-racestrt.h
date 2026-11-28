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

extern tU32 gNet_synch_start_racestrt;
extern tNet_game_details* gChoose_car_net_game;
extern tInterface_spec* gStart_interface_spec;
extern tNet_sequence_type gNet_race_sequence_racestrt;
extern int gChange_race_net_mode;
extern br_pixelmap* gBullet_image;
extern int gCurrent_race_index;

extern int C2_HOOK_FASTCALL UpRace(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL DownRace(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL ClickOnRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL UpClickRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL DownClickRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern void C2_HOOK_FASTCALL StartChangeRace(void);

extern int C2_HOOK_FASTCALL ChangeRace(int* pRace_index, int pNet_mode, tNet_sequence_type pNet_race_sequence);

extern void C2_HOOK_FASTCALL DrawCar(int pCurrent_choice, int pCurrent_mode);

extern void C2_HOOK_FASTCALL SetCarFlic(void);

extern int C2_HOOK_FASTCALL UpCar(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL DownCar(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL UpClickCar(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL DownClickCar(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset);

extern int C2_HOOK_FASTCALL ChangeCarGoAhead(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL ChangeCar(int pNet_mode, int* pCar_index, tNet_game_details* pNet_game);

extern int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one);

extern int C2_HOOK_FASTCALL SortOpponents(void);

// DoGridPosition

extern void C2_HOOK_FASTCALL CheckPlayersAreResponding(void);

extern void C2_HOOK_FASTCALL NetSynchStartDraw(int pCurrent_choice, int pCurrent_mode);

extern int C2_HOOK_FASTCALL NetSynchStartDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out);

extern int C2_HOOK_FASTCALL NetSynchStartGoAhead(int* pCurrent_choice, int* pCurrent_mode);

extern int C2_HOOK_FASTCALL ExitWhenReady(int* pCurrent_choice, int* pCurrent_mode);

extern tSO_result C2_HOOK_FASTCALL NetSynchRaceStart2_racestrt(tNet_synch_mode pMode);

extern tSO_result C2_HOOK_FASTCALL NetSynchRaceStart(void);

// ReceivedViewCar

#endif // GUARD_03_RACESTRT_H

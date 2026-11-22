/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_11_WORLD1_H
#define GUARD_11_WORLD1_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gDelete_count;
extern br_actor* gDelete_list[500];

extern br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMatrix, void* pArg);

// DeleteAnyZeroBastards

extern br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMatrix, void* pArg);

extern int C2_HOOK_FASTCALL FindSpecVolIndex(br_actor* pActor);

extern void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1);

extern void C2_HOOK_FASTCALL SetSpecVolMatSize(br_actor* pActor);

extern void C2_HOOK_FASTCALL UpdateSpecVol(void);

// PutAdditionalNonCarsOnToAdditionalActors

// PutAdditionalNonCarsOnToNonTrackActor

extern void C2_HOOK_FASTCALL SaveAdditionalActors(void);

extern void C2_HOOK_FASTCALL DoSaveAdditionalStuff(void);

extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);

extern void C2_HOOK_FASTCALL DropSpecVol0(void);

extern br_material* C2_HOOK_FASTCALL GetInternalMat(void);

extern br_material* C2_HOOK_FASTCALL GetExternalMat(void);

extern void C2_HOOK_FASTCALL DropSpecVol1(void);

extern void C2_HOOK_FASTCALL DropSpecVol(int pIndex);

extern void C2_HOOK_FASTCALL BuildSpecVolModel(tSpecial_volume* pSpec, int pIndex, br_material* pInt_mat, br_material* pExt_mat);

extern void BuildSpecVolModel(tSpecial_volume* pSpec, int pIndex, br_material* pInt_mat, br_material* pExt_mat);

extern void C2_HOOK_FASTCALL IdentifySpecVol(void);

extern void C2_HOOK_FASTCALL DelSpecVolumeGraph(int pIndex);

extern void C2_HOOK_FASTCALL ShowSpecialVolumes(void);

extern void C2_HOOK_FASTCALL HideSpecialVolumes(void);

#endif // GUARD_11_WORLD1_H

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
extern br_actor* gKnown_actor;
extern br_vector3 gActor_centre;
extern tRotate_mode gCurrent_rotate_mode;
extern tScale_mode gCurrent_scale_mode;
extern int gDropped_powerup_index;
extern char* gDrop_actor_name;

extern br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMatrix, void* pArg);

extern void C2_HOOK_FASTCALL DeleteAnyZeroBastards(void);

extern br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMatrix, void* pArg);

extern int C2_HOOK_FASTCALL FindSpecVolIndex(br_actor* pActor);

extern void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1);

extern void C2_HOOK_FASTCALL SetSpecVolMatSize(br_actor* pActor);

extern void C2_HOOK_FASTCALL UpdateSpecVol(void);

extern void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToAdditionalActors(void);

extern void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToNonTrackActor(void);

extern void C2_HOOK_FASTCALL SaveAdditionalActors(void);

extern void C2_HOOK_FASTCALL DoSaveAdditionalStuff(void);

extern void C2_HOOK_FASTCALL FUN_00507340(void);

extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);

extern void C2_HOOK_FASTCALL FUN_00507580(void);

extern void C2_HOOK_FASTCALL UniquificateActorsName(br_actor* pUniverse_actor, br_actor* pActor, int pPowerup_index);

extern void C2_HOOK_FASTCALL AccessoryHeadup(br_actor* pActor, const char* pPrefix);

extern void C2_HOOK_FASTCALL DuplicateIfNotAmpersand(br_actor* pActor);

extern void C2_HOOK_FASTCALL DropActor(int pIndex, int pPowerup);

extern intptr_t C2_HOOK_CDECL CalcHighestNonAmID(br_actor* pActor, void* pContext);

extern intptr_t C2_HOOK_CDECL SetID(br_actor* pActor, void* pContext);

extern intptr_t C2_HOOK_CDECL CalcHighestID(br_actor* pActor, void* pContext);

extern intptr_t C2_HOOK_CDECL SetIDAndDupModel(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL DropActor0(void);

extern void C2_HOOK_FASTCALL DropActor1(void);

extern void C2_HOOK_FASTCALL DropActor2(void);

extern void C2_HOOK_FASTCALL DropActor3(void);

extern void C2_HOOK_FASTCALL DropActor4(void);

extern void C2_HOOK_FASTCALL DropActor5(void);

extern void C2_HOOK_FASTCALL DropActor6(void);

extern void C2_HOOK_FASTCALL DropActor7(void);

extern void C2_HOOK_FASTCALL DropActor8(void);

extern void C2_HOOK_FASTCALL DropActor9(void);

extern void C2_HOOK_FASTCALL FUN_00508370(void);

extern void C2_HOOK_FASTCALL FUN_00508380(void);

extern void C2_HOOK_FASTCALL FUN_00508390(void);

extern void C2_HOOK_FASTCALL FUN_005083a0(void);

extern void C2_HOOK_FASTCALL FUN_005083b0(void);

extern void C2_HOOK_FASTCALL FUN_005083c0(void);

extern void C2_HOOK_FASTCALL FUN_005083d0(void);

extern void C2_HOOK_FASTCALL FUN_005083e0(void);

extern void C2_HOOK_FASTCALL FUN_005083f0(void);

extern void C2_HOOK_FASTCALL FUN_00508400(void);

extern void C2_HOOK_FASTCALL IdentifyAcc(void);

extern void C2_HOOK_FASTCALL FUN_00508690(const char* pName);

extern intptr_t C2_HOOK_CDECL DelReferencedModels(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL DeleteAcc(void);

extern void C2_HOOK_FASTCALL SnapAccToVertical(void);

extern void C2_HOOK_FASTCALL RotateAccessory(br_angle pAngle);

extern void C2_HOOK_FASTCALL RotateAccL(void);

extern intptr_t C2_HOOK_CDECL OffsetModel(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL OffsetActor(br_actor* pActor, br_vector3* pOffset);

extern void C2_HOOK_FASTCALL CentreActor(br_actor* pActor, br_vector3* pOffset);

extern void C2_HOOK_FASTCALL RotateAccL2(void);

extern void C2_HOOK_FASTCALL RotateAccL3(void);

extern void C2_HOOK_FASTCALL RotateAccL4(void);

extern void C2_HOOK_FASTCALL RotateAccR(void);

extern void C2_HOOK_FASTCALL RotateAccR2(void);

extern void C2_HOOK_FASTCALL RotateAccR3(void);

extern void C2_HOOK_FASTCALL RotateAccR4(void);

extern void C2_HOOK_FASTCALL CycleAccRotate(void);

extern void C2_HOOK_FASTCALL CycleAccScale(void);

extern void C2_HOOK_FASTCALL ScaleAccUp2(void);

extern void C2_HOOK_FASTCALL ScaleAccessory(float pScaling_factor);

extern void C2_HOOK_FASTCALL ScaleAccUp3(void);

extern void C2_HOOK_FASTCALL ScaleAccUp4(void);

extern void C2_HOOK_FASTCALL ScaleAccDown2(void);

extern void C2_HOOK_FASTCALL ScaleAccDown3(void);

extern void C2_HOOK_FASTCALL ScaleAccDown4(void);

extern void C2_HOOK_FASTCALL MoveAccessory(br_scalar pX_shift, br_scalar pY_shift, br_scalar pZ_shift);

extern void C2_HOOK_FASTCALL MoveXAccL(void);

extern void C2_HOOK_FASTCALL MoveXAccL2(void);

extern void C2_HOOK_FASTCALL MoveXAccL3(void);

extern void C2_HOOK_FASTCALL MoveXAccL4(void);

extern void C2_HOOK_FASTCALL MoveXAccR(void);

extern void C2_HOOK_FASTCALL MoveXAccR2(void);

extern void C2_HOOK_FASTCALL MoveXAccR3(void);

extern void C2_HOOK_FASTCALL MoveXAccR4(void);

extern void C2_HOOK_FASTCALL MoveYAccL(void);

extern void C2_HOOK_FASTCALL MoveYAccL2(void);

extern void C2_HOOK_FASTCALL MoveYAccL3(void);

extern void C2_HOOK_FASTCALL MoveYAccL4(void);

extern void C2_HOOK_FASTCALL MoveYAccR(void);

extern void C2_HOOK_FASTCALL MoveYAccR2(void);

extern void C2_HOOK_FASTCALL MoveYAccR3(void);

extern void C2_HOOK_FASTCALL MoveYAccR4(void);

extern void C2_HOOK_FASTCALL MoveZAccL(void);

extern void C2_HOOK_FASTCALL MoveZAccL2(void);

extern void C2_HOOK_FASTCALL MoveZAccL3(void);

extern void C2_HOOK_FASTCALL MoveZAccL4(void);

extern void C2_HOOK_FASTCALL MoveZAccR(void);

extern void C2_HOOK_FASTCALL MoveZAccR2(void);

extern void C2_HOOK_FASTCALL MoveZAccR3(void);

extern void C2_HOOK_FASTCALL MoveZAccR4(void);

extern void C2_HOOK_FASTCALL DropSpecVol0(void);

extern br_material* C2_HOOK_FASTCALL GetInternalMat(void);

extern br_material* C2_HOOK_FASTCALL GetExternalMat(void);

extern void C2_HOOK_FASTCALL DropSpecVol1(void);

extern void C2_HOOK_FASTCALL DropSpecVol(int pIndex);

extern void C2_HOOK_FASTCALL BuildSpecVolModel(tSpecial_volume* pSpec, int pIndex, br_material* pInt_mat, br_material* pExt_mat);

extern void BuildSpecVolModel(tSpecial_volume* pSpec, int pIndex, br_material* pInt_mat, br_material* pExt_mat);

extern void C2_HOOK_FASTCALL DropSpecVol2(void);

extern void C2_HOOK_FASTCALL DropSpecVol3(void);

extern void C2_HOOK_FASTCALL DropSpecVol4(void);

extern void C2_HOOK_FASTCALL DropSpecVol5(void);

extern void C2_HOOK_FASTCALL DropSpecVol6(void);

extern void C2_HOOK_FASTCALL DropSpecVol7(void);

extern void C2_HOOK_FASTCALL DropSpecVol8(void);

extern void C2_HOOK_FASTCALL DropSpecVol9(void);

extern void C2_HOOK_FASTCALL IdentifySpecVol(void);

extern void C2_HOOK_FASTCALL DeleteSpecVol(void);

extern void C2_HOOK_FASTCALL DelSpecVolumeGraph(int pIndex);

extern void C2_HOOK_FASTCALL RotateSpecVolL(void);

extern void C2_HOOK_FASTCALL RotateSpecVolL2(void);

extern void C2_HOOK_FASTCALL RotateSpecVolL3(void);

extern void C2_HOOK_FASTCALL RotateSpecVolL4(void);

extern void C2_HOOK_FASTCALL RotateSpecVolR(void);

extern void C2_HOOK_FASTCALL RotateSpecVolR2(void);

extern void C2_HOOK_FASTCALL RotateSpecVolR3(void);

extern void C2_HOOK_FASTCALL RotateSpecVolR4(void);

extern void C2_HOOK_FASTCALL CycleSpecVolRotate(void);

extern void C2_HOOK_FASTCALL CycleSpecVolScale(void);

extern void C2_HOOK_FASTCALL ScaleSpecVolUp2(void);

extern void C2_HOOK_FASTCALL ScaleSpecVolUp3(void);

extern void C2_HOOK_FASTCALL ScaleSpecVolUp4(void);

extern void C2_HOOK_FASTCALL ScaleSpecVolDown2(void);

extern void C2_HOOK_FASTCALL ScaleSpecVolDown3(void);

extern void C2_HOOK_FASTCALL ScaleSpecVolDown4(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolL(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolL2(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolL3(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolL4(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolR(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolR2(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolR3(void);

extern void C2_HOOK_FASTCALL MoveXSpecVolR4(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolL(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolL2(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolL3(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolL4(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolR(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolR2(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolR3(void);

extern void C2_HOOK_FASTCALL MoveYSpecVolR4(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolL(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolL2(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolL3(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolL4(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolR(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolR2(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolR3(void);

extern void C2_HOOK_FASTCALL MoveZSpecVolR4(void);

extern void C2_HOOK_FASTCALL SnapSpecVolToVertical(void);

extern void C2_HOOK_FASTCALL ShowSpecialVolumes(void);

extern void C2_HOOK_FASTCALL HideSpecialVolumes(void);

#endif // GUARD_11_WORLD1_H

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_56_AIWORLD_H
#define GUARD_56_AIWORLD_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include "brender/brender.h"

#include <stdio.h>

extern void C2_HOOK_FASTCALL MakeVertexAndOffsetIt(br_scalar pX, br_scalar pY, br_scalar pZ, br_model* pModel, int pVertex_num, const br_vector3* pOffset);

extern void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InitPanGameAIWorld(void);

extern void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF);

extern void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info);

// InitialiseAIWorldForRace

extern void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

// MungeAIWorld

extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);

extern void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp);

extern void C2_HOOK_FASTCALL AllocateMatsForOppoPathModel(void);

extern void C2_HOOK_FASTCALL FUN_004014a0(void);

extern void C2_HOOK_FASTCALL FUN_004017c0(void);

extern br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex);

extern void C2_HOOK_FASTCALL MakeAIEditCubeHere(br_model* pModel, int pVertex_index, int pFace_index, const br_vector3* pPos, br_material* pMaterial_1, br_material* pMaterial_2, br_material* pMaterial_3);

extern void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pNegative_x_vector, const br_vector3* pStart, const br_vector3* pFinish, br_scalar pLength);

extern void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVertex_index, int pFace_index, const br_vector3* pStart, const br_vector3* pFinish, br_material* pMaterial_1, br_material* pMaterial_2);

#endif // GUARD_56_AIWORLD_H

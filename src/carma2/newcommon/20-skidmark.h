/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_20_SKIDMARK_H
#define GUARD_20_SKIDMARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern const char* gBoring_material_names[2];
extern const char* gMaterial_names[2];
extern const char* gShrapnel_type_names[4];
extern const char* gInitial_position_type_names[3];
extern const char* gInitial_position_sphere_where[2];
extern const char* gExplosion_rotate_names[2];
extern br_material* gMaterial[2];
extern tSkid gSkids[100];
extern tU16 gCurrent_skid;

extern void C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step);

// MaterialFromIndex

extern void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

extern int C2_HOOK_FASTCALL FarFromLine2D(br_vector3* pPt, br_vector3* pL1, br_vector3* pL2);

extern int C2_HOOK_FASTCALL Reflex2D(br_vector3* pPt, br_vector3* pL1, br_vector3* pL2);

extern void C2_HOOK_FASTCALL InitSkids(void);

extern void C2_HOOK_FASTCALL HideSkid(int pSkid_num);

extern void C2_HOOK_FASTCALL HideSkids(void);

extern br_scalar C2_HOOK_FASTCALL SkidLen(int pSkid);

extern int C2_HOOK_FASTCALL UglyWidthChange(int pSkid, br_scalar pWidth);

// GenericSkidSection

extern int C2_HOOK_FASTCALL SkidSection(tU16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step);

extern void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num);

extern void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL SkidsPerFrame(void);

#endif // GUARD_20_SKIDMARK_H

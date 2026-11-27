/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_26_OPPOPROC_H
#define GUARD_26_OPPOPROC_H

#include "rec2_types.h"

// DoNotDprintf

extern int C2_HOOK_FASTCALL StraightestArcForCorner2D(br_vector2* pCent, br_scalar* pRadius, br_scalar* pEntry_length, int* pLeft_not_right, const br_vector2* p1, const br_vector2* p2, const br_vector2* p3, br_scalar pWidth12, br_scalar pWidth23);

extern float C2_HOOK_FASTCALL CornerFudge(tCar_spec* pCar_spec);

extern float C2_HOOK_FASTCALL MaxCurvatureForCarSpeed(tCar_spec* pCar, float pSpeed);

extern br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2);

extern int C2_HOOK_FASTCALL RoughlyColinear(const br_vector2* p1, const br_vector2* p2, const br_vector2* p3);

extern int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection_no, tOpponent_spec* pOpponent_spec);

extern int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec);

extern int C2_HOOK_FASTCALL TimeToBeBrakingWhenStruggling(tOpponent_spec* pOpponent_spec);

extern void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

extern int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection_no, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec);

extern br_scalar C2_HOOK_FASTCALL Distance2D(const br_vector3* pV1, const br_vector3* pV2);

extern br_scalar C2_HOOK_FASTCALL SectionLength2D(const tOpponent_spec* pOpponent_spec, tS16 pSection_no);

extern int C2_HOOK_FASTCALL NearestSectionInStraight(const br_vector2* pPos2d, const br_vector2* pStart2d, const br_vector2* pFinish2d, tS16 pSection_no, int pEnd_section, const tOpponent_spec* pOpponent_spec);

extern void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* pSocs, int pCount_socs, float* pDesired_speed, float pSpeed, br_vector2* pPos2d, tCorner* pCorners, tOpponent_spec* pOpponent_spec);

extern int C2_HOOK_FASTCALL CalcSOCs(int pSection_no, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL StuffDataFromCorner(tFollow_path_data* pFollow_path, tCorner* pCorner);

extern void C2_HOOK_FASTCALL SetCurvature(tCar_spec* pCar_spec, float pCurvature);

extern tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle);

extern tFollow_path_result C2_HOOK_FASTCALL FollowCheatyPath(tOpponent_spec* pOpponent_spec);

#endif // GUARD_26_OPPOPROC_H

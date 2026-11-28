/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_31_RACESUMM_H
#define GUARD_31_RACESUMM_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tWreck_info gWreck_array[30];

// SortScores

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary

extern void C2_HOOK_FASTCALL PrepareBoundingRadius_racesumm(br_model* model);

extern int C2_HOOK_FASTCALL MatrixIsIdentity(br_matrix34* pMat);

extern void C2_HOOK_FASTCALL SpinWrecks(tU32 pFrame_period);

extern int C2_HOOK_FASTCALL AnyBuyableCars(void);

extern void C2_HOOK_FASTCALL RecordAPOAndCredits(void);

// PickRaceWhenGameCompleted

// MungeRaceProgression

#endif // GUARD_31_RACESUMM_H

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "31-racesumm.h"

#include "30-opponent.h"

#include "globvars.h"

#include <math.h>

// SortScores

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary

// FUNCTION: CARMA2_HW 0x004e30c0
// FUNCTION: CARMA2_SW 0x004df440
// FUNCTION: CARMA2_CHEAT 0x004e8c40
void C2_HOOK_FASTCALL PrepareBoundingRadius_racesumm(br_model* model) {
    float d;
    float max;
    int v;
    br_vertex* vp;

    max = 0.0f;
    for (v = 0, vp = model->vertices; v < model->nvertices; v++, vp++) {
        d = BrVector3LengthSquared(&vp->p);
        if (d > max) {
            max = d;
        }
    }
    model->radius = (br_scalar)sqrt(max);
}

// FUNCTION: CARMA2_HW 0x004e33d0
// FUNCTION: CARMA2_SW 0x004df760
// FUNCTION: CARMA2_CHEAT 0x004e8f50
int C2_HOOK_FASTCALL AnyBuyableCars(void) {
    int count;
    int i;

    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        if (GetCarSpec(eVehicle_opponent, i) != NULL) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e3410
// FUNCTION: CARMA2_SW 0x004df7a0
// FUNCTION: CARMA2_CHEAT 0x004e8f90
void C2_HOOK_FASTCALL RecordAPOAndCredits(void) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, power_up_levels, 0x18a8);

    gMoney_pre_race = gProgram_state.credits;
    gAPO_pre_race[0] = gProgram_state.current_car.power_up_levels[0];
    gAPO_pre_race[1] = gProgram_state.current_car.power_up_levels[1];
    gAPO_pre_race[2] = gProgram_state.current_car.power_up_levels[2];
}

// PickRaceWhenGameCompleted

// MungeRaceProgression


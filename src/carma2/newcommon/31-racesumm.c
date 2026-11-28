/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "31-racesumm.h"

#include "16-graphics1.h"
#include "30-opponent.h"

#include "globvars.h"

#include <math.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x006a0da0
// GLOBAL: CARMA2_SW 0x0069adf8
// GLOBAL: CARMA2_CHEAT 0x006a8918
tWreck_info gWreck_array[30];

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

inline int C2_HOOK_FASTCALL MatrixIsIdentity(br_matrix34* pMat) {

    if (pMat->m[0][0] == 1.0f && pMat->m[1][1] == 1.0f && pMat->m[2][2] == 1.0f
        && pMat->m[0][1] == 0.0f && pMat->m[0][2] == 0.0f && pMat->m[1][0] == 0.0f
        && pMat->m[1][2] == 0.0f && pMat->m[2][0] == 0.0f && pMat->m[2][1] == 0.0f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e3130
// FUNCTION: CARMA2_SW 0x004df4b0
// FUNCTION: CARMA2_CHEAT 0x004e8cb0
void C2_HOOK_FASTCALL SpinWrecks(tU32 pFrame_period) {
    int i;
    br_vector3 translation;
    br_matrix34 old_mat;

    for (i = 0; i < gFrontend_wrecks_car_count; i++) {
        if (gWreck_array[i].customised == 0) {
            BrMatrix34RotateY(&gWreck_array[i].rotation, BrDegreeToAngle(0.05 * pFrame_period));
        }
        translation = gWreck_array[i].actor->t.t.translate.t;
        BrVector3Set(&gWreck_array[i].actor->t.t.translate.t, 0.0f, 0.0f, 0.0f);
        BrMatrix34Post(&gWreck_array[i].actor->t.t.mat, &gWreck_array[i].rotation);
        if (!MatrixIsIdentity(&gWreck_array[i].actor->t.t.mat)) {
            memcpy(&old_mat, &gWreck_array[i].actor->t.t.mat, sizeof(br_matrix34));
            BrMatrix34LPNormalise(&gWreck_array[i].actor->t.t.mat, &old_mat);
            BrMatrix34PostScale(&gWreck_array[i].actor->t.t.mat,
                gWreck_array[i].scaling_factor, gWreck_array[i].scaling_factor, gWreck_array[i].scaling_factor);
        }
        gWreck_array[i].actor->t.t.translate.t = translation;
    }
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


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "21-mainloop.h"

#include "16-graphics1.h"
#include "36-trig.h"
#include "41-utility.h"
#include "59-camera.h"
#include "61-pedestrn-phys.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x0068c814
// GLOBAL: CARMA2_SW 0x00686868
// GLOBAL: CARMA2_CHEAT 0x006940b4
tU32 gOld_camera_time;

// GLOBAL: CARMA2_HW 0x0068c820
// GLOBAL: CARMA2_SW 0x00686874
// GLOBAL: CARMA2_CHEAT 0x006940c0
tU32 gActual_last_tick_count;

// GLOBAL: CARMA2_HW 0x0068c824
// GLOBAL: CARMA2_SW 0x00686878
// GLOBAL: CARMA2_CHEAT 0x006940c4
tU32 gAverage_frame_period;

// GLOBAL: CARMA2_HW 0x00761ef0
// GLOBAL: CARMA2_SW 0x00758450
// GLOBAL: CARMA2_CHEAT 0x00769590
int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */

// GLOBAL: CARMA2_HW 0x0074b740
// GLOBAL: CARMA2_SW 0x00741e00
// GLOBAL: CARMA2_CHEAT 0x00752de0
int gAuto_increase_credits_dt[3];

// GLOBAL: CARMA2_HW 0x0074b720
// GLOBAL: CARMA2_SW 0x00741de0
// GLOBAL: CARMA2_CHEAT 0x00752dc0
tU32 gNet_auto_increase_credits_dt[8];

// GLOBAL: CARMA2_HW 0x007447d4
// GLOBAL: CARMA2_SW 0x0073b294
// GLOBAL: CARMA2_CHEAT 0x0074be74
int gPed_count;

// GLOBAL: CARMA2_HW 0x0068c738
// GLOBAL: CARMA2_SW 0x00686788
// GLOBAL: CARMA2_CHEAT 0x00693fd8
br_vector3 gPrev_camera_position;

// GLOBAL: CARMA2_HW 0x00704e44
// GLOBAL: CARMA2_SW 0x006fbb24
// GLOBAL: CARMA2_CHEAT 0x0070c4e4
br_scalar gCamera_to_horiz_angle;

// GLOBAL: CARMA2_HW 0x0068c748
// GLOBAL: CARMA2_SW 0x00686798
// GLOBAL: CARMA2_CHEAT 0x00693fe8
int gQueued_wasted_messages[5] = { 0 };

// GLOBAL: CARMA2_HW 0x0068c75c
// GLOBAL: CARMA2_SW 0x006867ac
int gCompletion_bonus_post_race = 0;

// GLOBAL: CARMA2_HW 0x0068c760
// GLOBAL: CARMA2_SW 0x006867b0
// GLOBAL: CARMA2_CHEAT 0x00694000
int gQueued_wasted_messages_count;

// GLOBAL: CARMA2_HW 0x0068c77c
// GLOBAL: CARMA2_SW 0x006867d0
// GLOBAL: CARMA2_CHEAT 0x0069401c
tU32 gLast_wasted_message_start;

// CalculateFrameRate

void C2_HOOK_FASTCALL LoseOldestWastedMassage(void) {
    int i;

    for (i = 1; i < gQueued_wasted_messages_count; i++) {
        gQueued_wasted_messages[i - 1] = gQueued_wasted_messages[i];
    }
    gQueued_wasted_messages_count--;
    gLast_wasted_message_start = GetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00492620
// FUNCTION: CARMA2_SW 0x0048f840
// FUNCTION: CARMA2_CHEAT 0x004925c0
void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex) {

    if (gQueued_wasted_messages_count == (int)REC2_ASIZE(gQueued_wasted_messages)) {
        LoseOldestWastedMassage();
    }
    if (gQueued_wasted_messages_count == 0) {
        gLast_wasted_message_start = GetTotalTime();
    }
    gQueued_wasted_messages[gQueued_wasted_messages_count] = pIndex;
    gQueued_wasted_messages_count++;
}

// MungeHeadups

// FUNCTION: CARMA2_HW 0x00492680
// FUNCTION: CARMA2_SW 0x0048f8a0
// FUNCTION: CARMA2_CHEAT 0x00492620
void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period) {
    tU32 new_tick_count;
    tU32 new_camera_tick_count;
    int error;

    if (gAction_replay_mode) {
        tU32 last_tick_count;
        tU32 camera_period;

        last_tick_count = gLast_tick_count;
        gFrame_period = abs((int)(gLast_replay_frame_time - last_tick_count));
        gLast_tick_count = gLast_replay_frame_time;
        new_camera_tick_count = PDGetTotalTime();
        new_tick_count = GetTotalTime();
        if (gOld_camera_time != 0) {
            camera_period = new_camera_tick_count - gOld_camera_time;
        } else {
            camera_period = 0;
        }
        *pCamera_period = camera_period;
        gOld_camera_time = new_camera_tick_count;
        if (gFrame_period != 0) {
            *pCamera_period = gFrame_period;
        }
    } else {
        int new_period;

        new_tick_count = PDGetTotalTime();
        gLast_tick_count += gFrame_period;
        error = new_tick_count - gLast_tick_count;
        gFrame_period = new_tick_count - gActual_last_tick_count;
        if (gFrame_period > 500 && new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
            gLast_tick_count = new_tick_count;
            if (gNet_mode != eNet_mode_none) {
                switch (gNet_mode) {
                case eNet_mode_client:
                    gProgram_state.current_car.collision_info->field_0x49c = 0;
                    break;
                case eNet_mode_host:
                    break;
                default:
                    break;
                }
            }
        }
        gAverage_frame_period = 9 * gAverage_frame_period / 10 + gFrame_period;
        if (new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
        }
        new_period = gFrame_period + error;
        if (new_period <= 0) {
            gLast_tick_count = new_tick_count;
        } else {
            gFrame_period = new_period;
        }
        *pCamera_period = gFrame_period;
        gActual_last_tick_count = new_tick_count;
    }
    if (gFrame_period < 10) {
        gFrame_period = 10;
        gLast_tick_count = new_tick_count;
    } else if (gFrame_period > 1000) {
        gFrame_period = 1000;
        gLast_tick_count = new_tick_count;
    }
    if (*pCamera_period < 10) {
        *pCamera_period = 10;
    } else if (*pCamera_period > 1000) {
        *pCamera_period = 1000;
    }
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00492830
// FUNCTION: CARMA2_SW 0x0048fa50
tU32 C2_HOOK_FASTCALL GetLastTickCount(void) {

    return gLast_tick_count;
}
#endif

// CheckTimer

// MungeRaceFinished

// FUNCTION: CARMA2_HW 0x004940e0
// FUNCTION: CARMA2_SW 0x004913e0
// FUNCTION: CARMA2_CHEAT 0x00494310
void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {

    gOur_pos = &gPlayer_car_master_actor->t.t.translate.t;
    PositionExternalCamera(&gProgram_state.current_car, pCamera_period);
    BrActorToActorMatrix34(&gCamera_to_world, gCamera, gUniverse_actor);
    BrActorToActorMatrix34(&gRearview_camera_to_world, gRearview_camera, gUniverse_actor);
    gCamera_to_horiz_angle = FastScalarArcTan2(gCamera_to_world.m[2][1], gCamera_to_world.m[1][1]);
    gYon_squared = gYon_multiplier * gYon_multiplier * ((br_camera*)gCamera->type_data)->yon_z * ((br_camera*)gCamera->type_data)->yon_z;
    if ((gCamera_to_world.m[3][2] - gPrev_camera_position.v[2]) * (gCamera_to_world.m[3][2] - gPrev_camera_position.v[2])
            + ((gCamera_to_world.m[3][0] - gPrev_camera_position.v[0]) * (gCamera_to_world.m[3][0] - gPrev_camera_position.v[0])
                + (gCamera_to_world.m[3][1] - gPrev_camera_position.v[1]) * (gCamera_to_world.m[3][1] - gPrev_camera_position.v[1]))
        > 400.0) {
        ResetPedNearness();
    }
    BrVector3Copy(&gPrev_camera_position, (br_vector3*)gCamera_to_world.m[3]);
}

// CheckActiveCarList

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00492980
// STUB: CARMA2_SW 0x0048fbf0
// STUB: CARMA2_CHEAT 0x00492920
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00492950
// FUNCTION: CARMA2_SW 0x0048fbc0
// FUNCTION: CARMA2_CHEAT 0x004928f0
tRace_result C2_HOOK_FASTCALL DoRace(void) {
    tRace_result result;

    if (PercentageChance(50)) {
        StartMusicTrack(9998);
    } else {
        StartMusicTrack(9997);
    }
    gRace_start = PDGetTotalTime();
    result = MainGameLoop();
    return result;
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "31-racesumm.h"

#include "01-network.h"
#include "18-graphics2.h"
#include "30-opponent.h"
#include "35-intrface.h"
#include "41-utility.h"
#include "45-cutscene.h"
#include "69-sound.h"
#include "72-interface.h"
#include "platform.h"

#include "globvars.h"
#include "globvrpb.h"

#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x007051a0
// GLOBAL: CARMA2_SW 0x006fbc60
// GLOBAL: CARMA2_CHEAT 0x0070c840
int gPlayer_lookup_racesumm[12]; /* FIXME: size is a guess (one per net player) */

// FUNCTION: CARMA2_HW 0x004e33a0
// FUNCTION: CARMA2_SW 0x004df720
// FUNCTION: CARMA2_CHEAT 0x004e8f20
static int C2_HOOK_CDECL SortScores(const void* pFirst_one, const void* pSecond_one) {

    return gNet_players[*(const int*)pSecond_one].score - gNet_players[*(const int*)pFirst_one].score;
}

void C2_HOOK_FASTCALL SortGameScores(void) {

    qsort(gPlayer_lookup_racesumm, gNumber_of_net_players, sizeof(int), SortScores);
}

void C2_HOOK_FASTCALL DoNetRaceSummary(void) {
    int i;
    tU32 start_time;

    NetPlayerStatusChanged(ePlayer_status_summary);
    start_time = PDGetTotalTime();
    while (!gReceived_game_scores && (int)(PDGetTotalTime() - start_time) < 20000) {
        NetService(0);
    }
    if (gReceived_game_scores) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            gPlayer_lookup_racesumm[i] = i;
        }
        SortGameScores();
        FRONTEND_Main(kFrontend_menu_networksummary);
        NetPlayerStatusChanged(ePlayer_status_loading);
        FadePaletteDown();
    }
}

// FUNCTION: CARMA2_HW 0x004e32b0
// FUNCTION: CARMA2_SW 0x004df630
// FUNCTION: CARMA2_CHEAT 0x004e8e30
void C2_HOOK_FASTCALL DoEndRaceSummary(tRace_result pRace_result, int pGame_completed) {

    ResetInterfaceTimeout__intrface();
    if (gNet_mode != eNet_mode_none) {
        DoNetRaceSummary();
    } else if (!pGame_completed && AnyBuyableCars()) {
        StartMusicTrack(9998);
        FRONTEND_Main(kFrontend_menu_wrecks);
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

inline void C2_HOOK_FASTCALL PickRaceWhenGameCompleted(void) {

    gProgram_state.current_race_index = IRandomBetween(0, gNumber_of_races - 1);
    gCurrent_race_group = gRace_list[gProgram_state.current_race_index].group;
}

// FUNCTION: CARMA2_HW 0x004e3440
// FUNCTION: CARMA2_SW 0x004df7d0
// FUNCTION: CARMA2_CHEAT 0x004e8fc0
int C2_HOOK_FASTCALL MungeRaceProgression(tRace_result pRace_result) {
    int i;
    int least_raced;
    int least_races;
    int game_completed;

    game_completed = 0;
    if (pRace_result == eRace_completed) {
        gCurrent_race.race_spec->count_opponents++;
        if (gProgram_state.game_completed) {
            PickRaceWhenGameCompleted();
        } else if (gCurrent_race.race_spec->is_boundary) {
            gIs_boundary_race = 0;
            gCurrent_race_group++;
            if (gCurrent_race_group - gRace_groups >= gCountRaceGroups) {
                gProgram_state.game_completed = 1;
                DoGameCompletedAnimation();
                FRONTEND_Main(kFrontend_menu_credits);
                PickRaceWhenGameCompleted();
                game_completed = 1;
            } else {
                gProgram_state.current_race_index = gCurrent_race_group->races - gRace_list;
            }
        } else {
            least_races = 10000;
            for (i = 0; i < gCurrent_race_group->count_races; i++) {
                if (gCurrent_race_group->races[i].count_opponents < least_races) {
                    least_races = gCurrent_race_group->races[i].count_opponents;
                    least_raced = i;
                }
            }
            if (least_races != 0) {
                gIs_boundary_race = 1;
                gProgram_state.current_race_index = gCurrent_race_group->mission - gRace_list;
            } else {
                gProgram_state.current_race_index = &gCurrent_race_group->races[least_raced] - gRace_list;
            }
        }
    } else if (!gNo_credits_APO_restore) {
        gProgram_state.credits = gMoney_pre_race;
        gProgram_state.current_car.power_up_levels[0] = gAPO_pre_race[0];
        gProgram_state.current_car.power_up_levels[1] = gAPO_pre_race[1];
        gProgram_state.current_car.power_up_levels[2] = gAPO_pre_race[2];
    }
    return game_completed;
}


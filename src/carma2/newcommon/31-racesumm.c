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

// PickRaceWhenGameCompleted

// STUB: CARMA2_HW 0x004e3440
// STUB: CARMA2_SW 0x004df7d0
// STUB: CARMA2_CHEAT 0x004e8fc0
int C2_HOOK_FASTCALL MungeRaceProgression(tRace_result pRace_result) {
    NOT_IMPLEMENTED();
}


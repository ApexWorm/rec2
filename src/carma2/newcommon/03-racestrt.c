/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "03-racestrt.h"

#include "rec2_types.h"

#include "04-netgame.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "42-input.h"
#include "69-sound.h"
#include "72-interface.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

// GLOBAL: CARMA2_HW 0x006a0c74
// GLOBAL: CARMA2_SW 0x0069accc
// GLOBAL: CARMA2_CHEAT 0x006a87ec
int gOur_starting_position;

// GLOBAL: CARMA2_HW 0x006a0d38
// GLOBAL: CARMA2_SW 0x0069ad90
// GLOBAL: CARMA2_CHEAT 0x006a88b0
tU32 gLast_host_query;

// GLOBAL: CARMA2_HW 0x006a0d00
// GLOBAL: CARMA2_SW 0x0069ad58
// GLOBAL: CARMA2_CHEAT 0x006a8878
int gChange_race_net_mode;

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004e21c0
// FUNCTION: CARMA2_SW 0x004de540
// FUNCTION: CARMA2_CHEAT 0x004e7d50
void C2_HOOK_FASTCALL FUN_004e21c0(void) {
    /* Unused */
}

// FUNCTION: CARMA2_HW 0x004e21d0
// FUNCTION: CARMA2_SW 0x004de550
// FUNCTION: CARMA2_CHEAT 0x004e7d60
void C2_HOOK_FASTCALL FUN_004e21d0(void) {
    /* Unused */
}
#endif

// FUNCTION: CARMA2_HW 0x004e21e0
// FUNCTION: CARMA2_SW 0x004de560
// FUNCTION: CARMA2_CHEAT 0x004e7d70
int C2_HOOK_FASTCALL UpRace(int* pCurrent_choice, int* pCurrent_mode) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004e21f0
// FUNCTION: CARMA2_SW 0x004de570
// FUNCTION: CARMA2_CHEAT 0x004e7d80
int C2_HOOK_FASTCALL DownRace(int* pCurrent_choice, int* pCurrent_mode) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004e2200
// FUNCTION: CARMA2_SW 0x004de580
// FUNCTION: CARMA2_CHEAT 0x004e7d90
int C2_HOOK_FASTCALL ClickOnRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {
    int x_coord;
    int y_coord;
    int race_delta;

    GetMousePosition(&x_coord, &y_coord);
    race_delta = (y_coord - (gCurrent_graf_data->field_0x164 - 5 * (gCurrent_graf_data->field_0x160 / 2) + gBig_font->glyph_y / 2)) / gCurrent_graf_data->field_0x160 - 2;
    if (race_delta > -3 && race_delta < 0) {
        do {
            UpRace(pCurrent_choice, pCurrent_mode);
        } while (++race_delta);
    } else if (race_delta < 3 && race_delta > 0) {
        do {
            DownRace(pCurrent_choice, pCurrent_mode);
        } while (--race_delta);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e2260
// FUNCTION: CARMA2_SW 0x004de5e0
// FUNCTION: CARMA2_CHEAT 0x004e7df0
int C2_HOOK_FASTCALL UpClickRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {

    UpRace(pCurrent_choice, pCurrent_mode);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e2270
// FUNCTION: CARMA2_SW 0x004de5f0
// FUNCTION: CARMA2_CHEAT 0x004e7e00
int C2_HOOK_FASTCALL DownClickRace(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {

    DownRace(pCurrent_choice, pCurrent_mode);
    return 0;
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004e2280
// FUNCTION: CARMA2_SW 0x004de600
// FUNCTION: CARMA2_CHEAT 0x004e7e10
void C2_HOOK_FASTCALL FUN_004e2280(void) {
    tS32 start_time;
    tS32 the_time;

    /* Unused */
    start_time = PDGetTotalTime();
    for (the_time = PDGetTotalTime(); the_time < start_time + 400; the_time = PDGetTotalTime()) {
    }
}
#endif

// FUNCTION: CARMA2_HW 0x004e2890
// FUNCTION: CARMA2_SW 0x004dec10
// FUNCTION: CARMA2_CHEAT 0x004e8420
int C2_HOOK_FASTCALL ChangeCarGoAhead(int* pCurrent_choice, int* pCurrent_mode) {

    if (gChange_race_net_mode != 0 && gCar_details[gProgram_state.cars_available[gFrontend_change_car_selected_car]].ownership == eCar_owner_someone) {
        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x004e2b50
// FUNCTION: CARMA2_SW 0x004deed0
// FUNCTION: CARMA2_CHEAT 0x004e86e0
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

// FUNCTION: CARMA2_HW 0x004e2b70
// FUNCTION: CARMA2_SW 0x004deef0
// FUNCTION: CARMA2_CHEAT 0x004e8700
int C2_HOOK_FASTCALL SortOpponents(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        if (gCurrent_race.opponent_list[i].index < 0) {
            return 3;
        }
    }

    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);
    qsort(gCurrent_race.opponent_list, gCurrent_race.number_of_racers, sizeof(tOpp_spec), SortGridFunction);
    gOur_starting_position = IRandomBetween(0, gCurrent_race.number_of_racers - 1);

    for (i = gCurrent_race.number_of_racers; i > gOur_starting_position; i--) {
        gCurrent_race.opponent_list[i] = gCurrent_race.opponent_list[i - 1];
    }
    gCurrent_race.opponent_list[gOur_starting_position].index = -1;
    gCurrent_race.opponent_list[gOur_starting_position].ranking = 0;
    gCurrent_race.opponent_list[gOur_starting_position].car_spec = &gProgram_state.current_car;
    gCurrent_race.number_of_racers++;
    return 3;
}

// DoGridPosition

// FUNCTION: CARMA2_HW 0x004e2ec0
// FUNCTION: CARMA2_SW 0x004df240
// FUNCTION: CARMA2_CHEAT 0x004e8a50
int C2_HOOK_FASTCALL NetSynchStartDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {

    if (pEscaped) {
        return -1;
    }
    if (pCurrent_choice == 0) {
        gProgram_state.prog_status = eProg_idling;
    }
    return pCurrent_choice;
}

// FUNCTION: CARMA2_HW 0x004e2ef0
// FUNCTION: CARMA2_SW 0x004df270
// FUNCTION: CARMA2_CHEAT 0x004e8a80
int C2_HOOK_FASTCALL NetSynchStartGoAhead(int* pCurrent_choice, int* pCurrent_mode) {

    if (*pCurrent_choice != 0 && (gNet_mode != eNet_mode_host || *pCurrent_choice < 0)) {
        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
        return 0;
    }
    if (*pCurrent_choice == 0) {
        gProgram_state.prog_status = eProg_idling;
        return 1;
    }
    if (gNet_mode == eNet_mode_host && *pCurrent_choice == 1) {
#ifdef REC2_CHEAT
        SignalToStartRace();
        gStart_race_sent = 1;
        gNo_races_yet = 0;
#else
        if (gNumber_of_net_players > 1) {
            SignalToStartRace();
            gStart_race_sent = 1;
            gNo_races_yet = 0;
            return 1;
        }
        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
        return 0;
#endif
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004e2f70
// FUNCTION: CARMA2_SW 0x004df2f0
// FUNCTION: CARMA2_CHEAT 0x004e8af0
int C2_HOOK_FASTCALL ExitWhenReady(int* pCurrent_choice, int* pCurrent_mode) {

    if (gStart_race_sent || gProgram_state.prog_status == eProg_game_starting) {
        *pCurrent_choice = 1;
        return 1;
    }
    if (gProgram_state.prog_status == eProg_idling) {
        *pCurrent_choice = 0;
        return 1;
    }
    return 0;
}

// ReceivedViewCar


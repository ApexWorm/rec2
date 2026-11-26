/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "44-mainmenu.h"

#include "01-network.h"
#include "07-structur.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "35-intrface.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "69-sound.h"
#include "72-interface.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

// GLOBAL: CARMA2_HW 0x0068c850
// GLOBAL: CARMA2_SW 0x006868a4
// GLOBAL: CARMA2_CHEAT 0x006940f0
int gMouse_was_started;

// GLOBAL: CARMA2_HW 0x0068c854
// GLOBAL: CARMA2_SW 0x006868a8
// GLOBAL: CARMA2_CHEAT 0x006940f4
int gReplace_background;

// FUNCTION: CARMA2_HW 0x004943f0
// FUNCTION: CARMA2_SW 0x004916f0
// FUNCTION: CARMA2_CHEAT 0x00494620
void C2_HOOK_FASTCALL QuitVerifyStart(void) {

    FadePaletteDown();
}

// FUNCTION: CARMA2_HW 0x00494400
// FUNCTION: CARMA2_SW 0x00491700
// FUNCTION: CARMA2_CHEAT 0x00494630
int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {

    FadePaletteDown();
    TurnOnPanelFlics();
    if (gMouse_was_started) {
        RemoveTransientBitmaps(1);
    }
    if (gReplace_background) {
        if (!pCurrent_choice) {
            ClearEntireScreen();
        }
        PDScreenBufferSwap(0);
        FadePaletteUp();
    }
    return pCurrent_choice == 0;
}

// GLOBAL: CARMA2_HW 0x00659198
// GLOBAL: CARMA2_SW 0x00653b80
// GLOBAL: CARMA2_CHEAT 0x0065fab0
static tFlicette gQuit_verify_flicker_on[2] = {
    { 43, { 181, 362 }, { 124, 298 } },
    { 43, { 84, 168 }, { 124, 298 } },
};

// GLOBAL: CARMA2_HW 0x006591c0
// GLOBAL: CARMA2_SW 0x00653ba8
// GLOBAL: CARMA2_CHEAT 0x0065fad8
static tFlicette gQuit_verify_flicker_off[2] = {
    { 42, { 181, 362 }, { 124, 298 } },
    { 42, { 84, 168 }, { 124, 298 } },
};

// GLOBAL: CARMA2_HW 0x006591e8
// GLOBAL: CARMA2_SW 0x00653bd0
// GLOBAL: CARMA2_CHEAT 0x0065fb00
static tFlicette gQuit_verify_push[2] = {
    { 135, { 181, 362 }, { 124, 298 } },
    { 45, { 84, 168 }, { 124, 298 } },
};

// GLOBAL: CARMA2_HW 0x00659210
// GLOBAL: CARMA2_SW 0x00653bf8
// GLOBAL: CARMA2_CHEAT 0x0065fb28
static tMouse_area gQuit_verify_mouse_areas[2] = {
    { { 181, 362 }, { 124, 298 }, { 244, 488 }, { 144, 346 }, 0, 0, 0, NULL },
    { { 84, 168 }, { 124, 298 }, { 147, 294 }, { 144, 346 }, 1, 0, 0, NULL },
};

// GLOBAL: CARMA2_HW 0x00659270
// GLOBAL: CARMA2_SW 0x00653c58
// GLOBAL: CARMA2_CHEAT 0x0065fb88
static tInterface_spec gQuit_verify_interface_spec = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    { -1, 0 },
    { -1, 0 },
    { 0, 0 },
    { 1, 0 },
    { NULL, NULL },
    { -1, 0 },
    { 1, 0 },
    { 0, 0 },
    { 1, 0 },
    { NULL, NULL },
    { -1, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { NULL, NULL },
    { -1, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { NULL, NULL },
    { 1, 1 },
    { NULL, NULL },
    { 1, 1 },
    { NULL, NULL },
    NULL,
    NULL,
    0,
    QuitVerifyStart,
    NULL,
    QuitVerifyDone,
    0,
    { 0, 0 },
    NULL,
    1,
    1,
    2,
    gQuit_verify_flicker_on,
    gQuit_verify_flicker_off,
    gQuit_verify_push,
    2,
    gQuit_verify_mouse_areas,
    0,
    NULL,
};

// FUNCTION: CARMA2_HW 0x00494450
// FUNCTION: CARMA2_SW 0x00491750
// FUNCTION: CARMA2_CHEAT 0x00494680
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {
    int result;
    int woz_in_race;

    woz_in_race = 0;
    if (gAusterity_mode) {
        return 1;
    }
    if (gProgram_state.racing) {
        FadePaletteDown();
        ClearEntireScreen();
        GoingToInterfaceFromRace();
        woz_in_race = 1;
        gProgram_state.racing = 0;
    }
    switch (IRandomBetween(1, 3)) {
    case 1:
        gQuit_verify_interface_spec.first_opening_flic = 130;
        break;
    case 2:
        gQuit_verify_interface_spec.first_opening_flic = 131;
        break;
    case 3:
        gQuit_verify_interface_spec.first_opening_flic = 132;
        break;
    }
    gMouse_was_started = gMouse_started;
    if (gMouse_was_started) {
        RemoveTransientBitmaps(1);
        EndMouseCursor();
    }
    TurnOffPanelFlics();
    gReplace_background = pReplace_background;
    DRS3StopOutletSound(gEffects_outlet);
    LoadInterfaceStuff(woz_in_race);
    result = DoInterfaceScreen(&gQuit_verify_interface_spec, 0, 0);
    if (gMouse_was_started) {
        LoadInterfaceStuff(woz_in_race);
        StartMouseCursor();
    }
    if (woz_in_race) {
        GoingBackToRaceFromInterface();
        gProgram_state.racing = 1;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00494540
// FUNCTION: CARMA2_SW 0x00491840
// FUNCTION: CARMA2_CHEAT 0x00494770
void C2_HOOK_FASTCALL DoMainScreen(void) {

    StartMusicTrack(9998);
    switch (FRONTEND_Main(kFrontend_menu_main)) {
    case 0:
        gProgram_state.prog_status = eProg_quit;
        break;
    case 1:
        gProgram_state.prog_status = eProg_game_starting;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00494570
// FUNCTION: CARMA2_SW 0x00491870
// FUNCTION: CARMA2_CHEAT 0x004947a0
void C2_HOOK_FASTCALL DoOptionsMenu(void) {
    tPlayer_status player_status;

    if (gNet_mode != eNet_mode_none) {
        player_status = gNet_players[gThis_net_player_index].player_status;
        NetPlayerStatusChanged(ePlayer_status_main_menu);
    }
    if (FRONTEND_Main(kFrontend_menu_options) == 0) {
        gProgram_state.prog_status = eProg_quit;
    }
    if (gNet_mode != eNet_mode_none && gNet_players[gThis_net_player_index].player_status == ePlayer_status_main_menu) {
        NetPlayerStatusChanged(player_status);
    }
}


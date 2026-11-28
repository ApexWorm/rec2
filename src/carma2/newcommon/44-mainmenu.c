/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "44-mainmenu.h"

#include "01-network.h"
#include "18-graphics2.h"
#include "25-grafdata.h"
#include "35-intrface.h"
#include "38-flicplay.h"
#include "69-sound.h"
#include "72-interface.h"
#include "globvars.h"
#include "globvrpb.h"

// GLOBAL: CARMA2_HW 0x0068c848
// GLOBAL: CARMA2_SW 0x0068689c
// GLOBAL: CARMA2_CHEAT 0x006940e8
tInterface_spec* gMain_menu_spec;

// FUNCTION: CARMA2_HW 0x00494210
// FUNCTION: CARMA2_SW 0x00491510
// FUNCTION: CARMA2_CHEAT 0x00494440
int C2_HOOK_FASTCALL MainMenuDone1(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {

    if (pTimed_out) {
        return -1;
    } else {
        switch (pCurrent_choice) {
        case 1:
            PreloadBunchOfFlics(4);
            break;
        case 2:
            PreloadBunchOfFlics(5);
            break;
        case 3:
            PreloadBunchOfFlics(1);
            break;
        case 4:
            PreloadBunchOfFlics(3);
            break;
        case 5:
            PreloadBunchOfFlics(2);
            break;
        case 7:
            PreloadBunchOfFlics(7);
            break;
        default:
            break;
        }
        return pCurrent_choice;
    }
}

// FUNCTION: CARMA2_HW 0x004942b0
// FUNCTION: CARMA2_SW 0x004915b0
// FUNCTION: CARMA2_CHEAT 0x004944e0
int C2_HOOK_FASTCALL MainMenuDone2(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {

    if (pTimed_out) {
        return -1;
    } else {
        switch (pCurrent_choice) {
        case 0:
            PreloadBunchOfFlics(4);
            break;
        case 1:
            PreloadBunchOfFlics(5);
            break;
        case 2:
            PreloadBunchOfFlics(1);
            break;
        case 3:
            PreloadBunchOfFlics(3);
            break;
        case 4:
            PreloadBunchOfFlics(7);
            break;
        default:
            break;
        }
        return pCurrent_choice;
    }
}

// FUNCTION: CARMA2_HW 0x00494340
// FUNCTION: CARMA2_SW 0x00491640
// FUNCTION: CARMA2_CHEAT 0x00494570
void C2_HOOK_FASTCALL StartMainMenu(void) {

    if (gFaded_palette) {
        PlayFlicsInDarkness();
        RunFlic(10);
    } else {
        RunFlic(11);
    }
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    if (!gProgram_state.racing) {
        DisableChoice(1);
        RunFlicAt(35,
            gMain_menu_spec->flicker_on_flics[1].x[gReal_graf_data_index],
            gMain_menu_spec->flicker_on_flics[1].y[gReal_graf_data_index]);
    }
    if (gDisallow_abandon_race) {
        DisableChoice(2);
        RunFlicAt(36,
            gMain_menu_spec->flicker_on_flics[2].x[gReal_graf_data_index],
            gMain_menu_spec->flicker_on_flics[2].y[gReal_graf_data_index]);
    }
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
    ReilluminateFlics();
}

// QuitVerifyStart

// QuitVerifyDone

// STUB: CARMA2_HW 0x00494450
// STUB: CARMA2_CHEAT 0x00494680
int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background) {
    NOT_IMPLEMENTED();
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


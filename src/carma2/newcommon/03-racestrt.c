/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "03-racestrt.h"

#include "rec2_macros.h"
#include "rec2_types.h"

#include "01-network.h"
#include "04-netgame.h"
#include "07-structur.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "25-grafdata.h"
#include "35-intrface.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "42-input.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "72-interface.h"

#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x006a0c74
// GLOBAL: CARMA2_SW 0x0069accc
// GLOBAL: CARMA2_CHEAT 0x006a87ec
int gOur_starting_position;

// GLOBAL: CARMA2_HW 0x006a0d38
// GLOBAL: CARMA2_SW 0x0069ad90
// GLOBAL: CARMA2_CHEAT 0x006a88b0
tU32 gLast_host_query;

// GLOBAL: CARMA2_HW 0x006a0c58
// GLOBAL: CARMA2_SW 0x0069acb0
// GLOBAL: CARMA2_CHEAT 0x006a87d0
tU32 gNet_synch_start_racestrt;

// GLOBAL: CARMA2_HW 0x006a0c5c
// GLOBAL: CARMA2_SW 0x0069acb4
// GLOBAL: CARMA2_CHEAT 0x006a87d4
tNet_game_details* gChoose_car_net_game;

// GLOBAL: CARMA2_HW 0x006a0c60
// GLOBAL: CARMA2_SW 0x0069acb8
// GLOBAL: CARMA2_CHEAT 0x006a87d8
tInterface_spec* gStart_interface_spec;

// GLOBAL: CARMA2_HW 0x006a0cfc
// GLOBAL: CARMA2_SW 0x0069ad54
// GLOBAL: CARMA2_CHEAT 0x006a8874
tNet_sequence_type gNet_race_sequence_racestrt;

// GLOBAL: CARMA2_HW 0x006a0d00
// GLOBAL: CARMA2_SW 0x0069ad58
// GLOBAL: CARMA2_CHEAT 0x006a8878
int gChange_race_net_mode;

// GLOBAL: CARMA2_HW 0x006a0d0c
// GLOBAL: CARMA2_SW 0x0069ad64
// GLOBAL: CARMA2_CHEAT 0x006a8884
br_pixelmap* gBullet_image;

// GLOBAL: CARMA2_HW 0x006a0d2c
// GLOBAL: CARMA2_SW 0x0069ad84
// GLOBAL: CARMA2_CHEAT 0x006a88a4
int gCurrent_race_index;

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

// FUNCTION: CARMA2_HW 0x004e2280
// FUNCTION: CARMA2_SW 0x004de600
// FUNCTION: CARMA2_CHEAT 0x004e7e10
void C2_HOOK_FASTCALL StartChangeRace(void) {
    tS32 start_time;
    tS32 the_time;

    start_time = PDGetTotalTime();
    for (the_time = PDGetTotalTime(); the_time < start_time + 400; the_time = PDGetTotalTime()) {
    }
}

// FUNCTION: CARMA2_HW 0x004e22b0
// FUNCTION: CARMA2_SW 0x004de630
// FUNCTION: CARMA2_CHEAT 0x004e7e40
int C2_HOOK_FASTCALL ChangeRace(int* pRace_index, int pNet_mode, tNet_sequence_type pNet_race_sequence) {
    // GLOBAL: CARMA2_HW 0x0065efc8
    // GLOBAL: CARMA2_SW 0x00659928
    // GLOBAL: CARMA2_CHEAT 0x00666778
    static tFlicette flicker_on[4] = {
        { 43, { 60, 120 }, { 154, 370 } },
        { 43, { 221, 442 }, { 154, 370 } },
        { 221, { 30, 60 }, { 78, 187 } },
        { 221, { 30, 60 }, { 78, 187 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f018
    // GLOBAL: CARMA2_SW 0x00659978
    // GLOBAL: CARMA2_CHEAT 0x006667c8
    static tFlicette flicker_off[4] = {
        { 42, { 60, 120 }, { 154, 370 } },
        { 42, { 221, 442 }, { 154, 370 } },
        { 220, { 30, 60 }, { 78, 187 } },
        { 220, { 30, 60 }, { 78, 187 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f068
    // GLOBAL: CARMA2_SW 0x006599c8
    // GLOBAL: CARMA2_CHEAT 0x00666818
    static tFlicette push[4] = {
        { 154, { 60, 120 }, { 154, 370 } },
        { 45, { 221, 442 }, { 154, 370 } },
        { 222, { 30, 60 }, { 78, 187 } },
        { 225, { 30, 60 }, { 118, 283 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f0b8
    // GLOBAL: CARMA2_SW 0x00659a18
    // GLOBAL: CARMA2_CHEAT 0x00666868
    static tMouse_area mouse_areas[5] = {
        { { 60, 120 }, { 154, 370 }, { 125, 250 }, { 174, 418 }, 0, 0, 0, NULL },
        { { 221, 442 }, { 154, 370 }, { 286, 572 }, { 174, 418 }, 1, 0, 0, NULL },
        { { 30, 60 }, { 78, 187 }, { 45, 90 }, { 104, 250 }, -1, 0, 0, UpClickRace },
        { { 30, 60 }, { 118, 283 }, { 45, 90 }, { 145, 348 }, -1, 0, 0, DownClickRace },
        { { 66, 132 }, { 33, 79 }, { 278, 556 }, { 144, 346 }, -1, 0, 0, ClickOnRace },
    };
    // GLOBAL: CARMA2_HW 0x0065f1a8
    // GLOBAL: CARMA2_SW 0x00659b08
    // GLOBAL: CARMA2_CHEAT 0x00666958
    static tInterface_spec interface_spec = {
        0, 230, 60, 231, 231, 231, 6,
        { -1, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { UpRace, NULL },
        { -1, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { DownRace, NULL },
        { 1, 1 }, { NULL, NULL }, { 1, 1 }, { NULL, NULL },
        NULL, NULL, 0, NULL, StartChangeRace, NULL, 0, { 0, 0 },
        NULL, 1, 1, REC2_ASIZE(flicker_on), flicker_on, flicker_off, push,
        REC2_ASIZE(mouse_areas), mouse_areas, 0, NULL
    };
    int result;

    gNet_race_sequence_racestrt = pNet_race_sequence;
    gChange_race_net_mode = pNet_mode;
    gStart_interface_spec = &interface_spec;
    gCurrent_race_index = *pRace_index;
    if (pNet_mode) {
        gBullet_image = NULL;
    } else {
        gBullet_image = LoadPixelmap("BULLET.PIX");
    }
    result = DoInterfaceScreen(&interface_spec, pNet_mode != 0, 0);
    if (result == 0) {
        *pRace_index = gCurrent_race_index;
    }
    if (gBullet_image != NULL) {
        BrPixelmapFree(gBullet_image);
    }
    return result == 0;
}

// FUNCTION: CARMA2_HW 0x004e2340
// FUNCTION: CARMA2_SW 0x004de6c0
// FUNCTION: CARMA2_CHEAT 0x004e7ed0
void C2_HOOK_FASTCALL DrawCar(int pCurrent_choice, int pCurrent_mode) {
    char s[64];
    int text_x;
    int text_width;

    PollCarDetails(gChoose_car_net_game);
    if (gChange_race_net_mode) {
        if (gCar_details[gProgram_state.cars_available[gFrontend_change_car_selected_car]].ownership == eCar_owner_someone) {
            sprintf(s, "%s %s", GetMiscString(eMiscString_this_car_already_taken_by), gCar_details[gProgram_state.cars_available[gFrontend_change_car_selected_car]].name);
        } else {
            sprintf(s, GetMiscString(eMiscString_available));
        }
    } else {
        if (gProgram_state.number_of_cars == 1) {
            sprintf(s, GetMiscString(eMiscString_no_other_cars_to_choose_from_yet));
        } else if (gProgram_state.cars_available[gFrontend_change_car_selected_car] != 0) {
            sprintf(s, "%s %s", GetMiscString(eMiscString_original_driver_was), gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].name);
        } else {
            sprintf(s, GetMiscString(eMiscString_your_original_car));
        }
    }
    text_width = BrPixelmapTextWidth(gBack_screen, gFont_7, s);
    text_x = (gCurrent_graf_data->field_0x2c4 + gCurrent_graf_data->field_0x2cc - text_width) / 2;
    BrPixelmapRectangleFill(gBack_screen,
        gCurrent_graf_data->field_0x2c4,
        gCurrent_graf_data->field_0x2c0,
        gCurrent_graf_data->field_0x2cc - gCurrent_graf_data->field_0x2c4,
        gFont_7->glyph_y,
        0);
    BrPixelmapText(gBack_screen,
        text_x,
        gCurrent_graf_data->field_0x2c0,
        3,
        gFont_7,
        s);
    BrPixelmapLine(gBack_screen,
        gCurrent_graf_data->field_0x2c4,
        gCurrent_graf_data->field_0x2c8,
        gCurrent_graf_data->field_0x2cc,
        gCurrent_graf_data->field_0x2c8,
        6);
}

inline void C2_HOOK_FASTCALL SetCarFlic(void) {

    ChangePanelFlic(0,
        gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].stolen_car_image_data,
        gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].stolen_car_image_data_length);
}

// FUNCTION: CARMA2_HW 0x004e24d0
// FUNCTION: CARMA2_SW 0x004de850
// FUNCTION: CARMA2_CHEAT 0x004e8060
int C2_HOOK_FASTCALL UpCar(int* pCurrent_choice, int* pCurrent_mode) {

    AddToFlicQueue(gStart_interface_spec->pushed_flics[2].flic_index,
        gStart_interface_spec->pushed_flics[2].x[gReal_graf_data_index],
        gStart_interface_spec->pushed_flics[2].y[gReal_graf_data_index],
        1);
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    if (gFrontend_change_car_selected_car != 0) {
        RemoveTransientBitmaps(1);
        DropOutImageThruBottom(GetPanelPixelmap(0),
            gCurrent_graf_data->field_0x2d0,
            gCurrent_graf_data->field_0x2d4,
            gCurrent_graf_data->field_0x2e0,
            gCurrent_graf_data->field_0x2e4);
        gFrontend_change_car_selected_car--;
        SetCarFlic();
        DropInImageFromTop(GetPanelPixelmap(0),
            gCurrent_graf_data->field_0x2d0,
            gCurrent_graf_data->field_0x2d4,
            gCurrent_graf_data->field_0x2e0,
            gCurrent_graf_data->field_0x2e4);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e25c0
// FUNCTION: CARMA2_SW 0x004de940
// FUNCTION: CARMA2_CHEAT 0x004e8150
int C2_HOOK_FASTCALL DownCar(int* pCurrent_choice, int* pCurrent_mode) {

    AddToFlicQueue(gStart_interface_spec->pushed_flics[3].flic_index,
        gStart_interface_spec->pushed_flics[3].x[gReal_graf_data_index],
        gStart_interface_spec->pushed_flics[3].y[gReal_graf_data_index],
        1);
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    if (gFrontend_change_car_selected_car < gProgram_state.number_of_cars - 1) {
        RemoveTransientBitmaps(1);
        DropOutImageThruTop(GetPanelPixelmap(0),
            gCurrent_graf_data->field_0x2d0,
            gCurrent_graf_data->field_0x2d4,
            gCurrent_graf_data->field_0x2e0,
            gCurrent_graf_data->field_0x2e4);
        gFrontend_change_car_selected_car++;
        SetCarFlic();
        DropInImageFromBottom(GetPanelPixelmap(0),
            gCurrent_graf_data->field_0x2d0,
            gCurrent_graf_data->field_0x2d4,
            gCurrent_graf_data->field_0x2e0,
            gCurrent_graf_data->field_0x2e4);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e26b0
// FUNCTION: CARMA2_SW 0x004dea30
// FUNCTION: CARMA2_CHEAT 0x004e8240
int C2_HOOK_FASTCALL UpClickCar(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {

    UpCar(pCurrent_choice, pCurrent_mode);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e27a0
// FUNCTION: CARMA2_SW 0x004deb20
// FUNCTION: CARMA2_CHEAT 0x004e8330
int C2_HOOK_FASTCALL DownClickCar(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {

    DownCar(pCurrent_choice, pCurrent_mode);
    return 0;
}

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

// FUNCTION: CARMA2_HW 0x004e28d0
// FUNCTION: CARMA2_SW 0x004dec50
// FUNCTION: CARMA2_CHEAT 0x004e8460
int C2_HOOK_FASTCALL ChangeCar(int pNet_mode, int* pCar_index, tNet_game_details* pNet_game) {
    // GLOBAL: CARMA2_HW 0x0065f2d8
    // GLOBAL: CARMA2_SW 0x00659c38
    // GLOBAL: CARMA2_CHEAT 0x00666a88
    static tFlicette flicker_on[4] = {
        { 43, { 60, 120 }, { 154, 370 } },
        { 43, { 221, 442 }, { 154, 370 } },
        { 221, { 30, 60 }, { 78, 187 } },
        { 221, { 30, 60 }, { 78, 187 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f328
    // GLOBAL: CARMA2_SW 0x00659c88
    // GLOBAL: CARMA2_CHEAT 0x00666ad8
    static tFlicette flicker_off[4] = {
        { 42, { 60, 120 }, { 154, 370 } },
        { 42, { 221, 442 }, { 154, 370 } },
        { 220, { 30, 60 }, { 78, 187 } },
        { 220, { 30, 60 }, { 78, 187 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f378
    // GLOBAL: CARMA2_SW 0x00659cd8
    // GLOBAL: CARMA2_CHEAT 0x00666b28
    static tFlicette push[4] = {
        { 154, { 60, 120 }, { 154, 370 } },
        { 45, { 221, 442 }, { 154, 370 } },
        { 222, { 30, 60 }, { 78, 187 } },
        { 225, { 30, 60 }, { 118, 283 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f3c8
    // GLOBAL: CARMA2_SW 0x00659d28
    // GLOBAL: CARMA2_CHEAT 0x00666b78
    static tMouse_area mouse_areas[4] = {
        { { 60, 120 }, { 154, 370 }, { 125, 250 }, { 174, 418 }, 0, 0, 0, NULL },
        { { 221, 442 }, { 154, 370 }, { 286, 572 }, { 174, 418 }, 1, 0, 0, NULL },
        { { 30, 60 }, { 78, 187 }, { 45, 90 }, { 104, 250 }, -1, 0, 0, UpClickCar },
        { { 30, 60 }, { 118, 283 }, { 45, 90 }, { 145, 348 }, -1, 0, 0, DownClickCar },
    };
    // GLOBAL: CARMA2_HW 0x0065f488
    // GLOBAL: CARMA2_SW 0x00659de8
    // GLOBAL: CARMA2_CHEAT 0x00666c38
    static tInterface_spec interface_spec = {
        0, 236, 62, 0, 0, 0, -1,
        { -1, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { UpCar, NULL },
        { -1, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { DownCar, NULL },
        { 1, 1 }, { ChangeCarGoAhead, NULL }, { 1, 1 }, { NULL, NULL },
        NULL, DrawCar, 0, NULL, NULL, NULL, 0, { 0, 0 },
        NULL, 1, 1, REC2_ASIZE(flicker_on), flicker_on, flicker_off, push,
        REC2_ASIZE(mouse_areas), mouse_areas, 0, NULL
    };
    int i;
    int result;
    int power_up_levels[3];

    gChoose_car_net_game = pNet_game;
    gChange_race_net_mode = pNet_mode;
    gStart_interface_spec = &interface_spec;
    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gProgram_state.cars_available[i] == *pCar_index) {
            gFrontend_change_car_selected_car = i;
            break;
        }
    }
    if (gProgram_state.game_completed) {
        gProgram_state.number_of_cars = gNumber_of_racers;
        for (i = 0; i < gProgram_state.number_of_cars; i++) {
            gProgram_state.cars_available[i] = i;
        }
    }
    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gOpponents[gProgram_state.cars_available[i]].stolen_car_image_data == NULL) {
            LoadFlicData(gOpponents[gProgram_state.cars_available[i]].stolen_car_flic_name,
                &gOpponents[gProgram_state.cars_available[i]].stolen_car_image_data,
                &gOpponents[gProgram_state.cars_available[i]].stolen_car_image_data_length);
        } else {
            MAMSLock((void**)&gOpponents[gProgram_state.cars_available[i]].stolen_car_image_data);
        }
    }
    InitialiseFlicPanel(0,
        gCurrent_graf_data->field_0x2d0,
        gCurrent_graf_data->field_0x2d4,
        gCurrent_graf_data->field_0x2d8 - gCurrent_graf_data->field_0x2d0,
        gCurrent_graf_data->field_0x2dc - gCurrent_graf_data->field_0x2d4);
    SetCarFlic();
    result = DoInterfaceScreen(&interface_spec, pNet_mode, 0);
    if (pNet_mode) {
        FadePaletteDown();
    } else {
        RunFlic(237);
    }
    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        MAMSUnlock((void**)&gOpponents[gProgram_state.cars_available[i]].stolen_car_image_data);
    }
    DisposeFlicPanel(0);
    if (result == 0) {
        if (pNet_mode == eNet_mode_none) {
            AboutToLoadFirstCar();
            for (i = 0; i < REC2_ASIZE(power_up_levels); i++) {
                power_up_levels[i] = gProgram_state.current_car.power_up_levels[i];
            }
            LoadCar(gOpponents[gProgram_state.cars_available[gFrontend_change_car_selected_car]].car_file_name,
                eDriver_local_human,
                &gProgram_state.current_car,
                gProgram_state.cars_available[gFrontend_change_car_selected_car],
                gProgram_state.player_name,
                &gOur_car_storage_space);
            for (i = 0; i < REC2_ASIZE(power_up_levels); i++) {
                gProgram_state.current_car.power_up_levels[i] = power_up_levels[i];
            }
            SetCarStorageTexturingLevel(&gOur_car_storage_space, GetCarTexturingLevel(), eCTL_full);
            DisposeRaceInfo(&gCurrent_race);
            SelectOpponents(&gCurrent_race);
            LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
        } else {
            *pCar_index = gProgram_state.cars_available[gFrontend_change_car_selected_car];
        }
        return 1;
    } else {
        return 0;
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

// FUNCTION: CARMA2_HW 0x004e2c30
// FUNCTION: CARMA2_SW 0x004defb0
// FUNCTION: CARMA2_CHEAT 0x004e87c0
void C2_HOOK_FASTCALL CheckPlayersAreResponding(void) {
    int i;
    tU32 time;
    tNet_message* message;

    time = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && time - gNet_players[i].last_heard_from_him > 20000) {
            gNet_players[i].player_status = ePlayer_status_not_responding;
        }
    }
    if (gNet_mode == eNet_mode_client && gLast_host_query == 0) {
        message = NetBuildGuaranteedMessage(eNet_message_chunk_type_netsynch, 0);
        NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
        gLast_host_query = time;
    }
}

// FUNCTION: CARMA2_HW 0x004e2cb0
// FUNCTION: CARMA2_SW 0x004df030
// FUNCTION: CARMA2_CHEAT 0x004e8840
void C2_HOOK_FASTCALL NetSynchStartDraw(int pCurrent_choice, int pCurrent_mode) {
    int i;
    int number_ready;
    char s[256];

    number_ready = 0;
    CheckPlayersAreResponding();
    NetPlayerStatusChanged(ePlayer_status_ready);
    for (i = 0; i < kMax_netplayers; i++) {
        BrPixelmapRectangleFill(gBack_screen,
            gCurrent_graf_data->field_0x3a4,
            gCurrent_graf_data->field_0x3b8 * i + gCurrent_graf_data->field_0x3b4,
            gCurrent_graf_data->field_0x3b0 - gCurrent_graf_data->field_0x3a8,
            gFont_7->glyph_y,
            0);
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        strcpy(s, gNet_players[i].player_name);
        if (gNet_players[i].host) {
            strcat(s, " -");
            strcat(s, GetMiscString(eMiscString_host));
            strcat(s, "-");
        }
        if (gNet_players[i].player_status == ePlayer_status_ready) {
            number_ready++;
        }
    }
    if (gNet_mode == eNet_mode_host && number_ready == gNumber_of_net_players && gNumber_of_net_players > 1 && (!gNo_races_yet || gNumber_of_net_players == kMax_netplayers)) {
        SignalToStartRace();
        gStart_race_sent = 1;
    }
}

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

inline tSO_result C2_HOOK_FASTCALL NetSynchRaceStart2_racestrt(tNet_synch_mode pMode) {
    // GLOBAL: CARMA2_HW 0x0065f5b8
    // GLOBAL: CARMA2_SW 0x00659f18
    // GLOBAL: CARMA2_CHEAT 0x00666d68
    static tFlicette flicker_on_hf[2] = {
        { 321, { 219, 112 }, { 172, 362 } },
        { 321, { 39, 480 }, { 172, 379 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f5e0
    // GLOBAL: CARMA2_SW 0x00659f40
    // GLOBAL: CARMA2_CHEAT 0x00666d90
    static tFlicette flicker_off_hf[2] = {
        { 322, { 219, 112 }, { 172, 362 } },
        { 322, { 39, 480 }, { 172, 379 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f608
    // GLOBAL: CARMA2_SW 0x00659f68
    // GLOBAL: CARMA2_CHEAT 0x00666db8
    static tFlicette push_hf[2] = {
        { 206, { 219, 112 }, { 172, 362 } },
        { 205, { 39, 480 }, { 172, 379 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f630
    // GLOBAL: CARMA2_SW 0x00659f90
    // GLOBAL: CARMA2_CHEAT 0x00666de0
    static tMouse_area mouse_areas_hf[2] = {
        { { 219, 480 }, { 172, 379 }, { 282, 182 }, { 192, 427 }, 0, 0, 0, NULL },
        { { 39, 112 }, { 172, 362 }, { 102, 182 }, { 192, 379 }, 1, 0, 0, NULL },
    };
    // GLOBAL: CARMA2_HW 0x0065f690
    // GLOBAL: CARMA2_SW 0x00659ff0
    // GLOBAL: CARMA2_CHEAT 0x00666e40
    static tInterface_spec interface_spec_hf = {
        0, 203, 0, 0, 0, 0, 8,
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { 1, 1 }, { NetSynchStartGoAhead, NetSynchStartGoAhead }, { 1, 1 }, { NULL, NULL },
        ExitWhenReady, NetSynchStartDraw, 0, NULL, CheckPlayersAreResponding, NetSynchStartDone, 0, { 0, 0 },
        NULL, -1, 1, REC2_ASIZE(flicker_on_hf), flicker_on_hf, flicker_off_hf, push_hf,
        REC2_ASIZE(mouse_areas_hf), mouse_areas_hf, 0, NULL
    };
    // GLOBAL: CARMA2_HW 0x0065f7c0
    // GLOBAL: CARMA2_SW 0x0065a120
    // GLOBAL: CARMA2_CHEAT 0x00666f70
    static tFlicette flicker_on_hs[1] = {
        { 321, { 219, 112 }, { 172, 362 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f7d8
    // GLOBAL: CARMA2_SW 0x0065a138
    // GLOBAL: CARMA2_CHEAT 0x00666f88
    static tFlicette flicker_off_hs[1] = {
        { 322, { 219, 112 }, { 172, 362 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f7f0
    // GLOBAL: CARMA2_SW 0x0065a150
    // GLOBAL: CARMA2_CHEAT 0x00666fa0
    static tFlicette push_hs[1] = {
        { 206, { 219, 112 }, { 172, 362 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f808
    // GLOBAL: CARMA2_SW 0x0065a168
    // GLOBAL: CARMA2_CHEAT 0x00666fb8
    static tMouse_area mouse_areas_hs[1] = {
        { { 219, 480 }, { 172, 379 }, { 282, 182 }, { 192, 427 }, 0, 0, 0, NULL },
    };
    // GLOBAL: CARMA2_HW 0x0065f838
    // GLOBAL: CARMA2_SW 0x0065a198
    // GLOBAL: CARMA2_CHEAT 0x00666fe8
    static tInterface_spec interface_spec_hs = {
        0, 209, 0, 0, 0, 0, 8,
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { 1, 1 }, { NetSynchStartGoAhead, NetSynchStartGoAhead }, { 1, 1 }, { NULL, NULL },
        ExitWhenReady, NetSynchStartDraw, 0, NULL, CheckPlayersAreResponding, NetSynchStartDone, 0, { 0, 0 },
        NULL, -1, 1, REC2_ASIZE(flicker_on_hs), flicker_on_hs, flicker_off_hs, push_hs,
        REC2_ASIZE(mouse_areas_hs), mouse_areas_hs, 0, NULL
    };
    // GLOBAL: CARMA2_HW 0x0065f968
    // GLOBAL: CARMA2_SW 0x0065a2c8
    // GLOBAL: CARMA2_CHEAT 0x00667118
    static tFlicette flicker_on_c[1] = {
        { 321, { 219, 112 }, { 172, 362 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f980
    // GLOBAL: CARMA2_SW 0x0065a2e0
    // GLOBAL: CARMA2_CHEAT 0x00667130
    static tFlicette flicker_off_c[1] = {
        { 322, { 219, 112 }, { 172, 362 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f998
    // GLOBAL: CARMA2_SW 0x0065a2f8
    // GLOBAL: CARMA2_CHEAT 0x00667148
    static tFlicette push_c[1] = {
        { 207, { 219, 112 }, { 172, 362 } },
    };
    // GLOBAL: CARMA2_HW 0x0065f9b0
    // GLOBAL: CARMA2_SW 0x0065a310
    // GLOBAL: CARMA2_CHEAT 0x00667160
    static tMouse_area mouse_areas_c[1] = {
        { { 219, 112 }, { 172, 362 }, { 282, 182 }, { 192, 379 }, 0, 0, 0, NULL },
    };
    // GLOBAL: CARMA2_HW 0x0065f9e0
    // GLOBAL: CARMA2_SW 0x0065a340
    // GLOBAL: CARMA2_CHEAT 0x00667190
    static tInterface_spec interface_spec_c = {
        0, 204, 0, 0, 0, 0, 8,
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { -1, 0 }, { 1, 0 }, { 0, 0 }, { 1, 0 }, { NULL, NULL },
        { 1, 1 }, { NetSynchStartGoAhead, NetSynchStartGoAhead }, { 1, 1 }, { NULL, NULL },
        ExitWhenReady, NetSynchStartDraw, 0, NULL, CheckPlayersAreResponding, NetSynchStartDone, 0, { 0, 0 },
        NULL, -1, 1, REC2_ASIZE(flicker_on_c), flicker_on_c, flicker_off_c, push_c,
        REC2_ASIZE(mouse_areas_c), mouse_areas_c, 0, NULL
    };
    int result;

    if (pMode != eNet_synch_client) {
        if (gCurrent_net_game->status.stage == eNet_game_stage0) {
            gCurrent_net_game->status.stage = 1;
        }
        SetUpNetCarPositions();
        gNet_synch_start_racestrt = PDGetTotalTime();
    }
    TurnOnPaletteConversion();
    switch (pMode) {
    case eNet_synch_host_first:
        result = DoInterfaceScreen(&interface_spec_hf, 0, 1);
        break;
    case eNet_synch_host_subsequent:
        result = DoInterfaceScreen(&interface_spec_hs, 0, -1);
        break;
    case eNet_synch_client:
        result = DoInterfaceScreen(&interface_spec_c, 0, -1);
        break;
#ifdef REC2_FIX_BUGS
    default:
        result = 1;
        break;
#endif
    }
    TurnOffPaletteConversion();
    FadePaletteDown();
    switch (result) {
    case -1:
    case 0:
        NetLeaveGame(gCurrent_net_game);
        return eSO_continue;
    case 1:
        return eSO_continue;
    }
    return eSO_continue;
}

// FUNCTION: CARMA2_HW 0x004e2fb0
// FUNCTION: CARMA2_SW 0x004df330
// FUNCTION: CARMA2_CHEAT 0x004e8b30
tSO_result C2_HOOK_FASTCALL NetSynchRaceStart(void) {

    SuspendPendingFlic();
    if (gNet_mode == eNet_mode_host) {
        if (gNo_races_yet) {
            return NetSynchRaceStart2_racestrt(eNet_synch_host_first);
        } else {
            return NetSynchRaceStart2_racestrt(eNet_synch_host_subsequent);
        }
    } else {
        return NetSynchRaceStart2_racestrt(eNet_synch_client);
    }
}

// ReceivedViewCar


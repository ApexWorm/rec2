/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "62-graphics3.h"

#include "01-network.h"
#include "02-init.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "36-trig.h"
#include "41-utility.h"
#include "42-input.h"
#include "49-physics.h"
#include "52-errors.h"
#include "53-controls.h"
#include "58-crush2.h"
#include "61-pedestrn-phys.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x006593a0
// GLOBAL: CARMA2_SW 0x00653d88
// GLOBAL: CARMA2_CHEAT 0x0065fcb8
const int gArrows[2][4][60] = {
    {
        // inner arrow (=fill)
        { 10,  0,  0, -1,  0,  1,  0,  0, -1,  0, -2,  0,  1, -1,  1,  1,  1, -2,  2,  2,  2, },
        { 11,  0,  0, -1,  0,  1,  0,  0, -1,  1, -1,  1, -2, -2,  1, -1,  1,  0,  1,  1,  1,  1,  2, },
        {  9,  0,  0, -2,  0, -1,  0,  1,  0,  0, -1,  1, -1,  2, -2,  0,  1,  0,  2, },
        { 11,  0,  0, -1,  0,  1,  0, -2, -1, -1, -1,  0, -1,  1, -1,  2, -1, -1,  1,  0,  1, -1,  2, },
    },
    {
        // outer arrow (=border)
        { 26,  1, -3,  1, -2,  1, -1,  2, -1,  2,  0,  2,  1,  3,  1,  3,  2,  3,  3,  2,  3,  1,  3,  1,  2,  0,  2, -1,  2,
          -1,  3, -2,  3, -3,  3, -3,  2, -3,  1, -2,  1, -2,  0, -2, -1, -1, -1, -1, -2, -1, -3,  0, -3, },
        { 22,  0, -3,  1, -3,  2, -3,  2, -2,  2, -1,  2,  0,  2,  1,  2,  2,  2,  3,  1,  3,  0,  3,  0,  2, -1,  2, -2,  2,
          -3,  2, -3,  1, -3,  0, -2,  0, -2, -1, -1, -1, -1, -2,  0, -2, },
        { 24,  1, -3,  2, -3,  3, -3,  3, -2,  3, -1,  2, -1,  2,  0,  2,  1,  1,  1,  1,  2,  1,  3,  0,  3, -1,  3, -1,  2,
          -1,  1, -2,  1, -3,  1, -3,  0, -3, -1, -2, -1, -1, -1, -1, -2,  0, -2,  1, -2, },
        { 22, -3, -2, -2, -2, -1, -2,  0, -2,  1, -2,  2, -2,  3, -2,  3, -1,  3,  0,  2,  0,  2,  1,  1,  1,  1,  2,  0,  2,
           0,  3, -1,  3, -2,  3, -2,  2, -2,  1, -2,  0, -3,  0, -3, -1, },
    },
};

// GLOBAL: CARMA2_HW 0x00659b20
// GLOBAL: CARMA2_SW 0x00654508
// GLOBAL: CARMA2_CHEAT 0x00660438
tOpponent_Status gPrevious_opponent_status = eOpponent_status_Uninitialized;

// GLOBAL: CARMA2_HW 0x00659b24
// GLOBAL: CARMA2_SW 0x0065450c
// GLOBAL: CARMA2_CHEAT 0x0066043c
int gPrevious_opponent_message_index = -1;

// GLOBAL: CARMA2_HW 0x00655df0
// GLOBAL: CARMA2_SW 0x00650cd8
// GLOBAL: CARMA2_CHEAT 0x0065c670
br_vector3 y_unit_vector = { { 0.0f, 1.0f, 0.0f } };

// GLOBAL: CARMA2_HW 0x00655dfc
// GLOBAL: CARMA2_SW 0x00650ce4
// GLOBAL: CARMA2_CHEAT 0x0065c67c
int gMini_map_visible = 2;

// GLOBAL: CARMA2_HW 0x0074a780
// GLOBAL: CARMA2_CHEAT 0x00751e20
tCar_icon gCar_icons[128];

// GLOBAL: CARMA2_HW 0x0074ab84
// GLOBAL: CARMA2_CHEAT 0x00752224
br_actor* gCar_icons_model_actor;

// GLOBAL: CARMA2_HW 0x0068d8c4
// GLOBAL: CARMA2_SW 0x0068791c
// GLOBAL: CARMA2_CHEAT 0x00695164
tCar_spec* gTarget_lock_car_1;

// GLOBAL: CARMA2_HW 0x0068d6f0
// GLOBAL: CARMA2_SW 0x00687748
// GLOBAL: CARMA2_CHEAT 0x00694f90
tCar_spec* gTarget_lock_car_2;

// GLOBAL: CARMA2_HW 0x0068d6f4
// GLOBAL: CARMA2_SW 0x0068774c
// GLOBAL: CARMA2_CHEAT 0x00694f94
int gNearest_smash_blip_index; /* FIXME: name guessed from the old DrawSmashBlip: index of the nearest smash blip, -1 for none */

// GLOBAL: CARMA2_HW 0x0068d8c8
// GLOBAL: CARMA2_SW 0x00687920
// GLOBAL: CARMA2_CHEAT 0x00695168
int gTarget_lock_enabled;

// GLOBAL: CARMA2_HW 0x0068d8cc
// GLOBAL: CARMA2_SW 0x00687924
// GLOBAL: CARMA2_CHEAT 0x0069516c
int gInitMap_done;

// GLOBAL: CARMA2_HW 0x0068d8d0
// GLOBAL: CARMA2_SW 0x00687928
// GLOBAL: CARMA2_CHEAT 0x00695170
tU32 gTime_oppobar_target_wasted;

// GLOBAL: CARMA2_HW 0x0074abdc
// GLOBAL: CARMA2_SW 0x00741288
// GLOBAL: CARMA2_CHEAT 0x0075227c
float gHeadup_map_x_float;

// GLOBAL: CARMA2_HW 0x0074abd8
// GLOBAL: CARMA2_SW 0x00741284
// GLOBAL: CARMA2_CHEAT 0x00752278
float gHeadup_map_y_float;

// GLOBAL: CARMA2_HW 0x0074abcc
// GLOBAL: CARMA2_SW 0x00741278
// GLOBAL: CARMA2_CHEAT 0x0075226c
float gHeadup_map_w_float;

// GLOBAL: CARMA2_HW 0x0074ab80
// GLOBAL: CARMA2_SW 0x00741230
// GLOBAL: CARMA2_CHEAT 0x00752220
float gHeadup_map_h_float;

// GLOBAL: CARMA2_HW 0x0074ab88
// GLOBAL: CARMA2_SW 0x00741234
// GLOBAL: CARMA2_CHEAT 0x00752228
int gINT_0074ab88;

// GLOBAL: CARMA2_HW 0x0074ab8c
// GLOBAL: CARMA2_SW 0x00741238
// GLOBAL: CARMA2_CHEAT 0x0075222c
int gINT_0074ab8c;

// GLOBAL: CARMA2_HW 0x0074ab90
// GLOBAL: CARMA2_SW 0x0074123c
// GLOBAL: CARMA2_CHEAT 0x00752230
float gFLOAT_0074ab90;

// GLOBAL: CARMA2_HW 0x0074ab94
// GLOBAL: CARMA2_SW 0x00741240
// GLOBAL: CARMA2_CHEAT 0x00752234
int gINT_0074ab94;

// GLOBAL: CARMA2_HW 0x0074ab98
// GLOBAL: CARMA2_SW 0x00741244
// GLOBAL: CARMA2_CHEAT 0x00752238
int gHeadup_map_half_height;

// GLOBAL: CARMA2_HW 0x0074abe4
// GLOBAL: CARMA2_SW 0x00741290
// GLOBAL: CARMA2_CHEAT 0x00752284
int gHeadup_map_half_width;

// GLOBAL: CARMA2_HW 0x0074abec
// GLOBAL: CARMA2_SW 0x00741298
// GLOBAL: CARMA2_CHEAT 0x0075228c
int gINT_0074abec;

// GLOBAL: CARMA2_HW 0x0068d8d8
// GLOBAL: CARMA2_SW 0x00687930
// GLOBAL: CARMA2_CHEAT 0x00695178
br_pixelmap* gMap_overlay;

// GLOBAL: CARMA2_HW 0x0074aba0
// GLOBAL: CARMA2_SW 0x0074124c
// GLOBAL: CARMA2_CHEAT 0x00752240
int gMini_map_x;

// GLOBAL: CARMA2_HW 0x0074aba4
// GLOBAL: CARMA2_SW 0x00741250
// GLOBAL: CARMA2_CHEAT 0x00752244
int gMini_map_y;

// GLOBAL: CARMA2_HW 0x0074abd4
// GLOBAL: CARMA2_SW 0x00741280
// GLOBAL: CARMA2_CHEAT 0x00752274
int gINT_0074abd4;

// GLOBAL: CARMA2_HW 0x0074abd0
// GLOBAL: CARMA2_SW 0x0074127c
// GLOBAL: CARMA2_CHEAT 0x00752270
int gINT_0074abd0;

// GLOBAL: CARMA2_HW 0x0068d8dc
// GLOBAL: CARMA2_SW 0x00687934
// GLOBAL: CARMA2_CHEAT 0x0069517c
br_pixelmap* gMini_map;

// GLOBAL: CARMA2_HW 0x00655f04
// GLOBAL: CARMA2_CHEAT 0x0065c784
int gINT_00655f04 = 520;

// GLOBAL: CARMA2_HW 0x00655f08
// GLOBAL: CARMA2_CHEAT 0x0065c788
int gINT_00655f08 = 380;

// GLOBAL: CARMA2_HW 0x0068d6a8
// GLOBAL: CARMA2_SW 0x00687700
// GLOBAL: CARMA2_CHEAT 0x00694f48
br_matrix34 gCar_in_map_space;

// GLOBAL: CARMA2_HW 0x0068d6d8
// GLOBAL: CARMA2_SW 0x00687730
// GLOBAL: CARMA2_CHEAT 0x00694f78
br_vector2 gVector2_0068d6d8;

// GLOBAL: CARMA2_HW 0x0068d6e0
// GLOBAL: CARMA2_SW 0x00687738
// GLOBAL: CARMA2_CHEAT 0x00694f80
float gFLOAT_0068d6e0;

// GLOBAL: CARMA2_HW 0x0068d890
// GLOBAL: CARMA2_SW 0x006878e8
// GLOBAL: CARMA2_CHEAT 0x00695130
int gMap_overlay_offset_x;

// GLOBAL: CARMA2_HW 0x0068c858
// GLOBAL: CARMA2_SW 0x006868b0
// GLOBAL: CARMA2_CHEAT 0x006940f8
int gMap_overlay_offset_y;

// GLOBAL: CARMA2_HW 0x0068d8b0
// GLOBAL: CARMA2_SW 0x00687908
// GLOBAL: CARMA2_CHEAT 0x00695150
br_vector2 gVector2_0068d8b0;

// GLOBAL: CARMA2_HW 0x0068d8b8
// GLOBAL: CARMA2_SW 0x00687910
// GLOBAL: CARMA2_CHEAT 0x00695158
float gFLOAT_0068d8b8;

// GLOBAL: CARMA2_HW 0x0074abc4
// GLOBAL: CARMA2_SW 0x00741270
// GLOBAL: CARMA2_CHEAT 0x00752264
float gFLOAT_0074abc4;

// GLOBAL: CARMA2_HW 0x0068d8a0
// GLOBAL: CARMA2_SW 0x006878f8
// GLOBAL: CARMA2_CHEAT 0x00695140
float gFLOAT_0068d8a0;

// GLOBAL: CARMA2_HW 0x0068d8a4
// GLOBAL: CARMA2_SW 0x006878fc
// GLOBAL: CARMA2_CHEAT 0x00695144
float gFLOAT_0068d8a4;

// GLOBAL: CARMA2_HW 0x0068d8a8
// GLOBAL: CARMA2_SW 0x00687900
// GLOBAL: CARMA2_CHEAT 0x00695148
float gMini_map_arrow_z;

// GLOBAL: CARMA2_HW 0x0074ab9c
// GLOBAL: CARMA2_SW 0x00741248
// GLOBAL: CARMA2_CHEAT 0x0075223c
float gFLOAT_0074ab9c;

// GLOBAL: CARMA2_HW 0x0074abb8
// GLOBAL: CARMA2_SW 0x00741264
// GLOBAL: CARMA2_CHEAT 0x00752258
float gFLOAT_0074abb8;

// GLOBAL: CARMA2_HW 0x0074abbc
// GLOBAL: CARMA2_SW 0x00741268
// GLOBAL: CARMA2_CHEAT 0x0075225c
int gMap_render_y_i;

// GLOBAL: CARMA2_HW 0x0074abc0
// GLOBAL: CARMA2_SW 0x0074126c
// GLOBAL: CARMA2_CHEAT 0x00752260
int gMap_render_x_i;

// GLOBAL: CARMA2_HW 0x0074abc8
// GLOBAL: CARMA2_SW 0x00741274
// GLOBAL: CARMA2_CHEAT 0x00752268
int gMap_render_height_i;

// GLOBAL: CARMA2_HW 0x0074abe8
// GLOBAL: CARMA2_SW 0x00741294
// GLOBAL: CARMA2_CHEAT 0x00752288
int gMap_render_width_i;

// GLOBAL: CARMA2_HW 0x0068d6e4
// GLOBAL: CARMA2_SW 0x0068773c
// GLOBAL: CARMA2_CHEAT 0x00694f84
br_pixelmap* gCheckpoint_numbers;

// GLOBAL: CARMA2_HW 0x0068d88c
// GLOBAL: CARMA2_SW 0x006878e4
// GLOBAL: CARMA2_CHEAT 0x0069512c
int gBack_original_base_x;

// GLOBAL: CARMA2_HW 0x0068d888
// GLOBAL: CARMA2_SW 0x006878e0
// GLOBAL: CARMA2_CHEAT 0x00695128
int gBack_original_base_y;

// GLOBAL: CARMA2_HW 0x0068d6ec
// GLOBAL: CARMA2_SW 0x00687744
// GLOBAL: CARMA2_CHEAT 0x00694f8c
int gBack_original_origin_x;

// GLOBAL: CARMA2_HW 0x0068d6e8
// GLOBAL: CARMA2_SW 0x00687740
// GLOBAL: CARMA2_CHEAT 0x00694f88
int gBack_original_origin_y;

// GLOBAL: CARMA2_HW 0x0068c87c
// GLOBAL: CARMA2_SW 0x006868d4
// GLOBAL: CARMA2_CHEAT 0x0069411c
br_pixelmap* gAlt_meter;

// GLOBAL: CARMA2_HW 0x0068c880
// GLOBAL: CARMA2_SW 0x006868d8
// GLOBAL: CARMA2_CHEAT 0x00694120
br_matrix23 gMatrix23_0068c880;

// GLOBAL: CARMA2_HW 0x0068c878
// GLOBAL: CARMA2_SW 0x006868d0
// GLOBAL: CARMA2_CHEAT 0x00694118
int gHeadup_map_src_x;

// GLOBAL: CARMA2_HW 0x0068c874
// GLOBAL: CARMA2_SW 0x006868cc
// GLOBAL: CARMA2_CHEAT 0x00694114
int gHeadup_map_src_y;

// GLOBAL: CARMA2_HW 0x0068c868
// GLOBAL: CARMA2_SW 0x006868c0
// GLOBAL: CARMA2_CHEAT 0x00694108
br_vector3 gOrigin_map;

// GLOBAL: CARMA2_HW 0x0068d898
// GLOBAL: CARMA2_SW 0x006878f0
// GLOBAL: CARMA2_CHEAT 0x00695138
br_vector2 gOrigin_headup_map;

// GLOBAL: CARMA2_HW 0x0068c860
// GLOBAL: CARMA2_SW 0x006868b8
// GLOBAL: CARMA2_CHEAT 0x00694100
int gCheckpoint_digit_height;

// GLOBAL: CARMA2_HW 0x0068d8bc
// GLOBAL: CARMA2_SW 0x00687914
// GLOBAL: CARMA2_CHEAT 0x0069515c
int gCheckpoint_digit_center_x;

// GLOBAL: CARMA2_HW 0x0068c85c
// GLOBAL: CARMA2_SW 0x006868b4
// GLOBAL: CARMA2_CHEAT 0x006940fc
tU32 gCheckpoint_digit_center_y;

// GLOBAL: CARMA2_HW 0x0068d8f0
// GLOBAL: CARMA2_SW 0x00687940
tU32 last_flash;

// GLOBAL: CARMA2_HW 0x0068d8f4
// GLOBAL: CARMA2_SW 0x00687944
int flash_state;

// GLOBAL: CARMA2_HW 0x0068d6f8
// GLOBAL: CARMA2_SW 0x00687750
// GLOBAL: CARMA2_CHEAT 0x00694f98
tOppo_status_messages gOppo_status_messages[9] = { 0 };

// GLOBAL: CARMA2_HW 0x0068d884
// GLOBAL: CARMA2_SW 0x006878dc
// GLOBAL: CARMA2_CHEAT 0x00695124
br_pixelmap* gSmashy_dot = NULL;

// GLOBAL: CARMA2_HW 0x0074abe0
// GLOBAL: CARMA2_SW 0x0074128c
// GLOBAL: CARMA2_CHEAT 0x00752280
int gCount_smashable_race_targets;

// GLOBAL: CARMA2_HW 0x0068c898
// GLOBAL: CARMA2_SW 0x006868f0
// GLOBAL: CARMA2_CHEAT 0x00694138
tSmashable_race_target gSmashable_race_targets[300];

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x0068d8c0
// GLOBAL: CARMA2_CHEAT 0x00695160
br_actor* gLit_op_stat_actor;
#endif

inline br_uint_32 C2_HOOK_FASTCALL CarArrowColour(tCar_spec* pCar, tVehicle_type pCategory) {

    if (gNet_mode != eNet_mode_none && pCar->shrapnel_materials[0] != NULL) {
        if (NetPlayerFromCar(pCar)->field_0x80) {
            return 0xffffffff;
        } else {
            return pCar->shrapnel_materials[0]->index_base;
        }
    } else if (pCar->knackered) {
        return 0;
    } else {
        return gCar_map_colours[pCategory];
    }
}

// FUNCTION: CARMA2_HW 0x004945f0
// FUNCTION: CARMA2_SW 0x004918f0
// FUNCTION: CARMA2_CHEAT 0x00494820
void C2_HOOK_FASTCALL ToggleTargetLock(void) {
    char name[256];
    char s[256];

    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_foxy) {
            return;
        }
        if (!gCurrent_net_game->options.show_players_on_map) {
            return;
        }
    }
    if (gTarget_lock_car_1 == NULL) {
        NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_no_target_to_lock_onto), 0);
    } else {
        gTarget_lock_enabled = !gTarget_lock_enabled;
        if (gTarget_lock_enabled) {
            if (gNet_mode != eNet_mode_none) {
                strcpy(name, NetPlayerFromCar(gTarget_lock_car_2)->player_name);
            } else {
                strcpy(name, GetDriverName((tU16)gTarget_lock_car_2->car_ID >> 8, (tU16)gTarget_lock_car_2->car_ID & 0xff));
            }
            sprintf(s, "%s %s", GetMiscString(eMiscString_locked_onto), name);
            NewTextHeadupSlot2(4, 0, 1500, -4, s, 0);
        } else {
            NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_target_lock_off), 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00494700
// FUNCTION: CARMA2_SW 0x00491a00
// FUNCTION: CARMA2_CHEAT 0x00494930
void C2_HOOK_FASTCALL CycleTargetLock(void) {
    tVehicle_type category;
    int i;
    int car_index;
    int car_count;
    tCar_spec* car;

    if (gNet_mode == eNet_mode_none || (gCurrent_net_game->type != eNet_game_type_foxy && gCurrent_net_game->options.show_players_on_map)) {
        category = gNet_mode != eNet_mode_none ? eVehicle_net_player : eVehicle_opponent;
        if (gTarget_lock_car_1 == NULL) {
            if (category == eVehicle_self) {
                gTarget_lock_car_1 = &gProgram_state.current_car;
            } else {
                gTarget_lock_car_1 = GetCarSpec(category, 0);
            }
        }
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (i = 0; i < car_count; i++) {
            if (category == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(category, i);
            }
            if (car == gTarget_lock_car_1) {
                if (i == car_count - 1) {
                    car_index = 0;
                } else {
                    car_index = i + 1;
                }
                do {
                    if (category == eVehicle_self) {
                        car = &gProgram_state.current_car;
                    } else {
                        car = GetCarSpec(category, car_index);
                    }
                    if (car_index == car_count - 1) {
                        car_index = 0;
                    } else {
                        car_index++;
                    }
                } while ((car != gTarget_lock_car_1 && car->knackered)
                    || (gNet_mode == eNet_mode_none && gCurrent_race.race_spec->race_type == kRaceType_Cars && !car->is_race_goal)
                    || (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_6 && NetPlayerFromCar(car)->field_0x80));
                gTarget_lock_car_1 = car;
                gTarget_lock_car_2 = car;
                gTarget_lock_enabled = 0;
                ToggleTargetLock();
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00494840
// FUNCTION: CARMA2_SW 0x00491b40
void C2_HOOK_FASTCALL CycleMapOptions(void) {

    gCheckpoint_finder_enabled = !gCheckpoint_finder_enabled;
    NewTextHeadupSlot2(4, 0, 1500, -4, gCheckpoint_finder_enabled ? GetMiscString(eMiscString_checkpoint_finder_turned_on) : GetMiscString(eMiscString_checkpoint_finder_turned_off), 0);
}

// FUNCTION: CARMA2_HW 0x00494880
// FUNCTION: CARMA2_SW 0x00491b80
// FUNCTION: CARMA2_CHEAT 0x00494ab0
void C2_HOOK_FASTCALL ToggleMapTransparency(void) {

    gMap_trans = !gMap_trans;
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x004948a0
// FUNCTION: CARMA2_SW 0x00491ba0
// FUNCTION: CARMA2_CHEAT 0x00494ad0
void C2_HOOK_FASTCALL InitTargetLock(void) {

    gTarget_lock_enabled = 0;
}

// FUNCTION: CARMA2_HW 0x004948b0
// FUNCTION: CARMA2_SW 0x00491bb0
// FUNCTION: CARMA2_CHEAT 0x00494ae0
void C2_HOOK_FASTCALL SetIntegerMapRenders(void) {

#ifdef REC2_HW
    gMap_render_width = 128.0f;
    gMap_render_height = 80.0f;
    gMap_render_x_i = ((int)gMap_render_x) & ~0x3;
    gMap_render_y_i = ((int)gMap_render_y) & ~0x1;
    gMap_render_width_i = (int)gMap_render_width;
    gMap_render_height_i = (int)gMap_render_height;
#endif
#ifdef REC2_SW
    gMap_render_x_i = ((int)gMap_render_x) & ~0x3;
    gMap_render_y_i = ((int)gMap_render_y) & ~0x1;
    gMap_render_width_i = ((int)gMap_render_width) & ~0x3;
    gMap_render_height_i = ((int)gMap_render_height) & ~0x1;
#endif
}

inline int C2_HOOK_FASTCALL OppositeColour(int pColour) {
    int brightness;

    if (pColour < 224) {
        if ((pColour & 0x7) < 4) {
            brightness = 255;
        } else {
            brightness = 0;
        }
    } else {
        if ((pColour & 0xf) < 8) {
            brightness = 255;
        } else {
            brightness = 0;
        }
    }
    return brightness;
}

// FUNCTION: CARMA2_HW 0x00495a00
// STUB: CARMA2_SW 0x00492400
// FUNCTION: CARMA2_CHEAT 0x00495c70
void C2_HOOK_FASTCALL DrawArrow(br_pixelmap* pScreen, br_uint_32 pArrow_index, const br_vector3* pPos, br_uint_32 pColour) {
    int colours[2];
    int colour;
    int x;
    int y;
    int temp;
    int point_count;
    const int* arrow_ptr;
    int i;
    int j;

    C2_HOOK_BUG_ON(REC2_ASIZE(gArrows) != 2);
    C2_HOOK_BUG_ON(sizeof(gArrows) != 1920);

    if (pColour == 0xffffffff) {
        colours[0] = 0;
        colours[1] = 0x84;
    } else {
        colours[0] = pColour;
        if ((int)pColour < 0xe0) {
            if ((int)(pColour & 0x7) < 4) {
                colours[1] = 0xff;
            } else {
                colours[1] = 0;
            }
        } else {
            if ((int)(pColour & 0xf) < 8) {
                colours[1] = 0xff;
            } else {
                colours[1] = 0;
            }
        }
    }
#ifdef REC2_HW
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        colours[0] = PaletteEntry16Bit(gRender_palette, colours[0]);
        colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        if (pColour == 0xffffffff) {
            colours[1] = Colour24BitTo16Bit(0xff00ff);
        }
    }
#endif
    for (i = 0; i < REC2_ASIZE(colours); i++) {
        arrow_ptr = &gArrows[i][pArrow_index & 0x3][0];
        point_count = *arrow_ptr++;
        for (j = 0; j < point_count; j++) {
            colour = colours[i];
            if (pArrow_index & 0x8) {
                x = -*arrow_ptr++;
                y = -*arrow_ptr++;
            } else {
                x = *arrow_ptr++;
                y = *arrow_ptr++;
            }
            if (pArrow_index & 0x4) {
                temp = x;
                x = -y;
                y = temp;
            }
            BrPixelmapPixelSet(pScreen, (int)(pPos->v[0] + x), (int)(pPos->v[1] + y), colour);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00496270
// FUNCTION: CARMA2_SW 0x00492d40
// FUNCTION: CARMA2_CHEAT 0x00496620
void C2_HOOK_FASTCALL DrawMapBlip(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, br_matrix34* pTrans, const br_vector3* pPos, br_uint_32 pColour) {
    tU32 time_diff;
    br_vector3 map_pos;
    br_vector3 offset;
    br_scalar distance;
    br_uint_32 colours[2];
    br_uint_32 colour;
    int arrow_index;
    int size;

    time_diff = pTime - gMap_time;
    BrMatrix34ApplyP(&map_pos, pPos, &gCurrent_race.map_transformation);
    map_pos.v[2] = 0.0f;
    if (((pCar != NULL && pCar->driver == eDriver_oppo) || (pCar != NULL && pCar->driver == eDriver_net_human))
        && (!pCar->knackered || pCar == gTarget_lock_car_1)
        && (gCurrent_race.race_spec->race_type != kRaceType_Cars || pCar->is_race_goal)
        && (gNet_mode == eNet_mode_none
            || (gCurrent_net_game->type == eNet_game_type_6 && !NetPlayerFromCar(pCar)->field_0x80)
            || (gCurrent_net_game->type == eNet_game_type_foxy && pCar == gNet_players[gIt_or_fox].car)
            || (gCurrent_net_game->type != eNet_game_type_6 && gCurrent_net_game->type != eNet_game_type_foxy))) {
        offset.v[0] = (float)(gHeadup_map_half_width + gMap_overlay_offset_x + gINT_0074ab8c) - map_pos.v[0];
        offset.v[1] = (float)(gHeadup_map_half_height + gMap_overlay_offset_y + gINT_0074ab88) - map_pos.v[1];
        offset.v[2] = 0.0f - map_pos.v[2];
        distance = offset.v[2] * offset.v[2] + offset.v[0] * offset.v[0] + offset.v[1] * offset.v[1];
        if ((gTarget_lock_car_2 == NULL
                || (!gTarget_lock_enabled && distance < gFLOAT_0074ab90)
                || (gTarget_lock_enabled && pCar == gTarget_lock_car_2))
            && !pCar->knackered) {
            gTarget_lock_car_2 = pCar;
            gVector2_0068d6d8.v[0] = offset.v[0];
            gVector2_0068d6d8.v[1] = offset.v[1];
            gFLOAT_0068d6e0 = offset.v[2];
            gFLOAT_0074ab90 = distance;
        }
    }
    if (gMini_map_visible || gMap_view == 2) {
        BrMatrix34Mul(&gCar_in_map_space, pTrans, &gCurrent_race.map_transformation);
        arrow_index = (int)((int)(360.0f - FastScalarArcTan2(gCar_in_map_space.m[2][0], gCar_in_map_space.m[2][1]) + 12.25) / 22.5) % 16;
        if (pCar == NULL || pCar->driver != eDriver_local_human || (pTime & 0x100)) {
            DrawArrow(pScreen, arrow_index, &map_pos, pColour);
        }
        colours[0] = pColour;
        colours[1] = OppositeColour(pColour);
        if (gBack_screen->type != BR_PMT_INDEX_8) {
            colours[0] = PaletteEntry16Bit(gRender_palette, colours[0]);
            colours[1] = PaletteEntry16Bit(gRender_palette, colours[1]);
        }
#ifndef REC2_FIX_BUGS
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
        /* the original indexes with the bit itself, so the second colour is read from far past the array */
        colour = colours[pTime & 0x100];
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#else
        colour = colours[(pTime & 0x100) != 0];
#endif
        if (gNet_mode != eNet_mode_none
            && gCurrent_net_game->type == eNet_game_type_foxy
            && pCar == gNet_players[gIt_or_fox].car) {
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.0f),
                (int)(map_pos.v[1] - 8.0f),
                (int)(map_pos.v[0] + 8.0f),
                (int)(map_pos.v[1] - 8.0f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.0f),
                (int)(map_pos.v[1] + 8.0f),
                (int)(map_pos.v[0] + 8.0f),
                (int)(map_pos.v[1] + 8.0f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 8.0f),
                (int)(map_pos.v[1] - 8.0f),
                (int)(map_pos.v[0] - 8.0f),
                (int)(map_pos.v[1] + 8.0f),
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 8.0f),
                (int)(map_pos.v[1] - 8.0f),
                (int)(map_pos.v[0] + 8.0f),
                (int)(map_pos.v[1] + 8.0f),
                colour);
        }
        if (time_diff <= 500 && pCar != NULL && pCar->driver == eDriver_local_human) {
            size = (35000 - 70 * time_diff) / 500;
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5) - size,
                (int)(map_pos.v[1] - 0.5) - size,
                (int)(map_pos.v[0] + 0.5) + size,
                (int)(map_pos.v[1] - 0.5) - size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5) - size,
                (int)(map_pos.v[1] + 0.5) + size,
                (int)(map_pos.v[0] + 0.5) + size,
                (int)(map_pos.v[1] + 0.5) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] - 0.5) - size,
                (int)(map_pos.v[1] - 0.5) - size,
                (int)(map_pos.v[0] - 0.5) - size,
                (int)(map_pos.v[1] + 0.5) + size,
                colour);
            BrPixelmapLine(pScreen,
                (int)(map_pos.v[0] + 0.5) + size,
                (int)(map_pos.v[1] - 0.5) - size,
                (int)(map_pos.v[0] + 0.5) + size,
                (int)(map_pos.v[1] + 0.5) + size,
                colour);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL DrawSmashBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3* pPos, int pIndex) {
    br_vector3 map_pos;
    br_vector3 offset;
    br_scalar distance;

    BrMatrix34ApplyP(&map_pos, pPos, &gCurrent_race.map_transformation);
    map_pos.v[2] = 0.0f;
    offset.v[0] = (float)(gHeadup_map_half_width + gMap_overlay_offset_x + gINT_0074ab8c) - map_pos.v[0];
    offset.v[1] = (float)(gHeadup_map_half_height + gMap_overlay_offset_y + gINT_0074ab88) - map_pos.v[1];
    offset.v[2] = 0.0f - map_pos.v[2];
    distance = offset.v[2] * offset.v[2] + offset.v[0] * offset.v[0] + offset.v[1] * offset.v[1];
    if (gNearest_smash_blip_index == -1 || distance < gFLOAT_0074abc4) {
        gFLOAT_0074abc4 = distance;
        gVector2_0068d8b0.v[0] = offset.v[0];
        gVector2_0068d8b0.v[1] = offset.v[1];
        gFLOAT_0068d8b8 = offset.v[2];
        gNearest_smash_blip_index = pIndex;
    }
    if (gNet_mode == eNet_mode_none && (pTime & 0x100)) {
#ifdef REC2_HW
        if (gMap_view == 2) {
            PossibleLock(1);
        }
#endif
        DRPixelmapRectangleMaskedCopy(pScreen,
            (br_int_16)(int)(map_pos.v[0] - gSmashy_dot->width / 2),
            (br_int_16)(int)(map_pos.v[1] - gSmashy_dot->height / 2),
            gSmashy_dot,
            0,
            0,
            gSmashy_dot->width,
            gSmashy_dot->height);
#ifdef REC2_HW
        if (gMap_view == 2) {
            PossibleUnlock(1);
        }
#endif
    }
}

// FUNCTION: CARMA2_HW 0x004967c0
// FUNCTION: CARMA2_CHEAT 0x00496b70
void C2_HOOK_FASTCALL DrawMapSmallBlip(br_pixelmap* pScreen, tU32 pTime, const br_vector3 *pPos, int pColour) {
    br_vector3 map_pos;

    if (pTime & 0x100) {
        pColour = 0;
    }
    BrMatrix34ApplyP(&map_pos, pPos, &gCurrent_race.map_transformation);
    if (map_pos.v[0] >= 0.0f && map_pos.v[0] < (float)pScreen->width
            && map_pos.v[1] >= 0 && map_pos.v[1] < (float)pScreen->height) {
#ifdef REC2_HW
        if (gMap_view == 2) {
            PossibleLock(1);
        }
#endif
        if (gBack_screen->type != BR_PMT_INDEX_8) {
            *(br_uint_16*)((br_uint_8*)pScreen->pixels + (int)map_pos.v[1] * pScreen->row_bytes + 2 * (int)map_pos.v[0]) = PaletteEntry16Bit(gRender_palette, pColour);
        } else {
            ((br_uint_8*)pScreen->pixels)[(int)map_pos.v[1] * pScreen->row_bytes + (int)map_pos.v[0]] = pColour;
        }
#ifdef REC2_HW
        if (gMap_view == 2) {
            PossibleUnlock(1);
        }
#endif
    }
}

inline void C2_HOOK_FASTCALL CopyMapToScreen(void) {

    if (gCurrent_race.map_image != NULL) {
#ifdef REC2_SW
        if (gGraf_data_index != 0) {
            BrPixelmapCopy(gBack_screen, gCurrent_race.map_image);
        } else {
            BrPixelmapCopy(gBack_screen, gCurrent_race.map_image);
        }
#else
        if (gGraf_data_index != 0) {
            PossibleUnlock(1);
            DrPixelmapRectangleCopyPossibleLock(gBack_screen,
                -gBack_screen->origin_x,
                -gBack_screen->origin_y,
                gCurrent_race.map_image,
                gCurrent_race.map_image->origin_x,
                gCurrent_race.map_image->origin_y,
                640, 480);
        } else {
            DRPixelmapCopy(gBack_screen, gCurrent_race.map_image);
        }
#endif
    }
    DimRectangleClipped(gBack_screen,
        gMap_render_x_i - gCurrent_graf_data->map_render_x_marg,
        gMap_render_y_i - gCurrent_graf_data->map_render_y_marg,
        gMap_render_width_i + gMap_render_x_i + gCurrent_graf_data->map_render_x_marg,
        gMap_render_height_i + gMap_render_y_i + gCurrent_graf_data->map_render_y_marg,
        1);
}

inline void C2_HOOK_FASTCALL DrawCarArrow(br_pixelmap* pScreen, tCar_spec* pCar, tU32 pTime, const br_vector3* pPos, tVehicle_type pCategory) {

    DrawMapBlip(pScreen, pCar, pTime, &pCar->car_master_actor->t.t.mat, pPos, CarArrowColour(pCar, pCategory));
}

// FUNCTION: CARMA2_HW 0x00495ba0
// FUNCTION: CARMA2_SW 0x00492540
// FUNCTION: CARMA2_CHEAT 0x00495e10
void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev) {
    int i;
    int checkpoint;
    int checkpoints[10];
    int checkpoint_flags;

    C2_HOOK_ASSERT(gCurrent_race.check_point_count <= REC2_ASIZE(checkpoints));
    for (i = 0; i < gCurrent_race.check_point_count; i++) {
        checkpoints[i] = 0;
    }
    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_checkpoint) {
            checkpoint_flags = gNet_players[gThis_net_player_index].score2;
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                if (checkpoint_flags & 0x1) {
                    pCallback(pMap, i, pTime, 1);
                    checkpoints[i] = 1;
                }
                checkpoint_flags >>= 1;
            }
        } else if (gCurrent_net_game->type == eNet_game_type_5 || gCurrent_net_game->type == eNet_game_type_4) {
            if (gNet_players[gThis_net_player_index].score2 >= 0) {
                checkpoint = gNet_players[gThis_net_player_index].score2 % gCurrent_race.check_point_count;
                pCallback(pMap, checkpoint, pTime, 1);
                checkpoints[checkpoint] = 1;
            }
        }
    } else {
        pCallback(pMap, gCheckpoint - 1, pTime, 1);
        checkpoints[gCheckpoint - 1] = 1;
    }
    if (pDraw_prev) {
        if (gNet_mode == eNet_mode_none
                || gCurrent_net_game->type == eNet_game_type_checkpoint
                || gCurrent_net_game->type == eNet_game_type_4
                || gCurrent_net_game->type == eNet_game_type_5) {
            for (i = 0; i < gCurrent_race.check_point_count; i++) {
                if (!checkpoints[i]) {
                    pCallback(pMap, i, pTime, 0);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004968f0
// FUNCTION: CARMA2_SW 0x00493260
void C2_HOOK_FASTCALL CalcMapCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 p;
    tRace_checkpoint_spec* checkpoint;

    checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
    BrMatrix34ApplyP(&p, &checkpoint->pos, &gCurrent_race.map_transformation);
    BrVector2Copy(&checkpoint->map_position, &p);
}

// FUNCTION: CARMA2_HW 0x00496940
void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    br_vector3 pos_mapspace;
    br_vector2 p;
    tRace_checkpoint_spec* checkpoint;

    checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
    BrMatrix34ApplyP(&pos_mapspace, &checkpoint->pos, &gCurrent_race.map_transformation);
    BrVector2Set(&p, pos_mapspace.v[0] - gOrigin_map.v[0] + gOrigin_headup_map.v[0], pos_mapspace.v[1] - gOrigin_map.v[1] + gOrigin_headup_map.v[1]);
    BrMatrix23TApplyV(&checkpoint->map_position, &p, &gMatrix23_0068c880);
    checkpoint->map_position.v[0] += (float)gINT_0074abd4;
    checkpoint->map_position.v[1] += (float)gINT_0074abd0;
}

// FUNCTION: CARMA2_HW 0x004969e0
// FUNCTION: CARMA2_CHEAT 0x00496d90
void C2_HOOK_FASTCALL DrawCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    // GLOBAL: CARMA2_HW 0x0068d8e8
    // GLOBAL: CARMA2_CHEAT 0x00695188
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d8ec
    // GLOBAL: CARMA2_CHEAT 0x0069518c
    static int flash_rate;
    tRace_checkpoint_spec* checkpoint;

    if ((pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count && gTrack_version > 0 && !pTarget)
            || Flash(300, &last_flash, &flash_rate)) {

        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
        if (checkpoint->map_position.v[0] >= 0.0f
                && checkpoint->map_position.v[0] < (float)pMap->width
                && checkpoint->map_position.v[1] >= 0.0f
                && checkpoint->map_position.v[1] < (float)pMap->height) {

#ifdef REC2_HW
            if (gMap_view == 2) {
                PossibleLock(1);
            }
#endif
            DRPixelmapRectangleMaskedCopy(pMap,
                (int)(checkpoint->map_position.v[0] - (float)gCheckpoint_digit_center_x),
                (int)(checkpoint->map_position.v[1] - (float)(int)gCheckpoint_digit_center_y),
                gCheckpoint_numbers,
                0,
                pCheckpoint * gCheckpoint_digit_height,
                gCheckpoint_numbers->width,
                gCheckpoint_digit_height);
#ifdef REC2_HW
            if (gMap_view == 2) {
                PossibleUnlock(1);
            }
#endif
        }
    }
}

// FUNCTION: CARMA2_HW 0x00495d00
// FUNCTION: CARMA2_SW 0x00493350
// FUNCTION: CARMA2_CHEAT 0x00495f70
void C2_HOOK_FASTCALL DrawOffsetCheckpoint(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    tRace_checkpoint_spec* checkpoint;

    if ((pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count
            && gTrack_version > 0 && pTarget == 0) || Flash(300, &last_flash, &flash_state)) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
        if (checkpoint->map_position.v[0] >= 0.0f
                && checkpoint->map_position.v[0] < (float)pMap->width
                && checkpoint->map_position.v[1] >= 0.0f
                && checkpoint->map_position.v[1] < (float)pMap->height) {
            DRPixelmapRectangleMaskedCopy(pMap,
#ifdef REC2_HW
                (br_int_16)(int)(checkpoint->map_position.v[0] + 8.0f - (float)gCheckpoint_digit_center_x),
#endif
#ifdef REC2_SW
                (br_int_16)(int)(checkpoint->map_position.v[0] - (float)gCheckpoint_digit_center_x),
#endif
                (br_int_16)(int)(checkpoint->map_position.v[1] - (float)(int)gCheckpoint_digit_center_y),
                gCheckpoint_numbers,
                0,
                (br_int_16)(pCheckpoint * gCheckpoint_digit_height),
                gCheckpoint_numbers->width,
                (br_int_16)gCheckpoint_digit_height);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00495e10
// FUNCTION: CARMA2_SW 0x004927b0
// FUNCTION: CARMA2_CHEAT 0x00496080
void C2_HOOK_FASTCALL DoMapOverlays(br_pixelmap* pScreen) {
    tU32 the_time;
    tVehicle_type category;
    int car_count;
    tCar_spec* car;
    int count;
    tNet_game_player_info* net_player;
    int i;
    br_vector3 pos;
    const br_vector3* position;
    tSmashable_race_target* smash_target;
#ifdef REC2_CHEAT
    char s[512];
    char s2[512];
#endif

#ifdef REC2_CHEAT
    if (gNet_mode == eNet_mode_none && gMap_view == 2) {
        if (gCurrent_race.race_spec->description[0] != '\0') {
            s2[0] = '\0';
            sprintf(s, "%s", gCurrent_race.race_spec->description);
            MungeMetaCharactersChar(s, 'R', 1);
            for (i = strlen(s) - 1; i >= 0; i--) {
                if (s[i] == 1) {
                    strcpy(s2, &s[i + 1]);
                    break;
                }
            }
        } else {
            sprintf(s2, "...%s", gCurrent_race.race_spec->name);
        }
        DRTextWidth(&gFonts[2], s2);
        BrPixelmapRectangleFill(gBack_screen, 0, 0, gBack_screen->width, 16, 0);
        gRender_poly_text = !gRender_poly_text;
        OoerrIveGotTextInMeBoxMissus(2, s2, gBack_screen, 0, 0, gBack_screen->width, gBack_screen->height, 0);
        gRender_poly_text = !gRender_poly_text;
    }
#endif
    the_time = PDGetTotalTime();
    if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {
        if (gMap_view == 2) {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint, 1);
            DoSomeThingsToCheckpoints(pScreen, the_time, DrawCheckpoint, 1);
        } else {
            DoSomeThingsToCheckpoints(pScreen, the_time, CalcMapCheckpoint2, 1);
        }
    }
    if (gShow_peds_on_map
        || gCurrent_race.race_spec->race_type == kRaceType_Peds
        || (gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)) {
        count = GetPedCount();
        for (i = 0; i < count; i++) {
            if (GetPedPosition(i, &pos)) {
                DrawMapSmallBlip(pScreen, the_time, &pos, 52);
            }
        }
    }
    if ((gNet_mode != eNet_mode_none && gCurrent_net_game->options.show_powerups_on_map)
        || gCurrent_race.race_spec->expansion) {
        count = GetPowerupCount();
        for (i = 0; i < count; i++) {
            GetPowerupPosition(i, &pos);
            DrawMapSmallBlip(pScreen, the_time, &pos, 4);
        }
    }
    for (category = gShow_opponents ? eVehicle_opponent : eVehicle_self; category >= (gMap_view == 1 ? eVehicle_net_player : eVehicle_self); category--) {
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (i = 0; i < car_count; i++) {
            if (category == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(category, i);
            }
            if (gNet_mode != eNet_mode_none) {
                net_player = NetPlayerFromCar(car);
            }
            if (gNet_mode == eNet_mode_none
                || (!car->knackered && !net_player->wasted && net_player->player_status >= ePlayer_status_racing)) {
                if (category == eVehicle_self) {
                    position = &gPlayer_car_master_actor->t.t.translate.t;
                } else {
                    position = &GetCarSpec(category, i)->car_master_actor->t.t.translate.t;
                }
                DrawCarArrow(pScreen, car, the_time, position, category);
            }
        }
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
        for (i = 0; i < gCount_smashable_race_targets; i++) {
            smash_target = &gSmashable_race_targets[i];
            if ((smash_target->model != NULL && smash_target->actor->render_style != BR_RSTYLE_NONE && smash_target->actor->model == smash_target->model)
                || (smash_target->field_0x8 >= 0 && gPedestrian_array[smash_target->field_0x8].hit_points > 0)) {
                if (smash_target->field_0x8 >= 0) {
                    position = &gPedestrian_array[smash_target->field_0x8].pos;
                } else {
                    position = &smash_target->actor->t.t.translate.t;
                }
                DrawSmashBlip(pScreen, the_time, position, i);
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL StartMap(void) {

    gBack_original_origin_x = gBack_screen->origin_x;
    gBack_original_origin_y = gBack_screen->origin_y;
    gBack_original_base_x = gBack_screen->base_x;
    gBack_original_base_y = gBack_screen->base_y;
    gBack_screen->origin_x = 0;
    gBack_screen->origin_y = 0;
    gBack_screen->base_x = 0;
    gBack_screen->base_y = 0;
}

void C2_HOOK_FASTCALL FinishMap(void) {

    gBack_screen->origin_x = (br_int_16)gBack_original_origin_x;
    gBack_screen->origin_y = (br_int_16)gBack_original_origin_y;
    gBack_screen->base_x = (br_uint_16)gBack_original_base_x;
    gBack_screen->base_y = (br_uint_16)gBack_original_base_y;
}

// FUNCTION: CARMA2_HW 0x00496b10
// FUNCTION: CARMA2_SW 0x00493450
// FUNCTION: CARMA2_CHEAT 0x00496ec0
void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget) {
    int p2_x;
    int p2_y;
    tRace_checkpoint_spec* checkpoint;

    if (pCheckpoint >= 0 && pCheckpoint < gCurrent_race.check_point_count) {
        checkpoint = &gCurrent_race.checkpoints[pCheckpoint];
        if (checkpoint->map_position.v[0] < 0.f) {
            p2_x = 8;
            p2_y = (int)((checkpoint->map_position.v[1] - (float)gINT_0074abd0)
                * ((float)gINT_0074abd4 / ((float)gINT_0074abd4 - checkpoint->map_position.v[0]))
                + (float)gINT_0074abd0);
        } else {
            p2_x = (int)(checkpoint->map_position.v[0] + 8.f);
            p2_y = (int)checkpoint->map_position.v[1];
        }
        FancyDrawLine(pMap, gINT_0074abd4 + 8, gINT_0074abd0, p2_x, p2_y, 132, 100);
    }
}

inline void C2_HOOK_FASTCALL DoDirectionFinderStuff(br_pixelmap* pMap) {
    tU32 the_time;
    const br_vector2* offset;
    br_scalar distance;
    br_vector2 dir;

    the_time = PDGetTotalTime();
    if ((gMini_map_visible || gMap_view == 2)
        && gCheckpoint_finder_enabled
        && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints)) {
        DoSomeThingsToCheckpoints(pMap, the_time, CheckpointLine, 0);
    }
    if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
        if (gNearest_smash_blip_index < 0) {
            return;
        }
        distance = gFLOAT_0074abc4;
        offset = &gVector2_0068d8b0;
    } else if ((gNet_mode != eNet_mode_none
                   || gCurrent_race.race_spec->race_type == kRaceType_Carma1
                   || gCurrent_race.race_spec->race_type == kRaceType_Cars)
               && gTarget_lock_car_2 != NULL) {
        distance = gFLOAT_0074ab90;
        offset = &gVector2_0068d6d8;
    } else {
        return;
    }
    if (distance > gFLOAT_0074abb8) {
        BrMatrix23TApplyV(&dir, offset, &gMatrix23_0068c880);
        distance = (br_scalar)sqrt(distance) - (float)gCurrent_graf_data->field_0x51c;
        BrVector2Normalise(&dir, &dir);
        if (fabs(distance * dir.v[0]) > gINT_0074abd4) {
            distance *= gINT_0074abd4 / fabs(distance * dir.v[0]);
        }
        if (gMini_map_visible || gMap_view == 2) {
            FancyDrawLine(pMap,
                gINT_0074abd4 + 8,
                gINT_0074abd0,
                (int)(gINT_0074abd4 - distance * dir.v[0] + 8.0f),
                (int)(gINT_0074abd0 - distance * dir.v[1]),
                52,
                75);
        }
    }
}

inline void C2_HOOK_FASTCALL MungeMoodMessage(char* pDest, const char* pSrc, int pIs_girl) {
    char* pos;
    const char* word;
    char rest[256];

    strcpy(pDest, pSrc);
    while ((pos = strchr(pDest, '%')) != NULL) {
        strcpy(rest, pos + 1);
        if (!pIs_girl) {
            word = GetMiscString(eMiscString_he);
        } else {
            word = GetMiscString(eMiscString_she);
        }
        strcpy(pos, word);
        strcat(pDest, rest);
    }
    while ((pos = strchr(pDest, '&')) != NULL) {
        strcpy(rest, pos + 1);
        if (!pIs_girl) {
            word = GetMiscString(eMiscString_him);
        } else {
            word = GetMiscString(eMiscString_her);
        }
        strcpy(pos, word);
        strcat(pDest, rest);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

#ifdef REC2_HW
#define OPPOBAR_FONT (&gFonts[23])
#else
#define OPPOBAR_FONT (&gFonts[6])
#endif

// FUNCTION: CARMA2_HW 0x00494900
// FUNCTION: CARMA2_SW 0x00493520
// FUNCTION: CARMA2_CHEAT 0x00494b30
void C2_HOOK_FASTCALL DoOpponentStatusHeadup(void) {
    // GLOBAL: CARMA2_HW 0x0068d900
    // GLOBAL: CARMA2_SW 0x00687958
    // GLOBAL: CARMA2_CHEAT 0x006951a0
    static tU32 last_label_flash;
    // GLOBAL: CARMA2_HW 0x0068d904
    // GLOBAL: CARMA2_SW 0x0068795c
    // GLOBAL: CARMA2_CHEAT 0x006951a4
    static tU32 last_lock_flash;
    // GLOBAL: CARMA2_HW 0x0068d908
    // GLOBAL: CARMA2_SW 0x00687960
    // GLOBAL: CARMA2_CHEAT 0x006951a8
    static int label_flash_state;
    // GLOBAL: CARMA2_HW 0x0068d90c
    // GLOBAL: CARMA2_SW 0x00687964
    // GLOBAL: CARMA2_CHEAT 0x006951ac
    static int lock_flash_state;
    char s[256];
    int percentage;
    int message_index;
    int colour;
    const char* label;
#ifdef REC2_HW
    br_model* model;
#endif
    tOpponent_Status mood;

    if (gLit_op_stat_bloc == NULL) {
#ifdef REC2_HW
        gLit_op_stat_bloc = LoadPixelmap("litopstat.tif");
#else
        gLit_op_stat_bloc = LoadPixelmap("litbloc1.tif");
#endif
    }
#ifdef REC2_HW
    BRPM_convert(gLit_op_stat_bloc, gBack_screen->type);
#endif
    Flash(250, &last_label_flash, &label_flash_state);
    if (gTarget_lock_enabled && Flash(500, &last_lock_flash, &lock_flash_state)) {
        DRPixelmapText(gBack_screen,
            gCurrent_graf_data->field_0x4d8,
            gCurrent_graf_data->field_0x4dc,
            OPPOBAR_FONT,
            GetMiscString(eMiscString_oppobar_target_locked),
            gBack_screen->width);
    } else {
        if (gTarget_lock_car_2 == NULL || gTarget_lock_car_2->driver == eDriver_oppo) {
            label = GetMiscString(eMiscString_oppobar_opponent);
        } else {
            label = GetMiscString(eMiscString_oppobar_player);
        }
        DRPixelmapText(gBack_screen,
            gCurrent_graf_data->field_0x4d8,
            gCurrent_graf_data->field_0x4dc,
            OPPOBAR_FONT,
            label,
            gBack_screen->width);
    }
    DRPixelmapText(gBack_screen,
        gCurrent_graf_data->field_0x4e0,
        gCurrent_graf_data->field_0x4e4,
        OPPOBAR_FONT,
        GetMiscString(eMiscString_oppobar_condition),
        gBack_screen->width);
#ifdef REC2_HW
    if (gNet_mode == eNet_mode_none) {
        DRPixelmapText(gBack_screen,
            gCurrent_graf_data->field_0x4e8,
            gCurrent_graf_data->field_0x4ec,
            OPPOBAR_FONT,
            GetMiscString(eMiscString_oppobar_status),
            gBack_screen->width);
    }
#else
    DRPixelmapText(gBack_screen,
        gCurrent_graf_data->field_0x4e8,
        gCurrent_graf_data->field_0x4ec,
        OPPOBAR_FONT,
        GetMiscString(eMiscString_oppobar_status),
        gBack_screen->width);
#endif
    if (gTarget_lock_car_2 == NULL) {
        return;
    }
    if (gNet_mode != eNet_mode_none) {
        strcpy(s, NetPlayerFromCar(gTarget_lock_car_2)->player_name);
    } else {
        strcpy(s, GetDriverName((tU16)gTarget_lock_car_2->car_ID >> 8, (tU16)gTarget_lock_car_2->car_ID & 0xff));
    }
    DRPixelmapText(gBack_screen,
        gCurrent_graf_data->field_0x4f0,
        gCurrent_graf_data->field_0x4f4,
        OPPOBAR_FONT,
        s,
        gBack_screen->width);
#ifdef REC2_HW
    BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat,
        (br_scalar)gCurrent_graf_data->field_0x4f8,
        (br_scalar)-gCurrent_graf_data->field_0x4fc,
        0.0f);
    gCar_icons_model_actor->material->colour_map = gTexture_maps[gCar_icons[gTarget_lock_car_2->index].index];
    BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
    gCar_icons_model_actor->model = gCar_icons[gTarget_lock_car_2->index].model;
    RenderThisHeadup(gCar_icons_model_actor);
#else
    DRPixelmapRectangleMaskedCopy(gBack_screen,
        gCurrent_graf_data->field_0x4f8,
        gCurrent_graf_data->field_0x4fc,
        gIcons_low_pix,
        0,
        gTarget_lock_car_2->index * gCurrent_graf_data->car_icon_height,
        gIcons_low_pix->width,
        gCurrent_graf_data->car_icon_height);
#endif
    if (gTarget_lock_car_2 == NULL) {
        return;
    }
    percentage = PercentageCarHealth(gTarget_lock_car_2);
    sprintf(s, "%d%%", percentage);
    DRPixelmapText(gBack_screen,
        gCurrent_graf_data->field_0x500,
        gCurrent_graf_data->field_0x504,
        OPPOBAR_FONT,
        s,
        gBack_screen->width);
#ifdef REC2_HW
    div(percentage, 10);
    if (gLit_op_stat_actor == NULL) {
        AllocateOppoBarActor(gCurrent_graf_data->field_0x508, gCurrent_graf_data->field_0x50c + 2);
    }
    if (percentage > 0) {
        if (percentage > 100) {
            percentage = 100;
        }
        model = gLit_op_stat_actor->model;
        model->vertices[1].p.v[0] = model->vertices[0].p.v[0] + percentage;
        model->vertices[3].p.v[0] = model->vertices[2].p.v[0] + percentage;
        colour = percentage * 255 / 100;
        model->vertices[1].red = 255 - colour;
        model->vertices[1].grn = colour;
        model->vertices[1].blu = 0;
        model->vertices[3].red = 255 - colour;
        model->vertices[3].grn = colour;
        model->vertices[3].blu = 0;
        BrModelUpdate(model, BR_MODU_VERTICES);
        RenderThisHeadup(gLit_op_stat_actor);
    }
#else
    if (percentage < 20) {
        colour = 4;
    } else if (percentage < 50) {
        colour = 51;
    } else {
        colour = 82;
    }
    DRPixelmapShadedHRectFill(gBack_screen,
        gCurrent_graf_data->field_0x508,
        gCurrent_graf_data->field_0x50c,
        (gCurrent_graf_data->field_0x500 - gCurrent_graf_data->field_0x508 - 5) * percentage / 100,
        3,
        colour);
#endif
    if (gTarget_lock_car_2 == NULL || gTarget_lock_car_2->driver != eDriver_oppo) {
        return;
    }
    mood = GetOpponentMood(gTarget_lock_car_2);
    if (gTarget_lock_car_1 == gTarget_lock_car_2 && gPrevious_opponent_status == mood) {
        message_index = gPrevious_opponent_message_index;
    } else {
        message_index = IRandomBetween(0, gOppo_status_messages[mood].count - 1);
    }
    if ((mood == eOpponent_status_Wasted && gTarget_lock_car_1 != gTarget_lock_car_2) || gPrevious_opponent_status != mood) {
        gTime_oppobar_target_wasted = PDGetTotalTime();
    }
    gPrevious_opponent_message_index = message_index;
    gPrevious_opponent_status = mood;
    gTarget_lock_car_1 = gTarget_lock_car_2;
#ifdef REC2_HW
    if ((mood != eOpponent_status_Wasted || label_flash_state) && gNet_mode == eNet_mode_none) {
#else
    if (mood != eOpponent_status_Wasted || label_flash_state) {
#endif
        MungeMoodMessage(s, gOppo_status_messages[mood].messages[message_index], gTarget_lock_car_2->is_girl);
        DRPixelmapText(gBack_screen,
            gCurrent_graf_data->field_0x510,
            gCurrent_graf_data->field_0x514,
            OPPOBAR_FONT,
            s,
            gBack_screen->width);
    }
}

#undef OPPOBAR_FONT

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x00494f40
// FUNCTION: CARMA2_CHEAT 0x00495170
void C2_HOOK_FASTCALL AllocateOppoBarActor(int pX, int pY) {
    br_material* material;
    br_model* model;
    br_actor* actor;
    int x;

    x = pX;
    material = BrMaterialAllocate("OppoBar Material");
    if (material == NULL) {
        return;
    }
    material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE;
    model = BrModelAllocate("OppoBar Model", 4, 2);
    if (model == NULL) {
        return;
    }
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 1;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 2;
    BrVector3Set(&model->vertices[0].p, (br_scalar)x, (br_scalar)-pY, -1.5f);
    BrVector3Set(&model->vertices[1].p, (br_scalar)(x + 20), (br_scalar)-pY, -1.5f);
    BrVector3Set(&model->vertices[2].p, (br_scalar)x, (br_scalar)(-7 - pY), -1.5f);
    BrVector3Set(&model->vertices[3].p, (br_scalar)(x + 20), (br_scalar)(-7 - pY), -1.5f);
    model->vertices[0].red = 0xff;
    model->vertices[0].grn = 0;
    model->vertices[0].blu = 0;
    model->vertices[2].red = 0xff;
    model->vertices[2].grn = 0;
    model->vertices[2].blu = 0;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrModelAdd(model);
    BrMaterialAdd(material);
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gLit_op_stat_actor = actor;
    if (actor != NULL) {
        actor->material = material;
        gLit_op_stat_actor->model = model;
    }
}
#endif

inline void C2_HOOK_FASTCALL DoNearestOpponentRelativeHeightMarkerStuff(br_pixelmap* pMap) {
    int half_height;
    int border_height;
    br_scalar delta_height;
    int ym;
    int yt;

    if ((gMini_map_visible || gMap_view == 2) && gTarget_lock_car_2 != NULL) {
        half_height = pMap->height / 2;
        border_height = (int)(pMap->height * 0.05);
        BrPixelmapLine(pMap, 1, border_height, 3, border_height, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 1, pMap->height - border_height, 3, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 0, border_height + 1, 0, pMap->height - border_height - 1, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 4, border_height + 1, 4, pMap->height - border_height - 1, RGB565_TO_BACKSCREEN_COLOUR(7, 46, 7));
        BrPixelmapLine(pMap, 0, half_height, 4, half_height, RGB565_TO_BACKSCREEN_COLOUR(31, 62, 31));
        BrPixelmapPixelSet(pMap, 0, border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 4, border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 0, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        BrPixelmapPixelSet(pMap, 4, pMap->height - border_height, RGB565_TO_BACKSCREEN_COLOUR(6, 42, 6));
        delta_height = gPlayer_car_master_actor->t.t.translate.t.v[1] - gTarget_lock_car_2->pos.v[1];
        if (fabs(delta_height) > 15.0 / WORLD_SCALE_D) {
            delta_height = (br_scalar)(sign((int)delta_height) * (15.0 / WORLD_SCALE_D));
        }
        delta_height = (br_scalar)(delta_height * 0.46);
        if (fabs(delta_height) < 0.05) {
            ym = half_height;
        } else {
            ym = (int)((half_height - border_height) * delta_height + half_height);
        }
        yt = border_height + 1;
        if (ym > border_height) {
            yt = ym;
            if (ym >= pMap->height - border_height) {
                yt = pMap->height - border_height - 1;
            }
        }
        if (yt < half_height) {
            BrPixelmapLine(pMap, 1, yt, 1, half_height - 1, RGB565_TO_BACKSCREEN_COLOUR(0, 38, 31));
            BrPixelmapLine(pMap, 2, yt, 2, half_height - 1, RGB565_TO_BACKSCREEN_COLOUR(6, 0, 31));
            BrPixelmapLine(pMap, 3, yt, 3, half_height - 1, 0x15);
        } else if (yt > half_height) {
            BrPixelmapLine(pMap, 1, half_height + 1, 1, yt, RGB565_TO_BACKSCREEN_COLOUR(0, 38, 31));
            BrPixelmapLine(pMap, 2, half_height + 1, 2, yt, RGB565_TO_BACKSCREEN_COLOUR(6, 0, 31));
            BrPixelmapLine(pMap, 3, half_height + 1, 3, yt, 0x15);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004950b0
// FUNCTION: CARMA2_SW 0x00491c00
// FUNCTION: CARMA2_CHEAT 0x004952e0
void C2_HOOK_FASTCALL MapOverlay(void) {
    tU32 the_time;
    br_uint_16 original_base_x;
    br_matrix23 mat23;
    br_vector3 player_pos;

    BrMatrix34ApplyP(&gOrigin_map, &gPlayer_car_master_actor->t.t.translate.t, &gCurrent_race.map_transformation);
    gOrigin_headup_map.v[0] = gOrigin_map.v[0] - (float)(gINT_0074ab94 / 2);
    gOrigin_headup_map.v[1] = gOrigin_map.v[1] - (float)(gINT_0074abec / 2);
    gHeadup_map_src_x = (int)(gOrigin_headup_map.v[0] + 0.5);
    gHeadup_map_src_y = (int)(gOrigin_headup_map.v[1] + 0.5);
    if (gHeadup_map_src_x < 0 || gHeadup_map_src_y < 0
        || gHeadup_map_src_x > gCurrent_race.map_image->width
        || gHeadup_map_src_y > gCurrent_race.map_image->height) {
        BrPixelmapFill(gMap_overlay, 0);
    }
    if (gINT_0074ab94 > gINT_0074abec) {
        gMap_overlay_offset_y = (gINT_0074ab94 - gINT_0074abec) / 2;
        gMap_overlay_offset_x = 0;
        BrPixelmapRectangleCopy(gMap_overlay,
            0,
            0,
            gCurrent_race.map_image,
            gHeadup_map_src_x,
            gHeadup_map_src_y - gMap_overlay_offset_y,
            gINT_0074ab94,
            gINT_0074ab94);
    } else {
        gMap_overlay_offset_x = (gINT_0074abec - gINT_0074ab94) / 2;
        gMap_overlay_offset_y = 0;
        BrPixelmapRectangleCopy(gMap_overlay,
            0,
            0,
            gCurrent_race.map_image,
            gHeadup_map_src_x - gMap_overlay_offset_x,
            gHeadup_map_src_y,
            gINT_0074abec,
            gINT_0074abec);
    }
    gCurrent_race.map_transformation.m[3][0] -= gOrigin_headup_map.v[0] - (float)gMap_overlay_offset_x;
    gCurrent_race.map_transformation.m[3][1] -= gOrigin_headup_map.v[1] - (float)gMap_overlay_offset_y;
    BrMatrix34Mul(&gCar_in_map_space, &gProgram_state.current_car.car_master_actor->t.t.mat, &gCurrent_race.map_transformation);
    mat23.m[0][0] = gCar_in_map_space.m[0][0];
    mat23.m[0][1] = gCar_in_map_space.m[0][1];
    mat23.m[1][0] = gCar_in_map_space.m[2][0];
    mat23.m[1][1] = gCar_in_map_space.m[2][1];
    mat23.m[2][0] = 0.0f;
    mat23.m[2][1] = 0.0f;
    BrMatrix23LPNormalise(&gMatrix23_0068c880, &mat23);
    DoMapOverlays(gMap_overlay);
    gCurrent_race.map_transformation.m[3][0] += gOrigin_headup_map.v[0] - (float)gMap_overlay_offset_x;
    gCurrent_race.map_transformation.m[3][1] += gOrigin_headup_map.v[1] - (float)gMap_overlay_offset_y;
    the_time = PDGetTotalTime();
#ifdef REC2_HW
    PossibleLock(1);
#endif
    if (gMini_map_visible && gMap_overlay != NULL) {
        DRPixelmapRotatedAndFeatheredCopy(&gMatrix23_0068c880,
            gBack_screen,
            gHeadup_map_x + 8,
            gHeadup_map_y,
            gMap_overlay,
            gMap_overlay_offset_x + gINT_0074ab8c,
            gMap_overlay_offset_y + gINT_0074ab88,
            gHeadup_map_w,
            gHeadup_map_h,
            gMap_trans);
    }
    InitMap();
    original_base_x = gMini_map->base_x;
    gMini_map->base_x = gMini_map->base_x / 2;
    DoDirectionFinderStuff(gMini_map);
    gMini_map->base_x = original_base_x;
    if (gMini_map_visible) {
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {
            DoSomeThingsToCheckpoints(gMini_map, the_time, DrawOffsetCheckpoint, 1);
        }
        if (gMini_map->type == BR_PMT_INDEX_8) {
            gMini_map->base_x = 0;
        } else {
            gMini_map->base_x /= 2;
        }
        player_pos.v[0] = (float)(gHeadup_map_w / 2 + gHeadup_map_x + 8);
        player_pos.v[1] = (float)(gHeadup_map_h / 2 + gHeadup_map_y);
        player_pos.v[2] = gMini_map_arrow_z;
        DrawArrow(gBack_screen, 0, &player_pos, CarArrowColour(&gProgram_state.current_car, eVehicle_self));
    }
    DoNearestOpponentRelativeHeightMarkerStuff(gMini_map);
    gMini_map->base_x = original_base_x;
#ifdef REC2_HW
    PossibleUnlock(1);
#endif
    if (gHeadup_detail_level > 2) {
        DoOpponentStatusHeadup();
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL InitNearestCar(void) {

    gNearest_smash_blip_index = -1;
    if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy) {
        gTarget_lock_car_2 = NULL;
    } else if (gPrevious_opponent_status == eOpponent_status_Wasted && gTarget_lock_car_1 != NULL) {
        if (PDGetTotalTime() - gTime_oppobar_target_wasted <= 2500) {
            gFLOAT_0074ab90 = 0.0f;
        } else {
            gTarget_lock_car_2 = NULL;
            gTarget_lock_car_1 = NULL;
            if (gTarget_lock_enabled) {
                gTarget_lock_enabled = 0;
            }
        }
    } else if (gTarget_lock_enabled) {
        gFLOAT_0074ab90 = 0.0f;
    } else {
        gTarget_lock_car_2 = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00496be0
// FUNCTION: CARMA2_SW 0x00493a80
// FUNCTION: CARMA2_CHEAT 0x00496f90
void C2_HOOK_FASTCALL MapStuffBeforeRender(void) {

    InitNearestCar();
    if (gCurrent_race.map_image != NULL && gMap_view == 2) {
        StartMap();
        CopyMapToScreen();
    }
}

// FUNCTION: CARMA2_HW 0x00496d80
// FUNCTION: CARMA2_SW 0x00493bf0
// FUNCTION: CARMA2_CHEAT 0x00497130
void C2_HOOK_FASTCALL MapStuffAfterRender(void) {
    if (gCurrent_race.map_image == NULL) {
        return;
    }
    if (gMap_view == 2) {
        BrVector2Set(&gOrigin_headup_map, 0.f, 0.f);
        DoMapOverlays(gBack_screen);
        FinishMap();
    } else if (gMap_view == 1) {
        if (!gAction_replay_mode
                && !(gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox)) {
            MapOverlay();
        }
    }
}

static void C2_HOOK_FASTCALL ReadMoodMessages(void) {
    tPath_name path;
    FILE *f;
    int mood;
    char s[256];

    PathCat(path, gApplication_path, "STATUS.TXT");
    f = PFfopen(path, "rt");
    if (f != NULL) {
        for (mood = 0; mood < (int)REC2_ASIZE(gOppo_status_messages); mood += 1) {
            int i;

            C2_HOOK_ASSERT(mood < REC2_ASIZE(gOppo_status_messages));

            gOppo_status_messages[mood].count = GetAnInt(f);
            C2_HOOK_ASSERT(gOppo_status_messages[mood].count < REC2_ASIZE(gOppo_status_messages[mood].messages));
            for (i = 0; i < gOppo_status_messages[mood].count; i++) {


                GetALineAndDontArgue(f, s);
                gOppo_status_messages[mood].messages[i] = (char *) BrMemAllocate(strlen(s) + 1, kMem_misc_string);
                strcpy(gOppo_status_messages[mood].messages[i], s);
            }
        }
        PFfclose(f);
    } else {
        FatalError(kFatalError_CantOpen_S, "STATUS.TXT");
    }
}

// FUNCTION: CARMA2_HW 0x00496e30
// FUNCTION: CARMA2_SW 0x00493ca0
// FUNCTION: CARMA2_CHEAT 0x004971e0
void C2_HOOK_FASTCALL InitMap(void) {

    if (!gInitMap_done) {
        gHeadup_map_x_float = (float)gHeadup_map_x;
        gHeadup_map_y_float = (float)gHeadup_map_y;
        gHeadup_map_w_float = (float)gHeadup_map_w;
        gHeadup_map_h_float = (float)gHeadup_map_h;
    } else {
        gHeadup_map_x = ((int)gHeadup_map_x_float + 3) & (tU16)~3;
        gHeadup_map_y = (int)gHeadup_map_y_float;
        gHeadup_map_w = ((int)gHeadup_map_w_float + 3) & (tU16)~3;
        gHeadup_map_h = (int)gHeadup_map_h_float;
    }
    if (gHeadup_map_x % 4 != 0) {
        gHeadup_map_x -= gHeadup_map_x % 4;
        gHeadup_map_x_float = (float)gHeadup_map_x;
    }
    if (gHeadup_map_w % 4 != 0) {
        gHeadup_map_w -= gHeadup_map_w % 4;
        gHeadup_map_w_float = (float)gHeadup_map_w;
    }
    if (gHeadup_map_w > 64) {
        gHeadup_map_w = 64;
        gHeadup_map_w_float = 64.0f;
    }
    if (gHeadup_map_h > 64) {
        gHeadup_map_h = 64;
        gHeadup_map_h_float = 64.0f;
    }
    gINT_0074ab8c = (int)(3.0 + 0.3 * gHeadup_map_w) & (tU16)~3;
    gINT_0074ab88 = (int)(3.0 + 0.3 * gHeadup_map_h) & (tU16)~3;
    if (gINT_0074ab8c > gINT_0074ab88) {
        gINT_0074ab88 = gINT_0074ab8c;
    } else {
        gINT_0074ab8c = gINT_0074ab88;
    }
    gINT_0074ab94 = gHeadup_map_w + 2 * gINT_0074ab8c;
    gINT_0074abec = gHeadup_map_h + 2 * gINT_0074ab88;
    gHeadup_map_half_width = gHeadup_map_w / 2;
    gHeadup_map_half_height = gHeadup_map_h / 2;
#ifdef REC2_SW
    if (gMap_overlay != NULL) {
        BrPixelmapFree(gMap_overlay);
    }
#endif
    if (gMap_overlay == NULL) {
        gMap_overlay = DRPixelmapAllocate(gBack_screen->type, MAX(gINT_0074ab94, gINT_0074abec), MAX(gINT_0074ab94, gINT_0074abec), NULL, 0);
    }
    gMini_map_x = gHeadup_map_x + gHeadup_map_w / 16;
    gMini_map_y = gHeadup_map_y + gHeadup_map_h / 16;
    if (gMini_map_x % 2 != 0) {
        gMini_map_x -= 1;
    }
    gINT_0074abd4 = gHeadup_map_w / 2 - gMini_map_x + gHeadup_map_x;
    gINT_0074abd0 = gHeadup_map_h / 2 - gMini_map_y + gHeadup_map_y;
    if (gMini_map != NULL) {
        BrPixelmapFree(gMini_map);
    }
#ifdef REC2_HW
    gINT_00655f04 = gHeadup_map_x;
    gINT_00655f08 = gHeadup_map_y;
#endif
    gMini_map = DRPixelmapAllocateSub(gBack_screen,
        gMini_map_x,
        gMini_map_y,
        gHeadup_map_w - (gHeadup_map_w + 8) / 8 + 8,
        gHeadup_map_h - gHeadup_map_h / 8);
    gMini_map->pixels = (br_uint_8*)gMini_map->pixels + gMini_map_y * gMini_map->row_bytes + gMini_map_x;
    gMini_map->base_x = gMini_map_x;
    gMini_map->base_y = 0;
    gFLOAT_0068d8a0 = (float)gINT_0074abd4;
    gFLOAT_0068d8a4 = (float)gINT_0074abd0;
    gMini_map_arrow_z = 0.0f;
    gFLOAT_0074ab9c = (float)((double)(gHeadup_map_half_height + gHeadup_map_half_width) * 0.45);
    gFLOAT_0074ab9c = REC2_SQR(gFLOAT_0074ab9c);
    gFLOAT_0074abb8 = (float)REC2_SQR(gCurrent_graf_data->field_0x51c + 1);
    if (!gInitMap_done) {
        gCheckpoint_numbers = LoadPixelmap("CPNUMB.PIX");
        if (gCheckpoint_numbers == NULL) {
            FatalError(kFatalError_CantFindFile_S, "CPNUMB.PIX");
        }
        BRPM_convert(gCheckpoint_numbers, gBack_screen->type);
        gSmashy_dot = LoadPixelmap("SMASHY.PIX");
        if (gSmashy_dot == NULL) {
            FatalError(kFatalError_CantFindFile_S, "SMASHY.PIX");
        }
        BRPM_convert(gSmashy_dot, gBack_screen->type);
        gAlt_meter = LoadPixelmap("ALTMETER.PIX");
        if (gAlt_meter == NULL) {
            FatalError(kFatalError_CantFindFile_S, "ALTMETER.PIX");
        }
        gCheckpoint_digit_height = gCheckpoint_numbers->height / 10;
        gCheckpoint_digit_center_y = gCheckpoint_digit_height / 2;
        gCheckpoint_digit_center_x = gCheckpoint_numbers->width / 2;
#ifdef REC2_SW
        gIcons_low_pix = LoadPixelmap("CARICONSLO.pix");
        if (gIcons_low_pix != NULL) {
            BrMapAdd(gIcons_low_pix);
        }
#endif
        gIcons_pix = LoadPixelmap("CARICONS.PIX");
        if (gIcons_pix != NULL) {
            BRPM_convert(gIcons_pix, gBack_screen->type);
            BrMapAdd(gIcons_pix);
        }
        ReadMoodMessages();
    }
    gInitMap_done = 1;
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x004973b0
void C2_HOOK_FASTCALL ConvertCarIcons(br_pixelmap* pMap) {
    int i;
    int j;
    int count_car_icons;
    int icon_width;
    int icon_height;
    int texture_x;
    int texture_y;
    tCar_icon* icon;
    int car_icon_height;
    br_model* model;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_icons) != 128);
    memset(gCar_icons, 0, sizeof(gCar_icons));
    car_icon_height = gCurrent_graf_data->car_icon_height;
    count_car_icons = gIcons_pix->height / car_icon_height;

    icon_width = NearestPowerOfTwo(pMap->width);
    icon_height = NearestPowerOfTwo(car_icon_height);

    texture_x = 0;
    texture_y = 0;
    for (i = 0; i < count_car_icons; i++) {
        icon = &gCar_icons[i];

        if (gTexture_maps[gSize_font_texture_pages] == NULL) {
            gTexture_maps[gSize_font_texture_pages] = BrPixelmapAllocate(pMap->type, 64, 64, NULL, 0);
            if (gTexture_maps[gSize_font_texture_pages] == NULL) {
                FatalError(kFatalError_CouldNotCreateTexturesPages_S, "Car Icons");
            }
            BrMapAdd(gTexture_maps[gSize_font_texture_pages]);
        }
        DRPixelmapRectangleCopy(
            gTexture_maps[gSize_font_texture_pages],
            texture_x,
            texture_y,
            pMap,
            0,
            car_icon_height * i,
            icon_width,
            car_icon_height);
        BrMapUpdate(gTexture_maps[gSize_font_texture_pages], BR_MAPU_ALL);
        icon->index = gSize_font_texture_pages;
        icon->model = model = CreateCharacterModel(icon_width, icon_height, texture_x, texture_y, "CAR ICONS");
        for (j = 0; j < 4; j++) {
            model->vertices[j].red = 0xff;
            model->vertices[j].grn = 0xff;
            model->vertices[j].blu = 0xff;
        }
        model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
        BrModelUpdate(model, BR_MODU_ALL);
        texture_x += icon_width;
        if (texture_x >= 64) {
            texture_x = 0;
            texture_y += icon_height;
            if (texture_y >= 64) {
                gSize_font_texture_pages++;
                if (gSize_font_texture_pages >= (int)REC2_ASIZE(gTexture_maps)) {
                    FatalError(kFatalError_CouldNotCreateTexturesPages_S, "CAR ICONS");
                }
                texture_x = 0;
                texture_y = 0;
            }
        }
    }
    gSize_font_texture_pages++;
    gCar_icons_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gCar_icons_model_actor->material = BrMaterialAllocate("Car Icon Material");
    gCar_icons_model_actor->material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(gCar_icons_model_actor->material);
}
#endif

// FUNCTION: CARMA2_HW 0x00497610
// FUNCTION: CARMA2_SW 0x00494240
// FUNCTION: CARMA2_CHEAT 0x004979c0
void C2_HOOK_FASTCALL MapChanged(void) {

    InitMap();
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00497620
// FUNCTION: CARMA2_SW 0x00494250
// FUNCTION: CARMA2_CHEAT 0x004979d0
void C2_HOOK_FASTCALL CheckMapMoveKeys(int pKey0, int pKey1) {

    if (KeyIsDown(31) && pKey0) {
        gHeadup_map_y_float = (float)((double)gHeadup_map_y_float - (double)gFrame_period / 20.0);
        if (gHeadup_map_y_float < 0.0f) {
            gHeadup_map_y_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(32) && pKey0) {
        gHeadup_map_y_float = (float)((double)gHeadup_map_y_float + (double)gFrame_period / 20.0);
        if (gHeadup_map_y_float + gHeadup_map_h_float > (float)gBack_screen->height) {
            gHeadup_map_y_float = (float)(gBack_screen->height - gHeadup_map_h);
        }
        MapChanged();
    }
    if (KeyIsDown(33) && pKey0) {
        gHeadup_map_x_float = (float)((double)gHeadup_map_x_float - (double)gFrame_period / 20.0);
        if (gHeadup_map_x_float < 0.0f) {
            gHeadup_map_x_float = 0.0f;
        }
        MapChanged();
    } else if (KeyIsDown(34) && pKey0) {
        gHeadup_map_x_float = (float)((double)gHeadup_map_x_float + (double)gFrame_period / 20.0);
        if (gHeadup_map_x_float + gHeadup_map_w_float + 8.0f > (float)gBack_screen->width) {
            gHeadup_map_x_float = (float)(gBack_screen->width - gHeadup_map_w - 8);
        }
        MapChanged();
    }
}

// FUNCTION: CARMA2_HW 0x004977e0
// FUNCTION: CARMA2_SW 0x00494410
// FUNCTION: CARMA2_CHEAT 0x00497b90
void C2_HOOK_FASTCALL InitSmashTargets(void) {
    gCount_smashable_race_targets = 0;
}

// FUNCTION: CARMA2_HW 0x004977f0
// FUNCTION: CARMA2_SW 0x00494420
// FUNCTION: CARMA2_CHEAT 0x00497ba0
void C2_HOOK_FASTCALL AddSmashTarget(br_model* pModel, br_actor* pActor, int pUnknown) {
    int index;

    index = gCount_smashable_race_targets;
    if (index < (int)REC2_ASIZE(gSmashable_race_targets)) {
        gSmashable_race_targets[index].model = pModel;
        gSmashable_race_targets[index].actor = pActor;
        gSmashable_race_targets[index].field_0x8 = pUnknown;
        gCount_smashable_race_targets = index + 1;
    }
}

// FUNCTION: CARMA2_HW 0x00497830
// FUNCTION: CARMA2_SW 0x00494460
int C2_HOOK_FASTCALL NearestPowerOfTwo(int pValue) {
    int power;

    for (power = 1; power < 32; power++) {
        if (pValue <= 1 << power) {
            return 1 << power;
        }
    }
    return INT_MIN;
}


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "59-camera.h"

#include "00-car.h"
#include "01-network.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "42-input.h"
#include "49-physics.h"
#include "53-controls.h"
#include "54-runtime.h"
#include "57-drone.h"
#include "61-pedestrn.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0058f5e8
// GLOBAL: CARMA2_SW 0x0058a5e8
// GLOBAL: CARMA2_CHEAT 0x00594678
int gInside_camera_zone = 1;

// GLOBAL: CARMA2_HW 0x0058f5fc
// GLOBAL: CARMA2_SW 0x0058a5fc
// GLOBAL: CARMA2_CHEAT 0x0059468c
int gNet_player_to_view_index = -1;

// GLOBAL: CARMA2_HW 0x00679278
// GLOBAL: CARMA2_SW 0x006739e8
// GLOBAL: CARMA2_CHEAT 0x00680b28
int gAction_replay_manual_camera_target_type;

// GLOBAL: CARMA2_HW 0x0067927c
// GLOBAL: CARMA2_SW 0x006739ec
// GLOBAL: CARMA2_CHEAT 0x00680b2c
br_vector3 gAction_replay_manual_camera_target_pos;

// GLOBAL: CARMA2_HW 0x00679288
// GLOBAL: CARMA2_SW 0x006739f8
// GLOBAL: CARMA2_CHEAT 0x00680b38
tCar_spec* gAction_replay_manual_camera_target_car;

// GLOBAL: CARMA2_HW 0x0067928c
// GLOBAL: CARMA2_SW 0x006739fc
// GLOBAL: CARMA2_CHEAT 0x00680b3c
tPedestrian* gAction_replay_manual_camera_target_ped;

// GLOBAL: CARMA2_HW 0x00679290
// GLOBAL: CARMA2_SW 0x00673a00
// GLOBAL: CARMA2_CHEAT 0x00680b40
br_actor* gAction_replay_manual_camera_target_actor;

// GLOBAL: CARMA2_HW 0x00679294
// GLOBAL: CARMA2_SW 0x00673a04
// GLOBAL: CARMA2_CHEAT 0x00680b44
int gCamera_mode;

// GLOBAL: CARMA2_HW 0x006792b4
// GLOBAL: CARMA2_SW 0x00673a24
// GLOBAL: CARMA2_CHEAT 0x00680b64
br_actor* gPed_actor;

// GLOBAL: CARMA2_HW 0x006792b8
// GLOBAL: CARMA2_SW 0x00673a28
// GLOBAL: CARMA2_CHEAT 0x00680b68
int gCamera_frozen;

// GLOBAL: CARMA2_HW 0x006792bc
// GLOBAL: CARMA2_SW 0x00673a2c
// GLOBAL: CARMA2_CHEAT 0x00680b6c
int gOpponent_viewing_mode;

// GLOBAL: CARMA2_HW 0x006792c0
// GLOBAL: CARMA2_SW 0x00673a30
// GLOBAL: CARMA2_CHEAT 0x00680b70
tU32 gSwitch_time;

// GLOBAL: CARMA2_HW 0x006792c8
// GLOBAL: CARMA2_SW 0x00673a38
// GLOBAL: CARMA2_CHEAT 0x00680b78
tSave_camera gSave_camera[2];

// GLOBAL: CARMA2_HW 0x006792e0
// GLOBAL: CARMA2_SW 0x00673a50
// GLOBAL: CARMA2_CHEAT 0x00680b90
br_angle gOld_yaw__car;

// GLOBAL: CARMA2_HW 0x006792e4
// GLOBAL: CARMA2_SW 0x00673a54
// GLOBAL: CARMA2_CHEAT 0x00680b94
br_angle gOld_zoom;

// GLOBAL: CARMA2_HW 0x006792e8
// GLOBAL: CARMA2_SW 0x00673a58
// GLOBAL: CARMA2_CHEAT 0x00680b98
br_vector3 gCamera_pos_before_collide;

// GLOBAL: CARMA2_HW 0x006792f4
// GLOBAL: CARMA2_SW 0x00673a64
// GLOBAL: CARMA2_CHEAT 0x00680ba4
br_angle gSwing_camera_omega;

// GLOBAL: CARMA2_HW 0x006792f8
// GLOBAL: CARMA2_SW 0x00673a68
// GLOBAL: CARMA2_CHEAT 0x00680ba8
int gSwing_camera_fast;

// GLOBAL: CARMA2_HW 0x006792fc
// GLOBAL: CARMA2_SW 0x00673a6c
// GLOBAL: CARMA2_CHEAT 0x00680bac
float gFLOAT_006792fc;

// GLOBAL: CARMA2_HW 0x007634ac
// GLOBAL: CARMA2_SW 0x00759a0c
float gCamera_cockpit_hither;

// GLOBAL: CARMA2_HW 0x0079efa8
// GLOBAL: CARMA2_SW 0x00795548
// GLOBAL: CARMA2_CHEAT 0x007a6648
tActionReplayCameraMode gAction_replay_camera_mode;

inline void C2_HOOK_FASTCALL DoCameraControls(tCamera_key_flags* pCamera_controls, tU32 pTime) {
    int flag;
    int swirl_mode;
    int up_and_down_mode;
    int going_down;
    // GLOBAL: CARMA2_HW 0x0067930c
    // GLOBAL: CARMA2_SW 0x00673a7c
    // GLOBAL: CARMA2_CHEAT 0x00680bbc
    static int last_swirl_mode;

    flag = 0;
    swirl_mode = !GetRuntimeVariable(99) && gRace_finished && !gAction_replay_mode
        && (gCar_to_view == &gProgram_state.current_car || gCar_to_view->knackered);
    up_and_down_mode = swirl_mode && !gCamera_has_collided;
    going_down = gCamera_zoom > 1.0f;
    if (last_swirl_mode != swirl_mode) {
        if (swirl_mode) {
            SaveCameraPosition(0);
        } else {
            RestoreCameraPosition(0);
        }
        last_swirl_mode = swirl_mode;
    }
    if (gMap_view != 2 && !gProgram_state.cockpit_on && (!gAction_replay_mode || gAction_replay_camera_mode <= kActionReplayCameraMode_Standard)) {
        if (pCamera_controls->field_0x0_bit2 || (up_and_down_mode && !going_down)) {
            gCamera_zoom += (float)pTime * 0.0005f / (float)(2 * swirl_mode + 1);
            if (gCamera_zoom > 2.0f) {
                gCamera_zoom = 2.0f;
            }
            if (up_and_down_mode && gCamera_zoom > 1.0f) {
                gCamera_zoom = 1.0f;
            }
        }
        if (pCamera_controls->field_0x0_bit1 || (up_and_down_mode && going_down)) {
            gCamera_zoom -= (float)pTime * 0.0005f / (float)(2 * swirl_mode + 1);
#define CAMERA_MIN_ZOOM() (gAction_replay_camera_mode == kActionReplayCameraMode_Peds ? 0.001 : 0.1)
            if (gCamera_zoom < CAMERA_MIN_ZOOM()) {
                gCamera_zoom = (br_scalar)CAMERA_MIN_ZOOM();
                if (up_and_down_mode && gCamera_zoom < 1.0f) {
                    gCamera_zoom = 1.0f;
                }
            }
#undef CAMERA_MIN_ZOOM
        }
        if (swirl_mode && gProgram_state.current_car.speedo_speed < 0.001449275362318841) {
            pCamera_controls->field_0x0_bit3 = 1;
            pCamera_controls->field_0x0_bit4 = 0;
        }
        if (gCamera_sign ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!gCamera_reset) {
                gCamera_yaw += BrDegreeToAngle((float)pTime * 0.05f);
            }
            flag = 1;
        }
        if (gCamera_sign ? pCamera_controls->field_0x0_bit4 : pCamera_controls->field_0x0_bit3) {
            if (!gCamera_reset) {
                gCamera_yaw -= BrDegreeToAngle((float)pTime * 0.05f);
            }
            if (flag) {
                gCamera_yaw = 0;
                gCamera_reset = 1;
            }
        } else if (!flag) {
            gCamera_reset = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0040ef90
// FUNCTION: CARMA2_SW 0x0040ef70
// FUNCTION: CARMA2_CHEAT 0x0040f050
void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime) {
    br_vector3* pos;
    br_vector3 old_camera_pos;
    br_vector3 tv;
    br_vector3 centre;
    br_scalar dist;
    br_scalar min_dist;
    br_scalar l;
    // GLOBAL: CARMA2_HW 0x00679310
    // GLOBAL: CARMA2_SW 0x00673a80
    // GLOBAL: CARMA2_CHEAT 0x00680bc0
    static br_scalar yaw_speed;
    // GLOBAL: CARMA2_HW 0x00679314
    // GLOBAL: CARMA2_SW 0x00673a84
    // GLOBAL: CARMA2_CHEAT 0x00680bc4
    static br_scalar zoom_speed;
    // GLOBAL: CARMA2_HW 0x00679318
    // GLOBAL: CARMA2_SW 0x00673a88
    // GLOBAL: CARMA2_CHEAT 0x00680bc8
    static br_scalar height_speed;

    if (gCamera_key_flags.field_0x0_bit3 && gCamera_key_flags.field_0x0_bit4) {
        InitialiseExternalCamera();
        NormalPositionExternalCamera(pCar, pTime);
        gAction_replay_manual_camera_target_type = 0;
    } else if (gCamera_key_flags.field_0x0_bit3) {
        yaw_speed -= 0.00025f;
    } else if (gCamera_key_flags.field_0x0_bit4) {
        yaw_speed += 0.00025f;
    } else {
        yaw_speed = 0.0f;
    }
    if (gCamera_key_flags.field_0x0_bit0) {
        zoom_speed = 0.0f;
        if (!gCamera_key_flags.field_0x0_bit1 && !gCamera_key_flags.field_0x0_bit2) {
            height_speed = 0.0f;
        } else {
            if (gCamera_key_flags.field_0x0_bit1) {
                height_speed += 0.00025f;
                if (gCamera->t.t.mat.m[3][1] > pCar->collision_info->transform_matrix.m[3][1] + 10.0f) {
                    height_speed = 0.0f;
                }
            }
            if (gCamera_key_flags.field_0x0_bit2) {
                height_speed -= 0.00025f;
            }
        }
    } else {
        height_speed = 0.0f;
        if (!gCamera_key_flags.field_0x0_bit1 && !gCamera_key_flags.field_0x0_bit2) {
            zoom_speed = 0.0f;
        } else {
            if (gCamera_key_flags.field_0x0_bit1) {
                zoom_speed += 0.00025f;
            }
            if (gCamera_key_flags.field_0x0_bit2) {
                zoom_speed -= 0.00025f;
            }
        }
    }
    BrVector3Copy(&old_camera_pos, &gCamera->t.t.translate.t);
    gCamera->t.t.mat.m[3][1] += height_speed * (float)pTime;
    min_dist = 0.2f;
    switch (gAction_replay_manual_camera_target_type) {
    case 0:
        pos = &pCar->pos;
        min_dist = pCar->collision_info->bb1.max.v[2] - pCar->collision_info->bb1.min.v[2];
        break;
    case 1:
        pos = &gAction_replay_manual_camera_target_pos;
        break;
    case 2:
        pos = &gAction_replay_manual_camera_target_pos;
        BrMatrix34ApplyP(pos, &gAction_replay_manual_camera_target_car->collision_info->cmpos,
            &gAction_replay_manual_camera_target_car->car_master_actor->t.t.mat);
        break;
    case 3:
        pos = &gAction_replay_manual_camera_target_pos;
        if (gAction_replay_manual_camera_target_ped->flags & 0x1) {
            BrVector3Add(&centre,
                &gAction_replay_manual_camera_target_ped->character->personality->bb.min,
                &gAction_replay_manual_camera_target_ped->character->personality->bb.max);
            BrVector3Scale(&centre, &centre, 0.5);
            BrMatrix34ApplyP(pos, &centre, &gAction_replay_manual_camera_target_actor->t.t.mat);
        }
        break;
    }
    BrVector3Sub(&tv, &gCamera->t.t.translate.t, pos);
    dist = BR_LENGTH2(tv.v[0], tv.v[2]);
    if (zoom_speed != 0.0f) {
        BrVector3Set(&tv, gCamera->t.t.mat.m[2][0], 0.0f, gCamera->t.t.mat.m[2][2]);
        l = zoom_speed * (float)pTime;
        BrVector3Normalise(&tv, &tv);
        if (dist - l < min_dist) {
            l = dist - min_dist;
        }
        BrVector3Scale(&tv, &tv, -l);
        BrVector3Accumulate(&gCamera->t.t.translate.t, &tv);
    }
    if (yaw_speed != 0.0f) {
        BrVector3Sub(&gCamera->t.t.translate.t, &gCamera->t.t.translate.t, pos);
        BrMatrix34PostRotateY(&gCamera->t.t.mat, BrRadianToAngle(yaw_speed * (float)pTime / dist));
        BrVector3Accumulate(&gCamera->t.t.translate.t, pos);
    }
    CollideCameraWithOtherCars(pos, &gCamera->t.t.translate.t);
    CollideCamera2(&old_camera_pos, &gCamera->t.t.translate.t, NULL, 1, pCar->collision_info);
    if (gCamera_has_collided) {
        yaw_speed = 0.0f;
        zoom_speed = 0.0f;
        height_speed = 0.0f;
        BrVector3Copy(&gCamera->t.t.translate.t, &old_camera_pos);
    }
    PointCameraAtCar(pos, &gCamera->t.t.mat, 1.0f);
}

// CheckCameraHither

// FUNCTION: CARMA2_HW 0x0040f4a0
// FUNCTION: CARMA2_SW 0x0040f480
tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void) {
    int i;
    int score;
    tCar_spec* car;

    score = gNet_players[0].last_score_index;
    car = gNet_players[0].car;
    if (gCurrent_net_game->type == eNet_game_type_foxy && gIt_or_fox >= 0 && gIt_or_fox < gNumber_of_net_players) {
        return gNet_players[gIt_or_fox].car;
    } else {
        for (i = 1; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].last_score_index < score) {
                score = gNet_players[i].last_score_index;
                car = gNet_players[i].car;
            }
        }
        return car;
    }
}

// AmIGettingBoredWatchingCameraSpin

// FUNCTION: CARMA2_HW 0x0040e430
// FUNCTION: CARMA2_SW 0x0040e410
// FUNCTION: CARMA2_CHEAT 0x0040e4f0
void C2_HOOK_FASTCALL ViewNetPlayer(void) {
    tCar_spec* car;
    tNet_game_player_info* net_player;
    tNet_message* message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, last_score_index, 0x94);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_view_car, player, 0x4);

    if (!gOpponent_viewing_mode) {
        return;
    }
    gNet_player_to_view_index++;
    if (gNet_player_to_view_index >= gNumber_of_net_players) {
        gNet_player_to_view_index = -1;
    }
    if (gNet_player_to_view_index >= 0) {
        car = gNet_players[gNet_player_to_view_index].car;
    } else {
        car = GetRaceLeader();
    }
    net_player = NetPlayerFromCar(car);
    if (gNet_mode == eNet_mode_host) {
        gCar_to_view = car;
        gCamera_yaw = 0;
        InitialiseExternalCamera();
        PositionExternalCamera(gCar_to_view, 200);
    } else if (gNet_mode == eNet_mode_client && gCar_to_view_id != net_player->ID) {
        gCar_to_view_id = net_player->ID;
        message = NetBuildGuaranteedMessage(eNet_message_chunk_type_view_car, 0);
        message->contents.view_car.player = gCar_to_view_id;
        NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
    }
}

// FUNCTION: CARMA2_HW 0x0040e590
// FUNCTION: CARMA2_SW 0x0040e570
void C2_HOOK_FASTCALL ViewOpponent(void) {
    tNet_message* message;

    // GLOBAL: CARMA2_HW 0x0058f628
    // GLOBAL: CARMA2_SW 0x0058a628
    static int n = -1;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_view_car, player, 0x4);

    n++;
    if (gNet_mode != eNet_mode_none) {
        if (n >= gNumber_of_net_players) {
            n = 0;
        }
        if (gNet_mode == eNet_mode_host) {
            gCar_to_view = gNet_players[n].car;
        } else if (gCar_to_view_id == 0) {
            gCar_to_view_id = gNet_players[n].ID;
            message = NetBuildGuaranteedMessage(eNet_message_chunk_type_view_car, 0);
            message->contents.view_car.player = gCar_to_view_id;
            NetGuaranteedSendMessageToHost(gCurrent_net_game, message, NULL);
            return;
        }
        NewTextHeadupSlot(4, 0, 2000, -3, gNet_players[n].player_name);
    } else {
        if (n >= gNum_viewable_cars) {
            n = 0;
        }
        gCar_to_view = gViewable_car_list[n];
        NewTextHeadupSlot(4, 0, 2000, -3, gViewable_car_list[n]->driver_name);
    }
    gCamera_yaw = 0;
    InitialiseExternalCamera();
    PositionExternalCamera(gCar_to_view, 200);
}

// FUNCTION: CARMA2_HW 0x0040e680
// FUNCTION: CARMA2_SW 0x0040e660
void C2_HOOK_FASTCALL ReallyViewOpponent(void) {
    int i;

    if (gNet_mode == eNet_mode_client) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == gCar_to_view_id) {
                gCar_to_view_id = 0;
                gCamera_yaw = 0;
                gCar_to_view = gNet_players[i].car;
                InitialiseExternalCamera();
                PositionExternalCamera(gCar_to_view, 200);
                return;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0040f510
// FUNCTION: CARMA2_SW 0x0040f4f0
void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar) {
    br_matrix34* mat;
    br_scalar height;
    int i;

    height = 0.f;
    if (pCar->collision_info->water_d != 10000.f) {
        mat = &pCar->car_master_actor->t.t.mat;
        for (i = 0; i < 3; i++) {
            if (mat->m[i][1] > 0.f) {
                height += pCar->collision_info->bb2.max.v[i] * mat->m[i][1];
            } else {
                height += pCar->collision_info->bb2.min.v[i] * mat->m[i][1];
            }
        }
        if ((mat->m[3][1] / WORLD_SCALE) + height < pCar->collision_info->water_d) {
            DisablePlingMaterials();
        }
    } else {
        DisablePlingMaterials();
    }
}

// SetPanningFieldOfView

int C2_HOOK_FASTCALL IsCameraTypeAllowed(int pCamera_mode) {
    if (gAction_replay_mode && !gCamera_type_allowed_replay[pCamera_mode]) {
        return 0;
    }
    if (!gAction_replay_mode && !gCamera_type_allowed_gameplay[pCamera_mode]) {
        return 0;
    }
    if (pCamera_mode >= 9) {
        return 0;
    }
    if (pCamera_mode == 6 && !OKToViewDrones()) {
        return 0;
    }
    if (pCamera_mode == 5 && gPed_count == 0) {
        return 0;
    }
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040e700
// FUNCTION: CARMA2_SW 0x0040e6e0
// FUNCTION: CARMA2_CHEAT 0x0040e7c0
void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam) {
    int i;

    for (i = 0; i < pCar->count_window_materials; i++) {
        int internal_map_missing = pInternal_cam;

        if (pInternal_cam) {
            int j;

            for (j = 0; j < pCar->window_materials[i].count_maps; j++) {
                if (pCar->window_materials[i].material->colour_map == pCar->window_materials[i].maps[j]) {
                    internal_map_missing = 0;
                }
            }
            if (internal_map_missing) {
                pCar->window_materials[i].material->flags &= ~BR_MATF_TWO_SIDED;
            }
        }
        if (!internal_map_missing) {
            pCar->window_materials[i].material->flags |= BR_MATF_TWO_SIDED;
        }
        BrMaterialUpdate(pCar->window_materials[i].material, BR_MATU_RENDERING);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040e790
// FUNCTION: CARMA2_SW 0x0040e770
void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos) {

    gAction_replay_camera_mode = pCamPos;
    switch (pCamPos) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        InitialiseExternalCamera();
        break;
    case kActionReplayCameraMode_Manual:
        gAction_replay_manual_camera_target_type = 0;
        break;
    default:
        break;
    }
    MungeCarMaterials(&gProgram_state.current_car, gAction_replay_camera_mode == kActionReplayCameraMode_Internal);
}

// FUNCTION: CARMA2_HW 0x0040e7f0
// FUNCTION: CARMA2_SW 0x0040e7d0
// FUNCTION: CARMA2_CHEAT 0x0040e8b0
void C2_HOOK_FASTCALL ChangeCameraType(void) {
    tActionReplayCameraMode mode;

    if (KeyIsDown(8)) {
        mode = gAction_replay_camera_mode;
        gAction_replay_camera_mode = mode - 1;
        if (gAction_replay_camera_mode >= 9 || gAction_replay_camera_mode < 0) {
            gAction_replay_camera_mode = 8;
        }
    } else {
        mode = gAction_replay_camera_mode;
        gAction_replay_camera_mode = mode + 1;
        if (gAction_replay_camera_mode >= 9) {
            gAction_replay_camera_mode = 0;
        }
    }
    if (!IsCameraTypeAllowed(gAction_replay_camera_mode)) {
        ChangeCameraType();
    }
    if (gAction_replay_mode) {
        gAR_camera_type = gAction_replay_camera_mode;
    } else {
        gCamera_type = gAction_replay_camera_mode;
    }
    SetCameraType(gAction_replay_camera_mode);
}

// FUNCTION: CARMA2_HW 0x0040e900
// FUNCTION: CARMA2_SW 0x0040e8e0
// FUNCTION: CARMA2_CHEAT 0x0040e9c0
void C2_HOOK_FASTCALL ToggleCockpit(void) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gCamera_type_allowed_replay) != 9);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCamera_type_allowed_gameplay) != 9);

    if ((gCar_to_view != &gProgram_state.current_car && !gProgram_state.cockpit_on) || gMap_view == 2) {
        return;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        tActionReplayCameraMode old_camera_mode = gAction_replay_camera_mode_00679324;

        if ((gAction_replay_mode && !gCamera_type_allowed_replay[gAction_replay_camera_mode_00679324])
                || (!gAction_replay_mode && !gCamera_type_allowed_gameplay[gAction_replay_camera_mode_00679324])
                || gAction_replay_camera_mode_00679324 >= (int)REC2_ASIZE(gCamera_type_allowed_replay)
                || (gAction_replay_camera_mode_00679324 == kActionReplayCameraMode_Drone && !OKToViewDrones())
                || (old_camera_mode == kActionReplayCameraMode_Peds && !gPed_count)) {
            SetCameraType(kActionReplayCameraMode_Standard);
        } else {
            SetCameraType(gAction_replay_camera_mode_00679324);
        }
    } else {
        gAction_replay_camera_mode_00679324 = gAction_replay_camera_mode;
        SetCameraType(kActionReplayCameraMode_Internal);
    }
}

// PositionCarMountedCamera

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040ea30
// STUB: CARMA2_SW 0x0040ea10
// STUB: CARMA2_CHEAT 0x0040eaf0
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040f760
// FUNCTION: CARMA2_SW 0x0040f740
// FUNCTION: CARMA2_CHEAT 0x0040f9b0
void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime) {

    if (gAction_replay_mode
            && (gAction_replay_camera_mode == kActionReplayCameraMode_ActionTracking || gAction_replay_camera_mode == kActionReplayCameraMode_Panning)
            && gPed_actor != NULL
            && !gProgram_state.cockpit_on) {
        IncidentCam(pCar, pTime);
    }
}

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040f790
// STUB: CARMA2_SW 0x0040f770
// STUB: CARMA2_CHEAT 0x0040f9e0
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// MoveCamToIncident

// FUNCTION: CARMA2_HW 0x004104b0
// FUNCTION: CARMA2_SW 0x00410490
// FUNCTION: CARMA2_CHEAT 0x00410700
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* pCar, tU32 pTime) {
    br_matrix34* m2;
    br_vector3 tv;
    br_scalar ts;

    /*
     * 102.91955471539592: REC2_SQR(70.0 / WORLD_SCALE_D)
     * 205.8391094307918: 9800.0 / REC2_SQR(WORLD_SCALE_D))
     */

    m2 = &gCamera->t.t.mat;
    BrVector3Sub(&tv, (br_vector3*)m2->m[3], &pCar->pos);
    ts = BrVector3LengthSquared(&tv);
    if (ts > 102.91955471539592 || (gSwitch_time != 0 && (ARPipeSearchForwards() ? (gSwitch_time <= GetTotalTime()) : (gSwitch_time >= GetTotalTime())))) {
        if ((gInside_camera_zone || ts > 205.83910943079184) && (ts > 25.0f || CheckForWall(&pCar->pos, &gCamera->t.t.translate.t))) {
            SetUpPanningCamera(pCar);
        }
    } else {
        gInside_camera_zone = 1;
    }
    PointCameraAtCar(&pCar->pos, &gCamera->t.t.mat, 1.0f);
}

inline int C2_HOOK_FASTCALL CheckForWall(br_vector3* pStart, br_vector3* pEnd) {
    br_vector3 dir;
    br_material* material;
    br_vector3 normal;
    br_scalar d;

    BrVector3Sub(&dir, pEnd, pStart);
    FindFace(pStart, &dir, &normal, &d, &material);
    return d <= 1.f;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004105f0
// STUB: CARMA2_SW 0x004105d0
// STUB: CARMA2_CHEAT 0x00410840
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00410be0
// FUNCTION: CARMA2_SW 0x00410bc0
// FUNCTION: CARMA2_CHEAT 0x00410e30
void C2_HOOK_FASTCALL SaveCameraPosition(int pIndex) {

    if (gSave_camera[pIndex].saved == 1) {
        return;
    }
    gSave_camera[pIndex].zoom = gCamera_zoom;
    gSave_camera[pIndex].yaw = gCamera_yaw;
    gSave_camera[pIndex].saved = 1;
}

// FUNCTION: CARMA2_HW 0x00410c20
// FUNCTION: CARMA2_SW 0x00410c00
// FUNCTION: CARMA2_CHEAT 0x00410e70
void C2_HOOK_FASTCALL RestoreCameraPosition(int pIndex) {

    if (gSave_camera[pIndex].saved == 0) {
        return;
    }
    gCamera_zoom = gSave_camera[pIndex].zoom;
    gCamera_yaw = gSave_camera[pIndex].yaw;
    gSave_camera[pIndex].saved = 0;
}

inline void C2_HOOK_FASTCALL DrVector3RotateY(br_vector3* v, br_angle t) {
    br_scalar c;
    br_scalar s;
    br_scalar ts;

    c = BR_COS(t);
    s = BR_SIN(t);
    ts = v->v[0] * c + v->v[2] * s;
    v->v[2] = v->v[2] * c - v->v[0] * s;
    v->v[0] = ts;
}

// FUNCTION: CARMA2_HW 0x00410c60
// FUNCTION: CARMA2_SW 0x00410c40
// FUNCTION: CARMA2_CHEAT 0x00410eb0
void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime) {
    br_matrix34* m1;
    br_vector3 old_camera_pos;
    br_vector3 vn;
    br_vector3 a;
    br_scalar d;
    br_scalar l;
    br_scalar dist;
    br_scalar height_inc;
    br_scalar time;
    tPhysics_object* collision_info;
    int swoop;
    int manual_swing;
    int manual_zoom;

    m1 = &gCamera->t.t.mat;
    swoop = gCountdown && pPos->v[1] + 0.001 < gCamera_height;
    BrVector3Copy(&old_camera_pos, (br_vector3*)m1->m[3]);
    if (!gProgram_state.cockpit_on) {
        DoCameraControls(&gCamera_key_flags, pTime);
        manual_swing = gCamera_key_flags.field_0x0_bit3 || gCamera_key_flags.field_0x0_bit4 || swoop;
        manual_zoom = gCamera_key_flags.field_0x0_bit1 || gCamera_key_flags.field_0x0_bit2;
        if (swoop) {
            manual_swing = 1;
            gCamera_yaw = 0;
        }
        if (fabs(pSpeedo_speed) > 0.0006 && gCamera_mode > 0) {
            gCamera_mode = -1;
            if (gAction_replay_camera_mode == kActionReplayCameraMode_Standard) {
                gCamera_sign = 0;
            } else if (BrVector3Dot((br_vector3*)pMat->m[2], pDirection) > 0.0f) {
                gCamera_sign = 1;
            } else {
                gCamera_sign = 0;
            }
        }
        if (pCar != NULL && pCar->frame_collision_flag && gCamera_mode != -2
                && (!gAction_replay_mode || ARReplayForwards())) {
            gCamera_mode = 1;
        }
        if (gCar_flying || gCamera_reset || gCamera_mode == -2
                || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
            gCamera_mode = 0;
        }
        d = (br_scalar)sqrt(gCamera_zoom) + 4.0f / WORLD_SCALE;
        if (gCamera_mode == 0 || gCamera_mode == -1) {
            if (gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                BrVector3Negate(&vn, (br_vector3*)pMat->m[2]);
            } else {
                BrVector3Copy(&vn, pDirection);
            }
            MoveWithWheels(pCar, &vn, manual_swing);
            vn.v[1] = 0.0f;
            BrVector3Normalise(&vn, &vn);
            if (gCar_flying || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                gCamera_sign = 0;
            }
            if (gAction_replay_mode && ARReplayIsReallyPaused()) {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, 0, pCar);
            } else {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, pTime, pCar);
            }
            BrVector3Scale(&a, &vn, d);
            BrVector3Sub((br_vector3*)m1->m[3], pPos, &a);
            BrVector3Copy(&gView_direction, &vn);
        } else {
            gSwing_camera_omega = 0;
            gSwing_camera_fast = 0;
        }
        if (gCamera_mode == 1) {
            BrVector3Copy(&old_camera_pos, &gCamera_pos_before_collide);
            BrVector3Sub(&a, pPos, &old_camera_pos);
            a.v[1] = 0.0f;
            if (manual_swing) {
                DrVector3RotateY(&a, (gCamera_sign == 0 ? 1 : -1) * (gCamera_yaw - gOld_yaw__car));
                gCamera_yaw = gOld_yaw__car;
            }
            BrVector3Normalise(&vn, &a);
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || manual_swing
                    || BrVector3Dot((br_vector3*)pMat->m[2], &vn) < BrVector3Dot((br_vector3*)pMat->m[2], &gView_direction)) {
                BrVector3Copy(&gView_direction, &vn);
            }
            BrVector3Scale(&vn, &gView_direction, -d);
            BrVector3Accumulate(&a, &vn);
            dist = BrVector3Length(&a);
            l = (float)pTime / 1000.0f * (dist + 1.0f) / dist;
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard
                    && l < 1.0f && BrVector3Dot(&a, &vn) > 0.0f) {
                BrVector3Scale(&a, &a, l - 1.0f);
                BrVector3Accumulate(&vn, &a);
            }
            BrVector3Add((br_vector3*)m1->m[3], pPos, &vn);
        }
        height_inc = gCamera_zoom * gCamera_zoom + 0.3f;
        time = (float)pTime * 0.001f;
        if (!gCamera_frozen && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
            if (pTime >= 5000) {
                gCamera_height = pPos->v[1];
            } else if (swoop) {
                if (time > 0.2f) {
                    time = 0.2f;
                }
                gCamera_height -= time * 5.0f;
                if (gCamera_height < pPos->v[1]) {
                    gCamera_height = pPos->v[1];
                }
            } else {
                gCamera_height += pPos->v[1] * 5.0f * time;
                gCamera_height /= 5.0f * time + 1.0f;
            }
        }
        l = pDirection->v[1] * d;
        if (l > 0.0f) {
            l = pPos->v[1] - l - height_inc / 2.0f;
            if (l > gCamera_height) {
                gCamera_height = l;
            }
        }
        m1->m[3][1] = gCamera_height + height_inc;
        BrVector3Copy(&gCamera_pos_before_collide, (br_vector3*)m1->m[3]);
        CollideCameraWithOtherCars(pPos, (br_vector3*)m1->m[3]);
        if (pCar != NULL) {
            collision_info = pCar->collision_info;
        } else {
            collision_info = NULL;
        }
        CollideCamera2(pPos, (br_vector3*)m1->m[3], &old_camera_pos, manual_swing || manual_zoom, collision_info);
        if (gCamera_has_collided && swoop) {
            gCamera_height = pPos->v[1];
        }
        PointCameraAtCar(pPos, m1, 1.0f);
    }
    gOld_yaw__car = gCamera_yaw;
    gOld_zoom = (br_angle)gCamera_zoom;
}

inline void C2_HOOK_FASTCALL NormalPositionExternalCamera(tCar_spec* pCar, tU32 pTime) {

    GeneralisedPositionExternalCamera(pCar, &pCar->car_master_actor->t.t.mat, &pCar->pos,
        pCar->speed, pCar->speedo_speed, &pCar->direction, &pCar->collision_info->omega, pTime);
}

// PositionPedCam

// PositionDroneCam

inline void C2_HOOK_FASTCALL MoveWithWheels(tCar_spec* pCar, br_vector3* pDir, int pManual_swing) {
    br_angle yaw;
    br_angle theta;
    // GLOBAL: CARMA2_HW 0x0058f638
    // GLOBAL: CARMA2_SW 0x0058a638
    // GLOBAL: CARMA2_CHEAT 0x005946c8
    static int move_with_wheels = 1;

    if (pCar != NULL && pCar->speed <= 0.0001f && gCamera_mode == 0) {
        if (pManual_swing) {
            if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                yaw = gCamera_yaw - BR_ANGLE_DEG(180);
            } else {
                yaw = gCamera_yaw;
            }
            if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                if (move_with_wheels) {
                    theta = BrRadianToAngle(atan2(pCar->wpos[0].v[2] * pCar->curvature, 1.0));
                    gCamera_yaw += (1 - 2 * gCamera_sign) * theta;
                    move_with_wheels = 0;
                }
            } else if (!move_with_wheels) {
                theta = BrRadianToAngle(atan2(pCar->wpos[0].v[2] * pCar->curvature, 1.0));
                gCamera_yaw -= (1 - 2 * gCamera_sign) * theta;
                move_with_wheels = 1;
            }
        }
        if (move_with_wheels && !gCar_flying && gAction_replay_camera_mode != kActionReplayCameraMode_Rigid) {
            theta = BrRadianToAngle(atan2(pCar->wpos[0].v[2] * pCar->curvature, 1.0));
            DrVector3RotateY(pDir, theta);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00411980
// FUNCTION: CARMA2_SW 0x00411960
// FUNCTION: CARMA2_CHEAT 0x00411bd0
void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime, tCar_spec* pCar) {
    br_scalar ts;
    br_scalar sign;
    br_scalar sin_dtheta;
    br_scalar cos_dtheta;
    br_scalar omega_y;
    br_scalar abs_omega_y;
    br_angle yaw;
    br_angle theta;
    br_angle alpha;
    br_angle omega;
    int manual_swing;
    // GLOBAL: CARMA2_HW 0x0058f63c
    // GLOBAL: CARMA2_SW 0x0058a63c
    // GLOBAL: CARMA2_CHEAT 0x005946cc
    static int elapsed_time = -1;
    // GLOBAL: CARMA2_HW 0x00679338
    // GLOBAL: CARMA2_SW 0x00673aa8
    // GLOBAL: CARMA2_CHEAT 0x00680be8
    static br_vector3 old_vn;

    manual_swing = gOld_yaw__car != gCamera_yaw;
    if (elapsed_time > 500) {
        elapsed_time = -1;
    }
    if (elapsed_time >= 0) {
        elapsed_time += pTime;
    }
    ts = -BrVector3Dot((br_vector3*)pM1->m[2], pVn);
    sign = BrVector3Dot(pVn, &old_vn);
    if ((ts < 0.0f) != gCamera_sign) {
        if (sign > 0.0f && elapsed_time < 0) {
            gCamera_sign = gCamera_sign == 0;
            if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                yaw = gCamera_yaw - BR_ANGLE_DEG(180);
            } else {
                yaw = gCamera_yaw;
            }
            if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                gCamera_yaw = -gCamera_yaw;
            }
        } else if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || gCamera_sign) {
            if (elapsed_time < 0) {
                elapsed_time = 0;
            }
            if (elapsed_time >= 500 || ts > 0.0f) {
                elapsed_time = 500;
                if (fabs(pSpeedo_speed) > (ts > 0.0f ? 0.0001 : 0.0006) && !gCar_flying) {
                    gCamera_sign = gCamera_sign == 0;
                    gSwing_camera_omega = BrDegreeToAngle((gSwing_camera_fast ? 0.06 : 0.04) * 200.0);
                    if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                        yaw = gCamera_yaw - BR_ANGLE_DEG(180);
                    } else {
                        yaw = gCamera_yaw;
                    }
                    theta = gCamera_yaw + BR_ANGLE_DEG(90);
                    if (theta > BR_ANGLE_DEG(180)) {
                        gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                    } else if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                        gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                    }
                } else {
                    BrVector3Negate(pVn, pVn);
                }
            } else {
                BrVector3Negate(pVn, pVn);
            }
        } else {
            BrVector3Negate(pVn, pVn);
        }
    } else {
        elapsed_time = -1;
    }
    BrVector3Copy(&old_vn, pVn);
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    if (!gCar_flying) {
        DrVector3RotateY(pVn, yaw);
    }
    cos_dtheta = pVn->v[0] * gView_direction.v[0] + pVn->v[2] * gView_direction.v[2];
    sin_dtheta = pVn->v[0] * gView_direction.v[2] - pVn->v[2] * gView_direction.v[0];
    if (cos_dtheta < 0.5 && gCamera_yaw == 0) {
        gSwing_camera_fast = 1;
    }
    omega_y = pOmega->v[0] * pM1->m[0][1] + pOmega->v[1] * pM1->m[1][1] + pOmega->v[2] * pM1->m[2][1];
    abs_omega_y = fabs(omega_y);
    alpha = BrRadianToAngle((float)pTime * (abs_omega_y + 0.08726646f) / 1000.0f);
    if (!(gSwing_camera_omega == 0 && cos_dtheta > 0.0f && BrRadianToAngle(asin((float)fabs(sin_dtheta))) < alpha) && !gCar_flying && !manual_swing) {
        if (gSwing_camera_omega == 0) {
            gSwing_camera_omega = BrDegreeToAngle((gSwing_camera_fast ? 0.06 : 0.04) * 50.0);
        }
        omega = gSwing_camera_omega * pTime / 100;
        if (omega < alpha) {
            omega = alpha;
        }
        if (cos(BrAngleToRadian(omega)) > cos_dtheta) {
            if (cos_dtheta < -0.7 && abs_omega_y > 0.8 && omega_y * sin_dtheta > 0.0f) {
                omega = -omega;
            }
            if (sin_dtheta > 0.0f) {
                pVn->v[0] = sin(BrAngleToRadian(omega)) * gView_direction.v[2] + cos(BrAngleToRadian(omega)) * gView_direction.v[0];
                pVn->v[2] = cos(BrAngleToRadian(omega)) * gView_direction.v[2] - sin(BrAngleToRadian(omega)) * gView_direction.v[0];
            } else {
                pVn->v[0] = cos(BrAngleToRadian(omega)) * gView_direction.v[0] - sin(BrAngleToRadian(omega)) * gView_direction.v[2];
                pVn->v[2] = sin(BrAngleToRadian(omega)) * gView_direction.v[0] + cos(BrAngleToRadian(omega)) * gView_direction.v[2];
            }
            gSwing_camera_omega += BrDegreeToAngle((int)pTime * (gSwing_camera_fast ? 0.06 : 0.04));
            if (gSwing_camera_omega > BrDegreeToAngle(gSwing_camera_fast ? 40.0 : 10.0)) {
                gSwing_camera_omega = BrDegreeToAngle(gSwing_camera_fast ? 40.0 : 10.0);
            }
            return;
        }
    }
    gSwing_camera_omega = 0;
    gCamera_mode = 0;
    gSwing_camera_fast = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00411fc0
// FUNCTION: CARMA2_SW 0x00411fa0
// FUNCTION: CARMA2_CHEAT 0x00412210
void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor) {
    br_vector3 vn;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar dist;
    br_scalar frac;
    br_angle off_centre_angle;
    br_vector3* pos;
    br_camera* camera_ptr;
    int swoop;

    camera_ptr = gCamera->type_data;
    off_centre_angle = (br_angle)(pFov_factor / 5.0f * (float)camera_ptr->field_of_view);
    swoop = gCountdown && gCamera_height > pPos->v[1] + 0.01f;
    if (swoop) {
        BrVector3Sub(&tv, &gAverage_grid_position, pPos);
        frac = (gCamera_height - pPos->v[1]) / 10.0f;
        BrVector3Scale(&tv, &tv, frac);
        BrVector3Accumulate(&tv, pPos);
        pos = &tv;
        off_centre_angle = (br_angle)((1.0f - frac) * (float)off_centre_angle);
    } else {
        pos = pPos;
    }
    BrVector3Sub(&vn, pPos, (br_vector3*)pMat->m[3]);
    vn.v[1] = 0.0f;
    BrVector3Normalise(&vn, &vn);
    pMat->m[0][0] = -vn.v[2];
    pMat->m[0][1] = 0.0f;
    pMat->m[0][2] = vn.v[0];
    pMat->m[1][0] = 0.0f;
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][0] = -vn.v[0];
    pMat->m[2][1] = 0.0f;
    pMat->m[2][2] = -vn.v[2];
    BrVector3Sub(&tv2, pos, (br_vector3*)pMat->m[3]);
    dist = BrVector3Dot(&tv2, &vn);
    BrMatrix34PreRotateX(pMat, off_centre_angle - BrRadianToAngle(atan2f(pMat->m[3][1] - pos->v[1], dist)));
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004121b0
// FUNCTION: CARMA2_SW 0x00412190
void C2_HOOK_FASTCALL PointCamera(const br_vector3* pPos, br_matrix34* pMat) {
    br_vector3 vn;
    br_scalar dist;
    br_angle theta;
    br_camera* camera_ptr;

    camera_ptr = gCamera->type_data;
    BrVector3Sub(&vn, pPos, (br_vector3*)pMat->m[3]);
    vn.v[1] = 0.0f;
    BrVector3Normalise(&vn, &vn);
    pMat->m[0][0] = -vn.v[2];
    pMat->m[0][1] = 0.0f;
    pMat->m[0][2] = vn.v[0];
    pMat->m[1][0] = 0.0f;
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][0] = -vn.v[0];
    pMat->m[2][1] = 0.0f;
    pMat->m[2][2] = -vn.v[2];
    dist = BR_LENGTH2(pMat->m[3][0] - pPos->v[0], pMat->m[3][2] - pPos->v[2]);
    theta = BR_ATAN2(pMat->m[3][1] - pPos->v[1], dist);
    BrMatrix34PreRotateX(pMat, camera_ptr->field_of_view / 5 - theta);
}

// FUNCTION: CARMA2_HW 0x004122b0
// FUNCTION: CARMA2_SW 0x00412290
// FUNCTION: CARMA2_CHEAT 0x00412500
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* pCar_pos, br_vector3* pCam_pos, br_vector3* pOld_camera_pos, int pManual_move, tPhysics_object* pCollision_info) {
    int i;
    int j;
    int k;
    int count;
    int count_min_dots;
    int loop_done;
    int index_min_dots[3];
    br_scalar hither;
    br_scalar dist;
    br_scalar ts;
    br_scalar t;
    br_scalar d;
    br_scalar cos_coll;
    br_scalar delta_hither;
    br_scalar min_dot;
    br_scalar a;
    br_scalar b;
    br_scalar discr;
    br_scalar dot;
    br_vector3 delta;
    br_vector3 dir;
    br_vector3 nor;
    br_vector3 new_cam_pos;
    br_vector3 cam_hither_pos;
    br_vector3 car_coll_pos;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 tv;
    br_vector3 tv2;
    br_vector3 temp_normal;
    br_material* material;
    br_actor* actor;
    tBounds bnds;
    br_matrix34 mat;
    tFace_ref face_list[6];
    br_scalar face_dots[6];

    hither = ((br_camera*)gCamera->type_data)->hither_z * 3.0f;
    gCamera_has_collided = 0;
    loop_done = 0;
    for (;;) {
        BrVector3Sub(&delta, pCam_pos, pCar_pos);
        dist = BrVector3Length(&delta);
        BrVector3Scale(&delta, &delta, 1.25f);
        ActorFindFace(pCar_pos, &delta, gTrack_actor, &nor, &ts, &material, &actor);
        if (ts < 1.0f) {
            gCamera_has_collided = 1;
            BrVector3Scale(&tv, &delta, ts);
            BrVector3Add(&new_cam_pos, pCar_pos, &tv);
        } else {
            BrVector3Copy(&new_cam_pos, pCam_pos);
        }
        BrVector3Normalise(&dir, &delta);
        BrVector3Scale(&tv, &dir, -hither);
        BrVector3Add(&cam_hither_pos, &new_cam_pos, &tv);
        BrVector3Add(&car_coll_pos, pCar_pos, &delta);
        for (i = 0; i < gNum_active_non_cars; i++) {
            if (gActive_non_car_list[i]->flags & 0x80000) {
                DrMatrix34ApplyLPInverse(&p1, &cam_hither_pos, &gActive_non_car_list[i]->collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &gActive_non_car_list[i]->collision_info->actor->t.t.mat);
                if (ShapeRayCast(&p1, &p2, gActive_non_car_list[i]->collision_info->shape, &new_cam_pos, &t, &temp_normal)) {
                    gCamera_has_collided = 1;
                    DrMatrix34ApplyLPInverse(&p1, pCar_pos, &gActive_non_car_list[i]->collision_info->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &gActive_non_car_list[i]->collision_info->actor->t.t.mat);
                    if (ShapeRayCast(&p1, &p2, gActive_non_car_list[i]->collision_info->shape, &new_cam_pos, &t, &temp_normal) && t < ts) {
                        ts = t;
                        BrVector3Copy(&nor, &temp_normal);
                        actor = NULL;
                        BrMatrix34ApplyP(&tv, &new_cam_pos, &gActive_non_car_list[i]->collision_info->actor->t.t.mat);
                        BrVector3Copy(&new_cam_pos, &tv);
                    }
                }
            }
        }
        if (ts > 1.0f) {
            break;
        }
        cos_coll = BrVector3Dot(&nor, &delta);
        if (cos_coll > 0.0f) {
            BrVector3Negate(&nor, &nor);
        } else {
            cos_coll = -cos_coll;
        }
        if (loop_done) {
            break;
        }
        delta_hither = cos_coll / 1.25 + hither - cos_coll * ts;
        if (delta_hither <= 0.0f) {
            break;
        }
        if (nor.v[1] < -0.7) {
            BrVector3Set(&tv, nor.v[0], nor.v[1] - 3.0f, nor.v[2]);
        } else {
            BrVector3Set(&tv, nor.v[0], nor.v[1] + 3.0f, nor.v[2]);
        }
        BrVector3Scale(&tv, &tv, delta_hither / BrVector3Length(&tv) * (br_scalar)fabs(nor.v[1]));
        BrVector3Accumulate(pCam_pos, &tv);
        BrVector3Sub(&delta, pCam_pos, pCar_pos);
        BrVector3Normalise(&delta, &delta);
        loop_done = 1;
        BrVector3Scale(&tv, &delta, dist);
        BrVector3Add(pCam_pos, pCar_pos, &tv);
    }
    if (ts <= 1.0f) {
        gCamera_has_collided = 1;
        if (actor != NULL && actor->identifier != NULL && actor->identifier[0] == '&') {
            BrMatrix34ApplyV(&tv, &nor, &actor->t.t.mat);
            BrVector3Copy(&nor, &tv);
        }
        d = -(hither / BrVector3Dot(&delta, &nor));
        ts -= d;
        if (ts * dist < 1.0f) {
            ts = MIN(ts + d - 0.001, 1.0f / dist);
        }
        if (ts > 0.8) {
            ts = 0.8f;
        }
        BrVector3Scale(&delta, &delta, ts);
        dist = BrVector3Length(&delta);
        BrVector3Add(pCam_pos, pCar_pos, &delta);
        if (dist < gMin_camera_car_distance && !loop_done) {
            BrVector3Scale(&tv, &nor, -nor.v[1]);
            tv.v[1] += 1.0f;
            if (gProgram_state.current_car.car_master_actor->t.t.mat.m[1][1] < 0.0f) {
                BrVector3Negate(&tv, &tv);
            }
            a = BrVector3LengthSquared(&tv);
            b = BrVector3Dot(&tv, &delta);
            discr = b * b - 4.0f * (a * (BrVector3LengthSquared(&delta) - REC2_SQR(gMin_camera_car_distance)));
            if (discr >= 0.0f && a != 0.0f) {
                BrVector3Scale(&tv, &tv, (-b + (br_scalar)sqrt(discr)) / (2.0 * a));
                FindFace(pCam_pos, &tv, &nor, &ts, &material);
                if (ts < 1.0f) {
                    BrVector3Scale(&tv, &tv, ts);
                }
                BrVector3Set(&tv2, delta.v[0], 0.0f, delta.v[2]);
                BrVector3Normalise(&tv2, &tv2);
                BrVector3Accumulate(&delta, &tv);
                dot = BrVector3Dot(&delta, &tv2);
                if (dot < 0.03f && !gAction_replay_mode) {
                    BrVector3Normalise(&tv, &tv);
                    d = BrVector3Dot(&tv, &tv2);
                    if (d < -0.03f) {
                        BrVector3Scale(&tv, &tv, (0.03f - dot) / d);
                        BrVector3Accumulate(&delta, &tv);
                    }
                }
            }
            BrVector3Add(pCam_pos, pCar_pos, &delta);
        }
    }
    bnds.mat = &mat;
    BrMatrix34Identity(bnds.mat);
    BrVector3Set(&tv, hither, hither, hither);
    BrVector3Sub(&bnds.original_bounds.min, pCam_pos, &tv);
    BrVector3Add(&bnds.original_bounds.max, pCam_pos, &tv);
    count = FindFacesInBox(&bnds, face_list, REC2_ASIZE(face_list), NULL);
    for (i = 0; i < count; i++) {
        if ((face_list[i].material->flags & BR_MATF_TWO_SIDED) && BrVector3Dot(&face_list[i].normal, &delta) > 0.0f) {
            BrVector3Negate(&face_list[i].normal, &face_list[i].normal);
        }
        BrVector3Sub(&tv, pCam_pos, &face_list[i].v[0]);
        face_dots[i] = BrVector3Dot(&tv, &face_list[i].normal);
    }
    count_min_dots = 0;
    for (i = 0; i < (int)REC2_ASIZE(index_min_dots); i++) {
        min_dot = 100.0f;
        for (j = 0; j < count; j++) {
            if (face_dots[j] > 0.0f && face_dots[j] < min_dot) {
                index_min_dots[i] = j;
                min_dot = face_dots[j];
            }
        }
        if (min_dot == 100.0f) {
            break;
        }
        count_min_dots++;
        face_dots[index_min_dots[i]] = -100.0f;
    }
    if (count_min_dots > 0) {
        BrVector3Sub(&tv, pCam_pos, &face_list[index_min_dots[0]].v[0]);
        d = BrVector3Dot(&tv, &face_list[index_min_dots[0]].normal);
        if (d < hither) {
            BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, hither - d);
            BrVector3Accumulate(pCam_pos, &tv);
        }
        if (count_min_dots > 1) {
            d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[1]].normal);
            if (d > 0.95f && count_min_dots > 2) {
                count_min_dots = 2;
                d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[2]].normal);
                index_min_dots[1] = index_min_dots[2];
            }
            if (d <= 0.95f) {
                BrVector3Sub(&tv, pCam_pos, &face_list[index_min_dots[1]].v[0]);
                d = BrVector3Dot(&tv, &face_list[index_min_dots[1]].normal);
                if (d < hither) {
                    a = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[1]].normal);
                    BrVector3Scale(&tv2, &face_list[index_min_dots[0]].normal, a);
                    BrVector3Sub(&face_list[index_min_dots[1]].normal, &face_list[index_min_dots[1]].normal, &tv2);
                    BrVector3Scale(&tv, &face_list[index_min_dots[1]].normal, hither - d);
                    BrVector3Accumulate(pCam_pos, &tv);
                }
            }
            if (count_min_dots > 2) {
                BrVector3Sub(&tv, pCam_pos, &face_list[index_min_dots[1]].v[0]);
                d = BrVector3Dot(&tv, &face_list[index_min_dots[1]].normal);
                if (d < hither && d >= 0.0f) {
                    BrVector3Cross(&tv2, &face_list[index_min_dots[0]].normal, &face_list[index_min_dots[1]].normal);
                    if (BrVector3Dot(&tv2, &face_list[index_min_dots[2]].normal) < 0.0f) {
                        BrVector3Negate(&tv2, &tv2);
                    }
                    if (d > 0.5f) {
                        BrVector3Scale(&tv, &tv2, (hither - d) / BrVector3Dot(&tv2, &face_list[index_min_dots[2]].normal));
                        BrVector3Accumulate(pCam_pos, &tv);
                    }
                }
            }
        }
    }
    if (pCollision_info != NULL && gAction_replay_camera_mode != kActionReplayCameraMode_Manual) {
        do {
            BrVector3Sub(&delta, pCam_pos, pCar_pos);
            BrVector3Normalise(&delta, &delta);
            BrVector3Scale(&delta, &delta, hither);
            BrVector3Sub(&tv, pCam_pos, &delta);
            DrMatrix34ApplyLPInverse(&p1, pCam_pos, &pCollision_info->actor->t.t.mat);
            DrMatrix34ApplyLPInverse(&p2, &tv, &pCollision_info->actor->t.t.mat);
            k = ShapeRayCast(&p1, &p2, pCollision_info->shape, &new_cam_pos, &ts, &temp_normal);
            if (k != 0) {
                if (k > 0) {
                    BrMatrix34ApplyP(pCam_pos, &new_cam_pos, &pCollision_info->actor->t.t.mat);
                }
                BrVector3Accumulate(pCam_pos, &delta);
            }
        } while (k < 0);
    }
    return loop_done;
}

// FUNCTION: CARMA2_HW 0x00413570
// FUNCTION: CARMA2_SW 0x004135a0
int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos) {
    (void)pPos;
    (void)pCamera_pos;
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00413580
// FUNCTION: CARMA2_SW 0x004135b0
// FUNCTION: CARMA2_CHEAT 0x004137e0
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {
    br_scalar ts;
    tCar_spec* c;
    br_angle yaw;

    c = gCar_to_view;
    if (!gProgram_state.racing) {
        c = &gProgram_state.current_car;
    }
    gCamera_height = c->pos.v[1];
    gView_direction.v[0] = c->direction.v[0];
    gView_direction.v[1] = 0.0f;
    gView_direction.v[2] = c->direction.v[2];
    gFLOAT_006792fc = 1.0f;
    BrVector3Normalise(&gView_direction, &gView_direction);
    ts = -((gView_direction.v[2] * c->car_master_actor->t.t.mat.m[2][2]) + gView_direction.v[0] * c->car_master_actor->t.t.mat.m[2][0] + gView_direction.v[1] * c->car_master_actor->t.t.mat.m[2][1]);
    gCamera_sign = ts < 0;
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    DrVector3RotateY(&gView_direction, yaw);
    gCamera_frozen = 0;
    gCamera_mode = -2;
    gMin_camera_car_distance = 0.6f;
    if (gCountdown && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.grid_start) && gCountdown > 4) {
        gCamera_height += 10.f;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00413770
// FUNCTION: CARMA2_SW 0x004137a0
void C2_HOOK_FASTCALL FreezeCamera(void) {

    gCamera_frozen = 1;
}

// FUNCTION: CARMA2_HW 0x00413780
// FUNCTION: CARMA2_SW 0x004137b0
// FUNCTION: CARMA2_CHEAT 0x004139e0
void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race) {
    int i;
    br_scalar total_cars;
    tCar_spec* car;

    total_cars = 0.0f;
    BrVector3SetFloat(&gAverage_grid_position, 0.0f, 0.0f, 0.0f);
    if (pThe_race->number_of_racers <= 2) {
        BrVector3Copy(&gAverage_grid_position, &gProgram_state.current_car.pos);
    } else {
        for (i = 0; i < pThe_race->number_of_racers; i++) {
            car = pThe_race->opponent_list[i].car_spec;
            BrVector3Accumulate(&gAverage_grid_position, &car->pos);
            total_cars += 1.0f;
        }
        gAverage_grid_position.v[0] /= total_cars;
        gAverage_grid_position.v[1] /= total_cars;
        gAverage_grid_position.v[2] /= total_cars;
    }
}

inline tCar_spec* C2_HOOK_FASTCALL FindACar(br_vector3* pPos, br_vector3* pDir, br_scalar* pT) {
    int i;
    int count;
    tVehicle_type type;
    tCar_spec* car;
    tCar_spec* found;
    br_vector3 start;
    br_vector3 dir;
    br_vector3 end;
    br_vector3 point;

    found = NULL;
    *pT = 1.0f;
    for (type = eVehicle_self; type <= eVehicle_drone; type++) {
        if (type == eVehicle_self) {
            count = 1;
        } else {
            count = GetCarCount(type);
        }
        for (i = 0; i < count; i++) {
            if (type == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(type, i);
            }
            DrMatrix34ApplyLPInverse(&start, pPos, &car->car_master_actor->t.t.mat);
            BrMatrix34TApplyV(&dir, pDir, &car->car_master_actor->t.t.mat);
            BrVector3Add(&end, &start, &dir);
            LineBoxColl(&start, &end, &car->collision_info->bb2, &point, pT);
            if (*pT < 1.0f) {
                BrVector3Scale(pDir, pDir, *pT);
                *pT = 1.0f;
                found = car;
            }
        }
    }
    return found;
}

inline tPedestrian* C2_HOOK_FASTCALL FindAPed(br_vector3* pPos, br_vector3* pDir, br_scalar* pT, br_actor** pActor) {
    int i;
    tPedestrian* ped;
    tPedestrian* found;
    br_actor* actor;
    br_vector3 start;
    br_vector3 dir;
    br_vector3 end;
    br_vector3 point;

    found = NULL;
    for (i = 0; i < gPed_count; i++) {
        ped = &gPedestrian_array[i];
        if (ped->flags & 0x1) {
            actor = GetCharacterActorPtr(ped->character);
            DrMatrix34ApplyLPInverse(&start, pPos, &actor->t.t.mat);
            BrMatrix34TApplyV(&dir, pDir, &actor->t.t.mat);
            BrVector3Add(&end, &start, &dir);
            LineBoxColl(&start, &end, &ped->character->personality->bb, &point, pT);
            if (*pT < 1.0f) {
                BrVector3Scale(pDir, pDir, *pT);
                *pActor = actor;
                *pT = 1.0f;
                found = ped;
            }
        }
    }
    return found;
}

// FUNCTION: CARMA2_HW 0x00413860
// FUNCTION: CARMA2_SW 0x00413890
// FUNCTION: CARMA2_CHEAT 0x00413ac0
void C2_HOOK_FASTCALL MouseClickInActionReplay(int pX_coord, int pY_coord, int pUnknown) {
    br_camera* camera_ptr;
    br_vector3 dir;
    br_vector3 tv;
    br_vector3 normal;
    br_scalar t;
    br_scalar ts;
    br_material* material;
    br_actor* actor;

    camera_ptr = gCamera->type_data;
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Manual && pUnknown == 0) {
        BrVector3Scale(&dir, (br_vector3*)gCamera->t.t.mat.m[2], -camera_ptr->yon_z);
        ts = (br_scalar)(camera_ptr->yon_z * tan(BrAngleToRadian(camera_ptr->field_of_view / 2))) * ((240 - pY_coord) / 240.0);
        BrVector3Scale(&tv, (br_vector3*)gCamera->t.t.mat.m[1], ts);
        BrVector3Accumulate(&dir, &tv);
        ts = (br_scalar)(camera_ptr->yon_z * tan(BrAngleToRadian(camera_ptr->field_of_view / 2))) * ((pX_coord - 320) / 240.0);
        BrVector3Scale(&tv, (br_vector3*)gCamera->t.t.mat.m[0], ts);
        BrVector3Accumulate(&dir, &tv);
        ActorFindFace(&gCamera->t.t.translate.t, &dir, gTrack_actor, &normal, &t, &material, &actor);
        if (t < 1.0f) {
            BrVector3Scale(&dir, &dir, t);
        }
        BrVector3Add(&gAction_replay_manual_camera_target_pos, &gCamera->t.t.translate.t, &dir);
        gAction_replay_manual_camera_target_type = 1;
        gAction_replay_manual_camera_target_car = FindACar(&gCamera->t.t.translate.t, &dir, &t);
        if (gAction_replay_manual_camera_target_car != NULL) {
            gAction_replay_manual_camera_target_type = 2;
        }
        gAction_replay_manual_camera_target_ped = FindAPed(&gCamera->t.t.translate.t, &dir, &t, &actor);
        if (gAction_replay_manual_camera_target_ped != NULL) {
            gAction_replay_manual_camera_target_type = 3;
            gAction_replay_manual_camera_target_actor = actor;
        }
    }
}


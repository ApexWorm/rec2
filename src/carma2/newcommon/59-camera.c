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
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "42-input.h"
#include "57-drone.h"
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

// DoCameraControls

// FrozenCamera

// CheckCameraHither

// FUNCTION: CARMA2_HW 0x0040f4a0
// FUNCTION: CARMA2_SW 0x0040f480
tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void) {
    int i;
    int score;
    tCar_spec* car;

    car = gNet_players[0].car;
    score = gNet_players[0].last_score_index;
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

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

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

// CollideCamera2

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

// FindACar

// FindAPed

// MouseClickInActionReplay


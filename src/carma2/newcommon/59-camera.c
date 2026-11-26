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

// GLOBAL: CARMA2_HW 0x006792b4
// GLOBAL: CARMA2_SW 0x00673a24
// GLOBAL: CARMA2_CHEAT 0x00680b64
br_actor* gPed_actor;

// GLOBAL: CARMA2_HW 0x006792b8
// GLOBAL: CARMA2_SW 0x00673a28
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

// FUNCTION: CARMA2_HW 0x004105f0
// FUNCTION: CARMA2_SW 0x004105d0
// FUNCTION: CARMA2_CHEAT 0x00410840
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {
    br_vector3 pos;
    br_vector3 perp;
    br_vector3 dir;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar ts;
    tU32 time;
    tU32 t;
    tU32 t2;
    tU32 time_step;
    br_matrix34* m1;
    br_vector3 left;
    br_vector3 right;
    br_vector3 car_centre;

    m1 = &c->car_master_actor->t.t.mat;
    gInside_camera_zone = 0;
    ScanCarsPositions(c, &c->pos, 411.678222f, -1, 5000, &pos, &time);
    BrVector3Sub(&dir, &pos, &c->pos);
    if (GetTotalTime() < time) {
        time_step = time - GetTotalTime();
    } else {
        time_step = GetTotalTime() - time;
    }
    time_step *= SRandomBetween(0.8f, 1.5f);
    if (BrVector3LengthSquared(&dir) < .01 || time == 0) {
        BrVector3Negate(&dir, (br_vector3*)m1->m[2]);
        BrVector3Copy(&car_centre, &c->pos);
        time_step = 0;
    } else {
        ScanCarsPositions(c, &c->pos, 102.919556f, -1, time_step / 2, &car_centre, &t);
        if (t == 0) {
            BrVector3Copy(&car_centre, &c->pos);
        }
    }
    BrVector3SetFloat(&tv, 0.f, 1.f, 0.f);
    BrVector3Cross(&perp, &tv, &dir);
    ts = BrVector3Length(&perp);
    if (ts < .1f) {
        return;
    }
    ts = 2.0 / ts;
    ts *= SRandomBetween(0.33333334f, 1.f);
    BrVector3Scale(&perp, &perp, ts);
    ts = SRandomBetween(0.33333334f, 1.f) * 2.f;
    BrVector3Set(&tv, 0.f, ts, 0.f);
    BrVector3Add(&tv, &car_centre, &tv);
    BrVector3Add(&left, &tv, &perp);
    BrVector3Sub(&right, &tv, &perp);
    CollideCamera2(&car_centre, &left, NULL, 1, NULL);
    CollideCamera2(&car_centre, &right, NULL, 1, NULL);
    BrVector3Sub(&tv, &left, &car_centre);
    BrVector3Sub(&tv2, &right, &car_centre);
    if (BrVector3LengthSquared(&tv) + SRandomPosNeg(.01f) > BrVector3LengthSquared(&tv2)) {
        BrVector3Copy(&gCamera->t.t.translate.t, &left);
    } else {
        BrVector3Copy(&gCamera->t.t.translate.t, &right);
    }
    if (time != 0 && CheckForWall(&c->pos, &gCamera->t.t.translate.t)) {
        ScanCarsPositions(c, &c->pos, 10000.f, -1, 1000, &tv, &t2);
        CollideCamera2(&tv, &gCamera->t.t.translate.t, NULL, 1, NULL);
    }
    if (time != 0 && CheckForWall(&pos, &gCamera->t.t.translate.t)) {
        time_step = time_step / 16;
        BrVector3Copy(&tv, &car_centre);
        do {
            ScanCarsPositions(c, &tv, 10000.f, abs(t - GetTotalTime()), time_step, &tv2, &t2);
            t += (ARGetReplayDirection() ? 1 : -1) * time_step;
            BrVector3Copy(&tv, &tv2);
        } while (!CheckForWall(&tv, &gCamera->t.t.translate.t) && t < GetTotalTime() + 5000);
        gSwitch_time = t;
    } else {
        if (time == 0) {
            time = 5000;
        }
        gSwitch_time = time;
    }
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

// DrVector3RotateY

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// SwingCamera

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00411fc0
// STUB: CARMA2_SW 0x00411fa0
// STUB: CARMA2_CHEAT 0x00412210
void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor) {
    NOT_IMPLEMENTED();
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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004122b0
// STUB: CARMA2_SW 0x00412290
// STUB: CARMA2_CHEAT 0x00412500
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* pCar_pos, br_vector3* pCam_pos, br_vector3* pOld_camera_pos, int pManual_move, tPhysics_object* pCollision_info) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// STUB: CARMA2_HW 0x00413580
// STUB: CARMA2_SW 0x004135b0
// STUB: CARMA2_CHEAT 0x004137e0
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {
    NOT_IMPLEMENTED();
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


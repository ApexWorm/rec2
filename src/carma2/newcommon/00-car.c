/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "00-car.h"

#include "01-network.h"
#include "02-init.h"
#include "04-netgame.h"
#include "10-loading2.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "20-skidmark.h"
#include "22-replay.h"
#include "23-piping.h"
#include "27-powerup.h"
#include "29-raycast.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "39-oil.h"
#include "41-utility.h"
#include "42-input.h"
#include "49-physics.h"
#include "51-smash.h"
#include "53-controls.h"
#include "55-volume.h"
#include "57-drone.h"
#include "58-crush2.h"
#include "59-camera.h"
#include "60-phil.h"
#include "61-pedestrn-phys.h"
#include "69-sound.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"

#include "c2_math.h"
#include "c2_stdio.h"
#include "c2_string.h"
#include "rec2_macros.h"

#include <limits.h>
#include <math.h>

// GLOBAL: CARMA2_HW 0x0079eda0
// GLOBAL: CARMA2_SW 0x00795340
// GLOBAL: CARMA2_CHEAT 0x007a6440
tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x006793c0
// GLOBAL: CARMA2_SW 0x00673b30
// GLOBAL: CARMA2_CHEAT 0x00680c70
int gSkid_tag[2];

// GLOBAL: CARMA2_HW 0x006793c8
// GLOBAL: CARMA2_SW 0x00673b38
// GLOBAL: CARMA2_CHEAT 0x00680c78
tCar_spec* gLast_car_to_skid[2];

// GLOBAL: CARMA2_HW 0x0067935c
// GLOBAL: CARMA2_SW 0x00673acc
// GLOBAL: CARMA2_CHEAT 0x00680c0c
int gLast_crunch_vol;

// GLOBAL: CARMA2_HW 0x0058f680
// GLOBAL: CARMA2_SW 0x0058a680
// GLOBAL: CARMA2_CHEAT 0x00594710
int gMetal_crunch_sound_id[5] = { 5000, 5001, 5002, 5003, 5004 };

// GLOBAL: CARMA2_HW 0x0067939c
// GLOBAL: CARMA2_SW 0x00673b0c
// GLOBAL: CARMA2_CHEAT 0x00680c4c
int gTesting_car_for_sensible_place;

// GLOBAL: CARMA2_HW 0x00763500
// GLOBAL: CARMA2_SW 0x00759a60
// GLOBAL: CARMA2_CHEAT 0x0076aba0
int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x0074d1c0
// GLOBAL: CARMA2_SW 0x00743720
// GLOBAL: CARMA2_CHEAT 0x00754860
float gArmour_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761f5c
// GLOBAL: CARMA2_SW 0x007584bc
// GLOBAL: CARMA2_CHEAT 0x007695fc
int gExplosion_sound_id;

// GLOBAL: CARMA2_HW 0x00761f60
// GLOBAL: CARMA2_SW 0x007584c0
// GLOBAL: CARMA2_CHEAT 0x00769600
float gPower_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761d40
// GLOBAL: CARMA2_SW 0x007582a0
// GLOBAL: CARMA2_CHEAT 0x007693e0
float gOffensive_starting_value[100];

// GLOBAL: CARMA2_HW 0x0075ba20
// GLOBAL: CARMA2_SW 0x00751f80
// GLOBAL: CARMA2_CHEAT 0x007630c0
tFloat_bunch_info gCar_softness;

// GLOBAL: CARMA2_HW 0x0074d600
// GLOBAL: CARMA2_SW 0x00743b60
// GLOBAL: CARMA2_CHEAT 0x00754ca0
tFloat_bunch_info gCar_car_damage_multiplier;

// GLOBAL: CARMA2_HW 0x0068b8d0
// GLOBAL: CARMA2_SW 0x00685e30
// GLOBAL: CARMA2_CHEAT 0x00693178
br_vector3 gZero_v__car;  // FIXME: make const?

// GLOBAL: CARMA2_HW 0x006793a0
// GLOBAL: CARMA2_SW 0x00673b10
int gFreeze_mechanics;

// GLOBAL: CARMA2_HW 0x006793dc
// GLOBAL: CARMA2_SW 0x00673b4c
// GLOBAL: CARMA2_CHEAT 0x00680c8c
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x006793d0
// GLOBAL: CARMA2_SW 0x00673b40
int gEliminate_faces;

// GLOBAL: CARMA2_HW 0x00679364
// GLOBAL: CARMA2_SW 0x00673ad4
// GLOBAL: CARMA2_CHEAT 0x00680c14
int gIn_the_sea;

// GLOBAL: CARMA2_HW 0x0065d010
// GLOBAL: CARMA2_SW 0x00657948
// GLOBAL: CARMA2_CHEAT 0x006646cc
int gFace_num__car = 1;

// GLOBAL: CARMA2_HW 0x00744820
// GLOBAL: CARMA2_SW 0x0073b2e0
// GLOBAL: CARMA2_CHEAT 0x0074bec0
tFace_ref gFace_list__car[300];

// GLOBAL: CARMA2_HW 0x00679360
// GLOBAL: CARMA2_SW 0x00673ad0
// GLOBAL: CARMA2_CHEAT 0x00680c10
br_scalar gMin_world_y;

// GLOBAL: CARMA2_HW 0x006793e4
// GLOBAL: CARMA2_SW 0x00673b54
// GLOBAL: CARMA2_CHEAT 0x00680c94
tPhysics_object* gUnknown_car_collision_info;

// GLOBAL: CARMA2_HW 0x006793d4
// GLOBAL: CARMA2_SW 0x00673b44
int gDouble_pling_water;

// GLOBAL: CARMA2_HW 0x0079ed88
// GLOBAL: CARMA2_SW 0x00795328
int gDrivable_on_count;

// GLOBAL: CARMA2_HW 0x0079ecc0
// GLOBAL: CARMA2_SW 0x00795260
tPhysics_object* gDrivable_on_list[50];

// GLOBAL: CARMA2_HW 0x00679390
br_vector3 gCar_to_view_original_v;

// FUNCTION: CARMA2_HW 0x00413cc0
// FUNCTION: CARMA2_SW 0x00413cf0
// FUNCTION: CARMA2_CHEAT 0x00413f20
void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount) {
    tDamage_unit* the_damage;

    if (pDamage_amount <= 0) {
        return;
    }
    the_damage = &pCar->damage_units[pUnit_type];
    the_damage->damage_level += pDamage_amount;
    if (the_damage->damage_level >= 100) {
        the_damage->damage_level = 99;
    }
}

// FUNCTION: CARMA2_HW 0x00413d00
// FUNCTION: CARMA2_SW 0x00413d30
// FUNCTION: CARMA2_CHEAT 0x00413f60
void C2_HOOK_FASTCALL DamageEngine(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_engine, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413d30
// FUNCTION: CARMA2_SW 0x00413d60
// FUNCTION: CARMA2_CHEAT 0x00413f90
void C2_HOOK_FASTCALL DamageTrans(int pAmount) {
    DamageUnit(&gProgram_state.current_car, eDamage_transmission, pAmount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413d60
// FUNCTION: CARMA2_SW 0x00413d90
// FUNCTION: CARMA2_CHEAT 0x00413fc0
void C2_HOOK_FASTCALL DamageSteering(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_steering, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413d90
// FUNCTION: CARMA2_SW 0x00413dc0
// FUNCTION: CARMA2_CHEAT 0x00413ff0
void C2_HOOK_FASTCALL DamageLFWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lf_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413dc0
// FUNCTION: CARMA2_SW 0x00413df0
// FUNCTION: CARMA2_CHEAT 0x00414020
void C2_HOOK_FASTCALL DamageLFBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lf_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413df0
// FUNCTION: CARMA2_SW 0x00413e20
// FUNCTION: CARMA2_CHEAT 0x00414050
void C2_HOOK_FASTCALL DamageLRBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lr_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e20
// FUNCTION: CARMA2_SW 0x00413e50
// FUNCTION: CARMA2_CHEAT 0x00414080
void C2_HOOK_FASTCALL DamageLRWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lr_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e50
// FUNCTION: CARMA2_SW 0x00413e80
// FUNCTION: CARMA2_CHEAT 0x004140b0
void C2_HOOK_FASTCALL DamageRFWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rf_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e80
// FUNCTION: CARMA2_SW 0x00413eb0
// FUNCTION: CARMA2_CHEAT 0x004140e0
void C2_HOOK_FASTCALL DamageRFBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rf_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413eb0
// FUNCTION: CARMA2_SW 0x00413ee0
// FUNCTION: CARMA2_CHEAT 0x00414110
void C2_HOOK_FASTCALL DamageRRBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rr_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413ee0
// FUNCTION: CARMA2_SW 0x00413f10
// FUNCTION: CARMA2_CHEAT 0x00414140
void C2_HOOK_FASTCALL DamageRRWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rr_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413f10
// FUNCTION: CARMA2_SW 0x00413f40
// FUNCTION: CARMA2_CHEAT 0x00414170
intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData) {

    if (pActor->user != NULL) {
        int* index = pData;
        tUser_crush_data* crush_data = pActor->user;
        br_model* model = crush_data->models[*index];

        if (model != NULL) {
            pActor->model = model;
            pActor->type = BR_ACTOR_MODEL;
            return 0;
        }
    }
    pActor->model = NULL;
    pActor->type = BR_ACTOR_NONE;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00413f40
// FUNCTION: CARMA2_CHEAT 0x004141a0
void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex) {
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &pIndex);
    pCar->field_0xe18 = pIndex;
}

// FUNCTION: CARMA2_HW 0x00413f70
// FUNCTION: CARMA2_SW 0x00413fa0
// FUNCTION: CARMA2_CHEAT 0x004141d0
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {
    int index;
    int j;
    br_matrix34 safe_position;
    tNet_game_player_info* net_player;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x261, 0x261);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, message_time, 0x268);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x49c, 0x49c);

    PossibleService();
    if (pCar->disabled && pClear_disabled_flag) {
        if (gNet_mode == eNet_mode_none) {
            EnableCar(pCar);
        } else {
            net_player = NetPlayerFromCar(pCar);
            if (net_player->player_status == ePlayer_status_ready || net_player->player_status == ePlayer_status_racing) {
                EnableCar(pCar);
            }
        }
    }
    InitCarSkidStuff(pCar);
    pCar->car_model_actor->render_style = BR_RSTYLE_DEFAULT;
    SwitchCarModels(pCar, 0);
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    pCar->field_0xc8 = NULL;
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        ResetRecoveryVouchers();
    }
    BrVector3SetFloat(&pCar->collision_info->v, 0.0f, 0.0f, 0.0f);
    BrVector3SetFloat(&pCar->collision_info->omega, 0.0f, 0.0f, 0.0f);
    pCar->curvature = 0.0f;
    pCar->field_0x1260 = 0.0f;
    BrMatrix34Copy(&safe_position, &pCar->car_master_actor->t.t.mat);
    if (safe_position.m[3][0] > 500.0f) {
        BrVector3Sub((br_vector3*)safe_position.m[3], (br_vector3*)safe_position.m[3], &gInitial_position);
    }
    BrMatrix34Copy(&pCar->old_frame_mat, &safe_position);
    BrMatrix34Copy(&pCar->collision_info->transform_matrix, &safe_position);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    for (j = 0; j < REC2_ASIZE(pCar->oldd); j++) {
        pCar->oldd[j] = pCar->ride_height;
    }
    pCar->gear = 0;
    pCar->revs = 0.0f;
    pCar->traction_control = 1;
    BrVector3Negate(&pCar->direction, (br_vector3*)pCar->car_master_actor->t.t.mat.m[2]);
    for (j = 0; j < REC2_ASIZE(pCar->last_safe_positions); j++) {
        BrMatrix34Copy(&pCar->last_safe_positions[j], &safe_position);
    }
    pCar->collision_info->field_0x261 = 0;
    pCar->collision_info->message_time = 0;
    pCar->dt = -1.0f;
    pCar->collision_info->field_0x49c = 1;
    pCar->time_to_recover = 0;
    pCar->repair_time  = 0;
    pCar->collision_info->water_d  = 10000.0f;

    switch (pCar->driver) {
    case eDriver_oppo:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_oppo) {
                if (pCar == gCurrent_race.opponent_list[j].car_spec) {
                    pCar->car_ID = (eVehicle_opponent << 8) + index;
                }
                index++;
            }
        }
        break;
    case eDriver_net_human:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_net_human) {
                if (pCar == gCurrent_race.opponent_list[j].car_spec) {
                    pCar->car_ID = (eVehicle_net_player << 8) + index;
                }
                index++;
            }
        }
        break;
    case eDriver_local_human:
        pCar->car_ID = 0;
        break;
#ifndef REC2_MATCHING
    default:
        break;
#endif
    }
    PossibleService();
    pCar->collision_info->box_face_ref = gFace_num__car - 2;
    pCar->collision_info->box_face_end = 0;
    pCar->collision_info->box_face_start = 0;
    pCar->collision_info->disable_move_rotate = 0;
    pCar->end_steering_damage_effect = 0;
    pCar->end_trans_damage_effect = 0;
    pCar->wheel_dam_offset[0] = 0.0f;
    pCar->wheel_dam_offset[1] = 0.0f;
    pCar->wheel_dam_offset[2] = 0.0f;
    pCar->wheel_dam_offset[3] = 0.0f;
    pCar->shadow_intersection_flags = 0;
    pCar->underwater_ability = 0;

    if (gNet_mode == eNet_mode_none) {
        net_player = NULL;
    } else {
        net_player = NetPlayerFromCar(pCar);
    }
    if (net_player != NULL && net_player->field_0x80) {
        pCar->invulnerable_no_crushage = 1;
        pCar->invulnerable_no_damage = 1;
        pCar->invulnerable_no_wastage = 1;
    } else {
        pCar->invulnerable_no_crushage = 0;
        pCar->invulnerable_no_damage = 0;
        pCar->invulnerable_no_wastage = 0;
    }
    pCar->wall_climber_mode = 0;
    pCar->grip_multiplier = 1.0f;
    pCar->damage_multiplier = 1.0f;
    pCar->field_0x4c8 = 1.0f;
    pCar->field_0x4d4 = 1.0f;
    pCar->bounce_rate = 0.0f;
    pCar->bounce_amount = 0.0f;
    pCar->knackered = 0;
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    RemoveFromCloakingList(pCar);
    TurnOffCloaking(NULL, pCar);
    if (pCar == NULL || pCar->driver != eDriver_local_human) {
        pCar->joystick.left = -1;
        pCar->joystick.right = -1;
    }
    TotallyRepairACar(pCar);
    SetCarSuspGiveAndHeight(pCar, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    for (j = 0; j < REC2_ASIZE(pCar->powerups); j++) {
        pCar->powerups[j] = 0;
    }
    if (gNet_mode != eNet_mode_none && (net_player == NULL || !net_player->field_0x80)) {
        for (j = 0; j < (int)REC2_ASIZE(pCar->power_up_levels); j++) {
            pCar->power_up_levels[j] = gNet_mode != eNet_mode_none ? gInitial_APO[j].initial_network[gCurrent_net_game->type] : gInitial_APO[j].initial[gProgram_state.skill_level];
            pCar->power_up_slots[j] = gNet_mode != eNet_mode_none ? gInitial_APO_potential[j].initial_network[gCurrent_net_game->type] : gInitial_APO_potential[j].initial[gProgram_state.skill_level];
        }
    }
}

// FUNCTION: CARMA2_HW 0x00414400
// FUNCTION: CARMA2_SW 0x00414430
// FUNCTION: CARMA2_CHEAT 0x00414660
void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar) {
    InitialiseCar2(pCar, 1);
}

// FUNCTION: CARMA2_HW 0x00414410
// FUNCTION: CARMA2_SW 0x00414440
// FUNCTION: CARMA2_CHEAT 0x00414670
void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race) {
    int i;
    int cat;
    int car_count;
    tCar_spec* car;
    br_bounds bnds;

    gProgram_state.initial_position = pThe_race->initial_position;
    gProgram_state.initial_yaw = pThe_race->initial_yaw;
    BrActorToBounds(&bnds, gProgram_state.track_spec.the_actor);
    gMin_world_y = bnds.min.v[1];
    gNum_active_non_cars = 0;
    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            PossibleService();
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                InitialiseCar2(car, 1);
            }
        }
    }
    gCamera_yaw = 0;
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Manual) {
        gCamera_type = 0;
        gAction_replay_camera_mode = kActionReplayCameraMode_Standard;
    }
    InitialiseExternalCamera();
    if (gUnknown_car_collision_info != NULL && gProgram_state.current_car.collision_info != gUnknown_car_collision_info->parent) {
        PhysicsAddObjectList(gProgram_state.current_car.collision_info, gUnknown_car_collision_info);
    }
}

// FUNCTION: CARMA2_HW 0x00414510
// FUNCTION: CARMA2_SW 0x00414540
// FUNCTION: CARMA2_CHEAT 0x00414770
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, tU32 pGrid_index) {
    int place_on_grid;
    int i;
    int start_i;
    int j;
    br_actor* car_actor;
    br_angle initial_yaw;
    br_scalar nearest_y_above;
    br_scalar nearest_y_below;
    int below_face_index;
    int above_face_index;
    br_model* below_model;
    br_model* above_model;
    tCar_spec* car;
    br_vector3 grid_offset;
    br_vector3 dist;
    br_vector3 real_pos;
    br_matrix34 initial_yaw_matrix;
    tCompressed_matrix3 compressed;
    int inactive;

    car = pThe_race->opponent_list[pCar_index].car_spec;
    car_actor = car->car_master_actor;
    BrMatrix34Identity(&car_actor->t.t.mat);
    place_on_grid = 1;
    if (gNet_mode != eNet_mode_none && !gCurrent_net_game->options.grid_start && pThe_race->count_network_start_points != 0) {
        start_i = i = IRandomBetween(0, pThe_race->count_network_start_points - 1);
        do {
            PossibleService();
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (j != pCar_index) {
                    BrVector3Copy(&real_pos, &pThe_race->opponent_list[j].car_spec->car_master_actor->t.t.translate.t);
                    if (real_pos.v[0] > 500.0f) {
                        BrVector3Sub(&real_pos, &real_pos, &gInitial_position);
                    }
                    BrVector3Sub(&dist, &real_pos, &pThe_race->net_starts[i].pos);
                    if (BrVector3LengthSquared(&dist) < 16.0f) {
                        break;
                    }
                }
            }
            if (j == gNumber_of_net_players) {
                BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[i].pos);
                initial_yaw = BrDegreeToAngle(pThe_race->net_starts[i].yaw);
                place_on_grid = 0;
                break;
            }
            i++;
            if (i == pThe_race->count_network_start_points) {
                i = 0;
            }
        } while (i != start_i);
    }
    if (gNet_mode == eNet_mode_none && (int)pGrid_index < 0) {
        BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[-((int)pGrid_index + 1)].pos);
        initial_yaw = BrDegreeToAngle(pThe_race->net_starts[-((int)pGrid_index + 1)].yaw);
        place_on_grid = 0;
    }
    if (place_on_grid) {
        initial_yaw = BrDegreeToAngle(pThe_race->initial_yaw);
        BrMatrix34RotateY(&initial_yaw_matrix, initial_yaw);
        grid_offset.v[0] = -(br_scalar)((int)pGrid_index % 2);
        grid_offset.v[1] = 0.0f;
        grid_offset.v[2] = (br_scalar)((int)pGrid_index % 2) * 0.4f - (br_scalar)((int)pGrid_index / 2) * -2.0f;
        BrMatrix34ApplyV(&car_actor->t.t.translate.t, &grid_offset, &initial_yaw_matrix);
        BrVector3Accumulate(&car_actor->t.t.translate.t, &pThe_race->initial_position);
    }
    if (gTrack_actor != NULL) {
        FindBestY(&car_actor->t.t.translate.t, gTrack_actor, 10.0f, &nearest_y_above, &nearest_y_below, &above_model, &below_model, &above_face_index, &below_face_index);
        if (nearest_y_above != 30000.0f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_above;
        } else if (nearest_y_below != -30000.0f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_below;
        } else {
            car_actor->t.t.translate.t.v[1] = 0.0f;
        }
    }
    BrMatrix34PreRotateY(&car_actor->t.t.mat, initial_yaw);
    if (gNet_mode != eNet_mode_none) {
        CompressMatrix34(&compressed, &inactive, &car_actor->t.t.mat);
        ExpandMatrix34(&car_actor->t.t.mat, &compressed, inactive);
        BrMatrix34Copy(&gNet_players[pThe_race->opponent_list[pCar_index].net_player_index].initial_position, &car->car_master_actor->t.t.mat);
    }
    if (gNet_mode != eNet_mode_none && car->disabled && car_actor->t.t.translate.t.v[0] < 500.0f) {
        DisableCar(car);
    }
}

// FUNCTION: CARMA2_HW 0x004148d0
// FUNCTION: CARMA2_SW 0x00414900
// FUNCTION: CARMA2_CHEAT 0x00414b30
void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race) {
    int i;

    for (i = 0; i < pThe_race->number_of_racers; i++) {
        int grid_index = pThe_race->opponent_list[i].ranking;

        if (grid_index >= 0) {
            grid_index = i;
        }
        SetInitialPosition(pThe_race, i, grid_index);
    }
}

// InitialiseNonCar

// FUNCTION: CARMA2_HW 0x00414910
// FUNCTION: CARMA2_SW 0x00414940
void C2_HOOK_FASTCALL NewFaceListCallBack(tPhysics_object* pObject, tFace_ref* pFace) {
    br_bounds3 current_bounds;
    br_scalar old_d;
    float prop7;

    old_d = pObject->water_d;
    if (pObject->owner == NULL) {
        return;
    }
    prop7 = PHILGetObjectProperty(pObject, 7);
    if (pObject->flags_0x238 != 1 && prop7 == 0.0f) {
        return;
    }
    if (pObject != NULL
            && pObject->owner != NULL
            && pObject->flags_0x238 == 1
            && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human
            && pObject->water_d != 10000.0f
            && gDouble_pling_water
            && BrVector3Dot(&pObject->water_normal, &pObject->field_0xf4.max) - pObject->water_d <= 0.0f) {
        gIn_the_sea = 1;
        FreezeCamera();
    }
    if (pFace != NULL && (float)fabs(pFace->normal.v[1]) > 0.9f) {
        BrVector3Copy(&pObject->water_normal, &pFace->normal);
        if (pObject->water_normal.v[1] < 0.0f) {
            BrVector3Negate(&pObject->water_normal, &pObject->water_normal);
        }
        pObject->water_d = BrVector3Dot(&pFace->v[0], &pObject->water_normal);
        if (pObject != NULL
            && pObject->owner != NULL
            && pObject->flags_0x238 == 1
            && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human) {
            if (pFace->material->identifier[1] == '!') {
                if (BrVector3Dot(&pObject->field_0xf4.min, &pObject->water_normal) - pObject->water_d < 0.0f) {
                    GetNewBoundingBox(&current_bounds, &pObject->bb1, &pObject->actor->t.t.mat);
                    if (BrVector3Dot(&current_bounds.min, &pObject->water_normal) - pObject->water_d < 0.0f) {
                        gIn_the_sea = 1;
                        FreezeCamera();
                    }
                }
                gDouble_pling_water = 1;
            } else {
                gDouble_pling_water = 0;
            }
        }
    } else {
        pObject->water_d = 10000.f;
        if (pObject != NULL
                && pObject->owner != NULL
                && pObject->flags_0x238 == 1
                && ((tCar_spec*)pObject->owner)->driver == eDriver_local_human) {
            gIn_the_sea = gIn_the_sea == 1 ? 2 : 0;
        }
    }
    if (fabs(old_d - pObject->water_d) > 1e-05) {
        if (PHILGetObjectProperty(pObject, 6) != 0.f) {
            ARStartPipingSession(ePipe_chunk_extended_splash);
            AddExtendedSplashToPipingSession(pObject, pObject);
            AREndPipingSession();
        }
        if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
            tCar_spec* owner = (tCar_spec*)pObject->owner;
            if (owner != NULL && owner->driver > 5) {
                ARStartPipingSession(ePipe_chunk_splash);
                AddSplashToPipingSession(pObject);
                AREndPipingSession();
                return;
            }
            ARStartPipingSession(ePipe_chunk_extended_splash);
            AddExtendedSplashToPipingSession(pObject, pObject->actor);
            AREndPipingSession();
            return;
        }
        if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
            ARStartPipingSession(ePipe_chunk_extended_splash);
            AddExtendedSplashToPipingSession(pObject, pObject->owner);
            AREndPipingSession();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00414ca0
// FUNCTION: CARMA2_SW 0x00414cd0
// FUNCTION: CARMA2_CHEAT 0x00414f00
int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return gIn_the_sea;
}

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

// FUNCTION: CARMA2_HW 0x004157e0
// FUNCTION: CARMA2_SW 0x00415810
// FUNCTION: CARMA2_CHEAT 0x00415a40
void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {

    pCar->steering_angle = (br_scalar)BrRadianToDegree(atan((pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (pCar->field_0x1260 + pCar->curvature)));
    pCar->lr_sus_position = (br_scalar)((pCar->wpos[0].v[1] - pCar->oldd[0]) / WORLD_SCALE_D);
    pCar->rr_sus_position = (br_scalar)((pCar->wpos[1].v[1] - pCar->oldd[1]) / WORLD_SCALE_D);
    pCar->lf_sus_position = (br_scalar)((pCar->wpos[2].v[1] - pCar->oldd[2]) / WORLD_SCALE_D);
    pCar->rf_sus_position = (br_scalar)((pCar->wpos[3].v[1] - pCar->oldd[3]) / WORLD_SCALE_D);
    PipeSingleGraphicalWheelStuff(pCar);
}

// FinishCars

// GetNonCars

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00415c40
// STUB: CARMA2_SW 0x00415c70
void C2_HOOK_FASTCALL GetNetPos(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL MungeCarsMass(tCar_spec* pCar, br_scalar pFactor) {

    pCar->collision_info->M *= pFactor;
    BrVector3Scale(&pCar->collision_info->I, &pCar->collision_info->I, pFactor);
}

// FUNCTION: CARMA2_HW 0x00415ef0
// FUNCTION: CARMA2_SW 0x00415f20
void C2_HOOK_FASTCALL AddDrag(const tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDelta_time) {
    br_scalar drag;
    br_scalar k;
    br_vector3 v;

    drag = pDelta_time * 0.0005f;
    if (pObject->last_special_volume != NULL) {
        k = pObject->last_special_volume->viscosity_multiplier;
        if (pCar->underwater_ability) {
            k = (br_scalar)(k * 0.6);
        }
        drag *= k;
        drag *= pObject->water_depth_factor;
    }

    k = (br_scalar)(BrVector3Length(&pObject->v) * drag * WORLD_SCALE_D / pObject->M);
    BrVector3Scale(&v, &pObject->v, k);
    DRVector3Diminish(&pObject->v, &v);

    k = BrVector3Length(&pObject->omega) * drag;
    BrVector3Scale(&v, &pObject->omega, k);
    ApplyDiminishingTorque(pObject, &v);
}

// FUNCTION: CARMA2_HW 0x00416030
// FUNCTION: CARMA2_SW 0x00416060
void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pObject) {

    for (; pObject != NULL; pObject = pObject->next) {
        AddDrag(pCar, pObject, 0.04f);
        if (pObject->child != NULL) {
            DragChildren(pCar, pObject->child);
        }
    }
}

inline void C2_HOOK_FASTCALL DoCarStuff(tCar_spec* pCar) {

    BrMatrix34Copy(&pCar->car_master_actor->t.t.mat, &pCar->collision_info->transform_matrix);
    if (pCar->collision_info->field_0x261 == 16
            && pCar->collision_info->message_time == gPHIL_last_physics_tick) {
        if (pCar == NULL || pCar->driver != eDriver_local_human) {
            pCar->curvature = (int)pCar->collision_info->field_0x278 * pCar->maxcurve / 32767.0f;
        }
        if (gNet_mode == eNet_mode_host) {
            pCar->collision_info->field_0xf0 = 1;
            GetNetPos(pCar);
        } else {
            GetNetPos(pCar);
        }
    }
    if (!pCar->disabled) {
        if (!(pCar->collision_info->disable_move_rotate
                && gPalette_fade_time != 0
                && pCar != NULL
                && pCar->driver == eDriver_local_human)) {
            if (pCar->collision_info->box_face_ref != gFace_num__car
                    && (pCar->collision_info->box_face_ref != gFace_num__car - 1
                        || pCar->collision_info->box_face_start <= gFace_count)) {
                GetFacesNearObject(pCar->collision_info, &gWorld_callbacks);
            }
            if (pCar->dt != 0.0f) {
                MoveAndCollideCar(pCar, 0.04f);
            }
            if (pCar->collision_info->child != NULL) {
                ApplyGravity(pCar->collision_info->child);
                DragChildren(pCar, pCar->collision_info->child);
                PositionChildren(pCar->collision_info);
            }
        }
    }
}

inline void C2_HOOK_FASTCALL DoNonCarStuff(tNon_car_spec* pNon_car) {

    if (pNon_car->collision_info->field_0x261 != 0
            && pNon_car->collision_info->message_time == gPHIL_last_physics_tick) {
        GetNetPos((tCar_spec*)pNon_car);
    }
    if (pNon_car->flags & 0x10000) {
        if (pNon_car->field_0xf8 != 0 && pNon_car->field_0xf8 < GetRaceTime()) {
            pNon_car->field_0xf8 = 0;
            pNon_car->flags = (pNon_car->flags & 0xffff0100) | 0x100;
            pNon_car->collision_info->disable_move_rotate = 0;
        }
    }
    if (!pNon_car->collision_info->disable_move_rotate
            || (pNon_car->flags & 0xff) != 0
            || (pNon_car->flags & 0xff00) != 0) {
        pNon_car->collision_info->disable_move_rotate = 0;
        if (pNon_car->field_0xc4 != 0.0f) {
            MoveAndCollideNonCar(pNon_car, 0.04f);
        }
        if (pNon_car->collision_info->child != NULL) {
            ApplyGravity(pNon_car->collision_info->child);
            PositionChildren(pNon_car->collision_info);
        }
    }
}

void C2_HOOK_FASTCALL RemoveFlyingCar(tCar_spec* pCar) {

    pCar->collision_info->transform_matrix.m[3][1] += 1000.0f;
    BrMatrix34Copy(&pCar->car_master_actor->t.t.mat, &pCar->collision_info->transform_matrix);
    PositionChildren(pCar->collision_info);
    SetHierarchyDoingNothingFlag(pCar->collision_info, 0x1);
}

// RestoreFlyingCar

void C2_HOOK_FASTCALL GetDrivableOnList(tPhysics_object *pObjects) {
    tPhysics_object* object;

    gDrivable_on_count = 0;
    for (object = pObjects; object != NULL && gDrivable_on_count < REC2_ASIZE(gDrivable_on_list); object = object->next) {
        if (object->drivable_on) {
            gDrivable_on_list[gDrivable_on_count++] = object;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00415890
// FUNCTION: CARMA2_SW 0x004158c0
void C2_HOOK_FASTCALL APTCPreCollision(void) {
    int i;

    GetDrivableOnList(gPHIL_list_collision_infos);

    BrVector3Copy(&gCar_to_view_original_v, &gCar_to_view->collision_info->v);

    for (i = 0; i < gNum_active_cars; i++) {
        if (gActive_car_list[i] != NULL && gActive_car_list[i]->driver > eDriver_5) {
            DoCarStuff(gActive_car_list[i]);
        }
    }
    for (i = 0; i < gNum_active_non_cars; i++) {
        DoNonCarStuff(gActive_non_car_list[i]);
    }
    for (i = 0; i < gNum_active_cars; i++) {
        if (gActive_car_list[i]->field_0x4c8 != 1.0f) {
            MungeCarsMass(gActive_car_list[i], gActive_car_list[i]->field_0x4c8);
        }
    }
    if (gCar_flying) {
        RemoveFlyingCar(gCar_to_view);
    }
    MinePreCollisionStuff();
    PedPreCollisionStuff();
    DronePreCollisionStuff();
}

// FUNCTION: CARMA2_HW 0x004161a0
// FUNCTION: CARMA2_SW 0x004161d0
void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tPhysics_object* pObject) {
    tPhysics_object* child;
    tCar_spec* car;
    int collided;

    for (; pObject != NULL; pObject = pObject->next) {
        if (gNet_mode == eNet_mode_host) {
            collided = (pObject->collision_flag & 0x2) != 0 || (pObject->flags & 0x1000) != 0;
            for (child = pObject->child; child != NULL; child = child->next) {
                collided |= child->collision_flag & 0x2;
            }
            if (collided) {
                pObject->field_0x49c = gPHIL_last_physics_tick + 40;
                if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
                    car = pObject->owner;
                    if (car->driver > 5 && car->car_crush_spec != NULL) {
                        SetAllShapeStatusFlags(car->car_crush_spec->network_stuff);
                    }
                }
            }
        }
        pObject->disable_move_rotate &= ~0x2;
        SetCollisionFlagsAndStuff(pObject->child);
    }
}

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004168c0
// STUB: CARMA2_SW 0x004168f0
void C2_HOOK_FASTCALL MoveAndCollideCar(tCar_spec* pCar, br_scalar pDt) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00417030
// FUNCTION: CARMA2_SW 0x00417060
void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {

    if (pNon_car->collision_info->water_d != 10000.0f) {
        TestAutoSpecialVolume(pNon_car->collision_info);
    }
    MungeSpecialVolume(pNon_car->collision_info);
    if (pNon_car->field_0xc4 >= 0.0f) {
        pDt = pNon_car->field_0xc4;
    }
    NonCarCalcForce(pNon_car, pDt);
    BrMatrix34ApplyP(&pNon_car->pos, &pNon_car->centre_of_mass_world_scale, &pNon_car->car_master_actor->t.t.mat);
    BrVector3InvScale(&pNon_car->pos, &pNon_car->pos, WORLD_SCALE);
}

// FUNCTION: CARMA2_HW 0x00417180
// FUNCTION: CARMA2_SW 0x004171b0
// FUNCTION: CARMA2_CHEAT 0x004173e0
void C2_HOOK_FASTCALL ControlCar2(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04 / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed += time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04 / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed -= time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x004173b0
// FUNCTION: CARMA2_SW 0x004173e0
// FUNCTION: CARMA2_CHEAT 0x00417610
void C2_HOOK_FASTCALL ControlCar3(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f || pCar->collision_info->omega.v[1] < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04 / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f || pCar->collision_info->omega.v[1] > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04 / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x004175e0
// FUNCTION: CARMA2_SW 0x00417610
// FUNCTION: CARMA2_CHEAT 0x00417840
void C2_HOOK_FASTCALL ControlCar4(tCar_spec* pCar, br_scalar pDt) {

    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->collision_info->velocity_car_space.v[2] > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + pDt / 4.0);
        } else if ((pCar->curvature < 0.0f || pCar->collision_info->omega.v[1] < -0.001) && pCar->turn_speed == 0.0f) {
            pCar->turn_speed = (float)(25.0f * pDt * (0.05 / (5.0 + WORLD_SCALE_D * BrVector3Length(&pCar->collision_info->v))));
            if (pCar->collision_info->omega.v[1] < -0.01) {
                pCar->turn_speed = (float)(pCar->turn_speed - pDt * pCar->collision_info->omega.v[1] / 4.0);
            }
        } else {
            pCar->turn_speed = (float)(pCar->turn_speed - 25.0f * pDt * (0.05 / (5.0 + WORLD_SCALE_D * BrVector3Length(&pCar->collision_info->v))) / -4.0);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->collision_info->velocity_car_space.v[2] > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - pDt / 4.0);
        } else if ((pCar->curvature > 0.0f || pCar->collision_info->omega.v[1] > 0.001) && pCar->turn_speed == 0.0f) {
            pCar->turn_speed = (float)(25.0f * pDt * (0.05 / (5.0 + WORLD_SCALE_D * BrVector3Length(&pCar->collision_info->v))) * -1.0);
            if (pCar->collision_info->omega.v[1] < -0.01) {
                pCar->turn_speed = (float)(pCar->turn_speed - pDt * pCar->collision_info->omega.v[1] / 4.0);
            }
        } else {
            pCar->turn_speed = (float)(pCar->turn_speed - 25.0f * pDt * (0.05 / (5.00 + WORLD_SCALE_D * BrVector3Length(&pCar->collision_info->v))) / 4.0);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    } else if (fabs(pCar->turn_speed) < fabs(2.0f * pDt * pCar->curvature) && pCar->curvature * pCar->turn_speed < 0.0f) {
        pCar->turn_speed = -(2.0f * pDt * pCar->curvature);
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->joystick.left > 0) {
        pCar->curvature = (float)pCar->joystick.left * (float)pCar->joystick.left / (float)UINT_MAX * pCar->maxcurve;
    } else if (pCar->joystick.right >= 0) {
        pCar->curvature = -((float)pCar->joystick.right * (float)pCar->joystick.right / (float)UINT_MAX) * pCar->maxcurve;
    }
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x00417a20
// FUNCTION: CARMA2_SW 0x00417a50
// FUNCTION: CARMA2_CHEAT 0x00417c80
void C2_HOOK_FASTCALL ControlCar5(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04 / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04 / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
        if (pCar->curvature < 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature + (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
            if (pCar->curvature > 0.0f) {
                pCar->curvature = 0.0f;
            }
        } else if (pCar->curvature > 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature - (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
            if (pCar->curvature < 0.0f) {
                pCar->curvature = 0.0f;
            }
        }
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
    pCar->keys.left = 1;
}

// FUNCTION: CARMA2_HW 0x00417de0
// FUNCTION: CARMA2_SW 0x00417e10
// FUNCTION: CARMA2_CHEAT 0x00418040
void C2_HOOK_FASTCALL ControlCar1(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->curvature < 0.0f) {
            pCar->curvature = (float)(pCar->curvature + pDt * 0.25);
        } else {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))));
        }
    }
    if (pCar->keys.right) {
        if (pCar->curvature > 0.0f) {
            pCar->curvature = (float)(pCar->curvature - pDt * 0.25);
        } else {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))));
        }
    }
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

inline int C2_HOOK_FASTCALL SteeringSelfCentre(tCar_spec* c, br_scalar dt, br_vector3* n) {
    br_scalar ts;
    br_scalar ts2;

    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }
    if (c->keys.left || c->keys.right || c->keys.holdw) {
        return 0;
    }
    if (!gINT_0074cf98 && (c->joystick.left > 0 || c->joystick.right > 0)) {
        return 0;
    }
    if (c->susp_height[1] <= c->oldd[2] && c->susp_height[1] <= c->oldd[3]) {
        return 0;
    }
    ts = -((dt / (c->wpos[0].v[2] - c->wpos[2].v[2])) * BrVector3Dot(&c->collision_info->omega, n));
    ts2 = -(dt * c->curvature);
    if (fabs(ts) < fabs(ts2) || ts * ts2 < 0.0f) {
        ts = ts2;
    }
    if (gKey_mapping[47] < 143 || gKey_mapping[48] < 143) {
        c->curvature += ts;
        if (ts * c->curvature > 0.0f) {
            c->curvature = 0.0f;
        }
    }
    return (int)(240000.0 * ts);
}

// NonCarSnapOff

// TestNonCarSnapOff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00417fb0
// STUB: CARMA2_SW 0x00417fe0
// STUB: CARMA2_CHEAT 0x00418210
void C2_HOOK_FASTCALL TumbleObjectWithV(tPhysics_object* pObject, br_vector3* pV, float pArg3, float pArg4) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL TumbleObject(tPhysics_object* pObject, float pTumble_factor, float pTumble_threshold) {

    TumbleObjectWithV(pObject, &pObject->v, pTumble_factor, pTumble_threshold);
}

// FUNCTION: CARMA2_HW 0x00418230
// FUNCTION: CARMA2_SW 0x00418260
// FUNCTION: CARMA2_CHEAT 0x00418490
void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, flags, 0x100);

    if (gNet_mode != eNet_mode_client) {
        pNon_car->flags &= ~0xff;
        pNon_car->flags |= 0x1;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00418250
// FUNCTION: CARMA2_SW 0x00418280
// FUNCTION: CARMA2_CHEAT 0x004184b0
void C2_HOOK_FASTCALL NonCarCalcForce(tNon_car_spec* pNon_car, br_scalar pDt) {
    tSpecial_volume* vol;
    tPhysics_object* other;
    tJoint_translation_params* params;
    br_scalar ts;
    br_scalar speed;
    br_scalar relative_speed;
    br_scalar acc;
    br_scalar resistance;
    br_vector3 tv;
    br_vector3 axis;

    vol = pNon_car->collision_info->last_special_volume;
    pNon_car->flags |= 0x100000;
    if (gCar_flying && gCar_to_view == (tCar_spec*)pNon_car) {
        return;
    }
    if (pNon_car->car_master_actor->identifier[3] != '!') {
        pNon_car->car_master_actor->identifier[3] = '&';
        if (pNon_car->collision_info->physics_joint2 == NULL) {
            pNon_car->collision_info->actor->identifier[3] = '!';
            BrVector3Set(&pNon_car->collision_info->omega, 0.0f, 0.0f, 0.0f);
            BrVector3Set(&pNon_car->collision_info->v, 0.0f, 0.0f, 0.0f);
            pNon_car->collision_info->M = pNon_car->free_mass;
            pNon_car->collision_info->field_0xed = 1;
            BrVector3Scale(&pNon_car->collision_info->I, &pNon_car->I_over_M, pNon_car->collision_info->M);
            other = pNon_car->collision_info->field183_0x1d8;
            if (other != NULL && other->owner != NULL && other->flags_0x238 == 1 && other->owner != pNon_car) {
                NonCarSnapSmashCallback(pNon_car->collision_info, &pNon_car->pos, &pNon_car->collision_info->v, &gBR_VECTOR3_00661678);
            }
        } else {
            BrVector3SetFloat(&pNon_car->collision_info->v, 0.0f, 0.0f, 0.0f);
            BrVector3SetFloat(&pNon_car->collision_info->omega, 0.0f, 0.0f, 0.0f);
            BrVector3SetFloat(&pNon_car->collision_info->rotate_omega, 0.0f, 0.0f, 0.0f);
            pNon_car->collision_info->disable_move_rotate = 1;
        }
    }
    if (pNon_car->car_master_actor->identifier[3] != '!') {
        return;
    }
    if (vol == NULL) {
        ts = 10.0f / WORLD_SCALE * pDt;
    } else {
        ts = pDt * 10.0f * vol->gravity_multiplier / WORLD_SCALE;
    }
    BrVector3Set(&pNon_car->collision_info->field_0x54, 0.0f, -ts, 0.0f);
    pNon_car->collision_info->v.v[1] -= ts;
    ts = BrVector3Length(&pNon_car->collision_info->v) * WORLD_SCALE;
    if (vol != NULL) {
        ts *= vol->viscosity_multiplier;
    }
    ts = ts * pDt * 0.0005f / pNon_car->collision_info->M;
    BrVector3Scale(&tv, &pNon_car->collision_info->v, ts);
    DRVector3Diminish(&pNon_car->collision_info->v, &tv);
    ts = BrVector3Length(&pNon_car->collision_info->omega);
    if (vol != NULL) {
        ts *= vol->viscosity_multiplier;
    }
    ts = ts * pDt * 0.0005f;
    BrVector3Scale(&tv, &pNon_car->collision_info->omega, ts);
    ApplyDiminishingTorque(pNon_car->collision_info, &tv);
    if (pNon_car->tumble_factor != 0.0f) {
        TumbleObject(pNon_car->collision_info, pNon_car->tumble_factor, pNon_car->tumble_threshold);
    }
    if (!(pNon_car->flags & 0x10000)) {
        return;
    }
    BrMatrix34ApplyV(&axis, &pNon_car->field_0xf0->hinge_axis, &pNon_car->collision_info->transform_matrix);
    speed = BrVector3Dot(&pNon_car->field_0xf0->hinge_axis, &pNon_car->collision_info->velocity_car_space);
    params = pNon_car->translation_parameters;
    resistance = params->forward_resistance;
    acc = 0.0f;
    relative_speed = speed - BrVector3Dot(&pNon_car->collision_info->field_0x1b4, &axis);
    if ((pNon_car->flags & 0x20000) && (pNon_car->collision_info->collision_flag & 0x2)
            && (pNon_car->flags & 0xff) == 0 && (pNon_car->flags & 0xff00) == 0 && pNon_car->field_0xf8 == 0) {
        MakeLiftGoUp(pNon_car);
    }
    if ((pNon_car->flags & 0xff) == 0 && (pNon_car->collision_info->disable_move_rotate || pNon_car->field_0xf8 == 0)) {
        if ((pNon_car->flags & 0xff00) != 0 || (!pNon_car->collision_info->disable_move_rotate && pNon_car->field_0xf8 == 0)) {
            resistance = params->reverse_resistance;
            acc -= (br_scalar)(0.4 / WORLD_SCALE_D * params->reverse_acceleration);
            if (relative_speed > 0.05 && speed > -0.005) {
                if (((pNon_car->flags >> 8) & 0xff) < pNon_car->number_of_pushes && (pNon_car->collision_info->collision_flag & 0x2)) {
                    pNon_car->flags = (pNon_car->flags & ~0xff00) | ((pNon_car->flags + 0x100) & 0xff00);
                } else {
                    pNon_car->flags &= ~0xff00;
                    pNon_car->collision_info->disable_move_rotate = 1;
                    BrVector3SetFloat(&pNon_car->collision_info->v, 0.0f, 0.0f, 0.0f);
                    BrVector3SetFloat(&pNon_car->collision_info->omega, 0.0f, 0.0f, 0.0f);
                    acc = 0.0f;
                }
            }
        }
    } else {
        acc += (br_scalar)(0.4 / WORLD_SCALE_D * params->forward_acceleration);
        if (relative_speed < -0.05 && speed < 0.005) {
            if (pNon_car->field_0xf8 == 0 || (pNon_car->flags & 0xff) == 1) {
                pNon_car->field_0xf8 = GetRaceTime() + pNon_car->translation_parameters->pause_at_top;
            }
            if ((pNon_car->flags & 0xff) < pNon_car->number_of_pushes && (pNon_car->collision_info->collision_flag & 0x2)) {
                if ((pNon_car->flags & 0xff) != 0) {
                    pNon_car->flags = (pNon_car->flags & ~0xff) | ((pNon_car->flags + 1) & 0xff);
                }
            } else {
                pNon_car->flags &= ~0xff;
                pNon_car->collision_info->disable_move_rotate = 1;
                BrVector3SetFloat(&pNon_car->collision_info->v, 0.0f, 0.0f, 0.0f);
                BrVector3SetFloat(&pNon_car->collision_info->omega, 0.0f, 0.0f, 0.0f);
                acc = 0.0f;
            }
        }
    }
    BrVector3Scale(&tv, &pNon_car->field_0xf0->hinge_axis, acc);
    BrVector3Accumulate(&pNon_car->collision_info->v, &tv);
    BrVector3Scale(&pNon_car->collision_info->v, &pNon_car->collision_info->v, resistance);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL DoBumpiness(tCar_spec* c, br_vector3* wheel_pos, br_vector3* norm, br_scalar* d, int n) {
    br_vector3 tv;
    int delta;
    int x;
    int y;
    tMaterial_modifiers* mat_list;

    mat_list = gCurrent_race.material_modifiers;
    BrVector3Scale(&tv, &c->nor[n], d[n]);
    BrVector3Accumulate(&tv, &wheel_pos[n]);
    x = abs((int)(512.0f * tv.v[0])) % 2048;
    y = abs((int)(512.0f * tv.v[2])) % 2048;
    if (x > 1024) {
        x = 2048 - x;
    }
    if (y > 1024) {
        y = 2048 - y;
    }
    if (x + y > 1024) {
        delta = 2048 - x - y;
    } else {
        delta = x + y;
    }
    delta -= 400;
    if (delta < 0) {
        delta = 0;
    }
    d[n] = delta * mat_list[c->material_index[n]].bumpiness / 42400.0f * norm[n].v[1] + d[n];
}

inline void C2_HOOK_FASTCALL SmashFacesWithWheels(tCar_spec* c, tFace_ref** pFace_refs, br_scalar* pForces, br_vector3* pWheel_pos) {
    int i;
    int j;
    int count;
    int best;
    br_scalar max_magnitude;
    tFace_ref* smashed_faces[4];
    br_scalar magnitudes[4];
    br_vector3 positions[4];
    br_vector3 force;

    count = 0;
    best = -1;
    max_magnitude = 0.0f;
    for (i = 0; i < 4; i++) {
        if (pFace_refs[i] != NULL && pFace_refs[i]->material != NULL && pFace_refs[i]->material->identifier != NULL
                && strlen(pFace_refs[i]->material->identifier) == 11 && pFace_refs[i]->material->identifier[5] == '|') {
            for (j = 0; j < count; j++) {
                if (pFace_refs[i] == smashed_faces[j]) {
                    break;
                }
            }
            if (j == count) {
                BrVector3Scale(&positions[count], (br_vector3*)c->car_master_actor->t.t.mat.m[1], -c->oldd[i]);
                BrVector3Accumulate(&positions[count], &pWheel_pos[i]);
                magnitudes[count] = pForces[i] * 0.04f;
                if (magnitudes[count] > max_magnitude) {
                    best = count;
                    max_magnitude = magnitudes[count];
                }
                smashed_faces[count] = pFace_refs[i];
                count++;
            }
        }
    }
    if (best >= 0) {
        BrVector3InvScale(&positions[best], &positions[best], WORLD_SCALE);
        BrVector3Scale(&force, (br_vector3*)c->car_master_actor->t.t.mat.m[1], -magnitudes[best]);
        SmashEnvironment(c->collision_info, smashed_faces[best], magnitudes[best], &positions[best], &c->collision_info->v, &force, 0, 0);
    }
    FlushSmashQueue(1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0041bd40
// FUNCTION: CARMA2_SW 0x0041bd70
// FUNCTION: CARMA2_CHEAT 0x0041bfa0
void C2_HOOK_FASTCALL ConditionallyNoteSkid(tCar_spec* pCar, tFace_ref** pFace_refs, int pWheel) {
    br_vector3 pos;
    br_scalar d;

    if (pFace_refs[pWheel] == NULL) {
        return;
    }
    if (pFace_refs[pWheel]->material != NULL
            && pFace_refs[pWheel]->material->colour_map != NULL
            && pFace_refs[pWheel]->material->colour_map->type == BR_PMT_RGBA_4444) {
        return;
    }
    if (pFace_refs[pWheel]->material != NULL
            && pFace_refs[pWheel]->material->identifier != NULL
            && strlen(pFace_refs[pWheel]->material->identifier) == 11
            && pFace_refs[pWheel]->material->identifier[5] == '|') {
        return;
    }
    if (pCar->new_skidding & (1 << pWheel)) {
        return;
    }
    pCar->new_skidding |= 1 << pWheel;
    if (pWheel & 1) {
        pos.v[0] = pCar->wpos[pWheel].v[0] - 0.1725f;
    } else {
        pos.v[0] = pCar->wpos[pWheel].v[0] + 0.1725f;
    }
    pos.v[0] = pos.v[0] / WORLD_SCALE;
    pos.v[1] = (pCar->wpos[pWheel].v[1] - pCar->oldd[pWheel]) / WORLD_SCALE;
    pos.v[2] = pCar->wpos[pWheel].v[2] / WORLD_SCALE;
    BrMatrix34ApplyP(&pCar->special_start[pWheel], &pos, &pCar->collision_info->transform_matrix);
    d = BrVector3Dot(&pFace_refs[pWheel]->v[0], &pFace_refs[pWheel]->normal) - BrVector3Dot(&pCar->special_start[pWheel], &pFace_refs[pWheel]->normal);
    pCar->special_start[pWheel].v[0] += pFace_refs[pWheel]->normal.v[0] * d;
    pCar->special_start[pWheel].v[1] += pFace_refs[pWheel]->normal.v[1] * d;
    pCar->special_start[pWheel].v[2] += pFace_refs[pWheel]->normal.v[2] * d;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL NudgeObject(tPhysics_object* pObject) {
    tNon_car_spec* non_car;

    if (pObject == NULL || pObject->owner == NULL || pObject->flags_0x238 != 1) {
        return;
    }
    non_car = pObject->owner;
    if (non_car != NULL && non_car->driver > eDriver_5) {
        return;
    }
    if ((non_car->flags & 0x10000) && (non_car->flags & 0x40000)
            && (non_car->flags & 0xff) == 0 && (non_car->flags & 0xff00) == 0 && non_car->field_0xf8 == 0) {
        MakeLiftGoUp(non_car);
    }
}

// FUNCTION: CARMA2_HW 0x00418850
// FUNCTION: CARMA2_SW 0x00418880
// FUNCTION: CARMA2_CHEAT 0x00418ab0
void C2_HOOK_FASTCALL CalcForce(tCar_spec* c, br_scalar dt) {
    int n;
    int normnum;
    int i;
    int sign;
    int ffb_force;
    int material_override;
    br_scalar force[4];
    br_scalar d[4];
    br_scalar d_change[4];
    br_scalar ts;
    br_scalar ts2;
    br_scalar ts3;
    br_scalar ts4;
    br_scalar ts5;
    br_scalar ts6;
    br_scalar max_d;
    br_scalar deltaomega;
    br_scalar wheelratio;
    br_scalar modf;
    br_scalar maxfl;
    br_scalar maxfr;
    br_scalar max_retardation;
    br_scalar front_retardation;
    br_scalar friction_number;
    br_scalar inertia;
    br_scalar direction;
    br_scalar v;
    br_scalar ffi;
    br_scalar ffk;
    br_scalar fbi;
    br_scalar fbk;
    br_scalar fl_oil_factor;
    br_scalar fr_oil_factor;
    br_scalar rl_oil_factor;
    br_scalar rr_oil_factor;
    br_vector3 a;
    br_vector3 b;
    br_vector3 tv;
    br_vector3 tv2;
    br_vector3 norm[4];
    br_vector3 vel;
    br_vector3 vcs;
    br_vector3 vplane;
    br_vector3 rightplane;
    br_vector3 t;
    br_vector3 f;
    br_vector3 ff;
    br_vector3 fb;
    br_vector3 wheel_pos[4];
    br_matrix34* mat;
    tMaterial_modifiers* mat_list;
    tSpecial_volume* vol;
    tFace_ref* face_refs[4];
    tPhysics_object* objects[4];
    tNon_car_spec* non_car;
    // GLOBAL: CARMA2_HW 0x006793f8
    // GLOBAL: CARMA2_SW 0x00673b68
    // GLOBAL: CARMA2_CHEAT 0x00680ca8
    static br_scalar slide_dist;
    // GLOBAL: CARMA2_HW 0x006793fc
    // GLOBAL: CARMA2_SW 0x00673b6c
    // GLOBAL: CARMA2_CHEAT 0x00680cac
    static br_scalar stop_timer;

    (void)stop_timer;

    c->curvature += c->field_0x1260;
    c->field_0x195c = NULL;
    normnum = 0;
    BrVector3Set(&t, 0.0f, 0.0f, 0.0f);
    BrVector3Set(&f, 0.0f, 0.0f, 0.0f);
#ifdef REC2_FIX_BUGS
    BrVector3Set(&fb, 0.0f, 0.0f, 0.0f);
#endif
    mat = &c->car_master_actor->t.t.mat;
    mat_list = gCurrent_race.material_modifiers;
    vol = c->collision_info->last_special_volume;
    for (n = 0; n < 4; n++) {
        face_refs[n] = NULL;
        objects[n] = NULL;
    }
    BrVector3Negate(&b, (br_vector3*)mat->m[1]);
    wheelratio = (c->wpos[2].v[2] - c->centre_of_mass_world_scale.v[2]) / (c->wpos[0].v[2] - c->centre_of_mass_world_scale.v[2]);
    BrVector3Scale(&vel, &c->collision_info->v, WORLD_SCALE);
    BrVector3Set(&c->road_normal, 0.0f, 0.0f, 0.0f);
    for (n = 0; n < 4; n++) {
        BrMatrix34ApplyP(&wheel_pos[n], &c->wpos[n], mat);
    }
    max_d = MAX(c->susp_height[0], c->susp_height[1]) + 0.5f;
    BrVector3Scale(&tv, &b, max_d);
    MultiFindFloorInBoxM(4, wheel_pos, &tv, c->nor, d, c, face_refs, objects);
    material_override = 0;
    if (c->collision_info->last_special_volume != NULL && c->collision_info->last_special_volume->material_modifier_index != 0) {
        material_override = c->collision_info->last_special_volume->material_modifier_index;
    }
    for (n = 0; n < 4; n++) {
        d[n] *= max_d;
        if (face_refs[n] == NULL || face_refs[n]->material == NULL || face_refs[n]->material->identifier == NULL) {
            c->material_index[n] = 0;
        } else {
            i = material_override;
            if (i == 0) {
                i = face_refs[n]->material->identifier[0] - ('0' - 1);
            }
            c->material_index[n] = i;
            if (c->material_index[n] < 0 || c->material_index[n] > 11) {
                c->material_index[n] = 0;
            }
            if (face_refs[n]->material != NULL && face_refs[n]->material->identifier != NULL
                    && strlen(face_refs[n]->material->identifier) == 11 && face_refs[n]->material->identifier[5] == '|') {
                c->oil_remaining[n] = 0.0f;
            }
        }
        BrMatrix34TApplyV(&norm[n], &c->nor[n], mat);
        if (mat_list[c->material_index[n]].bumpiness != 0.0f) {
            DoBumpiness(c, wheel_pos, norm, d, n);
        }
        if (d[n] < -0.5f || d[n] > c->susp_height[n >> 1] + c->wheel_dam_offset[n ^ 2] * WORLD_SCALE) {
            force[n] = 0.0f;
            d[n] = c->susp_height[n >> 1];
        } else {
            BrVector3Accumulate(&c->road_normal, &norm[n]);
            normnum++;
            d[n] -= c->wheel_dam_offset[n ^ 2] * WORLD_SCALE;
            force[n] = (c->susp_height[n >> 1] - d[n]) * c->sk[n >> 1] - c->sb[n >> 1] * ((d[n] - c->oldd[n]) / dt);
            if (c->oldd[n] == c->susp_height[n >> 1] && BrVector3Dot(&c->nor[n], &vel) > -0.01f
                    && force[n] > 10.0f * 2.0f / 4.0f * c->collision_info->M) {
                force[n] = 10.0f * 2.0f / 4.0f * c->collision_info->M;
                d[n] = c->susp_height[n >> 1];
            }
            if (force[n] < 0.0f) {
                force[n] = 0.0f;
            }
            f.v[1] += force[n];
            t.v[0] -= (c->wpos[n].v[2] - c->centre_of_mass_world_scale.v[2]) * force[n];
            t.v[2] += (c->wpos[n].v[0] - c->centre_of_mass_world_scale.v[0]) * force[n];
        }
        d_change[n] = c->oldd[n] - d[n];
        c->oldd[n] = d[n];
        if (objects[n] != NULL) {
            c->field_0x195c = objects[n];
            non_car = objects[n]->owner;
            if (force[n] != 0.0f
                    && (!objects[n]->disable_move_rotate || non_car == NULL || objects[n]->flags_0x238 != 1
                        || non_car->driver > eDriver_5 || !(non_car->flags & 0x10000))) {
                objects[n]->disable_move_rotate = 0;
                ts = dt * force[n] / WORLD_SCALE;
                BrVector3Scale(&tv, &b, ts / objects[n]->M);
                BrVector3Accumulate(&objects[n]->v, &tv);
                if (gNet_mode == eNet_mode_host) {
                    c->collision_info->field_0x49c = gPHIL_last_physics_tick + 40;
                }
                if (!(objects[n]->flags & 0x800)) {
                    BrMatrix34ApplyP(&tv, &objects[n]->cmpos, &objects[n]->transform_matrix);
                    BrVector3InvScale(&tv2, &wheel_pos[n], WORLD_SCALE);
                    BrVector3Sub(&tv, &tv2, &tv);
                    BrVector3Cross(&tv2, &tv, &b);
                    BrVector3Scale(&tv2, &tv2, ts);
                    BrMatrix34TApplyV(&tv, &tv2, &objects[n]->transform_matrix);
                    Vector3Div(&tv, &tv, &objects[n]->I);
                    BrVector3Accumulate(&objects[n]->omega, &tv);
                }
            }
        }
    }
    if (objects[0] != NULL && objects[0] == objects[1] && objects[0] == objects[2] && objects[0] == objects[3]) {
        NudgeObject(objects[0]);
    }
    if (!c->collision_info->disable_move_rotate || normnum != 0) {
        if (c->collision_info->disable_move_rotate) {
            SetGroupDoingNothingFlag(c->collision_info, 0);
        }
        if (c == NULL || c->driver <= eDriver_5 || !c->wall_climber_mode || Vector3IsZero(&c->road_normal)) {
            ts = 1.0f;
            if (vol != NULL) {
                ts = c->collision_info->water_depth_factor * (1.0f - vol->gravity_multiplier);
                if (c->underwater_ability) {
                    ts = (br_scalar)(ts * 0.6);
                }
                ts = 1.0f - ts;
            }
            ts = 10.0f * ts * gGravity_multiplier;
            BrVector3Set(&c->collision_info->field_0x54, 0.0f, -ts * dt / WORLD_SCALE, 0.0f);
            ts = c->collision_info->M * ts;
            f.v[0] -= mat->m[0][1] * ts;
            f.v[1] -= mat->m[1][1] * ts;
            f.v[2] -= mat->m[2][1] * ts;
        } else {
            BrVector3Normalise(&tv, &c->road_normal);
            BrVector3Scale(&tv, &tv, -(10.0f * c->collision_info->M));
            BrVector3Accumulate(&f, &tv);
        }
    }
    if (c != NULL && c->driver >= eDriver_net_human) {
        ffb_force = SteeringSelfCentre(c, dt, &c->road_normal);
        if (c->driver == eDriver_local_human) {
            PDSetJoystickForceFFB(ffb_force - (int)((d_change[2] - d_change[3]) * -30000.0f));
        }
    }
    if (normnum == 0) {
        c->max_force_front = 0.0f;
        c->max_force_rear = 0.0f;
        StopSkid(c);
    } else {
        SmashFacesWithWheels(c, face_refs, force, wheel_pos);
        BrVector3Scale(&vcs, &c->collision_info->velocity_car_space, WORLD_SCALE);
        BrVector3NormaliseQuick(&c->road_normal, &c->road_normal);
        ts = c->road_normal.v[0] * mat->m[0][1] + c->road_normal.v[1] * mat->m[1][1] + c->road_normal.v[2] * mat->m[2][1];
        if (c != NULL && c->driver > eDriver_5 && c->wall_climber_mode) {
            ts = 1.0f;
        }
        ts = ts * mat_list[c->material_index[0]].down_force;
        if (ts > 0.0f) {
            ts = (br_scalar)fabs(vcs.v[2]) * c->collision_info->M * 10.0f * ts / c->downforce_to_weight;
            if (ts > 10.0f * c->collision_info->M) {
                ts = 10.0f * c->collision_info->M;
            }
            if (c->number_of_wheels_on_ground == 4 && c->oldd[2] == c->susp_height[1] && c->oldd[3] == c->susp_height[1]) {
                BrVector3Scale(&tv, (br_vector3*)mat->m[2], c->wpos[2].v[2]);
                BrVector3Accumulate(&tv, (br_vector3*)mat->m[3]);
                BrVector3Scale(&b, &b, c->wpos[0].v[2] - c->wpos[2].v[2]);
                findfloor(&tv, &b, norm, &ts2);
                if (ts2 > 1.0f) {
                    c->down_force_flag = 1;
                }
            } else if (c->down_force_flag && (c->oldd[2] < c->susp_height[1] || c->oldd[3] < c->susp_height[1])) {
                c->down_force_flag = 0;
            }
            if (c->down_force_flag) {
                ts = ts * ((c->wpos[2].v[2] - c->centre_of_mass_world_scale.v[2]) / (c->wpos[2].v[2] - c->wpos[0].v[2]));
                t.v[0] += (c->wpos[0].v[2] - c->centre_of_mass_world_scale.v[2]) * ts;
            }
            f.v[1] -= ts;
        }
        BrVector3Scale(&vplane, &c->road_normal, BrVector3Dot(&vcs, &c->road_normal));
        BrVector3Sub(&vplane, &vcs, &vplane);
        if (vplane.v[2] < 0.0f) {
            direction = 1.0f;
        } else {
            direction = -1.0f;
        }
        deltaomega = direction * c->curvature * BrVector3Length(&vplane) - BrVector3Dot(&c->collision_info->omega, &c->road_normal);
        rightplane.v[0] = c->road_normal.v[1];
        rightplane.v[1] = -c->road_normal.v[0];
        rightplane.v[2] = 0.0f;
        BrVector3Normalise(&rightplane, &rightplane);
        inertia = WORLD_SCALE * WORLD_SCALE * c->collision_info->I.v[1];
        ts = inertia / dt * deltaomega;
        ffi = ts / (c->wpos[2].v[2] - c->wpos[0].v[2]);
        fbi = -ffi;
        a.v[0] = 0.0f;
        a.v[1] = c->road_normal.v[2];
        a.v[2] = -c->road_normal.v[1];
        BrVector3Normalise(&a, &a);
        ts3 = BrVector3Dot(&vplane, &rightplane);
        ts = (c->wpos[0].v[2] - c->centre_of_mass_world_scale.v[2]) * BrVector3Dot(&vplane, &a) * (br_scalar)fabs(c->curvature);
        if (c->curvature > 0.0f) {
            ts = ts3 + ts;
        } else {
            ts = ts3 - ts;
        }
        ts *= -(c->collision_info->M / dt);
        ts -= BrVector3Dot(&f, &rightplane);
        ts = ts / (1.0f - wheelratio);
        ffi += ts;
        fbi += ts * -wheelratio;
        ffk = ffi * (c->wpos[0].v[2] - c->wpos[2].v[2]) * c->curvature;
        max_retardation = c->collision_info->M * BrVector3Dot(&vcs, &a) / dt + BrVector3Dot(&f, &a);
        ts5 = c->brake_force * c->mu.v[0] / (c->mu.v[0] + c->mu.v[1] / c->traction_multiplier);
        ts6 = c->brake_force - ts5;
        i = (c->damage_units[eDamage_lr_brake].damage_level + c->damage_units[eDamage_rr_brake].damage_level) / 2;
        if (i > 20) {
            ts = 1.0f - (br_scalar)(i - 20) / 80.0f;
            ts5 = ts5 * ts * ts;
        }
        i = (c->damage_units[eDamage_lf_brake].damage_level + c->damage_units[eDamage_rf_brake].damage_level) / 2;
        if (i > 20) {
            ts = 1.0f - (br_scalar)(i - 20) / 80.0f;
            ts6 = ts6 * ts * ts;
        }
        ts2 = (force[0] + force[1]) * c->rolling_resistance + ts5;
        ts3 = (force[2] + force[3]) * c->steerable_rolling_resistance + ts6;
        ts5 = c->wpos[0].v[2] - c->wpos[2].v[2];
        ts5 = (br_scalar)sqrt(ts5 * ts5 * c->curvature * c->curvature + 1.0f);
        front_retardation = ts3 / ts5;
        ts4 = ts2 + front_retardation;
        if (fabs(max_retardation) < fabs(ts4)) {
            ts2 = ts2 * (br_scalar)fabs(max_retardation / ts4);
            front_retardation = front_retardation * (br_scalar)fabs(max_retardation / ts4);
        }
        if ((ts2 + ts3) * max_retardation < 0.0f) {
            ts2 = -ts2;
            front_retardation = -front_retardation;
        }
        fbk = c->acc_force - ts2;
        if (c->keys.brake && c->damage_units[eDamage_lr_brake].damage_level < 60 && c->damage_units[eDamage_rr_brake].damage_level < 60) {
            fbk += -(max_retardation - (ts2 + front_retardation));
            c->gear = 0;
        }
        fbk = fbk / c->traction_multiplier;
        modf = BR_LENGTH2(fbi, fbk) / 2.0f;
        GetOilFrictionFactors(c, &fl_oil_factor, &fr_oil_factor, &rl_oil_factor, &rr_oil_factor);
        for (n = 0; n < 4; n++) {
            if (c->oil_remaining[n] != 0.0f) {
                ConditionallyNoteSkid(c, face_refs, n);
            }
        }
        if (c != NULL && c->driver > eDriver_5) {
            friction_number = c->grip_multiplier;
        } else {
            friction_number = 1.0f;
        }
        BrVector3Sub(&tv2, &c->wpos[0], &c->centre_of_mass_world_scale);
        BrVector3Cross(&tv, &c->collision_info->omega, &tv2);
        BrVector3Accumulate(&tv, &vcs);
        if (c != NULL && c->driver >= eDriver_net_human
                && (((c->keys.left || c->joystick.left > 32768) && c->curvature > 0.0f && deltaomega > 0.1 && tv.v[0] > 0.0f)
                    || ((c->keys.right || c->joystick.right > 32768) && c->curvature < 0.0f && deltaomega < 0.1 && tv.v[0] < 0.0f))
                && direction > 0.0f) {
            ts = c->mu.v[0];
        } else {
            ts2 = (br_scalar)fabs(tv.v[0]) / 10.0f;
            if (ts2 > 1.0f) {
                ts2 = 1.0f;
            }
            ts = (c->mu.v[2] - c->mu.v[0]) * ts2 + c->mu.v[2];
        }
        maxfl = ts * (br_scalar)sqrt(force[0]) * (friction_number * rl_oil_factor) * mat_list[c->material_index[0]].tyre_road_friction;
        maxfr = ts * (br_scalar)sqrt(force[1]) * (friction_number * rr_oil_factor) * mat_list[c->material_index[1]].tyre_road_friction;
        c->max_force_rear = maxfl + maxfr;
        if (rl_oil_factor == 1.0f && rr_oil_factor == 1.0f && c->traction_control && 2.0f * modf > c->max_force_rear && c->acc_force > 0.0f
                && (c == NULL || c->driver < eDriver_net_human || (c->target_revs > 1000.0f && c->gear > 0))) {
            ts2 = fbk;
            if (fbk * fbk > 4.0f * (modf * modf)) {
                ts3 = 0.0f;
            } else {
                ts3 = (br_scalar)sqrt(4.0f * (modf * modf) - fbk * fbk);
            }
            if (ts3 < c->max_force_rear) {
                if (ts2 >= 0.0f) {
                    sign = 1;
                } else {
                    sign = -1;
                }
                ts4 = 0.99f * c->max_force_rear;
                ts3 = ts2 - sign * (br_scalar)sqrt(ts4 * ts4 - ts3 * ts3);
                if (fabs(ts3) < fabs(ts2)) {
                    ts2 = ts3;
                }
            } else {
                c->torque = (br_scalar)(-(c->revs * c->revs) / (10000.0f * 10000.0f) - 0.1);
            }
            fbk -= ts2;
            modf = BR_LENGTH2(fbi, fbk) / 2.0f;
        } else if (c != NULL && c->driver >= eDriver_net_human && c->gear > 0 && c->revs > c->target_revs && !c->traction_control) {
            if (!c->keys.change_down) {
                c->traction_control = 1;
            }
            ts = 1.0f - (c->revs - c->target_revs) / (br_scalar)(c->gear * 400);
            if (ts < 0.4f) {
                ts = 0.4f;
            }
            maxfl = maxfl * ts;
            maxfr = maxfr * ts;
        }
        if (fabs(fbi) > maxfl + maxfr && maxfl + maxfr > 0.1) {
            ts3 = (maxfl + maxfr) / (br_scalar)fabs(fbi) * dt;
            fbi *= ts3;
            fbk = fbk * ts3 * c->traction_multiplier;
            ts4 = ffi * (c->wpos[2].v[2] - c->centre_of_mass_world_scale.v[2]);
            ts = -((ffi * ffi + ffk * ffk) / c->collision_info->M + ts4 * ts4 / inertia);
            ts2 = (BrVector3Dot(&vplane, &rightplane) + fbi / c->collision_info->M) * ffi;
            ts2 += BrVector3Dot(&vplane, &a) * ffk;
            ts2 += BrVector3Dot(&c->collision_info->omega, &c->road_normal) * (c->wpos[2].v[2] - c->centre_of_mass_world_scale.v[2]) * ffi;
            ts2 += fbi * ((c->wpos[0].v[2] - c->centre_of_mass_world_scale.v[2]) * (c->wpos[2].v[2] - c->centre_of_mass_world_scale.v[2])) / inertia * ffi;
            if (fabs(ts) > 0.1) {
                ts = ts2 / (ts * dt);
                ffi = ts * ffi;
                ffk = ts * ffk;
            }
            fbi /= ts3;
            fbk /= ts3 * c->traction_multiplier;
        }
        ffk += -front_retardation;
        ffi += front_retardation * c->curvature * (c->wpos[0].v[2] - c->wpos[2].v[2]);
        if (modf > 0.0001f) {
            fbi = fbi / (2.0f * modf);
            fbk = fbk / (2.0f * modf);
        }
        fbk = fbk * c->traction_multiplier;
        force[0] = modf;
        force[1] = modf;
        c->wheel_slip = 0;
        switch ((force[0] > maxfl) + ((force[1] > maxfr) << 1)) {
        case 0:
            stop_timer = 0.0f;
            break;
        case 1:
            force[0] = maxfl * c->friction_slipping_reduction;
            force[1] += modf - force[0];
            if (force[1] > maxfr) {
                if (maxfr > 0.1) {
                    v = (force[1] - maxfr) / maxfr;
                    if (v >= ((c == &gProgram_state.current_car) ? 20.0f : 60.0f)) {
                        ConditionallyNoteSkid(c, face_refs, 1);
                    }
                    SkidNoise(c, 1, v, c->material_index[1]);
                }
                force[1] = maxfr * c->friction_slipping_reduction;
                c->wheel_slip |= 2;
            } else {
                stop_timer = 0.0f;
            }
            break;
        case 2:
            force[1] = maxfr * c->friction_slipping_reduction;
            force[0] += modf - force[1];
            if (force[0] > maxfl) {
                if (maxfl > 0.1) {
                    v = (force[0] - maxfl) / maxfl;
                    if (v >= ((c == &gProgram_state.current_car) ? 20.0f : 60.0f)) {
                        ConditionallyNoteSkid(c, face_refs, 0);
                    }
                    SkidNoise(c, 0, v, c->material_index[0]);
                }
                force[0] = maxfl * c->friction_slipping_reduction;
                c->wheel_slip |= 2;
            } else {
                stop_timer = 0.0f;
            }
            break;
        case 3:
            force[0] = maxfl * c->friction_slipping_reduction;
            force[1] = maxfr * c->friction_slipping_reduction;
            c->wheel_slip |= 2;
            v = (2.0f * modf - maxfl - maxfr) / (maxfl + maxfr);
            if (v >= ((c == &gProgram_state.current_car) ? 20.0f : 60.0f)) {
                if (maxfl > 0.1) {
                    ConditionallyNoteSkid(c, face_refs, 0);
                }
                if (maxfr > 0.1) {
                    ConditionallyNoteSkid(c, face_refs, 1);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1) {
                    SkidNoise(c, 0, v, c->material_index[0]);
                }
            } else {
                if (maxfr > 0.1) {
                    SkidNoise(c, 1, v, c->material_index[1]);
                }
            }
            break;
        }
        if (c->wheel_slip && c->curvature * c->turn_speed > 0.0f && fabs(fbi) > 0.01f && c->curvature * fbi < 0.0f
                && !c->keys.brake && !c->keys.change_down) {
            c->turn_speed = 0.0f;
        }
        modf = BR_LENGTH2(ffi, ffk) / 2.0f;
        if (modf > 0.0001f) {
            ffi = ffi / (2.0f * modf);
            ffk = ffk / (2.0f * modf);
        }
        maxfl = c->mu.v[1] * (br_scalar)sqrt(force[2]) * (friction_number * fl_oil_factor) * mat_list[c->material_index[2]].tyre_road_friction;
        maxfr = c->mu.v[1] * (br_scalar)sqrt(force[3]) * (friction_number * fr_oil_factor) * mat_list[c->material_index[3]].tyre_road_friction;
        if (c != NULL && c->driver == eDriver_local_human) {
            PDSetJoystickFrictionFFB((int)((maxfl * force[2] + maxfr * force[3]) * 225.0f / (1.0f + c->speed * 750.0f)));
        }
        c->max_force_front = maxfl + maxfr;
        force[2] = modf;
        force[3] = modf;
        switch ((force[2] > maxfl) + ((force[3] > maxfr) << 1)) {
        case 1:
            force[2] = maxfl * c->friction_slipping_reduction;
            force[3] += modf - force[2];
            if (force[3] > maxfr) {
                if (maxfr > 0.1) {
                    v = (force[3] - maxfr) / maxfr;
                    if (v >= ((c == &gProgram_state.current_car) ? 20.0f : 60.0f)) {
                        ConditionallyNoteSkid(c, face_refs, 3);
                    }
                    SkidNoise(c, 3, v, c->material_index[3]);
                }
                force[3] = maxfr * c->friction_slipping_reduction;
                c->wheel_slip |= 1;
            }
            break;
        case 2:
            force[3] = maxfr * c->friction_slipping_reduction;
            force[2] += modf - force[3];
            if (force[2] > maxfl) {
                if (maxfl > 0.1) {
                    v = (force[2] - maxfl) / maxfl;
                    if (v >= ((c == &gProgram_state.current_car) ? 20.0f : 60.0f)) {
                        ConditionallyNoteSkid(c, face_refs, 2);
                    }
                    SkidNoise(c, 2, v, c->material_index[2]);
                }
                force[2] = maxfl * c->friction_slipping_reduction;
                c->wheel_slip |= 1;
            }
            break;
        case 3:
            force[2] = maxfl * c->friction_slipping_reduction;
            force[3] = maxfr * c->friction_slipping_reduction;
            c->wheel_slip |= 1;
            v = (2.0f * modf - maxfl - maxfr) / (maxfl + maxfr);
            if (v >= ((c == &gProgram_state.current_car) ? 20.0f : 60.0f)) {
                if (maxfl > 0.1) {
                    ConditionallyNoteSkid(c, face_refs, 2);
                }
                if (maxfr > 0.1) {
                    ConditionallyNoteSkid(c, face_refs, 3);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1) {
                    SkidNoise(c, 2, v, c->material_index[2]);
                }
            } else {
                if (maxfr > 0.1) {
                    SkidNoise(c, 3, v, c->material_index[3]);
                }
            }
            break;
        }
        BrVector3Scale(&fb, &a, fbk);
        BrVector3Scale(&tv, &rightplane, fbi);
        BrVector3Accumulate(&fb, &tv);
        BrVector3Scale(&ff, &a, ffk);
        BrVector3Scale(&tv, &rightplane, ffi);
        BrVector3Accumulate(&ff, &tv);

        BrVector3Copy(&a, &c->wpos[0]);
        a.v[1] -= c->oldd[0];
        BrVector3Sub(&a, &a, &c->centre_of_mass_world_scale);
        BrVector3Scale(&b, &fb, force[0]);
        BrVector3Accumulate(&f, &b);
        BrVector3Cross(&tv, &a, &b);
        BrVector3Accumulate(&t, &tv);

        BrVector3Copy(&a, &c->wpos[1]);
        a.v[1] -= c->oldd[1];
        BrVector3Sub(&a, &a, &c->centre_of_mass_world_scale);
        BrVector3Scale(&b, &fb, force[1]);
        BrVector3Accumulate(&f, &b);
        BrVector3Cross(&tv, &a, &b);
        BrVector3Accumulate(&t, &tv);

        BrVector3Copy(&a, &c->wpos[2]);
        a.v[1] -= c->oldd[2];
        BrVector3Sub(&a, &a, &c->centre_of_mass_world_scale);
        BrVector3Scale(&b, &ff, force[2]);
        BrVector3Accumulate(&f, &b);
        BrVector3Cross(&tv, &a, &b);
        BrVector3Accumulate(&t, &tv);

        BrVector3Copy(&a, &c->wpos[3]);
        a.v[1] -= c->oldd[3];
        BrVector3Sub(&a, &a, &c->centre_of_mass_world_scale);
        BrVector3Scale(&b, &ff, force[3]);
        BrVector3Accumulate(&f, &b);
        BrVector3Cross(&tv, &a, &b);
        BrVector3Accumulate(&t, &tv);
    }
    c->number_of_wheels_on_ground = normnum;
    BrMatrix34ApplyV(&b, &f, mat);
    BrVector3Scale(&a, &t, dt);
    ApplyTorque(c->collision_info, &a);
    BrVector3Scale(&a, &b, dt / (c->collision_info->M * WORLD_SCALE));
    BrVector3Accumulate(&c->collision_info->v, &a);
    if (BrVector3LengthSquared(&c->collision_info->v) < 0.1f
            && (!(c->keys.acc || c->joystick.acc > 0) || c->gear == 0)
            && !(c->keys.dec || c->joystick.dec > 0)
            && c->bounce_rate == 0.0f) {
        if (BrVector3Length(&c->collision_info->omega) < 0.05) {
            if (vol == NULL) {
                ts = BrVector3Length(&b);
            } else {
                if (c != NULL && c->driver > eDriver_5 && c->underwater_ability) {
                    ts2 = (br_scalar)(1.0 - 0.6 * (1.0 - vol->gravity_multiplier));
                } else {
                    ts2 = vol->gravity_multiplier;
                }
                ts = BrVector3Length(&b) / ts2 / gGravity_multiplier;
            }
            if (ts < c->collision_info->M || (c->keys.brake && normnum > 2)) {
                if (slide_dist == 100.0f) {
                    slide_dist = 0.0f;
                }
                if (slide_dist > 0.5f) {
                    BrVector3SetFloat(&c->collision_info->v, 0.0f, 0.0f, 0.0f);
                    BrVector3SetFloat(&c->collision_info->omega, 0.0f, 0.0f, 0.0f);
                    slide_dist = 0.5f;
                }
            }
        }
    }
    slide_dist += dt;
    if (slide_dist > 1.0f) {
        slide_dist = 100.0f;
    }
    AddDrag(c, c->collision_info, dt);
    if (c != NULL && c->driver >= eDriver_net_human) {
        c->acc_force = -(fb.v[2] * force[0]) - fb.v[2] * force[1];
    }
    c->curvature -= c->field_0x1260;
}

// DoRevs

// ScrapeNoise

// FUNCTION: CARMA2_HW 0x0041bed0
// FUNCTION: CARMA2_SW 0x0041bf00
// FUNCTION: CARMA2_CHEAT 0x0041c130
void C2_HOOK_FASTCALL SkidNoise(tCar_spec* pC, int pWheel_num, br_scalar pV, int pMaterial) {
    br_vector3 pos;
    br_vector3 world_pos;
    br_vector3 wv;
    br_vector3 wvw;
    br_scalar ts;
    br_vector3 tv;
    // GLOBAL: CARMA2_HW 0x00679408
    // GLOBAL: CARMA2_SW 0x00673b78
    // GLOBAL: CARMA2_CHEAT 0x00680cb8
    static int last_skid_vol[2];
    int i;

    i = IRandomBetween(0, 1);
    if (pV < 5.0f) {
        return;
    }
    if (gCurrent_race.material_modifiers[pMaterial].tyre_noise_index == -1) {
        return;
    }
    if (IRandomBetween(0, 4) != 0) {
        return;
    }
    last_skid_vol[i] = (int)(pV * 10.0f);
    pos.v[0] = pC->wpos[pWheel_num].v[0];
    pos.v[1] = pC->wpos[pWheel_num].v[1] - pC->oldd[pWheel_num];
    pos.v[2] = pC->wpos[pWheel_num].v[2];
    BrMatrix34ApplyP(&world_pos, &pos, &pC->car_master_actor->t.t.mat);
    BrVector3Scale(&world_pos, &world_pos, 1.0f / WORLD_SCALE);
    if (!DRS3SoundStillPlaying(gSkid_tag[i]) || (pC != NULL && pC->driver == eDriver_local_human && gLast_car_to_skid[i] != pC)) {
        gSkid_tag[i] = DRS3StartSound3D(gCar_outlet, IRandomBetween(0, 4) + 9000, &world_pos, &pC->vel, 1, last_skid_vol[i], IRandomBetween(0xc000, 0x14000), 0x10000);
        gLast_car_to_skid[i] = pC;
    }
    if (gCurrent_race.material_modifiers[pMaterial].smoke_type == 1) {
        BrVector3Cross(&wv, &pC->collision_info->omega, &pos);
        BrVector3Scale(&tv, &pC->collision_info->velocity_car_space, WORLD_SCALE);
        BrVector3Accumulate(&wv, &tv);
        ts = -BrVector3Dot(&wv, &pC->road_normal);
        BrVector3Scale(&wvw, &pC->road_normal, ts);
        BrVector3Accumulate(&wv, &wvw);
        BrMatrix34ApplyV(&wvw, &wv, &pC->car_master_actor->t.t.mat);
        CreatePuffOfSmoke(&world_pos, &wvw, pV * 0.04f, 1.0f, 4);
    }
}

inline void C2_HOOK_FASTCALL StopSkid(tCar_spec* pC) {

    if (gLast_car_to_skid[0] == pC) {
        DRS3StopSound(gSkid_tag[0]);
    }
    if (gLast_car_to_skid[1] == pC) {
        DRS3StopSound(gSkid_tag[1]);
    }
}

// FUNCTION: CARMA2_HW 0x0041ddc0
// FUNCTION: CARMA2_SW 0x0041ddf0
// FUNCTION: CARMA2_CHEAT 0x0041e020
void C2_HOOK_FASTCALL CrashNoise(br_vector3* pForce, br_vector3* pPosition, int pMaterial) {
    // GLOBAL: CARMA2_HW 0x00679410
    // GLOBAL: CARMA2_SW 0x00673b80
    // GLOBAL: CARMA2_CHEAT 0x00680cc0
    static int crunch_tag;
    int vol;
    br_vector3 velocity;

    vol = (int)(60.0 * BrVector3Length(pForce));
    if (gCurrent_race.material_modifiers[pMaterial].crash_noise_index != -1) {
        if (vol > 255) {
            vol = 255;
        }
        if ((crunch_tag == 0 || !DRS3SoundStillPlaying(crunch_tag)) && vol > 30) {
            gLast_crunch_vol = vol;
            BrVector3Set(&velocity, 0.0f, 0.0f, 0.0f);
            crunch_tag = DRS3StartSound3D(gCar_outlet,
                gMetal_crunch_sound_id[IRandomBetween(0, REC2_ASIZE(gMetal_crunch_sound_id) - 1)],
                pPosition, &velocity, 1, vol, IRandomBetween(0xc000, 0x14000), 0x10000);
        }
    }
}

// CrushAndDamageCar

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// STUB: CARMA2_HW 0x0041c1b0
// STUB: CARMA2_SW 0x0041c1e0
int C2_HOOK_FASTCALL ProcessForcesCallback(void* pArg1, float *pArg2, int pArg3) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0041e310
// FUNCTION: CARMA2_SW 0x0041e340
int C2_HOOK_FASTCALL ProcessJointForcesCallBack(void* pArg1, void* pArg2, void* pArg3) {

    return 0;
}

inline void C2_HOOK_FASTCALL MultiFindFloorInBoxM(int pNum_rays, br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d, tCar_spec* c, tFace_ref** pFace_refs, tPhysics_object** pObjects) {
    br_vector3 aa[4];
    br_vector3 bb;
    int i;

    for (i = 0; i < pNum_rays; i++) {
        BrVector3InvScale(&aa[i], &a[i], WORLD_SCALE);
        d[i] = 2.0f;
    }
    BrVector3InvScale(&bb, b, WORLD_SCALE);
    MultiFindFloorInBoxBU(pNum_rays, aa, &bb, nor, d, c, pFace_refs);
    MultiRayCastOnObjects(pNum_rays, aa, &bb, nor, d, c, pFace_refs, pObjects);
}

inline void C2_HOOK_FASTCALL MultiRayCastOnObjects(int pNum_rays, br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d, tCar_spec* c, tFace_ref** pFace_refs, tPhysics_object** pObjects) {
    br_bounds3 bounds;
    br_vector3 start;
    br_vector3 end;
    br_vector3 dir;
    br_vector3 point;
    br_vector3 normal;
    br_scalar t;
    int i;
    int j;
    int k;
    tPhysics_object* object;
    tPhysics_shape* shape;

    BrVector3Copy(&bounds.min, &a[0]);
    BrVector3Copy(&bounds.max, &a[0]);
    for (i = 1; i < pNum_rays; i++) {
        for (j = 0; j < 3; j++) {
            bounds.min.v[j] = MIN(a[i].v[j], bounds.min.v[j]);
            bounds.max.v[j] = MAX(a[i].v[j], bounds.max.v[j]);
        }
    }
    for (j = 0; j < 3; j++) {
        if (b->v[j] > 0.0f) {
            bounds.max.v[j] += b->v[j];
        } else {
            bounds.min.v[j] += b->v[j];
        }
    }
    for (k = 0; k < gDrivable_on_count; k++) {
        object = gDrivable_on_list[k];
        if (!BoundsOverlapTest(&bounds, &object->field_0xf4)) {
            continue;
        }
        BrMatrix34TApplyV(&dir, b, &object->transform_matrix);
        for (shape = object->shape; shape != NULL; shape = shape->common.next) {
            for (i = 0; i < pNum_rays; i++) {
                DrMatrix34ApplyLPInverse(&start, &a[i], &object->transform_matrix);
                BrVector3Add(&end, &dir, &start);
                ShapeRayCast(&start, &end, shape, &point, &t, &normal);
                if (t < d[i]) {
                    BrVector3Negate(&normal, &normal);
                    d[i] = t;
                    BrMatrix34ApplyV(&nor[i], &normal, &object->transform_matrix);
                    pFace_refs[i] = NULL;
                    pObjects[i] = object;
                }
            }
        }
    }
}

inline void C2_HOOK_FASTCALL MultiFindFloorInBoxBU(int pNum_rays, br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d, tCar_spec* c, tFace_ref** pFace_refs) {
    br_vector3 nor2;
    int i;
    int l;
    br_scalar dist[4];
    tFace_ref* face_ref;

    i = c->collision_info->box_face_start;
    face_ref = &gFace_list__car[i];
    for (; i < c->collision_info->box_face_end; i++, face_ref++) {
        if (gEliminate_faces && (face_ref->flags & 0x80) != 0) {
            continue;
        }
        MultiRayCheckSingleFace(pNum_rays, face_ref, a, b, &nor2, dist);
        for (l = 0; l < pNum_rays; l++) {
            if (dist[l] < d[l]) {
                pFace_refs[l] = face_ref;
                d[l] = dist[l];
                BrVector3Copy(&nor[l], &nor2);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041e340
// FUNCTION: CARMA2_SW 0x0041e370
// FUNCTION: CARMA2_CHEAT 0x0041e5a0
void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, gTrack_actor, nor, t, material, NULL);
}

// FUNCTION: CARMA2_HW 0x0041e360
// FUNCTION: CARMA2_SW 0x0041e390
// FUNCTION: CARMA2_CHEAT 0x0041e5c0
void C2_HOOK_FASTCALL findfloor(br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d) {
    br_material* material;
    br_vector3 aa;
    br_vector3 bb;

    BrVector3Set(&aa, a->v[0] / WORLD_SCALE, a->v[1] / WORLD_SCALE, a->v[2] / WORLD_SCALE);
    BrVector3Set(&bb, b->v[0] / WORLD_SCALE, b->v[1] / WORLD_SCALE, b->v[2] / WORLD_SCALE);
    FindFace(&aa, &bb, nor, d, &material);
}

// FUNCTION: CARMA2_HW 0x0041e480
// FUNCTION: CARMA2_SW 0x0041e4b0
int C2_HOOK_FASTCALL FindFloorInBoxBU(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pDistance, tPhysics_object* pObject) {
    br_vector3 nor2;
    br_vector3 collision_position;
    int i;
    int j;
    br_scalar dist;
    tFace_ref* face_ref;

    *pDistance = 2.f;
    i = pObject->box_face_start;
    face_ref = &gFace_list__car[i];
    for (; i < pObject->box_face_end; i++, face_ref++) {
        if (gEliminate_faces && (face_ref->flags & 0x80) != 0) {
            continue;
        }
        CheckSingleFace(face_ref, pPosition, pDirection, &nor2, &dist, &collision_position);
        if (dist < *pDistance) {
            *pDistance = dist;
            j = i;
            BrVector3Copy(pNormal, &nor2);
        }
    }
    if (*pDistance < 2.f) {
        i = gFace_list__car[j].material->identifier[0] - ('0' - 1);
        if (i >= 0 && i < 11) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041e580
// FUNCTION: CARMA2_SW 0x0041e5b0
// FUNCTION: CARMA2_CHEAT 0x0041e7e0
void C2_HOOK_FASTCALL CancelPendingCunningStunt(void) {
    gQuite_wild_end = 0;
    gQuite_wild_start = 0;
    gOn_me_wheels_start = 0;
    gWoz_upside_down_at_all = 0;
    gWild_start = 0;
}

// SetAmbientPratCam

void C2_HOOK_FASTCALL SetTextureBits(tCar_spec* pCar) {
    pCar->field_0x18cc = 0;
    if (pCar->keys.brake || (pCar->brake_force != 0.f && (float)fabs(pCar->collision_info->velocity_car_space.v[2]) > 1.0 / (2000.0 * WORLD_SCALE_D))) {
        pCar->field_0x18cc |= 0x4;
    }
    if (pCar->gear < 0 || (!(pCar != NULL && pCar->driver == eDriver_local_human) && pCar->collision_info->velocity_car_space.v[2] > 0.0f)) {
        pCar->field_0x18cc |= 0x8;
    }
}

// FUNCTION: CARMA2_HW 0x0041e5a0
// FUNCTION: CARMA2_SW 0x0041e5d0
// FUNCTION: CARMA2_CHEAT 0x0041e800
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {
    int i;
    tCar_spec* car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gear, 0x135c);

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car->car_master_actor->render_style != BR_RSTYLE_NONE && !gAction_replay_mode) {
            SetTextureBits(car);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041e660
// STUB: CARMA2_SW 0x0041e690
// STUB: CARMA2_CHEAT 0x0041e8c0
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0041f110
// FUNCTION: CARMA2_SW 0x0041f140
// FUNCTION: CARMA2_CHEAT 0x0041f370
void C2_HOOK_FASTCALL DoLODCarModels(void) {
    int i;
    int j;
    tCar_spec* car;
    float level;
    float factor;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_simplification_factor) != 2);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_simplification_factor[0]) != 5);

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_oppo - 1 && car->car_master_actor->render_style != BR_RSTYLE_NONE) {
            level = Vector3DistanceSquared(&car->car_master_actor->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]);
            factor = gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level];
            if (!(factor >= 0.001f)) {
                level = BR_SCALAR_MAX;
            } else {
                level /= factor;
            }
#ifdef REC2_SW
            if (gNet_mode != eNet_mode_none && (gNet_players[gIt_or_fox].car == car || NetPlayerFromCar(car)->field_0x80 != 0)) {
                level = 0.0f;
            }
#endif
            for (j = car->count_detail_levels - 1; j > 0; j--) {
                if (level > car->detail_levels[j]) {
                    break;
                }
            }
            if (j > 0) {
                int variant;

                variant = j;
                DRActorEnumRecurse(car->car_model_actor, SwitchCarModel, &variant);
                car->field_0xe18 = variant;
            } else if (!gAction_replay_mode && car->use_shell_model && car->shell_model != NULL) {
                DRActorEnumRecurse(car->car_model_actor, TurnOffNonGroovers, NULL);
                car->car_model_actor->type = BR_ACTOR_MODEL;
                car->car_model_actor->model = car->shell_model;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f280
// FUNCTION: CARMA2_SW 0x0041f300
// FUNCTION: CARMA2_CHEAT 0x0041f4e0
intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data* crush_data = pActor->user;

    (void)pData;
    if (crush_data != NULL && crush_data->groove == NULL) {
        pActor->type = BR_ACTOR_NONE;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041f2a0
// FUNCTION: CARMA2_SW 0x0041f320
// FUNCTION: CARMA2_CHEAT 0x0041f500
void C2_HOOK_FASTCALL DoComplexCarModels(void) {
    int i;
    tCar_spec* car;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > 5) {
            SwitchCarModels(car, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f300
// FUNCTION: CARMA2_SW 0x0041f380
// FUNCTION: CARMA2_CHEAT 0x0041f560
void C2_HOOK_FASTCALL ResetCarScreens(void) {
    int cat;
    int car_count;
    int i;
    tCar_spec* the_car;

    for (cat = eVehicle_self; cat <= eVehicle_rozzer; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            the_car->collision_info->last_special_volume = NULL;
        }
    }
    MungeCarGraphics(gFrame_period);
}

// FUNCTION: CARMA2_HW 0x0041f360
// FUNCTION: CARMA2_SW 0x0041f3e0
void C2_HOOK_FASTCALL ToggleCarToCarCollisions(void) {

    gCar_car_collisions = !gCar_car_collisions;
    if (gCar_car_collisions) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Car Car Collisions");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Ghost Cars");
    }
}

// FUNCTION: CARMA2_HW 0x0041f3b0
// FUNCTION: CARMA2_SW 0x0041f430
void C2_HOOK_FASTCALL SwapCar(void) {
}

// FUNCTION: CARMA2_HW 0x0041f3c0
// FUNCTION: CARMA2_SW 0x0041f440
void C2_HOOK_FASTCALL AdjustDownForce(void) {
    char s[100];
    tCar_spec* c;

    c = gCar_to_view;
    c->downforce_to_weight += 50.f;
    if (c->downforce_to_weight > 2000.f) {
        c->downforce_to_weight = 50.f;
    }
    if (c->downforce_to_weight > 300.f) {
        c->downforce_to_weight = 2000.f;
    }
    sprintf(s, "DownForceSpeed %f", c->downforce_to_weight);
    NewTextHeadupSlot(4, 0, 1500, -4, s);
}

// FUNCTION: CARMA2_HW 0x0041f450
// FUNCTION: CARMA2_SW 0x0041f4d0
void C2_HOOK_FASTCALL FreezeMechanics(void) {

    gFreeze_mechanics = !gFreeze_mechanics;
    if (gFreeze_mechanics) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Mechanics Frozen");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Thawed Mechanics");
    }
}

// FlyCar

// FUNCTION: CARMA2_HW 0x0041fbe0
// FUNCTION: CARMA2_SW 0x0041fc60
// FUNCTION: CARMA2_CHEAT 0x0041fe40
float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(const tCar_spec* pCar_spec) {
    float rear;
    float rear_susp;
    float rear_min;
    float front_min;
    float min_value;

    rear = pCar_spec->wpos[0].v[1];
    rear_susp = pCar_spec->susp_height[0];
    rear_min = rear - rear_susp;
    front_min = pCar_spec->wpos[2].v[1] - pCar_spec->susp_height[1];

    rear_min = rear_min / WORLD_SCALE;
    front_min = front_min / WORLD_SCALE;
    min_value = pCar_spec->collision_info->bb1.min.v[1];

    if (rear_min < front_min && rear_min < min_value) {
        return rear_min;
    }
    if (front_min < min_value) {
        return front_min;
    }
    return min_value;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041fc60
// STUB: CARMA2_SW 0x0041fce0
// STUB: CARMA2_CHEAT 0x0041fec0
void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0041fe50
// FUNCTION: CARMA2_SW 0x0041fed0
// FUNCTION: CARMA2_CHEAT 0x004200b0
int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3) {
    int r;

    if (!gProgram_state.racing) {
        return 1;
    }
    gTesting_car_for_sensible_place = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info,
        gPHIL_list_collision_infos,
        pVec3,
        &gWorld_callbacks);
    gTesting_car_for_sensible_place = 0;
    return r;
}

// FUNCTION: CARMA2_HW 0x0041ff00
// FUNCTION: CARMA2_SW 0x0041ff80
tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* pActor) {
    if (!gPHIL_doing_physics && !gTesting_car_for_sensible_place) {
        return NULL;
    } else {
        return DoPullActorFromWorld(pActor);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041ff20
// STUB: CARMA2_SW 0x0041ffa0
tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* pActor) {
    NOT_IMPLEMENTED();
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

// FUNCTION: CARMA2_HW 0x004207e0
// FUNCTION: CARMA2_SW 0x00420860
// FUNCTION: CARMA2_CHEAT 0x00420a40
void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix) {
    tU8 column_x;
    tU8 column_z;
    br_actor* parent;

    BrMatrix34Copy(&pActor->t.t.mat, pMatrix);
    if (pActor->parent == gNon_track_actor) {
        return;
    }
    XZToColumnXZ(&column_x, &column_z, pActor->t.t.mat.m[3][0], pActor->t.t.mat.m[3][2], &gProgram_state.track_spec);
    parent = gProgram_state.track_spec.columns[column_z][column_x].actor_0x0;
    if (parent != pActor->parent && parent != NULL) {
        BrActorRemove(pActor);
        BrActorAdd(gProgram_state.track_spec.columns[column_z][column_x].actor_0x0, pActor);
    }
}

// FUNCTION: CARMA2_HW 0x00420880
// FUNCTION: CARMA2_SW 0x00420900
// FUNCTION: CARMA2_CHEAT 0x00420ae0
int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs) {

    if (pCar->collision_info->box_face_ref != gFace_num__car) {
        if (pCar->collision_info->box_face_ref != gFace_num__car - 1 || pCar->collision_info->box_face_start <= gFace_count) {
            return 0;
        }
    }
    *pFace_refs = &gFace_list__car[pCar->collision_info->box_face_start];
    return pCar->collision_info->box_face_end - pCar->collision_info->box_face_start;
}

// FUNCTION: CARMA2_HW 0x004208f0
// FUNCTION: CARMA2_SW 0x00420970
// FUNCTION: CARMA2_CHEAT 0x00420b50
void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x00420900
// FUNCTION: CARMA2_SW 0x00420980
// FUNCTION: CARMA2_CHEAT 0x00420b60
void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_NONE;
}

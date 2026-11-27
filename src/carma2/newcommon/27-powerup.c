/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "27-powerup.h"

#include "00-car.h"
#include "01-network.h"
#include "08-loading1.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "23-piping.h"
#include "25-grafdata.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "39-oil.h"
#include "41-utility.h"
#include "42-input.h"
#include "43-pratcam.h"

#include "49-physics.h"
#include "51-smash.h"
#include "53-controls.h"
#include "58-crush2.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "joystick.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006a0a70
// GLOBAL: CARMA2_SW 0x0069aac8
// GLOBAL: CARMA2_CHEAT 0x006a85e8
int gInventory[20];

// GLOBAL: CARMA2_HW 0x006a0a6c
// GLOBAL: CARMA2_SW 0x0069aac4
// GLOBAL: CARMA2_CHEAT 0x006a85e4
int gINT_006a0a6c;

// GLOBAL: CARMA2_HW 0x006a0960
// GLOBAL: CARMA2_SW 0x0069a9b8
// GLOBAL: CARMA2_CHEAT 0x006a84d8
tRepulse_link gRepulse_links[6] = { 0 };

// GLOBAL: CARMA2_HW 0x006a0a50
// GLOBAL: CARMA2_SW 0x0069aaa8
// GLOBAL: CARMA2_CHEAT 0x006a85c8
tU8* gRace_powerup_respawn_bools = NULL;

// GLOBAL: CARMA2_HW 0x006a0a54
// GLOBAL: CARMA2_SW 0x0069aaac
// GLOBAL: CARMA2_CHEAT 0x006a85cc
tPowerup* gPowerup_array = NULL;

// GLOBAL: CARMA2_HW 0x006a0ad0
// GLOBAL: CARMA2_SW 0x0069ab28
// GLOBAL: CARMA2_CHEAT 0x006a8648
int gNumber_of_powerups;

// GLOBAL: CARMA2_HW 0x006a0a58
// GLOBAL: CARMA2_SW 0x0069aab0
// GLOBAL: CARMA2_CHEAT 0x006a85d0
int gInventory_selected = 0;

// GLOBAL: CARMA2_HW 0x006a0a5c
// GLOBAL: CARMA2_SW 0x0069aab4
// GLOBAL: CARMA2_CHEAT 0x006a85d4
int gINT_006a0a5c = 0;

// GLOBAL: CARMA2_HW 0x006a0ac0
// GLOBAL: CARMA2_SW 0x0069ab18
// GLOBAL: CARMA2_CHEAT 0x006a8638
br_pixelmap* gFizzle_in[3];

// GLOBAL: CARMA2_HW 0x006a0ad4
// GLOBAL: CARMA2_SW 0x0069ab2c
// GLOBAL: CARMA2_CHEAT 0x006a864c
const char* gPowerup_txt_path;

// GLOBAL: CARMA2_HW 0x006a0ad8
// GLOBAL: CARMA2_SW 0x0069ab30
// GLOBAL: CARMA2_CHEAT 0x006a8650
tU32* gReal_render_palette;

// GLOBAL: CARMA2_HW 0x006a0adc
// GLOBAL: CARMA2_SW 0x0069ab34
// GLOBAL: CARMA2_CHEAT 0x006a8654
int gCollision_info_uid_counter;

// GLOBAL: CARMA2_HW 0x006a0ae0
// GLOBAL: CARMA2_SW 0x0069ab38
// GLOBAL: CARMA2_CHEAT 0x006a8658
br_model* gModel_powerup_armor;

// GLOBAL: CARMA2_HW 0x006a0ae4
// GLOBAL: CARMA2_SW 0x0069ab3c
// GLOBAL: CARMA2_CHEAT 0x006a865c
br_model* gModel_powerup_power;

// GLOBAL: CARMA2_HW 0x006a0ae8
// GLOBAL: CARMA2_SW 0x0069ab40
// GLOBAL: CARMA2_CHEAT 0x006a8660
br_model* gModel_powerup_offence;

// GLOBAL: CARMA2_HW 0x00763480
// GLOBAL: CARMA2_SW 0x007599e0
// GLOBAL: CARMA2_CHEAT 0x0076ab20
tSlot_info gCost_APO;

// GLOBAL: CARMA2_HW 0x0075b900
// GLOBAL: CARMA2_SW 0x00751e60
// GLOBAL: CARMA2_CHEAT 0x00762fa0
tSlot_info gTrade_in_value_APO;

// GLOBAL: CARMA2_HW 0x0074d380
// GLOBAL: CARMA2_SW 0x007438e0
// GLOBAL: CARMA2_CHEAT 0x00754a20
tSlot_info gSubstitution_value_APO;

// GLOBAL: CARMA2_HW 0x00761c60
// GLOBAL: CARMA2_SW 0x007581c0
// GLOBAL: CARMA2_CHEAT 0x00769300
tSlot_info gPotential_substitution_value_APO;

// GLOBAL: CARMA2_HW 0x0074d1a4
// GLOBAL: CARMA2_SW 0x00743704
// GLOBAL: CARMA2_CHEAT 0x00754844
int gNet_powerup_time_replacement;

// GLOBAL: CARMA2_HW 0x007447d8
// GLOBAL: CARMA2_SW 0x0073b298
// GLOBAL: CARMA2_CHEAT 0x0074be78
int gPickup_respawn_min_time_ms;

// GLOBAL: CARMA2_HW 0x007447e8
// GLOBAL: CARMA2_SW 0x0073b2a8
// GLOBAL: CARMA2_CHEAT 0x0074be88
int gPickup_respawn_max_extra_time_ms;

// GLOBAL: CARMA2_HW 0x007447cc
// GLOBAL: CARMA2_SW 0x0073b28c
// GLOBAL: CARMA2_CHEAT 0x0074be6c
int gCount_killed_peds;

// GLOBAL: CARMA2_HW 0x007447d0
// GLOBAL: CARMA2_SW 0x0073b290
// GLOBAL: CARMA2_CHEAT 0x0074be70
int gBlind_pedestrians;

// GLOBAL: CARMA2_HW 0x007447dc
// GLOBAL: CARMA2_SW 0x0073b29c
// GLOBAL: CARMA2_CHEAT 0x0074be7c
float gPed_gravity_multiplier;

// GLOBAL: CARMA2_HW 0x007447e4
// GLOBAL: CARMA2_SW 0x0073b2a4
// GLOBAL: CARMA2_CHEAT 0x0074be84
int gPed_dismemberfest;

// GLOBAL: CARMA2_HW 0x007447f8
// GLOBAL: CARMA2_SW 0x0073b2b8
// GLOBAL: CARMA2_CHEAT 0x0074be98
int gPanicking_peds;

// GLOBAL: CARMA2_HW 0x00705560
// GLOBAL: CARMA2_SW 0x006fc020
// GLOBAL: CARMA2_CHEAT 0x0070cc00
tShit_mine gShit_mines[20] = { 0 };

// GLOBAL: CARMA2_HW 0x007059c0
// GLOBAL: CARMA2_SW 0x006fc480
// GLOBAL: CARMA2_CHEAT 0x0070d060
int gCount_mutant_tail_parts = 0;

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x007059c4
// GLOBAL: CARMA2_SW 0x006fc484
// GLOBAL: CARMA2_CHEAT 0x0070d064
undefined4 gDAT_007059c4 = 0;

// GLOBAL: CARMA2_HW 0x007059c8
// GLOBAL: CARMA2_SW 0x006fc488
// GLOBAL: CARMA2_CHEAT 0x0070d068
undefined4 gDAT_007059c8[6] = { 0 };
#endif

// GLOBAL: CARMA2_HW 0x007059e0
// GLOBAL: CARMA2_SW 0x006fc4a0
// GLOBAL: CARMA2_CHEAT 0x0070d080
tHeadup_icon gPickedup_powerups[20] = { 0 };

// GLOBAL: CARMA2_HW 0x00705540
// GLOBAL: CARMA2_SW 0x006fc000
// GLOBAL: CARMA2_CHEAT 0x0070cbe0
int gMutant_tail_state;

// GLOBAL: CARMA2_HW 0x00705b78
// GLOBAL: CARMA2_SW 0x006fc638
// GLOBAL: CARMA2_CHEAT 0x0070d218
float gMass_mutant_tail_link;

// GLOBAL: CARMA2_HW 0x006a0458
// GLOBAL: CARMA2_SW 0x0069a4b0
// GLOBAL: CARMA2_CHEAT 0x006a7fd0
tRespawn_powerup gRespawn_powerups[100] = { 0 };

// GLOBAL: CARMA2_HW 0x006a0908
// GLOBAL: CARMA2_SW 0x0069a960
// GLOBAL: CARMA2_CHEAT 0x006a8480
tPhysics_object* gMutant_tail_object = NULL;

// GLOBAL: CARMA2_HW 0x006a0acc
// GLOBAL: CARMA2_SW 0x0069ab24
// GLOBAL: CARMA2_CHEAT 0x006a8644
tPhysics_joint* gMutant_tail_first_joint;

// GLOBAL: CARMA2_HW 0x00705b74
// GLOBAL: CARMA2_SW 0x006fc634
// GLOBAL: CARMA2_CHEAT 0x0070d214
float gMass_mutant_tail_ball;

// GLOBAL: CARMA2_HW 0x0065ebbc
// GLOBAL: CARMA2_SW 0x0065951c
// GLOBAL: CARMA2_CHEAT 0x00666324
float gMass_mine = 1.0f;

// GLOBAL: CARMA2_HW 0x006a0454
// GLOBAL: CARMA2_SW 0x0069a4ac
// GLOBAL: CARMA2_CHEAT 0x006a7fcc
int gCount_cloaked_cars;

// GLOBAL: CARMA2_HW 0x006a0910
// GLOBAL: CARMA2_SW 0x0069a968
// GLOBAL: CARMA2_CHEAT 0x006a8488
tCar_spec* gCloaked_cars[12];

// GLOBAL: CARMA2_HW 0x006a0940
// GLOBAL: CARMA2_SW 0x0069a998
// GLOBAL: CARMA2_CHEAT 0x006a84b8
int gInventory_cycling;

// GLOBAL: CARMA2_HW 0x006a0944
// GLOBAL: CARMA2_SW 0x0069a99c
// GLOBAL: CARMA2_CHEAT 0x006a84bc
int gNumber_of_icons;

// GLOBAL: CARMA2_HW 0x006a0950
// GLOBAL: CARMA2_SW 0x0069a9a8
int gFizzle_height;

// GLOBAL: CARMA2_HW 0x006a0954
// GLOBAL: CARMA2_SW 0x0069a9ac
// GLOBAL: CARMA2_CHEAT 0x006a84cc
tU32 gInventory_timeout;

// GLOBAL: CARMA2_HW 0x0079ec50
// GLOBAL: CARMA2_SW 0x007951b0
// GLOBAL: CARMA2_CHEAT 0x007a62f0
int gInstant_handbrake;

// FUNCTION: CARMA2_HW 0x004d8d10
// FUNCTION: CARMA2_SW 0x004d5240
// FUNCTION: CARMA2_CHEAT 0x004de8d0
char* C2_HOOK_FASTCALL GetPowerupMessage(int pPowerup_index) {

    return gPowerup_array[pPowerup_index].message;
}

// GetNextGoodyTime

// DoPowerupTextHeadup

// QueueIconGoing

// FUNCTION: CARMA2_HW 0x004d9670
// FUNCTION: CARMA2_SW 0x004d5b40
// FUNCTION: CARMA2_CHEAT 0x004df230
void C2_HOOK_FASTCALL QueueIconComing(tPowerup* pPowerup, tHeadup_icon* pHeadup_icons, int* pCount_icons) {
    int i;

    for (i = 0; i < *pCount_icons; i++) {
        if (pPowerup == pHeadup_icons[i].powerup) {
            pHeadup_icons[i].fizzle_stage = 3;
            pHeadup_icons[i].fizzle_direction = -1;
            pHeadup_icons[i].fizzle_start = GetTotalTime();
            break;
        }
    }
}

inline void C2_HOOK_FASTCALL LosePowerupX(tPowerup* pPowerup, int pTell_net_players) {
    tNet_message* the_message;

    if (pPowerup->got_time != 0 && pPowerup->lose_proc != NULL) {
        pPowerup->lose_proc(pPowerup, pPowerup->car);
    }
    if (pPowerup->net_type & 0x80) {
        PipeSinglePowerupLose(pPowerup, pPowerup->car);
    }
    if (gNet_mode != eNet_mode_none) {
        the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_powerup, 0);
        the_message->contents.powerup.event = 2;
        the_message->contents.powerup.player = gLocal_net_ID;
        the_message->contents.powerup.powerup_index = pPowerup - gPowerup_array;
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
    }
    gProgram_state.current_car.powerups[pPowerup - gPowerup_array] = 0;
    if (pPowerup->initial_value == 0) {
        QueueIconComing(pPowerup, gPickedup_powerups, &gNumber_of_icons);
    }
    pPowerup->got_time = 0;
}

inline void C2_HOOK_FASTCALL LosePowerup(tPowerup* pPowerup) {

    LosePowerupX(pPowerup, 1);
}

// LoseKeyboardPowerupN

// KeyboardPowerupFinished

// LoseAllSimilarPowerups

// ExcludedPowerup

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d9150
// STUB: CARMA2_SW 0x004d5680
int C2_HOOK_FASTCALL GotPowerupX(tCar_spec* pCar, int pIndex, int pArg3, int pMessage, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GotPowerupXX

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d8d40
// STUB: CARMA2_SW 0x004d5270
// STUB: CARMA2_CHEAT 0x004de900
void C2_HOOK_FASTCALL GotPowerupEarwig(tCar_spec* pCar, int pIndex, int pMessage, int pSelect_inventory) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// AutoGoody

// FUNCTION: CARMA2_HW 0x004d8d30
// FUNCTION: CARMA2_SW 0x004d5260
// FUNCTION: CARMA2_CHEAT 0x004de8f0
void C2_HOOK_FASTCALL GotPowerup(tCar_spec* pCar, int pIndex) {
    GotPowerupEarwig(pCar, pIndex, 1, 1);
}

// DRMapFindMeAPixie


// STUB: CARMA2_HW 0x004d96c0
// STUB: CARMA2_SW 0x004d5b90
// STUB: CARMA2_CHEAT 0x004df280
void C2_HOOK_FASTCALL LoadPowerups(void) {
    NOT_IMPLEMENTED();
}

// InitRepulseEffects

// FUNCTION: CARMA2_HW 0x004da290
// FUNCTION: CARMA2_SW 0x004d6760
// FUNCTION: CARMA2_CHEAT 0x004dfe50
tPhysics_object* C2_HOOK_FASTCALL MungeBoxObject(br_model* pModel, float pMass) {
    tPhysics_object* box;

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 0x4d8);

    box = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    box->shape = (tPhysics_shape*)AllocateShapeBox(kMem_collision_shape);
    box->shape->common.bb = pModel->bounds;
    FillInShape(box->shape);
    box->uid = (tU8)gCollision_info_uid_counter;
    gCollision_info_uid_counter++;
    UpdateCollisionObject(box);
    box->M = pMass;
    BrVector3Set(&box->I,
        (REC2_SQR((pModel->bounds.max.v[1] - pModel->bounds.min.v[1]) * 0.5f) + REC2_SQR((pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) * 0.5f)) * pMass / 12.0f,
        (REC2_SQR((pModel->bounds.max.v[0] - pModel->bounds.min.v[0]) * 0.5f) + REC2_SQR((pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) * 0.5f)) * pMass / 12.0f,
        (REC2_SQR((pModel->bounds.max.v[0] - pModel->bounds.min.v[0]) * 0.5f) + REC2_SQR((pModel->bounds.max.v[1] - pModel->bounds.min.v[1]) * 0.5f)) * pMass / 12.0f);
    BrVector3SetFloat(&box->field_0x54, 0.0f, (br_scalar)(-1.0 / (2.5 * 6.9)), 0.0f);
    box->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&box->actor->t.t.mat, &box->transform_matrix);
    box->actor->model = pModel;
    box->box_face_ref = (tU16)(gFace_num__car - 2);
    return box;
}

// FUNCTION: CARMA2_HW 0x004da400
// FUNCTION: CARMA2_SW 0x004d68d0
// FUNCTION: CARMA2_CHEAT 0x004dffc0
tPhysics_object* C2_HOOK_FASTCALL MungeShereObject(br_model* pModel, float pWeight) {
    tPhysics_object* collision_info;
    tPhysics_shape* shape;

    C2_HOOK_BUG_ON(sizeof(tPhysics_object) != 0x4d8);

    collision_info = BrMemAllocate(sizeof(tPhysics_object), kMem_collision_object);
    collision_info->shape = (tPhysics_shape*)AllocateShapeSphere(kMem_collision_shape);
    BrVector3Set(&collision_info->shape->sphere.sphere.center, 0.0f, 0.0f, 0.0f);
    collision_info->shape->sphere.sphere.radius = (pModel->bounds.max.v[0] - pModel->bounds.min.v[0]
        + pModel->bounds.max.v[1] - pModel->bounds.min.v[1]
        + pModel->bounds.max.v[2] - pModel->bounds.min.v[2]) / 6.0f;
    FillInShape(collision_info->shape);
    collision_info->uid = (tU8)gCollision_info_uid_counter;
    gCollision_info_uid_counter++;
    UpdateCollisionObject(collision_info);
    shape = collision_info->shape;
    collision_info->M = pWeight;
    BrVector3Set(&collision_info->I,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.0f,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.0f,
        REC2_SQR(shape->sphere.sphere.radius) * pWeight / 6.0f);
    BrVector3SetFloat(&collision_info->field_0x54, 0.0f, (br_scalar)(-1.0 / (2.5 * 6.9)), 0.0f);
    collision_info->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&collision_info->actor->t.t.mat, &collision_info->transform_matrix);
    collision_info->actor->model = pModel;
    collision_info->box_face_ref = (tU16)(gFace_num__car - 2);
    return collision_info;
}

// InitTail

// FUNCTION: CARMA2_HW 0x004d9d00
// FUNCTION: CARMA2_SW 0x004d61d0
// FUNCTION: CARMA2_CHEAT 0x004df8c0
void C2_HOOK_FASTCALL ThisObjectHitAMineSoSpamItUpUsingThisPowerupsSettings(tPhysics_object* pObject, int pPowerup_index) {
    tPedestrian* ped;
    int old_dismemberfest;

    if (pObject->field_0x1a0 & 0xf0000) {
        if (IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()) {
            ped = ((tPed_character_instance*)pObject->owner)->ped;
            if (!(ped->character->field_0x14 & 0x4) && !gPHIL_doing_physics) {
                SetThisPedPhysicing(ped);
            }
            if (pPowerup_index != 14) {
                BrVector3Set(&pObject->v, 0.0f, 0.0f, 0.0f);
            }
            pObject->v.v[1] = (br_scalar)(pObject->v.v[1] + FRandomBetween(0.7f, 1.0f) / (pObject->M * WORLD_SCALE_D));
            pObject->omega.v[2] = (br_scalar)(pObject->omega.v[2] + (FRandomPosNeg(0.5f) * TAU) / pObject->M);
            pObject->omega.v[0] = (br_scalar)(pObject->omega.v[0] + (FRandomPosNeg(0.5f) * TAU) / pObject->M);
            pObject->disable_move_rotate = 0;
            old_dismemberfest = gPed_dismemberfest;
            gPed_dismemberfest = 1;
            DamagePedestrian(((tPed_character_instance*)pObject->owner)->ped, 500, 0x61, 0x61, gProgram_state.current_car.collision_info);
            gPed_dismemberfest = old_dismemberfest;
        }
    } else if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        pObject->v.v[1] = (br_scalar)(pObject->v.v[1] + FRandomBetween(3.0f, 4.0f) / (pObject->M * WORLD_SCALE_D));
        pObject->omega.v[2] = (br_scalar)(pObject->omega.v[2] + (FRandomPosNeg(1.75f) * TAU) / pObject->M);
        pObject->omega.v[0] = (br_scalar)(pObject->omega.v[0] + (FRandomPosNeg(1.75f) * TAU) / pObject->M);
        pObject->disable_move_rotate = 0;
    } else {
        pObject->v.v[1] = (br_scalar)(pObject->v.v[1] + FRandomBetween(1.0f, 2.0f) / (pObject->M * WORLD_SCALE_D));
        pObject->omega.v[2] = (br_scalar)(pObject->omega.v[2] + (FRandomPosNeg(0.5f) * TAU) / pObject->M);
        pObject->omega.v[0] = (br_scalar)(pObject->omega.v[0] + (FRandomPosNeg(0.5f) * TAU) / pObject->M);
        pObject->disable_move_rotate = 0;
    }
}

// InitMineShit

// STUB: CARMA2_HW 0x004d9ea0
// STUB: CARMA2_SW 0x004d6370
// STUB: CARMA2_CHEAT 0x004dfa60
void C2_HOOK_FASTCALL InitPowerups(void) {
    NOT_IMPLEMENTED();
}

// RemoveRepulseEffect

// ResetRepulseEffects

// FUNCTION: CARMA2_HW 0x004da600
// FUNCTION: CARMA2_SW 0x004d6ad0
// FUNCTION: CARMA2_CHEAT 0x004e01c0
void C2_HOOK_FASTCALL EmptyInventory(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gInventory) != 20);

    for (i = 0; i < (int)REC2_ASIZE(gInventory); i++) {
        gInventory[i] = -1;
    }
    gInventory_selected = -1;
    gINT_006a0a6c = 0;
    gINT_006a0a5c = 0;
}

// ResetPowerups

// STUB: CARMA2_HW 0x004da730
// STUB: CARMA2_SW 0x004d6c00
// STUB: CARMA2_CHEAT 0x004e02f0
void C2_HOOK_FASTCALL CloseDownPowerUps(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004da870
// FUNCTION: CARMA2_SW 0x004d6d40
// FUNCTION: CARMA2_CHEAT 0x004e0430
void C2_HOOK_FASTCALL NewRepulseRay(tU32 pTime, tCar_spec* pCar, tCar_spec* pVictim_car, br_vector3* pPos_origin, br_vector3* pPos_victim, float pRadius1, float pRadius2) {
    int i;
    int link_index;
    int oldest_link;
    tU32 oldest_time;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRepulse_link, field_0x24, 0x24);

    link_index = -1;
    oldest_time = 0x7fffffff;
    for (i = 0; i < (int)REC2_ASIZE(gRepulse_links); i++) {
        if (gRepulse_links[i].time == 0) {
            link_index = i;
            break;
        }
        if (gRepulse_links[i].time < oldest_time) {
            oldest_time = gRepulse_links[i].time;
            oldest_link = i;
        }
    }
    if (link_index < 0) {
        link_index = oldest_link;
    }
    gRepulse_links[link_index].pos_origin = pPos_origin;
    gRepulse_links[link_index].pos_victim = pPos_victim;
    gRepulse_links[link_index].time = pTime;
    gRepulse_links[link_index].field_0x10 = -1;
    gRepulse_links[link_index].field_0x18 = pRadius1;
    gRepulse_links[link_index].field_0x1c = pRadius2;
    gRepulse_links[link_index].car = pCar;
    gRepulse_links[link_index].field_0x24 = pVictim_car;
}

// RemoveRepulsiveReferences

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004db8e0
// STUB: CARMA2_SW 0x004d7de0
void C2_HOOK_FASTCALL MungeRepulseRays(tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004da960
// FUNCTION: CARMA2_SW 0x004d6e30
// FUNCTION: CARMA2_CHEAT 0x004e0520
void C2_HOOK_FASTCALL AdjustRepulseRay(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    C2_HOOK_BUG_ON(sizeof(tRepulse_link) != 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRepulse_link, pos_origin, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRepulse_link, pos_victim, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRepulse_link, actor, 0xc);

    gRepulse_links[pIndex].pos_origin = pOrigin;
    gRepulse_links[pIndex].pos_victim = pVictim;
    gRepulse_links[pIndex].time = pTime;
}

// FUNCTION: CARMA2_HW 0x004da990
// FUNCTION: CARMA2_SW 0x004d6e60
// FUNCTION: CARMA2_CHEAT 0x004e0550
void C2_HOOK_FASTCALL ResetRepulseRay(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gRepulse_links); i++) {
        if (gRepulse_links[i].actor->parent == gNon_track_actor) {
            BrActorRemove(gRepulse_links[i].actor);
        }
        gRepulse_links[i].time = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004daff0
// FUNCTION: CARMA2_SW 0x004d74c0
// FUNCTION: CARMA2_CHEAT 0x004e0bb0
int C2_HOOK_FASTCALL DrawSinglePowerupIcon(int pDraw, int pTime, tPowerup* pPowerup, tHeadup_icon* pIcon, int pX, int pY, int pUpdate_x) {
    br_pixelmap* fizzle_pix;
    char s[8];
    int timer;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dx, 0x294);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, power_up_space_between_icon_text_dy, 0x2a0);

    if (pIcon != NULL && pIcon->fizzle_stage < 4) {
        fizzle_pix = gFizzle_in[pPowerup->fizzle_type];
        if (pDraw) {
#ifdef REC2_HW
            PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 1, pTime);
#endif
#ifdef REC2_SW
            DRPixelmapRectangleMaskedCopy(gBack_screen, pX, pY,
                fizzle_pix, 0, gFizzle_height * pIcon->fizzle_stage,
                fizzle_pix->width, gFizzle_height);
#endif
        }
        if (pUpdate_x) {
            return pX + fizzle_pix->width;
        }
        return 0;
    }
    if (pDraw) {
#ifdef REC2_HW
        PrintPowerupIconIn3D(pX, pY, pIcon, pPowerup, 0, pTime);
#endif
#ifdef REC2_SW
        DRPixelmapRectangleMaskedCopy(gBack_screen, pX, pY,
            pPowerup->icon, 0, 0,
            pPowerup->icon->width, pPowerup->icon->height);
#endif
    }
    if (pPowerup->initial_value < 0) {
        sprintf(s, "%d", -pPowerup->value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->initial_value > 0) {
        TimerString(pPowerup->value, s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    } else if (pPowerup->type == ePowerup_timed) {

        timer = pPowerup->lose_time - pTime;
        TimerString(MAX(0, timer), s, 0, 0, 1);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[1],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[1], s);
        }
    } else if (pPowerup->current_value > 0) {

        sprintf(s, "%d", pPowerup->current_value);
        if (pDraw) {
            DRPixelmapText(gBack_screen,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx,
                pY + gCurrent_graf_data->power_up_space_between_icon_text_dy,
                &gFonts[2],
                s,
                pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + 30);
        }
        if (pUpdate_x) {
            return pX + gCurrent_graf_data->power_up_space_between_icon_text_dx + DRTextWidth(&gFonts[2], s);
        }
    }
    return 0;
}

inline void C2_HOOK_FASTCALL DrawPowerupIcons2(tU32 pTime, tHeadup_icon* pHeadup_icons, int* pCount_icons, int pMax_count_icons, int pX, int pY) {
    int i;
    int y;
    tPowerup* the_powerup;
    tHeadup_icon* the_icon;

    C2_HOOK_BUG_ON(sizeof(tHeadup_icon) != 0x14);

    y = pY;
    for (i = 0, the_icon = pHeadup_icons; i < *pCount_icons; i++, the_icon++) {
        if (i >= pMax_count_icons) {
            break;
        }
        the_powerup = the_icon->powerup;
        if (the_powerup->icon == NULL) {
            continue;
        }
        y += gCurrent_graf_data->power_up_icon_y_pitch;
        if (the_icon->fizzle_stage < 4) {
            if (the_icon->fizzle_direction < 0) {
                the_icon->fizzle_stage = 3 - (pTime - the_icon->fizzle_start) / 150;
            } else {
                the_icon->fizzle_stage = (pTime - the_icon->fizzle_start) / 150;
            }
            if (the_icon->fizzle_stage > 4) {
                the_icon->fizzle_stage = 4;
            } else if (the_icon->fizzle_stage < 0) {
                memcpy(the_icon, the_icon + 1, sizeof(tHeadup_icon) * (*pCount_icons - i - 1));
                (*pCount_icons)--;
                continue;
            }
        }
        DrawSinglePowerupIcon(1, pTime, the_icon->powerup, the_icon, pX, y, 0);
    }
}

inline void C2_HOOK_FASTCALL DrawKeyPowerups(tU32 pTime) {
    int x;
    int i;

    if (gInventory_cycling) {
        if (gInventory_selected >= 0) {
            if (GetTotalTime() >= gInventory_timeout) {
                gInventory_timeout = 0;
                gInventory_cycling = 0;
            }
            x = gCurrent_graf_data->keyboard_powerup_x;
            i = gInventory_selected;
            do {
                if (gInventory[i] >= 0) {
                    x = DrawSinglePowerupIcon(0, pTime, &gPowerup_array[gInventory[i]], NULL,
                        x, gCurrent_graf_data->keyboard_powerup_y, 1) + gCurrent_graf_data->keyboard_powerup_spacing_x;
                }
                i++;
                if (i >= (int)REC2_ASIZE(gInventory)) {
                    i = 0;
                }
            } while (i != gInventory_selected);
            if (x >= 0) {
                DimRectangle(gBack_screen, gCurrent_graf_data->keyboard_powerup_bg_x, gCurrent_graf_data->keyboard_powerup_bg_y,
                    x, gCurrent_graf_data->keyboard_powerup_bg_bottom, 1);
                i = gInventory_selected;
                x = gCurrent_graf_data->keyboard_powerup_x;
#ifdef REC2_HW
                DRPixelmapText(gBack_screen,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 1, gCurrent_graf_data->keyboard_powerup_bg_bottom - 9,
                    &gFonts[2], gPowerup_array[gInventory[gInventory_selected]].message,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 100);
#endif
#ifdef REC2_SW
                DRPixelmapText(gBack_screen,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 4, gCurrent_graf_data->keyboard_powerup_bg_bottom - 8,
                    &gFonts[2], gPowerup_array[gInventory[gInventory_selected]].message,
                    gCurrent_graf_data->keyboard_powerup_bg_x + 100);
#endif
                do {
                    if (gInventory[i] >= 0) {
                        tPowerup* the_powerup = &gPowerup_array[gInventory[i]];

                        if (gInventory_cycling) {
                            x = DrawSinglePowerupIcon(1, pTime, the_powerup, NULL,
                                x, gCurrent_graf_data->keyboard_powerup_y, 1) + gCurrent_graf_data->keyboard_powerup_spacing_x;
                        }
                    }
                    i++;
                    if (i >= (int)REC2_ASIZE(gInventory)) {
                        i = 0;
                    }
                } while (i != gInventory_selected);
            }
        }
    } else {
        if (gInventory_selected >= 0) {
            DrawSinglePowerupIcon(1, pTime, &gPowerup_array[gInventory[gInventory_selected]], NULL,
                gCurrent_graf_data->keyboard_powerup_x, gCurrent_graf_data->keyboard_powerup_y, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004da9d0
// FUNCTION: CARMA2_SW 0x004d6ea0
// FUNCTION: CARMA2_CHEAT 0x004e0590
void C2_HOOK_FASTCALL ToggleInventory(void) {

    gInventory_cycling = !gInventory_cycling;
    if (gInventory_cycling) {
        gInventory_timeout = GetTotalTime() + 3000;
    }
}

// RightInventory

// LeftInventory

// FUNCTION: CARMA2_HW 0x004dad00
// FUNCTION: CARMA2_SW 0x004d71d0
// FUNCTION: CARMA2_CHEAT 0x004e08c0
void C2_HOOK_FASTCALL DrawPowerups(tU32 pTime) {

    DrawPowerupIcons2(pTime, gPickedup_powerups, &gNumber_of_icons, 5,
        gCurrent_graf_data->power_up_icon_x, gCurrent_graf_data->power_up_icon_y);
    DrawKeyPowerups(pTime);
}

// MungeKeyboardPowerup

// DoPowerupPeriodics

inline void C2_HOOK_FASTCALL CheckRespawnQueue(tU32 pTime) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gRespawn_powerups); i++) {
        tRespawn_powerup* respawn_powerup = &gRespawn_powerups[i];

        if (respawn_powerup->actor == NULL || (tS32)pTime < (tS32)respawn_powerup->respawn_time) {
            continue;
        }
        if (respawn_powerup->actor->render_style != BR_RSTYLE_FACES) {
            respawn_powerup->actor->render_style = BR_RSTYLE_FACES;
        }
        PipeSinglePowerupRespawn(respawn_powerup->actor, respawn_powerup->index);
        DoPowerupRespawnSmash(respawn_powerup->actor);
        respawn_powerup->actor = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004db880
// FUNCTION: CARMA2_SW 0x004d7d80
// FUNCTION: CARMA2_CHEAT 0x004e1440
void C2_HOOK_FASTCALL MungePowerupStuff(tU32 pFrame_period) {
    tU32 time;

    (void)pFrame_period;
    time = GetTotalTime();
    MungeRepulseRays(time);
    CheckRespawnQueue(time);
}

void C2_HOOK_FASTCALL GotPowerupN(int pN) {
    int modifiers;

    modifiers = PDKeyDown(0) ? 10 : 0;
    modifiers += PDKeyDown(1) ? 20 : 0;
    modifiers += PDKeyDown(2) ? 40 : 0;
    modifiers += PDKeyDown(4) ? 80 : 0;
    GotPowerupEarwig(&gProgram_state.current_car, modifiers + pN, 1, 1);
}

// FUNCTION: CARMA2_HW 0x004dbdb0
// FUNCTION: CARMA2_SW 0x004d82b0
// FUNCTION: CARMA2_CHEAT 0x004e1970
void C2_HOOK_FASTCALL GotPowerup0(void) {
    GotPowerupN(0);
}

// FUNCTION: CARMA2_HW 0x004dbe10
// FUNCTION: CARMA2_SW 0x004d8310
// FUNCTION: CARMA2_CHEAT 0x004e19d0
void C2_HOOK_FASTCALL GotPowerup1(void) {
    GotPowerupN(1);
}

// FUNCTION: CARMA2_HW 0x004dbe70
// FUNCTION: CARMA2_SW 0x004d8370
// FUNCTION: CARMA2_CHEAT 0x004e1a30
void C2_HOOK_FASTCALL GotPowerup2(void) {
    GotPowerupN(2);
}

// FUNCTION: CARMA2_HW 0x004dbed0
// FUNCTION: CARMA2_SW 0x004d83d0
// FUNCTION: CARMA2_CHEAT 0x004e1a90
void C2_HOOK_FASTCALL GotPowerup3(void) {
    GotPowerupN(3);
}

// FUNCTION: CARMA2_HW 0x004dbf30
// FUNCTION: CARMA2_SW 0x004d8430
// FUNCTION: CARMA2_CHEAT 0x004e1af0
void C2_HOOK_FASTCALL GotPowerup4(void) {
    GotPowerupN(4);
}

// FUNCTION: CARMA2_HW 0x004dbf90
// FUNCTION: CARMA2_SW 0x004d8490
// FUNCTION: CARMA2_CHEAT 0x004e1b50
void C2_HOOK_FASTCALL GotPowerup5(void) {
    GotPowerupN(5);
}

// FUNCTION: CARMA2_HW 0x004dbff0
// FUNCTION: CARMA2_SW 0x004d84f0
// FUNCTION: CARMA2_CHEAT 0x004e1bb0
void C2_HOOK_FASTCALL GotPowerup6(void) {
    GotPowerupN(6);
}

// FUNCTION: CARMA2_HW 0x004dc050
// FUNCTION: CARMA2_SW 0x004d8550
// FUNCTION: CARMA2_CHEAT 0x004e1c10
void C2_HOOK_FASTCALL GotPowerup7(void) {
    GotPowerupN(7);
}

// FUNCTION: CARMA2_HW 0x004dc0b0
// FUNCTION: CARMA2_SW 0x004d85b0
// FUNCTION: CARMA2_CHEAT 0x004e1c70
void C2_HOOK_FASTCALL GotPowerup8(void) {
    GotPowerupN(8);
}

// FUNCTION: CARMA2_HW 0x004dc110
// FUNCTION: CARMA2_SW 0x004d8610
// FUNCTION: CARMA2_CHEAT 0x004e1cd0
void C2_HOOK_FASTCALL GotPowerup9(void) {
    GotPowerupN(9);
}

// FUNCTION: CARMA2_HW 0x004dc170
// FUNCTION: CARMA2_SW 0x004d8670
// FUNCTION: CARMA2_CHEAT 0x004e1d30
int C2_HOOK_FASTCALL GotCredits(tPowerup* pPowerup, tCar_spec* pCar) {
    int credits;
    char s[256];

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        credits = 100 * (IRandomBetween(pPowerup->integer_params[0], pPowerup->integer_params[1]) / 100);
        strcpy(s, pPowerup->message);
        strcat(s, " ");
        EarnCredits2(credits, s);
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc3e0
// FUNCTION: CARMA2_SW 0x004d88e0
// FUNCTION: CARMA2_CHEAT 0x004e1fa0
int C2_HOOK_FASTCALL GainAPO2(tCar_spec* pCar, int pIndex, int pAmount) {
    int old;

    if (pAmount < 0) {
        if (pCar->power_up_levels[pIndex] >= pCar->power_up_slots[pIndex]) {
            return 0;
        } else {
            return GainAPO2(pCar, pIndex, pCar->power_up_slots[pIndex] - pCar->power_up_levels[pIndex]);
        }
    }
    if (pIndex == 1 && gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
        return 0;
    }
    old = pCar->power_up_levels[pIndex];
    pCar->power_up_levels[pIndex] = old + pAmount;
    if (pCar->power_up_levels[pIndex] > pCar->power_up_slots[pIndex]) {
        pCar->power_up_levels[pIndex] = pCar->power_up_slots[pIndex];
    }
    return pCar->power_up_levels[pIndex] - old;
}

// GainAPOPot2

// FUNCTION: CARMA2_HW 0x004dc240
// FUNCTION: CARMA2_SW 0x004d8740
// FUNCTION: CARMA2_CHEAT 0x004e1e00
int C2_HOOK_FASTCALL GainAPO(tPowerup* pPowerup, tCar_spec* pCar) {
    int powerup_index;
    int old_armour;
    int old_power;
    int old_offensive;
    int index;
    int amount;
    int original_index;
    int direction;

    powerup_index = pPowerup - gPowerup_array;
    old_armour = pCar->power_up_levels[0];
    old_power = pCar->power_up_levels[1];
    old_offensive = pCar->power_up_levels[2];
    if (pPowerup->integer_params[0] != 0 && pPowerup->integer_params[1] != 0 && pPowerup->integer_params[2] != 0) {
        GainAPO2(pCar, 0, pPowerup->integer_params[0]);
        GainAPO2(pCar, 1, pPowerup->integer_params[1]);
        GainAPO2(pCar, 2, pPowerup->integer_params[2]);
    } else {
        if (pPowerup->integer_params[0] != 0) {
            index = 0;
            amount = pPowerup->integer_params[0];
        } else if (pPowerup->integer_params[1] != 0) {
            index = 1;
            amount = pPowerup->integer_params[1];
        } else {
            index = 2;
            amount = pPowerup->integer_params[2];
        }
        if (GainAPO2(pCar, index, amount) == 0) {
            original_index = index;
            direction = PercentageChance(50) ? 1 : -1;
            index += direction;
            if (index < 0) {
                index = 2;
            } else if (index > 2) {
                index = 0;
            }
            if (GainAPO2(pCar, index, amount) == 0) {
                index += direction;
                if (index < 0) {
                    index = 2;
                } else if (index > 2) {
                    index = 0;
                }
                GainAPO2(pCar, index, amount);
            }
            powerup_index += index - original_index;
        }
    }
    if (old_armour == pCar->power_up_levels[0] && old_power == pCar->power_up_levels[1] && old_offensive == pCar->power_up_levels[2]) {
        if (gNet_mode != eNet_mode_none) {
            EarnCredits(gSubstitution_value_APO.initial_network[gCurrent_net_game->type]);
        } else {
            EarnCredits(gSubstitution_value_APO.initial[gProgram_state.skill_level]);
        }
        return -1;
    }
    return powerup_index;
}

// FUNCTION: CARMA2_HW 0x004dc470
// FUNCTION: CARMA2_SW 0x004d8970
// FUNCTION: CARMA2_CHEAT 0x004e2030
int C2_HOOK_FASTCALL GotTimeOrPower(tPowerup* pPowerup, tCar_spec* pCar) {
    int time;

    if (gNet_mode != eNet_mode_none) {
        return GotPowerupX(pCar, gNet_powerup_time_replacement, 1, 1, 0);
    }
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        time = IRandomBetween(pPowerup->integer_params[0], pPowerup->integer_params[1]);
        AwardTime(time);
    }
    return pPowerup - gPowerup_array;
}

// GainAPOPotential

// FUNCTION: CARMA2_HW 0x004dc720
// FUNCTION: CARMA2_SW 0x004d8c20
// FUNCTION: CARMA2_CHEAT 0x004e22e0
int C2_HOOK_FASTCALL SetPedSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gPedestrian_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc750
// FUNCTION: CARMA2_SW 0x004d8c50
// FUNCTION: CARMA2_CHEAT 0x004e2310
int C2_HOOK_FASTCALL SetPedSize(tPowerup* pPowerup, tCar_spec* pCar) {
    br_vector3 scale;

    BrVector3Set(&scale, pPowerup->float_params[0], pPowerup->float_params[1], pPowerup->float_params[2]);
    ChangePedScaling(&scale);
    return pPowerup - gPowerup_array;
}

// SetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dc7d0
// FUNCTION: CARMA2_SW 0x004d8cd0
// FUNCTION: CARMA2_CHEAT 0x004e2390
int C2_HOOK_FASTCALL SetPedExplode(tPowerup* pPowerup, tCar_spec* pCar) {

    gExploding_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc800
// FUNCTION: CARMA2_SW 0x004d8d00
// FUNCTION: CARMA2_CHEAT 0x004e23c0
int C2_HOOK_FASTCALL SetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc830
// FUNCTION: CARMA2_SW 0x004d8d30
// FUNCTION: CARMA2_CHEAT 0x004e23f0
int C2_HOOK_FASTCALL SetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {

    pCar->invulnerable_no_crushage = 1;
    pCar->invulnerable_no_damage = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc860
// FUNCTION: CARMA2_SW 0x004d8d60
// FUNCTION: CARMA2_CHEAT 0x004e2420
void C2_HOOK_FASTCALL ResetInvulnerability(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && NetPlayerFromCar(pCar)->field_0x80 != 0) {
        return;
    }
    pCar->invulnerable_no_crushage = 0;
    pCar->invulnerable_no_damage = 0;
}

// FUNCTION: CARMA2_HW 0x004dc8a0
// FUNCTION: CARMA2_SW 0x004d8da0
// FUNCTION: CARMA2_CHEAT 0x004e2460
int C2_HOOK_FASTCALL SetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc8d0
// FUNCTION: CARMA2_SW 0x004d8dd0
// FUNCTION: CARMA2_CHEAT 0x004e2490
void C2_HOOK_FASTCALL ResetFreeRepairs(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFree_repairs = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dc8f0
// FUNCTION: CARMA2_SW 0x004d8df0
// FUNCTION: CARMA2_CHEAT 0x004e24b0
int C2_HOOK_FASTCALL SetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc920
// FUNCTION: CARMA2_SW 0x004d8e20
// FUNCTION: CARMA2_CHEAT 0x004e24e0
void C2_HOOK_FASTCALL ResetStupidPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    gBlind_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dc930
// FUNCTION: CARMA2_SW 0x004d8e30
// FUNCTION: CARMA2_CHEAT 0x004e24f0
int C2_HOOK_FASTCALL FreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = !gFreeze_timer;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc970
// FUNCTION: CARMA2_SW 0x004d8e70
// FUNCTION: CARMA2_CHEAT 0x004e2530
void C2_HOOK_FASTCALL UnfreezeTimer(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gFreeze_timer = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dc990
// FUNCTION: CARMA2_SW 0x004d8e90
// FUNCTION: CARMA2_CHEAT 0x004e2550
int C2_HOOK_FASTCALL DoInstantRepair(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        TotallyRepairCar();
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dc9c0
// FUNCTION: CARMA2_SW 0x004d8ec0
// FUNCTION: CARMA2_CHEAT 0x004e2580
void C2_HOOK_FASTCALL ResetPedSpeed(tPowerup* powerup, tCar_spec* car) {

    gPedestrian_speed_factor = 1.f;
}

// FUNCTION: CARMA2_HW 0x004dc9d0
// FUNCTION: CARMA2_SW 0x004d8ed0
// FUNCTION: CARMA2_CHEAT 0x004e2590
void C2_HOOK_FASTCALL ResetPedSize(tPowerup* pPowerup, tCar_spec* pCar) {
    br_vector3 scale;

    BrVector3Set(&scale, 1.0f, 1.0f, 1.0f);
    ChangePedScaling(&scale);
}

// ResetPedHeadSize

// FUNCTION: CARMA2_HW 0x004dca10
// FUNCTION: CARMA2_SW 0x004d8f10
// FUNCTION: CARMA2_CHEAT 0x004e25d0
void C2_HOOK_FASTCALL ResetPedExplode(tPowerup* powerup, tCar_spec* car) {

    gExploding_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004dca20
// FUNCTION: CARMA2_SW 0x004d8f20
// FUNCTION: CARMA2_CHEAT 0x004e25e0
void C2_HOOK_FASTCALL ResetPedBrittle(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_dismemberfest = 0;
}

// FUNCTION: CARMA2_HW 0x004dca30
// FUNCTION: CARMA2_SW 0x004d8f30
// FUNCTION: CARMA2_CHEAT 0x004e25f0
int C2_HOOK_FASTCALL SetEngineFactor(tPowerup* pPowerup, tCar_spec* pCar) {
    float front_give;
    float rear_give;
    float damping;
    float front_height;
    float rear_height;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x4d4, 0x4d4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    if (pPowerup->initial_value > 0) {
        PlayExclusiveFFBEffect("Afterburner", 1);
    }
    pCar->field_0x4d4 = pPowerup->float_params[0];
    pCar->grip_multiplier = pPowerup->float_params[1];
    front_give = pPowerup->float_params[2];
    rear_give = pPowerup->float_params[3];
    front_height = pPowerup->float_params[4];
    rear_height = pPowerup->float_params[5];
    damping = pPowerup->float_params[6];
    SetCarSuspGiveAndHeight(pCar, front_give, rear_give, damping, front_height, rear_height);
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcab0
// FUNCTION: CARMA2_SW 0x004d8fb0
// FUNCTION: CARMA2_CHEAT 0x004e2670
int C2_HOOK_FASTCALL SetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcae0
// FUNCTION: CARMA2_SW 0x004d8fe0
// FUNCTION: CARMA2_CHEAT 0x004e26a0
int C2_HOOK_FASTCALL TrashBodywork(tPowerup* pPowerup, tCar_spec* pCar) {

    TotallySpamTheCar(pCar, (br_scalar)(pCar->softness_factor * 0.1f / 0.7));
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        DRS3StartSound2(gCar_outlet, eSoundId_Smash1, 1, 255, 255, -1, -1);
        DRS3StartSound2(gCar_outlet, eSoundId_Smash2, 1, 255, 255, -1, -1);
        DRS3StartSound2(gCar_outlet, eSoundId_Smash3, 1, 255, 255, -1, -1);
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcb90
// FUNCTION: CARMA2_SW 0x004d9090
// FUNCTION: CARMA2_CHEAT 0x004e2750
int C2_HOOK_FASTCALL TakeDrugs(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_BUG_ON(sizeof(tU32) != 4);

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        if (gReal_render_palette == NULL) {
            gReal_render_palette = BrMemAllocate(256 * sizeof(tU32), kMem_misc);
            memcpy(gReal_render_palette, gRender_palette->pixels, 256 * sizeof(tU32));
        }
        gOn_drugs = 1;
    }
    return pPowerup - gPowerup_array;
}

void C2_HOOK_FASTCALL PaletteFuckedUpByDrugs(br_pixelmap* pPalette, int pOffset) {
    int i;

    (void)pPalette;
    if (pOffset == 0) {
        memcpy(gRender_palette->pixels, gReal_render_palette, 256 * sizeof(tU32));
    }
    for (i = 1; i < 224; i++) {
        ((tU32*)gRender_palette->pixels)[i] = gReal_render_palette[(i + pOffset) & 0xff];
    }
    ((tU32*)gRender_palette->pixels)[0] = gReal_render_palette[0];
    for (i = 224; i < 256; i++) {
        ((tU32*)gRender_palette->pixels)[i] = gReal_render_palette[i];
    }
}

// FUNCTION: CARMA2_HW 0x004dcc00
// FUNCTION: CARMA2_SW 0x004d9100
// FUNCTION: CARMA2_CHEAT 0x004e27c0
void C2_HOOK_FASTCALL TheEffectsOfDrugs(tPowerup* pPowerup, tU32 pPeriod) {

    PaletteFuckedUpByDrugs(gRender_palette, GetTotalTime() / 100);
    ResetPalette();
}

// FUNCTION: CARMA2_HW 0x004dcca0
// FUNCTION: CARMA2_SW 0x004d91a0
// FUNCTION: CARMA2_CHEAT 0x004e2860
int C2_HOOK_FASTCALL SetOpponentsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers && gCurrent_race.opponent_list[i].index != 29; i++) {
    }
    if (i < gCurrent_race.number_of_racers) {
        SetEngineFactor(&gPowerup_array[5], gCurrent_race.opponent_list[i].car_spec);
    }
    gOpponent_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcd70
// FUNCTION: CARMA2_SW 0x004d9270
// FUNCTION: CARMA2_CHEAT 0x004e2930
int C2_HOOK_FASTCALL SetCopsSpeed(tPowerup* pPowerup, tCar_spec* pCar) {

    gCop_speed_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcda0
// FUNCTION: CARMA2_SW 0x004d92a0
// FUNCTION: CARMA2_CHEAT 0x004e2960
int C2_HOOK_FASTCALL SetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcdd0
// FUNCTION: CARMA2_SW 0x004d92d0
// FUNCTION: CARMA2_CHEAT 0x004e2990
int C2_HOOK_FASTCALL SetPinball(tPowerup* pPowerup, tCar_spec* pCar) {

    gPinball_factor = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce00
// FUNCTION: CARMA2_SW 0x004d9300
// FUNCTION: CARMA2_CHEAT 0x004e29c0
int C2_HOOK_FASTCALL SetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce30
// FUNCTION: CARMA2_SW 0x004d9330
// FUNCTION: CARMA2_CHEAT 0x004e29f0
int C2_HOOK_FASTCALL SetBouncey(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_rate, 0x4c0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_amount, 0x4c4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, last_bounce, 0x1598);

    pCar->bounce_rate = 1000.f / pPowerup->float_params[0];
    pCar->bounce_amount = pPowerup->float_params[1];
    pCar->last_bounce = 0;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dce80
// FUNCTION: CARMA2_SW 0x004d9380
// FUNCTION: CARMA2_CHEAT 0x004e2a40
int C2_HOOK_FASTCALL SetSuspension(tPowerup* pPowerup, tCar_spec* pCar) {
    float front_give;
    float rear_give;
    float damping;
    float front_height;
    float rear_height;

    front_give = pPowerup->float_params[0];
    rear_give = pPowerup->float_params[1];
    front_height = pPowerup->float_params[2];
    rear_height = pPowerup->float_params[3];
    damping = pPowerup->float_params[4];
    SetCarSuspGiveAndHeight(pCar, front_give, rear_give, damping, front_height, rear_height);
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dced0
// FUNCTION: CARMA2_SW 0x004d93d0
// FUNCTION: CARMA2_CHEAT 0x004e2a90
int C2_HOOK_FASTCALL SetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf00
// FUNCTION: CARMA2_SW 0x004d9400
// FUNCTION: CARMA2_CHEAT 0x004e2ac0
int C2_HOOK_FASTCALL SetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gImmortal_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf30
// FUNCTION: CARMA2_SW 0x004d9430
// FUNCTION: CARMA2_CHEAT 0x004e2af0
void C2_HOOK_FASTCALL ResetImmortalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gImmortal_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf40
// FUNCTION: CARMA2_SW 0x004d9440
// FUNCTION: CARMA2_CHEAT 0x004e2b00
int C2_HOOK_FASTCALL SetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dcf70
// FUNCTION: CARMA2_SW 0x004d9470
// FUNCTION: CARMA2_CHEAT 0x004e2b30
void C2_HOOK_FASTCALL ResetSuicidalPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPeds_suicidal = 0;
}

// FUNCTION: CARMA2_HW 0x004dcf80
// FUNCTION: CARMA2_SW 0x004d9480
// FUNCTION: CARMA2_CHEAT 0x004e2b40
int C2_HOOK_FASTCALL SetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

inline void C2_HOOK_FASTCALL FillInMine(tNet_message_chunk* pChunk, tShit_mine* pMine, int pArg3) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mine_explode, mine_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mine_explode, field_0x3, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mine_explode, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mine_explode, field_0x5, 0x5);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mine_explode, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mine_explode, object_hierarchy, 0xc);
    C2_HOOK_BUG_ON(REC2_ASIZE(gShit_mines) != 20);

    pChunk->mine_explode.mine_index = (tS8)(pMine - gShit_mines);
    pChunk->mine_explode.field_0x3 = pMine->field_0x35;
    pChunk->mine_explode.field_0x4 = pMine->field_0x34;
    pChunk->mine_explode.field_0x5 = (tU8)(NetPlayerFromCar(pMine->field_0x30) - gNet_players);
    pChunk->mine_explode.field_0x8 = pArg3;
    GetHierarchyNetworkStuff(pMine->collision_info, pChunk->mine_explode.object_hierarchy, NET_OBJECT_HIERARCHY_DEFAULT_SIZE);
}

// FUNCTION: CARMA2_HW 0x004dcfb0
// FUNCTION: CARMA2_SW 0x004d94b0
// FUNCTION: CARMA2_CHEAT 0x004e2b70
void C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time) {
    int i;
    int j;
    int size;
    int index;
    tShit_mine* mine;
    union {
        tNet_message_chunk chunk;
        tU8 raw[0x23c]; /* FIXME: should raw buffer be added to tNet_message_chunk or is sizeof(tNet_message_chunk) == 0x235? */
    } message;
    tNet_message_chunk* contents;
    br_vector3 tv;

    if (gNet_mode != eNet_mode_host) {
        return;
    }
    if (gNumber_of_net_players == 1) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gShit_mines); i++) {
        mine = &gShit_mines[i];
        if (!(mine->flags & 0x1)) {
            continue;
        }
        FillInMine(&message.chunk, mine, pNext_frame_time);
        size = (int)(GetHierarchyNetworkSize(gShit_mines[i].collision_info) + 12.0f);
        message.chunk.mine_explode.header.size = size;
        message.chunk.mine_explode.header.type = eNet_message_chunk_type_mine_explosion;
        for (j = 1; j < gNumber_of_net_players; j++) {
            if (gNet_players[j].field_0xd0 != 0) {
                for (index = 0; index < gNumber_of_net_players - 1; index++) {
                    if (gNet_players[index].ID == gNet_players[j].field_0xd0) {
                        break;
                    }
                }
            } else {
                index = j;
            }
            BrVector3Sub(&tv, (br_vector3*)mine->collision_info->transform_matrix.m[3], (br_vector3*)gNet_players[index].car->collision_info->transform_matrix.m[3]);
            if (BrVector3LengthSquared(&tv) < REC2_SQR(20.0f)) {
                contents = NetGetToPlayerContents(eNet_message_chunk_type_mine_explosion, size, gNet_players[j].ID);
                if (contents != NULL) {
                    memcpy(contents, &message.chunk, size);
                }
            }
        }
    }
}

// STUB: CARMA2_HW 0x004dd1b0
// STUB: CARMA2_SW 0x004d96b0
void C2_HOOK_FASTCALL MinePreCollisionStuff(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004dd3f0
// FUNCTION: CARMA2_SW 0x004d98f0
// FUNCTION: CARMA2_CHEAT 0x004e2fb0
void C2_HOOK_FASTCALL ActivateMine(tShit_mine* pMine, tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tShit_mine, field_0x1c, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tShit_mine, field_0x20, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tShit_mine, field_0x24, 0x24);

    pMine->flags = 0x5;
    /* Integer parameter #0: direction of fire (0 - shit out, 1 - mortar fire) */
    if (pPowerup->integer_params[0] != 0) {
        pMine->flags = 0x45;
    } else {
        pMine->flags = 0x5;
    }
    /* Floating point param #0: Explosion delay */
    pMine->next_think_time = GetTotalTime() - (int)(pPowerup->float_params[0] * -1000.0);

    /* Floating point param #1: min y strength */
    pMine->field3_0xc = pPowerup->float_params[1];

    /* Floating point param #2: max y strength*/
    pMine->initial_y_speed_factor = pPowerup->float_params[2];

    /* Floating point param #3: max roll rate */
    pMine->initial_z_omega_factor = pPowerup->float_params[3];

    /* Floating point param #4: max pitch rate */
    pMine->initial_x_omega_factor = pPowerup->float_params[4];

    /* Floating point param #5: force needed to trigger world exlposion */
    pMine->field_0x1c = pPowerup->float_params[5];

    /* Floating point param #6: firing up vector */
    pMine->field_0x20 = pPowerup->float_params[6];

    /* Floating point param #6: firing out vector */
    pMine->field_0x24 = pPowerup->float_params[7];

    pMine->collision_info->field_0xed = 1;
    pMine->collision_info->disable_move_rotate = 0;
    pMine->collision_info->flags &= ~0x40;
    if (pMine->collision_info->actor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_FACES;
    pMine->field_0x34 = (tU8)(pPowerup - gPowerup_array);
    PHILAddActiveObject(pMine->collision_info, NULL, NULL, NULL);
    PHILSetObjectProperty(pMine->collision_info, 0, 1.0);
    PHILSetObjectProperty(pMine->collision_info, 7, 1);
    PHILSetObjectProperty(pMine->collision_info, 6, 1);
    pMine->field_0x30 = pCar;
    pMine->collision_info->last_special_volume = pCar->collision_info->last_special_volume;
    pMine->collision_info->auto_special_volume = pCar->collision_info->auto_special_volume;
    pMine->collision_info->water_d = pCar->collision_info->water_d;
    BrVector3Copy(&pMine->collision_info->water_normal, &pCar->collision_info->water_normal);
}

// PositionMine

// MineSound

// ReceivedMine

// ShitMine

// FUNCTION: CARMA2_HW 0x004ddab0
// FUNCTION: CARMA2_SW 0x004d9fb0
// FUNCTION: CARMA2_CHEAT 0x004e3670
int C2_HOOK_FASTCALL MineExplode(tShit_mine* pMine) {
    tNet_message* message;
    br_vector3 offset;
    int size;

    if (pMine->flags & 0x2) {
        return 0;
    }
    pMine->flags |= 0x2;
    if (gNet_mode == eNet_mode_host) {
        size = GetHierarchyNetworkSize(pMine->collision_info);
        message = NetBuildGuaranteedMessage(eNet_message_chunk_type_mine_explosion, size + (int)offsetof(tNet_message_chunk_mine_explode, object_hierarchy));
        FillInMine(&message->contents, pMine, -1);
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
    }
    PHILRemoveObject(pMine->collision_info);
    pMine->next_think_time = GetTotalTime() + 1600;
    BrVector3Negate(&offset, &gExplosion_pix_animation_groups.groups[0].offset);
    Explode(&gExplosion_pix_animation_groups, pMine->collision_info->actor, &pMine->collision_info->actor->model->bounds, &offset);
    DRS3StartSound3D(gCar_outlet, eSoundId_Explosion, &pMine->collision_info->actor->t.t.translate.t, &gZero_v__car, 1, 255, S3_FIXED_INT(1), S3_FIXED_INT(1));
    PipeSingleEndShitMine(pMine->collision_info->actor);
    if (pMine->collision_info->actor->parent != NULL) {
        BrActorRemove(pMine->collision_info->actor);
    }
    pMine->collision_info->actor->render_style = BR_RSTYLE_NONE;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004ddc10
// FUNCTION: CARMA2_SW 0x004da110
// FUNCTION: CARMA2_CHEAT 0x004e37d0
void C2_HOOK_FASTCALL TiggerMineToExplode(tCar_spec* pCar) {

    pCar->index |= 0x20;
}

// HitAShatOutMine

// FUNCTION: CARMA2_HW 0x004ddde0
// FUNCTION: CARMA2_SW 0x004da2e0
// FUNCTION: CARMA2_CHEAT 0x004e39a0
void C2_HOOK_FASTCALL ClearMineReferencesToCar(tCar_spec* pCar) {
    int i;
    tShit_mine* mine;

    for (i = 0; i < 20; i++) {
        mine = &gShit_mines[i];
        if (mine->flags & 1) {
            if (mine->car == pCar) {
                mine->car = NULL;
            }
            if (mine->field_0x30 == pCar) {
                mine->field_0x30 = NULL;
                MineExplode(mine);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004dde20
// FUNCTION: CARMA2_SW 0x004da320
// FUNCTION: CARMA2_CHEAT 0x004e39e0
void C2_HOOK_FASTCALL MungeShatMines(tU32 pTime) {
    int i;
    int car_vulnerable;
    tShit_mine* mine;
    tCar_spec* car;
    tPhysics_object* object;

    C2_HOOK_BUG_ON(REC2_ASIZE(gShit_mines) != 20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x49c, 0x49c);

    for (i = 0; i < (int)REC2_ASIZE(gShit_mines); i++) {
        mine = &gShit_mines[i];
        if (!(mine->flags & 0x1)) {
            continue;
        }
        if ((mine->flags & 0x80) && mine->car != NULL) {
            car = mine->car;
            mine->flags &= ~0x80;
            car_vulnerable = car->field_0x4c8 <= 1.0
                    && !(gNet_mode != eNet_mode_none && NetPlayerFromCar(car) != NULL && NetPlayerFromCar(car)->field_0x80 != 0)
                    && !car->invulnerable_no_crushage;
            if (car_vulnerable) {
                TotallySpamTheCar(car, mine->max_damage * 0.1f);
            }
            if (car_vulnerable && (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host)) {
                object = car->collision_info;
                object->v.v[1] = (br_scalar)(FRandomBetween(mine->field3_0xc, mine->initial_y_speed_factor) / (object->M * WORLD_SCALE_D) + object->v.v[1]);
                object = car->collision_info;
                object->omega.v[2] = (br_scalar)((FRandomPosNeg(mine->initial_z_omega_factor) * TAU) / object->M + object->omega.v[2]);
                object = car->collision_info;
                object->omega.v[0] = (br_scalar)((FRandomPosNeg(mine->initial_x_omega_factor) * TAU) / object->M + object->omega.v[0]);
                car->collision_info->disable_move_rotate = 0;
                car->collision_info->field_0x49c = gPHIL_last_physics_tick + 40;
                if (!car->invulnerable_no_damage) {
                    for (i = 0; i < eDamage_count; i++) {
                        DamageUnit2(car, (tDamage_type)i, IRandomBetween(0, (int)(mine->max_damage * 25.0f)));
                    }
                    if (car != mine->field_0x30) {
                        car->last_hit_by = mine->field_0x30;
                        car->time_last_hit = PDGetTotalTime();
                        CrashEarnings(car, NULL);
                    }
                }
            }
        }
        if (mine->flags & 0x20) {
            mine->flags &= ~0x20;
            MineExplode(mine);
        } else if (mine->flags & 0x2) {
            if (mine->next_think_time < pTime) {
                mine->flags &= ~0x3;
            }
        } else if ((mine->flags & 0x4) && mine->next_think_time < pTime) {
            MineExplode(mine);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004de000
// FUNCTION: CARMA2_SW 0x004da500
// FUNCTION: CARMA2_CHEAT 0x004e3bc0
void C2_HOOK_FASTCALL ARDoShitMine(br_actor* pActor, br_matrix34* pMatrix) {
    (void)pMatrix;
    if (pActor->parent == NULL) {
        BrActorAdd(gNon_track_actor, pActor);
    }
    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004de020
// FUNCTION: CARMA2_SW 0x004da520
// FUNCTION: CARMA2_CHEAT 0x004e3be0
void C2_HOOK_FASTCALL ARUndoShitMine(br_actor* pActor, br_matrix34* pMatrix) {
    (void)pMatrix;
    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004de040
// FUNCTION: CARMA2_SW 0x004da540
// FUNCTION: CARMA2_CHEAT 0x004e3c00
int C2_HOOK_FASTCALL PissOutOil(tPowerup* pPowerup, tCar_spec* pCar) {
    PlayExclusiveFFBEffect("Oil_Slick", 0);
    QueueOilSpill2(NULL, pCar->car_master_actor, pCar->car_actor->model, 0,
        0.35f, 0.6f, 0.00009f, 0.0003f, 0.2f, 1.5f, 2.5f, 0.01f, 0.15f,
        oily_material, NULL);
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de0c0
// FUNCTION: CARMA2_SW 0x004da5c0
// FUNCTION: CARMA2_CHEAT 0x004e3c80
int C2_HOOK_FASTCALL KangerooJump(tPowerup* pPowerup, tCar_spec* pCar) {
    PlayExclusiveFFBEffect("Kangeroo", 0);
    PratcamEvent(42);
    pCar->collision_info->v.v[1] += (br_scalar)(pPowerup->float_params[0] / WORLD_SCALE_D);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        DRS3StartSound(gCar_outlet, eSoundId_Boing);
    }
    pCar->collision_info->disable_move_rotate = 0;
    return pPowerup - gPowerup_array;
}

// AnnihilatePeds

// NapalmPeds

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004de1f0
// STUB: CARMA2_SW 0x004da6f0
// STUB: CARMA2_CHEAT 0x004e3db0
int C2_HOOK_FASTCALL RepulseOpponents2(tPowerup* pPowerup, tCar_spec* pCar, int pSend_message) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004de1e0
// FUNCTION: CARMA2_SW 0x004da6e0
// FUNCTION: CARMA2_CHEAT 0x004e3da0
int C2_HOOK_FASTCALL RepulseOpponents(tPowerup* pPowerup, tCar_spec* pCar) {

    return RepulseOpponents2(pPowerup, pCar, 1);
}

// RepulsePedestrians

// ReceivedRepulse

// ResetEngineFactor

// FUNCTION: CARMA2_HW 0x004de600
// FUNCTION: CARMA2_SW 0x004dab00
// FUNCTION: CARMA2_CHEAT 0x004e41c0
void C2_HOOK_FASTCALL ResetUnderwater(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, underwater_ability, 0x1b0);

    pCar->underwater_ability = 0;
}

// FUNCTION: CARMA2_HW 0x004de610
// FUNCTION: CARMA2_SW 0x004dab10
// FUNCTION: CARMA2_CHEAT 0x004e41d0
void C2_HOOK_FASTCALL PukeDrugsBackUp(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gOn_drugs = 0;
        PaletteFuckedUpByDrugs(gRender_palette, 0);
        ResetPalette();
    }
}

// ResetOpponentsSpeed

// FUNCTION: CARMA2_HW 0x004de710
// FUNCTION: CARMA2_SW 0x004dac10
// FUNCTION: CARMA2_CHEAT 0x004e42d0
void C2_HOOK_FASTCALL ResetCopsSpeed(tPowerup* powerup, tCar_spec* car) {

    gCop_speed_factor = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de720
// FUNCTION: CARMA2_SW 0x004dac20
// FUNCTION: CARMA2_CHEAT 0x004e42e0
void C2_HOOK_FASTCALL ResetGravity(tPowerup* pPowerup, tCar_spec* pCar) {

    gGravity_multiplier = gDefault_gravity;
}

// FUNCTION: CARMA2_HW 0x004de730
// FUNCTION: CARMA2_SW 0x004dac30
// FUNCTION: CARMA2_CHEAT 0x004e42f0
void C2_HOOK_FASTCALL ResetPinball(tPowerup* powerup, tCar_spec* car) {

    gPinball_factor = 0.f;
}

// FUNCTION: CARMA2_HW 0x004de740
// FUNCTION: CARMA2_SW 0x004dac40
// FUNCTION: CARMA2_CHEAT 0x004e4300
void C2_HOOK_FASTCALL ResetWallclimb(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wall_climber_mode, 0x1c4);

    pCar->wall_climber_mode = 0;
}

// FUNCTION: CARMA2_HW 0x004de750
// FUNCTION: CARMA2_SW 0x004dac50
// FUNCTION: CARMA2_CHEAT 0x004e4310
void C2_HOOK_FASTCALL ResetBouncey(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_rate, 0x4c0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, bounce_amount, 0x4c4);

    pCar->bounce_rate = 0.f;
    pCar->bounce_amount = 0.f;
}

// FUNCTION: CARMA2_HW 0x004de760
// FUNCTION: CARMA2_SW 0x004dac60
// FUNCTION: CARMA2_CHEAT 0x004e4320
void C2_HOOK_FASTCALL ResetSuspension(tPowerup* pPowerup, tCar_spec* pCar) {

    SetCarSuspGiveAndHeight(pCar, 1.f, 1.f, 1.f, 0.f, 0.f);
}

// FUNCTION: CARMA2_HW 0x004de780
// FUNCTION: CARMA2_SW 0x004dac80
// FUNCTION: CARMA2_CHEAT 0x004e4340
void C2_HOOK_FASTCALL ResetDamageMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_multiplier, 0x4cc);

    pCar->damage_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de790
// FUNCTION: CARMA2_SW 0x004dac90
// FUNCTION: CARMA2_CHEAT 0x004e4350
void C2_HOOK_FASTCALL ResetTyreGrip(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grip_multiplier, 0x4d0);

    pCar->grip_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de7a0
// FUNCTION: CARMA2_SW 0x004daca0
// FUNCTION: CARMA2_CHEAT 0x004e4360
int C2_HOOK_FASTCALL PickAtRandom(tPowerup* pPowerup, tCar_spec* pCar) {
    int selected_powerup;

    selected_powerup = pPowerup->integer_params[IRandomBetween(0, pPowerup->number_of_integer_params - 1)];
    GotPowerup(pCar, selected_powerup);
    return -1;
}

// FUNCTION: CARMA2_HW 0x004de7d0
// FUNCTION: CARMA2_SW 0x004dacd0
// FUNCTION: CARMA2_CHEAT 0x004e4390
int C2_HOOK_FASTCALL GotVouchers(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        AddVouchers(pPowerup->integer_params[0]);
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de810
// FUNCTION: CARMA2_SW 0x004dad10
// FUNCTION: CARMA2_CHEAT 0x004e43d0
void C2_HOOK_FASTCALL MungeVouchers(tPowerup* pPowerup, tU32 pPeriod) {

    pPowerup->current_value = GetRecoverVoucherCount();
}

// FUNCTION: CARMA2_HW 0x004de820
// FUNCTION: CARMA2_SW 0x004dad20
// FUNCTION: CARMA2_CHEAT 0x004e43e0
int C2_HOOK_FASTCALL SetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gInstant_handbrake = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de850
// FUNCTION: CARMA2_SW 0x004dad50
// FUNCTION: CARMA2_CHEAT 0x004e4410
void C2_HOOK_FASTCALL ResetInstantHandbrake(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gInstant_handbrake = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004de870
// FUNCTION: CARMA2_SW 0x004dad70
// FUNCTION: CARMA2_CHEAT 0x004e4430
int C2_HOOK_FASTCALL SetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDancing_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8a0
// FUNCTION: CARMA2_SW 0x004dada0
// FUNCTION: CARMA2_CHEAT 0x004e4460
int C2_HOOK_FASTCALL SetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPanicking_peds = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de8d0
// FUNCTION: CARMA2_SW 0x004dadd0
// FUNCTION: CARMA2_CHEAT 0x004e4490
int C2_HOOK_FASTCALL SetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_gravity_multiplier = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de900
// FUNCTION: CARMA2_SW 0x004dae00
// FUNCTION: CARMA2_CHEAT 0x004e44c0
int C2_HOOK_FASTCALL SetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDrunk_pedestrians = 1;
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de930
// FUNCTION: CARMA2_SW 0x004dae30
// FUNCTION: CARMA2_CHEAT 0x004e44f0
int C2_HOOK_FASTCALL SetGhostPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    TurnOnGhostPeds();
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004de960
// FUNCTION: CARMA2_SW 0x004dae60
// FUNCTION: CARMA2_CHEAT 0x004e4520
void C2_HOOK_FASTCALL ResetDancingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDancing_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de970
// FUNCTION: CARMA2_SW 0x004dae70
// FUNCTION: CARMA2_CHEAT 0x004e4530
void C2_HOOK_FASTCALL ResetPanickingPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPanicking_peds = 0;
}

// FUNCTION: CARMA2_HW 0x004de980
// FUNCTION: CARMA2_SW 0x004dae80
// FUNCTION: CARMA2_CHEAT 0x004e4540
void C2_HOOK_FASTCALL ResetLowGravityPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gPed_gravity_multiplier = 1.f;
}

// FUNCTION: CARMA2_HW 0x004de990
// FUNCTION: CARMA2_SW 0x004dae90
// FUNCTION: CARMA2_CHEAT 0x004e4550
void C2_HOOK_FASTCALL ResetPissedPeds(tPowerup* pPowerup, tCar_spec* pCar) {

    gDrunk_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004de9a0
// FUNCTION: CARMA2_SW 0x004daea0
// FUNCTION: CARMA2_CHEAT 0x004e4560
void C2_HOOK_FASTCALL ResetGhostPeds(tPowerup* pPowerup, tCar_spec* pCar) {
    TurnOffGhostPeds();
}

// FUNCTION: CARMA2_HW 0x004de9b0
// FUNCTION: CARMA2_SW 0x004daeb0
// FUNCTION: CARMA2_CHEAT 0x004e4570
void C2_HOOK_FASTCALL DoBouncey(tPowerup* pPowerup, tU32 pPeriod) {

    if (gProgram_state.current_car.bounce_rate <= GetTotalTime() - gProgram_state.current_car.last_bounce
        && gProgram_state.current_car.number_of_wheels_on_ground > 2) {
        PratcamEvent(42);
        gProgram_state.current_car.last_bounce = GetTotalTime();
        gProgram_state.current_car.collision_info->v.v[1] = (br_scalar)((gProgram_state.current_car.bounce_amount * (1.0 / WORLD_SCALE_D)) + gProgram_state.current_car.collision_info->v.v[1]);
        DRS3StartSound(gCar_outlet, eSoundId_Boing);
        gProgram_state.current_car.collision_info->disable_move_rotate = 0;
    }
}

// HitMine

// FUNCTION: CARMA2_HW 0x004debb0
// FUNCTION: CARMA2_SW 0x004db0b0
// FUNCTION: CARMA2_CHEAT 0x004e4770
int C2_HOOK_FASTCALL SetMassMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x4c8, 0x4c8);

    pCar->field_0x4c8 = pPowerup->float_params[0];
    if (gNet_mode != eNet_mode_none) {
        DoSolidGraniteThing(pCar);
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004debf0
// FUNCTION: CARMA2_SW 0x004db0f0
// FUNCTION: CARMA2_CHEAT 0x004e47b0
void C2_HOOK_FASTCALL ResetMassMultiplier(tPowerup* pPowerup, tCar_spec* pCar) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x4c8, 0x4c8);

    (void)pPowerup;
    pCar->field_0x4c8 = 1.f;
    if (gNet_mode != eNet_mode_none) {
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004dec10
// FUNCTION: CARMA2_SW 0x004db110
// FUNCTION: CARMA2_CHEAT 0x004e47d0
int C2_HOOK_FASTCALL ShowPedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 1;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec40
// FUNCTION: CARMA2_SW 0x004db140
// FUNCTION: CARMA2_CHEAT 0x004e4800
void C2_HOOK_FASTCALL HidePedestrians(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gShow_peds_on_map = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004dec60
// FUNCTION: CARMA2_SW 0x004db160
// FUNCTION: CARMA2_CHEAT 0x004e4820
int C2_HOOK_FASTCALL SetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = pPowerup->float_params[0] * pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004dec90
// FUNCTION: CARMA2_SW 0x004db190
// FUNCTION: CARMA2_CHEAT 0x004e4850
void C2_HOOK_FASTCALL ResetElectroBastard(tPowerup* pPowerup, tCar_spec* pCar) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, proxy_ray_distance, 0x170c);

    pCar->proxy_ray_distance = 0.f;
}

// FUNCTION: CARMA2_HW 0x004deca0
// FUNCTION: CARMA2_SW 0x004db1a0
// FUNCTION: CARMA2_CHEAT 0x004e4860
int C2_HOOK_FASTCALL SetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar) {

    gMutant_speed = pPowerup->float_params[0];
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004decd0
// FUNCTION: CARMA2_SW 0x004db1d0
// FUNCTION: CARMA2_CHEAT 0x004e4890
void C2_HOOK_FASTCALL ResetMutantCorpses(tPowerup* pPowerup, tCar_spec* pCar) {

    gMutant_speed = 0.f;
}

// ReceivedPowerup

// FUNCTION: CARMA2_HW 0x004dedf0
// FUNCTION: CARMA2_SW 0x004db2f0
// FUNCTION: CARMA2_CHEAT 0x004e49b0
void C2_HOOK_FASTCALL SendCurrentPowerups(void) {
    int cat;
    int car_count;
    int i;
    int j;
    tCar_spec* car;
    tPlayer_ID ID;
    tNet_message_chunk* contents;

    for (cat = eVehicle_self; cat <= eVehicle_net_player; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (gNet_players[j].car == car) {
                    ID = gNet_players[j].ID;
                    break;
                }
            }
            for (j = 0; j < gNumber_of_powerups; j++) {
                if (car->powerups[j] != 0) {
                    contents = NetStartBroadcastContents(eNet_message_chunk_type_powerup, 0);
                    if (contents != NULL) {
                        contents->powerup.event = 1;
                        contents->powerup.player = ID;
                        contents->powerup.powerup_index = j;
                        contents->powerup.time_left = car->powerups[j] - GetTotalTime();
                        NetBroadcastContents(contents);
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004def00
// FUNCTION: CARMA2_SW 0x004db400
// FUNCTION: CARMA2_CHEAT 0x004e4ac0
void C2_HOOK_FASTCALL LoseAllLocalPowerups(tCar_spec* pCar) {
    int i;

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        for (i = 0; i < gNumber_of_powerups; i++) {
            if (pCar->powerups[i] != 0 && !(gPowerup_array[i].net_type & eNet_powerup_global)) {
                LosePowerup(&gPowerup_array[i]);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004df070
// FUNCTION: CARMA2_SW 0x004db570
// FUNCTION: CARMA2_CHEAT 0x004e4c30
void C2_HOOK_FASTCALL LoseDodgyPowerups(tCar_spec* pCar) {
    int i;

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        for (i = 0; i < gNumber_of_powerups; i++) {
            if (pCar->powerups[i] != 0 && (gPowerup_array[i].net_type & eNet_powerup_off_before_snapweld)) {
                LosePowerup(&gPowerup_array[i]);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004df1e0
// FUNCTION: CARMA2_SW 0x004db6e0
// FUNCTION: CARMA2_CHEAT 0x004e4da0
int C2_HOOK_FASTCALL GetPowerupCount(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004df1f0
// FUNCTION: CARMA2_SW 0x004db6f0
// FUNCTION: CARMA2_CHEAT 0x004e4db0
void C2_HOOK_FASTCALL GetPowerupPosition(int pIndex, br_vector3* pPosition) {

}

// FUNCTION: CARMA2_HW 0x004df200
// FUNCTION: CARMA2_SW 0x004db700
// FUNCTION: CARMA2_CHEAT 0x004e4dc0
void C2_HOOK_FASTCALL ARPowerupGot(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->got_proc != NULL) {
        pPowerup->got_proc(pPowerup, pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004df210
// FUNCTION: CARMA2_SW 0x004db710
// FUNCTION: CARMA2_CHEAT 0x004e4dd0
void C2_HOOK_FASTCALL ARPowerupLose(tPowerup* pPowerup, tCar_spec* pCar) {

    if (pPowerup->lose_proc != NULL) {
        pPowerup->lose_proc(pPowerup, pCar);
    }
}

// SetPissed

// FUNCTION: CARMA2_HW 0x004df300
// FUNCTION: CARMA2_SW 0x004db800
// FUNCTION: CARMA2_CHEAT 0x004e4ec0
void C2_HOOK_FASTCALL ResetPissed(tPowerup* pPowerup, tCar_spec* pCar) {
    br_vector3 p;

    gCredit_multiplier = 0;
    BrVector3Copy(&p, &gCamera->t.t.translate.t);
    BrMatrix34Identity(&gCamera->t.t.mat);
    BrVector3Copy(&gCamera->t.t.translate.t, &p);
    BrVector3Copy(&p, &gRearview_camera->t.t.translate.t);
    BrMatrix34Identity(&gRearview_camera->t.t.mat);
    BrVector3Copy(&gRearview_camera->t.t.translate.t, &p);
}

// PissedEffect

// FUNCTION: CARMA2_HW 0x004df650
// FUNCTION: CARMA2_SW 0x004dbb50
// FUNCTION: CARMA2_CHEAT 0x004e5210
void C2_HOOK_CDECL RenderSpinningPowerup(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    if (pActor->user == NULL) {
        pActor->user = (void*)(uintptr_t)GetTotalTime();
    } else {
        tU32 prev;
        tU32 now;
        tU32 elapsed;

        now = GetTotalTime();
        prev = (tU32)(uintptr_t)pActor->user;
        elapsed = (now - prev) / 16;
        pActor->user = (void*)(uintptr_t)now;
        BrMatrix34PreRotateY(&pActor->t.t.mat, BR_ANGLE_DEG(elapsed));
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004dfe10
// FUNCTION: CARMA2_SW 0x004dc310
// FUNCTION: CARMA2_CHEAT 0x004e59f0
void C2_HOOK_CDECL RenderChangingPowerup(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    if (pActor->user == NULL) {
        pActor->user = (void*)(uintptr_t)GetTotalTime();
        pActor->model = gModel_powerup_armor;
    } else {
        tU32 now;
        tU32 elapsed;

        now = GetTotalTime();
        elapsed = (now - (tU32)(uintptr_t)pActor->user) / 16;
        if (elapsed > 15) {
            if (pActor->model == gModel_powerup_armor) {
                pActor->model = gModel_powerup_power;
            } else if (pActor->model == gModel_powerup_power) {
                pActor->model = gModel_powerup_offence;
            } else {
                pActor->model = gModel_powerup_armor;
            }
            pActor->user = (void*)(uintptr_t)now;
        }
        BrMatrix34PreRotateY(&pActor->t.t.mat, BR_ANGLE_DEG(elapsed + 12));
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

// FUNCTION: CARMA2_HW 0x004df570
// FUNCTION: CARMA2_SW 0x004dba70
// FUNCTION: CARMA2_CHEAT 0x004e5130
void C2_HOOK_FASTCALL SetSpinningPowerup(br_actor* pActor, int pOpacity) {
    // GLOBAL: CARMA2_HW 0x0065ebc0
    // GLOBAL: CARMA2_SW 0x00659520
    // GLOBAL: CARMA2_CHEAT 0x00666328
    static br_token_value spinning_powerup_extra_prim[2] = {
        { BRT_BLEND_B, { 1 } },
        { BRT_OPACITY_X, { BrScalarToFixed(75) } },
    };
    unsigned int i;

    if (pActor == NULL) {
        return;
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, 0.f, .1f, 0.f);
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderSpinningPowerup;
    if (pOpacity == 0 || pOpacity == 255) {
        return;
    }
    if (pActor->material != NULL) {
        pActor->material->extra_prim = spinning_powerup_extra_prim;
        pActor->material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
        BrMaterialUpdate(pActor->material, BR_MATU_EXTRA_PRIM);
        return;
    }
    for (i = 0; i < pActor->model->nfaces; i++) {
        br_material* material;

        material = pActor->model->faces[i].material;
        if (material != NULL) {
            material->extra_prim = spinning_powerup_extra_prim;
            pActor->model->faces[i].material->extra_prim[1].v.x = BR_FIXED_INT(pOpacity);
            BrMaterialUpdate(pActor->model->faces[i].material, BR_MATU_EXTRA_PRIM);
        }
    }
}

inline br_model* C2_HOOK_FASTCALL DuplicateModel(br_model* pModel, const char* pName) {
    br_model* copy;

    copy = BrModelAllocate(pName, pModel->nvertices, pModel->nfaces);
    if (copy == NULL) {
        return copy;
    }
    copy->pivot = pModel->pivot;
    copy->flags = pModel->flags;
    copy->user = pModel->user;
    copy->crease_angle = pModel->crease_angle;
    copy->radius = pModel->radius;
    copy->bounds = pModel->bounds;
    memcpy(copy->vertices, pModel->vertices, pModel->nvertices * sizeof(br_vertex));
    memcpy(copy->faces, pModel->faces, pModel->nfaces * sizeof(br_face));
    return copy;
}

// FUNCTION: CARMA2_HW 0x004df6c0
// FUNCTION: CARMA2_SW 0x004dbbc0
// FUNCTION: CARMA2_CHEAT 0x004e5280
void C2_HOOK_FASTCALL SetChangingPowerup(br_actor* pActor) {
    br_model* model;
    tPath_name the_path;
    char s[256];
    char s2[256];
    FILE* f;
    int i;
    br_actor* actor;

    if (pActor == NULL) {
        return;
    }
    if (gModel_powerup_armor == NULL || gModel_powerup_power == NULL || gModel_powerup_offence == NULL) {

        model = BrModelFind("&68powerup1.ACT");
#ifdef REC2_CHEAT
        if (model == NULL) {
            PDFatalError("ARMOUR missing from REG directory");
        }
#endif
        gModel_powerup_armor = DuplicateModel(model, "PowArm");
        gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
        gModel_powerup_armor->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_armor);

        model = BrModelFind("&70powerup1.ACT");
#ifdef REC2_CHEAT
        if (model == NULL) {
            PDFatalError("POWER missing from REG directory");
        }
#endif
        gModel_powerup_power = DuplicateModel(model, "PowPow");
        gModel_powerup_power->flags |= BR_MODF_CUSTOM;
        gModel_powerup_power->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_power);

        model = BrModelFind("&69powerup1.ACT");
#ifdef REC2_CHEAT
        if (model == NULL) {
            PDFatalError("OFFENSIVE missing from REG directory");
        }
#endif
        gModel_powerup_offence = DuplicateModel(model, "PowOff");
        gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
        gModel_powerup_offence->custom = RenderChangingPowerup;
        BrModelAdd(gModel_powerup_offence);
    }

    if (gModel_powerup_armor == NULL) {
        BrModelFind("&77powerup.ACT");
        PathCat(the_path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 67; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_armor = DuplicateModel(actor->model, "PowArm");
            gModel_powerup_armor->flags |= BR_MODF_CUSTOM;
            gModel_powerup_armor->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_armor);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_power == NULL) {
        PathCat(the_path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 68; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_power = DuplicateModel(actor->model, "PowPow");
            gModel_powerup_power->flags |= BR_MODF_CUSTOM;
            gModel_powerup_power->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_power);
            BrActorFree(actor);
        }
        PFfclose(f);
    }

    if (gModel_powerup_offence == NULL) {
        PathCat(the_path, gApplication_path, "POWRSHIT.TXT");
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            sprintf(s, "Cant open %s", "POWRSHIT.TXT");
            PDFatalError(s);
        }
        for (i = 0; i < 69; i++) {
            GetAString(f, s2);
        }
        actor = LoadActor(s2);
        if (actor != NULL && actor->model != NULL) {
            gModel_powerup_offence = DuplicateModel(actor->model, "PowOff");
            gModel_powerup_offence->flags |= BR_MODF_CUSTOM;
            gModel_powerup_offence->custom = RenderChangingPowerup;
            BrModelAdd(gModel_powerup_offence);
            BrActorFree(actor);
        }
        PFfclose(f);
    }
    BrMatrix34PreTranslate(&pActor->t.t.mat, 0.0f, 0.1f, 0.0f);
    pActor->model = gModel_powerup_armor;
    pActor->model->flags |= BR_MODF_CUSTOM;
    pActor->model->custom = RenderChangingPowerup;
}

// FUNCTION: CARMA2_HW 0x004dfec0
// FUNCTION: CARMA2_SW 0x004dc3c0
// FUNCTION: CARMA2_CHEAT 0x004e5aa0
int C2_HOOK_FASTCALL PedValium(tPowerup* pPowerup, tCar_spec* pCar) {

    if (CalmDownAllPeds()) {
        NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_pedestrians_are_calm_now));
        DRS3StartSound2(gCar_outlet, eSoundId_PedValium, 1, 0xff, 0xff, -1, -1);
        return pPowerup - gPowerup_array;
    } else {
        return -1;
    }
}

// PowerupCancel

// FUNCTION: CARMA2_HW 0x004e02b0
// FUNCTION: CARMA2_SW 0x004dc7b0
// FUNCTION: CARMA2_CHEAT 0x004e5e90
int C2_HOOK_FASTCALL GrowATail(tPowerup* pPowerup, tCar_spec* pCar) {
    tPhysics_object* object;
    br_matrix34 mat;
    br_vector3 pos;

    if (gMutant_tail_state == 2) {
        return -1;
    }
    if (gMutant_tail_state == 1) {
        PHILRemoveObject(gMutant_tail_object);
        gMutant_tail_object->physics_joint1 = gMutant_tail_first_joint;
    } else {
        PipeSingleSpecial(1);
    }
    BrVector3SetFloat(&gMutant_tail_object->physics_joint1->field_0x14,
        0.0f,
        (pCar->collision_info->bb1.max.v[1] - pCar->collision_info->bb1.min.v[1]) * 0.5f,
        (br_scalar)(pCar->collision_info->bb1.max.v[2] * 0.95));
    PhysicsAddObject(pCar->collision_info, gMutant_tail_object);
    for (object = gMutant_tail_object; object != NULL; object = object->child) {
        if (object->actor->parent == NULL) {
            BrActorAdd(gNon_track_actor, object->actor);
        }
        object->owner = pCar;
        BrMatrix34Translate(&object->actor->t.t.mat,
            -object->physics_joint1->field_0x08.v[0],
            -object->physics_joint1->field_0x08.v[1],
            -object->physics_joint1->field_0x08.v[2]);
        BrMatrix34Copy(&mat, &pCar->collision_info->actor->t.t.mat);
        BrMatrix34Post(&object->actor->t.t.mat, &mat);
        BrMatrix34Rotate(&mat, BR_ANGLE_DEG(350), (br_vector3*)object->actor->t.t.mat.m[0]);
        BrMatrix34Post(&object->actor->t.t.mat, &mat);
        BrMatrix34ApplyP(&pos, &object->physics_joint1->field_0x14, &object->parent->actor->t.t.mat);
        BrMatrix34PostTranslate(&object->actor->t.t.mat, pos.v[0], pos.v[1], pos.v[2]);
        BrMatrix34Copy(&object->transform_matrix, &object->actor->t.t.mat);
        object->field_0xed = 1;
    }
    GotPowerupEarwig(pCar, 95, 0, 1);
    gMutant_tail_state = 2;
    DRS3StartSound2(gCar_outlet, eSoundId_GrowATail, 1, 0xff, 0xff, -1, -1);
    return pPowerup - gPowerup_array;
}

// CutOffTail

// FUNCTION: CARMA2_HW 0x004e06a0
// FUNCTION: CARMA2_SW 0x004dcba0
// FUNCTION: CARMA2_CHEAT 0x004e6280
void C2_HOOK_FASTCALL RemoveTail(void) {
    tPhysics_object* object;

    if (gMutant_tail_state != 0) {
        object = gMutant_tail_object;
        if (gMutant_tail_object != NULL) {
            while (object != NULL) {
                BrActorRemove(object->actor);
                object = object->child;
            }
        }
        if (gMutant_tail_state == 2) {
            PhysicsRemoveObject(gMutant_tail_object);
        } else {
            PHILRemoveObject(gMutant_tail_object);
        }
        gMutant_tail_object->physics_joint1 = gMutant_tail_first_joint;
        gMutant_tail_state = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e0710
// FUNCTION: CARMA2_SW 0x004dcc10
// FUNCTION: CARMA2_CHEAT 0x004e62f0
void C2_HOOK_FASTCALL DoInitialTail(void) {
    tPhysics_object* object;

    for (object = gMutant_tail_object; object != NULL; object = object->child) {
        object->actor->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004e0730
// FUNCTION: CARMA2_SW 0x004dcc30
// FUNCTION: CARMA2_CHEAT 0x004e6310
void C2_HOOK_FASTCALL UndoInitialTail(void) {
    tPhysics_object* object;

    for (object = gMutant_tail_object; object != NULL; object = object->child) {
        object->actor->render_style = BR_RSTYLE_NONE;
    }
}

// FUNCTION: CARMA2_HW 0x004e0750
// FUNCTION: CARMA2_SW 0x004dcc50
// FUNCTION: CARMA2_CHEAT 0x004e6330
void C2_HOOK_FASTCALL MayQueuePowerupRespawn(int pPowerup_index, br_actor* pActor) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gRespawn_powerups) != 100);
    C2_HOOK_BUG_ON(sizeof(tRespawn_powerup) != 0xc);

    if (pActor != NULL && gRace_powerup_respawn_bools[pPowerup_index]
#ifdef REC2_V102A
        && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.powerup_respawn)) {
#else
        ) {
#endif
        for (i = 0; i < (int)REC2_ASIZE(gRespawn_powerups); i++) {
            if (gRespawn_powerups[i].actor == NULL) {
                gRespawn_powerups[i].actor = pActor;
                gRespawn_powerups[i].index = pPowerup_index;
                gRespawn_powerups[i].respawn_time = GetTotalTime() + gPickup_respawn_max_extra_time_ms / 2 + gPickup_respawn_min_time_ms;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e07d0
// FUNCTION: CARMA2_SW 0x004dccd0
// FUNCTION: CARMA2_CHEAT 0x004e6390
void C2_HOOK_FASTCALL ARDoPowerupRespawn(br_actor* pActor, int pPowerup_id) {
    (void)pPowerup_id;

    pActor->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x004e07e0
// FUNCTION: CARMA2_SW 0x004dcce0
// FUNCTION: CARMA2_CHEAT 0x004e63a0
void C2_HOOK_FASTCALL ARUndoPowerupRespawn(br_actor* pActor, int pPowerup_id) {
    (void)pPowerup_id;

    pActor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e07f0
// FUNCTION: CARMA2_CHEAT 0x004e63b0
void C2_HOOK_FASTCALL PrintPowerupIconIn3D(int pX, int pY, tHeadup_icon* pIcon, tPowerup* pPowerup, int pScale, tU32 pTime) {
    float f;
    float scale;
    br_fixed_ls prev_opacity;

    if (pIcon == NULL) {
        pPowerup->icon_actor->render_style = BR_RSTYLE_FACES;
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
        return;
    }
    if (pIcon->icon_actor != NULL) {
        pIcon->icon_actor->render_style = BR_RSTYLE_FACES;
        if (pScale != 0) {
            f = (float)(pTime - pIcon->fizzle_start) / 600.0f;
            if (pIcon->fizzle_direction > 0) {
                f = 1.0f - f;
                scale = 1.0f + 5.0f * f;
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, scale, scale, 1.0f);
            } else {
                BrMatrix34Scale(&pIcon->icon_actor->t.t.mat, 1.01f - f, 1.01f - f, 1.0f);
            }
            BrMatrix34PostRotateZ(&pIcon->icon_actor->t.t.mat, (br_angle)((1.0f - f / 2.0f) * 65536.0f));
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT((1.0f - f) * 255.0f);
            BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
        } else {
            prev_opacity = pIcon->icon_actor->material->extra_prim[1].v.x;
            pIcon->icon_actor->material->extra_prim[1].v.x = BR_FIXED_INT(255);
            if (prev_opacity != pIcon->icon_actor->material->extra_prim[1].v.x) {
                BrMaterialUpdate(pIcon->icon_actor->material, BR_MATU_EXTRA_PRIM);
            }
            BrMatrix34Identity(&pIcon->icon_actor->t.t.mat);
        }
        BrVector3Set(&pIcon->icon_actor->t.t.translate.t, (float)pX + 16.0f, (float)-(pY + 16), 0.0f);
        RenderThisHeadup(pIcon->icon_actor);
    }
}

// FUNCTION: CARMA2_HW 0x004e09d0
// FUNCTION: CARMA2_SW 0x004dccf0
// FUNCTION: CARMA2_CHEAT 0x004e6590
br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pTexture) {
    br_actor* actor;
    br_material* material;
    br_model* model;
    int width;
    int height;
    br_scalar left;
    br_scalar right;
    br_scalar top;
    br_scalar bottom;

    width = pTexture->width;
    height = pTexture->height;
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    left = (float)-(width / 2);
    top = (float)+(height / 2);
    BrVector3Set(&model->vertices[0].p, left,  top, -2.0f);
    bottom = (float)-(height / 2);
    BrVector3Set(&model->vertices[1].p, left, bottom, -2.0f);
    right = (float)+(width / 2);
    BrVector3Set(&model->vertices[2].p, right, bottom, -2.0f);
    BrVector3Set(&model->vertices[3].p, right,  top, -2.0f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[2].map, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 0.0f);
    material->colour = 0;
    material->colour_map = pTexture;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pTexture);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

// FUNCTION: CARMA2_HW 0x004e0c00
// FUNCTION: CARMA2_SW 0x004dcf20
// FUNCTION: CARMA2_CHEAT 0x004e67c0
void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c10
// FUNCTION: CARMA2_SW 0x004dcf30
// FUNCTION: CARMA2_CHEAT 0x004e67d0
void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void) {

    gPowerup_txt_path = "POWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c20
// FUNCTION: CARMA2_SW 0x004dcf40
// FUNCTION: CARMA2_CHEAT 0x004e67e0
void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void) {

    gPowerup_txt_path = "ZOMPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c30
// FUNCTION: CARMA2_SW 0x004dcf50
// FUNCTION: CARMA2_CHEAT 0x004e67f0
void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void) {

    gPowerup_txt_path = "ALPOWERUP.TXT";
}

// FUNCTION: CARMA2_HW 0x004e0c40
// FUNCTION: CARMA2_SW 0x004dcf60
// FUNCTION: CARMA2_CHEAT 0x004e6800
int C2_HOOK_FASTCALL TurnOnCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && !IsCarCloaked(pCar)) {
        BlendifyCar(pCar);
        MasterDisableCarFunks(pCar);
        gCloaked_cars[gCount_cloaked_cars++] = pCar;
    }
    return pPowerup - gPowerup_array;
}

// FUNCTION: CARMA2_HW 0x004e0cb0
// FUNCTION: CARMA2_SW 0x004dcfd0
// FUNCTION: CARMA2_CHEAT 0x004e6870
void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {

            if (gCloaked_cars[i] == pCar) {
#ifdef REC2_FIX_BUGS
                memmove(&gCloaked_cars[i], &gCloaked_cars[i + 1], (gCount_cloaked_cars - i - 1) * sizeof(tCar_spec*));
#else
                memcpy(&gCloaked_cars[i], &gCloaked_cars[i + 1], (REC2_ASIZE(gCloaked_cars) - i - 1) * sizeof(tCar_spec*));
#endif
                gCount_cloaked_cars -= 1;
                break;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e0d10
// FUNCTION: CARMA2_SW 0x004dd030
// FUNCTION: CARMA2_CHEAT 0x004e68d0
int C2_HOOK_FASTCALL IsCarCloaked(tCar_spec* pCar) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gCount_cloaked_cars; i++) {
            if (gCloaked_cars[i] == pCar) {
                return 1;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e0d50
// FUNCTION: CARMA2_SW 0x004dd070
// FUNCTION: CARMA2_CHEAT 0x004e6910
void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none) {
        UnBlendifyCar(pCar);
        RemoveFromCloakingList(pCar);
        MasterEnableCarFunks(pCar);
        RestoreCarPixelmaps(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x004e0de0
// FUNCTION: CARMA2_CHEAT 0x004e69a0
void C2_HOOK_FASTCALL PeriodicCloaking(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {

        for (i = 0; i < gCount_cloaked_cars; i++) {
            DoCamouflageThing(gCloaked_cars[i]);
        }
    }
}

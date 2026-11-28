/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "61-pedestrn-phys.h"

#include "00-car.h"
#include "07-structur.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "23-piping.h"
#include "27-powerup.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "41-utility.h"
#include "42-input.h"
#include "43-pratcam.h"
#include "49-physics.h"
#include "51-smash.h"
#include "52-errors.h"
#include "54-runtime.h"
#include "57-drone.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"

#include "rec2_macros.h"

#include <stdarg.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x0065d7c8
// GLOBAL: CARMA2_SW 0x00658100
// GLOBAL: CARMA2_CHEAT 0x00664e80
int gAnimalsOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7cc
// GLOBAL: CARMA2_CHEAT 0x00664e84
int gFlameThrowerOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7d0
// GLOBAL: CARMA2_CHEAT 0x00664e88
int gExplosives_on = 1;

// GLOBAL: CARMA2_HW 0x0065d7d4
// GLOBAL: CARMA2_SW 0x0065810c
// GLOBAL: CARMA2_CHEAT 0x00664e8c
int gGoreLevel = 1;

// GLOBAL: CARMA2_HW 0x0065d7d8
// GLOBAL: CARMA2_SW 0x00658110
// GLOBAL: CARMA2_CHEAT 0x00664e90
float gPed_tumble_spin = 8.0f;

// GLOBAL: CARMA2_HW 0x0065e4d8
// GLOBAL: CARMA2_SW 0x00658e10
// GLOBAL: CARMA2_CHEAT 0x00665b90
const tPed_anim_seq gScare_ped_anim_sequence = {
    4, { 1, 2, 1, 99 },
};

// GLOBAL: CARMA2_HW 0x0065e4e0
// GLOBAL: CARMA2_SW 0x00658e18
// GLOBAL: CARMA2_CHEAT 0x00665b98
const tPed_anim_seq gKnocked_over_ped_anim_sequence = {
    3, { 1, 2, 1 },
};

// GLOBAL: CARMA2_HW 0x0065e4e4
// GLOBAL: CARMA2_SW 0x00658e1c
// GLOBAL: CARMA2_CHEAT 0x00665b9c
const tPed_anim_seq gCalm_ped_anim_sequence = {
    2, { 0, 99 },
};

// GLOBAL: CARMA2_HW 0x0065e560
// GLOBAL: CARMA2_SW 0x00658e98
// GLOBAL: CARMA2_CHEAT 0x00665c18
const char* gForm_non_humanoid_names[1] = {
    "NONHUMANOID",
};

#define GET_PED_COLLISION_OBJECT(PED) (((PED)->character->field_0x14 & 0x1) ? (PED)->character->personality->form->simple_physicing[(tS8)(PED)->character->field_0x5].collision_info : GetRootObject((PED)->character))
// GLOBAL: CARMA2_HW 0x0065e5b8
// GLOBAL: CARMA2_SW 0x00658ef0
// GLOBAL: CARMA2_CHEAT 0x00665c70
const float gFlamed_ped_flame_scales[7] = {
    0.275f,
    0.225f,
    0.2f,
    0.175f,
    0.15f,
    0.075f,
    0.05f,
};

// GLOBAL: CARMA2_HW 0x006a0414
// GLOBAL: CARMA2_SW 0x0069a46c
// GLOBAL: CARMA2_CHEAT 0x006a7f8c
const char* gPedsFolder;

// GLOBAL: CARMA2_HW 0x007447a0
// GLOBAL: CARMA2_SW 0x0073b260
// GLOBAL: CARMA2_CHEAT 0x0074be40
int gGhost_pedestrians = 0;

// FUNCTION: CARMA2_HW 0x004d5730
// FUNCTION: CARMA2_SW 0x004d1e20
// FUNCTION: CARMA2_CHEAT 0x004db2f0
inline int C2_HOOK_FASTCALL OrientationChanged(const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2, float pCos_angle) {
    if (BrVector3Dot((br_vector3*)pOrientation_1->m[0], (br_vector3*)pOrientation_2->m[0]) < pCos_angle
        || BrVector3Dot((br_vector3*)pOrientation_1->m[1], (br_vector3*)pOrientation_2->m[1]) < pCos_angle
        || BrVector3Dot((br_vector3*)pOrientation_1->m[2], (br_vector3*)pOrientation_2->m[2]) < pCos_angle) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004d57c0
// FUNCTION: CARMA2_SW 0x004d1eb0
// FUNCTION: CARMA2_CHEAT 0x004db380
void C2_HOOK_FASTCALL AssertRootObjectsMatrix(tPed_character_instance* pPed) {
    tPhysics_object* collision_info;
    int i;

    if (pPed->field_0x14 & 0x1) {
        collision_info = pPed->personality->form->simple_physicing[(tS8)pPed->field_0x5].collision_info;
    } else {
        collision_info = GetRootObject(pPed);
    }
    for (i = 0; i < 3; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            collision_info->transform_matrix.m[i][j] = collision_info->actor->t.t.mat.m[i][j];
        }
    }
    collision_info->flags |= 0x100;
}

inline void C2_HOOK_FASTCALL InitFaceCaches(void) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_sizes_1); i++) {
        gPed_face_cache_heads[i] = BrMemAllocate(gPed_cache_sizes_1[i] * sizeof(tPed_face_cache_0x34), kMem_ped_face_cache);
        for (j = 0; j < gPed_cache_sizes_1[i]; j++) {
            gPed_face_cache_heads[i][j].field_0x0 = BrMemAllocate(gPed_cache_sizes_2[i] * sizeof(tFace_ref), kMem_ped_face_cache);
            gPed_face_cache_heads[i][j].count = gPed_cache_sizes_2[i];
        }
    }
    gPed_face_cache = BrMemAllocate((gPed_cache_sizes_2[3] + 1) * sizeof(tFace_ref), kMem_ped_face_cache);
}

inline void C2_HOOK_FASTCALL ResetPedFaceCache(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_sizes_1); i++) {
        int j;

        gPed_face_caches[i] = gPed_face_cache_heads[i];
        for (j = 0; j < gPed_cache_sizes_1[i]; j++) {
            gPed_face_cache_heads[i][j].field_0x14 = NULL;
            if (j != gPed_cache_sizes_1[i] - 1) {
                gPed_face_cache_heads[i][j].next = &gPed_face_cache_heads[i][j + 1];
                gPed_face_cache_heads[i][j + 1].prev = &gPed_face_cache_heads[i][j];
            } else {
                gPed_face_cache_heads[i][j].next = &gPed_face_cache_heads[i][0];
                gPed_face_cache_heads[i][0].prev = &gPed_face_cache_heads[i][j];
            }
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(gPed_cache_006944c0); i++) {
        gPed_cache_006944c0[i].field_0xb4 = NULL;
        gPed_cache_006944c0[i].field_0xb0 = NULL;
        gPed_cache_006944c0[i].field_0x74 = 0;
    }
}

inline void C2_HOOK_FASTCALL SetPedCurrentFace(tPedestrian* pPed, br_vector3* pPos) {
    tPed_face_cache_0x34* face_cache;

    if (pPed->field_0x0c != NULL) {
        face_cache = pPed->field_0x0c->field_0xb0;
        face_cache->field_0x30 = CastRayDownThruFaces((br_vector3*)GetCharacterMatrixPtr(pPed->character)->m[3], face_cache->field_0x0, face_cache->field_0x4,
            GetClearanceFromCharacterInstance(pPed->character, 0), pPos);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d2060
// STUB: CARMA2_SW 0x004ce760
// STUB: CARMA2_CHEAT 0x004d7c00
void C2_HOOK_FASTCALL RecachePedestrian(tPedestrian* pPed, br_vector3* pPos, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004d3360
// FUNCTION: CARMA2_SW 0x004cfa50
// FUNCTION: CARMA2_CHEAT 0x004d8f20
void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp) {
    tPhysics_object* object;

    if (pPed->field_0x14 & 0x1) {
        object = pPed->personality->form->simple_physicing[(tS8)pPed->field_0x5].collision_info;
    } else {
        object = GetRootObject(pPed);
    }
    if (gAction_replay_mode && !gBOOL_00744804) {
        return;
    }
    if (!(pPed->field_0x14 & 0x4) && gProgram_state.racing) {
        PipeSinglePedDir(((tS16)pPed->ped->field_0x06 << 16) | (pPed->ped - gPedestrian_array), pDir);
    }
    SetCharacterDirection(pPed, pDir, pUp);
    if (gProgram_state.racing && OrientationChanged(&object->transform_matrix, &object->actor->t.t.mat, 0.7f)) {
        AssertRootObjectsMatrix(pPed);
    }
}

// FUNCTION: CARMA2_HW 0x004ca4d0
// FUNCTION: CARMA2_SW 0x004c6c00
// FUNCTION: CARMA2_CHEAT 0x004d0010
void C2_HOOK_FASTCALL PedApplyPedDir(tU32 pId, const br_vector3* pDir) {
    if (!gBOOL_00744804) {
        SetCharacterDirection(gPedestrian_array[pId & 0xffff].character, pDir, &y_unit_vector);
    }
}

// FUNCTION: CARMA2_HW 0x004d1c60
// FUNCTION: CARMA2_SW 0x004ce360
// FUNCTION: CARMA2_CHEAT 0x004d7800
void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, tU32 pTime) {
    br_matrix34* mat;
    br_vector3 old_pos;
    int old_move;
    int old_frame;
    float old_speed;

    if (gAction_replay_mode && !gBOOL_00744804) {
        return;
    }
    if (pSpeed == 0.0f) {
        pSpeed = 0.0f;
    }
    mat = GetCharacterMatrixPtr(pCharacter);
    BrVector3Copy(&old_pos, (br_vector3*)mat->m[3]);
    old_move = pCharacter->personality->form->moves[pCharacter->field_0x7].id;
    old_frame = pCharacter->field_0x1c;
    old_speed = (float)(tS16)pCharacter->field_0x1e;
    SetCharacterMove(pCharacter, pMove_action, pSpeed, pArg4, pArg5, pTime);
    if (pMove_action == -1) {
        PipeSinglePedFPChange(pCharacter->ped, (float)(tS16)pCharacter->field_0x1e, pSpeed);
    } else {
        PipeSinglePedMove(pCharacter->ped, (tS16)old_frame, (tS16)old_move, (tS16)pMove_action, old_speed, pSpeed, pArg5, pArg4, &old_pos, (br_vector3*)mat->m[3], -1, -1, &pCharacter->field_0x8c);
    }
}

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// FUNCTION: CARMA2_HW 0x004ca650
// FUNCTION: CARMA2_SW 0x004c6d80
// FUNCTION: CARMA2_CHEAT 0x004d0190
void C2_HOOK_FASTCALL PedApplyPedFPChange(tPedestrian* pPed, float pSpeed) {

    if (gBOOL_00744804 == 0) {
        SetCharacterMove(pPed->character, -1, pSpeed, 0, 0, GetTotalTime());
    }
}

// PedApplyStatus

// PedUndoStatus

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004cc860
// FUNCTION: CARMA2_SW 0x004c8f60
// FUNCTION: CARMA2_CHEAT 0x004d23d0
void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel, ...) {
    tU32 old_level;
    tU32 old_bit;
    tS8 old_field_0x4;
    undefined old_field_0x5;
    va_list ap;

    old_level = pCharacter->field_0x14;
    old_bit = (old_level >> 2) & 0x1;
    old_field_0x4 = pCharacter->field_0x4;
    old_field_0x5 = pCharacter->field_0x5;
    if (pLevel == 1) {
        va_start(ap, pLevel);
        SetCharacterPhysicsLevel(pCharacter, 1, va_arg(ap, int));
        va_end(ap);
    } else {
        SetCharacterPhysicsLevel(pCharacter, pLevel);
    }
    if (((pLevel >> 2) & 0x1) ^ old_bit) {
        PipeSinglePedStatus(pCharacter->ped, -1, -1, old_level, pLevel, old_field_0x4, pCharacter->field_0x4, old_field_0x5, pCharacter->field_0x5,
            &pCharacter->field_0xc0, &pCharacter->field_0xcc, &pCharacter->field_0x8c);
        pCharacter->ped->field_0x06++;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// PedApplyPedPhysics

// FUNCTION: CARMA2_HW 0x004ca830
// FUNCTION: CARMA2_SW 0x004c6f60
// FUNCTION: CARMA2_CHEAT 0x004d0370
void C2_HOOK_FASTCALL PedApplyPedPos(tPedestrian* pPedestrian, br_vector3* pPos) {

    if (!gBOOL_00744804) {
        if (Vector3IsZero(pPos)) {
            pPedestrian->flags |= 0x80;
        } else {
            SetCharacterPosition(pPedestrian->character, pPos, -1);
            if (pPedestrian->flags & 0x80) {
                pPedestrian->flags &= ~0x80;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ca890
// FUNCTION: CARMA2_SW 0x004c6fc0
// FUNCTION: CARMA2_CHEAT 0x004d03d0
void C2_HOOK_FASTCALL PedApplyVanishDismembered(tPedestrian* pPedestrian, tU32 pValue) {
    pPedestrian->character->field_0x10 = pValue;
}

// SetCharacterPositionAR

inline void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set) {

    if (pCharacter->field_0x4 >= 0) {
        if (pBone_index < 0) {
            PipeSinglePedModelChange(pCharacter->ped,
                pBone_index,
                NULL,
                -1,
                GetCharacterModelSet(pCharacter),
                pModel_set);
            SetCharacterAllBonesModel(pCharacter, pModel_index, pModel_set);
        } else {
            PipeSinglePedModelChange(pCharacter->ped,
                pBone_index,
                GetCharacterBoneModel(pCharacter, pBone_index),
                pModel_index,
                GetCharacterModelSet(pCharacter),
                pModel_set);
            SetCharacterBoneModel(pCharacter, pBone_index, pModel_index, pModel_set);
        }
    }
}

// PedApplyPedModelChangeP

// PedApplyPedModelChange

inline int C2_HOOK_FASTCALL DismemberCharacterAR(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pAngular_velocity) {
    tPed_form_actor_set* actor_set;
    int result;
    int old_value;

    old_value = pCharacter->field_0x6;
    result = DismemberCharacter(pCharacter, pBone_index, pVelocity, pAngular_velocity);
    if (result != 0 && pCharacter->field_0x6 >= 0) {
        PipeSinglePedDismember(pCharacter->ped, (tU8)pBone_index, (tS8)old_value, pCharacter->field_0x6);
        actor_set = &pCharacter->personality->form->actor_sets[pCharacter->field_0x4];
        MakeActorRenderable(actor_set->actors[pBone_index], pBone_index, pCharacter);
    }
    return result;
}

// PedApplyDismember

// FUNCTION: CARMA2_HW 0x004ca980
// FUNCTION: CARMA2_SW 0x004c70b0
// FUNCTION: CARMA2_CHEAT 0x004d04c0
void C2_HOOK_FASTCALL PedUndoDismember(tPedestrian* pPed, int pBone_index, tS8 pValue) {
    tPed_form_actor_set* actor_set;

    actor_set = &pPed->character->personality->form->actor_sets[pPed->character->field_0x4];
    pPed->character->field_0xc &= ~gPow2_array[pBone_index];
    pPed->character->field_0x10 &= ~gPow2_array[pBone_index];
    pPed->character->field_0x6 = pValue;
    SetCharacterBonePositions(pPed->character, 3, 0);
    StopActorBeingRenderable(actor_set->actors[pBone_index]);
}

// FUNCTION: CARMA2_HW 0x004ca9f0
// FUNCTION: CARMA2_SW 0x004c7120
// FUNCTION: CARMA2_CHEAT 0x004d0530
void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF) {
    int i;
    int j;
    int index;
    int len;
    int exclusions_ok_when_scared;
    char s[256];
    char s2[32];

    C2_HOOK_BUG_ON(sizeof(tRace_ped_spec) != 0x20);
    C2_HOOK_BUG_ON(sizeof(tRace_spec_exclusion_material) != 8);
    C2_HOOK_BUG_ON(sizeof(tPed_movement_spec) != 0x14);
    C2_HOOK_BUG_ON(sizeof(tPed_group_spec) != 8);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 0x54);

    /* Number of ped spawn sets */
    gCount_race_pedestrian_specs = GetAnInt(pF);
    gRace_pedestrian_specs = BrMemAllocate(gCount_race_pedestrian_specs * sizeof(tRace_ped_spec), kMem_misc_poly_ped);
    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        /* Material name */
        GetAString(pF, s);
        gRace_pedestrian_specs[i].spawn_material = BrMaterialFind(s);
        if (gRace_pedestrian_specs[i].spawn_material == NULL) {
            FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s);
        }

        /* Movement index */
        index = GetAnInt(pF);
        if (index < 0 || index >= gPed_movements_count) {
            FatalError(kFatalError_CannotMovePedIndexOutOfRangeForMaterial_S, s);
        }
        gRace_pedestrian_specs[i].movement = &gPed_movements[index];

        /* Group index */
        index = GetAnInt(pF);
        if (index < 0 || index >= gPed_groups_count) {
            FatalError(kFatalError_CannotGroupPedIndexOutOfRangeForMaterial_S, s);
        }
        gRace_pedestrian_specs[i].group = &gPed_groups[index];

        /* Peds per 100 square metres */
        gRace_pedestrian_specs[i].density = GetAScalar(pF);
        gRace_pedestrian_specs[i].field_0x10 = 0;

        /* Number of exclusion materials */
        gRace_pedestrian_specs[i].exclusion.count_exclusion_materials = (tU8)GetAnInt(pF);
        gRace_pedestrian_specs[i].exclusion.exclusion_materials = BrMemAllocate(gRace_pedestrian_specs[i].exclusion.count_exclusion_materials * sizeof(tRace_spec_exclusion_material), kMem_misc_poly_ped);
        exclusions_ok_when_scared = 0x1;
        for (j = 0; j < gRace_pedestrian_specs[i].exclusion.count_exclusion_materials; j++) {
            /* Exclusion flags (1 = OK when scared) */
            gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].flags = (tU8)GetAnInt(pF);
            exclusions_ok_when_scared &= gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].flags;

            /* Exclusion material #1 name */
            GetAString(pF, s2);
            gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].material = BrMaterialFind(s2);
            if (gRace_pedestrian_specs[i].exclusion.exclusion_materials[j].material == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
        if (exclusions_ok_when_scared & 0x1) {
            gRace_pedestrian_specs[i].exclusion.exclusions_ok_when_scared = 1;
        }

        /* Number of exception materials */
        gRace_pedestrian_specs[i].exclusion.count_exception_materials = (tU8)GetAnInt(pF);
        gRace_pedestrian_specs[i].exclusion.exception_materials = BrMemAllocate(gRace_pedestrian_specs[i].exclusion.count_exception_materials * sizeof(br_material*), kMem_misc_poly_ped);
        for (j = 0; j < gRace_pedestrian_specs[i].exclusion.count_exception_materials; j++) {
            GetAString(pF, s2);
            gRace_pedestrian_specs[i].exclusion.exception_materials[j] = BrMaterialFind(s2);
            if (gRace_pedestrian_specs[i].exclusion.exception_materials[j] == NULL) {
                FatalError(kFatalError_CannotFindPedSpawnMaterial_S, s2);
            }
        }
    }
    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        len = strlen(gRace_pedestrian_specs[i].spawn_material->identifier);
        if (len < 4) {
            len = 4;
        }
        if (len - 4 <= 7) {
            strcpy(s2, gRace_pedestrian_specs[i].spawn_material->identifier);
            strcpy(&s2[len - 4], "xxxxxxxx.MAT");
            BrResFree(gRace_pedestrian_specs[i].spawn_material->identifier);
            gRace_pedestrian_specs[i].spawn_material->identifier = BrResStrDup(gRace_pedestrian_specs[i].spawn_material, s2);
        }
        gRace_pedestrian_specs[i].spawn_material->identifier[7] = '?';
    }
    gPedestrian_array = BrMemAllocate(PEDESTRIAN_MAX_COUNT * sizeof(tPedestrian), kMem_ped_array);
}

// FUNCTION: CARMA2_HW 0x004cadb0
// FUNCTION: CARMA2_SW 0x004c74e0
// FUNCTION: CARMA2_CHEAT 0x004d08f0
br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed) {

    return (br_vector3*)GetCharacterMatrixPtr(pPed->character)->m[3];
}

inline void C2_HOOK_FASTCALL SetUpGibSlicks(void) {

    gPed_slick_material = BrMaterialAllocate("GIBSLICK");
    BrMaterialAdd(gPed_slick_material);
    gPed_slick_material->flags |= BR_MATF_LIGHT;
    gPed_slick_material->flags |= BR_MATF_PERSPECTIVE;
    gPed_slick_material->flags |= BR_MATF_SMOOTH;
    gPed_slick_material->ka = 0.99f;
    gPed_slick_material->kd = 0.0f;
    gPed_slick_material->ks = 0.0f;
    gPed_slick_material->power = 0.0f;
    gPed_slick_material->index_base = 0;
    gPed_slick_material->index_range = 0;
    BrMatrix23Identity(&gPed_slick_material->map_transform);
    gPed_slick_material->index_shade = BrTableFind("IDENTITY.TAB");
    gPed_slick_material->colour_map = BrMapFind("GIBSLICK");
#ifdef REC2_HW
    GlorifyMaterial(&gPed_slick_material, 1, kRendererShadingType_Diffuse2);
#endif
    BrMaterialUpdate(gPed_slick_material, BR_MATU_ALL);
}

// FUNCTION: CARMA2_HW 0x004cb1e0
// FUNCTION: CARMA2_SW 0x004c7910
// FUNCTION: CARMA2_CHEAT 0x004d0d20
void C2_HOOK_FASTCALL ReadInGiblets(FILE* pFile) {
    tPath_name the_path;
    char model_name[256];
    float scaling_factor;
    int i;
    int j;
    int r;
    int g;
    int b;
    int index;
    tPed_giblet_size_spec* size;
    tU8* model_count;

    C2_HOOK_BUG_ON(sizeof(tPed_giblet_size_spec) != 0x18);

    PathCat(the_path, gApplication_path, gPedsFolder);
    PathCat(the_path, the_path, "GIBLETS");
    LoadAllStuffInDirectory(&gMisc_storage_space, the_path, kRendererShadingType_Diffuse1);
    gGiblet_scrape_start = (float)GetAnInt(pFile);
    ReadShrapnelInitialPosition(pFile, &gGiblet_initial_position);
    ReadShrapnelMovementSpec(pFile, &gGiblet_initial_speed);
    ReadShrapnelTimeInfo(pFile, &gGiblet_min_max_time);
    gGiblet_size_count = GetAnInt(pFile);
    gGiblet_sizes = BrMemAllocate(gGiblet_size_count * sizeof(tPed_giblet_size_spec), kMem_misc_poly_ped);
    size = gGiblet_sizes;
    for (i = 0; i < gGiblet_size_count; i++) {
        model_count = &size[i].model_count;
        size[i].flags = (tU8)GetAnInt(pFile);
        size[i].min_force = GetAScalar(pFile);
        size[i].instance_force = GetAScalar(pFile);
        size[i].max_count = (tU8)GetAnInt(pFile);
        *model_count = (tU8)GetAnInt(pFile);
        for (j = 0; j < *model_count; j++) {
            scaling_factor = GetAScalar(pFile);
            index = GetAnInt(pFile);
            GetThreeInts(pFile, &r, &g, &b);
            GetAString(pFile, model_name);
            size[i].models[j] = BrModelFind(model_name);
            if (size[i].models[j] == NULL) {
                FatalError(kFatalError_CannotFindGibletModel_S, model_name);
            }
#ifdef REC2_HW
            if (i == gGiblet_size_count - 1) {
                scaling_factor = (scaling_factor * 3.0f) / 4.0f;
            }
#endif
            ScaleModel(size[i].models[j], scaling_factor);
            size[i].models[j]->faces[0].material->index_base = (tU8)index;
            size[i].models[j]->faces[0].material->index_range = 1;
            size[i].models[j]->faces[0].material->colour = BR_COLOUR_RGB(r, g, b);
            BrMaterialUpdate(size[i].models[j]->faces[0].material, BR_MATU_ALL);
        }
    }
    ReadExplosion(pFile, &gPed_explosion_small_blood_cloud);
    ReadExplosion(pFile, &gPed_medium_blood_cloud);
    ReadExplosion(pFile, &gPed_large_blood_cloud);
    SetUpGibSlicks();
    gPed_smear_material = BrMaterialFind("PEDSMEAR");
    ReadSpillData(pFile, &gPed_slick);
    gPed_spurty_lumps_count = GetAnInt(pFile);
    gPed_spurting_period = GetAnInt(pFile);
    gPed_dismember_spurting_member_count = GetAnInt(pFile);
    ReadShrapnelInitialPosition(pFile, &gPed_spurt_initial_position);
    ReadShrapnelMovementSpec(pFile, &gPed_spurt_initial_speed);
    ReadShrapnelTimeInfo(pFile, &gPed_spurt_min_max_time);
    scaling_factor = GetAScalar(pFile);
    index = GetAnInt(pFile);
    GetThreeInts(pFile, &r, &g, &b);
    GetAString(pFile, model_name);
    gPed_spurt_model = BrModelFind(model_name);
    if (gPed_spurt_model == NULL) {
        FatalError(kFatalError_CannotFindGibletModel_S, model_name);
    }
    ScaleModel(gPed_spurt_model, scaling_factor);
    gPed_spurt_model->faces[0].material->index_base = (tU8)index;
    gPed_spurt_model->faces[0].material->index_range = 1;
    gPed_spurt_model->faces[0].material->colour = BR_COLOUR_RGB(r, g, b);
    BrMaterialUpdate(gPed_spurt_model->faces[0].material, BR_MATU_ALL);
}

inline void C2_HOOK_FASTCALL ReadSettingsFile(void) {
    tPath_name the_path;
    FILE* file;
    int i;
    int j;
    float v;

    PathCat(the_path, gApplication_path, gPedsFolder);
    PathCat(the_path, the_path, "SETTINGS.TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        FatalError(kFatalError_CannotOpenPedFile_S, "SETTINGS.TXT");
    }
    gPed_process_distance = GetAScalar(file);
    gPed_process_distance_inner = gPed_process_distance * 0.6;
    gPed_popup_distance = GetAScalar(file);
    gPed_popup_distance_inner = gPed_popup_distance * 0.6;
    gPed_cos_max_slope = BR_COS(BR_ANGLE_DEG(GetAScalar(file)));
    v = GetAScalar(file);
    gPed_reach_squared = v * v;
    v = GetAScalar(file);
    gPed_min_dist_avoid_collisions_squared = v * v;
    gPed_buoyancy_factor = GetAScalar(file);
    GetThreeInts(file, &gPed_points[0], &gPed_points[1], &gPed_points[2]);
    GetThreeInts(file, &gPed_times[0], &gPed_times[1], &gPed_times[2]);
    gPed_severing_damage = GetAnInt(file);
    gPed_severing_chance = GetAnInt(file);
    gPed_max_survivable_impact = GetAScalar(file);
    gPed_damage_per_force = GetAScalar(file);
    ReadInGiblets(file);
    ReadExplosion(file, &gNapalmed_ped_animation);

    gPed_movements_count = GetAnInt(file);
    gPed_movements = BrMemAllocate(gPed_movements_count * sizeof(tPed_movement_spec), kMem_misc_poly_ped);
    for (i = 0; i < gPed_movements_count; i++) {
        gPed_movements[i].min_time_between = GetAScalar(file) * 1000.0f;
        gPed_movements[i].max_time_between = GetAScalar(file) * 1000.0f;
        gPed_movements[i].max_random_angle = BR_ANGLE_DEG(GetAScalar(file));
        gPed_movements[i].min_walk_speed_factor = 1000.0 / (GetAScalar(file) * 30.0);
        gPed_movements[i].max_walk_speed_factor = 1000.0 / (GetAScalar(file) * 30.0);
    }

    gPed_groups_count = GetAnInt(file);
    gPed_groups = BrMemAllocate(gPed_groups_count * sizeof(tPed_group_spec), kMem_misc_poly_ped);
    for (i = 0; i < gPed_groups_count; i++) {
        gPed_groups[i].count = GetAnInt(file);
        gPed_groups[i].members = BrMemAllocate(gPed_groups[i].count * sizeof(tPed_group_member_spec), kMem_misc_poly_ped);
        for (j = 0; j < gPed_groups[i].count; j++) {
            GetAString(file, gPed_groups[i].members[j].name);
        }
    }

    gPed_animal_count = GetAnInt(file);
    for (i = 0; i < gPed_animal_count; i++) {
        GetAString(file, gPed_animal_names[i]);
    }
    PFfclose(file);
}

inline void C2_HOOK_FASTCALL InitOtherPedStuff(void) {

    gSelected_ped = 0;
    gProx_ray_shade_table = GenerateShadeTable(8, gRender_palette, 0xd7, 0xff, 0xe9, 0.5f, 0.75f, 0.9f);
}

// FUNCTION: CARMA2_HW 0x004cb630
// FUNCTION: CARMA2_SW 0x004c7d30
// FUNCTION: CARMA2_CHEAT 0x004d1170
void C2_HOOK_FASTCALL InitNapalmNolts(void) {
    int i;
    int j;

    C2_HOOK_BUG_ON(REC2_ASIZE(gNapalm_bolt_texture_names) != 7);
    C2_HOOK_BUG_ON(REC2_ASIZE(gNapalm_bolt_materials) != REC2_ASIZE(gNapalm_bolt_texture_names));
    C2_HOOK_BUG_ON(REC2_ASIZE(gNapalm_bolt_models) != REC2_ASIZE(gNapalm_bolt_texture_names));
    C2_HOOK_BUG_ON(REC2_ASIZE(gNapalm_bolts[0].actors) != REC2_ASIZE(gNapalm_bolt_texture_names));
    C2_HOOK_BUG_ON(REC2_ASIZE(gNapalm_bolts) != 5);
    C2_HOOK_BUG_ON(sizeof(tNapalm_bolt) != 0x6c);

    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolt_materials); i++) {
        gNapalm_bolt_models[i] = BrModelAllocate(NULL, 4, 2);
        gNapalm_bolt_models[i]->faces[0].vertices[0] = 0;
        gNapalm_bolt_models[i]->faces[0].vertices[1] = 2;
        gNapalm_bolt_models[i]->faces[0].vertices[2] = 1;
        gNapalm_bolt_models[i]->faces[0].material = NULL;
        gNapalm_bolt_models[i]->faces[0].smoothing = 1;
        gNapalm_bolt_models[i]->faces[1].vertices[0] = 0;
        gNapalm_bolt_models[i]->faces[1].vertices[1] = 3;
        gNapalm_bolt_models[i]->faces[1].vertices[2] = 2;
        gNapalm_bolt_models[i]->faces[1].material = NULL;
        gNapalm_bolt_models[i]->faces[1].smoothing = 1;
        BrVector3Set(&gNapalm_bolt_models[i]->vertices[0].p, -0.5f, -0.5f, 0.0f);
        BrVector2Set(&gNapalm_bolt_models[i]->vertices[0].map, 0.0f, 1.0f);
        BrVector3Set(&gNapalm_bolt_models[i]->vertices[1].p, -0.5f, 0.5f, 0.0f);
        BrVector2Set(&gNapalm_bolt_models[i]->vertices[1].map, 0.0f, 0.0f);
        BrVector3Set(&gNapalm_bolt_models[i]->vertices[2].p, 0.5f, 0.5f, 0.0f);
        BrVector2Set(&gNapalm_bolt_models[i]->vertices[2].map, 1.0f, 0.0f);
        BrVector3Set(&gNapalm_bolt_models[i]->vertices[3].p, 0.5f, -0.5f, 0.0f);
        BrVector2Set(&gNapalm_bolt_models[i]->vertices[3].map, 1.0f, 1.0f);
        BrModelAdd(gNapalm_bolt_models[i]);
        gNapalm_bolt_materials[i] = BrMaterialAllocate("BURN!");
        gNapalm_bolt_materials[i]->flags &= ~BR_MATF_LIGHT;
        gNapalm_bolt_materials[i]->flags |= BR_MATF_ALWAYS_VISIBLE;
        gNapalm_bolt_materials[i]->colour_map = BrMapFind(gNapalm_bolt_texture_names[i]);
        if (gNapalm_bolt_materials[i]->colour_map == NULL) {
            FatalError(kFatalError_CantFindPedTexture_S, gNapalm_bolt_texture_names[i]);
        }
#ifdef REC2_HW
        GlorifyMaterial(&gNapalm_bolt_materials[i], 1, kRendererShadingType_Diffuse2);
#endif
        BrMaterialAdd(gNapalm_bolt_materials[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        gNapalm_bolts[i].ped = NULL;
        for (j = 0; j < (int)REC2_ASIZE(gNapalm_bolts[i].actors); j++) {
            gNapalm_bolts[i].actors[j] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
            gNapalm_bolts[i].actors[j]->material = gNapalm_bolt_materials[j];
            gNapalm_bolts[i].actors[j]->model = gNapalm_bolt_models[j];
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
            BrActorAdd(gNon_track_actor, gNapalm_bolts[i].actors[j]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004cadc0
// FUNCTION: CARMA2_SW 0x004c74f0
// FUNCTION: CARMA2_CHEAT 0x004d0900
void C2_HOOK_FASTCALL InitPolyPedSystem(void) {

    if (gPedsFolder == NULL) {
        SetDefaultPedFolderNames();
    }
    InitBoner(&gPed_forms_vtable);
    ReadSettingsFile();
    InitFaceCaches();
    InitNapalmNolts();
    InitOtherPedStuff();
}

inline void C2_HOOK_FASTCALL DisposePedSpawnSpec(void) {
    int i;

    for (i = 0; i < gCount_race_pedestrian_specs; i++) {
        if (gRace_pedestrian_specs[i].exclusion.exclusion_materials != NULL) {
            BrMemFree(gRace_pedestrian_specs[i].exclusion.exclusion_materials);
        }
        if (gRace_pedestrian_specs[i].exclusion.exception_materials != NULL) {
            BrMemFree(gRace_pedestrian_specs[i].exclusion.exception_materials);
        }
    }
    if (gRace_pedestrian_specs != NULL) {
        BrMemFree(gRace_pedestrian_specs);
    }
}

inline void C2_HOOK_FASTCALL DisposeCharacterInstances(void) {
    int i;

    for (i = 0; i < gPed_count; i++) {
        DisposeCharacterInstance(gPedestrian_array[i].character);
    }
}

// FUNCTION: CARMA2_HW 0x004cb8d0
// FUNCTION: CARMA2_SW 0x004c7fd0
// FUNCTION: CARMA2_CHEAT 0x004d1410
void C2_HOOK_FASTCALL DisposePedStuff(void) {

    DisposeAllBonerData();
    DisposeCharacterInstances();
    ClearOutStorageSpace(&gPedStorage);
    if (gPedestrian_array != NULL) {
        BrMemFree(gPedestrian_array);
    }
    DisposePedSpawnSpec();
}

// FUNCTION: CARMA2_HW 0x004cb980
// FUNCTION: CARMA2_SW 0x004c8080
// FUNCTION: CARMA2_CHEAT 0x004d14c0
FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "FORMS");
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}

// FUNCTION: CARMA2_HW 0x004cba10
// FUNCTION: CARMA2_SW 0x004c8110
// FUNCTION: CARMA2_CHEAT 0x004d1550
FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}

// FUNCTION: CARMA2_HW 0x004cbaa0
// FUNCTION: CARMA2_SW 0x004c81a0
// FUNCTION: CARMA2_CHEAT 0x004d15e0
FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    gTwtPeds = OpenPackFileAndSetTiffLoading(path);
    PathCat(path, path, pName);
    strcat(path, ".TXT");
    return DRfopen(path, "rt");
}

// FUNCTION: CARMA2_HW 0x004cbb50
// FUNCTION: CARMA2_SW 0x004c8250
// FUNCTION: CARMA2_CHEAT 0x004d1690
FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "MOVES");
    PathCat(path, path, pName);
    return DRfopen(path, "rb");
}

// FUNCTION: CARMA2_HW 0x004cbbb0
// FUNCTION: CARMA2_SW 0x004c82b0
// FUNCTION: CARMA2_CHEAT 0x004d16f0
FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "REMAPS");
    PathCat(path, path, pFile_name);
    return DRfopen(path, "rb");
}

// FUNCTION: CARMA2_HW 0x004cbc10
// FUNCTION: CARMA2_SW 0x004c8310
// FUNCTION: CARMA2_CHEAT 0x004d1750
void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName) {
    tPath_name path;

    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "PEEPS");
    PathCat(path, path, pName);
    DisallowDuplicates();
    LoadAllStuffInDirectory(&gPedStorage, path, kRendererShadingType_Diffuse1);
    AllowDuplicates();
}

// FUNCTION: CARMA2_HW 0x004cbc70
// FUNCTION: CARMA2_SW 0x004c8370
// FUNCTION: CARMA2_CHEAT 0x004d17b0
void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pBone_index, tPed_character_instance* pCharacter) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gPow2_array) != 32);

    if (pActor->parent == NULL && (pBone_index == 0 || (pCharacter->field_0xc & gPow2_array[pBone_index]) != 0)) {
        BrActorAdd(gNon_track_actor, pActor);
    }
}

// FUNCTION: CARMA2_HW 0x004cbca0
// FUNCTION: CARMA2_SW 0x004c83a0
// FUNCTION: CARMA2_CHEAT 0x004d17e0
void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor) {

    if (pActor->parent != NULL) {
        BrActorRemove(pActor);
    }
}

// FUNCTION: CARMA2_HW 0x004cbcc0
// FUNCTION: CARMA2_SW 0x004c83c0
// FUNCTION: CARMA2_CHEAT 0x004d1800
int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor) {
    if (pActor->parent != NULL) {
        return 1;
    } else {
        return 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cc2c0
// STUB: CARMA2_SW 0x004c89c0
// STUB: CARMA2_CHEAT 0x004d1e30
void C2_HOOK_FASTCALL SetPedXZDirection(tPedestrian* pPed, br_vector3* pDir, float pSpeed, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL StopPedNoise(tPedestrian* pPed) {

    if (pPed->field_0x0c != NULL && pPed->field_0x0c->field_0x9c != 0 && pPed->field_0x0c->field_0xa0 >= 0) {
        if (DRS3SoundStillPlaying(pPed->field_0x0c->field_0x9c)) {
            DRS3StopSound(pPed->field_0x0c->field_0x9c);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cd880
// STUB: CARMA2_SW 0x004c9f80
// STUB: CARMA2_CHEAT 0x004d3410
void C2_HOOK_FASTCALL MakePedNoise(tPedestrian* pPed, int pArg2, int pArg3, tPhysics_object* pCollision_info) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004cbcd0
// FUNCTION: CARMA2_SW 0x004c83d0
// FUNCTION: CARMA2_CHEAT 0x004d1810
void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, tU32 pTime, tPed_action pAction) {
    tPed_action original_action;
    int original_move_id;
    int original_field_0x1e;
    int original_field_0x1c;
    br_vector3 original_pos;
    br_matrix34* character_matrix;
    int new_move_lut_id;
    int i = 0;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_move_fsm) != 118);
    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_move_fsm[0]) != 7);

    original_action = pPed->action;
    original_move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
    original_field_0x1e = (tS16)pPed->character->field_0x1e;
    original_field_0x1c = pPed->character->field_0x1c;
    character_matrix = GetCharacterMatrixPtr(pPed->character);
    BrVector3Copy(&original_pos, (br_vector3*)character_matrix->m[3]);
    if ((int)pAction >= 0 && pMove_action >= 0 && pPed->action != ePed_action_dead) {
        pPed->action = pAction;
    }
    if (gDrunk_pedestrians) {
        new_move_lut_id = 4;
    } else if (gDancing_peds) {
        new_move_lut_id = 3;
    } else if (gPanicking_peds) {
        new_move_lut_id = 5;
    } else if (pPed->character->field_0xc & gPow2_array[3]) {
        new_move_lut_id = 1;
    } else if (pPed->character->field_0xc & gPow2_array[5]) {
        new_move_lut_id = 2;
    } else {
        new_move_lut_id = 0;
    }
    if (new_move_lut_id != 0) {
        pMove_action = gPed_move_fsm[pMove_action][new_move_lut_id + 1];
    }
    if (pMove_action == 112 || pMove_action == 31 || pMove_action == 111) {
        pPed->action = ePed_action_running;
    }
    if (pPed->field_0x0c != NULL && pMove_action != 112 && pMove_action != 31
            && pMove_action != 40 && pMove_action != 41 && pMove_action != 42) {
        pPed->field_0x0c->field_0x80 = 0;
    }
    for (; i < 10; i++) {
        int move_result;
        float speed;

        if (gPedestrian_speed_factor != 0.f) {
            speed = (float)pWalk_speed_factor / gPedestrian_speed_factor;
        } else {
            speed = (float)pWalk_speed_factor;
        }
        move_result = SetCharacterMove(pPed->character, pMove_action, speed, pArg4, pArg5, pTime);
        if (move_result == 2) {
            pMove_action = gPed_move_fsm[pMove_action][1];
            if (pMove_action <= -2) {
                char buffer[256];

                sprintf(buffer, "%d", pMove_action);
                FatalError(pMove_action == -2 ? kFatalError_CoreMoveMissingFor_SS : kFatalError_BadMoveAttemptedFor_SS,
                    buffer, pPed->character->personality->name);
            }
        }
        if (pMove_action == -1 || move_result != 2) {
            break;
        }
    }
    if (i >= 10) {
        char buffer[256];

        sprintf(buffer, "%d", pMove_action);
        FatalError(kFatalError_CyclicMoveDefinitionFor_SS, buffer, pPed->character->personality->name);
    }
    PipeSinglePedMove(pPed, (tS16)original_field_0x1c, (tS16)original_move_id,
        (tS16)pMove_action, (float)original_field_0x1e, (float)pWalk_speed_factor, pArg5, pArg4,
        &original_pos, (br_vector3*)character_matrix->m[3], original_action, pPed->action, &pPed->character->field_0x8c);
#ifdef REC2_CHEAT
    if (pPed->action == ePed_action_impacting && pPed->character->personality->form->moves[pPed->character->field_0x7].id == 0x29) {
        PDFatalError("Earwig alert! (My uncle has some spam.)");
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004cc660
// FUNCTION: CARMA2_SW 0x004c8d60
// FUNCTION: CARMA2_CHEAT 0x004d21d0
void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3) {

    if (!(pPed->character->field_0xc & gPow2_array[3])
        && !(pPed->character->field_0xc & gPow2_array[5])
        && pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x80 = 100;
        pPed->field_0x0c->field_0xb8 = 0.09f;
        SetPedMove(pPed, pArg3 ? 40 : 41, pPed->field_0x0c->field_0x80, 0, 0, pTime, ePed_action_running);
    } else {
        SetPedMove(pPed, pArg3 ? 40 : 41, -1, 0, 0, pTime, ePed_action_running);
    }
}

// AcceleratePed

// MungePedHeadAnim

inline void C2_HOOK_FASTCALL SetPedHeadAnim(tPedestrian* pPed, const tPed_anim_seq* pSeq, tU32 pTime) {

    if (pPed->hit_points > 0 && pPed->field_0x0c != NULL) {
        pPed->field_0x0c->field_0x1c = pSeq;
        pPed->field_0x0c->field_0x78 = pTime;
        pPed->field_0x0c->field_0x1a = 0;
        SetCharacterBoneModelAR(pPed->character, pPed->character->personality->form->index_head_bone, pPed->field_0x0c->field_0x1c->poses[0], 0);
    }
}

// ClearPedHeadAnim

inline void C2_HOOK_FASTCALL ScarePedestrian(tPedestrian* pPed, tU32 pTime, int pPanic, int pForce) {

    if (((pPed->action != ePed_action_running && pPed->action != ePed_action_panicking) || pForce) && pPed->action != ePed_action_getting_up) {
        if (!pPanic) {
            StartPedRunning(pPed, pTime, 0);
        } else {
            SetPedMove(pPed, gPed_move_fsm[114][IRandomBetween(0, 7)], -1, 0, 1, pTime, ePed_action_panicking);
            MakePedNoise(pPed, 6, 0, NULL);
            SetPedHeadAnim(pPed, &gScare_ped_anim_sequence, pTime);
        }
    }
    pPed->field_0x28 = pTime;
}

// PedAnimCausesMovement

// StopSmoothTurning

inline void C2_HOOK_FASTCALL SetKnockedOverMove(tPedestrian* pPed, tU32 pTime) {

    if (pPed->hit_points > 0) {
        SetPedMove(pPed, 0x5d, -1, 0, 0, pTime, ePed_action_impacting);
        SetPedHeadAnim(pPed, &gKnocked_over_ped_anim_sequence, pTime);
    } else {
        SetPedMove(pPed, 0x5e, -1, 0, 0, pTime, ePed_action_impacting);
    }
}

// FUNCTION: CARMA2_HW 0x004cc6e0
// FUNCTION: CARMA2_SW 0x004c8de0
// FUNCTION: CARMA2_CHEAT 0x004d2250
void C2_HOOK_FASTCALL DoPostElectricution(tPedestrian* pPed, tU32 pTime, float pSpeed, float pSpin) {
    tPhysics_object* object;
    float spin;

    SetKnockedOverMove(pPed, pTime);
    if (!(pPed->character->field_0x14 & 0x4)) {
        SetCharacterPhysicsLevelAR(pPed->character, 5);
    }
    object = pPed->character->personality->form->simple_physicing[(tS8)pPed->character->field_0x5].collision_info;
    if (object != NULL) {
        pSpeed *= .7;
        object->v.v[0] = SRandomPosNeg(pSpeed);
        object->v.v[1] = SRandomBetween(0.0f, pSpeed);
        object->v.v[2] = SRandomPosNeg(pSpeed);
        spin = pSpin * 4.0;
        object->omega.v[0] = SRandomPosNeg(spin);
        object->omega.v[1] = SRandomPosNeg(spin);
        object->omega.v[2] = SRandomPosNeg(spin);
        object->disable_move_rotate = 0;
        object->field_0x49c = gPHIL_last_physics_tick + 40;
    }
}

// FUNCTION: CARMA2_HW 0x004cbf20
// FUNCTION: CARMA2_SW 0x004c8620
// FUNCTION: CARMA2_CHEAT 0x004d1a90
int C2_HOOK_FASTCALL CBMoveCompleted(tPed_character_instance* pCharacter) {
    tU32 time;
    tPedestrian* ped;
    int move_id;
    br_vector3 dir;

    time = GetTotalTime();
    ped = pCharacter->ped;
    move_id = pCharacter->personality->form->moves[pCharacter->field_0x7].id;
    if (pCharacter->field_0x14 & 0x4) {
        EnableRetainRootMode();
    }
    switch (move_id) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 10:
    case 11:
        if (ped->field_0x0c != NULL && ped->field_0x0c->field_0x20[0] != 0) {
            StopPedNoise(ped);
        }
        if (!gPeds_suicidal) {
            BrVector3Scale(&dir, &pCharacter->field_0xc0, -1.0f);
            SetPedXZDirection(ped, &dir, 0.0f, time);
        }
        StartPedRunning(ped, time, 0);
        break;
    case 13:
        StartPedRunning(ped, time, 1);
        break;
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
        if (!gBlind_pedestrians && !gPed_valium_left) {
            ped->action = ePed_action_walking;
            ScarePedestrian(ped, time, 0, 0);
        } else {
            SetPedMove(ped, 0x1e, -1, 0, 0, time, ePed_action_walking);
        }
        break;
    case 90:
    case 91:
    case 92:
        SetPedMove(ped, 0x61, -1, 0, 1, time, ePed_action_impacting);
        break;
    case 96:
    case 97:
        SetKnockedOverMove(ped, time);
        break;
    case 113:
        DoPostElectricution(ped, time, 1.0f, 1.0f);
        break;
    }
    if (ped->field_0x0c != NULL && ped->field_0x0c->field_0xb0 != NULL) {
        ped->field_0x0c->field_0xb0->field_0x14 = NULL;
        ped->field_0x0c->field_0xb0 = NULL;
    }
    DisableRetainRootMode();
    return 1;
}

// FUNCTION: CARMA2_HW 0x004cc900
// FUNCTION: CARMA2_SW 0x004c9000
// FUNCTION: CARMA2_CHEAT 0x004d2470
void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF) {

    C2_HOOK_BUG_ON(REC2_ASIZE(gForm_non_humanoid_names) != 1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, non_humanoid, 0x4c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, index_head_bone, 0x4d);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_A, 0x4e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_form, axis_when_lying_down_B, 0x4f);

    /* Other stuff */

    pPed_form->non_humanoid = GetALineAndInterpretCommand(pF, gForm_non_humanoid_names, REC2_ASIZE(gForm_non_humanoid_names)) == 0;

    /* Index of head bone */
    pPed_form->index_head_bone = (tS8)GetAnInt(pF);

    /* Direction axis when lying down (A) */
    pPed_form->axis_when_lying_down_A = (tS8)GetAnInt(pF);

    /* Direction axis when lying down (B) */
    pPed_form->axis_when_lying_down_B = (tS8)GetAnInt(pF);
}

// FUNCTION: CARMA2_HW 0x004cc940
// FUNCTION: CARMA2_CHEAT 0x004d24b0
void C2_HOOK_FASTCALL CBDisposeForm(tPed_form* pForm) {
}

// FUNCTION: CARMA2_HW 0x004cc950
// FUNCTION: CARMA2_SW 0x004c9050
// FUNCTION: CARMA2_CHEAT 0x004d24c0
void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF) {
    char s[256];
    tPath_name path;
    FILE* sound_f;
    tPed_personality_sounds* sounds;
    int i;
    int j;

    C2_HOOK_BUG_ON(sizeof(tPed_personality_sounds) != 0x84);
    C2_HOOK_BUG_ON(REC2_ASIZE(sounds->sounds) != 11);

    GetAString(pF, s);
    PathCat(path, gApplication_path, gPedsFolder);
    PathCat(path, path, "SOUND");
    PathCat(path, path, s);
    sound_f = DRfopen(path, "rt");
    if (sound_f == NULL) {
        FatalError(kFatalError_CannotOpenPedFile_S, s);
    }

    sounds = BrMemAllocate(sizeof(*sounds), kMem_misc_poly_ped);
    pPersonality->sounds = sounds;
    for (i = 0; i < REC2_ASIZE(sounds->sounds); i++) {
        sounds->sounds[i].count_sounds = (tU16)GetAnInt(sound_f);
        for (j = 0; j < sounds->sounds[i].count_sounds; j++) {
            sounds->sounds[i].sounds[j] = (tU16)LoadSingleSound(&gPedStorage, GetAnInt(sound_f));
        }
    }
    PFfclose(sound_f);

    pPersonality->jump_height = 0.f;
    if (!PFfeof(pF)) {
        GetAString(pF, s);
        if (DRStricmp(s, "JUMPHEIGHT") == 0) {
            pPersonality->jump_height = GetAScalar(pF);
        }
    }
    ClosePackFileAndSetTiffLoading(gTwtPeds);
}

// FUNCTION: CARMA2_HW 0x004ccaa0
// FUNCTION: CARMA2_SW 0x004c91a0
// FUNCTION: CARMA2_CHEAT 0x004d2610
void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_personality, sounds, 0x64);

    BrMemFree(pPersonality->sounds);
}

// FUNCTION: CARMA2_HW 0x004ccab0
// FUNCTION: CARMA2_SW 0x004c91b0
// FUNCTION: CARMA2_CHEAT 0x004d2620
void C2_HOOK_FASTCALL CBFillInObject(tPhysics_object* pObject, undefined4 pType) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1a0, 0x1a0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1a4, 0x1a4);

    if (pType == 5) {
        pObject->field_0x1a0 = 0x40000;
        pObject->field_0x1a4 = 0x50000;
    } else {
        pObject->field_0x1a0 = 0x10000;
        pObject->field_0x1a4 = 0x50000;
    }
}

// FUNCTION: CARMA2_HW 0x004ccb10
// FUNCTION: CARMA2_SW 0x004c9210
// FUNCTION: CARMA2_CHEAT 0x004d2680
void C2_HOOK_FASTCALL DoGibShower(tU32 pId, tU32 pTime, unsigned int pSeed, float pForce, int pStart_size, br_vector3* pPosition, br_vector3* pAlternate_position, br_vector3* pImpactee_velocity) {
    int seed;
    int count;
    int i;
    int j;
    tPed_giblet_size_spec* size;
    tSmash_vertex* fragment;
    br_scalar scale;

    seed = rand();
    srand(pSeed);
    for (j = pStart_size, size = &gGiblet_sizes[pStart_size]; j < gGiblet_size_count; j++, size++) {
        if (pForce >= size->min_force) {
            count = (int)((pForce - size->min_force) / size->instance_force + 1.f);
            if (count > size->max_count) {
                count = size->max_count;
            }
            for (i = 0; i < count; i++) {
                fragment = FindShrapnelFragment(size->models[IRandomBetween(0, size->model_count - 1)], pId, 2);
                if (fragment == NULL) {
                    break;
                }
                scale = SetInitPosition(fragment->actor, &gGiblet_initial_position, NULL, pPosition, pAlternate_position, NULL, NULL, NULL);
                SetShrapnelFragment(fragment, &gGiblet_initial_speed, pImpactee_velocity, scale, &gGiblet_min_max_time, pTime, &gZero_vector__smash, NULL, NULL);
            }
        }
    }
    srand(seed);
}

// FUNCTION: CARMA2_HW 0x004ccae0
// FUNCTION: CARMA2_SW 0x004c91e0
// FUNCTION: CARMA2_CHEAT 0x004d2650
void C2_HOOK_FASTCALL PedApplyGibShower(tU32 pId, tU32 pTime, unsigned int pSeed, float pForce, int pStart_size, br_vector3* pPosition, br_vector3* pAlternate_position, br_vector3* pImpactee_velocity) {

    DoGibShower(pId, pTime, pSeed, pForce, pStart_size, pPosition, pAlternate_position, pImpactee_velocity);
}

// PedUndoGibShower

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004ccff0
// STUB: CARMA2_SW 0x004c96f0
// STUB: CARMA2_CHEAT 0x004d2b60
void C2_HOOK_FASTCALL DoGiblets(tPedestrian* pPed, tPhysics_object* pObject, tPhysics_object* pOther_object, float pForce, br_vector3* pPos, int pStart_size) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL DoBloodSpurt(tU32 pId, tU32 pTime, unsigned int pSeed, br_vector3* pPosition, br_vector3* pImpactee_velocity, br_vector3* pNormal) {
    int seed;
    int i;
    tSmash_vertex* fragment;
    br_scalar scale;

    seed = rand();
    srand(pSeed);
    for (i = 0; i < gPed_spurty_lumps_count; i++) {
        fragment = FindShrapnelFragment(gPed_spurt_model, pId, 1);
        if (fragment == NULL) {
            break;
        }
        scale = SetInitPosition(fragment->actor, &gPed_spurt_initial_position, NULL, pPosition, pPosition, NULL, NULL, NULL);
        SetShrapnelFragment(fragment, &gPed_spurt_initial_speed, pImpactee_velocity, scale, &gPed_spurt_min_max_time, pTime, pNormal, NULL, NULL);
    }
    srand(seed);
}

// DoSpurt

// FUNCTION: CARMA2_HW 0x004cccc0
// FUNCTION: CARMA2_SW 0x004c93c0
// FUNCTION: CARMA2_CHEAT 0x004d2830
void C2_HOOK_FASTCALL PedApplyBloodSpurt(tU32 pId, tU32 pTime, unsigned int pSeed, br_vector3* pPosition, br_vector3* pImpactee_velocity, br_vector3* pNormal) {

    DoBloodSpurt(pId, pTime, pSeed, pPosition, pImpactee_velocity, pNormal);
}

// FUNCTION: CARMA2_HW 0x004ccd70
// FUNCTION: CARMA2_SW 0x004c9470
// FUNCTION: CARMA2_CHEAT 0x004d28e0
void C2_HOOK_FASTCALL PedUndoBloodSpurt(tU32 pCurrent_time, tU32 pId, tU32 pTime, unsigned int pSeed, br_vector3* pPosition, br_vector3* pImpactee_velocity, br_vector3* pNormal) {
    int i;
    tSmash_vertex* fragment;
    float delta_time;

    DoBloodSpurt(pId, pTime, pSeed, pPosition, pImpactee_velocity, pNormal);
    delta_time = (float)(tS32)(pCurrent_time - pTime);
    for (i = 0, fragment = gSmash_glass_fragments; i < REC2_ASIZE(gSmash_glass_fragments); i++, fragment++) {
        if (fragment->end_time != 0 && fragment->field_0x18 == pId) {
            fragment->field_0x34 = 0;
            DoFragMovement(fragment, GetTotalTime(), delta_time);
            fragment->field_0x34 = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004cd160
// FUNCTION: CARMA2_SW 0x004c9860
// FUNCTION: CARMA2_CHEAT 0x004d2cd0
void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed) {
    tS16 flags;

    flags = pPed->flags & 0x100;
    if (flags == 0) {
        pPed->flags |= 0x100;
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 && ++gCount_killed_peds >= gPed_count) {
            NewTextHeadupSlot(4, 0, 5000, -4, GetMiscString(eMiscString_every_pedestrian_killed));
            RaceCompleted(eRace_over_1);
            return;
        }
        if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
            flags = pPed->flags & 0x200;
            if (flags != 0 && ++gCount_killed_peds >= gTotal_count_smash_peds) {
                RaceCompleted(eRace_over_1);
                return;
            }
        }
        if (gCurrent_race.race_spec->race_type == kRaceType_SmashNPed) {
            flags = pPed->flags & 0x200;
            if (flags != 0) {
                AccumulateRuntimeVariable(gCurrent_race.race_spec->options.smash_and_peds.var_smash_number, 1);
                if (GetRuntimeVariable(gCurrent_race.race_spec->options.smash_and_peds.var_smash_number) >= gCurrent_race.race_spec->options.smash_and_peds.var_smash_target) {
                    RaceCompleted(eRace_over_3);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004cd260
// FUNCTION: CARMA2_SW 0x004c9960
// FUNCTION: CARMA2_CHEAT 0x004d2dd0
void C2_HOOK_FASTCALL ScoreForKilledPedestrian(tPedestrian* pPed, float pHeight) {
    tU32 the_time;
    tCar_spec* car;
    tPhysics_object* ped_object;
    br_vector3 offset;
    br_vector3 local_offset;
    tU16 flags;
    int amount;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, field_0x30, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1b4, 0x1b4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPhysics_object, field_0x1b4, 12);

    the_time = GetTotalTime();
    pPed->hit_points = 0;
    if (pPed->action != ePed_action_dead) {
        PipeSinglePedMove(pPed, -1, -1, -1, 0.0f, 0.0f, 0, 0, NULL, NULL, pPed->action, ePed_action_dead, NULL);
        pPed->action = ePed_action_dead;
    }
#ifdef REC2_CHEAT
    if (pPed->action == ePed_action_walking || pPed->action == ePed_action_running) {
        PDFatalError("*** PED SYNCH MISMATCH - TELL BATWICK NOW!!!! ***");
    }
#endif
    pPed->field_0x30 = the_time;
    if (pPed->field_0x0c == NULL) {
        return;
    }
    car = pPed->field_0x0c->field_0xc0;
    if (car == NULL || car->driver != eDriver_local_human) {
        return;
    }
    if (the_time - gDAT_006a0418 <= 1000) {
        if (gPed_score_multiplier < 10) {
            gPed_score_multiplier++;
        }
    } else {
        gPed_score_multiplier = 1;
    }
    gDAT_006a0418 = the_time;
    amount = gPed_points[gProgram_state.skill_level] * gPed_score_multiplier;
    ped_object = pPed->character->personality->form->simple_physicing[(tS8)pPed->character->field_0x5].collision_info;
    BrVector3Sub(&offset, &pPed->pos, &car->pos);
    BrMatrix34TApplyV(&local_offset, &offset, &car->collision_info->actor->t.t.mat);
    flags = pPed->field_0x0c->field_0x2e;
    if (flags & 0x10) {
        amount *= 20;
        PratcamEvent(30);
        DoFancyHeadup(32);
    } else if (flags & 0x8) {
        amount *= 5;
        PratcamEvent(30);
        DoFancyHeadup(31);
    } else if (flags & 0x40) {
        amount *= 2;
        PratcamEvent(30);
        DoFancyHeadup(2);
    } else if (flags & 0x1) {
        amount *= 4;
        PratcamEvent(30);
        DoFancyHeadup(17);
    } else if (flags & 0x20) {
        amount *= 2;
        PratcamEvent(30);
        DoFancyHeadup(30);
    } else if ((pHeight == 0.0f
                    && ((float)fabs(car->collision_info->omega.v[0]) > 5.0f
                        || (float)fabs(car->collision_info->omega.v[2]) > 5.0f
                        || car->collision_info->actor->t.t.mat.m[1][1] < 0.1f
                        || car->collision_info->actor->t.t.mat.m[3][1] - pPed->pos.v[1] > 0.05f))
            || pPed->field_0x0c->field_0x98 > 0) {
        amount *= 4;
        PratcamEvent(30);
        DoFancyHeadup(3);
    } else if (pHeight > 1.0) {
        amount *= 2;
        DoFancyHeadup(6);
        PratcamEvent(30);
    } else {
        if (ped_object == NULL
                || (BrVector3LengthSquared(&ped_object->v) < 2.0
                    && BrVector3LengthSquared(&ped_object->field_0x1b4) < 2.0)) {
            if (pHeight == 0.0f
                    && (((float)fabs(local_offset.v[0]) / (float)fabs(local_offset.v[2]) >= 0.5
                            && ((float)fabs(car->collision_info->velocity_car_space.v[0]) > (float)fabs(car->collision_info->velocity_car_space.v[2])
                                || (float)fabs(car->collision_info->omega.v[1] / car->collision_info->velocity_car_space.v[2]) > 600.0f))
                        || (local_offset.v[2] > 0.0f && car->collision_info->velocity_car_space.v[2] > 0.0f))) {
                amount *= 2;
                DoFancyHeadup(2);
            } else if (gPed_score_multiplier > 1) {
                DoFancyHeadup(gPed_score_multiplier + 6);
                PratcamEvent(30);
            } else if (flags & 0x2) {
                amount *= 2;
                DoFancyHeadup(0);
                PratcamEvent(30);
            }
        } else {
            amount *= 4;
            DoFancyHeadup(5);
            PratcamEvent(30);
        }
    }
    EarnCredits(amount);
    gPed_recent_points = amount;
    gPTR_00694120 = pPed;
    AwardTime(gPed_times[gProgram_state.skill_level]);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cce70
// STUB: CARMA2_SW 0x004c9570
// STUB: CARMA2_CHEAT 0x004d29e0
void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tPhysics_object* pCollision_info) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004cd9e0
// FUNCTION: CARMA2_SW 0x004ca0e0
// FUNCTION: CARMA2_CHEAT 0x004d3570
int C2_HOOK_FASTCALL DismemberPed(tPedestrian* pPed, int pBone_index) {
    tPed_character_instance* character;
    br_vector3 velocity;
    br_vector3 angular_velocity;
    int count;
    int result;
    int i;

    character = pPed->character;
    count = 0;
    for (i = 0; i < character->personality->form->count_bones; i++) {
        if (character->personality->form->bones[i].indices[0] == pBone_index && !(character->field_0xc & gPow2_array[i])) {
            count += DismemberPed(pPed, i);
        }
    }
    pPed->field_0x0c->field_0x23[pBone_index] = (tU8)gPed_dismember_spurting_member_count;
    if (gExploding_pedestrians) {
        BrVector3Set(&velocity,
            SRandomPosNeg(3.0f),
            SRandomBetween(0.0f, 3.0f),
            SRandomPosNeg(3.0f));
        BrVector3Add(&velocity, &character->field_0xd8, &velocity);
        BrVector3Set(&angular_velocity,
            SRandomPosNeg(20.0f),
            SRandomPosNeg(20.0f),
            SRandomPosNeg(20.0f));
        result = DismemberCharacterAR(character, pBone_index, &velocity, &angular_velocity);
        return result + count;
    } else {
        result = DismemberCharacterAR(character, pBone_index, NULL, NULL);
        return result + count;
    }
}

// FUNCTION: CARMA2_HW 0x004cd640
// FUNCTION: CARMA2_SW 0x004c9d40
// FUNCTION: CARMA2_CHEAT 0x004d31d0
int C2_HOOK_FASTCALL DamagePedestrian(tPedestrian* pPed, int pDamage, int pArg3, int pArg4, tPhysics_object* pObject) {
    tPed_form* form;
    br_scalar mass_factor;
    br_scalar damage_left;
    int count;
    int severing_damage;

    count = 0;
    form = pPed->character->personality->form;
    mass_factor = 0.068 / form->simple_physicing[(tS8)pPed->character->field_0x5].collision_info->M;
    pDamage = (int)(mass_factor * mass_factor * pDamage);
    if (form->non_humanoid && (gPed_dismemberfest || gExploding_pedestrians) && pDamage < 100) {
        pDamage = 100;
    }
    damage_left = (br_scalar)pDamage;
    if (pObject != NULL && !gImmortal_peds && gGoreLevel >= 2) {
        while (1) {
            if (gPed_dismemberfest) {
                severing_damage = 5;
            } else {
                severing_damage = gPed_severing_damage;
            }
            damage_left -= severing_damage;
            if (damage_left < 0.0f || count >= 5) {
                break;
            }
            if (gExploding_pedestrians || gPed_dismemberfest || PercentageChance(gPed_severing_chance)) {
                count += DismemberPed(pPed, IRandomBetween(1, pPed->character->personality->form->count_bones - 1));
            }
        }
    }
    if (count > 4) {
        pPed->field_0x0c->field_0x2e |= 0x2;
    }
    if (pPed->hit_points <= 0) {
        if (pDamage > 2) {
            MakePedNoise(pPed, 5, 0, pObject);
        }
        return 1;
    }
    if (pPed->character->personality->form->non_humanoid && pPed->character->field_0xc) {
        pPed->hit_points = 0;
        pDamage = 0;
    }
    if (pPed->character->field_0xc & gPow2_array[pPed->character->personality->form->index_head_bone]) {
        pPed->hit_points = 0;
    } else if (pDamage >= 100) {
        pPed->hit_points = 0;
    } else {
        pPed->hit_points -= pDamage;
    }
    if (gImmortal_peds && pPed->hit_points <= 0) {
        pPed->hit_points = 1;
    }
    if (pPed->hit_points > 0) {
        if (pArg3 >= 0) {
            SetPedMove(pPed, pArg3, -1, 0, 1, GetTotalTime(), ePed_action_impacting);
        }
        MakePedNoise(pPed, 4, 0, pObject);
        return 0;
    } else {
        if (pArg4 >= 0) {
            SetPedMove(pPed, pArg4, -1, 0, 1, GetTotalTime(), ePed_action_impacting);
        }
        MakePedNoise(pPed, 1, 0, pObject);
        KillPedestrian(pPed, pObject);
        return 1;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cdeb0
// STUB: CARMA2_SW 0x004ca5b0
// STUB: CARMA2_CHEAT 0x004d3a40
void C2_HOOK_FASTCALL SetLastHitter(tPedestrian* pPed, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// MakePedVanish

// STUB: CARMA2_HW 0x004cdc00
// STUB: CARMA2_SW 0x004ca300
// STUB: CARMA2_CHEAT 0x004d3790
int C2_HOOK_FASTCALL CBPassiveCollision(tPed_character_instance* pCharacter, tPhysics_object* pObject, tPhysics_object* pOther_object) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ce050
// FUNCTION: CARMA2_SW 0x004ca750
// FUNCTION: CARMA2_CHEAT 0x004d3be0
int C2_HOOK_FASTCALL BastardIt(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pTime, void* pContext) {
    int i;

    pPed->field_0x0c->field_0xc0 = pCar;
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        PipeSinglePedIncident(pPed, pCar->collision_info->actor);
    }
    if (!gImmortal_peds) {
        KillPedestrian(pPed, pCar->collision_info);
        SetPedMove(pPed, 0x71, -1, 0, 1, pTime, ePed_action_dead);
    }
    for (i = 0; i < (int)REC2_ASIZE(gProximity_rays); i++) {
        if (gProximity_rays[i].start_time == 0) {
            gProximity_rays[i].start_time = GetTotalTime();
            gProximity_rays[i].car = pCar;
            gProximity_rays[i].ped = pPed;
            if (pCar != NULL && pCar->driver == eDriver_local_human) {
                DRS3StartSound(gCar_outlet, 0x157c);
            } else {
                DRS3StartSound3D(gCar_outlet, 0x157c, &pCar->pos, &gZero_v__car, 1, 255, 0x10000, 0x10000);
            }
            return 0;
        }
    }
    return 0;
}

inline int C2_HOOK_FASTCALL MungeProxRay(tPedestrian* pPed, tPhysics_object* pObject, float pDistance_squared, tU32 pTime) {
    tCar_spec* car;

    if (!gGhost_pedestrians && pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        car = pObject->owner;
        if (car != NULL && car->driver > eDriver_5 && pDistance_squared < car->proxy_ray_distance) {
            BastardIt(pPed, car, pDistance_squared, pTime, NULL);
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ce140
// FUNCTION: CARMA2_SW 0x004ca840
// FUNCTION: CARMA2_CHEAT 0x004d3cd0
int C2_HOOK_FASTCALL DoToPeds(tCar_spec* pCar, int pOnly_alive, float pRange_squared, int pCheck_visible, void* pContext, int (C2_HOOK_FASTCALL* pCallback)(tPedestrian* pPed, tCar_spec* pCar, float pDistance_squared, tU32 pTime, void* pContext)) {
    int result;
    float distance_squared;
    br_scalar t;
    br_material* material;
    tU32 time;
    br_vector3 normal;
    br_vector3 dir;
    tPedestrian* ped;
    br_vector3* pos;

    result = 0;
    if (gGhost_pedestrians) {
        return 0;
    }
    pos = &pCar->pos;
    time = GetTotalTime();
    for (ped = gPedestrians_in_sight; ped != NULL; ped = ped->next) {
        distance_squared = (pos->v[0] - ped->pos.v[0]) * (pos->v[0] - ped->pos.v[0]) + (pos->v[1] - ped->pos.v[1]) * (pos->v[1] - ped->pos.v[1]) + (pos->v[2] - ped->pos.v[2]) * (pos->v[2] - ped->pos.v[2]);
        if (!pOnly_alive || ped->hit_points > 0) {
            if (pRange_squared == 0.0f || distance_squared < pRange_squared) {
                if (pCheck_visible) {
                    BrVector3Sub(&dir, &ped->pos, pos);
                    FindFace(pos, &dir, &normal, &t, &material);
                } else {
                    t = 100.0f;
                }
                if (t >= 1.0) {
                    pCallback(ped, pCar, distance_squared, time, pContext);
                    result = 1;
                }
            }
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004ce030
// FUNCTION: CARMA2_SW 0x004ca730
// FUNCTION: CARMA2_CHEAT 0x004d3bc0
int C2_HOOK_FASTCALL DoPedAnnihilator(tCar_spec* pCar, float pRange_squared) {

    return DoToPeds(pCar, 1, pRange_squared, 1, NULL, BastardIt);
}

// RepulseIt

// DoPedestrianRepulsifictor

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

inline void C2_HOOK_FASTCALL KillNapalmBolt(tNapalm_bolt* pBolt) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pBolt->actors); i++) {
        pBolt->actors[i]->render_style = BR_RSTYLE_NONE;
    }
    pBolt->ped = NULL;
}

// FUNCTION: CARMA2_HW 0x004ce7d0
// FUNCTION: CARMA2_SW 0x004caed0
// FUNCTION: CARMA2_CHEAT 0x004d4360
void C2_HOOK_FASTCALL MungeNapalm(void) {
    tU32 the_time;
    int i;
    int j;
    tNapalm_bolt* bolt;
    tPed_character_instance* character;
    tPhysics_object* object;
    br_actor* actor;
    br_vector3 delta;
    br_vector3 local_delta;
    br_vector3 positions[7];
    float dist_squared;
    float dt;
    int age;
    int hit;
    int t;
    int k;

    C2_HOOK_BUG_ON(REC2_ASIZE(positions) != REC2_ASIZE(gFlamed_ped_flame_scales));
    C2_HOOK_BUG_ON(REC2_ASIZE(positions) != REC2_ASIZE(gNapalm_bolts[0].actors));

    the_time = GetTotalTime();
    ARStartPipingSession(ePipe_chunk_burning_ped);
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        bolt = &gNapalm_bolts[i];
        if (bolt->ped == NULL) {
            continue;
        }
        if (!gAction_replay_mode) {
            if (!(bolt->ped->flags & 0x1)) {
                bolt->ped->field_0x0c->field_0x98 = 0;
                KillNapalmBolt(bolt);
                continue;
            }
            age = the_time - bolt->field_0x28;
            if ((int)(the_time - bolt->field_0x24) > 50) {
                memmove(&bolt->field_0x2c[1], &bolt->field_0x2c[0], (REC2_ASIZE(bolt->field_0x2c) - 1) * sizeof(br_vector3));
                BrVector3Sub(&bolt->field_0x2c[0], &bolt->ped->pos, &bolt->actors[0]->t.t.translate.t);
                BrVector3Normalise(&bolt->field_0x2c[0], &bolt->field_0x2c[0]);
                bolt->field_0x2c[0].v[0] *= FRandomBetween(0.8f, 1.25f);
                bolt->field_0x2c[0].v[1] *= FRandomBetween(0.8f, 1.25f);
                bolt->field_0x2c[0].v[2] *= FRandomBetween(0.8f, 1.25f);
                if (age < 500) {
                    bolt->field_0x2c[0].v[1] += (500 - age) * 0.002f;
                }
            }
            dt = gFrame_period * 0.002f;
            bolt->actors[0]->t.t.translate.t.v[0] += bolt->field_0x2c[0].v[0] * dt;
            bolt->actors[0]->t.t.translate.t.v[1] += bolt->field_0x2c[0].v[1] * dt;
            bolt->actors[0]->t.t.translate.t.v[2] += bolt->field_0x2c[0].v[2] * dt;
            character = bolt->ped->character;
            object = character->personality->form->simple_physicing[(tS8)character->field_0x5].collision_info;
            BrVector3Sub(&delta, &bolt->actors[0]->t.t.translate.t, &object->actor->t.t.translate.t);
            dist_squared = BrVector3LengthSquared(&delta);
            hit = 0;
            if (dist_squared < REC2_SQR(object->bb1.max.v[0])
                    || dist_squared < REC2_SQR(object->bb1.max.v[1])
                    || dist_squared < REC2_SQR(object->bb1.max.v[2])
                    || dist_squared < REC2_SQR(object->bb1.min.v[0])
                    || dist_squared < REC2_SQR(object->bb1.min.v[1])
                    || dist_squared < REC2_SQR(object->bb1.min.v[2])) {
                for (j = 0; j < (int)REC2_ASIZE(bolt->actors); j++) {
                    if (j != 0) {
                        BrVector3Sub(&delta, &bolt->actors[j]->t.t.translate.t, &object->actor->t.t.translate.t);
                    }
                    BrMatrix34TApplyP(&local_delta, &delta, &object->actor->t.t.mat);
                    if (local_delta.v[0] <= object->bb1.max.v[0] && local_delta.v[0] >= object->bb1.min.v[0]
                            && local_delta.v[1] <= object->bb1.max.v[1] && local_delta.v[1] >= object->bb1.min.v[1]
                            && local_delta.v[2] <= object->bb1.max.v[2] && local_delta.v[2] >= object->bb1.min.v[2]) {
                        hit = 1;
                        break;
                    }
                }
            }
            if (hit) {
                character = bolt->ped->character;
                Explode(&gNapalmed_ped_animation, GetCharacterActorPtr(character), &character->personality->bb,
                    &character->personality->form->simple_physicing[(tS8)character->field_0x5].collision_info->cmpos);
                bolt->ped->field_0x0c->field_0x98 = the_time + 15000;
                actor = GetCharacterActorPtr(bolt->ped->character);
                bolt->ped->field_0x0c->field_0xc0 = bolt->field_0x68;
                bolt->ped->field_0x0c->field_0x2e |= 0x20;
                ScarePedestrian(bolt->ped, the_time, 1, 1);
                character = bolt->ped->character;
                CreateSmokeColumn2(2, actor, NULL, character->personality->form->simple_physicing[(tS8)character->field_0x5].collision_info,
                    0, IRandomBetween(0, V11MODEL(actor->model)->groups->nvertices), 25000);
                DRS3StartSound3D(gPedestrians_outlet, eSoundId_NapalmHitCrackls, &bolt->ped->pos, &gZero_v__car, 1, 255, S3_FIXED_INT(1), S3_FIXED_INT(1));
                if (bolt->ped->hit_points > 0 && bolt->field_0x68 != NULL && bolt->field_0x68->driver == eDriver_local_human) {
                    PipeSinglePedIncident(bolt->ped, bolt->field_0x68->collision_info->actor);
                }
                KillNapalmBolt(bolt);
                continue;
            }
        }
        t = the_time;
        memset(positions, 0, sizeof(positions));
        for (j = 0; j < (int)REC2_ASIZE(gFlamed_ped_flame_scales); j++) {
            actor = bolt->actors[j];
            if (gAction_replay_mode) {
                if (actor->render_style != BR_RSTYLE_FACES) {
                    break;
                }
            } else {
                t = t - 0.036;
                if (j != 0) {
                    if (t < (int)bolt->field_0x28) {
                        break;
                    }
                    k = (int)(bolt->field_0x24 - t) / 50;
                    if (k < 0) {
                        k = 0;
                    }
                    actor->t.t.translate.t.v[0] = bolt->actors[j - 1]->t.t.translate.t.v[0] - bolt->field_0x2c[k].v[0] * 0.036;
                    actor->t.t.translate.t.v[1] = bolt->actors[j - 1]->t.t.translate.t.v[1] - bolt->field_0x2c[k].v[1] * 0.036;
                    actor->t.t.translate.t.v[2] = bolt->actors[j - 1]->t.t.translate.t.v[2] - bolt->field_0x2c[k].v[2] * 0.036;
                    actor->render_style = BR_RSTYLE_FACES;
                }
            }
            memcpy(&actor->t.t.mat, &gCamera->t.t.mat, 3 * sizeof(br_vector3));
            BrMatrix34PreScale(&actor->t.t.mat,
                actor->material->colour_map->width * gFlamed_ped_flame_scales[j] / 128.0f,
                actor->material->colour_map->height * gFlamed_ped_flame_scales[j] / 128.0f,
                1.0f);
            BrMatrix34PreRotateZ(&actor->t.t.mat,
                (br_angle)(*(tU32*)&bolt->actors[j]->t.t.translate.t.v[2]
                    ^ *(tU32*)&bolt->actors[j]->t.t.translate.t.v[1]
                    ^ *(tU32*)&bolt->actors[j]->t.t.translate.t.v[0]));
            actor->render_style = BR_RSTYLE_FACES;
            BrVector3Copy(&positions[j], &actor->t.t.translate.t);
        }
        AddNapalmBoltToPipingSession(i, bolt->ped, positions);
    }
    AREndPipingSession();
}

inline int C2_HOOK_FASTCALL PedHeadingTowardsCollisionObject(tPedestrian* pPed, tPhysics_object** pObject, float* pDistance_squared, br_vector3* pPos, tU32 pTime) {
    tPhysics_object* object;
    br_vector3* ped_velocity;
    br_actor* car_actor;
    float max_t;
    float distance_squared;
    float t;
    br_vector3 offset;
    br_vector3 relative_velocity;

    ped_velocity = &pPed->character->field_0xd8;
    srand((unsigned int)pPed);
    max_t = (float)(SRandomBetween(0.8f, 1.2f) * 7.0);
    object = PHILGetFirstObject();
    *pDistance_squared = BR_SCALAR_MAX;
    if (gProgram_state.current_car.keys.horn) {
        car_actor = gProgram_state.current_car.collision_info->actor;
        if ((float)fabs(car_actor->t.t.translate.t.v[0] - pPed->pos.v[0]) < 5.0f
                && (float)fabs(car_actor->t.t.translate.t.v[1] - pPed->pos.v[1]) < 5.0f
                && (float)fabs(car_actor->t.t.translate.t.v[2] - pPed->pos.v[2]) < 5.0f) {
            *pObject = gProgram_state.current_car.collision_info;
            *pDistance_squared = 0.0f;
            BrVector3Copy(pPos, &(*pObject)->actor->t.t.translate.t);
            return 1;
        }
    }
    for (; object != NULL; object = PHILGetNextObject(object)) {
        if (!(object != NULL && object->owner != NULL && object->flags_0x238 >= 2 && object->flags_0x238 <= 4)
                && !(object != NULL && object->owner != NULL && object->flags_0x238 == 5)) {
            BrVector3Sub(&offset, &object->actor->t.t.translate.t, &pPed->pos);
            if ((float)fabs(offset.v[0]) < 5.0f && (float)fabs(offset.v[1]) < 5.0f && (float)fabs(offset.v[2]) < 5.0f) {
                if (gPeds_suicidal) {
                    distance_squared = BrVector3LengthSquared(&offset);
                    if (MungeProxRay(pPed, object, distance_squared, pTime)) {
                        return 0;
                    }
                    if (distance_squared < *pDistance_squared) {
                        *pDistance_squared = distance_squared;
                        *pObject = object;
                        BrVector3Copy(pPos, &object->actor->t.t.translate.t);
                    }
                } else {
                    relative_velocity.v[0] = object->v.v[0] - ped_velocity->v[0];
                    relative_velocity.v[2] = object->v.v[2] - ped_velocity->v[2];
                    if (relative_velocity.v[0] != 0.0f || relative_velocity.v[2] != 0.0f
                            || (object != NULL && object->owner != NULL && object->flags_0x238 == 1
                                && ((tCar_spec*)object->owner)->driver > eDriver_5
                                && ((tCar_spec*)object->owner)->proxy_ray_distance != 0.0f)) {
                        if (relative_velocity.v[0] == 0.0f && relative_velocity.v[2] == 0.0f) {
                            distance_squared = offset.v[0] * offset.v[0] + offset.v[2] * offset.v[2];
                        } else {
                            distance_squared = offset.v[0] * relative_velocity.v[2] - relative_velocity.v[0] * offset.v[2];
                            distance_squared = distance_squared * distance_squared
                                / (relative_velocity.v[0] * relative_velocity.v[0] + relative_velocity.v[2] * relative_velocity.v[2]);
                        }
                        if (MungeProxRay(pPed, object, distance_squared, pTime)) {
                            return 0;
                        }
                        if (distance_squared < 2.0f * object->radius_squared) {
                            t = -(offset.v[0] * relative_velocity.v[0] + offset.v[2] * relative_velocity.v[2])
                                / (relative_velocity.v[0] * relative_velocity.v[0] + relative_velocity.v[2] * relative_velocity.v[2]);
                            if (t >= 0.0f && t < max_t) {
                                *pObject = object;
                                *pDistance_squared = distance_squared;
                                BrVector3Copy(pPos, &object->actor->t.t.translate.t);
                                if (object != NULL && object->flags_0x238 == 0x10) {
                                    DroneLookWhereYoureGoingYouBlindCunt();
                                }
                                srand((unsigned int)pPed ^ pTime);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    srand((unsigned int)pPed ^ pTime);
    if (!gPeds_suicidal || *pDistance_squared == BR_SCALAR_MAX) {
        return 0;
    } else {
        return 1;
    }
}

inline int C2_HOOK_FASTCALL OutsideFaceXZ(const br_vector3* pPos, const tFace_ref* pFace) {
    int result;
    br_scalar dx1;
    br_scalar dz1;
    br_scalar dx2;
    br_scalar dz2;
    br_scalar px;
    br_scalar pz;

#ifdef REC2_FIX_BUGS
    result = 0;
#endif
    if (pFace != NULL) {
        dx1 = pFace->v[1].v[0] - pFace->v[0].v[0];
        dz1 = pFace->v[1].v[2] - pFace->v[0].v[2];
        dx2 = pFace->v[2].v[0] - pFace->v[0].v[0];
        dz2 = pFace->v[2].v[2] - pFace->v[0].v[2];
        px = pPos->v[0] - pFace->v[0].v[0];
        pz = pPos->v[2] - pFace->v[0].v[2];
        if ((dx1 * pz - dz1 * px) * (dx1 * dz2 - dz1 * dx2) >= 0.0f && (dx2 * pz - dz2 * px) * (dx2 * dz1 - dz2 * dx1) >= 0.0f) {
            dx1 = pFace->v[2].v[0] - pFace->v[1].v[0];
            dz1 = pFace->v[2].v[2] - pFace->v[1].v[2];
            if ((dx1 * (pPos->v[2] - pFace->v[1].v[2]) - dz1 * (pPos->v[0] - pFace->v[1].v[0])) * (dx1 * (pFace->v[0].v[2] - pFace->v[1].v[2]) - dz1 * (pFace->v[0].v[0] - pFace->v[1].v[0])) < 0.0f) {
                result = 1;
            } else {
                result = 0;
            }
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004cfab0
// FUNCTION: CARMA2_SW 0x004cc1b0
// FUNCTION: CARMA2_CHEAT 0x004d5640
br_scalar C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, const br_vector3* pDir) {
    tPed_face_cache_0x34* face_cache;
    tFace_ref* face;
    tRace_ped_exclusion_spec* exclusion_spec;
    tPed_vertex_side_cache* vertex_cache;
    br_model* model;
    br_vector3* pos;
    br_uint_16* vertices;
    fmt_vertex* model_vertices;
    br_vector3* p0;
    br_vector3* p1;
    br_vector3* p2;
    int i;
    int j;
    int group;
    int v0;
    int v1;
    int v2;
    int count_vertex_caches;
    int planes_set;
    int scared;
    int walkable;
    int cached;
    int side;
    int facing;
    br_scalar best_t;
    br_scalar t;
    br_scalar t1;
    br_scalar t2;
    br_scalar t3;
    br_scalar d;
    br_scalar dot;
    br_scalar xz_length;
    br_scalar dir_x;
    br_scalar dir_z;
    br_scalar e1x;
    br_scalar e1z;
    br_scalar e2x;
    br_scalar e2z;
    br_scalar e3x;
    br_scalar e3z;
    double half_width;
    br_vector3 dir;
    br_vector3 tv;
    tPed_proximity_plane planes[5];

    face_cache = pPed->field_0x0c->field_0xb0;
    if (face_cache == NULL || face_cache->field_0x30 == NULL) {
        return 10000.0f;
    }
    BrVector3Copy(&pPed->field_0x0c->field_0x00, &face_cache->field_0x30->normal);
    if (pDir != NULL) {
        BrVector3Normalise(&dir, pDir);
    } else {
        BrVector3Normalise(&dir, &pPed->character->field_0xc0);
    }
    BrVector3Copy(&pPed->field_0x0c->field_0x0c, &dir);
    best_t = 10000.0f;
    count_vertex_caches = 0;
    planes_set = 0;
    xz_length = (br_scalar)sqrt(dir.v[0] * dir.v[0] + dir.v[2] * dir.v[2]);
    if (pPed->action == ePed_action_running) {
        scared = 1;
    } else {
        scared = 0;
    }
    pPed->field_0x0c->field_0x18 &= ~0xb;
    dir_x = dir.v[0] / xz_length;
    dir_z = dir.v[2] / xz_length;
    pos = GetPedPos(pPed);

#define VERTEX_SIDE(RESULT, VERTEX, INDEX, PLANE)                                                              \
    do {                                                                                                       \
        if (vertex_cache != NULL && (VERTEX) < (int)REC2_ASIZE(vertex_cache->vertex_sides)) {                   \
            cached = 1;                                                                                        \
        } else {                                                                                               \
            cached = 0;                                                                                        \
        }                                                                                                      \
        if (cached && (vertex_cache->vertex_sides[VERTEX] & gPed_plane_tested_bits[PLANE])) {                  \
            RESULT = vertex_cache->vertex_sides[VERTEX] & gPed_plane_side_bits[PLANE];                         \
        } else {                                                                                               \
            RESULT = NormalSideOfPlane(&face->v[INDEX], &planes[PLANE].normal, planes[PLANE].d);               \
            if (cached) {                                                                                      \
                vertex_cache->vertex_sides[VERTEX] |= gPed_plane_tested_bits[PLANE];                           \
                if (RESULT) {                                                                                  \
                    vertex_cache->vertex_sides[VERTEX] |= gPed_plane_side_bits[PLANE];                         \
                }                                                                                              \
            }                                                                                                  \
        }                                                                                                      \
    } while (0)
#define FACE_SIDE(RESULT, PLANE)                   \
    do {                                           \
        VERTEX_SIDE(RESULT, v0, 0, PLANE);         \
        if (!(RESULT)) {                           \
            VERTEX_SIDE(RESULT, v1, 1, PLANE);     \
            if (!(RESULT)) {                       \
                VERTEX_SIDE(RESULT, v2, 2, PLANE); \
            }                                      \
        }                                          \
    } while (0)

    for (i = 0, face = face_cache->field_0x0; i < face_cache->field_0x4; i++, face++) {
        walkable = 0;
        if (face->normal.v[1] > gPed_cos_max_slope) {
            if (gPeds_suicidal) {
                continue;
            }
            exclusion_spec = pPed->exclusion_spec;
            if (exclusion_spec == NULL) {
                continue;
            }
            if (scared && exclusion_spec->exclusions_ok_when_scared) {
                continue;
            }
            for (j = 0; j < exclusion_spec->count_exclusion_materials; j++) {
                if (exclusion_spec->exclusion_materials[j].material == face->material
                        && (!scared || exclusion_spec->exclusion_materials[j].flags == 0)) {
                    walkable = 1;
                    break;
                }
            }
            if (j == exclusion_spec->count_exclusion_materials) {
                continue;
            }
        } else {
            exclusion_spec = pPed->exclusion_spec;
            if (exclusion_spec != NULL) {
                for (j = 0; j < exclusion_spec->count_exception_materials; j++) {
                    if (exclusion_spec->exception_materials[j] == face->material) {
                        break;
                    }
                }
                if (j != exclusion_spec->count_exception_materials) {
                    continue;
                }
            }
        }
        if (!walkable) {
            dot = BrVector3Dot(&dir, &face->normal);
            if (fabs(dot) < 0.001) {
                t = 20.0f;
            } else {
                t = (face->d - pos->v[0] * face->normal.v[0] - pos->v[1] * face->normal.v[1] - pos->v[2] * face->normal.v[2]) / dot;
            }
            if (t < 0.0f || t > 20.0f || t > best_t) {
                continue;
            }
        }
        if (!planes_set) {
            BrVector3Copy(&planes[0].normal, &face_cache->field_0x30->normal);
            planes[0].d = face_cache->field_0x30->d;
            DRVector3SafeCross(&planes[1].normal, &y_unit_vector, &dir);
            BrVector3Normalise(&planes[1].normal, &planes[1].normal);
            BrVector3Negate(&planes[2].normal, &planes[1].normal);
            half_width = pPed->character->personality->bb.max.v[0] * 1.3;
            BrVector3Scale(&tv, &planes[2].normal, half_width);
            BrVector3Accumulate(&tv, pos);
            planes[1].d = BrVector3Dot(&tv, &planes[1].normal);
            half_width = pPed->character->personality->bb.max.v[0] * 1.3;
            BrVector3Scale(&tv, &planes[1].normal, half_width);
            BrVector3Accumulate(&tv, pos);
            planes[2].d = BrVector3Dot(&tv, &planes[2].normal);
            BrVector3Copy(&planes[3].normal, &dir);
            planes[3].d = BrVector3Dot(pos, &planes[3].normal);
            planes_set = 1;
            BrVector3Negate(&planes[4].normal, &face_cache->field_0x30->normal);
            planes[4].d = BrVector3Dot(pos, &planes[4].normal);
        }
        model = face->a->model;
        group = face->g;
        if (group < 0) {
            if (face->f >= model->nfaces) {
                continue;
            }
            vertices = model->faces[face->f].vertices;
        } else {
            if (group >= model->prepared->ngroups) {
                continue;
            }
            if (face->f >= model->prepared->groups[group].nfaces) {
                continue;
            }
            vertices = model->prepared->groups[group].faces[face->f].vertices;
        }
        v0 = vertices[0];
        v1 = vertices[1];
        v2 = vertices[2];
        vertex_cache = NULL;
        for (j = 0; j < count_vertex_caches; j++) {
            if (gPed_vertex_side_caches[j].model == model && gPed_vertex_side_caches[j].group == group) {
                vertex_cache = &gPed_vertex_side_caches[j];
                break;
            }
        }
        if (vertex_cache == NULL && count_vertex_caches < (int)REC2_ASIZE(gPed_vertex_side_caches)) {
            vertex_cache = &gPed_vertex_side_caches[count_vertex_caches];
            count_vertex_caches++;
            memset(vertex_cache->vertex_sides, 0, sizeof(vertex_cache->vertex_sides));
            vertex_cache->model = model;
            vertex_cache->group = group;
        }
        FACE_SIDE(side, 3);
        if (!side) {
            continue;
        }
        FACE_SIDE(side, 1);
        if (!side) {
            continue;
        }
        FACE_SIDE(side, 2);
        if (!side) {
            continue;
        }
        if (walkable) {
            /* FIXME: the original takes the vertices from the prepared group, also when the face has no group */
            model_vertices = model->prepared->groups[group].vertices;
            p0 = &model_vertices[v0].p;
            p1 = &model_vertices[v1].p;
            p2 = &model_vertices[v2].p;
            e1x = p1->v[0] - p0->v[0];
            e1z = p1->v[2] - p0->v[2];
            e2x = p2->v[0] - p1->v[0];
            e2z = p2->v[2] - p1->v[2];
            e3x = p0->v[0] - p2->v[0];
            e3z = p0->v[2] - p2->v[2];
            /* FIXME: the original uses the first edge's end points for all three edges */
            d = dir_x * e1z - dir_z * e1x;
            if (fabs(d) > 0.0001) {
                t1 = (e1x * pos->v[2] - e1z * pos->v[0] + p0->v[0] * p1->v[2] - p1->v[0] * p0->v[2]) / d;
                if (t1 < 0.0f) {
                    t1 = BR_SCALAR_MAX;
                }
            } else {
                t1 = BR_SCALAR_MAX;
            }
            d = dir_x * e2z - dir_z * e2x;
            if (fabs(d) > 0.0001) {
                t2 = (e2x * pos->v[2] - e2z * pos->v[0] + p0->v[0] * p1->v[2] - p1->v[0] * p0->v[2]) / d;
                if (t2 < 0.0f) {
                    t2 = BR_SCALAR_MAX;
                }
            } else {
                t2 = BR_SCALAR_MAX;
            }
            d = dir_x * e3z - dir_z * e3x;
            if (fabs(d) > 0.0001) {
                t3 = (e3x * pos->v[2] - e3z * pos->v[0] + p0->v[0] * p1->v[2] - p1->v[0] * p0->v[2]) / d;
                if (t3 < 0.0f) {
                    t3 = BR_SCALAR_MAX;
                }
            } else {
                t3 = BR_SCALAR_MAX;
            }
            if (t1 < t2 && t1 < t3) {
                if (t1 >= best_t) {
                    continue;
                }
                best_t = t1;
                pPed->field_0x0c->field_0xa4.v[0] = e1x;
                pPed->field_0x0c->field_0xa4.v[1] = 0.0f;
                pPed->field_0x0c->field_0xa4.v[2] = -e1z;
            } else if (t2 < t1 && t2 < t3) {
                if (t2 >= best_t) {
                    continue;
                }
                best_t = t2;
                pPed->field_0x0c->field_0xa4.v[0] = e2x;
                pPed->field_0x0c->field_0xa4.v[1] = 0.0f;
                pPed->field_0x0c->field_0xa4.v[2] = -e2z;
            } else {
                if (t3 >= best_t) {
                    continue;
                }
                best_t = t3;
                pPed->field_0x0c->field_0xa4.v[0] = e3x;
                pPed->field_0x0c->field_0xa4.v[1] = 0.0f;
                pPed->field_0x0c->field_0xa4.v[2] = -e3z;
            }
            if (best_t <= 20.0f) {
                pPed->field_0x0c->field_0xbc = best_t * best_t;
                BrVector3Set(&pPed->field_0x0c->field_0x30, 0.0f, 0.0f, 0.0f);
                pPed->field_0x0c->field_0x3c.v[0] = dir_x * best_t + pos->v[0];
                pPed->field_0x0c->field_0x3c.v[1] = pos->v[1];
                pPed->field_0x0c->field_0x3c.v[2] = dir_z * best_t + pos->v[2];
                BrVector3Normalise(&pPed->field_0x0c->field_0xa4, &pPed->field_0x0c->field_0xa4);
                pPed->field_0x0c->field_0x18 |= 0x8;
            }
        } else {
            if (BrVector3Dot(&face->normal, &dir) > 0.0f) {
                facing = 1;
            } else {
                facing = 0;
            }
            if (facing && (face->material->flags & BR_MATF_TWO_SIDED)) {
                BrVector3Scale(&face->normal, &face->normal, -1.0f);
                face->d = -face->d;
                facing = 0;
            }
            if (facing) {
                FACE_SIDE(side, 4);
                if (side) {
                    pPed->field_0x0c->field_0x18 |= 0x1;
                }
            } else {
                FACE_SIDE(side, 0);
                if (side) {
                    pPed->field_0x0c->field_0x18 |= 0x2;
                }
            }
            if (pPed->field_0x0c->field_0x18 != 0) {
                pPed->field_0x0c->field_0xbc = t * t;
                BrVector3Set(&pPed->field_0x0c->field_0x30, 0.0f, 0.0f, 0.0f);
                pPed->field_0x0c->field_0x3c.v[0] = dir.v[0] * t + pos->v[0];
                pPed->field_0x0c->field_0x3c.v[1] = dir.v[1] * t + pos->v[1];
                pPed->field_0x0c->field_0x3c.v[2] = dir.v[2] * t + pos->v[2];
                best_t = t;
                BrVector3Copy(&pPed->field_0x0c->field_0xa4, &face->normal);
            }
        }
    }
#undef FACE_SIDE
#undef VERTEX_SIDE
    return best_t;
}

// FUNCTION: CARMA2_HW 0x004d1d70
// FUNCTION: CARMA2_SW 0x004ce470
// FUNCTION: CARMA2_CHEAT 0x004d7910
tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pOn_face, int* pFace_changed, br_vector3* pPos, int pForce, tU32 pTime) {
    br_vector3* pos;
    tPed_face_cache_0x34* face_cache;
    tFace_ref* old_face;

    *pOn_face = 0;
    *pFace_changed = 0;
    pos = GetPedPos(pPed);
    face_cache = pPed->field_0x0c->field_0xb0;
    if (!pForce && face_cache != NULL
            && pos->v[0] - face_cache->field_0x18.max.v[0] <= -.21739130434782608
            && pos->v[1] - face_cache->field_0x18.max.v[1] <= -.21739130434782608
            && pos->v[2] - face_cache->field_0x18.max.v[2] <= -.21739130434782608
            && face_cache->field_0x18.min.v[0] - pos->v[0] <= -.21739130434782608
            && face_cache->field_0x18.min.v[1] - pos->v[1] <= -.21739130434782608
            && face_cache->field_0x18.min.v[2] - pos->v[2] <= -.21739130434782608) {
        if (OutsideFaceXZ(pos, face_cache->field_0x30)) {
            old_face = face_cache->field_0x30;
            SetPedCurrentFace(pPed, pPos);
            *pOn_face = 1;
            if (old_face == NULL || face_cache->field_0x30 == NULL || BrVector3Dot(&old_face->normal, &pPed->field_0x0c->field_0x00) < .99) {
                *pFace_changed = 1;
            }
        }
        return face_cache;
    }
    RecachePedestrian(pPed, pPos, pTime);
    face_cache = pPed->field_0x0c->field_0xb0;
    if (face_cache != NULL) {
        *pOn_face = 1;
    } else {
        *pOn_face = 0;
    }
    *pFace_changed = 1;
    return face_cache;
}

// FUNCTION: CARMA2_HW 0x004cef80
// FUNCTION: CARMA2_SW 0x004cb680
// FUNCTION: CARMA2_CHEAT 0x004d4b10
void C2_HOOK_FASTCALL PedScanForObjects(tPedestrian* pPed, tU32 pTime) {
    tPhysics_object* object;
    float distance_squared;
    br_vector3 pos;
    br_vector3 dir;
    br_vector3 offset;
    br_matrix34 mat;
    int move;
    tPed_action action;

    if (pPed->field_0x0c == NULL) {
        return;
    }
    pPed->field_0x0c->field_0x18 &= ~0x4;
    if (PedHeadingTowardsCollisionObject(pPed, &object, &distance_squared, &pos, pTime)
            && ((!gBlind_pedestrians && !gPed_valium_left) || gProgram_state.current_car.keys.horn)
            && (!(pPed->field_0x0c->field_0x18 & 0x2) || distance_squared / pPed->field_0x0c->field_0xbc < 0.25)) {
        pPed->field_0x0c->field_0x18 |= 0x4;
        if (gPeds_suicidal) {
            BrVector3Sub(&dir, &pos, &pPed->pos);
            SetPedXZDirection(pPed, &dir, 0.0f, pTime);
            RescanPedProximity(pPed, NULL);
            return;
        }
        BrVector3Copy(&pPed->field_0x0c->field_0x30, &object->v);
        BrVector3Copy(&pPed->field_0x0c->field_0x3c, &pos);
        BrVector3Sub(&offset, &pPed->pos, &pos);
        BrVector3Normalise(&pos, &offset);
        BrMatrix34Rotate(&mat, IRandomPosNeg(0x2aa8), &y_unit_vector);
        BrVector3Copy(&pPed->field_0x0c->field_0xa4, &pos);
        if (BrVector3LengthSquared(&object->v) > 1.0
                || (gProgram_state.current_car.keys.horn && object == gProgram_state.current_car.collision_info)) {
            if (pPed->action == ePed_action_walking) {
                BrVector3Scale(&dir, &pPed->field_0x0c->field_0xa4, -1.0f);
                SetPedXZDirection(pPed, &dir, 0.0f, pTime);
                RescanPedProximity(pPed, NULL);
                ScarePedestrian(pPed, pTime, 1, 0);
                pPed->field_0x2c = pTime;
            } else {
                if (pTime - pPed->field_0x2c > 9999 && pPed->field_0x0c->field_0x98 <= 0) {
                    SetPedHeadAnim(pPed, &gKnocked_over_ped_anim_sequence, pTime);
                    /* FIXME: rows 114 and up of gPed_move_fsm are probably separate tables */
                    move = gPed_move_fsm[115][1 + IRandomBetween(0, 8)];
                    if (move < 0) {
                        move = -move;
                        action = ePed_action_running;
                    } else {
                        action = ePed_action_panicking;
                    }
                    if (move != 0xd && move != 0x28 && move != 0x29) {
                        BrVector3Scale(&dir, &pPed->field_0x0c->field_0xa4, -1.0f);
                        SetPedXZDirection(pPed, &dir, 0.0f, pTime);
                        RescanPedProximity(pPed, NULL);
                    }
                    SetPedMove(pPed, move, -1, 0, 1, pTime, action);
                    switch (move) {
                    case 2:
                        MakePedNoise(pPed, 6, 0, NULL);
                        break;
                    case 10:
                    case 11:
                        MakePedNoise(pPed, 8, 0, NULL);
                        break;
                    case 13:
                        MakePedNoise(pPed, 10, 1, NULL);
                        break;
                    }
                }
                ScarePedestrian(pPed, pTime, 1, 0);
            }
        }
        return;
    }
    if (pPed->hit_points > 0
            && pPed->action == ePed_action_running
            && pPed->field_0x0c->field_0x18 == 0
            && pTime - pPed->field_0x28 > 9999
            && pPed->character->field_0xc == 0
            && pPed->field_0x0c->field_0x98 <= 0) {
        SetPedMove(pPed, 0x1e, IRandomBetween(pPed->movement_spec->min_walk_speed_factor, pPed->movement_spec->max_walk_speed_factor), 0, 1, pTime, ePed_action_walking);
        SetPedHeadAnim(pPed, &gCalm_ped_anim_sequence, pTime);
    }
}

inline void C2_HOOK_FASTCALL CatAndMouseBonus(tPedestrian* pPed) {

    if (pPed->field_0x0c != NULL && pPed->field_0x0c->field_0xc0 == &gProgram_state.current_car && !(pPed->field_0x0c->field_0x2e & 0x4)) {
        pPed->field_0x0c->field_0x2e |= 0x4;
        EarnCredits(gPed_points[gProgram_state.skill_level]);
        DoFancyHeadup(4);
    }
}

inline void C2_HOOK_FASTCALL StillifyCorpse(tPedestrian* pPed, tU32 pTime, int pArg3) {

    if (gMutant_speed == 0.0f) {
        SetCharacterMoveAR(pPed->character, -1, 0.0f, pArg3, 0, pTime);
    } else {
        SetCharacterMoveAR(pPed->character, -1, -gMutant_speed, pArg3, 0, pTime);
    }
}

inline void C2_HOOK_FASTCALL StraightenOutPed(tPedestrian* pPed, tU32 pTime) {

    EnableRetainRootMode();
    switch (pPed->character->personality->form->moves[pPed->character->field_0x7].id) {
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x61:
    case 0x71:
        SetKnockedOverMove(pPed, pTime);
        break;
    }
    DisableRetainRootMode();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004d1030
// FUNCTION: CARMA2_SW 0x004cd730
// FUNCTION: CARMA2_CHEAT 0x004d6bd0
int C2_HOOK_FASTCALL ActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject, tFace_ref* pFace) {
    tPedestrian* ped;
    tU32 time;
    int i;
    int flat;
    int upright;
    int max_tumbles;
    int axis_index;
    int on_face;
    int face_changed;
    br_scalar max_up;
    br_scalar dot;
    br_vector3 normal;
    br_vector3 axis;
    br_vector3 tumble_v;
    br_vector3 pos;

    if (gAction_replay_mode) {
        return 1;
    }
    if (gPipe_halted_ped_status) {
        PipeSinglePedStatus(pCharacter->ped, -1, -1, 5, 1, pCharacter->field_0x4, pCharacter->field_0x4, pCharacter->field_0x5, pCharacter->field_0x5,
            &pCharacter->field_0xc0, &pCharacter->field_0xcc, &pCharacter->field_0x8c);
        return 1;
    }
    ped = pCharacter->ped;
    BrVector3Copy(&ped->pos, &pObject->actor->t.t.translate.t);
    time = GetTotalTime();
    if ((pCharacter->field_0x14 & 0x1) || pFace != NULL) {
        if (pFace != NULL) {
            BrVector3Copy(&normal, &pFace->normal);
            max_up = pFace->normal.v[1];
        } else {
#ifdef REC2_FIX_BUGS
            BrVector3Set(&normal, 0.0f, 0.0f, 0.0f);
#endif
            max_up = 0.0f;
            for (i = pObject->box_face_start; i < pObject->box_face_end; i++) {
                if (max_up < gFace_list__car[i].normal.v[1]) {
                    BrVector3Copy(&normal, &gFace_list__car[i].normal);
                    max_up = gFace_list__car[i].normal.v[1];
                }
            }
        }
        if (pCharacter->field_0x8c.m[1][1] <= 0.93 || ped->hit_points <= 0) {
            upright = 0;
            if (pObject->I.v[0] > pObject->I.v[1] && pObject->I.v[0] > pObject->I.v[2]) {
                BrVector3Scale(&axis, (br_vector3*)pObject->actor->t.t.mat.m[0], -1.0f);
            } else if (pObject->I.v[1] > pObject->I.v[2] && pObject->I.v[1] > pObject->I.v[0]) {
                BrVector3Scale(&axis, (br_vector3*)pObject->actor->t.t.mat.m[1], -1.0f);
            } else {
                BrVector3Scale(&axis, (br_vector3*)pObject->actor->t.t.mat.m[2], -1.0f);
            }
            dot = BrVector3Dot(&normal, &axis);
            if (max_up != 0.0f && (float)fabs(dot) < 0.706) {
                flat = 1;
            } else {
                flat = 0;
            }
        } else {
            flat = 0;
            upright = 1;
        }
        if (flat && ped->field_0x0c != NULL) {
            if (ped->hit_points > 0) {
                max_tumbles = 3;
            } else {
                max_tumbles = 2;
            }
            if (ped->field_0x0c->field_0x19 < max_tumbles) {
                StraightenOutPed(ped, time);
                if (!(pCharacter->field_0x14 & 0x1)) {
                    SetCharacterPhysicsLevelAR(pCharacter, 5);
                }
                if (PercentageChance(50)) {
                    gPed_tumble_spin = -gPed_tumble_spin;
                }
                MakePedNoise(ped, 3, 0, NULL);
                BrVector3Scale(&tumble_v, &axis, gPed_tumble_spin);
                TumbleObjectWithV(pObject, &tumble_v, 1.39f, 0.0f);
                if (ped->field_0x0c != NULL) {
                    ped->field_0x0c->field_0x19++;
                }
                pObject->disable_move_rotate = 0;
                AssertRootObjectsMatrix(pCharacter);
                return 0;
            }
        }
        PipeSinglePedStatus(pCharacter->ped, -1, -1, 5, 1, pCharacter->field_0x4, pCharacter->field_0x4, pCharacter->field_0x5, pCharacter->field_0x5,
            &pCharacter->field_0xc0, &pCharacter->field_0xcc, &pCharacter->field_0x8c);
        pCharacter->ped->field_0x06++;
        StraightenOutPed(ped, time);
        if (ped->field_0x0c != NULL) {
            ped->field_0x0c->field_0x19 = 0;
        }
        if (ped->hit_points > 0) {
            RecacheAndSetFace(ped, &on_face, &face_changed, &pos, 1, time);
            if ((flat || (upright && gPeds_suicidal)) && !gImmortal_peds) {
                MakePedNoise(ped, 5, 0, NULL);
                KillPedestrian(ped, NULL);
                SetKnockedOverMove(ped, time);
                StillifyCorpse(ped, time, 0);
            } else if (!(pCharacter->field_0xc & gPow2_array[3]) || !(pCharacter->field_0xc & gPow2_array[5])) {
                if (ped->character->field_0xc != 0) {
                    CatAndMouseBonus(ped);
                }
                ped->flags &= ~0x8;
                if (upright) {
                    SetPedXZDirection(ped, (br_vector3*)pCharacter->field_0x8c.m[2], 0.0f, time);
                    ped->action = ePed_action_walking;
                    ScarePedestrian(ped, time, 0, 0);
                } else {
                    if (dot > 0.0f) {
                        axis_index = pCharacter->personality->form->axis_when_lying_down_A;
                    } else {
                        axis_index = pCharacter->personality->form->axis_when_lying_down_B;
                    }
                    if (axis_index < 0) {
                        BrVector3Scale(&axis, (br_vector3*)pCharacter->field_0x8c.m[-axis_index], -1.0f);
                    } else {
                        BrVector3Copy(&axis, (br_vector3*)pCharacter->field_0x8c.m[axis_index]);
                    }
                    SetPedXZDirection(ped, &axis, 0.0f, time);
                    if (dot > 0.0f) {
                        SetPedMove(ped, 0x50, -1, 0, 1, time, ePed_action_getting_up);
                    } else {
                        SetPedMove(ped, 0x51, -1, 0, 1, time, ePed_action_getting_up);
                    }
                }
                RescanPedProximity(ped, NULL);
                PedScanForObjects(ped, time);
            } else {
                SetKnockedOverMove(ped, time);
                CatAndMouseBonus(ped);
            }
        } else {
            StillifyCorpse(ped, time, 0);
#define PED_ROOT_OBJECT(C) (((C)->field_0x14 & 0x1) ? (C)->personality->form->simple_physicing[(tS8)(C)->field_0x5].collision_info : GetRootObject(C))
            if (PED_ROOT_OBJECT(ped->character)->last_special_volume == NULL
                    || PED_ROOT_OBJECT(ped->character)->last_special_volume->gravity_multiplier >= 1.0f) {
                if (!gAction_replay_mode && PercentageChance(75) && !(ped->flags & 0x10) && gGoreLevel > 0) {
                    ped->flags |= 0x10;
                    DoSpillage(&gPed_slick, GetCharacterActorPtr(ped->character));
                }
            }
#undef PED_ROOT_OBJECT
        }
        AssertRootObjectsMatrix(pCharacter);
    }
    if (pObject->flags_0x238 == 5) {
        pObject->field_0x1a0 = 0x40000;
    } else {
        pObject->field_0x1a0 = 0x10000;
    }
    pObject->field_0x1a4 = 0x50000;
    ClearSplashes(pObject);
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004d1020
// FUNCTION: CARMA2_SW 0x004cd720
// FUNCTION: CARMA2_CHEAT 0x004d6bc0
int C2_HOOK_FASTCALL CBActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject) {
    return ActiveHalted(pCharacter, pObject, NULL);
}

inline void C2_HOOK_FASTCALL RecordPedSlide(tPedestrian* pPed, tFace_ref* pFace, br_vector3* pPos, br_vector3* pVelocity) {
    tPed_cache_006944c0* data;
    br_vector3 dir;
    float d;

    data = pPed->field_0x0c;
    if (data == NULL) {
        dr_dprintf("Can't smear a ped with no process data");
        return;
    }
    if (!(data->field_0xc4 & 0x2)) {
        data->field_0xc4 |= 0x2;
        data->field_0xca = 1;
        BrVector3Copy(&data->field_0xe4, pPos);
        BrVector3Copy(&data->field_0xf0, &pFace->normal);
        BrVector3Normalise(&dir, pVelocity);
        BrVector3Cross(&data->field_0x114, &pFace->normal, &dir);
        d = BrVector3Dot(&data->field_0x114, pPos);
        data->field_0x124 = d;
        data->field_0x128 = d;
    } else if (BrVector3Dot(&data->field_0xf0, &pFace->normal) > 0.997f) {
        if (fabs(BrVector3Dot(&data->field_0xe4, &data->field_0xf0) / data->field_0xca - BrVector3Dot(&data->field_0xf0, pPos)) < 0.01f) {
            d = BrVector3Dot(&data->field_0x114, pPos);
            if (data->field_0x124 < d) {
                data->field_0x124 = d;
            }
            if (d < data->field_0x128) {
                data->field_0x128 = d;
            }
            BrVector3Accumulate(&data->field_0xe4, pPos);
            data->field_0xca++;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d2320
// FUNCTION: CARMA2_SW 0x004cea20
// FUNCTION: CARMA2_CHEAT 0x004d7ee0
void C2_HOOK_FASTCALL PedProcessForces(tPhysics_object* pObject, tPhysics_object* pOther_object, tFace_ref* pFace, float pForce, br_vector3* pPos) {
    tPedestrian* ped;
    tPhysics_object* object;
    int damage;

    if (gAction_replay_mode) {
        return;
    }
    ped = ((tPed_character_instance*)pObject->owner)->ped;
    SetLastHitter(ped, pOther_object);
    if ((pOther_object != NULL && pOther_object->owner != NULL && pOther_object->flags_0x238 == 0x20 && pOther_object->M == gMass_mutant_tail_ball)
            || pObject->v.v[1] < -1.7) {
        pForce *= 10.0f;
    }
    if (gExploding_pedestrians) {
        damage = ped->character->personality->form->count_bones * gPed_severing_damage;
    } else {
        damage = pForce * gPed_damage_per_force * (pOther_object == NULL ? 1.5 : 1.0);
    }
    if (damage == 0) {
        return;
    }
    DamagePedestrian(ped, damage, -1, 0x5e, pOther_object);
    if (ped->hit_points > 0 && pOther_object != NULL && pOther_object->owner != NULL && pOther_object->flags_0x238 == 1
            && gProgram_state.current_car.collision_info == pOther_object
            && ped->field_0x0c != NULL && !(ped->field_0x0c->field_0x2e & 0x4)
            && BrVector3LengthSquared(&gProgram_state.current_car.collision_info->v) > 0.001) {
        ped->field_0x0c->field_0x21++;
        if (ped->field_0x0c->field_0x21 >= 80) {
            CatAndMouseBonus(ped);
        }
    }
    if (pForce >= 0.01) {
        if (pOther_object != NULL) {
            ped->flags |= 0x4;
        } else if (pFace != NULL) {
#ifdef REC2_FIX_BUGS
            if (fabs(pFace->normal.v[1]) < 0.707) {
#else
            if (fabs(pFace->normal.v[1] < 0.707)) {
#endif
                ped->flags |= 0x2;
            }
        }
        if ((ped->flags & 0x4) && (ped->flags & 0x2) && !(ped->flags & 0x8)) {
            SetPedMove(ped, 0x60, -1, 0, 1, GetTotalTime(), ePed_action_impacting);
            ped->flags |= 0x8;
            if (gPed_recent_points != 0 && gPTR_00694120 == ped && ped->hit_points <= 0 && GetTotalTime() - ped->field_0x30 < 750) {
                DoFancyHeadup(1);
                PratcamEvent(30);
                EarnCredits(gPed_recent_points);
                gPed_recent_points = 0;
            }
        }
    }
    if ((pObject->last_special_volume == NULL || pObject->last_special_volume->gravity_multiplier >= 1.0f) && gGoreLevel > 0) {
        object = ped->character->personality->form->simple_physicing[(tS8)ped->character->field_0x5].collision_info;
        DoGiblets(ped, pObject, pOther_object, pForce * REC2_SQR(0.068 / object->M), pPos, gGiblet_scrape_start);
        if (pOther_object == NULL) {
            if (pFace->material != NULL && pFace->material->colour_map != NULL && pFace->material->colour_map->type == BR_PMT_RGBA_4444) {
                return;
            }
            if (ped->hit_points <= 0
                    && (pFace->material == NULL || pFace->material->identifier == NULL
                        || strlen(pFace->material->identifier) != 11 || pFace->material->identifier[5] != '|')) {
                RecordPedSlide(ped, pFace, pPos, &pObject->v);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d2930
// FUNCTION: CARMA2_SW 0x004cf020
// FUNCTION: CARMA2_CHEAT 0x004d84f0
void C2_HOOK_FASTCALL CBMovedByPhysics(tPed_character_instance* pCharacter, tPhysics_object* pObject, tU32 pTime) {
    tPedestrian* ped;
    br_scalar speed;
    int frame_time;
    int move_id;

    ped = pCharacter->ped;
    BrVector3Copy(&ped->pos, (br_vector3*)pObject->actor->t.t.mat.m[3]);
    BrVector3Copy(&pCharacter->field_0xd8, &pObject->v);
    speed = BrVector3Length(&pObject->v);
    TumbleObjectWithV(pObject, &pObject->v, 1.39f, 0.1f);
    move_id = pCharacter->personality->form->moves[pCharacter->field_0x7].id;
    if (move_id == 0x5d || move_id == 0x5e || move_id == 0x61) {
        if (speed != 0.0f) {
            frame_time = 25.0 / speed * FRandomBetween(0.6f, 1.4f);
            if (frame_time < 20) {
                frame_time = 20;
            } else if (frame_time > 150) {
                frame_time = 150;
            }
            if (GET_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL && GET_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.0f) {
                frame_time = frame_time * 1.5;
            }
        } else {
            frame_time = 0;
        }
        SetCharacterMoveAR(pCharacter, -1, (float)frame_time, 0, 0, GetTotalTime());
    }
    if (GET_PED_COLLISION_OBJECT(ped)->last_special_volume != NULL && GET_PED_COLLISION_OBJECT(ped)->last_special_volume->gravity_multiplier < 1.0f && ped->hit_points > 0) {
        if (BrVector3Length(&pObject->omega) < 1.0) {
            pObject->omega.v[0] += SRandomPosNeg(5.0f);
            pObject->omega.v[1] += SRandomPosNeg(5.0f);
            pObject->omega.v[2] += SRandomPosNeg(5.0f);
        }
        if (pObject->water_depth_factor <= 1.0 && pObject->v.v[1] > -.02 && pObject->water_depth_factor < 1.0) {
            pObject->v.v[1] += FRandomBetween(0.0f, -0.3f);
        }
    }
    if (!gAction_replay_mode) {
        PipeSinglePedPhysics(((tS16)ped->field_0x06 << 16) | (ped - gPedestrian_array), GetCharacterMatrixPtr(pCharacter));
    }
}

// FUNCTION: CARMA2_HW 0x004d2c40
// FUNCTION: CARMA2_SW 0x004cf330
// FUNCTION: CARMA2_CHEAT 0x004d8800
int C2_HOOK_FASTCALL GetPedCount(void) {
    return gPed_count;
}

// FUNCTION: CARMA2_HW 0x004d2c50
// FUNCTION: CARMA2_SW 0x004cf340
// FUNCTION: CARMA2_CHEAT 0x004d8810
int C2_HOOK_FASTCALL GetPedPosition(int pIndex, br_vector3* pPos) {
    tPedestrian* ped;

    ped = &gPedestrian_array[pIndex];
    if (ped->hit_points > 0
            && ((ped->flags & 0x1) || gMap_view == 2)
            && (gCurrent_race.race_spec->race_type != kRaceType_Peds || ((tS16)ped->flags & 0x200) != 0)) {
        BrVector3Copy(pPos, &ped->pos);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004d34e0
// FUNCTION: CARMA2_SW 0x004cfbd0
// FUNCTION: CARMA2_CHEAT 0x004d90a0
void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_limbed_actors) != 30);

    if (gPed_count_limbed_actors < (int)REC2_ASIZE(gPed_limbed_actors)) {
        gPed_limbed_actors[gPed_count_limbed_actors] = pActor;
        gPed_count_limbed_actors++;
    }
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, pOn_screen, 0);
}

inline void C2_HOOK_FASTCALL SetModelCallBacks(tPed_character_instance* pCharacter) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(pCharacter->personality->bones->models); i++) {
        for (j = 0; j < (int)REC2_ASIZE(pCharacter->personality->bones->models[i].models); j++) {
            if (pCharacter->personality->bones->models[i].models[j] != NULL) {
                pCharacter->personality->bones->models[i].models[j]->flags |= BR_MODF_CUSTOM;
                pCharacter->personality->bones->models[i].models[j]->custom = TurnLimbsOnAndOff;
            }
        }
    }
}

inline void C2_HOOK_FASTCALL BuildPedestrian(tPedestrian* pPed, const char* pGroup_name, const br_vector3* pPos, br_vector3* pDir, tPed_movement_spec* pMovement_spec, tRace_ped_exclusion_spec* pExclusion_spec) {
    br_matrix34 mat;
    char s[32];
    char material_name[32];
    br_pixelmap* map;
    br_material* material;

    BrMatrix34Identity(&mat);
    BrVector3Copy((br_vector3*)mat.m[3], pPos);
    pPed->character = BuildCharacterInstance(pGroup_name, &mat);
    SetModelCallBacks(pPed->character);
    sprintf(s, "%sB", pGroup_name);
    map = BrMapFind(s);
    if (map == NULL) {
        FatalError(kFatalError_CantFindPedTexture_S, s);
    }
    strcpy(material_name, pGroup_name);
    sprintf(material_name + strlen(material_name), "%d.MAT", 1);
    material = BrMaterialFind(material_name);
    if (material != NULL) {
        material->colour_map = map;
        BrMaterialUpdate(material, BR_MATU_COLOURMAP);
    }
    pPed->hit_points = 100;
    pPed->movement_spec = pMovement_spec;
    pPed->exclusion_spec = pExclusion_spec;
    pPed->speed_factor = 1.0f;
    if (pPed->movement_spec->max_walk_speed_factor != 0) {
        SetPedMove(pPed, 30, IRandomBetween(pPed->movement_spec->min_walk_speed_factor, pPed->movement_spec->max_walk_speed_factor), 0, 0, 0, ePed_action_walking);
    } else {
        SetPedMove(pPed, 0, -1, 0, 0, 0, ePed_action_walking);
    }
    BrVector3Normalise(pDir, pDir);
    SetCharacterDirectionAR(pPed->character, pDir, &y_unit_vector);
    BrVector3Copy(&pPed->pos, pPos);
}

inline br_scalar C2_HOOK_FASTCALL AreaOfTriangle(const br_vector3* pV0, const br_vector3* pV1, const br_vector3* pV2) {

    return fabs(pV1->v[2] * pV0->v[0] - pV1->v[0] * pV0->v[2]
        + pV2->v[2] * pV1->v[0] - pV2->v[0] * pV1->v[2]
        + pV2->v[0] * pV0->v[2] - pV2->v[2] * pV0->v[0]) * 0.5f;
}

inline void C2_HOOK_FASTCALL MakeRandomPointInTriangle(br_vector3* pPos, const br_vector3* pV0, const br_vector3* pV1, const br_vector3* pV2) {
    float r0;
    float r1;
    float r2;

    r1 = SRandomBetween(0.1f, 0.9f);
    r2 = SRandomBetween(0.1f, 0.9f);
    if (r1 + r2 > 1.0f) {
        r1 = 1.0f - r1;
        r2 = 1.0f - r2;
    }
    r0 = 1.0f - r1 - r2;
    BrVector3Scale(pPos, pV1, r1);
    pPos->v[0] += pV0->v[0] * r0;
    pPos->v[1] += pV0->v[1] * r0;
    pPos->v[2] += pV0->v[2] * r0;
    pPos->v[0] += pV2->v[0] * r2;
    pPos->v[1] += pV2->v[1] * r2;
    pPos->v[2] += pV2->v[2] * r2;
}

// FUNCTION: CARMA2_HW 0x004d2cc0
// FUNCTION: CARMA2_SW 0x004cf3b0
// FUNCTION: CARMA2_CHEAT 0x004d8880
void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel) {
    tRace_ped_spec* spec;
    int i;
    int j;
    int k;
    int count;
    int is_animal;
    float area;
    float number;
    br_vector3 v0;
    br_vector3 v1;
    br_vector3 v2;
    br_vector3 pos;
    br_vector3 dir;
    char group_name[32];

    if (gNet_mode != eNet_mode_none) {
        return;
    }
    if (pFace->n.v[1] <= gPed_cos_max_slope) {
        return;
    }
    if (pFace->material == NULL || pFace->material->identifier == NULL) {
        return;
    }
    if (strlen(pFace->material->identifier) - 4 <= 7 || pFace->material->identifier[7] != '?') {
        return;
    }
    if (gPed_count >= 2000) {
        return;
    }
    spec = gRace_pedestrian_specs;
    for (i = 0; i < gCount_race_pedestrian_specs; i++, spec++) {
        if (spec->spawn_material == pFace->material) {
            BrVector3Copy(&v0, &pModel->vertices[pFace->vertices[0]].p);
            BrVector3Copy(&v1, &pModel->vertices[pFace->vertices[1]].p);
            BrVector3Copy(&v2, &pModel->vertices[pFace->vertices[2]].p);
            area = WORLD_SCALE_D * WORLD_SCALE_D * AreaOfTriangle(&v0, &v1, &v2);
            if (spec->density < 0.0f) {
                number = -spec->density;
            } else {
                number = spec->field_0x10 + area * spec->density * 0.001f;
            }
            count = floor(number);
            for (j = 0; j < count; j++) {
                if (gPed_count >= 2000) {
                    return;
                }
                strcpy(group_name, spec->group->members[IRandomBetween(0, spec->group->count - 1)].name);
                is_animal = 0;
                if (!gAnimalsOn) {
                    for (k = 0; k < gPed_animal_count; k++) {
                        if (DRStricmp(group_name, gPed_animal_names[k]) == 0) {
                            is_animal = 1;
                            break;
                        }
                    }
                }
                if (is_animal) {
                    continue;
                }
                dir.v[0] = SRandomPosNeg(1.0f);
                dir.v[1] = 0.0f;
                dir.v[2] = SRandomPosNeg(1.0f);
                MakeRandomPointInTriangle(&pos, &v0, &v1, &v2);
                BuildPedestrian(&gPedestrian_array[gPed_count], group_name, &pos, &dir, spec->movement, &spec->exclusion);
                if (gCurrent_race.race_spec->race_type == kRaceType_SmashNPed
                        && spec->group - gPed_groups == gCurrent_race.race_spec->options.smash_and_peds.ped_group_index) {
                    gPedestrian_array[gPed_count].flags |= 0x200;
                    AddSmashTarget(NULL, NULL, gPed_count);
                } else if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
                    if (gCurrent_race.race_spec->options.peds.count_ped_groups < 0) {
                        gTotal_count_smash_peds++;
                        gPedestrian_array[gPed_count].flags |= 0x200;
                    } else {
                        for (k = 0; k < gCurrent_race.race_spec->options.peds.count_ped_groups; k++) {
                            if (spec->group - gPed_groups == gCurrent_race.race_spec->options.peds.ped_groups[k]) {
                                gTotal_count_smash_peds++;
                                gPedestrian_array[gPed_count].flags |= 0x200;
                                break;
                            }
                        }
                    }
                }
                gPed_count++;
            }
            spec->field_0x10 = number - count;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d3520
// FUNCTION: CARMA2_SW 0x004cfc10
// FUNCTION: CARMA2_CHEAT 0x004d90e0
void C2_HOOK_FASTCALL FinishUpLoadingPeds(void) {
    tPedestrian* new_pedestrians;
    int i;

    dr_dprintf("%d pedestrians, %d bytes each, total = %d bytes\n", gPed_count, sizeof(tRace_pedestrian), gPed_count * sizeof(tRace_pedestrian));
    C2_HOOK_BUG_ON(sizeof(tRace_pedestrian) != 0x140);

    new_pedestrians = BrMemAllocate(gPed_count * sizeof(tPedestrian), kMem_ped_array);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPedestrian, character, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, ped, 0xe4);

    memcpy(new_pedestrians, gPedestrian_array, gPed_count * sizeof(tPedestrian));

    BrMemFree(gPedestrian_array);
    gPedestrian_array = new_pedestrians;

    for (i = 0; i < gPed_count; i++) {

        gPedestrian_array[i].character->ped = &gPedestrian_array[i];
        C2_HOOK_BUG_ON(offsetof(tRace_pedestrian, ped) != 0xe4);
    }
}

// FUNCTION: CARMA2_HW 0x004d35d0
// FUNCTION: CARMA2_SW 0x004cfcc0
// FUNCTION: CARMA2_CHEAT 0x004d9190
void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void) {

}

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

// FUNCTION: CARMA2_HW 0x004d35e0
// FUNCTION: CARMA2_SW 0x004cfcd0
// FUNCTION: CARMA2_CHEAT 0x004d91a0
void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed) {
    pPed->field_0x0c->field_0x6c = 0;
    SetCharacterPhysicsLevelAR(pPed->character, 5);
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x004d3600
// FUNCTION: CARMA2_SW 0x004cfcf0
void C2_HOOK_FASTCALL FUN_004d3600(void) {
    /* Unused */
}
#endif

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

// FUNCTION: CARMA2_HW 0x004d3610
// FUNCTION: CARMA2_SW 0x004cfd00
// FUNCTION: CARMA2_CHEAT 0x004d91d0
void C2_HOOK_FASTCALL RenderLimbs(void) {
    int i;
    int j;

    if (gLimbs_actor == NULL) {
        gLimbs_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
        gLimbs_actor->identifier = "Limbs_actor";
        BrActorAdd(gUniverse_actor, gLimbs_actor);
    }
    for (i = 0; i < gPed_count_limbed_actors; i++) {
        br_actor* a = gPed_limbed_actors[i];
        tPed_character_instance* ped = a->type_data;
        tPed_form* form = ped->personality->form;
        br_actor** bone_actors = form->actor_sets[ped->field_0x4].actors;

        for (j = 1, bone_actors++; j < form->count_bones; j++, bone_actors++) {
            if (!(gPow2_array[j] & ped->field_0xc)) {
                BrActorAdd(gLimbs_actor, *bone_actors);
            }
        }
    }
    BrZbsSceneRenderAdd(gLimbs_actor);
    while (gLimbs_actor->children != NULL) {
        BrActorRemove(gLimbs_actor->children);
    }
    gPed_count_limbed_actors = 0;
}

// MungePedestrians

// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

inline void C2_HOOK_FASTCALL ResetPedSystem(void) {
    ResetPedFaceCache();
}

inline void C2_HOOK_FASTCALL ResetNapalmBolts(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        int j;

        gNapalm_bolts[i].ped = NULL;
        for (j = 0; j < (int)REC2_ASIZE(gNapalm_bolts[i].actors); j++) {
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d5b10
// FUNCTION: CARMA2_SW 0x004d2200
// FUNCTION: CARMA2_CHEAT 0x004db6d0
void C2_HOOK_FASTCALL ARResetNapalmBolts(void) {
    int i;
    int j;

    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts); i++) {
        gNapalm_bolts[i].ped = NULL;
        for (j = 0; j < (int)REC2_ASIZE(gNapalm_bolts[i].actors); j++) {
            gNapalm_bolts[i].actors[j]->render_style = BR_RSTYLE_NONE;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d5b40
// FUNCTION: CARMA2_SW 0x004d2230
// FUNCTION: CARMA2_CHEAT 0x004db700
void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {
    int i;

    gNapalm_bolts[pIndex].ped = pPed;
    for (i = 0; i < (int)REC2_ASIZE(gNapalm_bolts[pIndex].actors); i++) {
        if (Vector3IsZero(&pPositions[i])) {
            break;
        }
        BrVector3Copy(&gNapalm_bolts[pIndex].actors[i]->t.t.translate.t, &pPositions[i]);
        gNapalm_bolts[pIndex].actors[i]->render_style = BR_RSTYLE_FACES;
    }
}

// FUNCTION: CARMA2_HW 0x004d5bd0
// FUNCTION: CARMA2_SW 0x004d22c0
// FUNCTION: CARMA2_CHEAT 0x004db790
void C2_HOOK_FASTCALL InitPedsForRace(void) {

    C2_HOOK_BUG_ON(sizeof(gPed_cache_006944c0) != 30000);

    ResetPedSystem();
    gPed_count = 0;
    gCount_changed_points = 0;
    gTwtPeds = -1;
    gTotal_count_smash_peds = 0;
    gPed_score_multiplier = 0;
    gPed_recent_points = 0;
    gPipe_halted_ped_status = 0;
    gImmortal_peds = 0;
    gPeds_suicidal = 0;
    gMutant_speed = 0.f;
    gExploding_pedestrians = 0;
    gPed_dismemberfest = 0;
    gBlind_pedestrians = 0;
    gDancing_peds = 0;
    gPanicking_peds = 0;
    gDrunk_pedestrians = 0;
    gGhost_pedestrians = 0;
    gPedestrian_speed_factor = 1.0f;
    BrVector3Set(&gPed_scale, 1.0f, 1.0f, 1.0f);
    gAverage_ped_scale = 1.0f;
    gPed_head_scale = 1.0f;
    gPed_gravity_multiplier = 1.0f;
    gBOOL_00744804 = 0;
    gCount_killed_peds = 0;
    gPed_valium_left = 0;
    ResetNapalmBolts();
    memset(gPed_cache_006944c0, 0, sizeof(gPed_cache_006944c0));
}

// RenderElectroBastardRays

// FUNCTION: CARMA2_HW 0x004d6180
// FUNCTION: CARMA2_SW 0x004d2780
// FUNCTION: CARMA2_CHEAT 0x004dbd40
void C2_HOOK_FASTCALL AdjustProxRay(int pRay_index, tU16 pCar_ID, tU16 pPed_index, tU32 pTime) {
    tCar_spec* car;

    if ((pCar_ID >> 8) == 0) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec(pCar_ID >> 8, pCar_ID & 0xff);
    }
    gProximity_rays[pRay_index].car = car;
    gProximity_rays[pRay_index].ped = &gPedestrian_array[pPed_index];
    gProximity_rays[pRay_index].start_time = pTime;
}

// ResetProxRay

// FUNCTION: CARMA2_HW 0x004d6210
// FUNCTION: CARMA2_SW 0x004d2810
// FUNCTION: CARMA2_CHEAT 0x004dbdd0
tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void) {
    if (gPed_count != 0) {
        return gPedestrian_array[gSelected_ped].character;
    } else {
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004d6240
// FUNCTION: CARMA2_SW 0x004d2840
// FUNCTION: CARMA2_CHEAT 0x004dbe00
void C2_HOOK_FASTCALL NextPedCam(void) {
    int original_ped;

    if (gPed_count != 0) {
        original_ped = gSelected_ped;
        do {
            gSelected_ped++;
            if (gSelected_ped >= gPed_count) {
                gSelected_ped = 0;
            }
        } while (!(gPedestrian_array[gSelected_ped].flags & 0x1) && original_ped != gSelected_ped);
    }
}

// FUNCTION: CARMA2_HW 0x004d6290
// FUNCTION: CARMA2_SW 0x004d2890
// FUNCTION: CARMA2_CHEAT 0x004dbe50
void C2_HOOK_FASTCALL PrevPedCam(void) {
    int selected_ped;

    if (gPed_count != 0) {
        selected_ped = gSelected_ped;
        do {
            gSelected_ped--;
            if (gSelected_ped < 0) {
                gSelected_ped = gPed_count - 1;
            }
        } while (!(gPedestrian_array[gSelected_ped].flags & 0x1) && selected_ped != gSelected_ped);
    }
}

// FUNCTION: CARMA2_HW 0x004d62e0
// FUNCTION: CARMA2_SW 0x004d28e0
// FUNCTION: CARMA2_CHEAT 0x004dbea0
void C2_HOOK_FASTCALL FindNearestPed(void) {
    int i;
    int nearest;
    tPedestrian* ped;
    float d;
    float best;

    if (gPed_count == 0) {
        return;
    }
    best = 1e7f;
    for (i = 0; i < gPed_count; i++) {
        ped = &gPedestrian_array[i];
        d = Vector3DistanceSquared(&ped->pos, &gProgram_state.current_car.pos);
        if (d < best) {
            best = d;
            nearest = i;
        }
    }
    gSelected_ped = nearest;
    ResetPedNearness();
}

// FUNCTION: CARMA2_HW 0x004d6390
// FUNCTION: CARMA2_SW 0x004d2990
// FUNCTION: CARMA2_CHEAT 0x004dbf50
void C2_HOOK_FASTCALL ResetPedNearness(void) {

    gPed_nearness = 1;
}

// FUNCTION: CARMA2_HW 0x004d63e0
// FUNCTION: CARMA2_SW 0x004d29e0
// FUNCTION: CARMA2_CHEAT 0x004dbfa0
void C2_HOOK_FASTCALL GetPedestrianDebugInfo(char* pBuffer, int pPed_index) {
    tPedestrian* ped;
    tPedestrian* candidate;
    br_scalar nearest_distance;
    br_scalar distance;
    char physics_mode[32];
    char action[32];
    int usage[4];
    int counts[4];
    char warnings[32];
    int i;
    int j;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_cache_sizes_1) != 4);
    C2_HOOK_BUG_ON(sizeof(tPed_face_cache_0x34) != 0x34);
    C2_HOOK_BUG_ON(sizeof(tPedestrian) != 0x54);

    ped = NULL;
    if (PDKeyDown(4)) {
        for (i = 0; i < (int)REC2_ASIZE(gPed_cache_sizes_1); i++) {
            counts[i] = 0;
            for (j = 0; j < gPed_cache_sizes_1[i]; j++) {
                if (gPed_face_cache_heads[i][j].field_0x14 != NULL) {
                    counts[i]++;
                }
            }
            usage[i] = counts[i] * 100 / gPed_cache_sizes_1[i];
        }
        if (gPed_cache_notify_flags & 0x1) {
            strcpy(warnings, "SATURATION!!  ");
        } else {
            strcpy(warnings, "");
        }
        if (gPed_cache_notify_flags & 0x2) {
            strcat(warnings, "OVERFLOW!!  ");
        }
        sprintf(pBuffer, "Cache states: %d%%, %d%%, %d%%, %d%%   %s", usage[0], usage[1], usage[2], usage[3], warnings);
        gPed_cache_notify_flags = 0;
    } else {
        if (pPed_index < 0) {
            nearest_distance = 10000000.f;
            for (i = 0, candidate = gPedestrian_array; i < gPed_count; i++, candidate++) {
                if (candidate->flags & 0x1) {
                    distance = Vector3DistanceSquared((br_vector3*)gCar_to_view->collision_info->actor->t.t.mat.m[3], &candidate->pos);
                    if (distance < nearest_distance) {
                        ped = candidate;
                        nearest_distance = distance;
                    }
                }
            }
        } else {
            ped = &gPedestrian_array[pPed_index];
        }
        if (ped != NULL) {
            switch (ped->action) {
            case ePed_action_walking:
                strcpy(action, "walking");
                break;
            case ePed_action_running:
                strcpy(action, "running");
                break;
            case ePed_action_panicking:
                strcpy(action, "panicking");
                break;
            case ePed_action_falling:
                strcpy(action, "falling");
                break;
            case ePed_action_getting_up:
                strcpy(action, "getting up");
                break;
            case ePed_action_impacting:
                strcpy(action, "impacting");
                break;
            case ePed_action_dead:
                strcpy(action, "dead");
                break;
            }
            if (ped->character->field_0x4 < 0) {
                strcpy(physics_mode, "INACTIVE");
            } else {
                switch (ped->character->field_0x14) {
                case 0:
                    strcpy(physics_mode, "PHYSICS OFF");
                    break;
                case 1:
                    strcpy(physics_mode, "PASSIVE SIMPLE");
                    break;
                case 2:
                    strcpy(physics_mode, "PASSIVE BONED");
                    break;
                case 5:
                    strcpy(physics_mode, "ACTIVE SIMPLE");
                    break;
                case 6:
                    strcpy(physics_mode, "ACTIVE BONED");
                    break;
                }
            }
            sprintf(pBuffer, "Ped #%d: %s, %s, %s", (int)(ped - gPedestrian_array), ped->character->personality->name, action, physics_mode);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d6820
// FUNCTION: CARMA2_SW 0x004d2e20
// FUNCTION: CARMA2_CHEAT 0x004dc3e0
void C2_HOOK_FASTCALL ShowActivePedCount(void) {
    char text[256];

    sprintf(text, "%d out of %d peds are active", gCount_active_peds, gPed_count);
    NewTextHeadupSlot2(4, 0, 1000, -4, text, 0);
}

// FUNCTION: CARMA2_HW 0x004d6880
// FUNCTION: CARMA2_SW 0x004d2e80
// FUNCTION: CARMA2_CHEAT 0x004dc440
void C2_HOOK_FASTCALL PedPreCollisionStuff(void) {
    tPhysics_object* object;
    br_vector3 torque;

    for (object = PHILGetFirstObject(); object != NULL; object = PHILGetNextObject(object)) {
        if (object->owner != NULL && object->flags_0x238 == 5) {
            // FIXME: BrVector3Scale(&torque, &object->omega, 0.04 * 0.0001 * BrVector3Length(&object->omega));
            torque.v[0] = (br_scalar)(object->omega.v[0] * (0.04 * 0.0001 * BrVector3Length(&object->omega)));
            torque.v[1] = (br_scalar)(object->omega.v[1] * (0.04 * 0.0001 * BrVector3Length(&object->omega)));
            torque.v[2] = object->omega.v[2] * (br_scalar)(0.04 * 0.0001 * BrVector3Length(&object->omega));
            ApplyDiminishingTorque(object, &torque);
        }
    }
}

inline void C2_HOOK_FASTCALL UpdateIfBackwardsAR(void) {
    int i;
    tPedestrian* ped;

    if (gAction_replay_mode && ARGetReplayRate() < 0.0f) {
        ped = gPedestrian_array;
        for (i = 0; i < gPed_count; i++, ped++) {
            if (ped->flags & 0x1) {
                SetCharacterBonePositions(ped->character, 3, 0);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d69b0
// FUNCTION: CARMA2_SW 0x004d2fb0
// FUNCTION: CARMA2_CHEAT 0x004dc570
void C2_HOOK_FASTCALL ChangePedScaling(const br_vector3* pScale) {
    br_vector3 scale;

    Vector3Div(&scale, pScale, &gPed_scale);
    if (scale.v[0] != 1.0 || scale.v[1] != 1.0 || scale.v[2] != 1.0) {
        ScaleBone(-1, &scale, 1);
        BrVector3Copy(&gPed_scale, pScale);
        gAverage_ped_scale = (gPed_scale.v[0] + gPed_scale.v[1] + gPed_scale.v[2]) / 3.0f;
        UpdateIfBackwardsAR();
    }
}

// ChangeHeadScaling

// FUNCTION: CARMA2_HW 0x004d6b70
// FUNCTION: CARMA2_SW 0x004d3170
// FUNCTION: CARMA2_CHEAT 0x004dc730
void C2_HOOK_FASTCALL MakeMaterialBlendy(br_material* pMaterial) {
    BlendifyMaterial(pMaterial, 25);
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

// FUNCTION: CARMA2_HW 0x004d6be0
// FUNCTION: CARMA2_SW 0x004d31e0
// FUNCTION: CARMA2_CHEAT 0x004dc7a0
void C2_HOOK_FASTCALL UnmakeMaterialBlendy(br_material* pMaterial) {

  BlendifyMaterial(pMaterial, 100);
  BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

// FUNCTION: CARMA2_HW 0x004d6b90
// FUNCTION: CARMA2_SW 0x004d3190
// FUNCTION: CARMA2_CHEAT 0x004dc750
void C2_HOOK_FASTCALL TurnOffCollisions(tPhysics_object* pObject) {

    pObject->flags |= 0x4;
}

// FUNCTION: CARMA2_HW 0x004d6c00
// FUNCTION: CARMA2_SW 0x004d3200
// FUNCTION: CARMA2_CHEAT 0x004dc7c0
void C2_HOOK_FASTCALL TurnOnCollisions(tPhysics_object* pObject) {

    pObject->flags &= ~0x4;
}

// FUNCTION: CARMA2_HW 0x004d6ba0
// FUNCTION: CARMA2_SW 0x004d31a0
// FUNCTION: CARMA2_CHEAT 0x004dc760
void C2_HOOK_FASTCALL UpdateModels(br_model* pModel) {

    BrModelUpdate(pModel, BR_MODU_PRIMITIVE_MATERIALS);
}

// FUNCTION: CARMA2_HW 0x004d6b40
// FUNCTION: CARMA2_SW 0x004d3140
// FUNCTION: CARMA2_CHEAT 0x004dc700
void C2_HOOK_FASTCALL TurnOnGhostPeds(void) {

    MungeAllCharacterMaterials(MakeMaterialBlendy);
    MungeAllCharacterObjects(TurnOffCollisions);
    MungeAllCharacterModels(UpdateModels);
    gGhost_pedestrians = 1;
}

// FUNCTION: CARMA2_HW 0x004d6bb0
// FUNCTION: CARMA2_SW 0x004d31b0
// FUNCTION: CARMA2_CHEAT 0x004dc770
void C2_HOOK_FASTCALL TurnOffGhostPeds(void) {
    MungeAllCharacterMaterials(UnmakeMaterialBlendy);
    MungeAllCharacterObjects(TurnOnCollisions);
    MungeAllCharacterModels(UpdateModels);
    gGhost_pedestrians = 0;
}

// FUNCTION: CARMA2_HW 0x004d6c10
// FUNCTION: CARMA2_SW 0x004d3210
// FUNCTION: CARMA2_CHEAT 0x004dc7d0
void C2_HOOK_FASTCALL MutatePed(tPed_character_instance* pCharacter, void* pUnused, tU8* pAngle_x, tU8* pAngle_y, tU8* pAngle_z) {

    if (pCharacter->ped->hit_points <= 0
        && pCharacter->ped->action == ePed_action_dead
        && !gPeds_already_munged) {
        *pAngle_x = (tU8)(*pAngle_x + IRandomPosNeg(30));
        *pAngle_y = (tU8)(*pAngle_y + IRandomPosNeg(30));
        *pAngle_z = (tU8)(*pAngle_z + IRandomPosNeg(30));
    }
}

// FUNCTION: CARMA2_HW 0x004d6c70
// FUNCTION: CARMA2_SW 0x004d3270
// FUNCTION: CARMA2_CHEAT 0x004dc830
void C2_HOOK_FASTCALL FlushAllPedCaches(void) {
    int i;
    tPedestrian* ped;

    for (i = 0, ped = gPedestrian_array; i < gPed_count; i++, ped++) {
        if (ped->field_0x0c != NULL && ped->field_0x0c->field_0xb0 != NULL) {
            ped->field_0x0c->field_0xb0->field_0x14 = NULL;
            ped->field_0x0c->field_0xb0 = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004d6d90
// FUNCTION: CARMA2_SW 0x004d3390
// FUNCTION: CARMA2_CHEAT 0x004dc950
void C2_HOOK_FASTCALL SavePedStuff(void) {

}

// FUNCTION: CARMA2_HW 0x004d6da0
// FUNCTION: CARMA2_SW 0x004d33a0
// FUNCTION: CARMA2_CHEAT 0x004dc960
void C2_HOOK_FASTCALL ComparePedStuff(void) {
}

// FUNCTION: CARMA2_HW 0x004d6db0
// FUNCTION: CARMA2_SW 0x004d33b0
// FUNCTION: CARMA2_CHEAT 0x004dc970
void C2_HOOK_FASTCALL AdjustPedDiagnostics(tU32 pSubject_index, void* pUnused2, void* pUnused3, void* pUnused4) {
    (void)pSubject_index;
    (void)pUnused2;
    (void)pUnused3;
    (void)pUnused4;
}

// FUNCTION: CARMA2_HW 0x004d6dc0
// FUNCTION: CARMA2_SW 0x004d33c0
// FUNCTION: CARMA2_CHEAT 0x004dc980
int C2_HOOK_FASTCALL CalmDownAllPeds(void) {
    tU32 the_time;
    int i;
    tPedestrian* ped;

    the_time = GetTotalTime();
    ped = gPedestrian_array;
    for (i = 0; i < gPed_count; i++, ped++) {
        if (ped->field_0x0c != NULL && ped->hit_points > 0 && ped->action == ePed_action_running) {
            SetPedMove(ped, 30, IRandomBetween(ped->movement_spec->min_walk_speed_factor, ped->movement_spec->max_walk_speed_factor), 0, 1, the_time, ePed_action_walking);
            SetPedHeadAnim(ped, &gCalm_ped_anim_sequence, the_time);
        }
    }
    gPed_valium_left = 5000;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004d6f10
// FUNCTION: CARMA2_SW 0x004d3510
// FUNCTION: CARMA2_CHEAT 0x004dcad0
void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void) {
    gPedsFolder = "PEDS";
    SetDefaultSoundFolderName();
    SetDefaultPowerupFilename();
    SetDefaultTextFileName();
    SetDefaultPixelmapFolderName();
}

// FUNCTION: CARMA2_HW 0x004d6f50
// FUNCTION: CARMA2_SW 0x004d3550
// FUNCTION: CARMA2_CHEAT 0x004dcb10
void C2_HOOK_FASTCALL SetZombiePedFolderNames(void) {
    gPedsFolder = "ZOMBIES";
    ConfigureZombiePedSoundPath();
    ConfigureZombiePedPowerupTxtPath();
    ConfigureZombiePedTextTxtPath();
    ConfigureZombiePedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f30
// FUNCTION: CARMA2_SW 0x004d3530
// FUNCTION: CARMA2_CHEAT 0x004dcaf0
void C2_HOOK_FASTCALL SetBloodPedFolderNames(void) {
    gPedsFolder = "PEDS";
    ConfigureBloodPedSoundPath();
    ConfigureBloodPedPowerupTxtPath();
    ConfigureBloodPedTextTxtPath();
    ConfigureBloodPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f70
// FUNCTION: CARMA2_SW 0x004d3570
// FUNCTION: CARMA2_CHEAT 0x004dcb30
void C2_HOOK_FASTCALL SetAlienPedFolderNames(void) {
    gPedsFolder = "ALIENS";
    ConfigureAlienPedSoundPath();
    ConfigureAlienPedPowerupTxtPath();
    ConfigureAlienPedTextTxtPath();
    ConfigureAlienPedTexturePath();
}

// FUNCTION: CARMA2_HW 0x004d6f90
// FUNCTION: CARMA2_SW 0x004d3590
// FUNCTION: CARMA2_CHEAT 0x004dcb50
void C2_HOOK_FASTCALL AddChangedPoint(br_vector3* pPoint) {
    C2_HOOK_ASSERT(gChanged_points < (int)REC2_ASIZE(gCount_changed_points));
    BrVector3Copy(&gChanged_points[gCount_changed_points], pPoint);
    gCount_changed_points++;
}

// FUNCTION: CARMA2_HW 0x004d6fc0
// FUNCTION: CARMA2_SW 0x004d35c0
// FUNCTION: CARMA2_CHEAT 0x004dcb80
int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void) {

    return 2 - gGoreLevel;
}

// FUNCTION: CARMA2_HW 0x004d6fd0
// FUNCTION: CARMA2_SW 0x004d35d0
// FUNCTION: CARMA2_CHEAT 0x004dcb90
int C2_HOOK_FASTCALL AreWeGoingToOutrageAnimalLoversAroundTheGlobe(void) {

    return gAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d6fe0
// FUNCTION: CARMA2_SW 0x004d35e0
// FUNCTION: CARMA2_CHEAT 0x004dcba0
int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void) {

    return gFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d6ff0
// FUNCTION: CARMA2_SW 0x004d35f0
// FUNCTION: CARMA2_CHEAT 0x004dcbb0
int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void) {

    return gExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004d7000
// FUNCTION: CARMA2_SW 0x004d3600
// FUNCTION: CARMA2_CHEAT 0x004dcbc0
void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn) {

    gAnimalsOn = pNewAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d7010
// FUNCTION: CARMA2_SW 0x004d3610
// FUNCTION: CARMA2_CHEAT 0x004dcbd0
void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn) {

    gFlameThrowerOn = pNewFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d7020
// FUNCTION: CARMA2_SW 0x004d3620
// FUNCTION: CARMA2_CHEAT 0x004dcbe0
void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on) {

    gExplosives_on = pExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004d7030
// FUNCTION: CARMA2_SW 0x004d3630
// FUNCTION: CARMA2_CHEAT 0x004dcbf0
void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel) {

    gGoreLevel = 2 - pNewLevel;
}

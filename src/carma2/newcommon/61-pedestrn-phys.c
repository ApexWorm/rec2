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
#include "21-mainloop.h"
#include "22-replay.h"
#include "23-piping.h"
#include "27-powerup.h"
#include "28-world3.h"
#include "29-raycast.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "41-utility.h"
#include "42-input.h"
#include "43-pratcam.h"
#include "49-physics.h"
#include "51-smash.h"
#include "52-errors.h"
#include "54-runtime.h"
#include "60-phil.h"
#include "61-pedestrn.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrkm.h"
#include "platform.h"

#include "rec2_macros.h"

#include <stdarg.h>
#include <string.h>

#define GET_CHARACTER_COLLISION_OBJECT(C) (((C)->field_0x14 & 0x1) ? (C)->personality->form->simple_physicing[(tS8)(C)->field_0x5].collision_info : GetRootObject(C))

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

// GLOBAL: CARMA2_HW 0x0065e4f4
// GLOBAL: CARMA2_SW 0x00658e2c
// GLOBAL: CARMA2_CHEAT 0x00665bac
const br_vector3 gPed_bone_look_vecs[9] = {
    { { 0.0f, 1.0f, 0.0f } },
    { { 0.0f, -1.0f, 0.0f } },
    { { 0.0f, -1.0f, 0.0f } },
    { { 0.0f, -1.0f, 0.0f } },
    { { 0.0f, -1.0f, 0.0f } },
    { { 1.0f, 0.0f, 0.0f } },
    { { 1.0f, 0.0f, 0.0f } },
    { { -1.0f, 0.0f, 0.0f } },
    { { -1.0f, 0.0f, 0.0f } },
};

// GLOBAL: CARMA2_HW 0x0065e560
// GLOBAL: CARMA2_SW 0x00658e98
// GLOBAL: CARMA2_CHEAT 0x00665c18
const char* gForm_non_humanoid_names[1] = {
    "NONHUMANOID",
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
int C2_HOOK_FASTCALL OrientationChanged(const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2, float pCos_angle) {
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

// InitFaceCaches

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

// SetPedCurrentFace

// RecachePedestrian

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
    if (gProgram_state.racing && OrientationChanged(&object->transform_matrix, &object->actor->t.t.mat, .7f)) {
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

inline void C2_HOOK_FASTCALL SetCharacterPositionAR(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3) {
    br_matrix34* mat;
    br_vector3 old_pos;

    mat = GetCharacterMatrixPtr(pCharacter);
    BrVector3Copy(&old_pos, (br_vector3*)mat->m[3]);
    SetCharacterPosition(pCharacter, pPos, pArg3);
    if (!(pCharacter->field_0x14 & 0x4)) {
        mat = GetCharacterMatrixPtr(pCharacter);
        PipeSinglePedPos(pCharacter->ped, &old_pos, (br_vector3*)mat->m[3]);
    }
}

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

// ReadSettingsFile

// InitOtherPedStuff

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

// STUB: CARMA2_HW 0x004cadc0
// STUB: CARMA2_SW 0x004c74f0
// STUB: CARMA2_CHEAT 0x004d0900
void C2_HOOK_FASTCALL InitPolyPedSystem(void) {
    NOT_IMPLEMENTED();
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

inline void C2_HOOK_FASTCALL AcceleratePed(tPedestrian* pPed, tU32 pTime) {
    tU32 acceleration;
    float speed;

    if (pPed->field_0x0c != NULL && pPed->field_0x0c->field_0x80 != 0) {
        if (!gAction_replay_mode || ARGetReplayRate() >= 0.0f) {
            pPed->field_0x0c->field_0x80 = (tU32)(pPed->field_0x0c->field_0x80 - gFrame_period * pPed->field_0x0c->field_0xb8);
            acceleration = pPed->field_0x0c->field_0x80;
            if (acceleration < 31) {
                acceleration = 30;
                pPed->field_0x0c->field_0x80 = 0;
            }
        } else {
            pPed->field_0x0c->field_0x80 = (tU32)(gFrame_period * pPed->field_0x0c->field_0xb8 + pPed->field_0x0c->field_0x80);
            acceleration = pPed->field_0x0c->field_0x80;
            if (acceleration > 99) {
                acceleration = 100;
                pPed->field_0x0c->field_0x80 = 0;
            }
        }
        if (gPedestrian_speed_factor == 0.0f) {
            speed = (float)acceleration;
        } else {
            speed = (float)acceleration / gPedestrian_speed_factor;
        }
        SetCharacterMoveAR(pPed->character, -1, speed, 0, 0, pTime);
    }
}

inline void C2_HOOK_FASTCALL MungePedHeadAnim(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* cache;

    cache = pPed->field_0x0c;
    if (cache != NULL && cache->field_0x1c != NULL && pTime > cache->field_0x78 + 125) {
        cache->field_0x1a++;
        if (cache->field_0x1a >= cache->field_0x1c->count) {
            pPed->field_0x0c->field_0x1a = 1;
        }
        cache = pPed->field_0x0c;
        if (cache->field_0x1c->poses[cache->field_0x1a] == 99) {
            cache->field_0x1c = NULL;
        } else {
            cache->field_0x78 = pTime;
            SetCharacterBoneModelAR(pPed->character,
                pPed->character->personality->form->index_head_bone,
                pPed->field_0x0c->field_0x1c->poses[pPed->field_0x0c->field_0x1a],
                pPed->hit_points <= 0);
        }
    }
}

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

inline int C2_HOOK_FASTCALL PedAnimCausesMovement(tPedestrian* pPed) {

    return pPed->character->personality->form->moves[pPed->character->field_0x7].move->field_0x2a == 0;
}

inline void C2_HOOK_FASTCALL StopSmoothTurning(tPedestrian* pPed) {

    pPed->field_0x0c->field_0x6c = 0;
}

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d5820
// STUB: CARMA2_SW 0x004d1f10
// STUB: CARMA2_CHEAT 0x004db3e0
void C2_HOOK_FASTCALL DoSpurt(br_matrix34* pMat, br_vector3* pPos, br_vector3* pNormal, br_vector3* pVelocity) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// SetLastHitter

inline void C2_HOOK_FASTCALL MakePedVanish(tPedestrian* pPed) {

    if (!(pPed->flags & 0x80)) {
        PipeSinglePedPos(pPed, &pPed->pos, &gZero_v__car);
        pPed->flags |= 0x80;
        gPipe_halted_ped_status = 1;
        SetCharacterPhysicsLevelAR(pPed->character, 0);
        CharacterNoLongerRenderable(pPed->character);
        gPipe_halted_ped_status = 0;
        OneLessPed(pPed);
    }
}

// CBPassiveCollision

// BastardIt

// MungeProxRay

// DoToPeds

// DoPedAnnihilator

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

// MungeNapalm

// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cfab0
// STUB: CARMA2_SW 0x004cc1b0
// STUB: CARMA2_CHEAT 0x004d5640
float C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, br_vector3* pDir) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d1d70
// STUB: CARMA2_SW 0x004ce470
// STUB: CARMA2_CHEAT 0x004d7910
tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pOn_face, int* pFace_changed, br_vector3* pPos, int pForce, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004cef80
// STUB: CARMA2_SW 0x004cb680
// STUB: CARMA2_CHEAT 0x004d4b10
void C2_HOOK_FASTCALL PedScanForObjects(tPedestrian* pPed, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CatAndMouseBonus

inline void C2_HOOK_FASTCALL StillifyCorpse(tPedestrian* pPed, tU32 pTime, int pArg3) {

    if (gMutant_speed == 0.0f) {
        SetCharacterMoveAR(pPed->character, -1, 0.0f, pArg3, 0, pTime);
    } else {
        SetCharacterMoveAR(pPed->character, -1, -gMutant_speed, pArg3, 0, pTime);
    }
}

// StraightenOutPed

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004d1030
// STUB: CARMA2_SW 0x004cd730
// STUB: CARMA2_CHEAT 0x004d6bd0
int C2_HOOK_FASTCALL ActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject, tFace_ref* pFace) {
    NOT_IMPLEMENTED();
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

// RecordPedSlide

// PedProcessForces

// CBMovedByPhysics

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

// SetModelCallBacks

// BuildPedestrian

// AreaOfTriangle

// MakeRandomPointInTriangle

// SpawnPedsOnFace

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

inline void C2_HOOK_FASTCALL SetNextRandomTurn(tPedestrian* pPed, tU32 pTime) {

    if (pPed->movement_spec->max_time_between != 0) {
        pPed->field_0x0c->next_turn_time = IRandomBetween(pPed->movement_spec->min_time_between, pPed->movement_spec->max_time_between) + pTime;
    } else {
        pPed->field_0x0c->next_turn_time = 0;
    }
}

inline void C2_HOOK_FASTCALL RandomWander(tPedestrian* pPed, tU32 pTime) {
    br_matrix34 mat;
    br_vector3 dir;
    br_vector3 new_dir;
    br_vector3 old_dir;
    tPed_cache_006944c0 saved_cache;

    if (pPed->field_0x0c->next_turn_time != 0 && pPed->field_0x0c->next_turn_time <= pTime) {
        SetNextRandomTurn(pPed, pTime);
        if (PercentageChance(50)) {
            BrMatrix34Rotate(&mat, IRandomBetween(1820, pPed->movement_spec->max_random_angle), &y_unit_vector);
        } else {
            BrMatrix34Rotate(&mat, -IRandomBetween(1820, pPed->movement_spec->max_random_angle), &y_unit_vector);
        }
        dir.v[0] = pPed->character->field_0xc0.v[0];
        dir.v[1] = 0.0f;
        dir.v[2] = pPed->character->field_0xc0.v[2];
        BrMatrix34ApplyV(&new_dir, &dir, &mat);
        saved_cache = *pPed->field_0x0c;
        BrVector3Copy(&old_dir, &pPed->character->field_0xc0);
        SetPedXZDirection(pPed, &new_dir, 1.0f, pTime);
        RescanPedProximity(pPed, &new_dir);
        if (pPed->field_0x0c->field_0x18 != 0) {
            *pPed->field_0x0c = saved_cache;
            SetPedXZDirection(pPed, &old_dir, 0.0f, pTime);
        }
    }
}

inline void C2_HOOK_FASTCALL ResetScanDirection(tPedestrian* pPed) {

    BrVector3Set(&pPed->field_0x0c->field_0x0c, 0.0f, 0.0f, 0.0f);
}

inline void C2_HOOK_FASTCALL CheckForAvoidingAction(tPedestrian* pPed, tU32 pTime) {
    br_vector3* pos;
    float dist_squared;
    float speed;
    float proximity;
    int check_side;
    br_vector3 side;
    br_vector3 dir;
    br_vector3 other_dir;
    br_vector3 old_scan;
    tPed_cache_006944c0 saved_cache;

    pos = GetPedPos(pPed);
    dist_squared = Vector3DistanceSquared(pos, &pPed->field_0x0c->field_0x3c);
    if (dist_squared < gPed_min_dist_avoid_collisions_squared) {
        if (dist_squared > gPed_reach_squared) {
            speed = FRandomBetween(0.7f, 1.0f) * ((br_scalar)sqrt(dist_squared) - (br_scalar)sqrt(gPed_reach_squared));
            if (speed > 0.5) {
                speed = 0.5f;
            }
        } else {
            speed = 0.0f;
        }
        if (pPed->field_0x0c->field_0x18 == 1
            && (gPeds_suicidal || gBlind_pedestrians || (speed == 0.0f && pPed->action == ePed_action_running && PercentageChance(100)))) {
            pPed->field_0x0c->field_0x18 = 0;
        } else if (!gPeds_suicidal || !(pPed->field_0x0c->field_0x18 & 0x4)) {
            if (pPed->field_0x0c->field_0x18 == 4) {
                if (pTime < pPed->field_0x0c->field_0x90) {
                    return;
                }
                pPed->field_0x0c->field_0x90 = pTime + IRandomBetween(200, 500);
            }
            check_side = !Vector3IsZero(&pPed->field_0x0c->field_0x30);
            if (dist_squared == 0.0f) {
                BrVector3Copy(&dir, &pPed->field_0x0c->field_0xa4);
            } else {
                if (fabs(BrVector3Dot(&pPed->field_0x0c->field_0xa4, &pPed->character->field_0xc0)) > 0.985) {
                    side.v[0] = 0.0f;
                    side.v[1] = (float)(PercentageChance(50) ? -1 : 1);
                    side.v[2] = 0.0f;
                } else {
                    DRVector3SafeCross(&side, &pPed->field_0x0c->field_0xa4, &pPed->character->field_0xc0);
                }
                DRVector3SafeCross(&dir, &side, &pPed->field_0x0c->field_0xa4);
                if (check_side && BrVector3Dot(&dir, &pPed->field_0x0c->field_0x30) < 0.0f) {
                    BrVector3Scale(&side, &side, -1.0f);
                    DRVector3SafeCross(&dir, &side, &pPed->field_0x0c->field_0xa4);
                }
            }
            SetPedXZDirection(pPed, &dir, speed, pTime);
            BrVector3Copy(&old_scan, &pPed->field_0x0c->field_0xa4);
            proximity = RescanPedProximity(pPed, &dir);
            if (pPed->field_0x0c->field_0x18 != 0 && dist_squared != 0.0f) {
                BrVector3Scale(&side, &side, -1.0f);
                DRVector3SafeCross(&other_dir, &side, &old_scan);
                SetPedXZDirection(pPed, &other_dir, speed, pTime);
                saved_cache = *pPed->field_0x0c;
                if (RescanPedProximity(pPed, &other_dir) <= proximity) {
                    *pPed->field_0x0c = saved_cache;
                    SetPedXZDirection(pPed, &dir, speed, pTime);
                    ResetScanDirection(pPed);
                }
            }
        }
    } else {
        ResetScanDirection(pPed);
    }
}

inline void C2_HOOK_FASTCALL SmoothTurnPedestrian(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* cache;
    float t;
    float s;
    br_vector3 dir;

    cache = pPed->field_0x0c;
    if (pTime >= cache->field_0x70) {
        StopSmoothTurning(pPed);
        SetCharacterDirectionAR(pPed->character, &pPed->field_0x0c->field_0x48, &pPed->character->field_0xcc);
    } else {
        t = (float)(pTime - cache->field_0x6c) / (float)(cache->field_0x70 - cache->field_0x6c);
        s = 1.0f - t;
        dir.v[0] = cache->field_0x60.v[0] * s + cache->field_0x54.v[0] * t;
        dir.v[1] = pPed->field_0x0c->field_0x60.v[1] * s + pPed->field_0x0c->field_0x54.v[1] * t;
        dir.v[2] = pPed->field_0x0c->field_0x60.v[2] * s + pPed->field_0x0c->field_0x54.v[2] * t;
        SetCharacterDirectionAR(pPed->character, &dir, &pPed->character->field_0xcc);
    }
}

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

inline void C2_HOOK_FASTCALL InitProcessData(tPedestrian* pPed, tU32 pTime) {
    tPed_cache_006944c0* cache;

    cache = pPed->field_0x0c;
    if (cache->field_0xb0 != NULL) {
        cache->field_0xb0->field_0x14 = NULL;
        cache->field_0xb0 = NULL;
    }
    SetNextRandomTurn(pPed, pTime);
    cache->field_0x18 = 0;
    cache->field_0x6c = 0;
    cache->field_0x80 = 0;
    cache->field_0x19 = 0;
    cache->field_0xc0 = NULL;
    cache->field_0x1c = NULL;
    cache->field_0x2e = 0;
    cache->field_0x21 = 0;
    cache->field_0xa0 = -1;
    cache->field_0x9c = 0;
    cache->field_0x88 = 0;
    cache->field_0x8c = 0;
    cache->field_0x84 = gPed_spurting_period;
    cache->field_0xc4 &= ~0x1;
    cache->field_0x94 = 0;
    cache->field_0x90 = 0;
    cache->field_0x98 = 0;
    memset(cache->field_0x23, 0, sizeof(cache->field_0x23));
}

inline int C2_HOOK_FASTCALL PedFallingForever(tPedestrian* pPed) {

    if (!(pPed->flags & 0x80)) {
        MakePedVanish(pPed);
        if (pPed->hit_points > 0) {
            ScoreForKilledPedestrian(pPed, pPed->pos.v[1] - FindYVerticallyBelow(&pPed->pos));
        }
        return 1;
    } else {
        return 0;
    }
}

inline void C2_HOOK_FASTCALL MakeEmBleed(tPedestrian* pPed, tU32 pTime) {
    int i;
    int parent;
    tPed_cache_006944c0* cache;
    tPed_personality* personality;
    tPed_form* form;
    tPhysics_object* object;
    tU8* lumps;
    br_matrix34* mat;
    br_vector3* v;
    br_vector3 dir;

    if (GET_CHARACTER_COLLISION_OBJECT(pPed->character)->last_special_volume != NULL
        && GET_CHARACTER_COLLISION_OBJECT(pPed->character)->last_special_volume->gravity_multiplier < 1.0f) {
        return;
    }
    if (gGoreLevel > 0) {
        if (gMutant_speed != 0.0f && pPed->hit_points <= 0 && pPed->action == ePed_action_dead
            && pTime - gLast_bleed_giblet_time > 300 && PercentageChance(40)) {
            gLast_bleed_giblet_time = pTime;
            DoGiblets(pPed, GET_CHARACTER_COLLISION_OBJECT(pPed->character), NULL, 0.06f, &pPed->pos, (int)gGiblet_scrape_start);
        }
        if (pPed->character->field_0xc != 0 && pPed->field_0x0c != NULL) {
            cache = pPed->field_0x0c;
            if (pTime - cache->field_0x88 > cache->field_0x84) {
                if (pPed->hit_points <= 0) {
                    cache->field_0x84 = (tU32)(cache->field_0x84 * 1.2);
                    if (pPed->field_0x0c->field_0x84 > 1000) {
                        pPed->field_0x0c->field_0x84 = 0;
                    }
                }
                pPed->field_0x0c->field_0x88 = pTime;
                personality = pPed->character->personality;
                form = personality->form;
                for (i = 1; i < form->count_bones; i++) {
                    if ((pPed->character->field_0xc & gPow2_array[i]) && !(pPed->character->field_0x10 & gPow2_array[i])) {
                        object = pPed->character->personality->form->boned_physicing[pPed->character->field_0x6].collision_infos[i];
                        lumps = &pPed->field_0x0c->field_0x23[i];
                        if (*lumps < gPed_spurty_lumps_count && BrVector3LengthSquared(&object->v) == 0.0f) {
                            *lumps = 0;
                        } else {
                            if (*lumps != 0) {
                                *lumps -= gPed_spurty_lumps_count;
                            }
                            mat = GetBoneMatrixPtr(pPed->character, i);
                            BrVector3Copy(&dir, (br_vector3*)mat->m[0]);
                            DoSpurt(mat, &personality->bones[i].field_0x20, &dir, &object->v);
                        }
                        parent = form->bones[i].indices[0];
                        if (pPed->character->field_0xc & gPow2_array[parent]) {
                            v = &pPed->character->personality->form->boned_physicing[pPed->character->field_0x6].collision_infos[parent]->v;
                            if (pPed->field_0x0c->field_0x23[i] == 0) {
                                continue;
                            }
                        } else {
                            v = &pPed->character->field_0xd8;
                        }
                        if (BrVector3LengthSquared(v) != 0.0f || pPed->field_0x0c->field_0x84 != 0) {
                            mat = GetBoneMatrixPtr(pPed->character, parent);
                            if (parent != 0) {
                                BrVector3Copy(&dir, (br_vector3*)mat->m[0]);
                            } else {
                                BrMatrix34ApplyV(&dir, &gPed_bone_look_vecs[i - 1], mat);
                            }
                            DoSpurt(mat, &personality->bones[i].field_0x2c, &dir, v);
                        }
                    }
                }
            }
        }
    }
}

inline void C2_HOOK_FASTCALL CheckPowerupMoveSubstitution(tPedestrian* pPed, tU32 pTime) {
    int move_id;
    int new_move_lut_id;
    int new_move_id;

    move_id = pPed->character->personality->form->moves[pPed->character->field_0x7].id;
    if (gDrunk_pedestrians) {
        new_move_lut_id = 4;
    } else if (gDancing_peds) {
        new_move_lut_id = 3;
    } else if (gPanicking_peds) {
        new_move_lut_id = 5;
    } else {
        if (move_id == 112 || move_id == 31 || move_id == 111) {
            SetPedMove(pPed, 30, -1, 0, 0, pTime, -1);
        }
        return;
    }
    new_move_id = gPed_move_fsm[move_id][new_move_lut_id + 1];
    if (move_id != new_move_id) {
        SetPedMove(pPed, new_move_id, -1, 0, 0, pTime, -1);
    }
}

inline void C2_HOOK_FASTCALL SetRandomOmega(tPhysics_object* pObject, float pMax) {

    pObject->omega.v[0] = SRandomPosNeg(1.0f);
    pObject->omega.v[1] = SRandomPosNeg(1.0f);
    pObject->omega.v[2] = SRandomPosNeg(1.0f);
    pObject->omega.v[0] *= pMax / BrVector3Length(&pObject->omega);
    pObject->omega.v[1] *= pMax / BrVector3Length(&pObject->omega);
    pObject->omega.v[2] *= pMax / BrVector3Length(&pObject->omega);
}

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

// FUNCTION: CARMA2_HW 0x004d3740
// FUNCTION: CARMA2_SW 0x004cfe30
// FUNCTION: CARMA2_CHEAT 0x004d9300
void C2_HOOK_FASTCALL MungePedestrians(tU32 pFrame_period) {
    tU32 the_time;
    int count_scared_pedestrians;
    tPedestrian* scared_pedestrians[10];
    int scare_now;
    int i;
    int j;
    tPedestrian* ped;
    tPed_cache_006944c0* ped_cache;
    br_vector3* ped_pos;

    the_time = GetTotalTime();
    if (gPed_valium_left != 0) {
        gPed_valium_left -= gFrame_period;
        if (gPed_valium_left < 0) {
            gPed_valium_left = 0;
        }
    }
    gCount_active_peds = 0;
    gPedestrians_in_sight = NULL;
    gPeds_already_munged = GetTotalTime() == gPed_last_munging;
    gPed_last_munging = GetTotalTime();
    count_scared_pedestrians = 0;
    if (the_time > gLast_scare_time + 300 && !gBlind_pedestrians && gPed_valium_left == 0) {
        scare_now = 1;
        gLast_scare_time = the_time;
    } else {
        scare_now = 0;
    }
    if (!gAction_replay_mode) {
        ped = gPedestrian_array;
        for (i = 0; i < gPed_count; i++, ped++) {
            if ((ped->flags & 0x80)
                || (((float)fabs(ped->pos.v[0] - gCamera_to_world.m[3][0]) >= gPed_process_distance
                        || (float)fabs(ped->pos.v[1] - gCamera_to_world.m[3][1]) >= gPed_process_distance_inner
                        || (float)fabs(ped->pos.v[2] - gCamera_to_world.m[3][2]) >= gPed_process_distance)
                    && (ped->field_0x0c == NULL || ped->field_0x0c->field_0x8c == 0 || the_time - ped->field_0x0c->field_0x8c >= 7500))
                || (!(ped->flags & 0x1)
                    && !gPed_nearness
                    && (float)fabs(ped->pos.v[0] - gCamera_to_world.m[3][0]) <= gPed_popup_distance
                    && (float)fabs(ped->pos.v[1] - gCamera_to_world.m[3][1]) <= gPed_popup_distance_inner
                    && (float)fabs(ped->pos.v[2] - gCamera_to_world.m[3][2]) <= gPed_popup_distance)) {
                ped->flags &= ~0x1;
            } else {
                ped->flags |= 0x1;
                if (scare_now && (ped->flags & 0x1) && ped->action == ePed_action_walking && count_scared_pedestrians < (int)REC2_ASIZE(scared_pedestrians)) {
                    scared_pedestrians[count_scared_pedestrians] = ped;
                    count_scared_pedestrians++;
                }
            }
        }
    }
    ped = gPedestrian_array;
    for (i = 0; i < gPed_count; i++, ped++) {
        if (ped->flags & 0x1) {
            if (ped->character->field_0x4 < 0 && !gAction_replay_mode) {
                if (MakeCharacterRenderable(ped->character) == 1) {
                    ped->flags &= ~0x1;
                    continue;
                }
                if (MakeCharacterCollideworthy(ped->character, 0, 0) == 21) {
                    CharacterNoLongerRenderable(ped->character);
                    ped->flags &= ~0x1;
                    continue;
                }
                PipeSinglePedStatus(ped,
                    0, 1, 0, 1,
                    -1, ped->character->field_0x4,
                    -1, ped->character->field_0x5,
                    &ped->character->field_0xc0,
                    &ped->character->field_0xcc,
                    &ped->character->field_0x8c);
            }
            ped->next = gPedestrians_in_sight;
            gPedestrians_in_sight = ped;
            if (!gAction_replay_mode) {
                if (ped->flags & 0x20) {
                    ped->flags |= 0x40;
                } else {
                    ped->flags |= 0x20;
                }
            }
            if (ped->speed_factor != gPedestrian_speed_factor && ped->hit_points > 0) {
                SetCharacterMove(ped->character, -1, -1.0f, 0, 0, 0);
                if (ped->speed_factor == 0.0f) {
                    EnableOverallMovement();
                }
                if (gPedestrian_speed_factor != 0.0f) {
                    SetCharacterMove(ped->character, -1, (float)(tS16)ped->character->field_0x1e / gPedestrian_speed_factor, 0, 0, 0);
                } else {
                    DisableOverallMovement();
                }
                ped->speed_factor = gPedestrian_speed_factor;
            }
            gCount_active_peds++;
            if (ped->field_0x0c == NULL || ped->field_0x0c == &gPed_cache_00694328) {
                if (gAction_replay_mode) {
                    ped->field_0x0c = &gPed_cache_00694328;
                } else {
                    tU32 oldest_time;
                    int oldest_index;

                    oldest_index = 0;
                    oldest_time = the_time + 1;
                    for (j = 0; j < (int)REC2_ASIZE(gPed_cache_006944c0); j++) {
                        if (gPed_cache_006944c0[j].field_0xb4 == NULL) {
                            ped->field_0x0c = &gPed_cache_006944c0[j];
                            break;
                        }
                        if (gPed_cache_006944c0[j].field_0x74 < oldest_time) {
                            oldest_time = gPed_cache_006944c0[j].field_0x74;
                            oldest_index = j;
                        }
                    }
                    if (j == (int)REC2_ASIZE(gPed_cache_006944c0)) {
                        ped->field_0x0c = &gPed_cache_006944c0[oldest_index];
                        ped->field_0x0c->field_0xb4->field_0x0c = NULL;
                    }
                }
                InitProcessData(ped, the_time);
            }
            ped_cache = ped->field_0x0c;
            ped_cache->field_0xb4 = ped;
            ped_cache->field_0x74 = the_time;
            ped_cache->field_0xc4 &= ~0x2;
            ped_pos = GetPedPos(ped);
            if (!gAction_replay_mode) {
                if (ped->character->field_0x14 != 0) {
                    if (PHILGetObjectProperty(GET_CHARACTER_COLLISION_OBJECT(ped->character), 0) != gPed_gravity_multiplier) {
                        PHILSetObjectProperty(GET_CHARACTER_COLLISION_OBJECT(ped->character), 0, gPed_gravity_multiplier);
                        if (ped->character->field_0x6 >= 0) {
                            for (j = 0; j < ped->character->personality->form->count_bones; j++) {
                                PHILSetObjectProperty(ped->character->personality->form->boned_physicing[ped->character->field_0x6].collision_infos[j], 0, gPed_gravity_multiplier);
                            }
                        }
                    }
                }
            }
            if (!gAction_replay_mode) {
                if (GET_CHARACTER_COLLISION_OBJECT(ped->character)->last_special_volume != NULL
                    && GET_CHARACTER_COLLISION_OBJECT(ped->character)->last_special_volume->gravity_multiplier < 1.0f) {
                    if (ped->field_0x0c->field_0x98 < 0) {
                        ped->field_0x0c->field_0x98 = 0;
                        KillNapalmBolt(&gNapalm_bolts[1 - ped->field_0x0c->field_0x98]);
                    } else if (ped->field_0x0c->field_0x98 > 0) {
                        StopObjectSmokingInstantly(ped->character->personality->form->simple_physicing[(tS8)ped->character->field_0x5].collision_info);
                        ped->field_0x0c->field_0x98 = 0;
                    }
                    if (ped_cache->field_0x94 == 0) {
                        ped_cache->field_0x94 = the_time;
                    }
                    if (ped->hit_points > 0) {
                        if (ped->character->personality->form->moves[ped->character->field_0x7].id != 110) {
                            SetPedMove(ped, 110, -1, 0, 0, the_time, ePed_action_impacting);
                        }
                        if (the_time - ped_cache->field_0x94 > 30000) {
                            KillPedestrian(ped, NULL);
                        }
                    } else {
                        if (ped->character->personality->form->moves[ped->character->field_0x7].id != 94) {
                            SetPedMove(ped, 94, -1, 0, 0, the_time, ePed_action_impacting);
                        }
                    }
                } else {
                    ped_cache->field_0x94 = 0;
                }
                if (ped->field_0x0c->field_0x98 > 0 && ped->hit_points > 0) {
                    if (the_time >= (tU32)ped->field_0x0c->field_0x98 && !gImmortal_peds) {
                        KillPedestrian(ped, NULL);
                        DoPostElectricution(ped, the_time, 0.0f, 0.5f);
                        ped->field_0x0c->field_0x98 = 0;
                    } else {
                        MakePedNoise(ped, 6, 0, NULL);
                    }
                }
            }
            if (!(ped->character->field_0x14 & 0x4)
                && (ped->action == ePed_action_walking || ped->action == ePed_action_running)
                && (!gAction_replay_mode || gBOOL_00744804)) {
                int nearby_changed_point;
                int on_face;
                int face_changed;
                br_vector3 face_pos;
                float height;
                tPed_face_cache_0x34* face_cache;

                AcceleratePed(ped, the_time);
                ped->flags &= ~0x6;
                nearby_changed_point = 0;
                if (gCount_changed_points != 0) {
                    for (j = 0; j < gCount_changed_points; j++) {
                        if (Vector3DistanceSquared(&ped->pos, &gChanged_points[j]) < 400.0) {
                            nearby_changed_point = 1;
                            break;
                        }
                    }
                }
                face_cache = RecacheAndSetFace(ped, &on_face, &face_changed, &face_pos, gBOOL_00744804 | nearby_changed_point, the_time);
                if (face_cache == NULL) {
                    face_cache = RecacheAndSetFace(ped, &on_face, &face_changed, &face_pos, gBOOL_00744804 | nearby_changed_point, the_time);
                }
                if (on_face && ped->action != ePed_action_falling && ped->action != ePed_action_dead && ped->action != ePed_action_impacting) {
                    height = ped_pos->v[1] - face_pos.v[1] - GetClearanceFromCharacterInstance(ped->character, 0);
                    if (ped->action == ePed_action_falling
                        || (face_cache->field_0x30 != NULL
                            && height <= ped->character->personality->jump_height - (-3.0 / WORLD_SCALE_D)
                            && face_cache->field_0x30->normal.v[1] > gPed_cos_max_slope)) {
                        SetCharacterPositionAR(ped->character, &face_pos, 1);
                        if (ped->field_0x0c->field_0x6c != 0) {
                            StopSmoothTurning(ped);
                            SetPedXZDirection(ped, &ped->field_0x0c->field_0x54, 0.0f, the_time);
                        } else {
                            SetPedXZDirection(ped, &ped->character->field_0xc0, 0.0f, the_time);
                        }
                        face_changed = 1;
                    } else {
                        if (!(ped->flags & 0x40)) {
                            MakePedVanish(ped);
                            continue;
                        }
                        face_cache->field_0x30 = NULL;
                        dr_dprintf("!!!!!! %0x turned on physics due to falling (%s)", ped, ped->character->personality->name);
                        SetThisPedPhysicing(ped);
                        SetPedMove(ped, 72, -1, 0, 0, the_time, ePed_action_falling);
                        ped->action = ePed_action_falling;
                    }
                }
                if (!(ped->character->field_0x14 & 0x4) && ped->hit_points > 0) {
                    if (gBOOL_00744804) {
                        face_changed = 1;
                    }
                    if (!face_changed) {
                        face_changed = BrVector3Dot(&ped->character->field_0xc0, &ped->field_0x0c->field_0x0c) < 0.9998;
                    }
                    if (face_changed && face_cache->field_0x30 != NULL && ped->field_0x0c->field_0x6c == 0) {
                        RescanPedProximity(ped, NULL);
                    }
                    if (face_cache->field_0x30 != NULL && PedAnimCausesMovement(ped)) {
                        if (ped->field_0x0c->field_0x6c != 0) {
                            SmoothTurnPedestrian(ped, the_time);
                            RescanPedProximity(ped, NULL);
                        } else {
                            RandomWander(ped, the_time);
                        }
                        PedScanForObjects(ped, the_time);
                        if (ped->field_0x0c->field_0x18 != 0) {
                            CheckForAvoidingAction(ped, the_time);
                        }
                    }
                }
            }
            if (!gAction_replay_mode
                && !(ped->character->field_0x14 & 0x4)
                && GET_CHARACTER_COLLISION_OBJECT(ped->character)->last_special_volume != NULL
                && GET_CHARACTER_COLLISION_OBJECT(ped->character)->last_special_volume->gravity_multiplier < 1.0f
                && gGravity_multiplier == 1.0
                && GET_CHARACTER_COLLISION_OBJECT(ped->character)->water_depth_factor == 1.0) {
                SetThisPedPhysicing(ped);
                SetRandomOmega(GET_CHARACTER_COLLISION_OBJECT(ped->character), 2.0f);
            }
            CheckPowerupMoveSubstitution(ped, the_time);
            MungeCharacterAnimation(ped->character, the_time, 0);
            if (!(ped->character->field_0x14 & 0x4)) {
                tPhysics_object* object;

                object = ped->character->personality->form->simple_physicing[(tS8)ped->character->field_0x5].collision_info;
                if (object != NULL) {
                    BrVector3Copy(&object->v, &ped->character->field_0xd8);
                    if (ped->hit_points <= 0
                        && ped->action == ePed_action_dead
                        && ped->character->personality->form->moves[ped->character->field_0x7].id != 113
                        && ((gMutant_speed != 0.0f && ped->character->field_0x1e == 0) || (gMutant_speed == 0.0f && ped->character->field_0x1e != 0))) {
                        StillifyCorpse(ped, the_time, 0);
                    }
                }
            }
            BrVector3Copy(&ped->pos, ped_pos);
            if (!gAction_replay_mode) {
                MungePedHeadAnim(ped, the_time);
                MakeEmBleed(ped, the_time);
                if (scare_now && (ped->action == ePed_action_running || ped->hit_points <= 0 || ped->action == ePed_action_impacting)) {
                    for (j = 0; j < count_scared_pedestrians; j++) {
                        if (scared_pedestrians[j]->action == ePed_action_walking && Vector3DistanceSquared(ped_pos, &scared_pedestrians[j]->pos) < 9.0f) {
                            ScarePedestrian(scared_pedestrians[j], the_time, 1, 0);
                        }
                    }
                }
            }
        } else {
            if (ped->character->field_0x4 >= 0 && !gAction_replay_mode) {
                tU32 old_dismembered;

                PipeSinglePedStatus(ped,
                    1, 0, 1, 0,
                    ped->character->field_0x4, 255,
                    ped->character->field_0x5, 255,
                    &ped->character->field_0xc0,
                    &ped->character->field_0xcc,
                    &ped->character->field_0x8c);
                if (ped->character->field_0xbc != NULL) {
                    StopCharacterMorphing(ped->character);
                }
                if (ped->character->field_0x14 & 0x4) {
                    PedFallingForever(ped);
                }
                old_dismembered = ped->character->field_0x10;
                SetCharacterPhysicsLevel(ped->character, 0);
                CharacterNoLongerRenderable(ped->character);
                if (ped->character->field_0x10 != old_dismembered) {
                    PipeSingleVanishedDismembered(ped, old_dismembered, ped->character->field_0x10);
                }
                if (ped->field_0x0c != NULL && ped->field_0x0c->field_0x98 != 0) {
                    if (ped->field_0x0c->field_0x98 < 0) {
                        ped->field_0x0c->field_0x98 = 0;
                        KillNapalmBolt(&gNapalm_bolts[1 - ped->field_0x0c->field_0x98]);
                    } else {
                        if (ped->hit_points > 0) {
                            ScoreForKilledPedestrian(ped, 0.0f);
                        }
                        StopObjectSmokingInstantly(ped->character->personality->form->simple_physicing[(tS8)ped->character->field_0x5].collision_info);
                        ped->field_0x0c->field_0x98 = 0;
                        MakePedVanish(ped);
                    }
                }
            }
        }
    }
    gPed_nearness = 0;
    gCount_changed_points = 0;
}

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

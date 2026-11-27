/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "11-world1.h"

#include "00-car.h"
#include "08-loading1.h"

#include "15-displays.h"
#include "28-world3.h"
#include "41-utility.h"
#include "55-volume.h"
#include "61-pedestrn-phys.h"
#include "70-packfile.h"
#include "globvars.h"

#include "c2_stdio.h"

#include <string.h>

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006ab7a8
// GLOBAL: CARMA2_SW 0x006a51f8
// GLOBAL: CARMA2_CHEAT 0x006b2e38
int gDelete_count;

// GLOBAL: CARMA2_HW 0x006aafc8
// GLOBAL: CARMA2_SW 0x006a4a18
// GLOBAL: CARMA2_CHEAT 0x006b2658
br_actor* gDelete_list[500];

// GLOBAL: CARMA2_HW 0x006ab7b0
// GLOBAL: CARMA2_SW 0x006a5200
// GLOBAL: CARMA2_CHEAT 0x006b2e40
br_actor* gSpec_vol_actors[100];

// GLOBAL: CARMA2_HW 0x006ab940
// GLOBAL: CARMA2_SW 0x006a5390
// GLOBAL: CARMA2_CHEAT 0x006b2fd0
int gSpec_vol_mode;

// GLOBAL: CARMA2_HW 0x006b6400
// GLOBAL: CARMA2_SW 0x006ad0e0
// GLOBAL: CARMA2_CHEAT 0x006bdaa0
char gAdditional_model_path[256];

// GLOBAL: CARMA2_HW 0x006b6500
// GLOBAL: CARMA2_SW 0x006ad1e0
// GLOBAL: CARMA2_CHEAT 0x006bdba0
char gAdditional_actor_path[256];

// GLOBAL: CARMA2_HW 0x006b6600
// GLOBAL: CARMA2_SW 0x006ad2e0
// GLOBAL: CARMA2_CHEAT 0x006bdca0
br_actor* gLast_actor;

// GLOBAL: CARMA2_HW 0x006b6620
// GLOBAL: CARMA2_SW 0x006ad300
// GLOBAL: CARMA2_CHEAT 0x006bdcc0
br_model* gAdditional_models[1000];

// GLOBAL: CARMA2_HW 0x006ab7a4
// GLOBAL: CARMA2_SW 0x006a51f4
// GLOBAL: CARMA2_CHEAT 0x006b2e34
br_actor* gKnown_actor;

// GLOBAL: CARMA2_HW 0x006ab798
// GLOBAL: CARMA2_SW 0x006a51e8
// GLOBAL: CARMA2_CHEAT 0x006b2e28
br_vector3 gActor_centre;

// GLOBAL: CARMA2_HW 0x00661080
// GLOBAL: CARMA2_SW 0x0065b9d4
// GLOBAL: CARMA2_CHEAT 0x006688b8
tRotate_mode gCurrent_rotate_mode = eRotate_mode_y;

// GLOBAL: CARMA2_HW 0x006ab944
// GLOBAL: CARMA2_SW 0x006a5394
// GLOBAL: CARMA2_CHEAT 0x006b2fd4
tScale_mode gCurrent_scale_mode;

// GLOBAL: CARMA2_HW 0x006ab948
// GLOBAL: CARMA2_SW 0x006a5398
// GLOBAL: CARMA2_CHEAT 0x006b2fd8
int gNumber_of_additional_models;

// FUNCTION: CARMA2_HW 0x00507210
// FUNCTION: CARMA2_SW 0x00502660
// FUNCTION: CARMA2_CHEAT 0x0050cc00
br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* pActor, br_matrix34* pMatrix, void* pArg) {
    int i;
    int parent_already_doomed;

    if ((pActor != gAdditional_actors
            && (pActor->identifier == NULL || pActor->identifier[0] == '&')
            && Vector3IsZero((br_vector3*)pMatrix->m[3]))
        || (pActor->model == NULL && pActor->type == BR_ACTOR_MODEL)) {
        parent_already_doomed = 0;
        for (i = 0; i < gDelete_count; i++) {
            if (gDelete_list[i] == pActor) {
                parent_already_doomed = 1;
                break;
            }
        }
        if (!parent_already_doomed) {
            gDelete_list[gDelete_count] = pActor;
            gDelete_count++;
        }
    }
    return 0;
}

inline void C2_HOOK_FASTCALL DeleteAnyZeroBastards(void) {
    int i;

    gDelete_count = 0;
    DRActorEnumRecurseWithTrans(gAdditional_actors, NULL, DeleteBastards, NULL);
    for (i = gDelete_count - 1; i >= 0; i--) {
        BrActorRemove(gDelete_list[i]);
    }
}

// FUNCTION: CARMA2_HW 0x005072b0
// FUNCTION: CARMA2_SW 0x00502700
// FUNCTION: CARMA2_CHEAT 0x0050cca0
br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMatrix, void* pArg) {
    int i;
    br_vector3 temp_point;

    if (pActor->identifier != NULL && pActor->identifier[0] != '&') {
        if (pActor->model != NULL) {
            for (i = 0; i < pActor->model->nvertices; i++) {
                temp_point = pActor->model->vertices[i].p;
                BrMatrix34ApplyP(&pActor->model->vertices[i].p, &temp_point, pMatrix);
            }
            BrModelUpdate(pActor->model, BR_MODU_ALL);
        }
        BrMatrix34Identity(&pActor->t.t.mat);
    }
    return 0;
}

int C2_HOOK_FASTCALL FindSpecVolIndex(br_actor* pActor) {
    int i;

    if (pActor != NULL) {
        for (i = 0; i < gProgram_state.special_volume_count; i++) {

            if (gSpec_vol_actors[i] == pActor) {
                return i;
            }
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x005074d0
// FUNCTION: CARMA2_SW 0x00502920
// FUNCTION: CARMA2_CHEAT 0x0050cec0
void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1) {

    pMat_1->map_transform.m[0][0] = 6.0f * BrVector3Length((br_vector3*)pMat->m[pAxis_0]);
    pMat_1->map_transform.m[1][1] = 6.0f * BrVector3Length((br_vector3*)pMat->m[pAxis_1]);
    BrMatrix23Copy(&pMat_2->map_transform, &pMat_1->map_transform);
}

void C2_HOOK_FASTCALL SetSpecVolMatSize(br_actor* pActor) {
    br_model* model;

    model = pActor->model;
    MungeMaterialSV(&pActor->t.t.mat,model->faces[5].material, model->faces[17].material, 0, 1);
    MungeMaterialSV(&pActor->t.t.mat,model->faces[11].material, model->faces[23].material, 1, 2);
    MungeMaterialSV(&pActor->t.t.mat,model->faces[7].material, model->faces[19].material, 0, 2);
}

void C2_HOOK_FASTCALL UpdateSpecVol(void) {
    int index;
    tSpecial_volume* v;

    index = FindSpecVolIndex(gLast_actor);
    if (index >= 0) {
        v = &gProgram_state.special_volumes[index];
        BrMatrix34Copy(&v->boundary.box.mat, &gLast_actor->t.t.mat);
        FindInverseAndWorldBox(v);
        SetSpecVolMatSize(gLast_actor);
    }
}

inline void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToAdditionalActors(void) {
    int i;
    tNon_car_spec* non_car;

    for (i = 0; i < gNum_active_non_cars; i++) {
        non_car = gActive_non_car_list[i];
        if ((tU16)non_car->car_ID == 0xffff && non_car->car_master_actor->parent == gNon_track_actor) {
            BrActorRemove(non_car->car_master_actor);
            BrActorAdd(gAdditional_actors, non_car->car_master_actor);
        }
    }
}

inline void C2_HOOK_FASTCALL PutAdditionalNonCarsOnToNonTrackActor(void) {
    int i;
    tNon_car_spec* non_car;

    for (i = 0; i < gNum_active_non_cars; i++) {
        non_car = gActive_non_car_list[i];
        if ((tU16)non_car->car_ID == 0xffff && non_car->car_master_actor->parent == gAdditional_actors) {
            BrActorRemove(non_car->car_master_actor);
            BrActorAdd(gNon_track_actor, non_car->car_master_actor);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00506f40
// FUNCTION: CARMA2_SW 0x00502390
// FUNCTION: CARMA2_CHEAT 0x0050c930
void C2_HOOK_FASTCALL SaveAdditionalActors(void) {
    int index;
    int found_free_name;
    tPath_name base_name;
    tPath_name file_name;
    char* dot;
    FILE* f;

    index = 0;
    found_free_name = 0;
    PutAdditionalNonCarsOnToAdditionalActors();
    DeleteAnyZeroBastards();
    if (gLast_actor != NULL) {
        DRActorEnumRecurseWithTrans(gLast_actor, NULL, ApplyTransToModels, NULL);
    }
    if (strstr(gAdditional_actor_path, "autosave") != NULL) {
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, gNumber_of_additional_models);
    } else {
        sprintf(base_name, "%s", gAdditional_actor_path);
        dot = strstr(base_name, ".");
        if (dot != NULL) {
            *dot = '\0';
        }
        while (!found_free_name) {
            sprintf(file_name, "%s%04d.ACT", base_name, index);
            f = DRfopen(file_name, "rb");
            if (f != NULL) {
                PFfclose(f);
                index++;
            } else {
                found_free_name = 1;
            }
        }
        strcpy(file_name, base_name);
        sprintf(base_name, "%s%04d.ACT", file_name, index);
        BrActorSave(base_name, gAdditional_actors);
        BrActorSave(gAdditional_actor_path, gAdditional_actors);
        sprintf(file_name, "%s", gAdditional_model_path);
        dot = strstr(file_name, ".");
        if (dot != NULL) {
            *dot = '\0';
        }
        sprintf(base_name, "%s%04d.DAT", file_name, index);
        BrModelSaveMany(base_name, gAdditional_models, gNumber_of_additional_models);
        BrModelSaveMany(gAdditional_model_path, gAdditional_models, gNumber_of_additional_models);
    }
    PutAdditionalNonCarsOnToNonTrackActor();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL DoSaveAdditionalStuff(void) {

    if (gSpec_vol_mode) {
        UpdateSpecVol();
        SaveSpecialVolumes();
    } else {
        SaveAdditionalActors();
    }
}

// FUNCTION: CARMA2_HW 0x00507360
// FUNCTION: CARMA2_SW 0x005027b0
// FUNCTION: CARMA2_CHEAT 0x0050cd50
void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void) {
    size_t len_actor_path;
    size_t len_model_path;

    len_actor_path = strlen(gAdditional_actor_path);
    len_model_path = strlen(gAdditional_actor_path);
    strcat(gAdditional_actor_path, " autosave");
    strcat(gAdditional_model_path, " autosave");
    DoSaveAdditionalStuff();
    gAdditional_actor_path[len_actor_path] = '\0';
    gAdditional_model_path[len_model_path] = '\0';
    FlushAllPedCaches();
}

// FUNCTION: CARMA2_HW 0x00508a50
// FUNCTION: CARMA2_SW 0x00503ea0
// FUNCTION: CARMA2_CHEAT 0x0050e440
void C2_HOOK_FASTCALL SnapAccToVertical(void) {

    if (gLast_actor != NULL) {
        BrVector3Set((br_vector3*)gLast_actor->t.t.mat.m[0], 1.0f, 0.0f, 0.0f);
        BrVector3Set((br_vector3*)gLast_actor->t.t.mat.m[1], 0.0f, 1.0f, 0.0f);
        BrVector3Set((br_vector3*)gLast_actor->t.t.mat.m[2], 0.0f, 0.0f, 1.0f);
        AutoSaveAdditionalStuff();
    }
}

inline void C2_HOOK_FASTCALL RotateAccessory(br_angle pAngle) {
    br_vector3 mr_offset;

    if (gLast_actor != NULL) {
        if (gSpec_vol_mode == 0 && gLast_actor->identifier != NULL && gLast_actor->identifier[0] == '@') {
            CentreActor(gLast_actor, &mr_offset);
        }
        switch (gCurrent_rotate_mode) {
        case eRotate_mode_x:
            BrMatrix34PreRotateX(&gLast_actor->t.t.mat, pAngle);
            break;
        case eRotate_mode_y:
            BrMatrix34PreRotateY(&gLast_actor->t.t.mat, pAngle);
            break;
        case eRotate_mode_z:
            BrMatrix34PreRotateZ(&gLast_actor->t.t.mat, pAngle);
            break;
        }
        if (gSpec_vol_mode == 0 && gLast_actor->identifier != NULL && gLast_actor->identifier[0] == '@') {
            DRActorEnumRecurseWithTrans(gLast_actor, NULL, ApplyTransToModels, NULL);
            OffsetActor(gLast_actor, &mr_offset);
        }
        AutoSaveAdditionalStuff();
    }
}

// FUNCTION: CARMA2_HW 0x00508c20
// FUNCTION: CARMA2_SW 0x00504070
// FUNCTION: CARMA2_CHEAT 0x0050e610
void C2_HOOK_FASTCALL RotateAccL(void) {

    RotateAccessory(BR_ANGLE_DEG(90));
}

// FUNCTION: CARMA2_HW 0x00508e50
// FUNCTION: CARMA2_SW 0x005042a0
// FUNCTION: CARMA2_CHEAT 0x0050e840
intptr_t C2_HOOK_CDECL OffsetModel(br_actor* pActor, void* pContext) {
    int i;

    if (pActor->model != NULL) {
        for (i = 0; i < pActor->model->nvertices; i++) {
            BrVector3Accumulate(&pActor->model->vertices[i].p, (br_vector3*)pContext);
        }
    }
    return 0;
}

inline void C2_HOOK_FASTCALL OffsetActor(br_actor* pActor, br_vector3* pOffset) {

    DRActorEnumRecurse(pActor, OffsetModel, pOffset);
}

// FUNCTION: CARMA2_HW 0x00508ec0
// FUNCTION: CARMA2_SW 0x00504310
// FUNCTION: CARMA2_CHEAT 0x0050e8b0
void C2_HOOK_FASTCALL CentreActor(br_actor* pActor, br_vector3* pOffset) {

    if (pActor->model != NULL) {
        if (gKnown_actor == gLast_actor) {
            BrVector3Scale(pOffset, &gActor_centre, -1.0f);
        } else {
            pOffset->v[0] = -(pActor->model->bounds.max.v[0] + pActor->model->bounds.min.v[0]) / 2.0f;
            pOffset->v[1] = -(pActor->model->bounds.max.v[1] + pActor->model->bounds.min.v[1]) / 2.0f;
            pOffset->v[2] = -(pActor->model->bounds.max.v[2] + pActor->model->bounds.min.v[2]) / 2.0f;
        }
    } else {
        BrVector3Set(pOffset, 0.0f, 0.0f, 0.0f);
    }
    OffsetActor(pActor, pOffset);
    BrVector3Negate(pOffset, pOffset);
}

// FUNCTION: CARMA2_HW 0x00508f70
// FUNCTION: CARMA2_SW 0x005043c0
// FUNCTION: CARMA2_CHEAT 0x0050e960
void C2_HOOK_FASTCALL RotateAccL2(void) {

    RotateAccessory(BR_ANGLE_DEG(15));
}

// FUNCTION: CARMA2_HW 0x005091a0
// FUNCTION: CARMA2_SW 0x005045f0
// FUNCTION: CARMA2_CHEAT 0x0050eb90
void C2_HOOK_FASTCALL RotateAccL3(void) {

    RotateAccessory(BR_ANGLE_DEG(5));
}

// FUNCTION: CARMA2_HW 0x005093d0
// FUNCTION: CARMA2_SW 0x00504820
// FUNCTION: CARMA2_CHEAT 0x0050edc0
void C2_HOOK_FASTCALL RotateAccL4(void) {

    RotateAccessory(BR_ANGLE_DEG(1));
}

// FUNCTION: CARMA2_HW 0x00509600
// FUNCTION: CARMA2_SW 0x00504a50
// FUNCTION: CARMA2_CHEAT 0x0050eff0
void C2_HOOK_FASTCALL RotateAccR(void) {

    RotateAccessory(BR_ANGLE_DEG(-90));
}

// FUNCTION: CARMA2_HW 0x00509830
// FUNCTION: CARMA2_SW 0x00504c80
// FUNCTION: CARMA2_CHEAT 0x0050f220
void C2_HOOK_FASTCALL RotateAccR2(void) {

    RotateAccessory(BR_ANGLE_DEG(-15));
}

// FUNCTION: CARMA2_HW 0x00509a60
// FUNCTION: CARMA2_SW 0x00504eb0
// FUNCTION: CARMA2_CHEAT 0x0050f450
void C2_HOOK_FASTCALL RotateAccR3(void) {

    RotateAccessory(BR_ANGLE_DEG(-5));
}

// FUNCTION: CARMA2_HW 0x00509c90
// FUNCTION: CARMA2_SW 0x005050e0
// FUNCTION: CARMA2_CHEAT 0x0050f680
void C2_HOOK_FASTCALL RotateAccR4(void) {

    RotateAccessory(BR_ANGLE_DEG(-1));
}

// FUNCTION: CARMA2_HW 0x00509ec0
// FUNCTION: CARMA2_SW 0x00505310
// FUNCTION: CARMA2_CHEAT 0x0050f8b0
void C2_HOOK_FASTCALL CycleAccRotate(void) {

    if (gCurrent_rotate_mode++ == eRotate_mode_z) {
        gCurrent_rotate_mode = eRotate_mode_x;
    }
    switch (gCurrent_rotate_mode) {
    case eRotate_mode_x:
        NewTextHeadupSlot(4, 0, 2000, -2, "Rotate mode: X");
        break;
    case eRotate_mode_y:
        NewTextHeadupSlot(4, 0, 2000, -2, "Rotate mode: Y");
        break;
    case eRotate_mode_z:
        NewTextHeadupSlot(4, 0, 2000, -2, "Rotate mode: Z");
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00509f30
// FUNCTION: CARMA2_SW 0x00505380
// FUNCTION: CARMA2_CHEAT 0x0050f920
void C2_HOOK_FASTCALL CycleAccScale(void) {

    if (gCurrent_scale_mode++ == eScale_mode_z) {
        gCurrent_scale_mode = eScale_mode_all;
    }
    switch (gCurrent_scale_mode) {
    case eScale_mode_all:
        NewTextHeadupSlot(4, 0, 2000, -2, "Scale mode: ALL");
        break;
    case eScale_mode_x:
        NewTextHeadupSlot(4, 0, 2000, -2, "Scale mode: X");
        break;
    case eScale_mode_y:
        NewTextHeadupSlot(4, 0, 2000, -2, "Scale mode: Y");
        break;
    case eScale_mode_z:
        NewTextHeadupSlot(4, 0, 2000, -2, "Scale mode: Z");
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00509fd0
// FUNCTION: CARMA2_SW 0x00505420
// FUNCTION: CARMA2_CHEAT 0x0050f9c0
void C2_HOOK_FASTCALL ScaleAccUp2(void) {

    ScaleAccessory(1.2f);
}

// FUNCTION: CARMA2_HW 0x00509fe0
// FUNCTION: CARMA2_SW 0x00505430
// FUNCTION: CARMA2_CHEAT 0x0050f9d0
void C2_HOOK_FASTCALL ScaleAccessory(float pScaling_factor) {
    br_vector3 mr_offset;

    if (gLast_actor != NULL) {
        if (gSpec_vol_mode == 0 && gLast_actor->identifier != NULL && gLast_actor->identifier[0] == '@') {
            CentreActor(gLast_actor, &mr_offset);
        }
        switch (gCurrent_scale_mode) {
        case eScale_mode_all:
            BrMatrix34PreScale(&gLast_actor->t.t.mat, pScaling_factor, pScaling_factor, pScaling_factor);
            break;
        case eScale_mode_x:
            BrMatrix34PreScale(&gLast_actor->t.t.mat, pScaling_factor, 1.0f, 1.0f);
            break;
        case eScale_mode_y:
            BrMatrix34PreScale(&gLast_actor->t.t.mat, 1.0f, pScaling_factor, 1.0f);
            break;
        case eScale_mode_z:
            BrMatrix34PreScale(&gLast_actor->t.t.mat, 1.0f, 1.0f, pScaling_factor);
            break;
        }
        if (gSpec_vol_mode == 0 && gLast_actor->identifier != NULL && gLast_actor->identifier[0] == '@') {
            DRActorEnumRecurseWithTrans(gLast_actor, NULL, ApplyTransToModels, NULL);
            OffsetActor(gLast_actor, &mr_offset);
        }
        AutoSaveAdditionalStuff();
    }
}

// FUNCTION: CARMA2_HW 0x0050a230
// FUNCTION: CARMA2_SW 0x00505680
// FUNCTION: CARMA2_CHEAT 0x0050fc20
void C2_HOOK_FASTCALL ScaleAccUp3(void) {

    ScaleAccessory(1.05f);
}

// FUNCTION: CARMA2_HW 0x0050a240
// FUNCTION: CARMA2_SW 0x00505690
// FUNCTION: CARMA2_CHEAT 0x0050fc30
void C2_HOOK_FASTCALL ScaleAccUp4(void) {

    ScaleAccessory(1.002f);
}

// FUNCTION: CARMA2_HW 0x0050a250
// FUNCTION: CARMA2_SW 0x005056a0
// FUNCTION: CARMA2_CHEAT 0x0050fc40
void C2_HOOK_FASTCALL ScaleAccDown2(void) {

    ScaleAccessory(1 / 1.2f);
}

// FUNCTION: CARMA2_HW 0x0050a260
// FUNCTION: CARMA2_SW 0x005056b0
// FUNCTION: CARMA2_CHEAT 0x0050fc50
void C2_HOOK_FASTCALL ScaleAccDown3(void) {

    ScaleAccessory((float)(1 / 1.05));
}

// FUNCTION: CARMA2_HW 0x0050a270
// FUNCTION: CARMA2_SW 0x005056c0
// FUNCTION: CARMA2_CHEAT 0x0050fc60
void C2_HOOK_FASTCALL ScaleAccDown4(void) {

    ScaleAccessory((float)(1 / 1.002));
}

inline void C2_HOOK_FASTCALL MoveAccessory(br_scalar pX_shift, br_scalar pY_shift, br_scalar pZ_shift) {
    br_vector3 v;

    if (gLast_actor != NULL) {
        BrVector3Set(&v, pX_shift, pY_shift, pZ_shift);
        BrVector3Accumulate(&gLast_actor->t.t.translate.t, &v);
        AutoSaveAdditionalStuff();
    }
}

// FUNCTION: CARMA2_HW 0x0050a280
// FUNCTION: CARMA2_SW 0x005056d0
// FUNCTION: CARMA2_CHEAT 0x0050fc70
void C2_HOOK_FASTCALL MoveXAccL(void) {

    MoveAccessory(1.0f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050a420
// FUNCTION: CARMA2_SW 0x00505870
// FUNCTION: CARMA2_CHEAT 0x0050fe10
void C2_HOOK_FASTCALL MoveXAccL2(void) {

    MoveAccessory(0.2f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050a5c0
// FUNCTION: CARMA2_SW 0x00505a10
// FUNCTION: CARMA2_CHEAT 0x0050ffb0
void C2_HOOK_FASTCALL MoveXAccL3(void) {

    MoveAccessory(0.02f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050a760
// FUNCTION: CARMA2_SW 0x00505bb0
// FUNCTION: CARMA2_CHEAT 0x00510150
void C2_HOOK_FASTCALL MoveXAccL4(void) {

    MoveAccessory(0.002f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050a900
// FUNCTION: CARMA2_SW 0x00505d50
// FUNCTION: CARMA2_CHEAT 0x005102f0
void C2_HOOK_FASTCALL MoveXAccR(void) {

    MoveAccessory(-1.0f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050aaa0
// FUNCTION: CARMA2_SW 0x00505ef0
// FUNCTION: CARMA2_CHEAT 0x00510490
void C2_HOOK_FASTCALL MoveXAccR2(void) {

    MoveAccessory(-0.2f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050ac40
// FUNCTION: CARMA2_SW 0x00506090
// FUNCTION: CARMA2_CHEAT 0x00510630
void C2_HOOK_FASTCALL MoveXAccR3(void) {

    MoveAccessory(-0.02f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050ade0
// FUNCTION: CARMA2_SW 0x00506230
// FUNCTION: CARMA2_CHEAT 0x005107d0
void C2_HOOK_FASTCALL MoveXAccR4(void) {

    MoveAccessory(-0.002f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050af80
// FUNCTION: CARMA2_SW 0x005063d0
// FUNCTION: CARMA2_CHEAT 0x00510970
void C2_HOOK_FASTCALL MoveYAccL(void) {

    MoveAccessory(0.0f, 1.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050b120
// FUNCTION: CARMA2_SW 0x00506570
// FUNCTION: CARMA2_CHEAT 0x00510b10
void C2_HOOK_FASTCALL MoveYAccL2(void) {

    MoveAccessory(0.0f, 0.2f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050b2c0
// FUNCTION: CARMA2_SW 0x00506710
// FUNCTION: CARMA2_CHEAT 0x00510cb0
void C2_HOOK_FASTCALL MoveYAccL3(void) {

    MoveAccessory(0.0f, 0.02f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050b460
// FUNCTION: CARMA2_SW 0x005068b0
// FUNCTION: CARMA2_CHEAT 0x00510e50
void C2_HOOK_FASTCALL MoveYAccL4(void) {

    MoveAccessory(0.0f, 0.002f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050b600
// FUNCTION: CARMA2_SW 0x00506a50
// FUNCTION: CARMA2_CHEAT 0x00510ff0
void C2_HOOK_FASTCALL MoveYAccR(void) {

    MoveAccessory(0.0f, -1.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050b7a0
// FUNCTION: CARMA2_SW 0x00506bf0
// FUNCTION: CARMA2_CHEAT 0x00511190
void C2_HOOK_FASTCALL MoveYAccR2(void) {

    MoveAccessory(0.0f, -0.2f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050b940
// FUNCTION: CARMA2_SW 0x00506d90
// FUNCTION: CARMA2_CHEAT 0x00511330
void C2_HOOK_FASTCALL MoveYAccR3(void) {

    MoveAccessory(0.0f, -0.02f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050bae0
// FUNCTION: CARMA2_SW 0x00506f30
// FUNCTION: CARMA2_CHEAT 0x005114d0
void C2_HOOK_FASTCALL MoveYAccR4(void) {

    MoveAccessory(0.0f, -0.002f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x0050bc80
// FUNCTION: CARMA2_SW 0x005070d0
// FUNCTION: CARMA2_CHEAT 0x00511670
void C2_HOOK_FASTCALL MoveZAccL(void) {

    MoveAccessory(0.0f, 0.0f, 1.0f);
}

// FUNCTION: CARMA2_HW 0x0050be20
// FUNCTION: CARMA2_SW 0x00507270
// FUNCTION: CARMA2_CHEAT 0x00511810
void C2_HOOK_FASTCALL MoveZAccL2(void) {

    MoveAccessory(0.0f, 0.0f, 0.2f);
}

// FUNCTION: CARMA2_HW 0x0050bfc0
// FUNCTION: CARMA2_SW 0x00507410
// FUNCTION: CARMA2_CHEAT 0x005119b0
void C2_HOOK_FASTCALL MoveZAccL3(void) {

    MoveAccessory(0.0f, 0.0f, 0.02f);
}

// FUNCTION: CARMA2_HW 0x0050c160
// FUNCTION: CARMA2_SW 0x005075b0
// FUNCTION: CARMA2_CHEAT 0x00511b50
void C2_HOOK_FASTCALL MoveZAccL4(void) {

    MoveAccessory(0.0f, 0.0f, 0.002f);
}

// FUNCTION: CARMA2_HW 0x0050c300
// FUNCTION: CARMA2_SW 0x00507750
// FUNCTION: CARMA2_CHEAT 0x00511cf0
void C2_HOOK_FASTCALL MoveZAccR(void) {

    MoveAccessory(0.0f, 0.0f, -1.0f);
}

// FUNCTION: CARMA2_HW 0x0050c4a0
// FUNCTION: CARMA2_SW 0x005078f0
// FUNCTION: CARMA2_CHEAT 0x00511e90
void C2_HOOK_FASTCALL MoveZAccR2(void) {

    MoveAccessory(0.0f, 0.0f, -0.2f);
}

// FUNCTION: CARMA2_HW 0x0050c640
// FUNCTION: CARMA2_SW 0x00507a90
// FUNCTION: CARMA2_CHEAT 0x00512030
void C2_HOOK_FASTCALL MoveZAccR3(void) {

    MoveAccessory(0.0f, 0.0f, -0.02f);
}

// FUNCTION: CARMA2_HW 0x0050c7e0
// FUNCTION: CARMA2_SW 0x00507c30
// FUNCTION: CARMA2_CHEAT 0x005121d0
void C2_HOOK_FASTCALL MoveZAccR4(void) {

    MoveAccessory(0.0f, 0.0f, -0.002f);
}

// FUNCTION: CARMA2_HW 0x0050c980
// FUNCTION: CARMA2_SW 0x00507dd0
// FUNCTION: CARMA2_CHEAT 0x00512370
void C2_HOOK_FASTCALL DropSpecVol0(void) {
}

br_material* C2_HOOK_FASTCALL GetInternalMat(void) {

    return BrMaterialFind("SPECVOL.MAT");
}

br_material* C2_HOOK_FASTCALL GetExternalMat(void) {

    return BrMaterialFind("SPECVOL2.MAT");
}

// FUNCTION: CARMA2_HW 0x0050c990
// FUNCTION: CARMA2_SW 0x00507de0
// FUNCTION: CARMA2_CHEAT 0x00512380
void C2_HOOK_FASTCALL DropSpecVol1(void) {

    DropSpecVol(1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0050c9a0
// STUB: CARMA2_SW 0x00507df0
// STUB: CARMA2_CHEAT 0x00512390
void C2_HOOK_FASTCALL DropSpecVol(int pIndex) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#define DrVertexSet(V, X, Y, Z) \
    do {                        \
        (V)[0] = (X);           \
        (V)[1] = (Y);           \
        (V)[2] = (Z);           \
    } while (0)

// FUNCTION: CARMA2_HW 0x0050cbf0
// FUNCTION: CARMA2_SW 0x00508040
// FUNCTION: CARMA2_CHEAT 0x005125e0
void C2_HOOK_FASTCALL BuildSpecVolModel(tSpecial_volume* pSpec, int pIndex, br_material* pInt_mat, br_material* pExt_mat) {
    int i;
    int temp;
    br_actor* actor;
    br_model* model;

    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    BrMatrix34Copy(&actor->t.t.mat, &pSpec->boundary.box.mat);
    actor->render_style = BR_RSTYLE_FACES;
    model = BrModelAllocate("", 12, 24);
    model->flags |= BR_MODF_KEEP_ORIGINAL | BR_MODF_DONT_WELD;
    BrVector3Set(&model->vertices[0].p, 1.0f, -1.0f, -1.0f);
    BrVector2Set(&model->vertices[0].map, 1.0f, 0.0f);
    BrVector3Set(&model->vertices[1].p, -1.0f, -1.0f, -1.0f);
    BrVector2Set(&model->vertices[1].map, 0.0f, 0.0f);
    BrVector3Set(&model->vertices[2].p, -1.0f, 1.0f, -1.0f);
    BrVector2Set(&model->vertices[2].map, 0.0f, 1.0f);
    BrVector3Set(&model->vertices[3].p, 1.0f, 1.0f, -1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 1.0f);
    BrVector3Set(&model->vertices[4].p, 1.0f, -1.0f, 1.0f);
    BrVector2Set(&model->vertices[4].map, 1.0f, 1.0f);
    BrVector3Set(&model->vertices[5].p, -1.0f, -1.0f, 1.0f);
    BrVector2Set(&model->vertices[5].map, 0.0f, 1.0f);
    BrVector3Set(&model->vertices[6].p, -1.0f, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[6].map, 0.0f, 0.0f);
    BrVector3Set(&model->vertices[7].p, 1.0f, 1.0f, 1.0f);
    BrVector2Set(&model->vertices[7].map, 1.0f, 0.0f);
    BrVector3Set(&model->vertices[8].p, 1.0f, -1.0f, -1.0f);
    BrVector2Set(&model->vertices[8].map, 0.0f, 1.0f);
    BrVector3Set(&model->vertices[9].p, 1.0f, -1.0f, 1.0f);
    BrVector2Set(&model->vertices[9].map, 0.0f, 0.0f);
    BrVector3Set(&model->vertices[10].p, -1.0f, -1.0f, -1.0f);
    BrVector2Set(&model->vertices[10].map, 1.0f, 1.0f);
    BrVector3Set(&model->vertices[11].p, -1.0f, -1.0f, 1.0f);
    BrVector2Set(&model->vertices[11].map, 1.0f, 0.0f);

    DrVertexSet(model->faces[0].vertices, 0, 3, 1);
    DrVertexSet(model->faces[1].vertices, 1, 3, 2);
    DrVertexSet(model->faces[2].vertices, 3, 7, 2);
    DrVertexSet(model->faces[3].vertices, 2, 7, 6);
    DrVertexSet(model->faces[4].vertices, 6, 7, 4);
    DrVertexSet(model->faces[5].vertices, 6, 4, 5);
    DrVertexSet(model->faces[6].vertices, 0, 5, 4);
    DrVertexSet(model->faces[7].vertices, 1, 5, 0);
    DrVertexSet(model->faces[8].vertices, 9, 7, 8);
    DrVertexSet(model->faces[9].vertices, 8, 7, 3);
    DrVertexSet(model->faces[10].vertices, 11, 2, 6);
    DrVertexSet(model->faces[11].vertices, 11, 10, 2);

    memcpy(&model->faces[12], model->faces, 12 * sizeof(br_face));
    for (i = 12; i < 24; i++) {
        temp = model->faces[i].vertices[0];
        model->faces[i].vertices[0] = model->faces[i].vertices[1];
        model->faces[i].vertices[1] = temp;
    }
    model->faces[5].material = model->faces[4].material = model->faces[1].material = model->faces[0].material = DRMaterialClone(pExt_mat, 1);
    model->faces[11].material = model->faces[10].material = model->faces[9].material = model->faces[8].material = DRMaterialClone(pExt_mat, 1);
    model->faces[7].material = model->faces[6].material = model->faces[3].material = model->faces[2].material = DRMaterialClone(pExt_mat, 1);
    model->faces[17].material = model->faces[16].material = model->faces[13].material = model->faces[12].material = DRMaterialClone(pInt_mat, 1);
    model->faces[23].material = model->faces[22].material = model->faces[21].material = model->faces[20].material = DRMaterialClone(pInt_mat, 1);
    model->faces[19].material = model->faces[18].material = model->faces[15].material = model->faces[14].material = DRMaterialClone(pInt_mat, 1);
    BrModelUpdate(model, BR_MODU_ALL);
    actor->model = model;
    BrActorAdd(gNon_track_actor, actor);
    gSpec_vol_actors[pIndex] = actor;
    SetSpecVolMatSize(actor);
}

// FUNCTION: CARMA2_HW 0x0050d270
// FUNCTION: CARMA2_SW 0x005086c0
// FUNCTION: CARMA2_CHEAT 0x00512c60
void C2_HOOK_FASTCALL IdentifySpecVol(void) {
    int i;
    int min_index;
    tSpecial_volume* v;
    br_scalar min_d;
    br_scalar d;
    br_vector3* p;
    char s[256];

    min_d = BR_SCALAR_MAX;
    p = &gProgram_state.current_car.pos;
    min_index = -1;
    for (i = 0, v = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, v++) {
        d = Vector3DistanceSquared((br_vector3*)v->boundary.box.mat.m[3], p);
        if (d < min_d) {
            min_d = d;
            min_index = i;
        }
    }
    if (min_index >= 0) {
        sprintf(s, "Locked onto Special Volume #%d", min_index);
        NewTextHeadupSlot(4, 0, 2000, -2, s);
        gLast_actor = gSpec_vol_actors[min_index];
    } else {
        gLast_actor = NULL;
        NewTextHeadupSlot(4, 0, 2000, -2, "No special volumes to lock onto");
    }
}

// FUNCTION: CARMA2_HW 0x0050d470
// FUNCTION: CARMA2_SW 0x005088c0
// FUNCTION: CARMA2_CHEAT 0x00512e60
void C2_HOOK_FASTCALL DelSpecVolumeGraph(int pIndex) {
    br_actor* actor;
    br_model* model;

    actor = gSpec_vol_actors[pIndex];
    model = actor->model;

    BrMaterialRemove(model->faces[5].material);
    BrMaterialRemove(model->faces[11].material);
    BrMaterialRemove(model->faces[7].material);
    BrMaterialRemove(model->faces[17].material);
    BrMaterialRemove(model->faces[23].material);
    BrMaterialRemove(model->faces[19].material);
    BrMaterialFree(model->faces[5].material);
    BrMaterialFree(model->faces[11].material);
    BrMaterialFree(model->faces[7].material);
    BrMaterialFree(model->faces[17].material);
    BrMaterialFree(model->faces[23].material);
    BrMaterialFree(model->faces[19].material);
    BrModelRemove(model);
    BrModelFree(model);
    BrActorRemove(actor);
    BrActorFree(actor);
}

// FUNCTION: CARMA2_HW 0x00511140
// FUNCTION: CARMA2_CHEAT 0x00516b30
void C2_HOOK_FASTCALL ShowSpecialVolumes(void) {
    int i;
    tSpecial_volume* v;
    br_material* internal_mat;
    br_material* external_mat;

    gLast_actor = NULL;
    gSpec_vol_mode = 1;
    internal_mat = GetInternalMat();
    external_mat = GetExternalMat();
    for (i = 0, v = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, v++) {
        if (v->boundary_type == eFx_boundary_box) {
            BuildSpecVolModel(v, i, internal_mat, external_mat);
        } else {
            gSpec_vol_actors[i] = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x005111c0
// FUNCTION: CARMA2_SW 0x0050c610
// FUNCTION: CARMA2_CHEAT 0x00516bb0
void C2_HOOK_FASTCALL HideSpecialVolumes(void) {
    int i;
    tSpecial_volume* v;

    gSpec_vol_mode = 0;
    for (i = 0, v = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, v++) {
        if (v->boundary_type == eFx_boundary_box) {
            DelSpecVolumeGraph(i);
        }
    }
}

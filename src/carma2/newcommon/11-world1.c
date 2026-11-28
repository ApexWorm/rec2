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
#include "27-powerup.h"
#include "28-world3.h"
#include "41-utility.h"
#include "42-input.h"
#include "49-physics.h"
#include "55-volume.h"
#include "61-pedestrn-phys.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"

#include "c2_stdio.h"

#include <math.h>
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

// GLOBAL: CARMA2_HW 0x006b75c4
// GLOBAL: CARMA2_SW 0x006ae2a4
// GLOBAL: CARMA2_CHEAT 0x006bec64
int gDropped_powerup_index;

// GLOBAL: CARMA2_HW 0x006ab94c
// GLOBAL: CARMA2_SW 0x006a539c
// GLOBAL: CARMA2_CHEAT 0x006b2fdc
char* gDrop_actor_name;

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

// FUNCTION: CARMA2_HW 0x00507340
// FUNCTION: CARMA2_SW 0x00502790
// FUNCTION: CARMA2_CHEAT 0x0050cd30
void C2_HOOK_FASTCALL FUN_00507340(void) {

    NewTextHeadupSlot(4, 0, 2000, -2, "Saving Additional Stuff");
    SaveAdditionalActors();
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

// FUNCTION: CARMA2_HW 0x00507580
// FUNCTION: CARMA2_SW 0x005029d0
// FUNCTION: CARMA2_CHEAT 0x0050cf70
void C2_HOOK_FASTCALL FUN_00507580(void) {
    int i;

    if (gNum_active_non_cars != 0) {
        NewTextHeadupSlot(4, 0, 2000, -2, "Can't load with active non cars");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -2, "Loading Additional Stuff");
        for (i = 0; i < gNumber_of_additional_models; i++) {
            BrModelRemove(gAdditional_models[i]);
            BrModelFree(gAdditional_models[i]);
        }
        BrActorRemove(gAdditional_actors);
        BrActorFree(gAdditional_actors);
        gLast_actor = NULL;
        gNumber_of_additional_models = BrModelLoadMany(gAdditional_model_path, gAdditional_models, REC2_ASIZE(gAdditional_models));
        for (i = 0; i < gNumber_of_additional_models; i++) {
            gAdditional_models[i]->flags = BR_MODF_UPDATEABLE;
        }
        BrModelAddMany(gAdditional_models, gNumber_of_additional_models);
        gAdditional_actors = BrActorLoad(gAdditional_actor_path);
        if (gAdditional_actors == NULL) {
            gAdditional_actors = BrActorAllocate(BR_ACTOR_NONE, NULL);
        }
        BrActorAdd(gUniverse_actor, gAdditional_actors);
    }
}

inline void C2_HOOK_FASTCALL UniquificateActorsName(br_actor* pUniverse_actor, br_actor* pActor, int pPowerup_index) {
    int highest;

    if (pActor->identifier != NULL && pActor->identifier[0] != '@') {
        highest = 0;
        DRActorEnumRecurse(pUniverse_actor, CalcHighestNonAmID, &highest);
        gDropped_powerup_index = pPowerup_index;
        DRActorEnumRecurse(pActor, SetID, (void*)(intptr_t)(highest + 1));
    }
}

inline void C2_HOOK_FASTCALL AccessoryHeadup(br_actor* pActor, const char* pPrefix) {
    char s[256];

    strcpy(s, pPrefix);
    if (pActor->identifier != NULL) {
        strcat(s, pActor->identifier);
    }
    NewTextHeadupSlot(4, 0, 2000, -2, s);
}

inline void C2_HOOK_FASTCALL DuplicateIfNotAmpersand(br_actor* pActor) {
    int highest;

    if (pActor->identifier != NULL && pActor->identifier[0] != '&') {
        highest = 0;
        DRActorEnumRecurse(gUniverse_actor, CalcHighestID, &highest);
        DRActorEnumRecurse(pActor, SetIDAndDupModel, &highest);
    }
}

// FUNCTION: CARMA2_HW 0x005076a0
// FUNCTION: CARMA2_SW 0x00502af0
// FUNCTION: CARMA2_CHEAT 0x0050d090
void C2_HOOK_FASTCALL DropActor(int pIndex, int pPowerup) {
    FILE* f;
    tPath_name the_path;
    char s[256];
    char s2[256];
    int i;
    int face_bastard;
    int face_count;
    tBounds kev_bounds;
    tFace_ref the_list[50];
    br_scalar nearest_bastard;
    br_scalar distance_bastard;
    br_scalar ts;
    br_vector3 side_vector;
    br_transform new_transform;

    if (gDrop_actor_name == NULL) {
        if (PDKeyDown(4)) {
            pIndex += 80;
        }
        if (PDKeyDown(2)) {
            pIndex += 40;
        }
        if (PDKeyDown(1)) {
            pIndex += 20;
        }
        if (PDKeyDown(0)) {
            pIndex += 10;
        }
        PathCat(the_path, gApplication_path, pPowerup ? "POWRSHIT.TXT" : "ACCESSRY.TXT");
        f = DRfopen(the_path, "rt");
        for (i = 0; i <= pIndex; i++) {
            GetAString(f, s);
        }
    } else {
        sprintf(s, "%s", gDrop_actor_name);
    }
    gLast_actor = LoadActor(s);
    if (gLast_actor == NULL || gLast_actor->model == NULL) {
        sprintf(s2, "CAN'T FIND %s", s);
        NewTextHeadupSlot(4, 0, 2000, -2, s2);
    } else if (gLast_actor->identifier != NULL && gLast_actor->identifier[0] != '&' && gLast_actor->model->nvertices == 0) {
        sprintf(s2, "CAN'T SHIT %s BECAUSE MODEL MUST BE IN REG OR START WITH '-'", s);
        NewTextHeadupSlot(4, 0, 2000, -2, s2);
        gLast_actor = NULL;
    } else {
        if (strlen(s) > 3 && gLast_actor->identifier != NULL && strlen(gLast_actor->identifier) > 3) {
            for (i = 0; i < 3; i++) {
                gLast_actor->identifier[i] = s[i];
            }
        }
        if (gLast_actor->identifier[0] == '&'
            && gLast_actor->identifier[1] >= '0' && gLast_actor->identifier[1] <= '9'
            && gNon_car_spec_list[(gLast_actor->identifier[1] - '0') * 10 + (gLast_actor->identifier[2] - '0')] == 0) {
            sprintf(s2, "CAN'T SHIT %s BECAUSE YOU DON'T HAVE A NONCAR.TXT FILE", s);
            NewTextHeadupSlot(4, 0, 2000, -2, s2);
            gLast_actor = NULL;
        }
        if (gLast_actor != NULL) {
            BrMatrix34Identity(&gLast_actor->t.t.mat);
            BrVector3Set(&kev_bounds.original_bounds.min, -0.05f, -0.05f, -0.05f);
            BrVector3Set(&kev_bounds.original_bounds.max, 0.05f, 0.05f, 0.05f);
            kev_bounds.mat = &gProgram_state.current_car.car_master_actor->t.t.mat;
            do {
                face_count = FindFacesInBox(&kev_bounds, the_list, REC2_ASIZE(the_list), NULL);
                BrVector3Scale(&kev_bounds.original_bounds.min, &kev_bounds.original_bounds.min, 2.0f);
                BrVector3Scale(&kev_bounds.original_bounds.max, &kev_bounds.original_bounds.max, 2.0f);
            } while (face_count == 0);
            face_bastard = -1;
            nearest_bastard = BR_SCALAR_MAX;
            for (i = 0; i < face_count; i++) {
                distance_bastard = DistanceFromFace(gOur_pos, &the_list[i]);
                if ((float)fabs(distance_bastard) < (float)fabs(nearest_bastard)) {
                    nearest_bastard = distance_bastard;
                    face_bastard = i;
                }
            }
            if (face_bastard >= 0) {
                if (gLast_actor->identifier[0] == '&'
                    && gLast_actor->identifier[1] >= '0' && gLast_actor->identifier[1] <= '9'
                    && gLast_actor->model->bounds.min.v[1] < 0.001) {
                    nearest_bastard += gLast_actor->model->bounds.min.v[1] - 0.001;
                }
                ts = -nearest_bastard;
                gLast_actor->t.t.mat.m[3][0] = the_list[face_bastard].normal.v[0] * ts + gOur_pos->v[0];
                gLast_actor->t.t.mat.m[3][1] = the_list[face_bastard].normal.v[1] * ts + gOur_pos->v[1];
                gLast_actor->t.t.mat.m[3][2] = the_list[face_bastard].normal.v[2] * ts + gOur_pos->v[2];
                if (!PDKeyDown(4)) {
                    BrVector3Set(&side_vector, -1.0f, 0.0f, 0.0f);
                    if ((float)fabs(BrVector3Dot(&side_vector, &the_list[face_bastard].normal)) > 0.9) {
                        BrVector3Set(&side_vector, 0.0f, -1.0f, 0.0f);
                    }
                    new_transform.type = BR_TRANSFORM_LOOK_UP;
                    BrVector3Cross(&new_transform.t.look_up.look, &side_vector, &the_list[face_bastard].normal);
                    BrVector3Copy(&new_transform.t.look_up.up, &the_list[face_bastard].normal);
                    BrVector3Copy(&new_transform.t.look_up.t, &gLast_actor->t.t.translate.t);
                    BrTransformToTransform(&gLast_actor->t, &new_transform);
                }
                gKnown_actor = gLast_actor;
                BrVector3Copy(&gActor_centre, &gLast_actor->t.t.translate.t);
                if (pPowerup) {
                    if (pIndex >= 66 && pIndex <= 85) {
                        SetSpinningPowerup(gLast_actor, 0);
                    }
                    if (pIndex >= 86 && pIndex <= 87) {
                        SetChangingPowerup(gLast_actor);
                    }
                }
                if (!pPowerup) {
                    DuplicateIfNotAmpersand(gLast_actor);
                } else {
                    gLast_actor->identifier[0] = '&';
                }
                UniquificateActorsName(gUniverse_actor, gLast_actor, pPowerup ? pIndex : -1);
                BrActorAdd(gAdditional_actors, gLast_actor);
                AutoSaveAdditionalStuff();
                if (pPowerup) {
                    sprintf(s, "SHAT OUT A %s", GetPowerupMessage(pIndex));
                    NewTextHeadupSlot(4, 0, 2000, -2, s);
                } else {
                    AccessoryHeadup(gLast_actor, "Shat out ");
                }
            }
        }
    }
    if (gDrop_actor_name == NULL) {
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x00507ea0
// FUNCTION: CARMA2_SW 0x005032f0
// FUNCTION: CARMA2_CHEAT 0x0050d890
intptr_t C2_HOOK_CDECL CalcHighestNonAmID(br_actor* pActor, void* pContext) {
    char s[256];
    int number;

    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        if (strlen(pActor->identifier) != 12) {
            number = 0;
        } else {
            strcpy(s, &pActor->identifier[4]);
            strtok(s, ".");
            sscanf(s, "%d", &number);
        }
        if (number > *(int*)pContext) {
            *(int*)pContext = number;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00507f40
// FUNCTION: CARMA2_SW 0x00503390
// FUNCTION: CARMA2_CHEAT 0x0050d930
intptr_t C2_HOOK_CDECL SetID(br_actor* pActor, void* pContext) {
    char s[256];

    if (pActor->identifier != NULL) {
        strcpy(s, pActor->identifier);
        strtok(s, ".");
        if (gDropped_powerup_index >= 0) {
            sprintf(&s[1], "\xa3%02d", gDropped_powerup_index);
        }
        strcat(s, "0000");
        sprintf(&s[4], "%04d", (int)(intptr_t)pContext);
        strcat(s, ".ACT");
        BrResFree(pActor->identifier);
        pActor->identifier = BrResStrDup(pActor, s);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00508050
// FUNCTION: CARMA2_SW 0x005034a0
// FUNCTION: CARMA2_CHEAT 0x0050da40
intptr_t C2_HOOK_CDECL CalcHighestID(br_actor* pActor, void* pContext) {
    char s[256];
    int number;

    if (pActor->identifier != NULL && pActor->identifier[0] == '@') {
        strcpy(s, &pActor->identifier[4]);
        s[4] = '\0';
        sscanf(s, "%d", &number);
        if (number > *(int*)pContext) {
            *(int*)pContext = number;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005080d0
// FUNCTION: CARMA2_SW 0x00503520
// FUNCTION: CARMA2_CHEAT 0x0050dac0
intptr_t C2_HOOK_CDECL SetIDAndDupModel(br_actor* pActor, void* pContext) {
    char s[256];
    char s2[256];
    br_model* new_model;

    if (pActor->identifier != NULL && pActor->identifier[0] != '@') {
        *(int*)pContext += 1;
        strcpy(s, pActor->identifier);
        s[0] = '@';
        strtok(s, ".");
        strcat(s, "0000");
        sprintf(&s[4], "%04d", *(int*)pContext);
        strcpy(s2, s);
        strcat(s, ".ACT");
        BrResFree(pActor->identifier);
        pActor->identifier = BrResStrDup(pActor, s);
        if (pActor->model != NULL) {
            strcat(s2, ".DAT");
            new_model = BrModelAllocate(s2, pActor->model->nvertices, pActor->model->nfaces);
            memcpy(new_model->vertices, pActor->model->vertices, pActor->model->nvertices * sizeof(br_vertex));
            memcpy(new_model->faces, pActor->model->faces, pActor->model->nfaces * sizeof(br_face));
            new_model->flags = pActor->model->flags;
            BrModelAdd(new_model);
            pActor->model = new_model;
            gAdditional_models[gNumber_of_additional_models] = new_model;
            gNumber_of_additional_models++;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005082d0
// FUNCTION: CARMA2_SW 0x00503720
// FUNCTION: CARMA2_CHEAT 0x0050dcc0
void C2_HOOK_FASTCALL DropActor0(void) {

    DropActor(0, 0);
}

// FUNCTION: CARMA2_HW 0x005082e0
// FUNCTION: CARMA2_SW 0x00503730
// FUNCTION: CARMA2_CHEAT 0x0050dcd0
void C2_HOOK_FASTCALL DropActor1(void) {

    DropActor(1, 0);
}

// FUNCTION: CARMA2_HW 0x005082f0
// FUNCTION: CARMA2_SW 0x00503740
// FUNCTION: CARMA2_CHEAT 0x0050dce0
void C2_HOOK_FASTCALL DropActor2(void) {

    DropActor(2, 0);
}

// FUNCTION: CARMA2_HW 0x00508300
// FUNCTION: CARMA2_SW 0x00503750
// FUNCTION: CARMA2_CHEAT 0x0050dcf0
void C2_HOOK_FASTCALL DropActor3(void) {

    DropActor(3, 0);
}

// FUNCTION: CARMA2_HW 0x00508310
// FUNCTION: CARMA2_SW 0x00503760
// FUNCTION: CARMA2_CHEAT 0x0050dd00
void C2_HOOK_FASTCALL DropActor4(void) {

    DropActor(4, 0);
}

// FUNCTION: CARMA2_HW 0x00508320
// FUNCTION: CARMA2_SW 0x00503770
// FUNCTION: CARMA2_CHEAT 0x0050dd10
void C2_HOOK_FASTCALL DropActor5(void) {

    DropActor(5, 0);
}

// FUNCTION: CARMA2_HW 0x00508330
// FUNCTION: CARMA2_SW 0x00503780
// FUNCTION: CARMA2_CHEAT 0x0050dd20
void C2_HOOK_FASTCALL DropActor6(void) {

    DropActor(6, 0);
}

// FUNCTION: CARMA2_HW 0x00508340
// FUNCTION: CARMA2_SW 0x00503790
// FUNCTION: CARMA2_CHEAT 0x0050dd30
void C2_HOOK_FASTCALL DropActor7(void) {

    DropActor(7, 0);
}

// FUNCTION: CARMA2_HW 0x00508350
// FUNCTION: CARMA2_SW 0x005037a0
// FUNCTION: CARMA2_CHEAT 0x0050dd40
void C2_HOOK_FASTCALL DropActor8(void) {

    DropActor(8, 0);
}

// FUNCTION: CARMA2_HW 0x00508360
// FUNCTION: CARMA2_SW 0x005037b0
// FUNCTION: CARMA2_CHEAT 0x0050dd50
void C2_HOOK_FASTCALL DropActor9(void) {

    DropActor(9, 0);
}

// FUNCTION: CARMA2_HW 0x00508370
// FUNCTION: CARMA2_SW 0x005037c0
// FUNCTION: CARMA2_CHEAT 0x0050dd60
void C2_HOOK_FASTCALL FUN_00508370(void) {

    DropActor(0, 1);
}

// FUNCTION: CARMA2_HW 0x00508380
// FUNCTION: CARMA2_SW 0x005037d0
// FUNCTION: CARMA2_CHEAT 0x0050dd70
void C2_HOOK_FASTCALL FUN_00508380(void) {

    DropActor(1, 1);
}

// FUNCTION: CARMA2_HW 0x00508390
// FUNCTION: CARMA2_SW 0x005037e0
// FUNCTION: CARMA2_CHEAT 0x0050dd80
void C2_HOOK_FASTCALL FUN_00508390(void) {

    DropActor(2, 1);
}

// FUNCTION: CARMA2_HW 0x005083a0
// FUNCTION: CARMA2_SW 0x005037f0
// FUNCTION: CARMA2_CHEAT 0x0050dd90
void C2_HOOK_FASTCALL FUN_005083a0(void) {

    DropActor(3, 1);
}

// FUNCTION: CARMA2_HW 0x005083b0
// FUNCTION: CARMA2_SW 0x00503800
// FUNCTION: CARMA2_CHEAT 0x0050dda0
void C2_HOOK_FASTCALL FUN_005083b0(void) {

    DropActor(4, 1);
}

// FUNCTION: CARMA2_HW 0x005083c0
// FUNCTION: CARMA2_SW 0x00503810
// FUNCTION: CARMA2_CHEAT 0x0050ddb0
void C2_HOOK_FASTCALL FUN_005083c0(void) {

    DropActor(5, 1);
}

// FUNCTION: CARMA2_HW 0x005083d0
// FUNCTION: CARMA2_SW 0x00503820
// FUNCTION: CARMA2_CHEAT 0x0050ddc0
void C2_HOOK_FASTCALL FUN_005083d0(void) {

    DropActor(6, 1);
}

// FUNCTION: CARMA2_HW 0x005083e0
// FUNCTION: CARMA2_SW 0x00503830
// FUNCTION: CARMA2_CHEAT 0x0050ddd0
void C2_HOOK_FASTCALL FUN_005083e0(void) {

    DropActor(7, 1);
}

// FUNCTION: CARMA2_HW 0x005083f0
// FUNCTION: CARMA2_SW 0x00503840
// FUNCTION: CARMA2_CHEAT 0x0050dde0
void C2_HOOK_FASTCALL FUN_005083f0(void) {

    DropActor(8, 1);
}

// FUNCTION: CARMA2_HW 0x00508400
// FUNCTION: CARMA2_SW 0x00503850
// FUNCTION: CARMA2_CHEAT 0x0050ddf0
void C2_HOOK_FASTCALL FUN_00508400(void) {

    DropActor(9, 1);
}

// FUNCTION: CARMA2_HW 0x00508830
// FUNCTION: CARMA2_SW 0x00503c80
// FUNCTION: CARMA2_CHEAT 0x0050e220
intptr_t C2_HOOK_CDECL DelReferencedModels(br_actor* pActor, void* pContext) {
    int i;

    for (i = 0; i < gNumber_of_additional_models; i++) {
        if (pActor->model == gAdditional_models[i]) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
            memcpy(&gAdditional_models[i], &gAdditional_models[i + 1], (gNumber_of_additional_models - i - 1) * sizeof(br_model*));
            gNumber_of_additional_models--;
            break;
        }
    }
    return 0;
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

// FUNCTION: CARMA2_HW 0x0050d1f0
// FUNCTION: CARMA2_SW 0x00508640
// FUNCTION: CARMA2_CHEAT 0x00512be0
void C2_HOOK_FASTCALL DropSpecVol2(void) {

    DropSpecVol(2);
}

// FUNCTION: CARMA2_HW 0x0050d200
// FUNCTION: CARMA2_SW 0x00508650
// FUNCTION: CARMA2_CHEAT 0x00512bf0
void C2_HOOK_FASTCALL DropSpecVol3(void) {

    DropSpecVol(3);
}

// FUNCTION: CARMA2_HW 0x0050d210
// FUNCTION: CARMA2_SW 0x00508660
// FUNCTION: CARMA2_CHEAT 0x00512c00
void C2_HOOK_FASTCALL DropSpecVol4(void) {

    DropSpecVol(4);
}

// FUNCTION: CARMA2_HW 0x0050d220
// FUNCTION: CARMA2_SW 0x00508670
// FUNCTION: CARMA2_CHEAT 0x00512c10
void C2_HOOK_FASTCALL DropSpecVol5(void) {

    DropSpecVol(5);
}

// FUNCTION: CARMA2_HW 0x0050d230
// FUNCTION: CARMA2_SW 0x00508680
// FUNCTION: CARMA2_CHEAT 0x00512c20
void C2_HOOK_FASTCALL DropSpecVol6(void) {

    DropSpecVol(6);
}

// FUNCTION: CARMA2_HW 0x0050d240
// FUNCTION: CARMA2_SW 0x00508690
// FUNCTION: CARMA2_CHEAT 0x00512c30
void C2_HOOK_FASTCALL DropSpecVol7(void) {

    DropSpecVol(7);
}

// FUNCTION: CARMA2_HW 0x0050d250
// FUNCTION: CARMA2_SW 0x005086a0
// FUNCTION: CARMA2_CHEAT 0x00512c40
void C2_HOOK_FASTCALL DropSpecVol8(void) {

    DropSpecVol(8);
}

// FUNCTION: CARMA2_HW 0x0050d260
// FUNCTION: CARMA2_SW 0x005086b0
// FUNCTION: CARMA2_CHEAT 0x00512c50
void C2_HOOK_FASTCALL DropSpecVol9(void) {

    DropSpecVol(9);
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

// FUNCTION: CARMA2_HW 0x0050d370
// FUNCTION: CARMA2_SW 0x005087c0
// FUNCTION: CARMA2_CHEAT 0x00512d60
void C2_HOOK_FASTCALL DeleteSpecVol(void) {
    int index;

    index = FindSpecVolIndex(gLast_actor);
    if (index >= 0) {
        DelSpecVolumeGraph(index);
        memcpy(&gProgram_state.special_volumes[index], &gProgram_state.special_volumes[index + 1], (gProgram_state.special_volume_count - index - 1) * sizeof(tSpecial_volume));
        memcpy(&gSpec_vol_actors[index], &gSpec_vol_actors[index + 1], (gProgram_state.special_volume_count - index - 1) * sizeof(br_actor*));
        gProgram_state.special_volume_count--;
        NewTextHeadupSlot(4, 0, 2000, -2, "There's been a special volumes MURDER!!");
        gLast_actor = NULL;
        if (gDefault_water_spec_vol_real > &gProgram_state.special_volumes[index]) {
            gDefault_water_spec_vol_real--;
        }
        SaveSpecialVolumes();
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

// FUNCTION: CARMA2_HW 0x0050d580
// FUNCTION: CARMA2_SW 0x005089d0
// FUNCTION: CARMA2_CHEAT 0x00512f70
void C2_HOOK_FASTCALL RotateSpecVolL(void) {

    RotateAccL();
}

// FUNCTION: CARMA2_HW 0x0050d7b0
// FUNCTION: CARMA2_SW 0x00508c00
// FUNCTION: CARMA2_CHEAT 0x005131a0
void C2_HOOK_FASTCALL RotateSpecVolL2(void) {

    RotateAccL2();
}

// FUNCTION: CARMA2_HW 0x0050d9e0
// FUNCTION: CARMA2_SW 0x00508e30
// FUNCTION: CARMA2_CHEAT 0x005133d0
void C2_HOOK_FASTCALL RotateSpecVolL3(void) {

    RotateAccL3();
}

// FUNCTION: CARMA2_HW 0x0050dc10
// FUNCTION: CARMA2_SW 0x00509060
// FUNCTION: CARMA2_CHEAT 0x00513600
void C2_HOOK_FASTCALL RotateSpecVolL4(void) {

    RotateAccL4();
}

// FUNCTION: CARMA2_HW 0x0050de40
// FUNCTION: CARMA2_SW 0x00509290
// FUNCTION: CARMA2_CHEAT 0x00513830
void C2_HOOK_FASTCALL RotateSpecVolR(void) {

    RotateAccR();
}

// FUNCTION: CARMA2_HW 0x0050e070
// FUNCTION: CARMA2_SW 0x005094c0
// FUNCTION: CARMA2_CHEAT 0x00513a60
void C2_HOOK_FASTCALL RotateSpecVolR2(void) {

    RotateAccR2();
}

// FUNCTION: CARMA2_HW 0x0050e2a0
// FUNCTION: CARMA2_SW 0x005096f0
// FUNCTION: CARMA2_CHEAT 0x00513c90
void C2_HOOK_FASTCALL RotateSpecVolR3(void) {

    RotateAccR3();
}

// FUNCTION: CARMA2_HW 0x0050e4d0
// FUNCTION: CARMA2_SW 0x00509920
// FUNCTION: CARMA2_CHEAT 0x00513ec0
void C2_HOOK_FASTCALL RotateSpecVolR4(void) {

    RotateAccR4();
}

// FUNCTION: CARMA2_HW 0x0050e700
// FUNCTION: CARMA2_SW 0x00509b50
// FUNCTION: CARMA2_CHEAT 0x005140f0
void C2_HOOK_FASTCALL CycleSpecVolRotate(void) {

    CycleAccRotate();
}

// FUNCTION: CARMA2_HW 0x0050e770
// FUNCTION: CARMA2_SW 0x00509bc0
// FUNCTION: CARMA2_CHEAT 0x00514160
void C2_HOOK_FASTCALL CycleSpecVolScale(void) {

    CycleAccScale();
}

// FUNCTION: CARMA2_HW 0x0050e810
// FUNCTION: CARMA2_SW 0x00509c60
// FUNCTION: CARMA2_CHEAT 0x00514200
void C2_HOOK_FASTCALL ScaleSpecVolUp2(void) {

    ScaleAccUp2();
}

// FUNCTION: CARMA2_HW 0x0050e820
// FUNCTION: CARMA2_SW 0x00509c70
// FUNCTION: CARMA2_CHEAT 0x00514210
void C2_HOOK_FASTCALL ScaleSpecVolUp3(void) {

    ScaleAccUp3();
}

// FUNCTION: CARMA2_HW 0x0050e830
// FUNCTION: CARMA2_SW 0x00509c80
// FUNCTION: CARMA2_CHEAT 0x00514220
void C2_HOOK_FASTCALL ScaleSpecVolUp4(void) {

    ScaleAccUp4();
}

// FUNCTION: CARMA2_HW 0x0050e840
// FUNCTION: CARMA2_SW 0x00509c90
// FUNCTION: CARMA2_CHEAT 0x00514230
void C2_HOOK_FASTCALL ScaleSpecVolDown2(void) {

    ScaleAccDown2();
}

// FUNCTION: CARMA2_HW 0x0050e850
// FUNCTION: CARMA2_SW 0x00509ca0
// FUNCTION: CARMA2_CHEAT 0x00514240
void C2_HOOK_FASTCALL ScaleSpecVolDown3(void) {

    ScaleAccDown3();
}

// FUNCTION: CARMA2_HW 0x0050e860
// FUNCTION: CARMA2_SW 0x00509cb0
// FUNCTION: CARMA2_CHEAT 0x00514250
void C2_HOOK_FASTCALL ScaleSpecVolDown4(void) {

    ScaleAccDown4();
}

// FUNCTION: CARMA2_HW 0x0050e870
// FUNCTION: CARMA2_SW 0x00509cc0
// FUNCTION: CARMA2_CHEAT 0x00514260
void C2_HOOK_FASTCALL MoveXSpecVolL(void) {

    MoveXAccL();
}

// FUNCTION: CARMA2_HW 0x0050ea10
// FUNCTION: CARMA2_SW 0x00509e60
// FUNCTION: CARMA2_CHEAT 0x00514400
void C2_HOOK_FASTCALL MoveXSpecVolL2(void) {

    MoveXAccL2();
}

// FUNCTION: CARMA2_HW 0x0050ebb0
// FUNCTION: CARMA2_SW 0x0050a000
// FUNCTION: CARMA2_CHEAT 0x005145a0
void C2_HOOK_FASTCALL MoveXSpecVolL3(void) {

    MoveXAccL3();
}

// FUNCTION: CARMA2_HW 0x0050ed50
// FUNCTION: CARMA2_SW 0x0050a1a0
// FUNCTION: CARMA2_CHEAT 0x00514740
void C2_HOOK_FASTCALL MoveXSpecVolL4(void) {

    MoveXAccL4();
}

// FUNCTION: CARMA2_HW 0x0050eef0
// FUNCTION: CARMA2_SW 0x0050a340
// FUNCTION: CARMA2_CHEAT 0x005148e0
void C2_HOOK_FASTCALL MoveXSpecVolR(void) {

    MoveXAccR();
}

// FUNCTION: CARMA2_HW 0x0050f090
// FUNCTION: CARMA2_SW 0x0050a4e0
// FUNCTION: CARMA2_CHEAT 0x00514a80
void C2_HOOK_FASTCALL MoveXSpecVolR2(void) {

    MoveXAccR2();
}

// FUNCTION: CARMA2_HW 0x0050f230
// FUNCTION: CARMA2_SW 0x0050a680
// FUNCTION: CARMA2_CHEAT 0x00514c20
void C2_HOOK_FASTCALL MoveXSpecVolR3(void) {

    MoveXAccR3();
}

// FUNCTION: CARMA2_HW 0x0050f3d0
// FUNCTION: CARMA2_SW 0x0050a820
// FUNCTION: CARMA2_CHEAT 0x00514dc0
void C2_HOOK_FASTCALL MoveXSpecVolR4(void) {

    MoveXAccR4();
}

// FUNCTION: CARMA2_HW 0x0050f570
// FUNCTION: CARMA2_SW 0x0050a9c0
// FUNCTION: CARMA2_CHEAT 0x00514f60
void C2_HOOK_FASTCALL MoveYSpecVolL(void) {

    MoveYAccL();
}

// FUNCTION: CARMA2_HW 0x0050f710
// FUNCTION: CARMA2_SW 0x0050ab60
// FUNCTION: CARMA2_CHEAT 0x00515100
void C2_HOOK_FASTCALL MoveYSpecVolL2(void) {

    MoveYAccL2();
}

// FUNCTION: CARMA2_HW 0x0050f8b0
// FUNCTION: CARMA2_SW 0x0050ad00
// FUNCTION: CARMA2_CHEAT 0x005152a0
void C2_HOOK_FASTCALL MoveYSpecVolL3(void) {

    MoveYAccL3();
}

// FUNCTION: CARMA2_HW 0x0050fa50
// FUNCTION: CARMA2_SW 0x0050aea0
// FUNCTION: CARMA2_CHEAT 0x00515440
void C2_HOOK_FASTCALL MoveYSpecVolL4(void) {

    MoveYAccL4();
}

// FUNCTION: CARMA2_HW 0x0050fbf0
// FUNCTION: CARMA2_SW 0x0050b040
// FUNCTION: CARMA2_CHEAT 0x005155e0
void C2_HOOK_FASTCALL MoveYSpecVolR(void) {

    MoveYAccR();
}

// FUNCTION: CARMA2_HW 0x0050fd90
// FUNCTION: CARMA2_SW 0x0050b1e0
// FUNCTION: CARMA2_CHEAT 0x00515780
void C2_HOOK_FASTCALL MoveYSpecVolR2(void) {

    MoveYAccR2();
}

// FUNCTION: CARMA2_HW 0x0050ff30
// FUNCTION: CARMA2_SW 0x0050b380
// FUNCTION: CARMA2_CHEAT 0x00515920
void C2_HOOK_FASTCALL MoveYSpecVolR3(void) {

    MoveYAccR3();
}

// FUNCTION: CARMA2_HW 0x005100d0
// FUNCTION: CARMA2_SW 0x0050b520
// FUNCTION: CARMA2_CHEAT 0x00515ac0
void C2_HOOK_FASTCALL MoveYSpecVolR4(void) {

    MoveYAccR4();
}

// FUNCTION: CARMA2_HW 0x00510270
// FUNCTION: CARMA2_SW 0x0050b6c0
// FUNCTION: CARMA2_CHEAT 0x00515c60
void C2_HOOK_FASTCALL MoveZSpecVolL(void) {

    MoveZAccL();
}

// FUNCTION: CARMA2_HW 0x00510410
// FUNCTION: CARMA2_SW 0x0050b860
// FUNCTION: CARMA2_CHEAT 0x00515e00
void C2_HOOK_FASTCALL MoveZSpecVolL2(void) {

    MoveZAccL2();
}

// FUNCTION: CARMA2_HW 0x005105b0
// FUNCTION: CARMA2_SW 0x0050ba00
// FUNCTION: CARMA2_CHEAT 0x00515fa0
void C2_HOOK_FASTCALL MoveZSpecVolL3(void) {

    MoveZAccL3();
}

// FUNCTION: CARMA2_HW 0x00510750
// FUNCTION: CARMA2_SW 0x0050bba0
// FUNCTION: CARMA2_CHEAT 0x00516140
void C2_HOOK_FASTCALL MoveZSpecVolL4(void) {

    MoveZAccL4();
}

// FUNCTION: CARMA2_HW 0x005108f0
// FUNCTION: CARMA2_SW 0x0050bd40
// FUNCTION: CARMA2_CHEAT 0x005162e0
void C2_HOOK_FASTCALL MoveZSpecVolR(void) {

    MoveZAccR();
}

// FUNCTION: CARMA2_HW 0x00510a90
// FUNCTION: CARMA2_SW 0x0050bee0
// FUNCTION: CARMA2_CHEAT 0x00516480
void C2_HOOK_FASTCALL MoveZSpecVolR2(void) {

    MoveZAccR2();
}

// FUNCTION: CARMA2_HW 0x00510c30
// FUNCTION: CARMA2_SW 0x0050c080
// FUNCTION: CARMA2_CHEAT 0x00516620
void C2_HOOK_FASTCALL MoveZSpecVolR3(void) {

    MoveZAccR3();
}

// FUNCTION: CARMA2_HW 0x00510dd0
// FUNCTION: CARMA2_SW 0x0050c220
// FUNCTION: CARMA2_CHEAT 0x005167c0
void C2_HOOK_FASTCALL MoveZSpecVolR4(void) {

    MoveZAccR4();
}

// FUNCTION: CARMA2_HW 0x00510f70
// FUNCTION: CARMA2_SW 0x0050c3c0
// FUNCTION: CARMA2_CHEAT 0x00516960
void C2_HOOK_FASTCALL SnapSpecVolToVertical(void) {

    SnapAccToVertical();
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

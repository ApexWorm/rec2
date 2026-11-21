/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "11-world1.h"

#include "globvars.h"
#include "28-world3.h"
#include "41-utility.h"
#include "55-volume.h"
#include "61-pedestrn.h"

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
// GLOBAL: CARMA2_CHEAT 0x006b2e40
br_actor* gSpec_vol_actors[100];

// GLOBAL: CARMA2_HW 0x006ab940
// GLOBAL: CARMA2_CHEAT 0x006b2fd0
int gSpec_vol_mode;

// GLOBAL: CARMA2_HW 0x006b6400
// GLOBAL: CARMA2_CHEAT 0x006bdaa0
char gAdditional_model_path[256];

// GLOBAL: CARMA2_HW 0x006b6500
// GLOBAL: CARMA2_CHEAT 0x006bdba0
char gAdditional_actor_path[256];

// GLOBAL: CARMA2_HW 0x006b6600
// GLOBAL: CARMA2_CHEAT 0x006bdca0
br_actor* gLast_actor;

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

// DeleteAnyZeroBastards

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

// PutAdditionalNonCarsOnToAdditionalActors

// PutAdditionalNonCarsOnToNonTrackActor

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00506f40
// STUB: CARMA2_CHEAT 0x0050c930
void C2_HOOK_FASTCALL SaveAdditionalActors(void) {
    NOT_IMPLEMENTED();
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
    len_model_path = strlen(gAdditional_model_path);
    strcat(gAdditional_actor_path, " autosave");
    strcat(gAdditional_model_path, " autosave");
    DoSaveAdditionalStuff();
    gAdditional_actor_path[len_actor_path] = '\0';
    gAdditional_model_path[len_model_path] = '\0';
    FlushAllPedCaches();
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

#define DrVertexSet(V, X, Y, Z) \
    do {                        \
        (V)[0] = (X);           \
        (V)[1] = (Y);           \
        (V)[2] = (Z);           \
    } while (0)

// FUNCTION: CARMA2_HW 0x0050cbf0
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

// FUNCTION: CARMA2_HW 0x0050d470
// FUNCTION: CARMA2_cHEAT 0x00512e60
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

// STUB: CARMA2_HW 0x005111c0
// STUB: CARMA2_CHEAT 0x00516bb0
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

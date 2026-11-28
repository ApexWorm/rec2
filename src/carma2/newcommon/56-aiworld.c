/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "56-aiworld.h"

#include "05-drmem.h"
#include "10-loading2.h"
#include "18-graphics2.h"
#include "30-opponent.h"
#include "57-drone.h"

inline void C2_HOOK_FASTCALL MakeVertexAndOffsetIt(br_scalar pX, br_scalar pY, br_scalar pZ, br_model* pModel, int pVertex_num, const br_vector3* pOffset) {

    BrVector3Set(&pModel->vertices[pVertex_num].p, pX, pY, pZ);
    BrVector3Accumulate(&pModel->vertices[pVertex_num].p, pOffset);
}

// FUNCTION: CARMA2_HW 0x00401f90
// FUNCTION: CARMA2_SW 0x00401f90
// FUNCTION: CARMA2_CHEAT 0x00401f90
void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial) {

    pModel->faces[pFace_num].vertices[0] = pV0;
    pModel->faces[pFace_num].vertices[1] = pV1;
    pModel->faces[pFace_num].vertices[2] = pV2;
    pModel->faces[pFace_num].smoothing = 0xffff;
    pModel->faces[pFace_num].material = pMaterial;
}

// FUNCTION: CARMA2_HW 0x00401000
// FUNCTION: CARMA2_SW 0x00401000
// FUNCTION: CARMA2_CHEAT 0x00401000
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {

    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
}

// FUNCTION: CARMA2_HW 0x00401030
// FUNCTION: CARMA2_SW 0x00401030
void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF) {

    PrintMemoryDump(0, "BEFORE LoadInOppoPaths()");
    LoadInOppoPaths(pF);
    PrintMemoryDump(0, "AFTER LoadInOppoPaths(), BEFORE LoadInDronePaths()");
    LoadInDronePaths(pF);
    PrintMemoryDump(0, "AFTER LoadInDronePaths()");
}

// FUNCTION: CARMA2_HW 0x00401070
// FUNCTION: CARMA2_SW 0x00401070
void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0, "BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars(pRace_info);
    PrintMemoryDump(0, "AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0, "AFTER LoadPerRaceDroneStuff()");
}

// InitialiseAIWorldForRace

// FUNCTION: CARMA2_HW 0x00401160
// FUNCTION: CARMA2_SW 0x00401160
// FUNCTION: CARMA2_CHEAT 0x00401160
void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {

    DisposeOpponents();
    DisposeDronesRaceStuff();
}

// MungeAIWorld

// FUNCTION: CARMA2_HW 0x004013d0
// FUNCTION: CARMA2_SW 0x004013d0
// FUNCTION: CARMA2_CHEAT 0x004013d0
void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pThe_vector);
    BrVector3Set(&trans.t.look_up.up, 0.f, 1.f, 0.f);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

// FUNCTION: CARMA2_HW 0x00401430
// FUNCTION: CARMA2_SW 0x00401430
// FUNCTION: CARMA2_CHEAT 0x00401430
void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp) {
    br_transform trans;

    trans.type = BR_TRANSFORM_LOOK_UP;
    BrVector3Copy(&trans.t.look_up.t, &pThe_actor->t.t.translate.t);
    BrVector3Copy(&trans.t.look_up.look, pLook);
    BrVector3Copy(&trans.t.look_up.up, pUp);
    BrTransformToTransform(&pThe_actor->t, &trans);
}

inline void C2_HOOK_FASTCALL AllocateMatsForOppoPathModel(void) {

    gMat_dk_yel = CreateSimpleMaterial(50);
    gMat_md_yel = CreateSimpleMaterial(51);
    gMat_lt_yel = CreateSimpleMaterial(52);
    gMat_dk_red = CreateSimpleMaterial(2);
    gMat_md_red = CreateSimpleMaterial(3);
    gMat_lt_red = CreateSimpleMaterial(4);
    gMat_dk_grn = CreateSimpleMaterial(66);
    gMat_md_grn = CreateSimpleMaterial(67);
    gMat_lt_grn = CreateSimpleMaterial(68);
    gMat_dk_blu = CreateSimpleMaterial(162);
    gMat_lt_blu = CreateSimpleMaterial(164);
    gMat_dk_turq = CreateSimpleMaterial(130);
    gMat_lt_turq = CreateSimpleMaterial(132);
    gMat_dk_gry = CreateSimpleMaterial(253);
    gMat_md_gry = CreateSimpleMaterial(254);
    gMat_lt_gry = CreateSimpleMaterial(255);
    gMats_allocated = 1;
}

// FUNCTION: CARMA2_HW 0x004014a0
// FUNCTION: CARMA2_SW 0x004014a0
// FUNCTION: CARMA2_CHEAT 0x004014a0
void C2_HOOK_FASTCALL FUN_004014a0(void) {

    if (!gMats_allocated) {
        AllocateMatsForOppoPathModel();
    }
    FUN_004af490();
    FUN_00453460();
}

// FUNCTION: CARMA2_HW 0x00401760
// FUNCTION: CARMA2_SW 0x00401760
// FUNCTION: CARMA2_CHEAT 0x00401760
br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex) {
    br_material* material;

    material = BrMaterialAllocate(NULL);
    material->colour = ((br_colour*)gRender_palette->pixels)[pIndex];
    material->index_base = (br_uint_8)pIndex;
    material->index_range = 1;
    material->flags = BR_MATF_TWO_SIDED;
    material->index_shade = NULL;
    material->colour_map = NULL;
    material->identifier = NULL;
    BrMaterialAdd(material);
    return material;
}

// FUNCTION: CARMA2_HW 0x004017c0
// FUNCTION: CARMA2_SW 0x004017c0
// FUNCTION: CARMA2_CHEAT 0x004017c0
void C2_HOOK_FASTCALL FUN_004017c0(void) {

    if (!gMats_allocated) {
        AllocateMatsForOppoPathModel();
    }
    ToggleDronePaths();
    FUN_004af490();
    if (gINT_0067689c++ != 0) {
        gINT_0067689c = 0;
    } else {
        FUN_004af4a0();
    }
}

// FUNCTION: CARMA2_HW 0x00401aa0
// FUNCTION: CARMA2_SW 0x00401aa0
// FUNCTION: CARMA2_CHEAT 0x00401aa0
void C2_HOOK_FASTCALL MakeAIEditCubeHere(br_model* pModel, int pVertex_index, int pFace_index, const br_vector3* pPos, br_material* pMaterial_1, br_material* pMaterial_2, br_material* pMaterial_3) {
    br_vector3 pos;
    br_vector3 offsets[8];

    BrVector3Set(&offsets[0], 0.1f, 0.1f, 0.1f);
    BrVector3Set(&offsets[1], 0.1f, -0.1f, 0.1f);
    BrVector3Set(&offsets[2], -0.1f, -0.1f, 0.1f);
    BrVector3Set(&offsets[3], -0.1f, 0.1f, 0.1f);
    BrVector3Set(&offsets[4], 0.1f, 0.1f, -0.1f);
    BrVector3Set(&offsets[5], 0.1f, -0.1f, -0.1f);
    BrVector3Set(&offsets[6], -0.1f, -0.1f, -0.1f);
    BrVector3Set(&offsets[7], -0.1f, 0.1f, -0.1f);
    BrVector3Copy(&pos, pPos);
    pos.v[1] += 0.15f;
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index, &offsets[0]);
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index + 1, &offsets[1]);
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index + 2, &offsets[2]);
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index + 3, &offsets[3]);
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index + 4, &offsets[4]);
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index + 5, &offsets[5]);
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index + 6, &offsets[6]);
    MakeVertexAndOffsetIt(pos.v[0], pos.v[1], pos.v[2], pModel, pVertex_index + 7, &offsets[7]);
    MakeFaceAndTextureIt(pModel, pFace_index, pVertex_index + 3, pVertex_index + 2, pVertex_index + 1, pMaterial_1);
    MakeFaceAndTextureIt(pModel, pFace_index + 1, pVertex_index, pVertex_index + 3, pVertex_index + 1, pMaterial_1);
    MakeFaceAndTextureIt(pModel, pFace_index + 2, pVertex_index + 1, pVertex_index + 5, pVertex_index + 4, pMaterial_2);
    MakeFaceAndTextureIt(pModel, pFace_index + 3, pVertex_index + 1, pVertex_index + 4, pVertex_index, pMaterial_2);
    MakeFaceAndTextureIt(pModel, pFace_index + 4, pVertex_index, pVertex_index + 4, pVertex_index + 3, pMaterial_3);
    MakeFaceAndTextureIt(pModel, pFace_index + 5, pVertex_index + 3, pVertex_index + 4, pVertex_index + 7, pMaterial_3);
    MakeFaceAndTextureIt(pModel, pFace_index + 6, pVertex_index + 4, pVertex_index + 5, pVertex_index + 7, pMaterial_1);
    MakeFaceAndTextureIt(pModel, pFace_index + 7, pVertex_index + 5, pVertex_index + 6, pVertex_index + 7, pMaterial_1);
    MakeFaceAndTextureIt(pModel, pFace_index + 8, pVertex_index + 2, pVertex_index + 7, pVertex_index + 6, pMaterial_2);
    MakeFaceAndTextureIt(pModel, pFace_index + 9, pVertex_index + 2, pVertex_index + 3, pVertex_index + 7, pMaterial_2);
    MakeFaceAndTextureIt(pModel, pFace_index + 10, pVertex_index + 1, pVertex_index + 2, pVertex_index + 6, pMaterial_3);
    MakeFaceAndTextureIt(pModel, pFace_index + 11, pVertex_index + 1, pVertex_index + 6, pVertex_index + 5, pMaterial_3);
}

// FUNCTION: CARMA2_HW 0x00401fe0
// FUNCTION: CARMA2_SW 0x00401fe0
// FUNCTION: CARMA2_CHEAT 0x00401fe0
void C2_HOOK_FASTCALL CalcNegativeXVector(br_vector3* pNegative_x_vector, const br_vector3* pStart, const br_vector3* pFinish, br_scalar pLength) {
    br_vector3 positive_y_vector;
    br_vector3 path_vector;

    BrVector3Sub(&path_vector, pFinish, pStart);
    BrVector3Set(&positive_y_vector, 0.0f, 1.0f, 0.0f);
    BrVector3Cross(pNegative_x_vector, &path_vector, &positive_y_vector);
    BrVector3Normalise(pNegative_x_vector, pNegative_x_vector);
    BrVector3Scale(pNegative_x_vector, pNegative_x_vector, pLength);
}

// FUNCTION: CARMA2_HW 0x004020e0
// FUNCTION: CARMA2_SW 0x004020e0
// FUNCTION: CARMA2_CHEAT 0x004020e0
void C2_HOOK_FASTCALL MakeAISimpleEditSectionHere(br_model* pModel, int pVertex_index, int pFace_index, const br_vector3* pStart, const br_vector3* pFinish, br_material* pMaterial_1, br_material* pMaterial_2) {
    br_vector3 start;
    br_vector3 finish;
    br_vector3 offset_up;
    br_vector3 offset_down;

    BrVector3Set(&offset_up, 0.0f, 0.1f, 0.0f);
    BrVector3Set(&offset_down, 0.0f, -0.1f, 0.0f);
    BrVector3Copy(&start, pStart);
    BrVector3Copy(&finish, pFinish);
    start.v[1] += 0.15f;
    finish.v[1] += 0.15f;
    MakeVertexAndOffsetIt(start.v[0], start.v[1], start.v[2], pModel, pVertex_index, &offset_up);
    MakeVertexAndOffsetIt(finish.v[0], finish.v[1], finish.v[2], pModel, pVertex_index + 1, &offset_up);
    MakeVertexAndOffsetIt(start.v[0], start.v[1], start.v[2], pModel, pVertex_index + 2, &offset_down);
    MakeVertexAndOffsetIt(finish.v[0], finish.v[1], finish.v[2], pModel, pVertex_index + 3, &offset_down);
    MakeFaceAndTextureIt(pModel, pFace_index, pVertex_index + 1, pVertex_index + 3, pVertex_index + 2, pMaterial_1);
    MakeFaceAndTextureIt(pModel, pFace_index + 1, pVertex_index, pVertex_index + 1, pVertex_index + 2, pMaterial_2);
}


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_28_WORLD3_H
#define GUARD_28_WORLD3_H

#include "brender/brender.h"
#include "rec2_types.h"

extern const char* gDepth_effect_names[3];
extern tU8* gTrack_flic_buffer;
extern tU32 gTrack_flic_buffer_size;
extern tFlic_descriptor gTrack_flic_descriptor;
extern br_angle gSky_image_width;
extern br_angle gSky_image_height;
extern br_angle gSky_image_underground;
extern int gTrack_depth_colour_red;
extern int gTrack_depth_colour_green;
extern int gTrack_depth_colour_blue;
extern tU8 gNon_car_spec_indices[100];
extern br_actor* gAdditional_actors;

extern tBrender_storage* gStorage_for_callbacks;

extern tRendererShadingType gMaterial_shading_for_callback;

extern void C2_HOOK_FASTCALL InitTreeSurgery(void);

extern int C2_HOOK_FASTCALL ModelIsATree(br_model* pModel, char* pName_replacement);

extern void C2_HOOK_FASTCALL PerformTreeSurgery(tBrender_storage* pStorage, br_model* pModel, const char* pName_replacement);

extern void C2_HOOK_FASTCALL PossibleTreeSurgery(br_model* pModel);

extern void C2_HOOK_FASTCALL AddExceptionToList(tMaterial_exception** pList, tMaterial_exception* pItem);

extern void C2_HOOK_FASTCALL LoadExceptionsFile(const char* pPath);

extern void C2_HOOK_FASTCALL LoadExceptionsFileForTrack(const char* pTrack_name);

extern void C2_HOOK_FASTCALL FreeExceptions(void);

extern int C2_HOOK_FASTCALL GetFaceFlag(int pIndex);

extern void C2_HOOK_FASTCALL MungeTrackModel(br_model* pModel);

extern void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info);

extern intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg);

// RemoveBoundsStructures

extern void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec);

extern void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world);

// AnimateSky

// CombineBounds

extern void C2_HOOK_FASTCALL CheckNonCarModelBounds(void);

extern void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds);

extern void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd);

extern void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod);

extern tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id);

extern int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path);

extern br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName);

extern br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName);

extern br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName);

extern tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id);

extern int C2_HOOK_FASTCALL AddShadeTables(tBrender_storage* pStorage_space, const char* pPath);

extern void C2_HOOK_FASTCALL LoadModelsFromLineInto(tBrender_storage* pStorage_space, FILE* pF);

extern int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath);

extern void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage* pStorage, FILE* pFile, tRendererShadingType pShading);

extern int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM);

extern void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath);

extern int C2_HOOK_FASTCALL GetFileName(const char *pPath, void *pContext);

extern int C2_HOOK_FASTCALL GetAdditionalFileName(const char* pPath, void* pContext);

extern void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);

extern void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);

extern void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

extern void C2_HOOK_FASTCALL DisallowDuplicates(void);

extern void C2_HOOK_FASTCALL AllowDuplicates(void);

#endif // GUARD_28_WORLD3_H

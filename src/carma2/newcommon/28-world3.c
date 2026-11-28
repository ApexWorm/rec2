/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "28-world3.h"

#include "08-loading1.h"
#include "17-world2.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "51-smash.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x006b75c0
// GLOBAL: CARMA2_SW 0x006ae2a0
// GLOBAL: CARMA2_CHEAT 0x006bec60
br_actor* gAdditional_actors;

// GLOBAL: CARMA2_HW 0x006b7820
// GLOBAL: CARMA2_SW 0x006ae500
// GLOBAL: CARMA2_CHEAT 0x006beec0
tBrender_storage* gStorage_for_callbacks;

// GLOBAL: CARMA2_HW 0x006aaa2c
// GLOBAL: CARMA2_SW 0x006a447c
// GLOBAL: CARMA2_CHEAT 0x006b25a4
int gDisallow_duplicates;

// GLOBAL: CARMA2_HW 0x006aaa20
// GLOBAL: CARMA2_SW 0x006a4470
// GLOBAL: CARMA2_CHEAT 0x006b2598
br_pixelmap* gDuplicate_pixelmap;

// GLOBAL: CARMA2_HW 0x006aaa28
// GLOBAL: CARMA2_SW 0x006a4478
// GLOBAL: CARMA2_CHEAT 0x006b25a0
br_material* gDuplicate_material;

// GLOBAL: CARMA2_HW 0x006aaa24
// GLOBAL: CARMA2_SW 0x006a4474
// GLOBAL: CARMA2_CHEAT 0x006b259c
br_model* gDuplicate_model;

// GLOBAL: CARMA2_HW 0x00660cb8
// GLOBAL: CARMA2_SW 0x0065b62c
// GLOBAL: CARMA2_CHEAT 0x00668500
tRendererShadingType gMaterial_shading_for_callback = kRendererShadingType_Undefined;

// FUNCTION: CARMA2_HW 0x00504b30
// FUNCTION: CARMA2_SW 0x005002f0
void C2_HOOK_FASTCALL InitTreeSurgery(void) {
    tPath_name the_path;
    FILE* file;
    int i;

    PathCat(the_path, gApplication_path, "TreeSurgery.TXT");
    file = PFfopen(the_path, "rt");
    if (file != NULL) {
        gTree_surgery_pass1_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass1_count; i++) {
            GetAString(file, gTree_surgery_pass1[i].name);
        }
        gTree_surgery_pass2_count = GetAnInt(file);
        for (i = 0; i < gTree_surgery_pass2_count; i++) {
            GetAString(file, gTree_surgery_pass2[i].original);
            GetAString(file, gTree_surgery_pass2[i].replacement);
        }
#ifdef REC2_FIX_BUGS
        PFfclose(file);
#endif
    } else {
        gTree_surgery_pass1_count = 0;
        gTree_surgery_pass2_count = 0;
    }
}

// ModelIsATree

// PerformTreeSurgery

// PossibleTreeSurgery

// AddExceptionToList

// LoadExceptionsFile

// LoadExceptionsFileForTrack

// FreeExceptions

// GetFaceFlag

// MungeTrackModel

// LoadTrack

// FUNCTION: CARMA2_HW 0x00506e20
// FUNCTION: CARMA2_SW 0x00502270
// FUNCTION: CARMA2_CHEAT 0x0050c810
intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, type, 0x12);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, type_data, 0x5c);
    (void)pArg;

    if (pActor->type == BR_ACTOR_BOUNDS || pActor->type == BR_ACTOR_BOUNDS_CORRECT) {
        BrResFree(pActor->type_data);
        pActor->type_data = NULL;
    }
    return 0;
}

// RemoveBoundsStructures

// STUB: CARMA2_HW 0x00506c20
// STUB: CARMA2_SW 0x00502070
// STUB: CARMA2_CHEAT 0x0050c610
void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00506e50
// FUNCTION: CARMA2_SW 0x005022a0
// FUNCTION: CARMA2_CHEAT 0x0050c840
void C2_HOOK_FASTCALL ProcessTrack(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world) {

    RenderTrack(pWorld, pTrack_spec, pCamera, pCamera_to_world);
    if (gAdditional_actors != NULL) {
        BrZbsSceneRenderAdd(gAdditional_actors);
    }
}

// AnimateSky

// CombineBounds

// CheckNonCarModelBounds

// FUNCTION: CARMA2_HW 0x00500d50
// FUNCTION: CARMA2_SW 0x004fc6b0
// FUNCTION: CARMA2_CHEAT 0x00506a00
void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds) {

    pStorage_space->pixelmaps_count = 0;
    pStorage_space->shade_tables_count = 0;
    pStorage_space->materials_count = 0;
    pStorage_space->models_count = 0;
    pStorage_space->sounds_count = 0;
    pStorage_space->flags = pUnknown;
    pStorage_space->max_pixelmaps = pMax_pixelmaps;
    pStorage_space->max_shade_tables = pMax_shade_tables;
    pStorage_space->max_materials = pMax_materials;
    pStorage_space->max_models = pMax_models;
    pStorage_space->max_sounds = pMax_sounds;
    pStorage_space->pixelmaps = BrMemCalloc(pMax_pixelmaps, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->shade_tables = BrMemCalloc(pMax_shade_tables, sizeof(br_pixelmap*), kMem_stor_space_table);
    pStorage_space->materials = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
    pStorage_space->models = BrMemCalloc(pMax_models, sizeof(br_model*), kMem_stor_space_table);
    pStorage_space->sounds = BrMemCalloc(pMax_sounds, sizeof(int), kMem_stor_space_table);
    pStorage_space->materialProps = BrMemCalloc(pMax_materials, sizeof(br_material*), kMem_stor_space_table);
}

// FUNCTION: CARMA2_HW 0x00500e10
// FUNCTION: CARMA2_SW 0x004fc770
// FUNCTION: CARMA2_CHEAT 0x00506ac0
void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage) {

    BrMemFree(pStorage->pixelmaps);
    BrMemFree(pStorage->shade_tables);
    BrMemFree(pStorage->materials);
    BrMemFree(pStorage->models);
    BrMemFree(pStorage->sounds);
    BrMemFree(pStorage->materialProps);
}

// FUNCTION: CARMA2_HW 0x00500e60
// FUNCTION: CARMA2_SW 0x004fc7c0
// FUNCTION: CARMA2_CHEAT 0x00506b10
void C2_HOOK_FASTCALL ClearMatertrialSetFromStorageSpace(tBrender_storage* pStorage_space, int pStart, int pEnd) {
    int i;
    int move_to;

    for (i = pStart; i < pEnd && i < pStorage_space->materials_count; i++) {
        BrMaterialRemove(pStorage_space->materials[i]);
        BrMaterialFree(pStorage_space->materials[i]);
        RemoveMaterialFromFogification(pStorage_space->materials[i]);
    }
    for (move_to = pStart, i = pEnd; i < pStorage_space->materials_count; ) {
        pStorage_space->materials[move_to++] = pStorage_space->materials[i++];
    }
    pStorage_space->materials_count = move_to;
    if (pStorage_space == &gNet_cars_storage_space) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            tCar_spec* car = gNet_players[i].car;
            if (car != NULL && car->old_material_count >= pEnd) {
                car->old_material_count += pStart - pEnd;
                car->new_material_count += pStart - pEnd;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00500f30
// FUNCTION: CARMA2_SW 0x004fc890
// FUNCTION: CARMA2_CHEAT 0x00506be0
void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space) {
    int i;

    DRS3StopAllOutletSoundsExceptCDA();
    for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
        if (pStorage_space->pixelmaps[i] != NULL) {
            BrMapRemove(pStorage_space->pixelmaps[i]);
            BrPixelmapFree(pStorage_space->pixelmaps[i]);
        }
    }
    pStorage_space->pixelmaps_count = 0;
    for (i = 0; i < pStorage_space->shade_tables_count; i++) {
        if (pStorage_space->shade_tables[i] != NULL) {
            BrTableRemove(pStorage_space->shade_tables[i]);
            BrPixelmapFree(pStorage_space->shade_tables[i]);
        }
    }
    pStorage_space->shade_tables_count = 0;
    for (i = 0; i < pStorage_space->materials_count; i++) {
        if (pStorage_space->materials[i] != NULL) {
            BrMaterialRemove(pStorage_space->materials[i]);
            BrMaterialFree(pStorage_space->materials[i]);
        }
    }
    pStorage_space->materials_count = 0;
    for (i = 0; i < pStorage_space->models_count; i++) {
        if (pStorage_space->models[i] != NULL) {
            BrModelRemove(pStorage_space->models[i]);
            BrModelFree(pStorage_space->models[i]);
        }
    }
    pStorage_space->models_count = 0;
}

// FUNCTION: CARMA2_HW 0x00501020
// FUNCTION: CARMA2_SW 0x004fc980
// FUNCTION: CARMA2_CHEAT 0x00506cd0
tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm) {
    int i;

    gDuplicate_pixelmap = NULL;
    if (pStorage_space->pixelmaps_count < pStorage_space->max_pixelmaps) {
        for (i = 0; i < pStorage_space->pixelmaps_count; i++) {
            if (pStorage_space->pixelmaps[i]->identifier != NULL
                    && pThe_pm->identifier != NULL
                    && strcmp(pStorage_space->pixelmaps[i]->identifier, pThe_pm->identifier) == 0) {
                gDuplicate_pixelmap = pStorage_space->pixelmaps[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->pixelmaps[pStorage_space->pixelmaps_count] = pThe_pm;
        pStorage_space->pixelmaps_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x005010e0
// FUNCTION: CARMA2_SW 0x004fca40
// FUNCTION: CARMA2_CHEAT 0x00506d90
tAdd_to_storage_result C2_HOOK_FASTCALL AddShadeTableToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_st) {
    int i;

    if (pStorage_space->shade_tables_count < pStorage_space->max_shade_tables) {
        for (i = 0; i < pStorage_space->shade_tables_count; i++) {
            if (pStorage_space->shade_tables[i]->identifier != NULL
                && pThe_st->identifier != NULL
                && strcmp(pStorage_space->shade_tables[i]->identifier, pThe_st->identifier) == 0) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->shade_tables[pStorage_space->shade_tables_count] = pThe_st;
        pStorage_space->shade_tables_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501190
// FUNCTION: CARMA2_SW 0x004fcaf0
// FUNCTION: CARMA2_CHEAT 0x00506e40
inline tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat) {
    int i;

    gDuplicate_material = NULL;
    if (pStorage_space->materials_count < pStorage_space->max_materials) {
        for (i = 0; i < pStorage_space->materials_count; i++) {
            if (pStorage_space->materials[i]->identifier != NULL
                    && pThe_mat->identifier != NULL
                    && strcmp(pStorage_space->materials[i]->identifier, pThe_mat->identifier) == 0) {
                gDuplicate_material = pStorage_space->materials[i];
                return eStorage_duplicate;
            }
        }
        pStorage_space->materialProps[pStorage_space->materials_count] = 0; /* FIXME */
        pStorage_space->materials[pStorage_space->materials_count] = pThe_mat;
        pStorage_space->materials_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501260
// FUNCTION: CARMA2_SW 0x004fcbc0
// FUNCTION: CARMA2_CHEAT 0x00506f10
inline tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod) {
    int i;

    gDuplicate_model = NULL;
    if (pStorage_space->models_count < pStorage_space->max_models) {
        if (pStorage_space->flags & 0x1) { /* FIXME: add enum (0x1 -> eStorage_space_flags_No_duplicates*/
            for (i = 0; i < pStorage_space->models_count; i++) {
                if (pStorage_space->models[i] != NULL
                    && pStorage_space->models[i]->identifier != NULL
                        && pThe_mod->identifier != NULL
                        && strcmp(pStorage_space->models[i]->identifier, pThe_mod->identifier) == 0) {
                    gDuplicate_model = pStorage_space->models[i];
                    return eStorage_duplicate;
                }
            }
        }
        pStorage_space->models[pStorage_space->models_count] = pThe_mod;
        pStorage_space->models_count += 1;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501330
// FUNCTION: CARMA2_SW 0x004fcc90
// FUNCTION: CARMA2_CHEAT 0x00506fe0
tAdd_to_storage_result C2_HOOK_FASTCALL AddSoundToStorage(tBrender_storage* pStorage_space, int pSound_id) {
    int i;

    if (pStorage_space->sounds_count < pStorage_space->max_sounds) {
        for (i = 0; i < pStorage_space->sounds_count; i++) {
            if (pStorage_space->sounds[i] != 0 && pStorage_space->sounds[i] == pSound_id) {
                return eStorage_duplicate;
            }
        }
        pStorage_space->sounds[pStorage_space->sounds_count] = pSound_id;
        pStorage_space->sounds_count++;
        return eStorage_allocated;
    } else {
        return eStorage_not_enough_room;
    }
}

// FUNCTION: CARMA2_HW 0x00501380
// FUNCTION: CARMA2_SW 0x004fcce0
// FUNCTION: CARMA2_CHEAT 0x00507030
int C2_HOOK_FASTCALL LoadNPixelmaps(tBrender_storage* pStorage_space, FILE* pF, int pCount) {
    int i;
    int j;
    int new_ones;
    int total;
    char s[256];
    char* str;
    br_pixelmap* temp_array[500];

    total = 0;
    for (i = 0; i < pCount; i++) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        DoNotAllowOpenToFail();
        new_ones = DRLoadMultiplePix(str, temp_array, REC2_ASIZE(temp_array));
        if (new_ones == 0) {
            FatalError(kFatalError_CantLoadPixelmapFile_S, str);
        }
        for (j = 0; j < new_ones; j++) {
            if (temp_array[j] != NULL) {
                switch (AddPixelmapToStorage(pStorage_space, temp_array[j])) {
                case eStorage_allocated:
                    BrMapAdd(temp_array[j]);
                    total++;
                    break;
                case eStorage_duplicate:
                    if (gDisallow_duplicates) {
                        FatalError(kFatalError_DuplicatePixelmap_S, temp_array[j]->identifier);
                    } else {
                        BrPixelmapFree(temp_array[j]);
                    }
                    break;
                case eStorage_not_enough_room:
                    FatalError(kFatalError_InsufficientPixelmapSlots);
                    break;
                }
            }
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x005024f0
// FUNCTION: CARMA2_SW 0x004fde40
// FUNCTION: CARMA2_CHEAT 0x005081a0
int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path) {
    int i;
    int new_ones;
    int total;
    tPath_name path_dirname;
    char path_stem[32];
    br_pixelmap* temp_array[500];

    total = 0;
    if (gDisableTiffConversion) {
        SepDirAndFilename(path, path_dirname, path_stem);
        new_ones = LoadBunchOfPixies(path_dirname, path_stem, temp_array, REC2_ASIZE(temp_array));
    } else {
        new_ones = DRPixelmapLoadMany(path, temp_array, REC2_ASIZE(temp_array));
    }
    if (new_ones == 0) {
        FatalError(kFatalError_CantLoadPixelmapFile_S, path);
    }
    for (i = 0; i < new_ones; i++) {
        if (temp_array[i] != NULL) {
            EnsurePixelmapAllowed(temp_array[i], 1);
            switch (AddPixelmapToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMapAdd(temp_array[i]);
                total += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
                } else {
                    BrPixelmapFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientPixelmapSlots);
                break;
            }
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501560
// FUNCTION: CARMA2_SW 0x004fcec0
// FUNCTION: CARMA2_CHEAT 0x00507210
br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName) {
    br_pixelmap* map;
    tAdd_to_storage_result addResult;

    map = LoadPixelmap(pName);
    if (map == NULL) {
        return BrMapFind(pName);
    }

    addResult = AddPixelmapToStorage(pStorage, map);
    switch (addResult) {
    case eStorage_allocated:
        BrMapAdd(map);
        return map;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, map->identifier);
        } else {
            BrPixelmapFree(map);
        }
        return gDuplicate_pixelmap;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientPixelmapSlots);
        break;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x005016a0
// FUNCTION: CARMA2_SW 0x004fd000
// FUNCTION: CARMA2_CHEAT 0x00507350
br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName) {
    br_pixelmap* temp;

    temp = LoadShadeTable(pName);
    if (temp == NULL) {
        return BrTableFind(pName);
    }

    switch (AddShadeTableToStorage(pStorage_space, temp)) {
    case eStorage_allocated:
        BrTableAdd(temp);
        return temp;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicatePixelmap_S, temp->identifier);
        } else {
            BrPixelmapFree(temp);
        }
        return BrTableFind(pName);
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientShadeTableSlots);
        break;
    }

    return NULL;
}

// FUNCTION: CARMA2_HW 0x005017e0
// FUNCTION: CARMA2_SW 0x004fd140
// FUNCTION: CARMA2_CHEAT 0x00507490
br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName) {
    br_material* material;

    material = LoadMaterial(pName);
    if (material == NULL) {
        return BrMaterialFind(pName);
    }

    switch (AddMaterialToStorage(pStorage_space, material)) {
    case eStorage_allocated:
        BrMaterialAdd(material);
        return material;
    case eStorage_duplicate:
        if (gDisallow_duplicates) {
            FatalError(kFatalError_DuplicateMaterial_S, material->identifier);
        } else {
            BrMaterialFree(material);
        }
        return gDuplicate_material;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientMaterialSlots);
        return NULL;
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00501930
// FUNCTION: CARMA2_SW 0x004fd290
// FUNCTION: CARMA2_CHEAT 0x005075e0
tAdd_to_storage_result C2_HOOK_FASTCALL LoadSingleSound(tBrender_storage* pStorage_space, int pSound_id) {

    if (S3GetBufferDescription(pSound_id) != NULL) {
        return pSound_id;
    }

    switch (AddSoundToStorage(pStorage_space, pSound_id)) {

    case eStorage_duplicate:
        return pSound_id;

    case eStorage_allocated:
        S3LoadSample(pSound_id);
        return pSound_id;
    case eStorage_not_enough_room:
        FatalError(kFatalError_InsufficientSoundSlotsInStorageArea);
        break;
    }
    return 0;
}

inline int C2_HOOK_FASTCALL AddShadeTables(tBrender_storage* pStorage_space, const char* pPath) {
    br_pixelmap* temp_array[50];
    int new_ones;
    int total;
    int i;

    new_ones = 0;
    total = BrPixelmapLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadShadeTableFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddShadeTableToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            BrTableAdd(temp_array[i]);
            new_ones++;
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicatePixelmap_S, temp_array[i]->identifier);
            } else {
                BrPixelmapFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientShadeTableSlots);
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x005019c0
// FUNCTION: CARMA2_SW 0x004fd320
// FUNCTION: CARMA2_CHEAT 0x00507670
int C2_HOOK_FASTCALL LoadNShadeTables(tBrender_storage* pStorage_space, FILE* pF, int pCount) {
    tPath_name the_path;
    int i;
    int total;
    char s[256];
    char* str;

    total = 0;
    for (i = 0; i < pCount; i++) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        PathCat(the_path, gApplication_path, "SHADETAB");
        PathCat(the_path, the_path, str);
        total += AddShadeTables(pStorage_space, the_path);
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501bb0
// FUNCTION: CARMA2_SW 0x004fd510
// FUNCTION: CARMA2_CHEAT 0x00507860
int C2_HOOK_FASTCALL LoadNMaterials(tBrender_storage* pStorage_space, FILE* pF, int pCount, tRendererShadingType pShading) {
    tPath_name the_path;
    int i;
    int j;
    int new_ones;
    int total;
    char s[256];
    char* str;
    br_material* temp_array[500];

    total = 0;
    for (i = 0; i < pCount; i++) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        PathCat(the_path, gApplication_path, "MATERIAL");
        PathCat(the_path, the_path, str);
        new_ones = BrMaterialLoadMany(the_path, temp_array, REC2_ASIZE(temp_array));
        if (new_ones == 0) {
            FatalError(kFatalError_CannotLoadMaterialFileOrItIsEmpty_S, str);
        }
        GlorifyMaterial(temp_array, new_ones, pShading);
        for (j = 0; j < new_ones; j++) {
            if (temp_array[j] != NULL) {
                switch (AddMaterialToStorage(pStorage_space, temp_array[j])) {
                case eStorage_allocated:
                    BrMaterialAdd(temp_array[j]);
                    total++;
                    break;
                case eStorage_duplicate:
                    if (gDisallow_duplicates) {
                        FatalError(kFatalError_DuplicateMaterial_S, temp_array[j]->identifier);
                    } else {
                        BrMaterialFree(temp_array[j]);
                    }
                    break;
                case eStorage_not_enough_room:
                    FatalError(kFatalError_InsufficientMaterialSlots);
                    break;
                }
            }
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x00501dd0
// FUNCTION: CARMA2_SW 0x004fd730
// FUNCTION: CARMA2_CHEAT 0x00507a80
void C2_HOOK_FASTCALL LoadModelsFromLineInto(tBrender_storage* pStorage_space, FILE* pF) {
    tPath_name the_path;
    char s[256];
    char* str;

    PossibleService();
    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    PathCat(the_path, gApplication_path, "MODELS");
    PathCat(the_path, the_path, str);
    AddModels(pStorage_space, the_path);
}

// FUNCTION: CARMA2_HW 0x00501e40
// FUNCTION: CARMA2_SW 0x004fd7a0
// FUNCTION: CARMA2_CHEAT 0x00507af0
int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath) {
    int i;
    int new_ones;
    int total;
    br_model* temp_array[2000];

    new_ones = 0;
    total = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
#ifdef REC2_HW
    WhitenVertexRGB(temp_array, total);
#endif
    if (total == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < total; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage_space, temp_array[i])) {
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            temp_array[i]->flags &= ~BR_MODF_DONT_WELD;
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        case eStorage_duplicate:
            if (gDisallow_duplicates) {
                FatalError(kFatalError_DuplicateModel_S, temp_array[i]->identifier);
            } else {
                BrModelFree(temp_array[i]);
            }
            break;
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientModelSlots);
            break;
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00501fe0
// FUNCTION: CARMA2_SW 0x004fd930
// FUNCTION: CARMA2_CHEAT 0x00507c90
void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage* pStorage, FILE* pFile, tRendererShadingType pShading) {
    tPath_name the_path;
    char s[256];
    char* str;

    PossibleService();
    GetALineAndDontArgue(pFile, s);
    str = strtok(s, "\t ,/");
    PathCat(the_path, gApplication_path, "MATERIAL");
    PathCat(the_path, the_path, str);
    AddMaterials(pStorage, the_path, pShading);
}

// FUNCTION: CARMA2_HW 0x00502060
// FUNCTION: CARMA2_SW 0x004fd9b0
// FUNCTION: CARMA2_CHEAT 0x00507d10
int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading) {
    int i;
    int new_ones;
    int total;
    br_material* temp_array[500];

    new_ones = 0;
    total = BrMaterialLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    if (total == 0) {
        FatalError(kFatalError_CannotLoadMaterialFileOrItIsEmpty_S, pPath);
    }
    GlorifyMaterial(temp_array, total, pShading);
    for (i = 0; i < total; ++i) {
        if (temp_array[i] != NULL) {
            switch (AddMaterialToStorage(pStorage_space, temp_array[i])) {
            case eStorage_allocated:
                BrMaterialAdd(temp_array[i]);
                new_ones += 1;
                break;
            case eStorage_duplicate:
                if (gDisallow_duplicates) {
                    FatalError(kFatalError_DuplicateMaterial_S, temp_array[i]->identifier);
                } else {
                    BrMaterialFree(temp_array[i]);
                }
                break;
            case eStorage_not_enough_room:
                FatalError(kFatalError_InsufficientMaterialSlots);
                break;
            }
        }
    }
    return new_ones;
}

// FUNCTION: CARMA2_HW 0x00502210
// FUNCTION: CARMA2_SW 0x004fdb60
// FUNCTION: CARMA2_CHEAT 0x00507ec0
void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM) {

    BrResFree(pM->faces);
    BrResFree(pM->vertices);
    pM->nfaces = 0;
    pM->nvertices = 0;
    pM->faces = NULL;
    pM->vertices = NULL;
}

// FUNCTION: CARMA2_HW 0x00502240
// FUNCTION: CARMA2_SW 0x004fdb90
// FUNCTION: CARMA2_CHEAT 0x00507ef0
int C2_HOOK_FASTCALL LoadNTrackModels(tBrender_storage* pStorage_space, FILE* pF, int pCount) {
    tPath_name the_path;
    int i;
    int j;
    int new_ones;
    int total;
    char s[256];
    char* str;
    br_model* temp_array[2000];

    total = 0;
    for (i = 0; i < pCount; i++) {
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        PathCat(the_path, gApplication_path, "MODELS");
        PathCat(the_path, the_path, str);
        new_ones = BrModelLoadMany(the_path, temp_array, REC2_ASIZE(temp_array));
        if (new_ones == 0) {
            FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, str);
        }
        for (j = 0; j < new_ones; j++) {
            if (temp_array[j] != NULL) {
                switch (AddModelToStorage(pStorage_space, temp_array[j])) {
                case eStorage_allocated:
                    temp_array[j]->flags |= BR_MODF_UPDATEABLE;
                    if (gRoad_texturing_level == eRTL_none) {
                        ProcessModelFaceMaterials(temp_array[j], RoadPerspToUntex);
                    }
                    if (gWall_texturing_level == eWTL_linear) {
                        ProcessModelFaceMaterials(temp_array[j], WallPerspToLinear);
                    }
                    if (gWall_texturing_level == eWTL_none) {
                        ProcessModelFaceMaterials(temp_array[j], WallPerspToUntex);
                    }
                    BrModelAdd(temp_array[j]);
                    total++;
                    break;
                case eStorage_duplicate:
                    if (gDisallow_duplicates) {
                        FatalError(kFatalError_DuplicateModel_S, temp_array[j]->identifier);
                    } else {
                        BrModelFree(temp_array[j]);
                    }
                    break;
                case eStorage_not_enough_room:
                    FatalError(kFatalError_InsufficientModelSlots);
                    break;
                }
            }
        }
    }
    return total;
}

// FUNCTION: CARMA2_HW 0x005024b0
// FUNCTION: CARMA2_SW 0x004fde00
void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".PIX") != NULL) {
        AddPixelmaps(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502490
// FUNCTION: CARMA2_SW 0x004fdde0
// FUNCTION: CARMA2_CHEAT 0x00508140
void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAPixelmap);
}

// FUNCTION: CARMA2_HW 0x005026b0
// FUNCTION: CARMA2_SW 0x004fe000
// FUNCTION: CARMA2_CHEAT 0x00508360
int C2_HOOK_FASTCALL GetFileName(const char *pPath, void* pContext) {
    tPath_name path;
    tPath_name pathUpper;
    tPath_name dir_path;
    tPath_name stem_path;
    tName_list *list;

    list = pContext;
    strcpy(path, pPath);
    Uppercaseificate(pathUpper, path);
    if (strstr(pathUpper, ".PIX") == NULL
            && strstr(pathUpper, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(pathUpper, dir_path, stem_path);
    strcpy(list->items[list->size], stem_path);
    if (list->size < (int)REC2_ASIZE(list->items)) {
        list->size += 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00502780
// FUNCTION: CARMA2_SW 0x004fe0d0
// FUNCTION: CARMA2_CHEAT 0x00508430
int C2_HOOK_FASTCALL GetAdditionalFileName(const char* pPath, void* pContext) {
    tPath_name path;
    tPath_name upperPath;
    tPath_name dir_path;
    tPath_name stem_path;
    int alreadyInList;
    int i;
    tName_list* list;

    list = pContext;
    strcpy(path, pPath);
    Uppercaseificate(upperPath, path);

    if (strstr(upperPath, ".PIX") == NULL
            && strstr(upperPath, ".P16") == NULL
            && strstr(upperPath, ".P08") == NULL
            && strstr(upperPath, ".TIF") == NULL) {
        return 0;
    }
    SepDirAndFilename(upperPath, dir_path, stem_path);

    for (i = 0, alreadyInList = 0; i < list->size; i++) {
        if (strcmp(list->items[i], stem_path) == 0) {
            alreadyInList = 1;
            break;
        }
    }
    if (!alreadyInList) {
        strcpy(list->items[list->size], stem_path);
        if (list->size < (int)REC2_ASIZE(list->items)) {
            list->size += 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005028f0
// FUNCTION: CARMA2_SW 0x004fe240
// FUNCTION: CARMA2_CHEAT 0x00508590
void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path) {
    tPath_name pathCopy;
    tPath_name pixPath;
    tPath_name tifPath;
    tName_list list;
    int i;

    C2_HOOK_BUG_ON(sizeof(tBrender_storage) != 68);

    gStorage_for_callbacks = pStorage_space;
    // TwatPIX16(path);
    list.size = 0;
    strcpy(pathCopy, path);
    if (gDisableTiffConversion) {
        PFForEveryFile2(pathCopy, GetAdditionalFileName, &list);
    }
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFX");
        PFForEveryFile2(tifPath, GetFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX8");
    PFForEveryFile2(pixPath, GetAdditionalFileName, &list);
    if (!gDisableTiffConversion) {
        PathCat(tifPath, pathCopy, "TIFFRGB");
        PFForEveryFile2(tifPath, GetAdditionalFileName, &list);
    }
    PathCat(pixPath, pathCopy, "PIX16");
    PFForEveryFile2(pixPath, GetAdditionalFileName, &list);
    for (i = 0; i < list.size; i++) {
        PathCat(pathCopy, path, list.items[i]);
        AddPixelmaps(gStorage_for_callbacks, pathCopy);
    }
}

// FUNCTION: CARMA2_HW 0x00502a70
// FUNCTION: CARMA2_SW 0x004fe3c0
// FUNCTION: CARMA2_CHEAT 0x00508710
void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath) {
    char path[256];
    tRendererShadingType shading;

    if (gMaterial_shading_for_callback == kRendererShadingType_Undefined) {
        shading = kRendererShadingType_Default;
    } else {
        shading = gMaterial_shading_for_callback;
    }
    Uppercaseificate(path, pPath);
    if (strstr(path, ".MAT") != NULL) {
        AddMaterials(gStorage_for_callbacks, pPath, shading);
    }
}

// FUNCTION: CARMA2_HW 0x00502ad0
// FUNCTION: CARMA2_SW 0x004fe420
// FUNCTION: CARMA2_CHEAT 0x00508770
void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    gMaterial_shading_for_callback = pShading;
    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAMaterial);
    gMaterial_shading_for_callback = kRendererShadingType_Undefined;
}

// FUNCTION: CARMA2_HW 0x00502b20
// FUNCTION: CARMA2_SW 0x004fe470
void C2_HOOK_FASTCALL LoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (strstr(s, ".DAT") != NULL) {
        AddModels(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b00
// FUNCTION: CARMA2_SW 0x004fe450
// FUNCTION: CARMA2_CHEAT 0x005087a0
void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAModel);
}

// FUNCTION: CARMA2_HW 0x00502b80
// FUNCTION: CARMA2_SW 0x004fe4d0
void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath) {
    char path[256];

    Uppercaseificate(path, pPath);
    if (strstr(path, ".TAB") != NULL) {
        AddShadeTables(gStorage_for_callbacks, pPath);
    }
}

// FUNCTION: CARMA2_HW 0x00502b60
// FUNCTION: CARMA2_SW 0x004fe4b0
// FUNCTION: CARMA2_CHEAT 0x00508800
void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath) {

    gStorage_for_callbacks = pStorage;
    PFForEveryFile(pPath, LoadIfItsAShadeTable);
}

// FUNCTION: CARMA2_HW 0x00502cf0
// FUNCTION: CARMA2_SW 0x004fe640
// FUNCTION: CARMA2_CHEAT 0x00508990
void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading) {

    LoadAllShadeTablesInDirectory(pStorage, pPath);
    LoadAllPixelmapsInDirectory(pStorage, pPath);
    LoadAllImagesInDirectory(pStorage, pPath);
    LoadAllMaterialsInDirectory(pStorage, pPath, pShading);
    LoadAllModelsInDirectory(pStorage, pPath);
}

// FUNCTION: CARMA2_HW 0x00502d60
// FUNCTION: CARMA2_SW 0x004fe6b0
// FUNCTION: CARMA2_CHEAT 0x00508a00
void C2_HOOK_FASTCALL DisallowDuplicates(void) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00502d70
// FUNCTION: CARMA2_SW 0x004fe6c0
// FUNCTION: CARMA2_CHEAT 0x00508a10
void C2_HOOK_FASTCALL AllowDuplicates(void) {

    gDisallow_duplicates = 0;
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "09-funks.h"

#include "08-loading1.h"
#include "10-loading2.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "23-piping.h"
#include "36-trig.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00655b50
// GLOBAL: CARMA2_SW 0x00650a38
// GLOBAL: CARMA2_CHEAT 0x0065c3d0
const char* gFunk_type_names[5] = { "spin", "rock", "throb", "slither", "roll" };

// GLOBAL: CARMA2_HW 0x00655b68
// GLOBAL: CARMA2_SW 0x00650a50
// GLOBAL: CARMA2_CHEAT 0x0065c3e8
const char* gFunk_anim_names[4] = { "frames", "flic", "camera", "mirror" };

// GLOBAL: CARMA2_HW 0x00655b78
// GLOBAL: CARMA2_SW 0x00650a60
// GLOBAL: CARMA2_CHEAT 0x0065c3f8
const char* gTime_mode_names[2] = { "approximate", "accurate" };

// GLOBAL: CARMA2_HW 0x00655b80
// GLOBAL: CARMA2_SW 0x00650a68
// GLOBAL: CARMA2_CHEAT 0x0065c400
const char* gFunk_move_names[7] = { "linear", "harmonic", "flash", "controlled", "absolute", "continuous", "texturebits" };

// GLOBAL: CARMA2_HW 0x00655ba0
// GLOBAL: CARMA2_SW 0x00650a88
// GLOBAL: CARMA2_CHEAT 0x0065c420
const char* gFunk_nature_names[4] = { "constant", "distance", "lastlap", "otherlaps" };

// GLOBAL: CARMA2_HW 0x00655bb0
// GLOBAL: CARMA2_SW 0x00650a98
// GLOBAL: CARMA2_CHEAT 0x0065c430
const char* gGroove_nature_names[2] = { "constant", "distance" };

// GLOBAL: CARMA2_HW 0x00655bb8
// GLOBAL: CARMA2_SW 0x00650aa0
// GLOBAL: CARMA2_CHEAT 0x0065c438
const char* gAxis_names[3] = { "x", "y", "z" };

// GLOBAL: CARMA2_HW 0x00655bc8
// GLOBAL: CARMA2_SW 0x00650ab0
// GLOBAL: CARMA2_CHEAT 0x0065c448
const char* gLollipop_names[3] = { "xlollipop", "ylollipop", "zlollipop" };

// GLOBAL: CARMA2_HW 0x00655bd8
// GLOBAL: CARMA2_SW 0x00650ac0
// GLOBAL: CARMA2_CHEAT 0x0065c458
const char* gGroove_path_names[2] = { "straight", "circular" };

// GLOBAL: CARMA2_HW 0x00655be0
// GLOBAL: CARMA2_SW 0x00650ac8
// GLOBAL: CARMA2_CHEAT 0x0065c460
const char* gGroove_object_names[4] = { "spin", "rock", "throb", "shear" };

// GLOBAL: CARMA2_HW 0x00655bf0
// GLOBAL: CARMA2_SW 0x00650ad8
// GLOBAL: CARMA2_CHEAT 0x0065c470
const char* gCamera_animation_names[2] = { "static", "tracking" };

// GLOBAL: CARMA2_HW 0x0068b840
// GLOBAL: CARMA2_SW 0x00685da0
// GLOBAL: CARMA2_CHEAT 0x006930e8
br_scalar gSight_distance_squared;

// GLOBAL: CARMA2_HW 0x0068b84c
// GLOBAL: CARMA2_SW 0x00685dac
// GLOBAL: CARMA2_CHEAT 0x006930f4
tFunkotronic_spec* gFunkotronics_array;

// GLOBAL: CARMA2_HW 0x0068b850
// GLOBAL: CARMA2_SW 0x00685db0
// GLOBAL: CARMA2_CHEAT 0x006930f8
tGroovidelic_spec* gGroovidelics_array;

// GLOBAL: CARMA2_HW 0x0068b848
// GLOBAL: CARMA2_SW 0x00685da8
// GLOBAL: CARMA2_CHEAT 0x006930f0
int gGroovidelics_array_size;

// GLOBAL: CARMA2_HW 0x0068b830
// GLOBAL: CARMA2_SW 0x00685d90
// GLOBAL: CARMA2_CHEAT 0x006930d8
tFunk_temp_buffer* gFunk_temp_vertices;

// GLOBAL: CARMA2_HW 0x0068b838
// GLOBAL: CARMA2_SW 0x00685d98
// GLOBAL: CARMA2_CHEAT 0x006930e0
tU32 gPrevious_groove_times[2];

// GLOBAL: CARMA2_HW 0x00688b28
// GLOBAL: CARMA2_SW 0x00683088
// GLOBAL: CARMA2_CHEAT 0x006903d0
float gTemp;

// GLOBAL: CARMA2_HW 0x00688b30
// GLOBAL: CARMA2_SW 0x00683090
// GLOBAL: CARMA2_CHEAT 0x006903d8
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x0068b844
// GLOBAL: CARMA2_SW 0x00685da4
// GLOBAL: CARMA2_CHEAT 0x006930ec
int gFunkotronics_array_size;

// GLOBAL: CARMA2_HW 0x0068b854
// GLOBAL: CARMA2_SW 0x00685db4
// GLOBAL: CARMA2_CHEAT 0x006930fc
br_actor* gGroove_by_proxy_actor;

inline int C2_HOOK_FASTCALL CalculateTextureBitsValue(tFunk_texturebits* pTexturebits) {
    int i;
    int value;

    value = 0;
    for (i = 0; i < pTexturebits->count; i++) {
        if (pTexturebits->car->field_0x18cc & (1 << pTexturebits->bits[i])) {
            value += 1 << i;
        }
    }
    return value;
}

inline float C2_HOOK_FASTCALL MapSawToTriangle(float pNumber) {

    if (pNumber < 0.5) {
        return pNumber * 4.0f - 1.0f;
    } else {
        return 3.0f - pNumber * 4.0f;
    }
}

// FUNCTION: CARMA2_HW 0x00474880
// FUNCTION: CARMA2_SW 0x00473de0
// FUNCTION: CARMA2_CHEAT 0x00474c00
void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {

    gSight_distance_squared = (br_scalar)REC2_SQR(pYon * 1.02);
}

inline void C2_HOOK_FASTCALL AddFunkGrooveBinding(int pSlot_number, float* pPeriod_address) {

    if (pSlot_number < 0 || pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_DefinedRefNumOfControlledGoorvFunkOutOfRange);
    }
    gGroove_funk_bindings[pSlot_number].v = pPeriod_address;
    gGroove_funk_bindings[pSlot_number].field_0x4 = 0;
    *pPeriod_address = 0.0f;
}

// FUNCTION: CARMA2_HW 0x004748a0
// FUNCTION: CARMA2_CHEAT 0x00474c20
void C2_HOOK_FASTCALL ControlBoundFunkGroove(int pSlot_number, float pValue) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroove_funk_binding, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tGroove_funk_binding, field_0x4, 0x1);
    C2_HOOK_BUG_ON(sizeof(tGroove_funk_binding) != 0x8);
    C2_HOOK_BUG_ON(REC2_ASIZE(gGroove_funk_bindings) != 1440);

    if (pSlot_number < 0) {
        return;
    }
    if (pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = pValue;
    }
}

// ControlBoundFunkGroovePlus

inline void C2_HOOK_FASTCALL ShiftBoundGrooveFunks(char* pStart, char* pEnd, ptrdiff_t pDelta) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gGroove_funk_bindings); i++) {
        if ((intptr_t)pStart <= (intptr_t)gGroove_funk_bindings[i].v && (intptr_t)gGroove_funk_bindings[i].v < (intptr_t)pEnd) {
            gGroove_funk_bindings[i].v += pDelta / sizeof(float);
        }
    }
}

inline tFunkotronic_spec* C2_HOOK_FASTCALL AddNewFunkotronic(void) {
    tFunkotronic_spec* new_array;
    int i;

    for (i = 0; i < gFunkotronics_array_size; i++) {
        if (gFunkotronics_array[i].owner == -999) {
            memset(&gFunkotronics_array[i], 0, sizeof(tFunkotronic_spec));
            return &gFunkotronics_array[i];
        }
    }
    gFunkotronics_array_size += 16;
    new_array = BrMemCalloc(gFunkotronics_array_size, sizeof(tFunkotronic_spec), kMem_funk_spec);
    if (gFunkotronics_array != NULL) {
        memcpy(new_array, gFunkotronics_array, (gFunkotronics_array_size - 16) * sizeof(tFunkotronic_spec));
        ShiftBoundGrooveFunks(
            (char*)gFunkotronics_array,
            (char*)&gFunkotronics_array[gFunkotronics_array_size - 16],
            (char*)new_array - (char*)gFunkotronics_array);
        BrMemFree(gFunkotronics_array);
    }
    gFunkotronics_array = new_array;
    for (i = 0; i < 16; i++) {
        gFunkotronics_array[gFunkotronics_array_size + i - 16].owner = -999;
    }
    return &gFunkotronics_array[gFunkotronics_array_size - 16];
}

// STUB: CARMA2_HW 0x00474950
// STUB: CARMA2_SW 0x00473eb0
// STUB: CARMA2_CHEAT 0x00474cd0
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004761e0
// FUNCTION: CARMA2_SW 0x00475740
// FUNCTION: CARMA2_CHEAT 0x00476560
br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    int i;

    (void)pMat;
    the_funk = pData;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);

    if (pActor->model != NULL) {
        for (i = 0, the_face = pActor->model->faces; i < pActor->model->nfaces; i++, the_face++) {
            if (the_face->material == the_funk->material) {
                the_funk->proximity_count++;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00476230
// FUNCTION: CARMA2_SW 0x00475790
// FUNCTION: CARMA2_CHEAT 0x004765b0
br_uint_32 C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    int i;
    int j;
    br_vertex* vertex;

    (void)pMat;
    the_funk = pData;
    if (pActor->model != NULL) {
        the_face = pActor->model->faces;
        for (i = 0; i < pActor->model->nfaces; i++, the_face++) {
            if (the_face->material == the_funk->material) {
                for (j = 0; j < 3; j++) {
                    vertex = &pActor->model->vertices[the_face->vertices[j]];
                    the_funk->proximity_array[the_funk->proximity_count].v[j] = vertex->p;
                    gFunk_temp_vertices[the_funk->proximity_count].vertices[j] = vertex;
                }
                BrVector3Normalise(&the_funk->proximity_array[the_funk->proximity_count].n, &the_face->n);
                the_funk->proximity_array[the_funk->proximity_count].d = -the_face->d;
                the_funk->proximity_count++;
            }
        }
    }
    return 0;
}

inline br_material* C2_HOOK_FASTCALL TryThisFunkLink(tCar_crush_buffer_entry* pFunk_link, const char* pStr, tFunkotronic_spec* pFunk) {
    int i;

    if (pFunk_link == NULL) {
        return NULL;
    }
    for (i = 0; i < pFunk_link->count_smashables; i++) {
        if (strcmp(pFunk_link->smashables[i].material_name, pStr) == 0) {
            pFunk_link->smashables[i].funk = pFunk - gFunkotronics_array;
            return pFunk_link->smashables[i].funk_material;
        }
    }
    return NULL;
}

inline br_material* C2_HOOK_FASTCALL FindSmashableMaterial(const char* pStr, tFunkotronic_spec* pFunk, tCar_crush_buffer* pCar_crush_datas) {
    int i;
    br_material* material;

    for (i = 0; i < pCar_crush_datas->count_entries; i++) {
        material = TryThisFunkLink(pCar_crush_datas->entries[i], pStr, pFunk);
        if (material != NULL) {
            return material;
        }
    }
    return NULL;
}

#define SAFE_DIV(VAL) (gTemp = (VAL), gTemp != 0.0f ? 1000.0 / gTemp : 0.0)

#define READ_TEXTURE_BITS(DEST)                                                   \
    do {                                                                          \
        tFunk_texturebits* texture_bits;                                          \
        char texture_string[32];                                                  \
        int k;                                                                    \
        int l;                                                                    \
                                                                                  \
        texture_bits = BrMemAllocate(sizeof(tFunk_texturebits), kMem_funk_spec);  \
        (DEST) = texture_bits;                                                    \
        GetAString(pF, texture_string);                                           \
        texture_bits->count = (tU8)strlen(texture_string);                        \
        for (k = 0; k < texture_bits->count; k++) {                               \
            for (l = 0; l < 7; l++) {                                             \
                if (texture_string[k] == "THBVLRF"[l]) {                          \
                    texture_bits->bits[k] = (tU8)l;                               \
                    break;                                                        \
                }                                                                 \
            }                                                                     \
        }                                                                         \
        texture_bits->car = gCurrent_car_spec;                                    \
    } while (0)

// FUNCTION: CARMA2_HW 0x00474ac0
// FUNCTION: CARMA2_SW 0x00474020
// FUNCTION: CARMA2_CHEAT 0x00474e40
void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas) {
    char s[256];
    char* str;
    int first_time;
    int i;
    int j;
    int k;
    tFunkotronic_spec* the_funk;
    float speed1;
    float speed2;
    float x_0;
    float x_1;
    int d_0;
    int d_1;
    float a_min;
    float d_min;
    float s_min;
    float a_max;
    float d_max;
    float s_max;

    gFunk_temp_vertices = BrMemAllocate(500 * sizeof(tFunk_temp_buffer), BR_MEMORY_APPLICATION);
    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF FUNK") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT FUNK") != 0) {
                FatalError(kFatalError_FunkotronicFile);
            } else {
                GetALineAndDontArgue(pF, s);
            }
        }
        first_time = 0;
        str = strtok(s, "\t ,/");
        the_funk = AddNewFunkotronic();
        the_funk->owner = pOwner;
        if (pCar_crush_datas != NULL) {
            the_funk->material = FindSmashableMaterial(str, the_funk, pCar_crush_datas);
        }
        if (pCar_crush_datas == NULL || the_funk->material == NULL) {
            the_funk->material = BrMaterialFind(str);
            if (the_funk->material == NULL) {
                FatalError(kFatalError_CannotFindMainMaterialInFunkotronicFile_S, str);
            }
        }
        str = strtok(NULL, "\t ,/");
        if (str != NULL && strcmp(str, "multiple") == 0 && the_funk->material->identifier != NULL) {
            BrResFree(the_funk->material->identifier);
            the_funk->material->identifier = NULL;
        }
        the_funk->mode = GetALineAndInterpretCommand(pF, gFunk_nature_names, REC2_ASIZE(gFunk_nature_names));
        the_funk->proximity_count = 0;
        the_funk->proximity_array = NULL;
        if (the_funk->mode == eFunk_mode_distance) {
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, CalcProximities, the_funk);
            the_funk->proximity_array = BrMemAllocate(the_funk->proximity_count * sizeof(tFunk_proximity), kMem_funk_prox_array);
            the_funk->proximity_count = 0;
            DRActorEnumRecurseWithMat(gUniverse_actor, NULL, AddProximities, the_funk);
        }
        the_funk->matrix_mod_type = GetALineAndInterpretCommand(pF, gFunk_type_names, REC2_ASIZE(gFunk_type_names));
        the_funk->flags &= ~0x1;
        if (the_funk->matrix_mod_type != eMatrix_mod_none) {
            the_funk->matrix_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_funk->matrix_mod_type) {
        case eMatrix_mod_spin:
            if (the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_funk->matrix_mod_data.spin_info.period);
            } else if (the_funk->matrix_mode == eMove_texturebits) {
                READ_TEXTURE_BITS(the_funk->matrix_mod_data.texture_info.data);
            } else {
                the_funk->matrix_mod_data.spin_info.period = (float)SAFE_DIV(GetAScalar(pF));
            }
            break;
        case eMatrix_mod_rock:
            if (the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_funk->matrix_mod_data.rock_info.period);
            } else if (the_funk->matrix_mode == eMove_texturebits) {
                READ_TEXTURE_BITS(the_funk->matrix_mod_data.texture_info.data);
            } else {
                the_funk->matrix_mod_data.rock_info.period = (float)SAFE_DIV(GetAScalar(pF));
            }
            the_funk->matrix_mod_data.rock_info.rock_angle = GetAScalar(pF);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.rock_info.x_centre = x_0 * 0.01f;
            the_funk->matrix_mod_data.rock_info.y_centre = x_1 * 0.01f;
            break;
        case eMatrix_mod_throb:
            if (the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
                GetPairOfInts(pF, &d_0, &d_1);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(d_0 + pRef_offset, &the_funk->matrix_mod_data.throb_info.x_period);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(d_1 + pRef_offset, &the_funk->matrix_mod_data.throb_info.y_period);
                }
            } else {
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.throb_info.x_period = (float)SAFE_DIV(speed1);
                the_funk->matrix_mod_data.throb_info.y_period = (float)SAFE_DIV(speed2);
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.throb_info.x_magnitude,
                &the_funk->matrix_mod_data.throb_info.y_magnitude);
            GetPairOfFloats(pF, &x_0, &x_1);
            the_funk->matrix_mod_data.throb_info.x_centre = x_0 * 0.01f;
            the_funk->matrix_mod_data.throb_info.y_centre = x_1 * 0.01f;
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.throb_info.x_period == 0.0f) {
                    the_funk->matrix_mod_data.throb_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.x_magnitude = 0.0f;
                }
                if (the_funk->matrix_mod_data.throb_info.y_period == 0.0f) {
                    the_funk->matrix_mod_data.throb_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.throb_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_slither:
            if (the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
                GetPairOfInts(pF, &d_0, &d_1);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(d_0 + pRef_offset, &the_funk->matrix_mod_data.slither_info.x_period);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(d_1 + pRef_offset, &the_funk->matrix_mod_data.slither_info.y_period);
                }
            } else {
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.slither_info.x_period = (float)SAFE_DIV(speed1);
                the_funk->matrix_mod_data.slither_info.y_period = (float)SAFE_DIV(speed2);
            }
            GetPairOfFloatPercents(pF,
                &the_funk->matrix_mod_data.slither_info.x_magnitude,
                &the_funk->matrix_mod_data.slither_info.y_magnitude);
            if (the_funk->matrix_mode != eMove_controlled) {
                if (the_funk->matrix_mod_data.slither_info.x_period == 0.0f) {
                    the_funk->matrix_mod_data.slither_info.x_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.x_magnitude = 0.0f;
                }
                if (the_funk->matrix_mod_data.slither_info.y_period == 0.0f) {
                    the_funk->matrix_mod_data.slither_info.y_period = 1.0f;
                    the_funk->matrix_mod_data.slither_info.y_magnitude = 0.0f;
                }
            }
            break;
        case eMatrix_mod_roll:
            if (the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
                GetPairOfInts(pF, &d_0, &d_1);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(d_0 + pRef_offset, &the_funk->matrix_mod_data.roll_info.x_period);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(d_1 + pRef_offset, &the_funk->matrix_mod_data.roll_info.y_period);
                }
            } else {
                GetPairOfFloats(pF, &speed1, &speed2);
                the_funk->matrix_mod_data.roll_info.x_period = (float)SAFE_DIV(speed1);
                the_funk->matrix_mod_data.roll_info.y_period = (float)SAFE_DIV(speed2);
            }
            break;
        default:
            break;
        }
        the_funk->lighting_animation_type = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        if (the_funk->lighting_animation_type != eMove_none) {
            if (the_funk->lighting_animation_type == eMove_controlled || the_funk->lighting_animation_type == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_funk->lighting_animation_data.controlled.period);
            } else if (the_funk->lighting_animation_type == eMove_texturebits) {
                READ_TEXTURE_BITS(the_funk->lighting_animation_data.texture_info.data);
            } else {
                the_funk->lighting_animation_data.rock_info.period = (float)SAFE_DIV(GetAScalar(pF));
            }
            GetThreeFloatPercents(pF, &a_min, &d_min, &s_min);
            GetThreeFloatPercents(pF, &a_max, &d_max, &s_max);
            the_funk->ambient_base = (a_min + a_max) / 2.0f;
            the_funk->direct_base = (d_min + d_max) / 2.0f;
            the_funk->specular_base = (s_min + s_max) / 2.0f;
            the_funk->ambient_delta = (a_max - a_min) / 2.0f;
            the_funk->direct_delta = (d_max - d_min) / 2.0f;
            the_funk->specular_delta = (s_max - s_min) / 2.0f;
        }
        the_funk->texture_animation_type = GetALineAndInterpretCommand(pF, gFunk_anim_names, REC2_ASIZE(gFunk_anim_names));
        if (the_funk->texture_animation_type != eTexture_animation_none
                && the_funk->texture_animation_type != eTexture_animation_mirror
                && the_funk->texture_animation_type != eTexture_animation_camera) {
            the_funk->time_mode = GetALineAndInterpretCommand(pF, gTime_mode_names, REC2_ASIZE(gTime_mode_names));
        }
        if (the_funk->texture_animation_type == eTexture_animation_flic && gAusterity_mode) {
            the_funk->texture_animation_type = eTexture_animation_none;
            GetALineAndDontArgue(pF, s);
        }
        the_funk->last_frame = 0.0f;
        switch (the_funk->texture_animation_type) {
        case eTexture_animation_frames:
            the_funk->texture_animation_data.frames_info.mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
            if (the_funk->texture_animation_data.frames_info.mode == eMove_controlled
                    || the_funk->texture_animation_data.frames_info.mode == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_funk->texture_animation_data.frames_info.controlled.period);
            } else if (the_funk->texture_animation_data.frames_info.mode == eMove_texturebits) {
                READ_TEXTURE_BITS(the_funk->texture_animation_data.frames_info.texture_info.data);
            } else {
                the_funk->texture_animation_data.frames_info.rock_info.period = (float)SAFE_DIV(GetAScalar(pF));
            }
            the_funk->texture_animation_data.frames_info.texture_count = (int)GetAScalar(pF);
            the_funk->texture_animation_data.frames_info.current_frame = 0;
            the_funk->texture_animation_data.frames_info.has_matrix = 0;
            for (i = 0; i < the_funk->texture_animation_data.frames_info.texture_count; i++) {
                GetALineAndDontArgue(pF, s);
                str = strtok(s, "\t ,/");
                the_funk->texture_animation_data.frames_info.textures[i] = BrMapFind(str);
                if (the_funk->texture_animation_data.frames_info.textures[i] == NULL) {
                    FatalError(kFatalError_CannotFindAnimationFramePixelmapReferencedInFunkotronicFile);
                }
                BrMatrix23Identity(&the_funk->texture_animation_data.frames_info.mat[i]);
                str = strtok(NULL, "\t ,/");
                if (str != NULL && strlen(str) != 0) {
                    int size_x;
                    int pos_x;
                    int size_y;
                    int pos_y;

                    the_funk->texture_animation_data.frames_info.has_matrix = 1;
                    sscanf(str, "%d", &size_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_x);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &size_y);
                    str = strtok(NULL, "\t ,/");
                    sscanf(str, "%d", &pos_y);
                    the_funk->texture_animation_data.frames_info.mat[i].m[0][0] = (br_scalar)(1.0 / size_x);
                    the_funk->texture_animation_data.frames_info.mat[i].m[1][1] = (br_scalar)(1.0 / size_y);
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][0] = (br_scalar)pos_x / size_x;
                    the_funk->texture_animation_data.frames_info.mat[i].m[2][1] = (br_scalar)pos_y / size_y;
                }
            }
            break;
        case eTexture_animation_flic:
            GetAString(pF, s);
            the_funk->texture_animation_data.flic_info.flic_data = NULL;
            if (!LoadFlicData(s, &the_funk->texture_animation_data.flic_info.flic_data, &the_funk->texture_animation_data.flic_info.flic_data_length)) {
                the_funk->texture_animation_type = eTexture_animation_none;
            } else {
                void* the_pixels;
                br_pixelmap* the_pixelmap;

                the_funk->texture_animation_data.flic_info.flic_descriptor.data_start = NULL;
                StartFlic(s,
                    -1,
                    &the_funk->texture_animation_data.flic_info.flic_descriptor,
                    the_funk->texture_animation_data.flic_info.flic_data_length,
                    (tS8*)the_funk->texture_animation_data.flic_info.flic_data,
                    NULL,
                    0,
                    0,
                    0);
                the_funk->last_frame = 0.0f;
                the_pixels = BrMemAllocate(
                    the_funk->texture_animation_data.flic_info.flic_descriptor.height
                        * ((the_funk->texture_animation_data.flic_info.flic_descriptor.width + 3) & ~3),
                    kMem_video_pixels);
                if (gScreen->row_bytes < 0) {
                    BrFatal("C:\\Carma2\\Source\\Common\\Funkgroo.c", 732,
                        "Bruce bug at line %d, file C:\\Carma2\\Source\\Common\\Funkgroo.c", 732);
                }
                the_pixelmap = DRPixelmapAllocate(gScreen->type,
                    (br_uint_16)the_funk->texture_animation_data.flic_info.flic_descriptor.width,
                    (br_uint_16)the_funk->texture_animation_data.flic_info.flic_descriptor.height,
                    the_pixels,
                    0);
                AssertFlicPixelmap(&the_funk->texture_animation_data.flic_info.flic_descriptor, the_pixelmap);
                the_funk->material->colour_map = the_pixelmap;
                the_funk->material->user = the_pixelmap;
                BrMaterialUpdate(the_funk->material, BR_MATU_ALL);
            }
            break;
        case eTexture_animation_camera:
            {
                float yon_factor;
                float fov_factor;

                the_funk->texture_animation_data.camera_info.mode = GetALineAndInterpretCommand(pF, gCamera_animation_names, REC2_ASIZE(gCamera_animation_names));
                GetPairOfFloats(pF, &yon_factor, &fov_factor);
                the_funk->texture_animation_data.camera_info.field_0x60 = GetAnInt(pF);
                the_funk->texture_animation_data.camera_info.count = GetAnInt(pF);
                for (i = 0; i < the_funk->texture_animation_data.camera_info.count; i++) {
                    br_actor* actor;
                    br_camera* camera;
                    br_camera* global_camera;
                    br_vector3 cam_pos;

                    the_funk->texture_animation_data.camera_info.actors[i] = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                    actor = the_funk->texture_animation_data.camera_info.actors[i];
                    if (actor == NULL) {
                        FatalError(kFatalError_CannotAllocateCamera);
                    }
                    camera = actor->type_data;
                    global_camera = gCamera->type_data;
                    camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                    camera->field_of_view = (br_angle)(global_camera->field_of_view * fov_factor);
                    camera->hither_z = global_camera->hither_z;
                    camera->yon_z = global_camera->yon_z * yon_factor;
                    actor = BrActorAdd(gUniverse_actor, actor);
                    if (actor == NULL) {
                        FatalError(kFatalError_CannotAllocateCamera);
                    }
                    GetThreeFloats(pF, &cam_pos.v[0], &cam_pos.v[1], &cam_pos.v[2]);
                    if (the_funk->texture_animation_data.camera_info.mode == eFunk_camera_static) {
                        br_euler e;
                        float angle1;
                        float angle2;

                        GetPairOfFloats(pF, &angle1, &angle2);
                        e.order = 0x12;
                        e.a = BrDegreeToAngle(angle1);
                        e.b = BrDegreeToAngle(360.0f - angle2);
                        e.c = 0;
                        BrEulerToMatrix34(&actor->t.t.mat, &e);
                    }
                    BrVector3Copy(&actor->t.t.translate.t, &cam_pos);
                }
            }
            break;
        case eTexture_animation_mirror:
            if (the_funk->proximity_count == 0) {
                the_funk->texture_animation_data.mirror_info.actor = NULL;
                GetALineAndDontArgue(pF, s);
                GetALineAndDontArgue(pF, s);
            } else {
                br_actor* actor;
                br_camera* camera;
                br_camera* global_camera;
                br_vector3 n;
                br_matrix34 mat;

                global_camera = gCamera->type_data;
                the_funk->texture_animation_data.mirror_info.yon = GetAScalar(pF) * global_camera->yon_z;
                the_funk->texture_animation_data.mirror_info.field_0x5c = GetAnInt(pF);
                the_funk->texture_animation_data.mirror_info.actor = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
                actor = the_funk->texture_animation_data.mirror_info.actor;
                if (actor == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                camera = actor->type_data;
                camera->type = BR_CAMERA_PERSPECTIVE_FOV;
                camera->field_of_view = global_camera->field_of_view;
                camera->hither_z = global_camera->hither_z;
                camera->yon_z = 2.0f * global_camera->yon_z;
                if (BrActorAdd(gUniverse_actor, actor) == NULL) {
                    FatalError(kFatalError_CannotAllocateCamera);
                }
                n = the_funk->proximity_array->n;
                the_funk->texture_animation_data.mirror_info.mat.m[0][0] = 1.0f - 2.0f * n.v[0] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[0][1] = 2.0f * n.v[0] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[0][2] = 2.0f * n.v[0] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[1][0] = 2.0f * n.v[1] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[1][1] = 1.0f - 2.0f * n.v[1] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[1][2] = 2.0f * n.v[1] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[2][0] = 2.0f * n.v[2] * n.v[0];
                the_funk->texture_animation_data.mirror_info.mat.m[2][1] = 2.0f * n.v[2] * n.v[1];
                the_funk->texture_animation_data.mirror_info.mat.m[2][2] = 1.0f - 2.0f * n.v[2] * n.v[2];
                the_funk->texture_animation_data.mirror_info.mat.m[3][0] = n.v[0] * the_funk->proximity_array->d * -2.0f;
                the_funk->texture_animation_data.mirror_info.mat.m[3][1] = n.v[1] * the_funk->proximity_array->d * -2.0f;
                the_funk->texture_animation_data.mirror_info.mat.m[3][2] = n.v[2] * the_funk->proximity_array->d * -2.0f;
                BrVector3Sub((br_vector3*)mat.m[0], &the_funk->proximity_array->v[1], &the_funk->proximity_array->v[0]);
                BrVector3Copy((br_vector3*)mat.m[1], &the_funk->proximity_array->n);
                BrVector3Cross((br_vector3*)mat.m[2], (br_vector3*)mat.m[0], (br_vector3*)mat.m[1]);
                /* FIXME: the original never sets mat.m[3] before using mat */
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.min, 2147483648.0f, 2147483648.0f, 2147483648.0f);
                BrVector3SetFloat(&the_funk->texture_animation_data.mirror_info.bounds.max, -2147483648.0f, -2147483648.0f, -2147483648.0f);
                for (i = 0; i < the_funk->proximity_count; i++) {
                    for (j = 0; j < 3; j++) {
                        /* The original always transforms the vertices of the first proximity triangle. */
                        BrMatrix34ApplyP(&gFunk_temp_vertices[i].points[j], &the_funk->proximity_array->v[j], &mat);
                        for (k = 0; k < 3; k++) {
                            if (gFunk_temp_vertices[i].points[j].v[k] < the_funk->texture_animation_data.mirror_info.bounds.min.v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (gFunk_temp_vertices[i].points[j].v[k] > the_funk->texture_animation_data.mirror_info.bounds.max.v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
                for (i = 0; i < the_funk->proximity_count; i++) {
                    for (j = 0; j < 3; j++) {
                        gFunk_temp_vertices[i].vertices[j]->map.v[0] =
                            (gFunk_temp_vertices[i].points[j].v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0])
                            / (the_funk->texture_animation_data.mirror_info.bounds.max.v[0] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
#ifdef REC2_FIX_BUGS
                        gFunk_temp_vertices[i].vertices[j]->map.v[1] =
                            (gFunk_temp_vertices[i].points[j].v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2])
                            / (the_funk->texture_animation_data.mirror_info.bounds.max.v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2]);
#else
                        gFunk_temp_vertices[i].vertices[j]->map.v[1] =
                            (gFunk_temp_vertices[i].points[j].v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[2])
                            / (the_funk->texture_animation_data.mirror_info.bounds.max.v[2] - the_funk->texture_animation_data.mirror_info.bounds.min.v[0]);
#endif
                        for (k = 0; k < 3; k++) {
                            if (gFunk_temp_vertices[i].points[j].v[k] < the_funk->texture_animation_data.mirror_info.bounds.min.v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.min.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                            if (gFunk_temp_vertices[i].points[j].v[k] > the_funk->texture_animation_data.mirror_info.bounds.max.v[k]) {
                                the_funk->texture_animation_data.mirror_info.bounds.max.v[k] = gFunk_temp_vertices[i].points[j].v[k];
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    BrMemFree(gFunk_temp_vertices);
}

// FUNCTION: CARMA2_HW 0x00476430
// FUNCTION: CARMA2_SW 0x00475990
// FUNCTION: CARMA2_CHEAT 0x004767b0
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    tGroovidelic_spec* the_groove;
    int i;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            PossibleService();
            if (the_groove->owner == pOwner) {
                the_groove->owner = -999;
            }
        }
    }
}

inline tGroovidelic_spec* C2_HOOK_FASTCALL AddNewGroovidelic(void) {
    tGroovidelic_spec* new_array;
    int i;

    for (i = 0; i < gGroovidelics_array_size; i++) {
        if (gGroovidelics_array[i].owner == -999) {
            memset(&gGroovidelics_array[i], 0, sizeof(tGroovidelic_spec));
            return &gGroovidelics_array[i];
        }
    }
    gGroovidelics_array_size += 16;
    new_array = BrMemCalloc(gGroovidelics_array_size, sizeof(tGroovidelic_spec), kMem_groove_spec);
    if (gGroovidelics_array != NULL) {
        memcpy(new_array, gGroovidelics_array, (gGroovidelics_array_size - 16) * sizeof(tGroovidelic_spec));
        ShiftBoundGrooveFunks(
            (char*)gGroovidelics_array,
            (char*)&gGroovidelics_array[gGroovidelics_array_size - 16],
            (char*)new_array - (char*)gGroovidelics_array);
        BrMemFree(gGroovidelics_array);
    }
    gGroovidelics_array = new_array;
    for (i = 0; i < 16; i++) {
        gGroovidelics_array[gGroovidelics_array_size + i - 16].owner = -999;
    }
    return &gGroovidelics_array[gGroovidelics_array_size - 16];
}

// FUNCTION: CARMA2_HW 0x00476470
// FUNCTION: CARMA2_SW 0x004759d0
// FUNCTION: CARMA2_CHEAT 0x004767f0
void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent) {
    char s[256];
    char* str;
    int first_time;
    tGroovidelic_spec* the_groove;
    br_vector3 p;
    float x_0;
    float x_1;
    float x_2;
    int d_0;
    int d_1;
    int d_2;

    first_time = 1;
    while (!PFfeof(pF)) {
        PossibleService();
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "END OF GROOVE") == 0) {
            break;
        }
        if (!first_time) {
            if (strcmp(s, "NEXT GROOVE") != 0) {
                FatalError(kFatalError_ErrorWithinGroovidelicFile);
            } else {
                GetALineAndDontArgue(pF, s);
            }
        }
        first_time = 0;
        str = strtok(s, "\t ,/");
        the_groove = AddNewGroovidelic();
        the_groove->owner = pOwner;
        the_groove->block_flags = 0;
        the_groove->actor = DRActorFindRecurse(pParent_actor, str);
        if (the_groove->actor == NULL) {
            if (!pAllowed_to_be_absent && !gAusterity_mode) {
                FatalError(kFatalError_CannotFindActorReferencedInGroovidelicFile_S, str);
            } else {
                if (gGroove_by_proxy_actor == NULL) {
                    gGroove_by_proxy_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
                    gGroove_by_proxy_actor->model = LoadModel("PROXY.DAT");
                    BrModelAdd(gGroove_by_proxy_actor->model);
                    BrActorAdd(gDont_render_actor, gGroove_by_proxy_actor);
                }
                the_groove->actor = gGroove_by_proxy_actor;
            }
        }
        the_groove->lollipop_mode = GetALineAndInterpretCommand(pF, gLollipop_names, REC2_ASIZE(gLollipop_names));
        the_groove->mode = GetALineAndInterpretCommand(pF, gGroove_nature_names, REC2_ASIZE(gGroove_nature_names));
        the_groove->path_type = GetALineAndInterpretCommand(pF, gGroove_path_names, REC2_ASIZE(gGroove_path_names));
        the_groove->path_interrupt_status = eInterrupt_none;
        the_groove->object_interrupt_status = eInterrupt_none;
        if (the_groove->path_type != eGroove_path_none) {
            the_groove->path_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->path_type) {
        case eGroove_path_circular:
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->path_data.circular_info.centre = p;
#ifdef REC2_FIX_BUGS
            if (Vector3IsZero(&the_groove->path_data.circular_info.centre)) {
                the_groove->path_data.circular_info.centre = the_groove->actor->t.t.translate.t;
            }
#else
            /* FIXME: the original tests and fills in the centre of the straight path here */
            if (Vector3IsZero(&the_groove->path_data.straight_info.centre)) {
                the_groove->path_data.straight_info.centre = the_groove->actor->t.t.translate.t;
            }
#endif
            if (the_groove->path_mode == eMove_controlled || the_groove->path_mode == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_groove->path_data.circular_info.period.value);
            } else if (the_groove->path_mode == eMove_texturebits) {
                READ_TEXTURE_BITS(the_groove->path_data.circular_info.texture_info.data);
            } else {
                the_groove->path_data.circular_info.period.value = (float)SAFE_DIV(GetAScalar(pF));
            }
            the_groove->path_data.circular_info.radius = GetAScalar(pF);
            the_groove->path_data.circular_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            break;
        case eGroove_path_straight:
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->path_data.straight_info.centre = p;
            if (Vector3IsZero(&the_groove->path_data.straight_info.centre)) {
                the_groove->path_data.straight_info.centre = the_groove->actor->t.t.translate.t;
            }
            if (the_groove->path_mode == eMove_controlled || the_groove->path_mode == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_groove->path_data.straight_info.period.value);
            } else if (the_groove->path_mode == eMove_texturebits) {
                READ_TEXTURE_BITS(the_groove->path_data.straight_info.texture_info.data);
            } else {
                the_groove->path_data.straight_info.period.value = (float)SAFE_DIV(GetAScalar(pF));
            }
            GetThreeFloats(pF,
                &the_groove->path_data.straight_info.x_delta,
                &the_groove->path_data.straight_info.y_delta,
                &the_groove->path_data.straight_info.z_delta);
            break;
        default:
            break;
        }
        the_groove->object_type = GetALineAndInterpretCommand(pF, gGroove_object_names, REC2_ASIZE(gGroove_object_names));
        the_groove->object_position = the_groove->actor->t.t.translate.t;
        if (the_groove->object_type != eGroove_object_none) {
            the_groove->object_mode = GetALineAndInterpretCommand(pF, gFunk_move_names, REC2_ASIZE(gFunk_move_names));
        }
        switch (the_groove->object_type) {
        case eGroove_object_spin:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_groove->object_data.spin_info.period.value);
            } else if (the_groove->object_mode == eMove_texturebits) {
                READ_TEXTURE_BITS(the_groove->object_data.spin_info.texture_info.data);
            } else {
                the_groove->object_data.spin_info.period.value = (float)SAFE_DIV(GetAScalar(pF));
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            the_groove->object_data.spin_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            break;
        case eGroove_object_rock:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                AddFunkGrooveBinding(GetAnInt(pF) + pRef_offset, &the_groove->object_data.rock_info.period.value);
            } else if (the_groove->object_mode == eMove_texturebits) {
                READ_TEXTURE_BITS(the_groove->object_data.rock_info.texture_info.data);
            } else {
                the_groove->object_data.rock_info.period.value = (float)SAFE_DIV(GetAScalar(pF));
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            the_groove->object_data.rock_info.axis = GetALineAndInterpretCommand(pF, gAxis_names, REC2_ASIZE(gAxis_names));
            the_groove->object_data.rock_info.max_angle = GetAScalar(pF);
            break;
        case eGroove_object_throb:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(d_0 + pRef_offset, &the_groove->object_data.throb_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(d_1 + pRef_offset, &the_groove->object_data.throb_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(d_2 + pRef_offset, &the_groove->object_data.throb_info.z_period.value);
                }
            } else {
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.throb_info.x_period.value = (float)SAFE_DIV(x_0);
                the_groove->object_data.throb_info.y_period.value = (float)SAFE_DIV(x_1);
                the_groove->object_data.throb_info.z_period.value = (float)SAFE_DIV(x_2);
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            GetThreeFloatPercents(pF,
                &the_groove->object_data.throb_info.x_magnitude,
                &the_groove->object_data.throb_info.y_magnitude,
                &the_groove->object_data.throb_info.z_magnitude);
            break;
        case eGroove_object_shear:
            if (the_groove->object_mode == eMove_controlled || the_groove->object_mode == eMove_absolute) {
                GetThreeInts(pF, &d_0, &d_1, &d_2);
                if (d_0 >= 0) {
                    AddFunkGrooveBinding(d_0 + pRef_offset, &the_groove->object_data.shear_info.x_period.value);
                }
                if (d_1 >= 0) {
                    AddFunkGrooveBinding(d_1 + pRef_offset, &the_groove->object_data.shear_info.y_period.value);
                }
                if (d_2 >= 0) {
                    AddFunkGrooveBinding(d_2 + pRef_offset, &the_groove->object_data.shear_info.z_period.value);
                }
            } else {
                GetThreeFloats(pF, &x_0, &x_1, &x_2);
                the_groove->object_data.shear_info.x_period.value = (float)SAFE_DIV(x_0);
                the_groove->object_data.shear_info.y_period.value = (float)SAFE_DIV(x_1);
                the_groove->object_data.shear_info.z_period.value = (float)SAFE_DIV(x_2);
            }
            GetThreeFloats(pF, &p.v[0], &p.v[1], &p.v[2]);
            the_groove->object_centre = p;
            GetThreeFloatPercents(pF,
                &the_groove->object_data.shear_info.x_magnitude,
                &the_groove->object_data.shear_info.y_magnitude,
                &the_groove->object_data.shear_info.z_magnitude);
            break;
        default:
            break;
        }
    }
}

#undef READ_TEXTURE_BITS
#undef SAFE_DIV

// FUNCTION: CARMA2_HW 0x00477160
// FUNCTION: CARMA2_SW 0x004766c0
// FUNCTION: CARMA2_CHEAT 0x004774e0
void C2_HOOK_FASTCALL KillGroovadelic(int pOwner) {
    int i;
    tGroovidelic_spec* the_groove;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++) {
            if (the_groove[i].owner == pOwner
                && the_groove[i].path_mode != eMove_controlled
                && the_groove[i].path_mode != eMove_absolute
                && the_groove[i].object_mode != eMove_controlled
                && the_groove[i].object_mode != eMove_absolute) {
                the_groove[i].owner = -999;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004771c0
// FUNCTION: CARMA2_SW 0x00476720
// FUNCTION: CARMA2_CHEAT 0x00477540
void C2_HOOK_FASTCALL KillFunkotronic(int pOwner) {
    int i;
    tFunkotronic_spec* the_funk;

    if (gFunkotronics_array != NULL) {
        for (i = 0, the_funk = gFunkotronics_array; i < gFunkotronics_array_size; i++) {
            if (the_funk[i].owner == pOwner
                && the_funk[i].matrix_mode != eMove_controlled
                && the_funk[i].matrix_mode != eMove_absolute
                && the_funk[i].lighting_animation_type != eMove_controlled
                && the_funk[i].lighting_animation_type != eMove_absolute
                && ((the_funk[i].texture_animation_data.frames_info.mode != eMove_controlled
                        && the_funk[i].texture_animation_data.frames_info.mode != eMove_texturebits)
                    || the_funk[i].texture_animation_type != eTexture_animation_frames)) {
                the_funk[i].owner = -999;
            }
        }
    }
}

inline br_scalar C2_HOOK_FASTCALL NormaliseDegreeAngle(br_scalar pAngle) {

    while (pAngle < 0.0f) {
        pAngle += 360.0f;
    }
    return pAngle;
}

#define SAW(T, PERIOD) fmod((T), PERIOD) / (PERIOD)

#define GROOVE_PATH_SIN_COS(TRIG, TIME, PERIOD, AMPLITUDE) \
    ((PERIOD) != 0.0f \
            ? (br_scalar)(TRIG(BrAngleToRadian(BrDegreeToAngle( \
                              SAW(TIME, PERIOD) * 360.0f))) \
                  * (AMPLITUDE)) \
            : 0.0f)

#define GROOVE_PATH_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f \
                      ? SAW(TIME, PERIOD) * (AMPLITUDE) \
                      : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? PERIOD * AMPLITUDE \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f \
                                                    ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) \
                                                        * (AMPLITUDE) \
                                                    : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) \
                                                              ? -(AMPLITUDE) \
                                                              : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : GROOVE_PATH_SIN_COS(sinf, TIME, PERIOD, AMPLITUDE)))))))

/* FIXME: for a texture bits move, the original reads the bits of the period as the texture bits pointer,
 * also for periods that are not in a union with one. */
#define PERIOD_TEXTUREBITS(PERIOD) (*(tFunk_texturebits**)&(PERIOD))

// FUNCTION: CARMA2_HW 0x00477230
// FUNCTION: CARMA2_SW 0x00476790
// FUNCTION: CARMA2_CHEAT 0x004775b0
void C2_HOOK_FASTCALL FunkThoseTronics(void) {
    int i;
    int j;
    int k;
    int iteration_count;
    int finished;
    int update_flags;
    tFunkotronic_spec* the_funk;
    tFunk_proximity* the_proximity;
    tU32 the_time;
    tU32 old_last_time;
    br_matrix23* mat_matrix;
    br_material* the_material;
    float f_the_time;
    float rot_amount;
    float distance;
    float best_distance;
    br_vector3 tv;
    br_vector3 camera_offset;
    br_actor* camera_actor;

    if (gFunkotronics_array != NULL) {
        DontLetFlicFuckWithPalettes();
        the_time = GetTotalTime();
        f_the_time = (float)the_time;
        for (i = 0, the_funk = gFunkotronics_array; i < gFunkotronics_array_size; i++, the_funk++) {
            if (the_funk->owner == -999 || the_funk->flags != 0) {
                continue;
            }
            if (the_funk->mode == eFunk_mode_distance && the_funk->proximity_array != NULL) {
                for (j = 0, the_proximity = the_funk->proximity_array; j < the_funk->proximity_count && j >= 0; j++, the_proximity++) {
                    for (k = 0; k < 3; k++) {
                        if (Vector3DistanceSquared(&the_proximity->v[k], gOur_pos) <= gSight_distance_squared) {
                            BrVector3Sub(&tv, &the_proximity->v[k], (br_vector3*)gCamera_to_world.m[3]);
                            if (BrVector3Dot(&the_proximity->n, &tv) < 0.0f && BrVector3Dot(&tv, (br_vector3*)gCamera_to_world.m[2]) < 0.0f) {
                                j = -999;
                                break;
                            }
                        }
                    }
                }
                if (j >= 0) {
                    continue;
                }
            } else if ((the_funk->mode == eFunk_mode_last_lap_only && gLap < gTotal_laps)
                    || (the_funk->mode == eFunk_mode_all_laps_but_last && gLap >= gTotal_laps)) {
                continue;
            }
            the_material = the_funk->material;
            mat_matrix = &the_material->map_transform;
            if (!gAction_replay_mode || !ARReplayIsReallyPaused() || the_funk->matrix_mode == eMove_controlled || the_funk->matrix_mode == eMove_absolute) {
                switch (the_funk->matrix_mod_type) {
                case eMatrix_mod_spin:
                    BrMatrix23Identity(mat_matrix);
                    mat_matrix->m[2][0] -= 0.5f;
                    mat_matrix->m[2][1] -= 0.5f;
                    if (the_funk->matrix_mod_data.spin_info.period > 0.0f) {
                        rot_amount = (float)(1.0 - fmod(the_funk->matrix_mod_data.spin_info.period, 1.0));
                    } else {
                        rot_amount = (float)fmod(-the_funk->matrix_mod_data.spin_info.period, 1.0);
                    }
                    DRMatrix23PostRotate(mat_matrix, (br_angle)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                        rot_amount, PERIOD_TEXTUREBITS(rot_amount), 65536.0)));
                    mat_matrix->m[2][0] += 0.5f;
                    mat_matrix->m[2][1] += 0.5f;
                    break;
                case eMatrix_mod_rock:
                    BrMatrix23Identity(mat_matrix);
                    mat_matrix->m[2][0] -= the_funk->matrix_mod_data.rock_info.x_centre;
                    mat_matrix->m[2][1] -= the_funk->matrix_mod_data.rock_info.y_centre;
                    DRMatrix23PostRotate(mat_matrix, BrDegreeToAngle(NormaliseDegreeAngle((br_scalar)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                        the_funk->matrix_mod_data.rock_info.period, the_funk->matrix_mod_data.texture_info.data,
                        the_funk->matrix_mod_data.rock_info.rock_angle)))));
                    mat_matrix->m[2][0] += the_funk->matrix_mod_data.rock_info.x_centre;
                    mat_matrix->m[2][1] += the_funk->matrix_mod_data.rock_info.y_centre;
                    break;
                case eMatrix_mod_throb:
                    BrMatrix23Identity(mat_matrix);
                    mat_matrix->m[2][0] -= the_funk->matrix_mod_data.throb_info.x_centre;
                    mat_matrix->m[2][1] -= the_funk->matrix_mod_data.throb_info.y_centre;
                    BrMatrix23PostScale(mat_matrix,
                        (br_scalar)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                            the_funk->matrix_mod_data.throb_info.x_period, the_funk->matrix_mod_data.texture_info.data,
                            the_funk->matrix_mod_data.throb_info.x_magnitude) + 1.0),
                        (br_scalar)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                            the_funk->matrix_mod_data.throb_info.y_period, PERIOD_TEXTUREBITS(the_funk->matrix_mod_data.throb_info.y_period),
                            the_funk->matrix_mod_data.throb_info.y_magnitude) + 1.0));
                    mat_matrix->m[2][0] += the_funk->matrix_mod_data.throb_info.x_centre;
                    mat_matrix->m[2][1] += the_funk->matrix_mod_data.throb_info.y_centre;
                    break;
                case eMatrix_mod_slither:
                    mat_matrix->m[2][0] = (br_scalar)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                        the_funk->matrix_mod_data.slither_info.x_period, the_funk->matrix_mod_data.texture_info.data,
                        the_funk->matrix_mod_data.slither_info.x_magnitude));
                    mat_matrix->m[2][1] = (br_scalar)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                        the_funk->matrix_mod_data.slither_info.y_period, PERIOD_TEXTUREBITS(the_funk->matrix_mod_data.slither_info.y_period),
                        the_funk->matrix_mod_data.slither_info.y_magnitude));
                    break;
                case eMatrix_mod_roll:
                    mat_matrix->m[2][0] = (br_scalar)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                        the_funk->matrix_mod_data.roll_info.x_period, the_funk->matrix_mod_data.texture_info.data,
                        1.0));
                    mat_matrix->m[2][1] = (br_scalar)(GROOVE_PATH_FLOAT(the_funk->matrix_mode, f_the_time,
                        the_funk->matrix_mod_data.roll_info.y_period, PERIOD_TEXTUREBITS(the_funk->matrix_mod_data.roll_info.y_period),
                        1.0));
                    break;
                default:
                    break;
                }
                if (the_funk->matrix_mod_type != eMatrix_mod_none) {
                    BrMaterialUpdate(the_funk->material, BR_MATU_MAP_TRANSFORM);
                }
            }
            if (the_funk->lighting_animation_type != eMove_none) {
                the_material->ka = (br_ufraction)(the_funk->ambient_base + GROOVE_PATH_FLOAT(the_funk->lighting_animation_type, f_the_time,
                    the_funk->lighting_animation_data.controlled.period, the_funk->lighting_animation_data.texture_info.data,
                    the_funk->ambient_delta));
                the_material->kd = (br_ufraction)(the_funk->direct_base + GROOVE_PATH_FLOAT(the_funk->lighting_animation_type, f_the_time,
                    the_funk->lighting_animation_data.controlled.period, the_funk->lighting_animation_data.texture_info.data,
                    the_funk->direct_delta));
                the_material->ks = (br_ufraction)(the_funk->specular_base + GROOVE_PATH_FLOAT(the_funk->lighting_animation_type, f_the_time,
                    the_funk->lighting_animation_data.controlled.period, the_funk->lighting_animation_data.texture_info.data,
                    the_funk->specular_delta));
            }
            switch (the_funk->texture_animation_type) {
            case eTexture_animation_frames:
                if (!gAction_replay_mode || !ARReplayIsReallyPaused() || the_funk->mode == eFunk_mode_all_laps_but_last || the_funk->mode == 4) {
                    if (the_funk->time_mode == eTime_mode_accurate) {
                        the_funk->texture_animation_data.frames_info.current_frame = (int)(GROOVE_PATH_FLOAT(the_funk->texture_animation_data.frames_info.mode, f_the_time,
                            the_funk->texture_animation_data.frames_info.controlled.period, the_funk->texture_animation_data.frames_info.texture_info.data,
                            the_funk->texture_animation_data.frames_info.texture_count));
                    } else if (f_the_time - the_funk->last_frame >= the_funk->texture_animation_data.frames_info.controlled.period) {
                        the_funk->last_frame = f_the_time;
                        the_funk->texture_animation_data.frames_info.current_frame++;
                        if (the_funk->texture_animation_data.frames_info.current_frame >= the_funk->texture_animation_data.frames_info.texture_count) {
                            the_funk->texture_animation_data.frames_info.current_frame = 0;
                        }
                    }
                    update_flags = 0;
                    if (the_material->colour_map != the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame]) {
                        the_material->colour_map = the_funk->texture_animation_data.frames_info.textures[the_funk->texture_animation_data.frames_info.current_frame];
                        the_material->user = the_material->colour_map;
                        update_flags = BR_MATU_COLOURMAP;
                    }
                    if (the_funk->texture_animation_data.frames_info.has_matrix != 0) {
                        br_matrix23* frame_mat;

                        frame_mat = &the_funk->texture_animation_data.frames_info.mat[the_funk->texture_animation_data.frames_info.current_frame];
                        if (frame_mat->m[0][0] != the_funk->material->map_transform.m[0][0]
                                || frame_mat->m[1][1] != the_funk->material->map_transform.m[1][1]
                                || frame_mat->m[2][0] != the_funk->material->map_transform.m[2][0]
                                || frame_mat->m[2][1] != the_funk->material->map_transform.m[2][1]) {
                            BrMatrix23Copy(&the_funk->material->map_transform, frame_mat);
                            update_flags |= BR_MATU_MAP_TRANSFORM;
                        }
                    }
                    if (update_flags != 0) {
                        BrMaterialUpdate(the_funk->material, update_flags);
                    }
                }
                break;
            case eTexture_animation_flic:
                if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                    old_last_time = (tU32)the_funk->last_frame;
                    if (the_time < old_last_time) {
                        the_time = old_last_time + (old_last_time - the_time);
                    }
                    if (the_funk->time_mode == eTime_mode_accurate) {
                        if (old_last_time != 0) {
                            iteration_count = (the_time - old_last_time) / the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period;
                        } else {
                            iteration_count = 1;
                        }
                    } else if (the_time - the_funk->last_frame >= the_funk->texture_animation_data.flic_info.flic_descriptor.frame_period) {
                        iteration_count = 1;
                    } else {
                        iteration_count = 0;
                    }
                    for (j = 0; j < iteration_count; j++) {
                        finished = PlayNextFlicFrame(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                        BrMapUpdate(the_funk->material->colour_map, BR_MAPU_ALL);
                        BrMaterialUpdate(the_funk->material, BR_MATU_COLOURMAP);
                        if (finished) {
                            EndFlic(&the_funk->texture_animation_data.flic_info.flic_descriptor);
                            StartFlic(the_funk->texture_animation_data.flic_info.flic_descriptor.file_name,
                                -1,
                                &the_funk->texture_animation_data.flic_info.flic_descriptor,
                                the_funk->texture_animation_data.flic_info.flic_data_length,
                                (tS8*)the_funk->texture_animation_data.flic_info.flic_data,
                                the_material->colour_map,
                                0,
                                0,
                                0);
                        }
                        the_funk->last_frame = f_the_time;
                    }
                }
                break;
            case eTexture_animation_camera:
                if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                    if (the_funk->texture_animation_data.camera_info.count == 1) {
                        camera_actor = the_funk->texture_animation_data.camera_info.actors[0];
                    } else {
                        camera_actor = NULL;
                        best_distance = 2147483648.0f;
                        for (j = 0; j < the_funk->texture_animation_data.camera_info.count; j++) {
                            BrVector3Sub(&tv, &gProgram_state.current_car.pos, &the_funk->texture_animation_data.camera_info.actors[j]->t.t.translate.t);
                            distance = BrVector3LengthSquared(&tv);
                            if (distance < best_distance) {
                                camera_actor = the_funk->texture_animation_data.camera_info.actors[j];
                                best_distance = distance;
                                camera_offset = tv;
                            }
                        }
                    }
                    if (camera_actor != NULL) {
                        if (the_funk->texture_animation_data.camera_info.mode == eFunk_camera_tracking) {
                            /* FIXME: with a single camera, the original uses camera_offset without setting it */
                            camera_actor->t.type = BR_TRANSFORM_LOOK_UP;
                            BrVector3Copy(&camera_actor->t.t.look_up.look, &camera_offset);
                            BrVector3Set(&camera_actor->t.t.look_up.up, 0.0f, 1.0f, 0.0f);
                        }
                        AddExtraRender(camera_actor, the_funk->material);
                    }
                }
                break;
            case eTexture_animation_mirror:
                if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                    if (the_funk->texture_animation_data.mirror_info.actor != NULL) {
                        BrMatrix34Mul(&the_funk->texture_animation_data.mirror_info.actor->t.t.mat, &gCamera_to_world, &the_funk->texture_animation_data.mirror_info.mat);
                        AddExtraRender(the_funk->texture_animation_data.mirror_info.actor, the_funk->material);
                    }
                }
                break;
            default:
                break;
            }
        }
        LetFlicFuckWithPalettes();
    }
}

#undef PERIOD_TEXTUREBITS

// FUNCTION: CARMA2_HW 0x00478e30
// FUNCTION: CARMA2_SW 0x00478390
// FUNCTION: CARMA2_CHEAT 0x004791b0
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;

    switch (pGroove->path_type) {
    case eGroove_path_straight:
        if (pGroove->path_data.straight_info.x_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[0]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.x_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][0]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][0] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            }
        }

        if (pGroove->path_data.straight_info.y_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[1]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.y_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][1]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][1] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            }
        }

        if (pGroove->path_data.straight_info.z_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[2]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.z_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][2]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][2] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            }
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    case eGroove_path_circular:
        pGroove->actor->t.t.translate.t = pGroove->path_data.circular_info.centre;
        if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    default:
        break;
    }
}

#define GROOVE_ANGLE(MODE, TIME, PERIOD, TEXTUREBITS) \
    BrDegreeToAngle(((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? SAW(TIME, PERIOD) * 360.0 : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (PERIOD) * 360.0 \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) * 360.0 : 0.0) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -360.0 : 360.0) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? sin(BrAngleToRadian(BrDegreeToAngle(SAW(TIME, PERIOD) * 360.0))) * 360.0 : 0.0))))))))

#define GROOVE_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? SAW(TIME, PERIOD) * (AMPLITUDE) : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (PERIOD) * (AMPLITUDE) \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) * (AMPLITUDE) : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -(AMPLITUDE) : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? BR_SIN(BrDegreeToAngle(SAW(TIME, PERIOD) * 360.0)) * (AMPLITUDE) : 0.0f)))))))

// FUNCTION: CARMA2_HW 0x00479890
// FUNCTION: CARMA2_SW 0x00478df0
// FUNCTION: CARMA2_CHEAT 0x00479c10
void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    int rock_it;
    br_scalar x_size;
    br_scalar y_size;
    br_scalar z_size;
    br_scalar pos;
    br_bounds* bounds;

    switch (pGroove->object_type) {
    case eGroove_object_spin:
        switch (pGroove->object_data.spin_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        }
        break;
    case eGroove_object_rock:
        rock_it = 1;
        pos = (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
            pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle));

        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pGroove->object_data.rock_info.current_angle > pos) {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_less_than) {
            if (pGroove->object_resumption_value > pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
            if (pGroove->object_resumption_value < pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        }
        if (rock_it) {
            pGroove->object_data.rock_info.current_angle = pos;
        }

        switch (pGroove->object_data.rock_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        }
        break;
    case eGroove_object_throb:
        BrMatrix34PostScale(pMat,
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude)
                + 1.0));
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        x_size = bounds->max.v[0] - bounds->min.v[0];
        y_size = bounds->max.v[1] - bounds->min.v[1];
        z_size = bounds->max.v[2] - bounds->min.v[2];
        if (pGroove->object_data.shear_info.x_magnitude == 0.0f) {
            BrMatrix34PostShearX(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else if (pGroove->object_data.shear_info.y_magnitude == 0.0f) {
            BrMatrix34PostShearY(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else {
            BrMatrix34PostShearZ(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size));
        }
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00478c80
// FUNCTION: CARMA2_SW 0x004781e0
// FUNCTION: CARMA2_CHEAT 0x00479000
void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it) {
    br_actor* the_actor;
    br_vector3 actor_pos;
    br_matrix34* the_mat;
    tInterrupt_status old_path_interrupt;
    tInterrupt_status old_object_interrupt;

    old_path_interrupt = pGroove->path_interrupt_status;
    old_object_interrupt = pGroove->object_interrupt_status;
    the_actor = pGroove->actor;
    pGroove->done_this_frame = 1;
    CalcActorGlobalPos(&actor_pos, the_actor);
    if (PointOutOfSight(&actor_pos, pGroove->mode == eGroove_mode_distance ? gYon_squared : 36.f)) {
        return;
    }

    the_mat = &the_actor->t.t.mat;
    if ((!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->path_mode == eMove_controlled
            || pGroove->path_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if ((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
        && (!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->object_mode == eMove_controlled
            || pGroove->object_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x2)) {
        the_mat->m[0][0] = 1.0f;
        the_mat->m[0][1] = 0.0f;
        the_mat->m[0][2] = 0.0f;
        the_mat->m[1][0] = 0.0f;
        the_mat->m[1][1] = 1.0f;
        the_mat->m[1][2] = 0.0f;
        the_mat->m[2][0] = 0.0f;
        the_mat->m[2][1] = 0.0f;
        the_mat->m[2][2] = 1.0f;
        the_mat->m[3][0] = -pGroove->object_centre.v[0];
        the_mat->m[3][1] = -pGroove->object_centre.v[1];
        the_mat->m[3][2] = -pGroove->object_centre.v[2];
        ObjectGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
        the_mat->m[3][0] = pGroove->object_position.v[0] + the_mat->m[3][0] + pGroove->object_centre.v[0];
        the_mat->m[3][1] = pGroove->object_position.v[1] + the_mat->m[3][1] + pGroove->object_centre.v[1];
        the_mat->m[3][2] = pGroove->object_position.v[2] + the_mat->m[3][2] + pGroove->object_centre.v[2];
        if (pGroove->lollipop_mode != eLollipop_none) {
            LollipopizeActor(pGroove->actor, &gCamera_to_world, pGroove->lollipop_mode);
        }
    }
    if (old_path_interrupt != pGroove->path_interrupt_status || old_object_interrupt != pGroove->object_interrupt_status) {
        PipeSingleGrooveStop(pGroove - gGroovidelics_array, the_mat,
            pGroove->path_interrupt_status, pGroove->object_interrupt_status,
            pGroove->path_resumption_value, pGroove->object_resumption_value);
    }
}

// FUNCTION: CARMA2_HW 0x00478c00
// FUNCTION: CARMA2_SW 0x00478160
// FUNCTION: CARMA2_CHEAT 0x00478f80
void C2_HOOK_FASTCALL GrooveThoseDelics(void) {
    tGroovidelic_spec* the_groove;
    float f_the_time;
    int i;

    if (gGroovidelics_array != NULL) {
        f_the_time = (float)GetTotalTime();
        gPrevious_groove_times[1] = gPrevious_groove_times[0];
        gPrevious_groove_times[0] = (tU32)f_the_time;
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            if (the_groove->owner != -999 && !the_groove->done_this_frame) {
                GrooveThisDelic(the_groove, (tU32)f_the_time, 0);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b0e0
// FUNCTION: CARMA2_SW 0x0047a640
// FUNCTION: CARMA2_CHEAT 0x0047b460
void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor) {
    tGroovidelic_spec* the_groove;
    int i;

    for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            if (the_groove->path_interrupt_status == eInterrupt_none
                && the_groove->object_interrupt_status == eInterrupt_none) {
                GrooveThisDelic(the_groove, gPrevious_groove_times[1], 1);
            }
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b130
// FUNCTION: CARMA2_SW 0x0047a690
// FUNCTION: CARMA2_CHEAT 0x0047b4b0
void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags |= pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b160
// FUNCTION: CARMA2_SW 0x0047a6c0
// FUNCTION: CARMA2_CHEAT 0x0047b4e0
void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags &= ~pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b1a0
// FUNCTION: CARMA2_SW 0x0047a700
// FUNCTION: CARMA2_CHEAT 0x0047b520
void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value) {
    tGroovidelic_spec* the_groove;

    the_groove = &gGroovidelics_array[pIndex];
    the_groove->path_interrupt_status = pPath_interrupt_status;
    the_groove->object_interrupt_status = pObject_interrupt_status;
    the_groove->path_resumption_value = pPath_resumption_value;
    the_groove->object_resumption_value = pObject_resumption_value;
    BrMatrix34Copy(&the_groove->actor->t.t.mat, pMatrix);
}

// FUNCTION: CARMA2_HW 0x0047b1e0
// FUNCTION: CARMA2_SW 0x0047a740
// FUNCTION: CARMA2_CHEAT 0x0047b560
void C2_HOOK_FASTCALL ResetGrooveFlags(void) {
    int i;
    tGroovidelic_spec* the_groove;

    i = 0;
    if (gGroovidelics_array_size > 0) {
        the_groove = gGroovidelics_array;
        do {
            the_groove->done_this_frame = 0;
            i++;
            the_groove++;
        } while (i < gGroovidelics_array_size);
    }
}

// FUNCTION: CARMA2_HW 0x0047b210
// FUNCTION: CARMA2_SW 0x0047a770
// FUNCTION: CARMA2_CHEAT 0x0047b590
intptr_t C2_HOOK_CDECL DelGrooveRef(br_actor* pActor, void* pArg) {
    tGroovidelic_spec* the_groove;
    int i;

    for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
        if (pActor == the_groove->actor) {
            the_groove->owner = -999;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0047b250
// FUNCTION: CARMA2_SW 0x0047a7b0
// FUNCTION: CARMA2_CHEAT 0x0047b5d0
void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x1;
    PipeSingleFunkEnable(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x0047b280
// FUNCTION: CARMA2_SW 0x0047a7e0
// FUNCTION: CARMA2_CHEAT 0x0047b600
void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x1;
    PipeSingleFunkEnable(pIndex, 1);
}

// FUNCTION: CARMA2_HW 0x0047b2b0
// FUNCTION: CARMA2_SW 0x0047a810
// FUNCTION: CARMA2_CHEAT 0x0047b630
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x2;
}

// FUNCTION: CARMA2_HW 0x0047b2e0
// FUNCTION: CARMA2_SW 0x0047a840
// FUNCTION: CARMA2_CHEAT 0x0047b660
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x2;
}

// FUNCTION: CARMA2_HW 0x0047b310
// FUNCTION: CARMA2_SW 0x0047a870
// FUNCTION: CARMA2_CHEAT 0x0047b690
void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags) {
    if (pFlags != 0) {
        EnableFunkotronic(pIndex);
    } else {
        DisableFunkotronic(pIndex);
    }
}

// FUNCTION: CARMA2_HW 0x0047b360
// FUNCTION: CARMA2_SW 0x0047a8c0
// FUNCTION: CARMA2_CHEAT 0x0047b6e0
tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor) {
    tGroovidelic_spec* groove;
    int count;
    int i;

    groove = gGroovidelics_array;
    count = gGroovidelics_array_size;
    for (i = 0; i < count; i++, groove++) {
        if (groove->actor == pActor) {
            return groove;
        }
    }
    return NULL;
}

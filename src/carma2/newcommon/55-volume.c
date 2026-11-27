/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "55-volume.h"

#include "00-car.h"
#include "08-loading1.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "41-utility.h"
#include "52-errors.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"

#include "c2_string.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00761b80
// GLOBAL: CARMA2_SW 0x007580e0
// GLOBAL: CARMA2_CHEAT 0x00769220
tSpecial_volume gDefault_default_water_spec_vol;

// GLOBAL: CARMA2_HW 0x00761f44
// GLOBAL: CARMA2_SW 0x007584a4
tSpecial_volume* gDefault_water_spec_vol_real;

// GLOBAL: CARMA2_HW 0x00660268
// GLOBAL: CARMA2_SW 0x0065ab48
const char* gSoundType_choices[2] = {
    "SATURATED",
    "SCATTERED"
};

// GLOBAL: CARMA2_HW 0x00660258
// GLOBAL: CARMA2_SW 0x0065ab38
const char* gSpecial_effects_boundary_choices[4] = {
    "BOX",
    "PLANE",
    "DEFAULT",
    "NEW",
};

// FUNCTION: CARMA2_HW 0x004ff110
tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void) {
    return gDefault_water_spec_vol_real;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004ff120
// FUNCTION: CARMA2_SW 0x004faa00
// FUNCTION: CARMA2_CHEAT 0x00504dc0
tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3) {
    int i;
    int j;
    tSpecial_volume* v;
    tSpecial_volume* result;
    tSpecial_volume* sound_volume;
    tSpecial_volume* containing_volume;
    float sound_dist;
    float dist;
    float fartest_dist;
    int bnds_axis;
    int fartest_bnds_axis;
    int axis;
    br_vector3 sound_dir;
    br_vector3 pnt_norm;

    result = NULL;
    for (i = 0, v = gProgram_state.special_volumes; i < gProgram_state.special_volume_count && (result == NULL || pArg3); i++, v++) {
        containing_volume = NULL;
        sound_volume = NULL;
        sound_dist = 1e7f;
        switch (v->boundary_type) {
        case eFx_boundary_box:
            if (pP->v[0] > v->boundary.box.bounds.min.v[0]
                    && pP->v[0] < v->boundary.box.bounds.max.v[0]
                    && pP->v[2] > v->boundary.box.bounds.min.v[2]
                    && pP->v[2] < v->boundary.box.bounds.max.v[2]
                    && pP->v[1] > v->boundary.box.bounds.min.v[1]
                    && pP->v[1] < v->boundary.box.bounds.max.v[1]) {
                BrMatrix34ApplyP(&pnt_norm, pP, &v->boundary.box.inv_mat);
                fartest_dist = 0.0f;
                for (j = 0; j < 3; j++) {
                    if (pnt_norm.v[j] < -1.0f) {
                        dist = -1.0f - pnt_norm.v[j];
                        bnds_axis = j + 3;
                    } else if (pnt_norm.v[j] > 1.0f) {
                        dist = pnt_norm.v[j] - 1.0f;
                        bnds_axis = j;
                    } else {
                        dist = 0.0f;
                    }
                    if (dist > fartest_dist) {
                        fartest_dist = dist;
                        fartest_bnds_axis = bnds_axis;
                    }
                }
                if (fartest_dist == 0.0f || v->soundfx_type == kSoundFx_None) {
                    if (fartest_dist == 0.0f) {
                        containing_volume = v;
                    }
                } else {
                    axis = fartest_bnds_axis % 3;
                    if ((fartest_dist /= v->boundary.box.axis_length.v[axis]) < 1e7f) {
                        sound_dir.v[0] = v->boundary.box.mat.m[axis][0] * (float)(fartest_bnds_axis >= 3 ? -1 : 1);
                        sound_volume = v;
                        sound_dir.v[1] = v->boundary.box.mat.m[axis][1] * (float)(fartest_bnds_axis >= 3 ? -1 : 1);
                        sound_dist = fartest_dist;
                        sound_dir.v[2] = v->boundary.box.mat.m[axis][2] * (float)(fartest_bnds_axis >= 3 ? -1 : 1);
                    }
                }
            }
            break;
        case eFx_boundary_plane:
            dist = DistanceFromFaceND(pP, (br_vector3*)v->boundary.plane.v, v->boundary.plane.v[3]);
            if (dist < 0.0f) {
                containing_volume = v;
            } else if (dist < 1e7f && v->soundfx_type != kSoundFx_None) {
                sound_dist = dist;
                BrVector3Copy(&sound_dir, (br_vector3*)v->boundary.plane.v);
                sound_volume = v;
            }
            break;
        default:
            break;
        }
        if (containing_volume != NULL) {
            sound_dist = 0.0f;
            sound_volume = containing_volume;
            if (result == NULL) {
                result = containing_volume;
            }
        }
        if (pArg3 && sound_volume != NULL && sound_dist < 10.0f) {
            DoEnvSound(sound_volume, pP, sound_volume->soundfx_type, &sound_volume->soundfx_data, sound_dist, &sound_dir);
        }
    }
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ff410
// FUNCTION: CARMA2_SW 0x004facf0
void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject) {
    tSpecial_volume* new_special_volume;
    tCar_spec* car;

    new_special_volume = FindSpecialVolume(&pObject->pos, pObject->last_special_volume, 0);
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        car = pObject->owner;
    } else {
        car = NULL;
    }
    if (pObject->auto_special_volume != NULL
        && (new_special_volume == NULL || new_special_volume->gravity_multiplier == 1.f)) {
        if (pObject->water_d == 10000.f && pObject->water_depth_factor != 1.f) {
            pObject->auto_special_volume = NULL;
        } else {
            new_special_volume = pObject->auto_special_volume;
        }
    }
    if (new_special_volume != pObject->last_special_volume && car != NULL && car->driver == eDriver_local_human) {
        if (pObject->last_special_volume != NULL && pObject->last_special_volume->exit_noise >= 0
            && (new_special_volume == NULL || new_special_volume->exit_noise != pObject->last_special_volume->exit_noise)) {
            DRS3StartSound(gCar_outlet, pObject->last_special_volume->exit_noise);
        }
        if (new_special_volume != NULL && new_special_volume->entry_noise >= 0
            && (pObject->last_special_volume == NULL || pObject->last_special_volume->entry_noise != new_special_volume->entry_noise)) {
            DRS3StartSound(gCar_outlet, new_special_volume->entry_noise);
        }
    }
    pObject->last_special_volume = new_special_volume;
    if (car != NULL && new_special_volume != NULL && new_special_volume->gravity_multiplier < 1.f && IsCarSmoking(car)) {
        StopCarSmoking(car);
    }
}

// FUNCTION: CARMA2_HW 0x004ff530
// FUNCTION: CARMA2_SW 0x004fae10
// FUNCTION: CARMA2_CHEAT 0x005051d0
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pObject) {
    br_vector3 direction;
    br_vector3 normal;
    br_scalar distance;
    br_material* material;
    tSpecial_volume* volume;
    char* mat_id;

    volume = NULL;
    BrVector3Set(&direction, 0.f, 200.f, 0.f);
    DisablePlingMaterials();
    FindFace(&pObject->actor->t.t.translate.t, &direction, &normal, &distance, &material);
    EnablePlingMaterials();
    if (distance < 100.f && material != NULL) {
        mat_id = material->identifier;
        if (mat_id != NULL) {
            if (strlen(mat_id) != 0 && (mat_id[0] == '!' || mat_id[0] == '#')) {
                volume = GetDefaultSpecialVolumeForWater();
            }
        }
    }
    pObject->auto_special_volume = volume;
    pObject->water_depth_factor = 1.f;
}

// STUB: CARMA2_HW 0x004ff5d0
// STUB: CARMA2_SW 0x004faeb0
void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004ff8d0
// FUNCTION: CARMA2_SW 0x004fb1b0
// FUNCTION: CARMA2_CHEAT 0x00505580
void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    /* gravity multiplier */
    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !gRussels_fannies) {
        pSpec->gravity_multiplier = 1.f;
    }
    /* viscosity multiplier */
    pSpec->viscosity_multiplier = GetAFloat(pF);
    /* Car damage per millisecond */
    pSpec->car_damage_per_ms = GetAScalar(pF);
    /* Pedestrian damage per millisecond */
    pSpec->ped_damage_per_ms = GetAScalar(pF);
    /* camera effect index */
    pSpec->camera_special_effect_index = GetAnInt(pF);
    /* Sky colour */
    pSpec->sky_col = GetAnInt(pF);

    /* Windscreen material to use */
    GetAString(pF, s);
    if (pScreen_name_str != NULL) {
        strcpy(pScreen_name_str, s);
    } else {
        pSpec->screen_pixelmap = BrMapFind(s);
    }

    /* Sound ID of entry noise */
    pSpec->entry_noise = GetAnInt(pF);
    /* Sound ID of exit noise */
    pSpec->exit_noise = GetAnInt(pF);
    /* Engine noise index */
    pSpec->engine_noise_index = GetAnInt(pF);
    /* material index */
    pSpec->material_modifier_index = GetAnInt(pF);
    if (soundfx) {
        pSpec->soundfx_type = GetALineAndInterpretCommand(pF, gSoundType_choices, REC2_ASIZE(gSoundType_choices));
        if (pSpec->soundfx_type != kSoundFx_None) {
            ReadSoundSpec(pF, &pSpec->soundfx_data);
        }
    } else {
        pSpec->soundfx_type = kSoundFx_None;
    }
}

// FUNCTION: CARMA2_HW 0x004ffa20
// FUNCTION: CARMA2_SW 0x004fb300
// FUNCTION: CARMA2_CHEAT 0x005056d0
void C2_HOOK_FASTCALL SaveSpecialVolumes(void) {
    tPath_name the_path;
    FILE* f;
    int i;
    tSpecial_volume* v;
    const char* screen_name;

    PathCat(the_path, gApplication_path, "SPECSAVE.TXT");
    f = DRfopen(the_path, "wt");
    if (f != NULL) {
        fprintf(f, "// SPECIAL EFFECTS VOLUMES\n\n");
        fprintf(f, "%d\t\t\t\t// # special effects volumes\n\n", gProgram_state.special_volume_count);
        for (i = 0, v = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, v++) {
            switch (v->boundary_type) {
            case eFx_boundary_box:
                fprintf(f, "BOX\n");
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[0][0], v->boundary.box.mat.m[0][1], v->boundary.box.mat.m[0][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[1][0], v->boundary.box.mat.m[1][1], v->boundary.box.mat.m[1][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[2][0], v->boundary.box.mat.m[2][1], v->boundary.box.mat.m[2][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[3][0], v->boundary.box.mat.m[3][1], v->boundary.box.mat.m[3][2]);
                break;
            case eFx_boundary_plane:
                fprintf(f, "%.3f, %.3f, %.3f, %.3f\n", v->boundary.plane.v[0], v->boundary.plane.v[1], v->boundary.plane.v[2], v->boundary.plane.v[3]);
                break;
            case eFx_boundary_default:
                fprintf(f, "DEFAULT\n");
                break;
            default:
                break;
            }
            fprintf(f, "%.2f\t\t\t\t// gravity multiplier\n", v->gravity_multiplier);
            fprintf(f, "%.2f\t\t\t\t// viscosity multiplier\n", v->viscosity_multiplier);
            fprintf(f, "%.2f\t\t\t\t// Car damage per millisecond\n", v->car_damage_per_ms);
            fprintf(f, "%.2f\t\t\t\t// Pedestrian damage per millisecond\n", v->ped_damage_per_ms);
            fprintf(f, "%d\t\t\t\t\t// camera effect index\n", v->camera_special_effect_index);
            fprintf(f, "%d\t\t\t\t\t// sky colour\n", v->sky_col);
            if (v->screen_pixelmap != NULL) {
                screen_name = v->screen_pixelmap->identifier;
            } else {
                screen_name = "NONE";
            }
            fprintf(f, "%s\t\t\t\t// Windscreen texture to use\n", screen_name);
            fprintf(f, "%d\t\t\t\t\t// Sound ID of entry noise\n", v->entry_noise);
            fprintf(f, "%d\t\t\t\t\t// Sound ID of exit noise\n", v->exit_noise);
            fprintf(f, "%d\t\t\t\t\t// Engine noise index\n", v->engine_noise_index);
            fprintf(f, "%d\t\t\t\t\t// material index\n", v->material_modifier_index);
            if (v->boundary_type == eFx_boundary_box || v->boundary_type == eFx_boundary_plane) {
                if (v->soundfx_type == kSoundFx_None) {
                    fprintf(f, "NONE\t\t\t\t\t// sound type\n");
                } else {
                    fprintf(f, "%s\t\t\t\t\t// sound type\n", gSoundType_choices[v->soundfx_type]);
                    WriteOutSoundSpec(f, &v->soundfx_data);
                }
            }
            fprintf(f, "\n");
        }
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x004ffd20
// FUNCTION: CARMA2_SW 0x004fb600
// FUNCTION: CARMA2_CHEAT 0x005059d0
void C2_HOOK_FASTCALL FindInverseAndWorldBox(tSpecial_volume* pSpec) {
    br_bounds bnds;

    BrVector3Set(&bnds.min, -1.0f, -1.0f, -1.0f);
    BrVector3Set(&bnds.max, 1.0f, 1.0f, 1.0f);
    GetNewBoundingBox(&pSpec->boundary.box.bounds, &bnds, &pSpec->boundary.box.mat);
    BrMatrix34Inverse(&pSpec->boundary.box.inv_mat, &pSpec->boundary.box.mat);
}

// FUNCTION: CARMA2_HW 0x004ffd80
// FUNCTION: CARMA2_SW 0x004fb660
void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF) {
    int i;
    int j;
    tSpecial_volume* spec;
    tSpecial_volume_boundary_type boundary_type;

    C2_HOOK_BUG_ON(sizeof(tSpecial_volume) != 0xdc);
    C2_HOOK_BUG_ON(REC2_ASIZE(gSpecial_effects_boundary_choices) != 4);

    gDefault_engine_noise_index = GetAnInt(pF);
    gDefault_water_spec_vol_real = &gDefault_default_water_spec_vol;
    gProgram_state.special_volume_count = GetAnInt(pF);
    if (gProgram_state.special_volume_count != 0) {
        gProgram_state.special_volumes = BrMemAllocate(gProgram_state.special_volume_count * sizeof(tSpecial_volume), kMem_special_volume);
        for (i = 0, spec = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, spec++) {
            PossibleService();
            boundary_type = GetALineAndInterpretCommand(pF, gSpecial_effects_boundary_choices, REC2_ASIZE(gSpecial_effects_boundary_choices));
            spec->boundary_type = boundary_type;
            if (spec->boundary_type == eFx_boundary_new) {
                spec->boundary_type = eFx_boundary_box;
            }
            switch (spec->boundary_type) {
            case eFx_boundary_box:
                GetThreeScalars(pF, &spec->boundary.box.mat.m[0][0], &spec->boundary.box.mat.m[0][1], &spec->boundary.box.mat.m[0][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[1][0], &spec->boundary.box.mat.m[1][1], &spec->boundary.box.mat.m[1][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[2][0], &spec->boundary.box.mat.m[2][1], &spec->boundary.box.mat.m[2][2]);
                GetThreeScalars(pF, &spec->boundary.box.mat.m[3][0], &spec->boundary.box.mat.m[3][1], &spec->boundary.box.mat.m[3][2]);
                FindInverseAndWorldBox(spec);
                spec->boundary.box.axis_length.v[0] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[0]);
                spec->boundary.box.axis_length.v[1] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[1]);
                spec->boundary.box.axis_length.v[2] = BrVector3Length((br_vector3*)spec->boundary.box.inv_mat.m[2]);
                ParseSpecialVolume(pF, spec, NULL, boundary_type == eFx_boundary_box);
                if (spec->soundfx_type != kSoundFx_None) {
                    for (j = 0; j < REC2_ASIZE(spec->boundary.box.bounds.min.v); j++) {
                        spec->boundary.box.bounds.min.v[j] -= 10.0f;
                    }
                    for (j = 0; j < REC2_ASIZE(spec->boundary.box.bounds.max.v); j++) {
                        spec->boundary.box.bounds.max.v[j] += 10.0f;
                    }
                }
                break;
            case eFx_boundary_plane:
                GetFourScalars(pF, &spec->boundary.plane.v[0], &spec->boundary.plane.v[1], &spec->boundary.plane.v[2], &spec->boundary.plane.v[3]);
                ParseSpecialVolume(pF, spec, NULL, 1);
                break;
            case eFx_boundary_default:
                ParseSpecialVolume(pF, spec, NULL, 0);
                gDefault_water_spec_vol_real = spec;
                break;
            default:
                FatalError(kFatalError_ProblemWithSpecialVolumeNumber_I);
                break;
            }
        }
    }
}

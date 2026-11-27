/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "16-graphics1.h"

#include "02-init.h"
#include "07-structur.h"
#include "15-displays.h"
#include "17-world2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "23-piping.h"
#include "25-grafdata.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "33-depth.h"
#include "41-utility.h"
#include "52-errors.h"
#include "53-controls.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "64-movie.h"
#include "66-tintedpoly.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_types.h"
#include "rec2_macros.h"

#include <limits.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00703e24
// GLOBAL: CARMA2_SW 0x006fab04
// GLOBAL: CARMA2_CHEAT 0x0070b4c4
int gWidth;

// GLOBAL: CARMA2_HW 0x00703e20
// GLOBAL: CARMA2_SW 0x006fab00
// GLOBAL: CARMA2_CHEAT 0x0070b4c0
int gHeight;

// GLOBAL: CARMA2_HW 0x00705060
// GLOBAL: CARMA2_SW 0x006fbb28
// GLOBAL: CARMA2_CHEAT 0x0070c700
int gX_offset;

// GLOBAL: CARMA2_HW 0x006baa2c
// GLOBAL: CARMA2_SW 0x006b1708
// GLOBAL: CARMA2_CHEAT 0x006c20cc
int gY_offset;

// GLOBAL: CARMA2_HW 0x0068be30
// GLOBAL: CARMA2_SW 0x00685e74
// GLOBAL: CARMA2_CHEAT 0x006936d0
int gBrZb_initialized;

// GLOBAL: CARMA2_HW 0x006a22bc
// GLOBAL: CARMA2_SW 0x0069c314
// GLOBAL: CARMA2_CHEAT 0x006a9e34
br_pixelmap* gRear_pixelmap;

// GLOBAL: CARMA2_HW 0x006a22b8
// GLOBAL: CARMA2_SW 0x0069c310
// GLOBAL: CARMA2_CHEAT 0x006a9e30
br_pixelmap* gEvalu;

// GLOBAL: CARMA2_HW 0x006a22f8
// GLOBAL: CARMA2_SW 0x0069c350
// GLOBAL: CARMA2_CHEAT 0x006a9e70
tWobble_spec gWobble_array[5] = { 0 };

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x006a2348
// GLOBAL: CARMA2_SW 0x0069c3a0
int gINT_006a2348 = 0;
#endif

// GLOBAL: CARMA2_HW 0x006a234c
// GLOBAL: CARMA2_SW 0x0069c3a4
// GLOBAL: CARMA2_CHEAT 0x006a9ec4
float gPrevious_rear_yon = 0.0f;

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x006a2350
// GLOBAL: CARMA2_SW 0x0069c3a8
int gINT_006a2350[2] = { 0 };
#endif

// GLOBAL: CARMA2_HW 0x006a2358
// GLOBAL: CARMA2_SW 0x0069c3b0
// GLOBAL: CARMA2_CHEAT 0x006a9ed0
int gAR_fudge_headups = 0;

// GLOBAL: CARMA2_HW 0x00705080
// GLOBAL: CARMA2_SW 0x006fbb40
// GLOBAL: CARMA2_CHEAT 0x0070c720
float gCosine_array[65] = { 0 };

// GLOBAL: CARMA2_HW 0x00705184
// GLOBAL: CARMA2_SW 0x006fbc44
// GLOBAL: CARMA2_CHEAT 0x0070c824
int gScreen_wobble_y = 0;

// GLOBAL: CARMA2_HW 0x0065fdc8
// GLOBAL: CARMA2_SW 0x0065a708
// GLOBAL: CARMA2_CHEAT 0x00667610
tShadow_level gShadow_level = eShadow_us_only;

// GLOBAL: CARMA2_HW 0x0065fb20
// GLOBAL: CARMA2_SW 0x0065a480
// GLOBAL: CARMA2_CHEAT 0x006672d0
br_matrix34 gSheer_mat = {
    { { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f } }
};

// GLOBAL: CARMA2_HW 0x0065fb54
// GLOBAL: CARMA2_CHEAT 0x00667304
int gHud_tinted4 = -1;

// GLOBAL: CARMA2_HW 0x0068be38
// GLOBAL: CARMA2_SW 0x00685e7c
// GLOBAL: CARMA2_CHEAT 0x006936d8
int gRender_indent;

// GLOBAL: CARMA2_HW 0x006baa40
// GLOBAL: CARMA2_SW 0x006b1720
// GLOBAL: CARMA2_CHEAT 0x006c20e0
tU8 gTemporary_physics_render_buffer[300000];

// GLOBAL: CARMA2_HW 0x006a2448
// GLOBAL: CARMA2_SW 0x0069c4a0
// GLOBAL: CARMA2_CHEAT 0x006a9fc0
tClip_details gShadow_clip_planes[8];

// GLOBAL: CARMA2_HW 0x006a23d8
// GLOBAL: CARMA2_SW 0x0069c430
// GLOBAL: CARMA2_CHEAT 0x006a9f50
int gFancy_shadow;

// GLOBAL: CARMA2_HW 0x006a27e0
// GLOBAL: CARMA2_SW 0x0069c838
// GLOBAL: CARMA2_CHEAT 0x006aa358
br_material* gShadow_material;

// GLOBAL: CARMA2_HW 0x006a27d0
// GLOBAL: CARMA2_CHEAT 0x006aa348
br_vector3 gShadow_light_ray;

// GLOBAL: CARMA2_HW 0x006a27c0
// GLOBAL: CARMA2_CHEAT 0x006aa338
br_vector3 gShadow_light_z;

// GLOBAL: CARMA2_HW 0x006a27b0
// GLOBAL: CARMA2_CHEAT 0x006aa328
br_vector3 gShadow_light_x;

// GLOBAL: CARMA2_HW 0x006a27e4
// GLOBAL: CARMA2_SW 0x0069c83c
// GLOBAL: CARMA2_CHEAT 0x006aa35c
br_model* gShadow_model;

// GLOBAL: CARMA2_HW 0x006a2444
// GLOBAL: CARMA2_SW 0x0069c49c
// GLOBAL: CARMA2_CHEAT 0x006a9fbc
br_actor* gShadow_actor;

// GLOBAL: CARMA2_HW 0x00705188
// GLOBAL: CARMA2_SW 0x006fbc48
// GLOBAL: CARMA2_CHEAT 0x0070c828
int gScreen_wobble_x;

// GLOBAL: CARMA2_HW 0x0070518c
// GLOBAL: CARMA2_SW 0x006fbc4c
// GLOBAL: CARMA2_CHEAT 0x0070c82c
br_material* gMaterial_below;

// GLOBAL: CARMA2_HW 0x00705190
// GLOBAL: CARMA2_SW 0x006fbc50
// GLOBAL: CARMA2_CHEAT 0x0070c830
int gFrontend_wrecks_car_count;

// GLOBAL: CARMA2_HW 0x006a2488
// GLOBAL: CARMA2_SW 0x0069c4e0
// GLOBAL: CARMA2_CHEAT 0x006aa000
tSaved_table gSaved_shade_tables[100];

// GLOBAL: CARMA2_HW 0x006a27a8
// GLOBAL: CARMA2_SW 0x0069c800
// GLOBAL: CARMA2_CHEAT 0x006aa320
int gSaved_table_count;

// GLOBAL: CARMA2_HW 0x00703e28
// GLOBAL: CARMA2_CHEAT 0x0070b4c8
int gHud_actor_storage_size;

// GLOBAL: CARMA2_HW 0x00704e60
// GLOBAL: CARMA2_CHEAT 0x0070c500
br_actor* gHud_actor_storage[128];

// GLOBAL: CARMA2_HW 0x006a22c4
// GLOBAL: CARMA2_SW 0x0069c31c
// GLOBAL: CARMA2_CHEAT 0x006a9e3c
int gNumber_of_lollipops;

// GLOBAL: CARMA2_HW 0x00762100
// GLOBAL: CARMA2_SW 0x00758660
// GLOBAL: CARMA2_CHEAT 0x007697a0
br_scalar gYon_squared;

// MungeClipPlane

// TryThisEdge

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e7650
// STUB: CARMA2_SW 0x004e3470
void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e74d0
// FUNCTION: CARMA2_SW 0x004e32f0
// FUNCTION: CARMA2_CHEAT 0x004ed170
void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform) {
    int i;
    int cat;
    int car_count;
    tCar_spec* the_car;
    br_vector3 camera_to_car;
    br_scalar distance_factor;

    if (gShadow_level != eShadow_none) {
        for (cat = eVehicle_self; cat <= (gShadow_level == eShadow_everyone ? eVehicle_drone : gShadow_level == eShadow_us_and_opponents ? eVehicle_rozzer : eVehicle_self); cat++) {
            if (cat == eVehicle_self) {
                car_count = 1;
            } else {
                car_count = GetCarCount(cat);
            }
            for (i = 0; i < car_count; i++) {
                if (cat == eVehicle_self) {
                    the_car = &gProgram_state.current_car;
                } else {
                    the_car = GetCarSpec(cat, i);
                }
                if (!the_car->active) {
                    continue;
                }
                if (IsCarCloaked(the_car)) {
                    continue;
                }
                if (!gAction_replay_mode) {
                    BrVector3Sub(&camera_to_car, (br_vector3*)gCamera_to_world.m[3], &the_car->car_master_actor->t.t.translate.t);
                    distance_factor = BrVector3LengthSquared(&camera_to_car);
                    if (distance_factor > 36.0f) {
                        continue;
                    }
                }
                ProcessShadow(the_car, gUniverse_actor, &gProgram_state.track_spec, gCamera, &gCamera_to_world, distance_factor);
            }
        }
        if (gFancy_shadow) {
            for (i = 0; i < gSaved_table_count; i++) {
                gSaved_shade_tables[i].original->height = gSaved_shade_tables[i].copy->height;
                gSaved_shade_tables[i].original->pixels = gSaved_shade_tables[i].copy->pixels;
                BrTableUpdate(gSaved_shade_tables[i].original, BR_TABU_ALL);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e9940
// FUNCTION: CARMA2_SW 0x004e5700
// FUNCTION: CARMA2_CHEAT 0x004ef5e0
void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel) {

    gShadow_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x004e9950
// FUNCTION: CARMA2_SW 0x004e5710
// FUNCTION: CARMA2_CHEAT 0x004ef5f0
tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void) {

    return gShadow_level;
}

// FUNCTION: CARMA2_HW 0x004e9960
// FUNCTION: CARMA2_SW 0x004e5720
// FUNCTION: CARMA2_CHEAT 0x004ef600
void C2_HOOK_FASTCALL ToggleShadow(void) {

    if (gShadow_level++ == eShadow_everyone) {
        gShadow_level = eShadow_none;
    }
    switch (gShadow_level) {
    case eShadow_none:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_ShadowNone));
        break;
    case eShadow_us_only:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_ShadowUsOnly));
        break;
    case eShadow_us_and_opponents:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_ShadowUsAndOpponents));
        break;
    case eShadow_everyone:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_ShadowEveryone));
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004e99d0
// FUNCTION: CARMA2_SW 0x004e5790
// FUNCTION: CARMA2_CHEAT 0x004ef670
void C2_HOOK_FASTCALL InitShadow(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(gShadow_clip_planes[0]) != 8);

    for (i = 0; i < (int)REC2_ASIZE(gShadow_clip_planes); i++) {
        gShadow_clip_planes[i].clip = BrActorAllocate(BR_ACTOR_CLIP_PLANE, NULL);
        BrActorAdd(gUniverse_actor, gShadow_clip_planes[i].clip);
        BrClipPlaneDisable(gShadow_clip_planes[i].clip);
        BrMatrix34Identity(&gShadow_clip_planes[i].clip->t.t.mat);
    }
    gFancy_shadow = 1;
    gShadow_material = BrMaterialFind("SHADOW.MAT");
    BrVector3Set(&gShadow_light_ray, 0.f, -1.f, 0.f);
    BrVector3Set(&gShadow_light_z, 0.f, 0.f, -1.f);
    BrVector3Set(&gShadow_light_x, 1.f, 0.f, 0.f);

    gShadow_model = BrModelAllocate("", 0, 0);
    gShadow_model->flags = BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL;
    gShadow_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gShadow_actor->model = gShadow_model;
    BrActorAdd(gUniverse_actor, gShadow_actor);
}

// FUNCTION: CARMA2_HW 0x004e9b10
// FUNCTION: CARMA2_SW 0x004e58d0
// FUNCTION: CARMA2_CHEAT 0x004ef7b0
br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg) {
    br_pixelmap* copy;

    (void)pArg;
    C2_HOOK_BUG_ON(REC2_ASIZE(gSaved_shade_tables) != 100);
    C2_HOOK_BUG_ON(sizeof(br_pixelmap) != 0x44);

    if (gSaved_table_count == (int)REC2_ASIZE(gSaved_shade_tables)) {
        return 1;
    } else {
        gSaved_shade_tables[gSaved_table_count].original = pTable;
        copy = BrMemAllocate(sizeof(br_pixelmap), kMem_misc);
        gSaved_shade_tables[gSaved_table_count].copy = copy;
        gSaved_table_count++;
        memcpy(copy, pTable, sizeof(br_pixelmap));
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e9ae0
// FUNCTION: CARMA2_SW 0x004e58a0
// FUNCTION: CARMA2_CHEAT 0x004ef780
void C2_HOOK_FASTCALL SaveShadeTables(void) {
    PossibleService();
    gSaved_table_count = 0;
    BrTableEnum("*", SaveShadeTable, NULL);
}

// FUNCTION: CARMA2_HW 0x004e9b60
// FUNCTION: CARMA2_SW 0x004e5920
// FUNCTION: CARMA2_CHEAT 0x004ef800
void C2_HOOK_FASTCALL DisposeSavedShadeTables(void) {
    int i;

    for (i = 0; i < gSaved_table_count; i++) {
        BrMemFree(gSaved_shade_tables[i].copy);
    }
}

// FUNCTION: CARMA2_HW 0x004e9b90
// FUNCTION: CARMA2_SW 0x004e5950
// FUNCTION: CARMA2_CHEAT 0x004ef830
void C2_HOOK_FASTCALL ShadowMode(void) {

    gFancy_shadow = !gFancy_shadow;
    if (gFancy_shadow) {
        NewTextHeadupSlot(4, 0, 2000, -4, "Translucent shadow");
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, "Solid shadow");
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e4650
// FUNCTION: CARMA2_SW 0x004e09e0
// FUNCTION: CARMA2_CHEAT 0x004ea1d0
void C2_HOOK_FASTCALL LollipopizeActor(br_actor* pSubject_actor, const br_matrix34* ref_to_world, tLollipop_mode pWhich_axis) {
    br_vector3 ref_to_subject;
    br_vector3 fixed_axis;
    br_vector3 vector_a;
    br_vector3 vector_b;
    br_matrix34 subject_to_world;
    br_matrix34 mat;

    BrActorToActorMatrix34(&subject_to_world, pSubject_actor, gNon_track_actor);
    BrVector3Sub(&ref_to_subject, (const br_vector3*)ref_to_world->m[3], (br_vector3*)subject_to_world.m[3]);
    switch (pWhich_axis) {
    case eLollipop_x_match:
        BrVector3SetFloat(&vector_a, 1.0f, 0.0f, 0.0f);
        break;
    case eLollipop_y_match:
        BrVector3SetFloat(&vector_a, 0.0f, 1.0f, 0.0f);
        break;
    case eLollipop_z_match:
        BrVector3SetFloat(&vector_a, 0.0f, 0.0f, 1.0f);
        break;
    default:
        break;
    }
    BrVector3Cross(&vector_b, &ref_to_subject, &vector_a);
    BrVector3Normalise(&vector_b, &vector_b);

    BrVector3Cross(&fixed_axis, &vector_a, &vector_b);

    switch (pWhich_axis) {
    case eLollipop_x_match:
        mat.m[0][0] = vector_a.v[0];
        mat.m[1][0] = vector_a.v[1];
        mat.m[2][0] = vector_a.v[2];
        mat.m[0][1] = vector_b.v[0];
        mat.m[1][1] = vector_b.v[1];
        mat.m[2][1] = vector_b.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.0f;
        mat.m[3][1] = 0.0f;
        mat.m[3][2] = 0.0f;
        break;
    case eLollipop_y_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = vector_a.v[0];
        mat.m[1][1] = vector_a.v[1];
        mat.m[2][1] = vector_a.v[2];
        mat.m[0][2] = fixed_axis.v[0];
        mat.m[1][2] = fixed_axis.v[1];
        mat.m[2][2] = fixed_axis.v[2];
        mat.m[3][0] = 0.0f;
        mat.m[3][1] = 0.0f;
        mat.m[3][2] = 0.0f;
        break;
    case eLollipop_z_match:
        mat.m[0][0] = vector_b.v[0];
        mat.m[1][0] = vector_b.v[1];
        mat.m[2][0] = vector_b.v[2];
        mat.m[0][1] = fixed_axis.v[0];
        mat.m[1][1] = fixed_axis.v[1];
        mat.m[2][1] = fixed_axis.v[2];
        mat.m[0][2] = vector_a.v[0];
        mat.m[1][2] = vector_a.v[1];
        mat.m[2][2] = vector_a.v[2];
        mat.m[3][0] = 0.0f;
        mat.m[3][1] = 0.0f;
        mat.m[3][2] = 0.0f;
        break;
    default:
        break;
    }
    BrMatrix34Pre(&pSubject_actor->t.t.mat, &mat);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4900
// FUNCTION: CARMA2_SW 0x004e0c90
// FUNCTION: CARMA2_CHEAT 0x004ea480
void C2_HOOK_FASTCALL ResetLollipopQueue(void) {
    gNumber_of_lollipops = 0;
}

// RenderLollipops

// FUNCTION: CARMA2_HW 0x004e4940
// FUNCTION: CARMA2_SW 0x004e0cd0
// FUNCTION: CARMA2_CHEAT 0x004ea4c0
void C2_HOOK_FASTCALL SetupDepthBuffer(void) {

    if (gDepth_buffer != NULL) {
        BrPixelmapFree(gDepth_buffer);
        gDepth_buffer = NULL;
    }
    gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
    if (gDepth_buffer == NULL) {
        FatalError(kFatalError_AllocateZBuffer);
    }
}

// FUNCTION: CARMA2_HW 0x004e4980
// FUNCTION: CARMA2_SW 0x004e0d10
// FUNCTION: CARMA2_CHEAT 0x004ea500
void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight) {

    PDAllocateScreenAndBack();
    if (pWidth == 0) {
        pWidth = gBack_screen->width;
    }
    if (pHeight == 0) {
        pHeight = gBack_screen->height;
    }
    gRender_screen = DRPixelmapAllocateSub(gBack_screen, (br_uint_16)pX_offset, (br_uint_16)pY_offset, (br_uint_16)pWidth, (br_uint_16)pHeight);
    gWidth = pWidth;
    gHeight = pHeight;
    gX_offset = pX_offset;
    gY_offset = pY_offset;
    if (gGraf_specs[gGraf_spec_index].doubled) {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - 2 * gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - 2 * gGraf_specs[gGraf_spec_index].total_height) / 2;
    } else {
        gScreen->base_x = (gGraf_specs[gGraf_spec_index].phys_width - gGraf_specs[gGraf_spec_index].total_width) / 2;
        gScreen->base_y = (gGraf_specs[gGraf_spec_index].phys_height - gGraf_specs[gGraf_spec_index].total_height) / 2;
    }

    gScreen->origin_x = 0;
    gScreen->origin_y = 0;
    if (gBack_screen == NULL) {
        FatalError(kFatalError_AllocateOffScreenBuffer);
    }

    SetupDepthBuffer();

    BrZbsBegin(gRender_screen->type, gDepth_buffer->type, gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
    gBrZb_initialized = 1;
    gRear_pixelmap = DRPixelmapAllocate(gScreen->type, 64, 64, NULL, 0);
    BrMapAdd(gRear_pixelmap);
}

// FUNCTION: CARMA2_HW 0x004e4b40
// FUNCTION: CARMA2_SW 0x004e0ed0
// FUNCTION: CARMA2_CHEAT 0x004ea6c0
void C2_HOOK_FASTCALL AdjustRenderScreenSize(void) {

    ReinitialiseRenderStuff();
    if (gMap_view == 2) {
        gRender_screen->base_x = (br_uint_16)gMap_render_x_i;
        gRender_screen->base_y = (br_uint_16)gMap_render_y_i;
        gRender_screen->width = (br_uint_16)gMap_render_width_i;
        gRender_screen->height = (br_uint_16)gMap_render_height_i;
    } else {
        gRender_screen->base_x = (br_uint_16)gProgram_state.current_render_left;
        gRender_screen->base_y = (br_uint_16)gProgram_state.current_render_top;
        gRender_screen->height = (br_uint_16)(gProgram_state.current_render_bottom - gProgram_state.current_render_top);
        gRender_screen->width = (br_uint_16)(gProgram_state.current_render_right - gProgram_state.current_render_left);
    }
    if (gRender_screen->row_bytes != gRender_screen->width) {
        gRender_screen->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
    } else {
        gRender_screen->flags |= BR_PMF_ROW_WHOLEPIXELS;
    }
    gRender_screen->origin_x = gRender_screen->width / 2;
    gRender_screen->origin_y = gRender_screen->height / 2;
    gWidth = gRender_screen->width;
    gHeight = gRender_screen->height;
    ReinitialiseForwardCamera();
#ifdef REC2_HW
    SetTintedPolySize(gHud_tinted1,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
    SetTintedPolySize(gHud_tinted2,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
    SetTintedPolySize(gHud_tinted4,
        gRender_screen->base_x,
        gRender_screen->base_y,
        gRender_screen->width,
        gRender_screen->height);
#endif
}

// FUNCTION: CARMA2_HW 0x004e4cd0
// FUNCTION: CARMA2_SW 0x004e0fe0
// FUNCTION: CARMA2_CHEAT 0x004ea850
void C2_HOOK_FASTCALL ScreenSmaller(void) {

    if (gMap_view != 2) {
        gRender_indent++;
        if (gRender_indent > 8) {
            gRender_indent = 8;
        }
        AdjustRenderScreenSize();
#ifdef REC2_SW
        gDepth_buffer->row_bytes = 2 * gRender_screen->width;
        gDepth_buffer->base_x = gRender_screen->base_x;
#endif
    }
}

// FUNCTION: CARMA2_HW 0x004e4d00
// FUNCTION: CARMA2_SW 0x004e1040
// FUNCTION: CARMA2_CHEAT 0x004ea880
void C2_HOOK_FASTCALL ScreenLarger(void) {

    if (gMap_view != 2) {
        gRender_indent--;
        if (gRender_indent < 0) {
            gRender_indent = 0;
        }
        AdjustRenderScreenSize();
#ifdef REC2_SW
        gDepth_buffer->row_bytes = 2 * gRender_screen->width;
        gDepth_buffer->base_x = gRender_screen->base_x;
#endif
    }
}

// FUNCTION: CARMA2_HW 0x004e4d30
// FUNCTION: CARMA2_SW 0x004e1090
// FUNCTION: CARMA2_CHEAT 0x004ea8b0
void C2_HOOK_FASTCALL ClearWobbles(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); ++i) {
        gWobble_array[i].time_started = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e4d50
// FUNCTION: CARMA2_SW 0x004e10b0
// FUNCTION: CARMA2_CHEAT 0x004ea8d0
void C2_HOOK_FASTCALL InitWobbleStuff(void) {
    int i;

    ClearWobbles();
    for (i = 0; i < (int)REC2_ASIZE(gCosine_array); i++) {
        gCosine_array[i] = (float)cos((double)i / 64.0 * 3.141592653589793 / 2.0);
    }

    gEvalu = LoadPixelmap("Evalu01.PIX");
}

// FUNCTION: CARMA2_HW 0x004e4db0
// FUNCTION: CARMA2_SW 0x004e1110
// FUNCTION: CARMA2_CHEAT 0x004ea930
void C2_HOOK_FASTCALL NewScreenWobble(double pAmplitude_x, double pAmplitude_y, double pPeriod) {
    int i;
    int oldest_time;
    int oldest_index;

    oldest_index = -1;
    oldest_time = INT_MAX;
    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); i++) {
        if (gWobble_array[i].time_started == 0) {
            oldest_index = i;
            break;
        } else if (gWobble_array[i].time_started < oldest_time) {
            oldest_time = gWobble_array[i].time_started;
            oldest_index = i;
        }
    }
    gWobble_array[oldest_index].time_started = (int)GetTotalTime();
    gWobble_array[oldest_index].amplitude_x = (float)pAmplitude_x;
    gWobble_array[oldest_index].amplitude_y = (float)pAmplitude_y;
    gWobble_array[oldest_index].period = (float)pPeriod;
}

// FUNCTION: CARMA2_HW 0x004e4e20
// FUNCTION: CARMA2_SW 0x004e1180
// FUNCTION: CARMA2_CHEAT 0x004ea9a0
void C2_HOOK_FASTCALL SetScreenWobble(int pX, int pY) {
    gScreen_wobble_x = pX;
    gScreen_wobble_y = pY;
}

// FUNCTION: CARMA2_HW 0x004e4e30
// FUNCTION: CARMA2_SW 0x004e1190
// FUNCTION: CARMA2_CHEAT 0x004ea9b0
void C2_HOOK_FASTCALL ResetScreenWobble(void) {
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
}

inline void C2_HOOK_FASTCALL CalculateWobblitude(tU32 pThe_time) {
    int i;
    tU32 time_going;
    double angle;
    double mod_angle;
    float cosine_over_angle;

    if (gProgram_state.new_view != eView_undefined) {
        return;
    }
    gScreen_wobble_x = 0;
    gScreen_wobble_y = 0;
    for (i = 0; i < (int)REC2_ASIZE(gWobble_array); i++) {
        if (gWobble_array[i].time_started != 0) {
            time_going = pThe_time - gWobble_array[i].time_started;
            if (time_going > 1000) {
                gWobble_array[i].time_started = 0;
            } else {
                mod_angle = fmod(time_going / gWobble_array[i].period, REC2_TAU);
                if (mod_angle > 1.5 * REC2_PI) {
                    cosine_over_angle = gCosine_array[(unsigned int)((REC2_TAU - mod_angle) / REC2_PI * 128.0)];
                } else if (mod_angle > REC2_PI) {
                    cosine_over_angle = -gCosine_array[(unsigned int)((mod_angle - REC2_PI) / REC2_PI * 128.0)];
                } else if (mod_angle > 0.5 * REC2_PI) {
                    cosine_over_angle = -gCosine_array[(unsigned int)((REC2_PI - mod_angle) / REC2_PI * 128.0)];
                } else {
                    cosine_over_angle = gCosine_array[(unsigned int)(mod_angle / REC2_PI * 128.0)];
                }
                angle = cosine_over_angle / (1.0 + time_going * 0.0035);
                gScreen_wobble_x = (int)(gScreen_wobble_x + gWobble_array[i].amplitude_x * angle);
                gScreen_wobble_y = (int)(gScreen_wobble_y + gWobble_array[i].amplitude_y * angle);
            }
        }
    }
    if (gScreen_wobble_x > gCurrent_graf_data->cock_margin_x) {
        gScreen_wobble_x = gCurrent_graf_data->cock_margin_x;
    } else if (gScreen_wobble_x < -gCurrent_graf_data->cock_margin_x) {
        gScreen_wobble_x = -gCurrent_graf_data->cock_margin_x;
    }
    if (gScreen_wobble_y > gCurrent_graf_data->cock_margin_y) {
        gScreen_wobble_y = gCurrent_graf_data->cock_margin_y;
    } else if (gScreen_wobble_y < -gCurrent_graf_data->cock_margin_y) {
        gScreen_wobble_y = -gCurrent_graf_data->cock_margin_y;
    }
    PipeSingleScreenWobble(gScreen_wobble_x, gScreen_wobble_y);
}

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e5680
// STUB: CARMA2_SW 0x004e1a40
// STUB: CARMA2_CHEAT 0x004eb200
void C2_HOOK_FASTCALL DoARenderPass(br_matrix34* pMat34, br_actor* pCamera, br_pixelmap* pColour, br_pixelmap* pDepth, br_scalar pYon_factor, int pShadows, int pEffects, int pArg8) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e54f0
// FUNCTION: CARMA2_SW 0x004e18b0
// FUNCTION: CARMA2_CHEAT 0x004eb070
void C2_HOOK_FASTCALL DoACompleteRenderPass(int pMirror, br_matrix34* pCamera_to_world, br_actor* pCamera, br_pixelmap* pScreen, br_pixelmap* pDepth) {
    int i;
    br_matrix34 mat;
    tExtra_render* extra_render;

    gRendering_mirror = pMirror;
    if (pMirror && gPrevious_rear_yon != ((br_camera*)pCamera->type_data)->yon_z) {
        gPrevious_rear_yon = ((br_camera*)pCamera->type_data)->yon_z;
        MungeRearviewSky();
    }
    if (!gRendering_mirror) {
        gRear_pixelmap->base_x = 0;
        gRear_pixelmap->base_y = 0;
        for (i = 0; i < gCount_extra_renders; i++) {
            extra_render = &gExtra_renders[i];
            gRear_pixelmap->origin_x = extra_render->material->colour_map->width / 2;
            gRear_pixelmap->origin_y = extra_render->material->colour_map->height / 2;
            gRear_pixelmap->width = extra_render->material->colour_map->width;
            gRear_pixelmap->height = extra_render->material->colour_map->height;
            if (gRear_pixelmap->row_bytes != gRear_pixelmap->width) {
                gRear_pixelmap->flags &= ~BR_PMF_ROW_WHOLEPIXELS;
            } else {
                gRear_pixelmap->flags |= BR_PMF_ROW_WHOLEPIXELS;
            }
            BrActorToActorMatrix34(&mat, extra_render->actor, gUniverse_actor);
            DoARenderPass(&mat, extra_render->actor, gRear_pixelmap, gDepth_buffer, 1.0f, 0, 0, 0);
            DRPixelmapCopyMapBlack(extra_render->material->colour_map, gRear_pixelmap);
            BrMaterialUpdate(extra_render->material, BR_MATU_COLOURMAP);
        }
    }
    DoARenderPass(pCamera_to_world, pCamera, pScreen, pDepth, 1.0f, !pMirror, !pMirror, !pMirror);
    gRendering_mirror = 0;
}

// FUNCTION: CARMA2_HW 0x004e4e40
// FUNCTION: CARMA2_SW 0x004e11a0
// FUNCTION: CARMA2_CHEAT 0x004ea9c0
void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on) {
    int i;
    int x_shift;
    int y_shift;
    int cockpit_on;
    char* old_pixels;
    br_matrix34 old_camera_matrix;
    br_matrix34 old_mirror_cam_matrix;
    tU32 the_time;

    gRender_screen->pixels = gBack_screen->pixels;
    the_time = GetTotalTime();
    old_pixels = gRender_screen->pixels;
    if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0 && gMap_view != 2) {
        cockpit_on = 1;
    } else {
        cockpit_on = 0;
    }
    if (gProgram_state.mirror_on && cockpit_on && gProgram_state.which_view == eView_forward) {
        gMirror_on__graphics = 1;
    } else {
        gMirror_on__graphics = 0;
    }
    StartRenderingHeadups();
    MapStuffBeforeRender();
    if (!gAction_replay_mode) {
        CalculateWobblitude(the_time);
    }
    if (cockpit_on) {
        if (-gScreen_wobble_x > gX_offset) {
            x_shift = -gX_offset;
        } else if (gBack_screen->width < gX_offset + gScreen_wobble_x + gRender_screen->width) {
            x_shift = gBack_screen->width - gX_offset - gRender_screen->width;
        } else {
            x_shift = gScreen_wobble_x;
        }
        if (-gScreen_wobble_y > gY_offset) {
            y_shift = -gY_offset;
        } else if (gBack_screen->height < gY_offset + gScreen_wobble_y + gRender_screen->height) {
            y_shift = gBack_screen->height - gY_offset - gRender_screen->height;
        } else {
            y_shift = gScreen_wobble_y;
        }
    } else {
        x_shift = 0;
        y_shift = 0;
    }
    old_camera_matrix = gCamera->t.t.mat;
    if (gMirror_on__graphics) {
        old_mirror_cam_matrix = gRearview_camera->t.t.mat;
    }
    if (cockpit_on) {
        gSheer_mat.m[2][1] = y_shift / (float)gRender_screen->height;
        gSheer_mat.m[2][0] = (float)(-x_shift / (double)gRender_screen->width);
        BrMatrix34Pre(&gCamera->t.t.mat, &gSheer_mat);
        gCamera->t.t.translate.t.v[0] -= gScreen_wobble_x * 1.5f / (float)gRender_screen->width * (1.0f / WORLD_SCALE);
        gCamera->t.t.translate.t.v[1] += (gScreen_wobble_y * 1.5f / (float)gRender_screen->width * (1.0f / WORLD_SCALE));
    }
    gRender_screen->pixels = (char*)gRender_screen->pixels + x_shift + y_shift * gRender_screen->row_bytes;
    if (gRender_indent && gMap_view != 2) {
        BrPixelmapRectangleFill(
            gBack_screen,
            0,
            0,
            gGraf_specs[gGraf_spec_index].total_width,
            gProgram_state.current_render_top,
            0);
        BrPixelmapRectangleFill(
            gBack_screen,
            0,
            gProgram_state.current_render_bottom,
            gGraf_specs[gGraf_spec_index].total_width,
            gGraf_specs[gGraf_spec_index].total_height - gProgram_state.current_render_bottom,
            0);
        BrPixelmapRectangleFill(
            gBack_screen,
            0,
            gProgram_state.current_render_top,
            gProgram_state.current_render_left,
            gProgram_state.current_render_bottom - gProgram_state.current_render_top,
            0);
        BrPixelmapRectangleFill(
            gBack_screen,
            gProgram_state.current_render_right,
            gProgram_state.current_render_top,
            gGraf_specs[gGraf_spec_index].total_width - gProgram_state.current_render_right,
            gProgram_state.current_render_bottom - gProgram_state.current_render_top,
            0);
    }
    gRendering_mirror = 0;
#ifdef REC2_SW
    for (i = 0; i < (gMap_view == 2 ? 3 : 1); i++) {
#else
    for (i = 0; i < 1; i++) {
#endif
        DoACompleteRenderPass(0, &gCamera_to_world, gCamera, gRender_screen, gDepth_buffer);
    }
    gCamera->t.t.mat = old_camera_matrix;
    if (gMirror_on__graphics) {
        DoACompleteRenderPass(1, &gRearview_camera_to_world, gRearview_camera, gRearview_screen, gRearview_depth_buffer);
        gRearview_camera->t.t.mat = old_mirror_cam_matrix;
    }
    RenderTintedPolys();
    if (gMap_view != 2) {
        DimAFewBits();
#ifdef REC2_SW
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DoDamageScreen(the_time);
        }
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DoHeadups(the_time);
        }
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DoInstruments(the_time);
        }
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DrawPowerups(the_time);
        }
#else
        DoDamageScreen(the_time);
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DoHeadups(the_time);
        }
        DoInstruments(the_time);
        if (!gAction_replay_mode || gAR_fudge_headups) {
            DrawPowerups(the_time);
        }
#endif
    }
#ifdef REC2_SW
    if (gHeadup_detail_level == 1 || gHeadup_detail_level == 4) {
        DRPixelmapText(gBack_screen, 280 - DRTextWidth(&gFonts[2], gSW_debug_text), 2, &gFonts[2], gSW_debug_text, 319);
    }
#else
    if (gMap_view != 2 && (!gAction_replay_mode || gAR_fudge_headups)) {
        DoTestHeadup();
        DoPSPowerupHeadups();
    }
#endif
    MapStuffAfterRender();
    if (gNet_mode != eNet_mode_none) {
        DisplayUserMessage();
    }
    if (gAction_replay_mode) {
        MovieRecordFrame(gBack_screen, gFrame_period);
    }
    if (gAction_replay_mode && !gAR_fudge_headups) {
        DoActionReplayHeadups();
    }
    StopRenderingHeadups();
    if (gAction_replay_mode) {
        SynchronizeActionReplay();
    } else {
        PipeSingleFrameFinish();
    }
    gRender_screen->pixels = old_pixels;
    if (!gPalette_fade_time || GetRaceTime() > gPalette_fade_time + 500) {
        PDScreenBufferSwap(0);
    }
    if (gAction_replay_mode) {
        DoActionReplayPostSwap();
    }
}

// FUNCTION: CARMA2_HW 0x004e5a70
// FUNCTION: CARMA2_SW 0x004e1c30
// FUNCTION: CARMA2_CHEAT 0x004eb780
void C2_HOOK_FASTCALL StartRenderingHeadups(void) {
    CleanPolyFontDanglers();
#ifdef REC2_HW
    gHud_actor_storage_size = 0;
    gRender_poly_text = 0;
#endif
}

// FUNCTION: CARMA2_HW 0x004e5ad0
// FUNCTION: CARMA2_SW 0x004e1c70
// FUNCTION: CARMA2_CHEAT 0x004eb7e0
void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor) {
#ifdef REC2_HW
    if (gHud_actor_storage_size >= (int)REC2_ASIZE(gHud_actor_storage)) {
        BrFailure("Not enough HUD actor storage");
    }
    gHud_actor_storage[gHud_actor_storage_size] = pActor;
    gHud_actor_storage_size += 1;
#endif
}

// FUNCTION: CARMA2_HW 0x004e5b00
// FUNCTION: CARMA2_SW 0x004e1c80
// FUNCTION: CARMA2_CHEAT 0x004eb810
void C2_HOOK_FASTCALL StopRenderingHeadups(void) {
#ifdef REC2_HW
    int original_origin_x;
    int original_origin_y;
    int original_base_x;
    int original_base_y;
    br_actor* actor;
    int i;

    if (gCount_polyfont_glyph_actors != 0) {
        BrActorAdd(gHUD_root, gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->prev == NULL) {
            BrActorAdd(gHUD_root, actor);
        }
    }
    original_origin_x = gRender_screen->origin_x;
    original_origin_y = gRender_screen->origin_y;
    original_base_x = gRender_screen->base_x;
    original_base_y = gRender_screen->base_y;
    gRender_screen->origin_x = 0;
    gRender_screen->origin_y = 0;
    gRender_screen->base_x = 0;
    gRender_screen->base_y = 0;
    BrZbSceneRender(gHUD_root, gHUD_camera, gRender_screen, gDepth_buffer);
    gRender_screen->origin_x = original_origin_x;
    gRender_screen->origin_y = original_origin_y;
    gRender_screen->base_x = original_base_x;
    gRender_screen->base_y = original_base_y;
    if (gString_root_actor->parent != NULL) {
        BrActorRemove(gString_root_actor);
    }
    for (i = 0; i < gHud_actor_storage_size; i++) {
        actor = gHud_actor_storage[i];
        if (actor != NULL && actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gHud_actor_storage_size = 0;
    CleanPolyFontDanglers();
    gRender_poly_text = 1;
#endif
}

// FUNCTION: CARMA2_HW 0x004e5c70
// FUNCTION: CARMA2_SW 0x004e1c90
// FUNCTION: CARMA2_CHEAT 0x004eb980
void C2_HOOK_FASTCALL CleanPolyFontDanglers(void) {
    int i;

    for (i = 0; i < gCount_polyfont_glyph_actors; i++) {
        br_actor* actor = gPolyfont_glyph_actors[i];
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
    }
    gCount_polyfont_glyph_actors = 0;
}

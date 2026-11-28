/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "33-depth.h"

#include "32-spark.h"

#include "00-car.h"
#include "07-structur.h"
#include "15-displays.h"
#include "17-world2.h"
#include "20-skidmark.h"
#include "36-trig.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "52-errors.h"
#include "55-volume.h"
#include "66-tintedpoly.h"
#include "globvars.h"
#include "globvrkm.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_stdio.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x00591188
// GLOBAL: CARMA2_SW 0x0058c180
// GLOBAL: CARMA2_CHEAT 0x00597ae0
int gSky_on = 1;

// GLOBAL: CARMA2_HW 0x0079ec1c
// GLOBAL: CARMA2_SW 0x0079517c
// GLOBAL: CARMA2_CHEAT 0x007a62bc
br_pixelmap* gSwap_sky_texture;

// GLOBAL: CARMA2_HW 0x0067c4e0
// GLOBAL: CARMA2_SW 0x00676c58
// GLOBAL: CARMA2_CHEAT 0x00683d88
br_material* gHorizon_material;

// GLOBAL: CARMA2_HW 0x0067c4a0
// GLOBAL: CARMA2_SW 0x00676c18
// GLOBAL: CARMA2_CHEAT 0x00683d48
br_model* gForward_sky_model;

// GLOBAL: CARMA2_HW 0x0059118c
// GLOBAL: CARMA2_SW 0x0058c184
// GLOBAL: CARMA2_CHEAT 0x00597ae4
int gDepth_cueing_on = 1;

// GLOBAL: CARMA2_HW 0x00591190
// GLOBAL: CARMA2_SW 0x0058c188
// GLOBAL: CARMA2_CHEAT 0x00597ae8
tDepth_effect_type gSwap_depth_effect_type = eDepth_effect_none;

// GLOBAL: CARMA2_HW 0x0079ec4c
// GLOBAL: CARMA2_CHEAT 0x007a62ec
int gSwap_depth_effect_start;

// GLOBAL: CARMA2_HW 0x0079ec48
// GLOBAL: CARMA2_CHEAT 0x007a62e8
int gSwap_depth_effect_end;

// GLOBAL: CARMA2_HW 0x0079ec34
// GLOBAL: CARMA2_CHEAT 0x007a62d4
int gSwap_depth_effect_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ec3c
// GLOBAL: CARMA2_CHEAT 0x007a62dc
int gSwap_depth_effect_colour_red;

// GLOBAL: CARMA2_HW 0x0079ec40
// GLOBAL: CARMA2_CHEAT 0x007a62e0
int gSwap_depth_effect_colour_green;

// GLOBAL: CARMA2_HW 0x0067c4a8
// GLOBAL: CARMA2_SW 0x00676c20
// GLOBAL: CARMA2_CHEAT 0x00683d50
int gHas_sky_texture;

// GLOBAL: CARMA2_HW 0x0079ec20
// GLOBAL: CARMA2_SW 0x00795180
// GLOBAL: CARMA2_CHEAT 0x007a62c0
br_pixelmap* gDepth_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4bc
// GLOBAL: CARMA2_SW 0x00676c34
// GLOBAL: CARMA2_CHEAT 0x00683d64
int gDepth_shade_table_power;

// GLOBAL: CARMA2_HW 0x0067c4a4
// GLOBAL: CARMA2_SW 0x00676c1c
// GLOBAL: CARMA2_CHEAT 0x00683d4c
int gFog_shade_table_power;

// GLOBAL: CARMA2_HW 0x0067c4dc
// GLOBAL: CARMA2_SW 0x00676c54
// GLOBAL: CARMA2_CHEAT 0x00683d84
int gAcid_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec28
// GLOBAL: CARMA2_SW 0x00795188
// GLOBAL: CARMA2_CHEAT 0x007a62c8
br_pixelmap* gWater_shade_table;

// GLOBAL: CARMA2_HW 0x0079ec2c
// GLOBAL: CARMA2_SW 0x0079518c
// GLOBAL: CARMA2_CHEAT 0x007a62cc
br_angle gSky_image_height;

// GLOBAL: CARMA2_HW 0x0079ec30
// GLOBAL: CARMA2_SW 0x00795190
// GLOBAL: CARMA2_CHEAT 0x007a62d0
br_angle gSky_image_underground;

// GLOBAL: CARMA2_HW 0x0067c4cc
// GLOBAL: CARMA2_SW 0x00676c44
// GLOBAL: CARMA2_CHEAT 0x00683d74
int gWater_shade_table_power;

// GLOBAL: CARMA2_HW 0x0067c4d8
// GLOBAL: CARMA2_SW 0x00676c50
// GLOBAL: CARMA2_CHEAT 0x00683d80
br_actor* gForward_sky_actor;

// GLOBAL: CARMA2_HW 0x0079ec44
// GLOBAL: CARMA2_SW 0x007951a4
// GLOBAL: CARMA2_CHEAT 0x007a62e4
tSpecial_volume* gLast_camera_special_volume;

static void C2_HOOK_FASTCALL FrobFog(void);

int C2_HOOK_CDECL Log2(int pNumber) {
    int i;
    int bits[16] = {
        0x1,
        0x2,
        0x4,
        0x8,
        0x10,
        0x20,
        0x40,
        0x80,
        0x100,
        0x200,
        0x400,
        0x800,
        0x1000,
        0x2000,
        0x4000,
        0x8000,
    };
    for (i = REC2_ASIZE(bits) - 1; i >= 0; --i) {
        if (bits[i] & pNumber) {
            return i;
        }
    }
    return 0;
}

// CalculateWrappingMultiplier

br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance) {

    return gCamera_yon * (br_scalar)pow(10.0, 0.1 * (double)pDistance);
}

// FUNCTION: CARMA2_HW 0x004451a0
// FUNCTION: CARMA2_CHEAT 0x004456c0
void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial) {
    int start;
    int end;
    int red;
    int green;
    int blue;

    red = gProgram_state.current_depth_effect.colour.red;
    green = gProgram_state.current_depth_effect.colour.green;
    blue = gProgram_state.current_depth_effect.colour.blue;
    start = gProgram_state.current_depth_effect.start;
    end = gProgram_state.current_depth_effect.end;

    switch (gProgram_state.current_depth_effect.type) {
    case eDepth_effect_none:
        pMaterial->flags &= ~BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_darkness:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = 0;
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_fog:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(0xf8, 0xf8, 0xf8);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_colour:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(red, green, blue);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

void C2_HOOK_FASTCALL FogCars(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        int j;
        tCar_spec* car = gCurrent_race.opponent_list[i].car_spec;

        for (j = 0; j < car->count_detail_levels; j++) {
            SwitchCarModels(car, j);
            ProcessMaterials(car->car_model_actor, FogAccordingToGPSCDE);
        }
        SwitchCarModels(car, 0);
    }
}

void C2_HOOK_FASTCALL FrobFog(void) {
    int i;
    br_material* material;

    if (gTrack_actor != NULL) {
        ProcessMaterials(gTrack_actor, FogAccordingToGPSCDE);
    }
    if (gNon_track_actor != NULL) {
        ProcessMaterials(gNon_track_actor, FogAccordingToGPSCDE);
    }
    FogCars();

    material = BrMaterialFind("GIBSLICK");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    material = BrMaterialFind("PEDSMEAR");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    for (i = 0; i < (int)REC2_ASIZE(gMaterial); i++) {
        FogAccordingToGPSCDE(gMaterial[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gCurrent_race.material_modifiers); i++) {
        if (gCurrent_race.material_modifiers[i].skid_mark_material != NULL) {
            FogAccordingToGPSCDE(gCurrent_race.material_modifiers[i].skid_mark_material);
        }
    }
    FogAccordingToGPSCDE(gDefault_track_material);
}

// FUNCTION: CARMA2_HW 0x00445500
// FUNCTION: CARMA2_SW 0x00445330
// FUNCTION: CARMA2_CHEAT 0x00445a20
void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial) {
    int i;
    float tex_y;
    float f_height;
    int height;
    int vertex_idx;

    if (pMaterial->colour_map == NULL) {
        return;
    }
    if (pMaterial->flags & BR_MATF_MAP_INTERPOLATION) {
        height = pMaterial->colour_map->height - 2;
        tex_y = 1.0f / (float)pMaterial->colour_map->height;
    } else {
        height = pMaterial->colour_map->height;
        tex_y = 0.0f;
    }
    for (i = 0; i < 12; i++) {
        pModel->vertices[i].map.v[1] = 1.0f - tex_y;
    }
    for (i = 80; i < 88; i++) {
        pModel->vertices[i].map.v[1] = tex_y;
    }
    f_height = (float)height;
    vertex_idx = 12;
    for (i = 0; i < 17; vertex_idx+=4, i++) {
        int j;

        pModel->vertices[vertex_idx].map.v[1] = tex_y + (float)(17 - i) * f_height / 18.0f / pMaterial->colour_map->height;
        for (j = 1; j < 4; j++) {
            pModel->vertices[vertex_idx + j].map.v[1] = pModel->vertices[vertex_idx].map.v[1];
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00445340
// FUNCTION: CARMA2_CHEAT 0x00445860
void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8) {

    if (pType == eDepth_effect_none) {
        pStart = 3;
        pEnd = 3;
    }
    gProgram_state.current_depth_effect.sky_texture = pSky_texture;

    gHorizon_material->colour_map = pSky_texture;
    BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
    MungeSkyVs(gForward_sky_model, gHorizon_material);

    gProgram_state.current_depth_effect.colour.red = pRed;
    gProgram_state.current_depth_effect.colour.green = pGreen;
    gProgram_state.current_depth_effect.colour.blue = pBlue;

    gProgram_state.default_depth_effect.colour.red = pRed;
    gProgram_state.default_depth_effect.colour.green = pGreen;
    gProgram_state.default_depth_effect.colour.blue = pBlue;

    gProgram_state.current_depth_effect.type = pType;
    gProgram_state.current_depth_effect.start = pStart;
    gProgram_state.current_depth_effect.end = pEnd;

    gProgram_state.default_depth_effect.type = pType;
    gProgram_state.default_depth_effect.start = pStart;
    gProgram_state.default_depth_effect.end = pEnd;
    if (gNo_fog && pParam_8) {
        FrobFog();
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline br_scalar C2_HOOK_FASTCALL Tan(br_scalar pAngle) {
    br_scalar s;
    br_scalar c;

    s = (br_scalar)sin(BrAngleToRadian(pAngle));
    c = (br_scalar)cos(BrAngleToRadian(pAngle));
    return s / c;
}

// EdgeU

inline br_model* C2_HOOK_FASTCALL CreateHorizonModel(br_actor* pCamera) {
    tU8 nbands;
    tU8 band;
    int vertex;
    tU8 stripe;
    br_model* model;

    nbands = 21;
    model = BrModelAllocate(NULL, 88, 6 * nbands);
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    for (band = 0; band < nbands; band++) {
        for (stripe = 0; stripe < 3; stripe++) {
            vertex = 6 * band + 2 * stripe;
            model->faces[vertex + 0].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 0].vertices[1] = stripe + 4 * band + 1;
            model->faces[vertex + 0].vertices[2] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 1].vertices[1] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[2] = stripe + 4 * band + 4;
            model->faces[vertex + 0].smoothing = 1;
            model->faces[vertex + 1].smoothing = 1;
            model->faces[vertex + 0].material = NULL;
            model->faces[vertex + 1].material = NULL;
        }
    }
    return model;
}

// FUNCTION: CARMA2_HW 0x00445860
// FUNCTION: CARMA2_SW 0x004455f0
// FUNCTION: CARMA2_CHEAT 0x00445d80
void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower) {
    tPath_name the_path;
    int i;
    int j;
    tU8 temp;

#define PTABLE_PIXEL_AT(X, Y) ((tU8*)((*pTable)->pixels))[(X) + (Y) * (*pTable)->row_bytes]

    PathCat(the_path, gApplication_path, "SHADETAB");
    PathCat(the_path, the_path, pName);
    *pTable = DRPixelmapLoad(the_path);
    if (*pTable == NULL) {
        FatalError(kFatalError_CannotLoadDepthCueShadeTable);
    }
    *pPower = Log2((*pTable)->height);
    for (i = 0; i < (*pTable)->width; i++) {
        for (j = 0; j < (*pTable)->height / 2; j++) {
            temp = PTABLE_PIXEL_AT(i, j);
            PTABLE_PIXEL_AT(i, j) = PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1));
            PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1)) = temp;
        }
    }

#undef PTABLE_PIXEL_AT
}

// FUNCTION: CARMA2_HW 0x00445620
// FUNCTION: CARMA2_SW 0x00445450
// FUNCTION: CARMA2_CHEAT 0x00445b40
void C2_HOOK_FASTCALL InitDepthEffects(void) {

    LoadDepthTable("DEPTHCUE.TAB", &gDepth_shade_table, &gDepth_shade_table_power);
    LoadDepthTable("FOG.TAB", &gFog_shade_table, &gFog_shade_table_power);
    LoadDepthTable("ACIDFOG.TAB", &gAcid_shade_table, &gAcid_shade_table_power);
    LoadDepthTable("BLUEGIT.TAB", &gWater_shade_table, &gWater_shade_table_power);
    GenerateSmokeShades();
    gHorizon_material = BrMaterialFind("HORIZON.MAT");
    if (gHorizon_material == NULL) {
        FatalError(kFatalError_CannotFindSkyMaterial_S);
    }
#ifdef REC2_HW
    if (gScreen->type == BR_PMT_INDEX_8 && !gNo_fog) {
        int i;
        int j;

        gHorizon_material->index_blend = BrPixelmapAllocate(BR_PMT_INDEX_8, 256, 256, NULL, 0);
        BrTableAdd(gHorizon_material->index_blend);
        for (i = 0; i < 256; i++) {
            for (j = 0; j < 256; j++) {
                ((tU8*)gHorizon_material->index_blend->pixels)[256 * i + j] = j;
            }
        }
        gHorizon_material->flags |= BR_MATF_PERSPECTIVE;
    }
    gHorizon_material->flags |= BR_MATF_MAP_INTERPOLATION;
#endif
    gForward_sky_model = CreateHorizonModel(gCamera);
    BrModelAdd(gForward_sky_model);
    gForward_sky_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gForward_sky_actor->model = gForward_sky_model;
    gForward_sky_actor->material = gHorizon_material;
    gForward_sky_actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(gUniverse_actor, gForward_sky_actor);
}

// FUNCTION: CARMA2_HW 0x004459e0
// FUNCTION: CARMA2_SW 0x00445770
// FUNCTION: CARMA2_CHEAT 0x00445f00
void C2_HOOK_FASTCALL ExternalSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    int dst_x;
    int src_x;
    int dx;
    int hori_y;
    int top_y;
    int hori_pixels;
    br_angle yaw;
    br_angle hori_sky;
    br_camera* camera;
    br_scalar tan_half_fov;
    br_scalar tan_half_hori_fov;
    br_scalar tan_half_hori_sky;
    br_scalar hshift;
    br_scalar tan_pitch;
    tU8 top_col;
    tU8 bot_col;
    int bot_height;
    int repetitions;
    br_pixelmap* col_map;

    dst_x = 0;
    col_map = gHorizon_material->colour_map;
    camera = (br_camera*)pCamera->type_data;
    tan_half_fov = Tan(camera->field_of_view / 2);
    tan_half_hori_fov = tan_half_fov * camera->aspect;

    tan_half_hori_sky = col_map->width * tan_half_hori_fov / pRender_buffer->width;
    hori_sky = 2 * BrRadianToAngle(atan2(tan_half_hori_sky, 1.0f));
    repetitions = (int)(1.0f / BrFixedToFloat(hori_sky) + 0.5f);
    hori_sky = BR_ANGLE_DEG(360) / repetitions;
    yaw = BrRadianToAngle(atan2(pCamera_to_world->m[2][0], pCamera_to_world->m[2][2]));
    hshift = -BrFixedToFloat(yaw) / BrFixedToFloat(hori_sky);

    src_x = (int)(col_map->width * hshift);
    while (src_x < 0) {
        src_x += col_map->width;
    }
    while (src_x > col_map->width) {
        src_x -= col_map->width;
    }

    tan_pitch = (br_scalar)(pCamera_to_world->m[2][1]
        / sqrt(pCamera_to_world->m[2][0] * pCamera_to_world->m[2][0] + pCamera_to_world->m[2][2] * pCamera_to_world->m[2][2]));
    hori_y = (int)(tan_pitch / tan_half_fov * pRender_buffer->height / 2.0f);
    hori_pixels = col_map->height - gSky_image_underground * col_map->height / gSky_image_height;
    top_y = -hori_y - hori_pixels;

    while (dst_x < pRender_buffer->width) {
        dx = MIN(col_map->width - src_x, pRender_buffer->width - dst_x);
        DRPixelmapRectangleCopy(pRender_buffer, dst_x - pRender_buffer->origin_x, top_y, col_map, src_x - col_map->origin_x, -col_map->origin_y, dx, col_map->height);
        src_x = 0;
        dst_x += dx;
    }

    top_col = ((tU8*)col_map->pixels)[0];
    if (pRender_buffer->origin_y + top_y > 0) {
        DRPixelmapRectangleFill(pRender_buffer, -pRender_buffer->origin_x, -pRender_buffer->origin_y, pRender_buffer->width, pRender_buffer->origin_y + top_y, top_col);
    }
    bot_col = ((tU8*)col_map->pixels)[col_map->row_bytes * (col_map->height - 1)];
    bot_height = pRender_buffer->height - pRender_buffer->origin_y - top_y - col_map->height;
    if (bot_height > 0) {
        DRPixelmapRectangleFill(pRender_buffer, -pRender_buffer->origin_x, top_y + col_map->height, pRender_buffer->width, bot_height, bot_col);
    }
}

// DoHorizon

// DoDepthCue

// DoFog

// DepthEffect

// FUNCTION: CARMA2_HW 0x00445ca0
// FUNCTION: CARMA2_CHEAT 0x004461c0
void C2_HOOK_FASTCALL RenderLollipops(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
}

// STUB: CARMA2_HW 0x00445cb0
// STUB: CARMA2_SW 0x00445d40
// STUB: CARMA2_CHEAT 0x004461d0
void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00446680
// FUNCTION: CARMA2_SW 0x00446700
// FUNCTION: CARMA2_CHEAT 0x00446ba0
void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera) {
    float f_time;
    // GLOBAL: CARMA2_HW 0x00591194
    // GLOBAL: CARMA2_SW 0x0058c18c
    // GLOBAL: CARMA2_CHEAT 0x00597aec
    static float mag00 = 0.02f;
    // GLOBAL: CARMA2_HW 0x00591198
    // GLOBAL: CARMA2_SW 0x0058c190
    // GLOBAL: CARMA2_CHEAT 0x00597af0
    static float mag01 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0059119c
    // GLOBAL: CARMA2_SW 0x0058c194
    // GLOBAL: CARMA2_CHEAT 0x00597af4
    static float mag02 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911a0
    // GLOBAL: CARMA2_SW 0x0058c198
    // GLOBAL: CARMA2_CHEAT 0x00597af8
    static float mag10 = 0.15f;
    // GLOBAL: CARMA2_HW 0x005911a4
    // GLOBAL: CARMA2_SW 0x0058c19c
    // GLOBAL: CARMA2_CHEAT 0x00597afc
    static float mag11 = 0.05f;
    // GLOBAL: CARMA2_HW 0x005911a8
    // GLOBAL: CARMA2_SW 0x0058c1a0
    // GLOBAL: CARMA2_CHEAT 0x00597b00
    static float mag12 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0067c4e4
    // GLOBAL: CARMA2_SW 0x00676c5c
    // GLOBAL: CARMA2_CHEAT 0x00683d8c
    static float mag20;
    // GLOBAL: CARMA2_HW 0x0067c4e8
    // GLOBAL: CARMA2_SW 0x00676c60
    // GLOBAL: CARMA2_CHEAT 0x00683d90
    static float mag21;
    // GLOBAL: CARMA2_HW 0x0067c4ec
    // GLOBAL: CARMA2_SW 0x00676c64
    // GLOBAL: CARMA2_CHEAT 0x00683d94
    static float mag22;
    // GLOBAL: CARMA2_HW 0x005911ac
    // GLOBAL: CARMA2_SW 0x0058c1a4
    // GLOBAL: CARMA2_CHEAT 0x00597b04
    static float period00 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b0
    // GLOBAL: CARMA2_SW 0x0058c1a8
    // GLOBAL: CARMA2_CHEAT 0x00597b08
    static float period01 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b4
    // GLOBAL: CARMA2_SW 0x0058c1ac
    // GLOBAL: CARMA2_CHEAT 0x00597b0c
    static float period02 = 4000.f;
    // GLOBAL: CARMA2_HW 0x005911b8
    // GLOBAL: CARMA2_SW 0x0058c1b0
    // GLOBAL: CARMA2_CHEAT 0x00597b10
    static float period10 = 2200.f;
    // GLOBAL: CARMA2_HW 0x005911bc
    // GLOBAL: CARMA2_SW 0x0058c1b4
    // GLOBAL: CARMA2_CHEAT 0x00597b14
    static float period11 = 3300.f;
    // GLOBAL: CARMA2_HW 0x005911c0
    // GLOBAL: CARMA2_SW 0x0058c1b8
    // GLOBAL: CARMA2_CHEAT 0x00597b18
    static float period12 = 3100.f;
    // GLOBAL: CARMA2_HW 0x005911c4
    // GLOBAL: CARMA2_SW 0x0058c1bc
    // GLOBAL: CARMA2_CHEAT 0x00597b1c
    static float period20 = 2800.f;
    // GLOBAL: CARMA2_HW 0x005911c8
    // GLOBAL: CARMA2_SW 0x0058c1c0
    // GLOBAL: CARMA2_CHEAT 0x00597b20
    static float period21 = 2500.f;
    // GLOBAL: CARMA2_HW 0x005911cc
    // GLOBAL: CARMA2_SW 0x0058c1c4
    // GLOBAL: CARMA2_CHEAT 0x00597b24
    static float period22 = 3900.f;

    f_time = (float)GetTotalTime();
    pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(360.0f * (f_time / period00), 360.0f)) * mag00;
    pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(360.0f * (f_time / period01), 360.0f)) * mag01;
    pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(360.0f * (f_time / period02), 360.0f)) * mag02;
    pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(360.0f * (f_time / period10), 360.0f)) * mag10;
    pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(360.0f * (f_time / period11), 360.0f)) * mag11;
    pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(360.0f * (f_time / period12), 360.0f)) * mag12;
    pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(360.0f * (f_time / period20), 360.0f)) * mag20;
    pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(360.0f * (f_time / period21), 360.0f)) * mag21;
    pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(360.0f * (f_time / period22), 360.0f)) * mag22;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL DoDrugWobbleCamera(br_actor* pCamera) {
    float f_time;
    // GLOBAL: CARMA2_HW 0x005911d0
    // GLOBAL: CARMA2_SW 0x0058c1c8
    // GLOBAL: CARMA2_CHEAT 0x00597b28
    static float mag00 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d4
    // GLOBAL: CARMA2_SW 0x0058c1cc
    // GLOBAL: CARMA2_CHEAT 0x00597b2c
    static float mag01 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d8
    // GLOBAL: CARMA2_SW 0x0058c1d0
    // GLOBAL: CARMA2_CHEAT 0x00597b30
    static float mag02 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911dc
    // GLOBAL: CARMA2_SW 0x0058c1d4
    // GLOBAL: CARMA2_CHEAT 0x00597b34
    static float mag10 = 0.2f;
    // GLOBAL: CARMA2_HW 0x005911e0
    // GLOBAL: CARMA2_SW 0x0058c1d8
    // GLOBAL: CARMA2_CHEAT 0x00597b38
    static float mag11 = 0.07f;
    // GLOBAL: CARMA2_HW 0x005911e4
    // GLOBAL: CARMA2_SW 0x0058c1dc
    // GLOBAL: CARMA2_CHEAT 0x00597b3c
    static float mag12 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911e8
    // GLOBAL: CARMA2_SW 0x0058c1e0
    // GLOBAL: CARMA2_CHEAT 0x00597b40
    static float mag20 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911ec
    // GLOBAL: CARMA2_SW 0x0058c1e4
    // GLOBAL: CARMA2_CHEAT 0x00597b44
    static float mag21 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911f0
    // GLOBAL: CARMA2_SW 0x0058c1e8
    // GLOBAL: CARMA2_CHEAT 0x00597b48
    static float mag22 = 0.01f;
    // GLOBAL: CARMA2_HW 0x005911f4
    // GLOBAL: CARMA2_SW 0x0058c1ec
    // GLOBAL: CARMA2_CHEAT 0x00597b4c
    static float period00 = 550.f;
    // GLOBAL: CARMA2_HW 0x005911f8
    // GLOBAL: CARMA2_SW 0x0058c1f0
    // GLOBAL: CARMA2_CHEAT 0x00597b50
    static float period01 = 700.f;
    // GLOBAL: CARMA2_HW 0x005911fc
    // GLOBAL: CARMA2_SW 0x0058c1f4
    // GLOBAL: CARMA2_CHEAT 0x00597b54
    static float period02 = 200.f;
    // GLOBAL: CARMA2_HW 0x00591200
    // GLOBAL: CARMA2_SW 0x0058c1f8
    // GLOBAL: CARMA2_CHEAT 0x00597b58
    static float period10 = 100.f;
    // GLOBAL: CARMA2_HW 0x00591204
    // GLOBAL: CARMA2_SW 0x0058c1fc
    // GLOBAL: CARMA2_CHEAT 0x00597b5c
    static float period11 = 1300.f;
    // GLOBAL: CARMA2_HW 0x00591208
    // GLOBAL: CARMA2_SW 0x0058c200
    // GLOBAL: CARMA2_CHEAT 0x00597b60
    static float period12 = 500.f;
    // GLOBAL: CARMA2_HW 0x0059120c
    // GLOBAL: CARMA2_SW 0x0058c204
    // GLOBAL: CARMA2_CHEAT 0x00597b64
    static float period20 = 800.f;
    // GLOBAL: CARMA2_HW 0x00591210
    // GLOBAL: CARMA2_SW 0x0058c208
    // GLOBAL: CARMA2_CHEAT 0x00597b68
    static float period21 = 1500.f;
    // GLOBAL: CARMA2_HW 0x00591214
    // GLOBAL: CARMA2_SW 0x0058c20c
    // GLOBAL: CARMA2_CHEAT 0x00597b6c
    static float period22 = 300.f;

    f_time = (float)(tS32)PDGetTotalTime();
    pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
    pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
    pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
    pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
    pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
    pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
    pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
    pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
    pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
}

inline int C2_HOOK_FASTCALL DoSubaquaCam(void) {
    int submerged;

    submerged = 0;
    if (InWater(gHud_tinted1) != NULL) {
        TurnTintedPolyOn(gHud_tinted1);
        submerged = 1;
    } else {
        TurnTintedPolyOff(gHud_tinted1);
    }
    return submerged;
}

// FUNCTION: CARMA2_HW 0x00446340
// FUNCTION: CARMA2_SW 0x004463c0
// FUNCTION: CARMA2_CHEAT 0x00446860
void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world) {

    UpdateTintedPolys();
    if (gOn_drugs && !gAction_replay_mode) {
        TurnTintedPolyOn(gHud_tinted2);
        DoDrugWobbleCamera(pCamera);
    } else {
        TurnTintedPolyOff(gHud_tinted2);
        gLast_camera_special_volume = FindSpecialVolume((br_vector3*)pCamera_to_world->m[3], gLast_camera_special_volume, 0);
        if (gLast_camera_special_volume != NULL) {
            switch (gLast_camera_special_volume->camera_special_effect_index) {
            case 0:
                DoWobbleCamera(pCamera);
            default:
                break;
            }
        }
        if (gHas_sky_texture && gSky_on) {
            if ((gProgram_state.current_depth_effect.sky_texture != NULL) == (gLast_camera_special_volume != NULL && gLast_camera_special_volume->sky_col == -2)) {
                ToggleSkyQuietly();
            }
        }
    }
    SetTintedPolyRefMaterial(gHud_tinted1, (br_vector3*)pCamera_to_world->m[3]);
    if (DoSubaquaCam()) {
        DoWobbleCamera(pCamera);
    }
}

// FUNCTION: CARMA2_HW 0x00446890
// FUNCTION: CARMA2_SW 0x00446910
// FUNCTION: CARMA2_CHEAT 0x00446db0
void C2_HOOK_FASTCALL ReduceDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start > 3) {
        gProgram_state.current_depth_effect.start--;
    }
    sprintf(s, "Depth start reduced to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x004468f0
// FUNCTION: CARMA2_SW 0x00446970
// FUNCTION: CARMA2_CHEAT 0x00446e10
void C2_HOOK_FASTCALL IncreaseDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start < 13) {
        gProgram_state.current_depth_effect.start++;
    }
    sprintf(s, "Depth start increased to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x00446950
// FUNCTION: CARMA2_SW 0x004469d0
// FUNCTION: CARMA2_CHEAT 0x00446e70
void C2_HOOK_FASTCALL ReduceDepthEnd(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.end < 13) {
        gProgram_state.current_depth_effect.end++;
    }
    sprintf(s, "Depth end reduced to %d", gProgram_state.current_depth_effect.end);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.end = gProgram_state.current_depth_effect.end;
}

// FUNCTION: CARMA2_HW 0x004469b0
// FUNCTION: CARMA2_SW 0x00446a30
// FUNCTION: CARMA2_CHEAT 0x00446ed0
void C2_HOOK_FASTCALL IncreaseDepthEnd(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.end > 0) {
        gProgram_state.current_depth_effect.end--;
    }
    sprintf(s, "Depth end increased to %d", gProgram_state.current_depth_effect.end);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.end = gProgram_state.current_depth_effect.end;
}

// FUNCTION: CARMA2_HW 0x00446a10
// FUNCTION: CARMA2_SW 0x00446a90
// FUNCTION: CARMA2_CHEAT 0x00446f30
void C2_HOOK_FASTCALL AssertYons(void) {
    br_camera* camera_ptr;
    int i;
    br_scalar yon_multiplier;

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); ++i) {
        camera_ptr = gCamera_list[i]->type_data;
        yon_multiplier = gYon_multiplier;
        camera_ptr->yon_z = yon_multiplier * gCamera_yon;
    }
}

// FUNCTION: CARMA2_HW 0x00446a40
// FUNCTION: CARMA2_SW 0x00446ac0
// FUNCTION: CARMA2_CHEAT 0x00446f60
void C2_HOOK_FASTCALL IncreaseYon(void) {
    char s[256];

    gCamera_yon += 5.f;
    AssertYons();
    sprintf(s, GetMiscString(eMiscString_yon_increased_to_D), (int)((br_camera*)gCamera_list[1]->type_data)->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}

// FUNCTION: CARMA2_HW 0x00446ad0
// FUNCTION: CARMA2_SW 0x00446b50
// FUNCTION: CARMA2_CHEAT 0x00446ff0
void C2_HOOK_FASTCALL DecreaseYon(void) {
    char s[256];
    br_camera *camera;

    gCamera_yon -= 5.0f;
    if (gCamera_yon < 5.0f) {
        gCamera_yon = 5.0f;
    }
    AssertYons();
    camera = gCamera_list[1]->type_data;
    sprintf(s, GetMiscString(eMiscString_yon_decreased_to_D), (int)camera->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}

// FUNCTION: CARMA2_HW 0x00446b70
// FUNCTION: CARMA2_SW 0x00446bf0
// FUNCTION: CARMA2_CHEAT 0x00447090
void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    int i;
    br_camera* camera_ptr;

    if (pYon < 5.0f) {
        pYon = 5.0f;
    }

    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        if (gCamera_list[i] != NULL) {
            camera_ptr = gCamera_list[i]->type_data;
            camera_ptr->yon_z = pYon;
        }
    }
    gCamera_yon = pYon;
}

// FUNCTION: CARMA2_HW 0x00446bb0
// FUNCTION: CARMA2_SW 0x00446c30
// FUNCTION: CARMA2_CHEAT 0x004470d0
br_scalar C2_HOOK_STDCALL GetYon(void) {

    return gCamera_yon;
}

// FUNCTION: CARMA2_HW 0x00446bc0
// FUNCTION: CARMA2_SW 0x00446c40
// FUNCTION: CARMA2_CHEAT 0x004470e0
void C2_HOOK_STDCALL IncreaseAngle(void) {
    br_camera* camera_ptr;
    int i;
    char s[256];

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->field_of_view += BrDegreeToAngle(2.5);
        if (camera_ptr->field_of_view > BrDegreeToAngle(170)) {
            camera_ptr->field_of_view = BrDegreeToAngle(170);
        }
        sprintf(s, "Camera angle increased to %d", gProgram_state.current_depth_effect.end);
        NewTextHeadupSlot(4, 0, 500, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00446c40
// FUNCTION: CARMA2_SW 0x00446cc0
// FUNCTION: CARMA2_CHEAT 0x00447160
void C2_HOOK_STDCALL DecreaseAngle(void) {
    br_camera* camera_ptr;
    int i;
    char s[256];

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->field_of_view -= BrDegreeToAngle(2.5);
        if (camera_ptr->field_of_view < BrDegreeToAngle(10)) {
            camera_ptr->field_of_view = BrDegreeToAngle(10);
        }
        sprintf(s, "Camera angle decreased to %d", gProgram_state.current_depth_effect.end);
        NewTextHeadupSlot(4, 0, 500, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00446e00
// FUNCTION: CARMA2_SW 0x00447040
// FUNCTION: CARMA2_CHEAT 0x00447320
int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return gSky_on;
}

// FUNCTION: CARMA2_HW 0x00446e10
// FUNCTION: CARMA2_SW 0x00447050
// FUNCTION: CARMA2_CHEAT 0x00447330
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {

    if (gSky_on != skyTextureOn) {
        ToggleSkyQuietly();
    }
    gSky_on = skyTextureOn;
}

// FUNCTION: CARMA2_HW 0x00446e80
// FUNCTION: CARMA2_CHEAT 0x004473a0
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {
    br_pixelmap* temp;

    temp = gProgram_state.current_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.sky_texture = gSwap_sky_texture;
    gSwap_sky_texture = temp;
    gProgram_state.default_depth_effect.sky_texture = gProgram_state.current_depth_effect.sky_texture;
    if (gHorizon_material != NULL && gProgram_state.default_depth_effect.sky_texture != NULL) {
        gHorizon_material->colour_map = gProgram_state.current_depth_effect.sky_texture;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        MungeSkyVs(gForward_sky_model, gHorizon_material);
    }
}

// FUNCTION: CARMA2_HW 0x00446ee0
// FUNCTION: CARMA2_SW 0x00447120
// FUNCTION: CARMA2_CHEAT 0x00447400
void C2_HOOK_FASTCALL ToggleSky(void) {

    SetSkyTextureOn(!GetSkyTextureOn());
    if (gProgram_state.current_depth_effect.sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_on));
    } else if (gSwap_sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_no_sky_texture_for_this_race));
    }
}

// FUNCTION: CARMA2_HW 0x00446f90
// FUNCTION: CARMA2_SW 0x004471d0
// FUNCTION: CARMA2_CHEAT 0x004474b0
int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return gDepth_cueing_on;
}

// FUNCTION: CARMA2_HW 0x00446fa0
// STUB: CARMA2_SW 0x004471e0
// FUNCTION: CARMA2_CHEAT 0x004474c0
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {
#ifdef REC2_HW
    if (gDepth_cueing_on != pOn && gHorizon_material != NULL) {
        ToggleDepthCueingQuietly();
    }
    gDepth_cueing_on = pOn;
#endif
#ifdef REC2_SW
    if (gDepth_cueing_on != pOn && gHorizon_material != NULL) {
        ToggleDepthCueingQuietly();
    }
    gDepth_cueing_on = pOn;
#endif
}

// FUNCTION: CARMA2_HW 0x00447070
// STUB: CARMA2_SW 0x00447350
// FUNCTION: CARMA2_CHEAT 0x00447590
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {
#ifdef REC2_HW
    int temp_red;
    int temp_green;
    int temp_blue;
    int temp_end;
    int temp_start;
    tDepth_effect_type temp_type;

    temp_type = gProgram_state.current_depth_effect.type;
    temp_start = gProgram_state.current_depth_effect.start;
    temp_end = gProgram_state.current_depth_effect.end;
    temp_red = gProgram_state.current_depth_effect.colour.red;
    temp_green = gProgram_state.current_depth_effect.colour.green;
    temp_blue = gProgram_state.current_depth_effect.colour.blue;
    InstantDepthChange(
        gSwap_depth_effect_type,
        gProgram_state.current_depth_effect.sky_texture,
        gSwap_depth_effect_start,
        gSwap_depth_effect_end,
        gSwap_depth_effect_colour_red,
        gSwap_depth_effect_colour_green,
        gSwap_depth_effect_colour_blue,
        gProgram_state.racing);
    gSwap_depth_effect_type = temp_type;
    gSwap_depth_effect_start = temp_start;
    gSwap_depth_effect_end = temp_end;
    gSwap_depth_effect_colour_red = temp_red;
    gSwap_depth_effect_colour_green = temp_green;
    gSwap_depth_effect_colour_blue = temp_blue;
#endif
#ifdef REC2_SW
    NOT_IMPLEMENTED();
#endif
}

// FUNCTION: CARMA2_HW 0x00447110
// FUNCTION: CARMA2_SW 0x00447490
// FUNCTION: CARMA2_CHEAT 0x00447630
// STUB: CARMA2_SW 0x00447490
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {
    int enabled;

    enabled = GetDepthCueingOn();
    SetDepthCueingOn(!enabled);
    if (gProgram_state.current_depth_effect.type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_on));
    } else if (gSwap_depth_effect_type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_there_is_no_depth_cueing_for_this_race));
    }
}

// FUNCTION: CARMA2_HW 0x00447220
// FUNCTION: CARMA2_SW 0x00447640
// FUNCTION: CARMA2_CHEAT 0x00447740
void C2_HOOK_FASTCALL ChangeDepthEffect(void) {
    gProgram_state.current_depth_effect.type = gProgram_state.default_depth_effect.type;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.start = gProgram_state.default_depth_effect.start;
    gProgram_state.current_depth_effect.end = gProgram_state.default_depth_effect.end;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00445e20
// STUB: CARMA2_SW 0x00445ea0
void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00447250
// FUNCTION: CARMA2_SW 0x00447670
// FUNCTION: CARMA2_CHEAT 0x00447770
void C2_HOOK_FASTCALL MungeForwardSky(void) {
    MungeSkyModel(gCamera, gForward_sky_model);
    gHas_sky_texture = gProgram_state.default_depth_effect.sky_texture != NULL;
}

// FUNCTION: CARMA2_HW 0x00447280
// FUNCTION: CARMA2_SW 0x004476a0
// FUNCTION: CARMA2_CHEAT 0x004477a0
void C2_HOOK_FASTCALL MungeRearviewSky(void) {
}

// FUNCTION: CARMA2_HW 0x00447290
// FUNCTION: CARMA2_SW 0x004476b0
// FUNCTION: CARMA2_CHEAT 0x004477b0
void C2_HOOK_FASTCALL SkyTextureChanged(void) {

    BrMaterialUpdate(gHorizon_material, BR_MATU_COLOURMAP);
}

// FUNCTION: CARMA2_HW 0x004472b0
// FUNCTION: CARMA2_SW 0x004476d0
// FUNCTION: CARMA2_CHEAT 0x004477d0
void C2_HOOK_FASTCALL SetSkyColour(br_colour c) {

    gHorizon_material->colour = c;
}

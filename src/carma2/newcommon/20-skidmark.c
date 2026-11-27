/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "20-skidmark.h"

#include "23-piping.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x0065fe08
// GLOBAL: CARMA2_SW 0x0065a748
// GLOBAL: CARMA2_CHEAT 0x00667650
const char* gBoring_material_names[2] = { "OILSMEAR.MAT", "ROBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x0065fe10
// GLOBAL: CARMA2_SW 0x0065a750
// GLOBAL: CARMA2_CHEAT 0x00667658
const char* gMaterial_names[2] = { "OILSMEAR.MAT", "GIBSMEAR.MAT" };

// GLOBAL: CARMA2_HW 0x0065fec0
// GLOBAL: CARMA2_SW 0x0065a800
// GLOBAL: CARMA2_CHEAT 0x00667708
const char* gShrapnel_type_names[4] = { "abstract", "shards", "ghostparts", "noncars" };

// GLOBAL: CARMA2_HW 0x0065fed0
// GLOBAL: CARMA2_SW 0x0065a810
// GLOBAL: CARMA2_CHEAT 0x00667718
const char* gInitial_position_type_names[3] = { "sphereclumped", "boxclumped", "actorbased" };

// GLOBAL: CARMA2_HW 0x0065fee0
// GLOBAL: CARMA2_SW 0x0065a820
// GLOBAL: CARMA2_CHEAT 0x00667728
const char* gInitial_position_sphere_where[2] = { "impact", "model" };

// GLOBAL: CARMA2_HW 0x0065fee8
// GLOBAL: CARMA2_SW 0x0065a828
const char* gExplosion_rotate_names[2] = {
    "norotate",
    "randomrotate",
};

// GLOBAL: CARMA2_HW 0x006a27f0
// GLOBAL: CARMA2_SW 0x0069c848
// GLOBAL: CARMA2_CHEAT 0x006aa368
tSkid gSkids[100] = { 0 };

// GLOBAL: CARMA2_HW 0x006a32e0
// GLOBAL: CARMA2_SW 0x0069d338
int gINT_006a32e0 = 0;

// GLOBAL: CARMA2_HW 0x006a27e8
// GLOBAL: CARMA2_SW 0x0069c840
// GLOBAL: CARMA2_CHEAT 0x006aa360
tU16 gCurrent_skid = 0;

// FUNCTION: CARMA2_HW 0x004ea2c0
// FUNCTION: CARMA2_SW 0x004e6060
// FUNCTION: CARMA2_CHEAT 0x004eff60
void C2_HOOK_FASTCALL StretchMark(tSkid* pMark, br_vector3* pFrom, br_vector3* pTo, br_scalar pTexture_start, br_scalar pTexture_step) {
    br_vector3 temp;
    br_vector3* rows;
    br_scalar len;
    br_scalar delta;
    br_model* model;

    rows = (br_vector3*)&pMark->actor->t.t.mat.m;
    BrVector3Sub(&temp, pTo, pFrom);
    len = BrVector3Length(&temp);
    if (len < 0.001f) {
        BrMatrix34Scale(&pMark->actor->t.t.mat, 0.001f, 1.0f, 0.001f);
    } else {
        delta = pTexture_step / len;
        BrVector3Copy(&rows[1], &pMark->normal);
        BrVector3Cross(&rows[2], &temp, &pMark->normal);
        BrVector3Scale(&rows[2], &rows[2], delta);
        BrVector3Scale(&rows[0], &temp, len / len);
        BrVector3Add(&pMark->pos, pFrom, pTo);
        BrVector3Scale(&pMark->pos, &pMark->pos, 0.5f);
        rows[3] = pMark->pos;

        model = pMark->actor->model;
        model->vertices[1].map.v[0] = pTexture_start / pTexture_step;
        model->vertices[0].map.v[0] = model->vertices[1].map.v[0];
        model->vertices[3].map.v[0] = (pTexture_start + len) / pTexture_step;
        model->vertices[2].map.v[0] = model->vertices[3].map.v[0];
        BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
    }
}

// MaterialFromIndex

// FUNCTION: CARMA2_HW 0x004e9be0
// FUNCTION: CARMA2_SW 0x004e59a0
// FUNCTION: CARMA2_CHEAT 0x004ef880
void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {
    gSkids[pSkid_num].actor->t.t.mat = *pMatrix;
    gSkids[pSkid_num].pos = *(br_vector3*)pMatrix->m[3];
    gSkids[pSkid_num].actor->material = pMaterial;
    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_DEFAULT;
}

inline int C2_HOOK_FASTCALL FarFromLine2D(br_vector3* pPt, br_vector3* pL1, br_vector3* pL2) {
    br_vector2 line;
    br_vector2 to_pt;
    br_scalar line_len;
    br_scalar cross;

    line.v[0] = pL2->v[0] - pL1->v[0];
    line.v[1] = pL2->v[2] - pL1->v[2];
    to_pt.v[0] = pPt->v[0] - pL2->v[0];
    to_pt.v[1] = pPt->v[2] - pL2->v[2];
    cross = (-line.v[0]) * to_pt.v[1] + to_pt.v[0] * line.v[1];
    line_len = BrVector2Length(&line);
    if (fabs(cross) > line_len * 0.05f) {
        return 1;
    } else {
        return 0;
    }
}

inline int C2_HOOK_FASTCALL Reflex2D(br_vector3* pPt, br_vector3* pL1, br_vector3* pL2) {
    br_vector2 line;
    br_vector2 to_pt;

    line.v[0] = pL2->v[0] - pL1->v[0];
    line.v[1] = pL2->v[2] - pL1->v[2];
    to_pt.v[0] = pPt->v[0] - pL2->v[0];
    to_pt.v[1] = pPt->v[2] - pL2->v[2];
    if (BrVector2Dot(&to_pt, &line) < 0.0f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e9c40
// FUNCTION: CARMA2_SW 0x004e5a00
// FUNCTION: CARMA2_CHEAT 0x004ef8e0
void C2_HOOK_FASTCALL InitSkids(void) {
    int skid;
    int mat;
    int sl;
    br_model* square;
    br_pixelmap* pm;
    char* str;

    for (mat = 0; mat < (int)REC2_ASIZE(gMaterial_names); mat++) {
        if (gProgram_state.sausage_eater_mode) {
            str = (char*)gBoring_material_names[mat];
        } else {
            str = (char*)gMaterial_names[mat];
        }
        gMaterial[mat] = BrMaterialFind(str);
        if (gMaterial[mat] == NULL) {
            if (gProgram_state.sausage_eater_mode) {
                str = (char*)gBoring_material_names[mat];
            } else {
                str = (char*)gMaterial_names[mat];
            }

            str = strtok(str, ".");
            sl = strlen(str);
            strcat(str, ".PIX");
            pm = LoadPixelmap(str);
            if (pm == NULL) {
                FatalError(kFatalError_CantLoadPixelmapFile_S, str);
            }
            BrMapAdd(pm);
            str[sl] = '\0';
            strcat(str, ".MAT");
            gMaterial[mat] = LoadMaterial(str);
            if (gMaterial[mat] == NULL) {
                BrFatal("C:\\Carma2\\Source\\Common\\Skidmark.c", 215, "Couldn't find %s", gMaterial_names[mat]);
            } else {
#ifdef REC2_HW
                GlorifyMaterial(&gMaterial[mat], 1, kRendererShadingType_AmbientOnly);
#endif
#ifdef REC2_SW
                BrMaterialAdd(gMaterial[mat]);
#endif
            }
#ifdef REC2_HW
            BrMaterialAdd(gMaterial[mat]);
#endif
        }
    }

    C2_HOOK_BUG_ON(sizeof(tSkid) != 28);

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        gSkids[skid].actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gOther_selfs[3], gSkids[skid].actor);
        gSkids[skid].actor->render_style = BR_RSTYLE_NONE;
        square = BrModelAllocate(NULL, 4, 2);
        BrVector3Set(&square->vertices[0].p, -0.5f, 0.0f, -0.5f);
        BrVector3Set(&square->vertices[1].p, -0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[2].p, 0.5f, 0.0f, 0.5f);
        BrVector3Set(&square->vertices[3].p, 0.5f, 0.0f, -0.5f);
        BrVector2Set(&square->vertices[0].map, 0.0f, 0.0f);
        BrVector2Set(&square->vertices[1].map, 0.0f, 1.0f);
        BrVector2Set(&square->vertices[2].map, 1.0f, 1.0f);
        BrVector2Set(&square->vertices[3].map, 1.0f, 0.0f);
        square->faces[0].vertices[0] = 0;
        square->faces[0].vertices[1] = 1;
        square->faces[0].vertices[2] = 2;
        square->faces[0].smoothing = 1;
        square->faces[1].vertices[0] = 0;
        square->faces[1].vertices[1] = 2;
        square->faces[1].vertices[2] = 3;
        square->faces[1].smoothing = 1;
        square->flags |= BR_MODF_KEEP_ORIGINAL;
        BrModelAdd(square);
        gSkids[skid].actor->model = square;
    }
}

// FUNCTION: CARMA2_HW 0x004e9ee0
// FUNCTION: CARMA2_SW 0x004e5c80
// FUNCTION: CARMA2_CHEAT 0x004efb80
void C2_HOOK_FASTCALL HideSkid(int pSkid_num) {

    gSkids[pSkid_num].actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x004e9f00
// FUNCTION: CARMA2_SW 0x004e5ca0
// FUNCTION: CARMA2_CHEAT 0x004efba0
void C2_HOOK_FASTCALL HideSkids(void) {
    int skid;

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        HideSkid(skid);
    }
}

inline br_scalar C2_HOOK_FASTCALL SkidLen(int pSkid) {
    return BrVector3Length((br_vector3*)gSkids[pSkid].actor->t.t.mat.m[0]);
}

inline int C2_HOOK_FASTCALL UglyWidthChange(int pSkid, br_scalar pWidth) {
    br_scalar old_width_sq;

    old_width_sq = BrVector3LengthSquared((br_vector3*)gSkids[pSkid].actor->t.t.mat.m[2]);
    if (old_width_sq < 2.0f * (pWidth * pWidth) && pWidth * pWidth <= 2.0f * old_width_sq) {
        return 0;
    } else {
        return 1;
    }
}

// GenericSkidSection

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e9f20
// FUNCTION: CARMA2_SW 0x004e5cc0
// FUNCTION: CARMA2_CHEAT 0x004efbc0
int C2_HOOK_FASTCALL SkidSection(tU16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step) {

    if (BrVector3Dot(pNorm, pPrev_norm) < 0.997f || fabs(BrVector3Dot(pNorm, pPos) - BrVector3Dot(pPrev_pos, pNorm)) > 0.01f) {
        *pSkid_id = -1;
        return 1;
    }
    if (*pSkid_id >= REC2_ASIZE(gSkids)
        || pMaterial != gSkids[*pSkid_id].actor->material
        || UglyWidthChange(*pSkid_id, pTexture_step)
        || SkidLen(*pSkid_id) > 0.5f
        || FarFromLine2D(pPos, pSkid_start, pSkid_end)
        || Reflex2D(pPos, pSkid_start, pPrev_pos)) {
        *pSkid_start = *pPrev_pos;
        *pSkid_end = *pPos;
        gSkids[gCurrent_skid].actor->render_style = BR_RSTYLE_DEFAULT;
        gSkids[gCurrent_skid].actor->material = pMaterial;
        gSkids[gCurrent_skid].normal = *pNorm;
        StretchMark(&gSkids[gCurrent_skid], pPrev_pos, pPos, pTexture_start, pTexture_step);
        PipeSingleSkidAdjustment(gCurrent_skid, &gSkids[gCurrent_skid].actor->t.t.mat, pMaterial);
        *pSkid_id = gCurrent_skid;
        gCurrent_skid = (gCurrent_skid + 1) % 100;
    } else {
        StretchMark(&gSkids[*pSkid_id], pSkid_start, pPos, pTexture_start, pTexture_step);
        PipeSingleSkidAdjustment(*pSkid_id, &gSkids[*pSkid_id].actor->t.t.mat, pMaterial);
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ea490
// FUNCTION: CARMA2_SW 0x004e6230
// FUNCTION: CARMA2_CHEAT 0x004f0130
void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num) {
    br_material* material;
    int skid_mask;
    br_vector3 disp;
    br_scalar dist;
    int on_ground;

    on_ground = pCar_spec->oldd[pWheel_num] < pCar_spec->susp_height[pWheel_num >> 1];
    material = gCurrent_race.material_modifiers[pCar_spec->material_index[pWheel_num]].skid_mark_material;
    if (pCar_spec->oil_remaining[pWheel_num] != 0.0f && on_ground) {
        pCar_spec->new_skidding |= 1 << pWheel_num;
        if (pCar_spec->field_0x16fc[pWheel_num] != NULL) {
            material = pCar_spec->field_0x16fc[pWheel_num];
        } else {
            material = gMaterial[0];
        }
    }
    if (material == NULL) {
        pCar_spec->old_skidding &= ~(1 << pWheel_num);
        return;
    }
    skid_mask = 1 << pWheel_num;
    if (!(pCar_spec->new_skidding & skid_mask) && !(pCar_spec->old_skidding & skid_mask)) {
        return;
    }
    if (pCar_spec->new_skidding & skid_mask) {
        if (skid_mask & pCar_spec->old_skidding) {
            BrVector3Sub(&disp, &pCar_spec->special_start[pWheel_num], &pCar_spec->prev_skid_pos[pWheel_num]);
            dist = BrVector3Length(&disp);
            if (dist < 0.05f) {
                return;
            }
            if (SkidSection(&pCar_spec->old_skid[pWheel_num],
                    &pCar_spec->field_0x160c[pWheel_num],
                    &pCar_spec->field_0x163c[pWheel_num],
                    material,
                    &pCar_spec->special_start[pWheel_num],
                    &pCar_spec->nor[pWheel_num],
                    &pCar_spec->prev_skid_pos[pWheel_num],
                    &pCar_spec->prev_nor[pWheel_num],
                    pCar_spec->total_length[pWheel_num],
                    0.05f)) {
                pCar_spec->old_skidding &= ~skid_mask;
            }
            pCar_spec->total_length[pWheel_num] += dist;
            pCar_spec->oil_remaining[pWheel_num] -= dist;
            if (pCar_spec->oil_remaining[pWheel_num] < 0.0f) {
                pCar_spec->oil_remaining[pWheel_num] = 0.0f;
            }
        } else {
            pCar_spec->old_skidding |= skid_mask;
            pCar_spec->total_length[pWheel_num] = 0.0f;
            pCar_spec->old_skid[pWheel_num] = -1;
        }
    } else {
        pCar_spec->old_skidding &= ~skid_mask;
    }
    pCar_spec->prev_skid_pos[pWheel_num] = pCar_spec->special_start[pWheel_num];
    pCar_spec->prev_nor[pWheel_num] = pCar_spec->nor[pWheel_num];
}

// FUNCTION: CARMA2_HW 0x004ea700
// FUNCTION: CARMA2_SW 0x004e64a0
// FUNCTION: CARMA2_CHEAT 0x004f03a0
void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar) {
    int wheel;

    pCar->old_skidding = 0;
    for (wheel = 0; wheel < (int)REC2_ASIZE(pCar->oil_remaining); wheel++) {
        pCar->oil_remaining[wheel] = 0.0f;
    }
}

// FUNCTION: CARMA2_HW 0x004ea720
// FUNCTION: CARMA2_SW 0x004e64c0
// FUNCTION: CARMA2_CHEAT 0x004f03c0
void C2_HOOK_FASTCALL SkidsPerFrame(void) {
    int skid;

    for (skid = 0; skid < (int)REC2_ASIZE(gSkids); skid++) {
        if (gSkids[skid].actor->render_style != BR_RSTYLE_NONE) {
            gSkids[skid].actor->t.t.translate.t = gSkids[skid].pos;
        }
    }
}


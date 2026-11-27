/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "29-raycast.h"

#include "16-graphics1.h"
#include "32-spark.h"
#include "globvars.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006a20d0
// GLOBAL: CARMA2_SW 0x0069c128
// GLOBAL: CARMA2_CHEAT 0x006a9c48
br_actor* gY_picking_camera;

// GLOBAL: CARMA2_HW 0x006a20d4
// GLOBAL: CARMA2_SW 0x0069c12c
// GLOBAL: CARMA2_CHEAT 0x006a9c4c
br_model* gBelow_model;

// GLOBAL: CARMA2_HW 0x006a20d8
// GLOBAL: CARMA2_SW 0x0069c130
// GLOBAL: CARMA2_CHEAT 0x006a9c50
int gBelow_face_index;

// GLOBAL: CARMA2_HW 0x006a20dc
// GLOBAL: CARMA2_SW 0x0069c134
// GLOBAL: CARMA2_CHEAT 0x006a9c54
float gLowest_y_above;

// GLOBAL: CARMA2_HW 0x006a20e0
// GLOBAL: CARMA2_SW 0x0069c138
// GLOBAL: CARMA2_CHEAT 0x006a9c58
float gCurrent_y;

// GLOBAL: CARMA2_HW 0x006a20e4
// GLOBAL: CARMA2_SW 0x0069c13c
// GLOBAL: CARMA2_CHEAT 0x006a9c5c
br_model* gAbove_model;

// GLOBAL: CARMA2_HW 0x006a20e8
// GLOBAL: CARMA2_SW 0x0069c140
// GLOBAL: CARMA2_CHEAT 0x006a9c60
int gAbove_face_index;

// GLOBAL: CARMA2_HW 0x006a20f0
// GLOBAL: CARMA2_SW 0x0069c148
// GLOBAL: CARMA2_CHEAT 0x006a9c68
br_matrix34 gPick_model_to_view__raycast;

// GLOBAL: CARMA2_HW 0x006a2120
// GLOBAL: CARMA2_SW 0x0069c178
// GLOBAL: CARMA2_CHEAT 0x006a9c98
float gHighest_y_below;

extern int C2_HOOK_FASTCALL DRActorToRoot(br_actor* pActor, br_actor* pWorld, br_matrix34* pMat);

inline int C2_HOOK_FASTCALL DRActorToRoot(br_actor* pActor, br_actor* pWorld, br_matrix34* pMat) {

    if (pActor == pWorld) {
        BrMatrix34Identity(pMat);
        return 1;
    } else {
        BrTransformToMatrix34(pMat, &pActor->t);
        for (pActor = pActor->parent; pActor != NULL && pActor != pWorld; pActor = pActor->parent) {
            if (pActor->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(pMat, &pActor->t);
            }
        }
        return pActor == pWorld;
    }
}

// FUNCTION: CARMA2_HW 0x004e3660
// FUNCTION: CARMA2_SW 0x004df9f0
// FUNCTION: CARMA2_CHEAT 0x004e91e0
void C2_HOOK_FASTCALL InitRayCasting(void) {
    br_camera* camera_ptr;

    gY_picking_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera_ptr = gY_picking_camera->type_data;
    camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera_ptr->field_of_view = BrDegreeToAngle(70.0f);
    camera_ptr->hither_z = 0.001f;
    camera_ptr->yon_z = 1000.0f;
    camera_ptr->aspect = 1.0f;
    gY_picking_camera->t.t.mat.m[0][0] =  1.0f;
    gY_picking_camera->t.t.mat.m[0][1] =  0.0f;
    gY_picking_camera->t.t.mat.m[0][2] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][0] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][1] =  0.0f;
    gY_picking_camera->t.t.mat.m[1][2] = -1.0f;
    gY_picking_camera->t.t.mat.m[2][0] =  0.0f;
    gY_picking_camera->t.t.mat.m[2][1] =  1.0f;
    gY_picking_camera->t.t.mat.m[2][2] =  0.0f;
}

// BadDiv

// DRVector2AccumulateScale

// FUNCTION: CARMA2_HW 0x004e3a30
// FUNCTION: CARMA2_SW 0x004dfdc0
// FUNCTION: CARMA2_CHEAT 0x004e95b0
int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    br_scalar inverse;
    br_scalar t;

    for (i = 0; i < (int)REC2_ASIZE(rd->v); i++) {
        if (rd->v[i] > 2 * BR_SCALAR_EPSILON) {
            inverse = rd->v[i];
            inverse = 1.0f / inverse;
            t = (rp->v[i] - b->max.v[i]) * inverse;
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
            t = (rp->v[i] - b->min.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] < -2 * BR_SCALAR_EPSILON) {
            inverse = rd->v[i];
            inverse = 1.0f / inverse;
            t = (rp->v[i] - b->max.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
            t = (rp->v[i] - b->min.v[i]) * inverse;
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
        } else if (rp->v[i] > b->max.v[i] || rp->v[i] < b->min.v[i]) {
            return 0;
        }
    }
    if (t_near <= t_far) {
        *new_t_near = t_near;
        *new_t_far = t_far;
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e3840
// FUNCTION: CARMA2_SW 0x004dfbd0
// FUNCTION: CARMA2_CHEAT 0x004e93c0
int C2_HOOK_FASTCALL ActorPick2D(br_actor* pActor, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback, void* pArg) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_matrix34 m_to_v;
    br_matrix34 v_to_m;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_vector3 dir;

    r = 0;
    if (pActor->model != NULL) {
        this_model = pActor->model;
    } else {
        this_model = pModel;
    }
    if (pActor->material != NULL) {
        this_material = pActor->material;
    } else {
        this_material = pMaterial;
    }
    if (pActor->render_style == BR_RSTYLE_NONE) {
        return 0;
    }
    m_to_v = gPick_model_to_view__raycast;
    BrMatrix34PreTransform(&gPick_model_to_view__raycast, &pActor->t);
    switch (pActor->type) {
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
        if (PickBoundsTestRay__raycast(pActor->type_data, (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2], 0.0f, BR_SCALAR_MAX, &t_near, &t_far)) {
            for (a = pActor->children; a != NULL; a = a->next) {
                r = ActorPick2D(a, this_model, this_material, pCallback, pArg);
                if (r != 0) {
                    break;
                }
            }
        }
        break;
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            BrMatrix34Inverse(&v_to_m, &gPick_model_to_view__raycast);
            if (PickBoundsTestRay__raycast(&this_model->bounds, (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2], 0.0f, BR_SCALAR_MAX, &t_near, &t_far)) {
                BrVector3Negate(&dir, (br_vector3*)v_to_m.m[2]);
                r = pCallback(pActor, this_model, this_material, (br_vector3*)v_to_m.m[3], &dir, t_near, t_far, pArg);
                if (r) {
                    break;
                }
            }
            if (r) {
                break;
            }
        }
    default:
        for (a = pActor->children; a != NULL; a = a->next) {
            r = ActorPick2D(a, this_model, this_material, pCallback, pArg);
            if (r != 0) {
                break;
            }
        }
        break;
    }
    gPick_model_to_view__raycast = m_to_v;
    return r;
}

// FUNCTION: CARMA2_HW 0x004e36f0
// FUNCTION: CARMA2_SW 0x004dfa80
// FUNCTION: CARMA2_CHEAT 0x004e9270
int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* pWorld, br_actor* pCamera, br_pixelmap* pViewport, int pPick_x, int pPick_y, dr_pick2d_cbfn* pCallback, void* pArg) {
    br_matrix34 camera_tfm;
    br_scalar scale;
    br_scalar cos_angle;
    br_scalar sin_angle;
    br_camera* camera_data;
    br_angle view_over_2;

    camera_data = pCamera->type_data;
    DRActorToRoot(pCamera, pWorld, &camera_tfm);
    BrMatrix34Inverse(&gPick_model_to_view__raycast, &camera_tfm);
    view_over_2 = camera_data->field_of_view / 2;
    cos_angle = BR_COS(view_over_2);
    sin_angle = BR_SIN(view_over_2);
    scale = cos_angle / sin_angle;
    BrMatrix34PostScale(&gPick_model_to_view__raycast, scale / camera_data->aspect, scale, 1.0f);
    BrMatrix34PostShearZ(&gPick_model_to_view__raycast,
        (float)(2 * pPick_x) / (float)pViewport->width,
        (float)(-2 * pPick_y) / (float)pViewport->height);
    return ActorPick2D(pWorld, NULL, gBlack_material, pCallback, pArg);
}

// DRScenePick2D

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e3d90
// STUB: CARMA2_SW 0x004e0120
// STUB: CARMA2_CHEAT 0x004e9910
int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, dr_modelpick2d_raycast_cbfn* pCallback, void* pArg) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4300
// FUNCTION: CARMA2_SW 0x004e0690
// FUNCTION: CARMA2_CHEAT 0x004e9e80
int C2_HOOK_CDECL FindHighestPolyCallBack__raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pArg) {
    br_scalar the_y;

    the_y = pPoint->v[1];
    if (the_y <= gCurrent_y) {
        if (the_y > gHighest_y_below) {
            gHighest_y_below = the_y;
            gBelow_face_index = pF;
            gBelow_model = pModel;
        }
    } else {
        if (the_y < gLowest_y_above) {
            gLowest_y_above = the_y;
            gAbove_face_index = pF;
            gAbove_model = pModel;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e3d50
// FUNCTION: CARMA2_SW 0x004e00e0
// FUNCTION: CARMA2_CHEAT 0x004e98d0
int C2_HOOK_CDECL FindHighestCallBack__raycast(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__raycast, pArg);
    }
    return 0;
}

// STUB: CARMA2_HW 0x004e3bc0
// STUB: CARMA2_SW 0x004dff50
// STUB: CARMA2_CHEAT 0x004e9740
void C2_HOOK_FASTCALL FindBestY(br_vector3* pPosition, br_actor* pWorld, br_scalar pStarting_height, br_scalar* pNearest_y_above, br_scalar* pNearest_y_below, br_model** pNearest_above_model, br_model** pNearest_below_model, int* pNearest_above_face_index, int* pNearest_below_face_index) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004e45b0
// FUNCTION: CARMA2_SW 0x004e0940
// FUNCTION: CARMA2_CHEAT 0x004ea130
int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext) {
    br_scalar the_y;

    if (pMaterial != NULL && pMaterial->identifier != NULL
            && (pMaterial->identifier[0] == '!' || pMaterial->identifier[0] == '>')) {
        return 0;
    }
    the_y = pPoint->v[1];
    if (the_y > gHighest_y_below) {
        gHighest_y_below = the_y;
        gMaterial_below = pMaterial;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004e4570
// FUNCTION: CARMA2_SW 0x004e0900
// FUNCTION: CARMA2_CHEAT 0x004ea0f0
int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg) {

    if (gProgram_state.current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindYVerticallyBelowPolyCallBack, pArg);
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e4370
// STUB: CARMA2_SW 0x004e0700
// STUB: CARMA2_CHEAT 0x004e9ef0
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e4600
// FUNCTION: CARMA2_SW 0x004e0990
// FUNCTION: CARMA2_CHEAT 0x004ea180
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point) {
    br_scalar result;
    int number_of_attempts;
    br_vector3 cast_point;

    number_of_attempts = 0;
    BrVector3Copy(&cast_point, pCast_point);
    do {
        result = FindYVerticallyBelow(&cast_point);
        cast_point.v[1] += .2f;
    } while (result < -100.f && number_of_attempts++ < 10);
    return result;
}


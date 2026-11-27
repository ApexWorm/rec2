/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "34-finteray.h"

#include "49-physics.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x005964c0
// GLOBAL: CARMA2_SW 0x00591404
int gPling_materials = 1;

// GLOBAL: CARMA2_HW 0x00686190
// GLOBAL: CARMA2_SW 0x006808d0
// GLOBAL: CARMA2_CHEAT 0x0068da38
br_scalar gNearest_T;

// GLOBAL: CARMA2_HW 0x006861c8
// GLOBAL: CARMA2_SW 0x00680908
// GLOBAL: CARMA2_CHEAT 0x0068da70
int gTemp_group;

// GLOBAL: CARMA2_HW 0x006861cc
// GLOBAL: CARMA2_SW 0x0068090c
// GLOBAL: CARMA2_CHEAT 0x0068da74
br_actor* gNearest_actor;

// GLOBAL: CARMA2_HW 0x00686188
// GLOBAL: CARMA2_SW 0x006808c8
// GLOBAL: CARMA2_CHEAT 0x0068da30
int gNearest_face;

// GLOBAL: CARMA2_HW 0x00686194
// GLOBAL: CARMA2_SW 0x006808d4
// GLOBAL: CARMA2_CHEAT 0x0068da3c
int gNearest_face_group;

// GLOBAL: CARMA2_HW 0x0068618c
// GLOBAL: CARMA2_SW 0x006808cc
// GLOBAL: CARMA2_CHEAT 0x0068da34
br_model* gNearest_model;

// GLOBAL: CARMA2_HW 0x00686198
// GLOBAL: CARMA2_SW 0x006808d8
// GLOBAL: CARMA2_CHEAT 0x0068da40
br_matrix34 gPick_model_to_view__finteray;

// BadDiv

// DRVector2AccumulateScale

// FUNCTION: CARMA2_HW 0x0045cd90
// FUNCTION: CARMA2_SW 0x0045c7d0
// FUNCTION: CARMA2_CHEAT 0x0045d200
int C2_HOOK_FASTCALL PickBoundsTestRay_finteray(br_bounds* pBounds, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, br_scalar* pNew_t_near, br_scalar* pNew_t_far) {
    int i;
    br_scalar inverse;
    br_scalar t;

    for (i = 0; i < 3; i++) {
        if (pRay_dir->v[i] < -2 * BR_SCALAR_EPSILON) {
            inverse = pRay_dir->v[i];
            inverse = -1.f / inverse;
            t = (pRay_pos->v[i] - pBounds->max.v[i]) * inverse;
            if (t > BR_SCALAR_MAX) {
                pT_near = BR_SCALAR_MAX;
            } else if (t > pT_near) {
                pT_near = t;
            }
            t = (pRay_pos->v[i] - pBounds->min.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                pT_far = BR_SCALAR_MIN;
            } else if (t < pT_far) {
                pT_far = t;
            }
        } else if (pRay_dir->v[i] > 2 * BR_SCALAR_EPSILON) {
            inverse = pRay_dir->v[i];
            inverse = -1.f / inverse;
            t = (pRay_pos->v[i] - pBounds->max.v[i]) * inverse;
            if (t < BR_SCALAR_MIN) {
                pT_far = BR_SCALAR_MIN;
            } else if (t < pT_far) {
                pT_far = t;
            }
            t = (pRay_pos->v[i] - pBounds->min.v[i]) * inverse;
            if (t > BR_SCALAR_MAX) {
                pT_near = BR_SCALAR_MAX;
            } else if (t > pT_near) {
                pT_near = t;
            }
        } else if (pRay_pos->v[i] > pBounds->max.v[i] || pRay_pos->v[i] < pBounds->min.v[i]) {
            return 0;
        }
    }
    if (pT_near <= pT_far) {
        *pNew_t_near = pT_near;
        *pNew_t_far = pT_far;
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0045cb70
// FUNCTION: CARMA2_SW 0x0045c5b0
// FUNCTION: CARMA2_CHEAT 0x0045cfe0
int C2_HOOK_FASTCALL ActorRayPick2D(br_actor* pActor, br_vector3* pPosition, br_vector3* pDir, br_model* pModel, br_material* pMaterial, dr_pick2d_cbfn* pCallback) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_matrix34 mat;
    br_matrix34 invmat;
    br_vector3 pos;
    br_vector3 dir;
    void* arg;

    t_near = 0.f;
    t_far = 1.f;
    r = 0;
    arg = NULL;
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
    if (pActor->identifier != NULL && pActor->identifier[0] == '&') {
        BrTransformToMatrix34(&mat, &pActor->t);
        BrMatrix34Inverse(&invmat, &mat);
        BrMatrix34ApplyP(&pos, pPosition, &invmat);
        BrMatrix34ApplyV(&dir, pDir, &invmat);
        pPosition = &pos;
        pDir = &dir;
    }
    switch (pActor->type) {
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            if (PickBoundsTestRay_finteray(&this_model->bounds, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
                t_near = 0.f;
                if (gNearest_T > 1.f) {
                    t_far = 1.f;
                } else {
                    t_far = gNearest_T;
                }
                r = pCallback(pActor, this_model, this_material, pPosition, pDir, t_near, t_far, arg);
                if (r) {
                    break;
                }
            }
            if (r) {
                break;
            }
        }
        /* fall through */
    default:
        for (a = pActor->children; a != NULL; a = a->next) {
            r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, pCallback);
            if (r) {
                break;
            }
        }
        break;
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        if (PickBoundsTestRay_finteray(pActor->type_data, pPosition, pDir, t_near, t_far, &t_near, &t_far)) {
            for (a = pActor->children; a != NULL; a = a->next) {
                r = ActorRayPick2D(a, pPosition, pDir, this_model, this_material, pCallback);
                if (r) {
                    break;
                }
            }
        }
        break;
    }
    return r;
}

// DRSceneRayPick2D

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0045cf60
// STUB: CARMA2_SW 0x0045c9a0
int C2_HOOK_FASTCALL DRModelPick2D_finteray(br_model* pModel, br_material* pMaterial, br_actor* pActor, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, dr_modelpick2d_cbfn* pCallback, void* pArg) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0045d570
// FUNCTION: CARMA2_SW 0x0045cfb0
// FUNCTION: CARMA2_CHEAT 0x0045d9e0
int C2_HOOK_CDECL FindHighestPolyCallBack(br_model* pModel, br_actor* pActor, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pData) {

    if (pT < gNearest_T) {
        gNearest_T = pT;
        gNearest_model = pModel;
        gNearest_actor = pActor;
        gNearest_face = pF;
        gNearest_face_group = gTemp_group;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045cf20
// FUNCTION: CARMA2_SW 0x0045c960
// FUNCTION: CARMA2_CHEAT 0x0045d390
int C2_HOOK_CDECL FindHighestCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    DRModelPick2D_finteray(pModel, pMaterial, pActor, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack, pArg);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045ca60
// FUNCTION: CARMA2_SW 0x0045c4a0
// FUNCTION: CARMA2_CHEAT 0x0045ced0
void C2_HOOK_FASTCALL ActorFindFace(br_vector3* pPosition, br_vector3* pDir, br_actor* pWorld, br_vector3* pNormal, br_scalar* pT, br_material** pMaterial, br_actor** pActor) {
    int group;

    gNearest_T = 100.f;
    BrMatrix34Inverse(&gPick_model_to_view__finteray, &pWorld->t.t.mat);
    ActorRayPick2D(pWorld, pPosition, pDir, NULL, NULL, FindHighestCallBack);
    *pT = gNearest_T;
    if (gNearest_T < 100.f) {
        group = gNearest_face_group;
        BrVector3Copy(pNormal, &V11MODEL(gNearest_model)->groups[group].faces[gNearest_face].eqn);
        *pMaterial = *V11MODEL(gNearest_model)->groups[group].face_colours.materials;
        if (pActor != NULL) {
            *pActor = gNearest_actor;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045d5c0
// FUNCTION: CARMA2_SW 0x0045d000
// FUNCTION: CARMA2_CHEAT 0x0045da30
void C2_HOOK_FASTCALL EnablePlingMaterials(void) {

    gPling_materials = 1;
}

// FUNCTION: CARMA2_HW 0x0045d5d0
// FUNCTION: CARMA2_SW 0x0045d010
// FUNCTION: CARMA2_CHEAT 0x0045da40
void C2_HOOK_FASTCALL DisablePlingMaterials(void) {

    gPling_materials = 0;
}

// FUNCTION: CARMA2_HW 0x0045d860
// FUNCTION: CARMA2_SW 0x0045d2a0
void C2_HOOK_FASTCALL StationarySphereSphereCollide(const br_vector3* pPoint, const br_vector3* pSphere_center, br_scalar pRadius, br_vector3* pNormal, br_scalar* pCollision_t) {
    br_vector3 dp;
    br_scalar distance_squared;
    br_scalar scale;

    BrVector3Sub(&dp, pPoint, pSphere_center);
    distance_squared = BrVector3LengthSquared(&dp);
    if (distance_squared < REC2_SQR(pRadius + 0.002f)
            && distance_squared > REC2_SQR(pRadius)) {
        scale = (br_scalar)(1.0 / sqrt(distance_squared));
        BrVector3Scale(pNormal, &dp, scale);
        *pCollision_t = 1.0f;
    }
}

// SphereSphereCollide

// FUNCTION: CARMA2_HW 0x0045dea0
// FUNCTION: CARMA2_SW 0x0045d8e0
// FUNCTION: CARMA2_CHEAT 0x0045e310
void C2_HOOK_FASTCALL StationarySphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pEdge_direction, br_scalar pEdge_length_squared, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_vector3 tv;
    br_scalar ratio;
    br_scalar length_squared;
    br_scalar length;

    BrVector3Sub(&tv, pSphere_end, pEdge_start);
    ratio = BrVector3Dot(&tv, pEdge_direction) / pEdge_length_squared;
    if (ratio <= 0.0f) {
        BrVector3Copy(pPoint, pEdge_start);
    } else if (ratio >= 1.0f) {
        BrVector3Copy(pPoint, pEdge_end);
    } else {
        BrVector3Scale(pPoint, pEdge_direction, ratio);
        BrVector3Accumulate(pPoint, pEdge_start);
    }
    BrVector3Sub(pNormal, pPoint, pSphere_end);
    length_squared = BrVector3LengthSquared(pNormal);
    if (length_squared > pSphere->radius_squared
            && length_squared < pSphere->radius_squared + (pSphere->radius * 0.002f) * 2.0f) {
        length = (br_scalar)sqrt(length_squared);
        *pT = 1.f;
        pNormal->v[0] /= length;
        pNormal->v[1] /= length;
        pNormal->v[2] /= length;
    }
}

// FUNCTION: CARMA2_HW 0x0045e020
// FUNCTION: CARMA2_SW 0x0045da60
// FUNCTION: CARMA2_CHEAT 0x0045e490
void C2_HOOK_FASTCALL PointSphereCollision(br_vector3* pVertex, br_vector3* pSphere_start, br_vector3* pSphere_end, br_vector3* pSphere_direction, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    br_vector3 delta;
    br_vector3 position;
    br_vector3 normal;
    br_scalar direction_squared;
    br_scalar projection;
    br_scalar distance_squared;
    br_scalar time;

    BrVector3Sub(&delta, pVertex, pSphere_start);
    direction_squared = BrVector3LengthSquared(pSphere_direction);
    projection = BrVector3Dot(pSphere_direction, &delta) / direction_squared;
    distance_squared = BrVector3LengthSquared(&delta) - (projection * projection) * direction_squared;
    if (distance_squared > pSphere->radius_squared) {
        return;
    }
    time = projection - (br_scalar)sqrt((pSphere->radius_squared - distance_squared) / direction_squared);
    if (time < 0.0f || time > 1.0f) {
        return;
    }
    BrVector3Copy(pPoint, pVertex);
    BrVector3Scale(&position, pSphere_direction, time);
    BrVector3Accumulate(&position, pSphere_start);
    BrVector3Sub(&normal, pVertex, &position);
    BrVector3Normalise(pNormal, &normal);
    *pT = time;
}

// FUNCTION: CARMA2_HW 0x0045d920
// FUNCTION: CARMA2_SW 0x0045d360
// FUNCTION: CARMA2_CHEAT 0x0045dd90
void C2_HOOK_FASTCALL SphereEdgeCollide(br_vector3* pEdge_start, br_vector3* pEdge_end, br_vector3* pSphere_start, br_vector3* pSphere_end, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT, int pStart_flags, int pEnd_flags) {
    br_vector3 edge;
    br_vector3 sphere_direction;
    br_vector3 along;
    br_vector3 perpendicular;
    br_vector3 offset;
    br_vector3 shifted;
    br_vector3 movement;
    br_vector3 position;
    br_vector3 normal;
    br_scalar edge_squared;
    br_scalar perpendicular_squared;
    br_scalar projection;
    br_scalar time;
    br_scalar distance;
    br_scalar edge_parameter;

    *pT = 100.0f;
    BrVector3Sub(&edge, pEdge_end, pEdge_start);
    BrVector3Sub(&sphere_direction, pSphere_end, pSphere_start);
    edge_squared = BrVector3LengthSquared(&edge);
    projection = BrVector3Dot(&sphere_direction, &edge) / edge_squared;
    BrVector3Scale(&along, &edge, projection);
    BrVector3Sub(&perpendicular, &sphere_direction, &along);
    BrVector3Sub(&offset, pEdge_start, pSphere_start);
    projection = -BrVector3Dot(&offset, &edge) / edge_squared;
    perpendicular_squared = BrVector3LengthSquared(&perpendicular);
    BrVector3Scale(&shifted, &edge, projection);
    BrVector3Accumulate(&shifted, &offset);
    if (perpendicular_squared >= 1e-6) {
        time = BrVector3Dot(&shifted, &perpendicular) / perpendicular_squared;
        BrVector3Scale(&perpendicular, &perpendicular, time);
        distance = BrVector3LengthSquared(&shifted) - BrVector3LengthSquared(&perpendicular);
        if (distance <= pSphere->radius_squared + pSphere->radius * 0.002f * 2.0f) {
            if (distance <= pSphere->radius_squared) {
                time -= (br_scalar)sqrt((pSphere->radius_squared - distance) / perpendicular_squared);
                BrVector3Scale(&movement, &along, time);
                BrVector3Accumulate(&movement, &shifted);
                BrVector3Sub(&movement, &movement, &offset);
                edge_parameter = BrVector3Dot(&movement, &edge) / edge_squared;
                if (edge_parameter < 0.0f) {
                    if (!(pStart_flags & 0x80)) {
                        PointSphereCollision(pEdge_start, pSphere_start, pSphere_end, &sphere_direction, pSphere, pPoint, pNormal, pT);
                    }
                    if (*pT >= 2.0f) {
                        StationarySphereEdgeCollide(pEdge_start, pEdge_end, &edge, edge_squared, pSphere_end, pSphere, pPoint, pNormal, pT);
                    }
                } else if (edge_parameter > 1.0f) {
                    if (!(pEnd_flags & 0x80)) {
                        PointSphereCollision(pEdge_end, pSphere_start, pSphere_end, &sphere_direction, pSphere, pPoint, pNormal, pT);
                    }
                    if (*pT >= 2.0f) {
                        StationarySphereEdgeCollide(pEdge_start, pEdge_end, &edge, edge_squared, pSphere_end, pSphere, pPoint, pNormal, pT);
                    }
                } else if (time < 0.0f || time > 1.0f) {
                    StationarySphereEdgeCollide(pEdge_start, pEdge_end, &edge, edge_squared, pSphere_end, pSphere, pPoint, pNormal, pT);
                } else {
                    BrVector3Add(pPoint, pEdge_start, &movement);
                    BrVector3Scale(&position, &sphere_direction, time);
                    BrVector3Accumulate(&position, pSphere_start);
                    BrVector3Sub(&normal, pPoint, &position);
                    BrVector3Normalise(pNormal, &normal);
                    *pT = time;
                }
            } else {
                StationarySphereEdgeCollide(pEdge_start, pEdge_end, &edge, edge_squared, pSphere_end, pSphere, pPoint, pNormal, pT);
            }
        }
    } else {
        StationarySphereEdgeCollide(pEdge_start, pEdge_end, &edge, edge_squared, pSphere_end, pSphere, pPoint, pNormal, pT);
    }
}

// SpherePolyFaceCollide

// SphereBoxFaceCollide

// SphereFaceCollide

// STUB: CARMA2_HW 0x0045ecc0
// STUB: CARMA2_SW 0x0045e730
// STUB: CARMA2_CHEAT 0x0045f130
void C2_HOOK_FASTCALL CheckSingleFace(tFace_ref* pFace, br_vector3* ray_pos, br_vector3* ray_dir, br_vector3* normal, br_scalar* rt, br_vector3* coll_pos) {
    NOT_IMPLEMENTED();
}

// MultiRayCheckSingleFace

// FUNCTION: CARMA2_HW 0x0045f5b0
// FUNCTION: CARMA2_SW 0x0045f020
// FUNCTION: CARMA2_CHEAT 0x0045fa20
void C2_HOOK_FASTCALL GetNewBoundingBox(br_bounds* b2, const br_bounds* b1, const br_matrix34* m) {
    br_vector3 a;
    br_vector3 c[3];
    int j;
    int i;

    BrMatrix34ApplyP(&b2->min, &b1->min, m);
    BrVector3Copy(&b2->max, &b2->min);
    BrVector3Sub(&a, &b1->max, &b1->min);
    for (j = 0; j < 3; j++) {
        BrVector3Scale(&c[j], (br_vector3*)m->m[j], a.v[j]);
    }
    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            if (c[i].v[j] < 0.0f) {
                b2->min.v[j] += c[i].v[j];
            } else {
                b2->max.v[j] += c[i].v[j];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045f690
// FUNCTION: CARMA2_SW 0x0045f100
// FUNCTION: CARMA2_CHEAT 0x0045fb00
void C2_HOOK_FASTCALL FillInBounds(tBounds* pBounds) {
    br_vector3 tv;
    br_vector3 c[3];
    int i;

    BrVector3Add(&tv, &pBounds->original_bounds.min, &pBounds->original_bounds.max);
    BrVector3InvScale(&tv, &tv, 2.0f);
    BrMatrix34ApplyP(&pBounds->box_centre, &tv, pBounds->mat);
    BrVector3Sub(&tv, &pBounds->original_bounds.max, &pBounds->original_bounds.min);
    pBounds->radius = BrVector3Length(&tv) / 2.0f;
    BrMatrix34ApplyP(&pBounds->real_bounds.min, &pBounds->original_bounds.min, pBounds->mat);
    BrVector3Copy(&pBounds->real_bounds.max, &pBounds->real_bounds.min);
    for (i = 0; i < 3; ++i) {
        BrVector3Scale(&c[i], (br_vector3*)pBounds->mat->m[i], tv.v[i]);
    }
    for (i = 0; i < 3; ++i) {
        pBounds->real_bounds.min.v[i] += c[0].v[i] * (float)(c[0].v[i] < 0.0f)
            + c[1].v[i] * (float)(c[1].v[i] < 0.0f)
            + c[2].v[i] * (float)(c[2].v[i] < 0.0f);
        pBounds->real_bounds.max.v[i] += c[0].v[i] * (float)(c[0].v[i] > 0.0f)
            + c[1].v[i] * (float)(c[1].v[i] > 0.0f)
            + c[2].v[i] * (float)(c[2].v[i] > 0.0f);
    }
}

// STUB: CARMA2_HW 0x0045f8c0
// STUB: CARMA2_SW 0x0045f330
// STUB: CARMA2_CHEAT 0x0045fd30
int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    NOT_IMPLEMENTED();
}

// ModelPickBox

// ClipToPlaneGE

// ClipToPlaneLE

// BoundsOverlapTest

// BoundsTransformTest

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00460bd0
// STUB: CARMA2_SW 0x00460640
// STUB: CARMA2_CHEAT 0x00461040
int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_scalar* pT) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00460da0
// FUNCTION: CARMA2_SW 0x00460810
int C2_HOOK_FASTCALL SphereBoxIntersection(const br_bounds* pBounds, const br_vector3* pCenter, br_scalar pRadius_squared, br_vector3* pClosest) {
    int axis;
    br_scalar closest;
    br_scalar distance;

    distance = 0.f;
    for (axis = 0; axis < 3; axis++) {
        if (pCenter->v[axis] <= pBounds->min.v[axis]) {
            closest = pBounds->min.v[axis];
        } else if (pCenter->v[axis] >= pBounds->max.v[axis]) {
            closest = pBounds->max.v[axis];
        } else {
            closest = pCenter->v[axis];
        }
        pClosest->v[axis] = closest;
        distance += (pCenter->v[axis] - closest) * (pCenter->v[axis] - closest);
    }
    return distance <= pRadius_squared;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00460e10
// STUB: CARMA2_SW 0x00460880
int C2_HOOK_FASTCALL LineBoxCollWithSphere(br_vector3* pStart, br_vector3* pEnd, br_bounds* pBounds, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00460f60
// STUB: CARMA2_SW 0x004609e0
int C2_HOOK_FASTCALL LineSphereColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004611b0
// STUB: CARMA2_SW 0x00460c40
int C2_HOOK_FASTCALL LineSphereCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_sphere_data* pSphere, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00461310
// STUB: CARMA2_SW 0x00460db0
// STUB: CARMA2_CHEAT 0x00461780
int C2_HOOK_FASTCALL LinePolyColl(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron_data* pPoly, br_vector3* pPoint, br_scalar pRadius, br_scalar* pT) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SpherePolyIntersection

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00461520
// STUB: CARMA2_SW 0x00460fc0
int C2_HOOK_FASTCALL LinePolyCollWithSphere(br_vector3* pStart, br_vector3* pEnd, tCollision_shape_polyhedron* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00461740
// FUNCTION: CARMA2_SW 0x004611e0
int C2_HOOK_FASTCALL ShapeRayCast(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_scalar* pT, br_vector3* pNormal) {
    int result;
    int axis;
    int plane_index;

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        result = LineBoxColl(pStart, pEnd, &pShape->common.bb, pPoint, pT);
        if (pNormal != NULL && result > 0) {
            axis = (result - 1) & 0x3;
            BrVector3Set(pNormal, 0.f, 0.f, 0.f);
            pNormal->v[axis] = (result & 0x4) ? 1.0f : -1.0f;
        }
        return result;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        result = LinePolyColl(pStart, pEnd, &pShape->polyhedron.polyhedron, pPoint, 0.0f, pT);
        if (pNormal != NULL && result > 0) {
            plane_index = result & 0x3f;
            plane_index--;
            BrVector3Copy(pNormal, &pShape->polyhedron.polyhedron.planes[plane_index]);
        }
        return result;
    case kCollisionShapeType_Sphere:
        return LineSphereColl(pStart, pEnd, &pShape->sphere.sphere, pPoint, pNormal, pT);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, "");
    case kCollisionShapeType_Wireframe:
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00461850
// FUNCTION: CARMA2_SW 0x004612f0
int C2_HOOK_FASTCALL ShapeRayCastWithSphere(br_vector3* pStart, br_vector3* pEnd, tPhysics_shape* pShape, br_vector3* pPoint, br_vector3* pNormal, br_scalar* pT) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        return LineBoxCollWithSphere(pStart, pEnd, &pShape->common.bb, pPoint, pNormal, pT);
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        return LinePolyCollWithSphere(pStart, pEnd, &pShape->polyhedron, pPoint, pNormal, pT);
    case kCollisionShapeType_Sphere:
        return LineSphereCollWithSphere(pStart, pEnd, &pShape->sphere.sphere, pPoint, pNormal, pT);
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, "");
    case kCollisionShapeType_Wireframe:
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004618e0
// FUNCTION: CARMA2_SW 0x00461380
// FUNCTION: CARMA2_CHEAT 0x00461d50
tFace_ref* C2_HOOK_FASTCALL CastRayDownThruFaces(br_vector3* pPosition, tFace_ref* pFaces, int pCount, br_scalar pOffset, br_vector3* pCollision_position) {
    br_vector3 direction;
    br_vector3 position;
    br_vector3 normal;
    br_scalar t;
    tFace_ref* face;

    BrVector3Set(&direction, 0.f, -1.f, 0.f);
    face = RayCastThruFaceList(pPosition, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    if (face == NULL && pOffset != 0.f) {
        BrVector3Set(&position, pPosition->v[0], pPosition->v[1] + pOffset, pPosition->v[2]);
        face = RayCastThruFaceList(&position, &direction, &normal, &t, pCollision_position, pFaces, pCount);
    }
    return face;
}


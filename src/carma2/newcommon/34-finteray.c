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

// GLOBAL: CARMA2_HW 0x005964c4
// GLOBAL: CARMA2_SW 0x00591408
// GLOBAL: CARMA2_CHEAT 0x0059cd44
int gActorBoxPick_StopGroovidelics = 1;

// GLOBAL: CARMA2_HW 0x0079d858
// GLOBAL: CARMA2_SW 0x00793db8
// GLOBAL: CARMA2_CHEAT 0x007a4ef8
tFace_ref* gPling_face;

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

// StationarySphereEdgeCollide

// PointSphereCollision

// SphereEdgeCollide

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

// FUNCTION: CARMA2_HW 0x0045f8c0
// FUNCTION: CARMA2_SW 0x0045f330
// FUNCTION: CARMA2_CHEAT 0x0045fd30
int C2_HOOK_FASTCALL ActorBoxPick(tBounds* bnds, br_actor* ap, br_model* model, br_material* material, tFace_ref* face_list, int max_face, br_matrix34* pMat, tWorld_callbacks* pWorld_callbacks) {
    br_model* this_model;
    br_material* this_material;
    int i;
    int n;
    int test_children;
    br_actor* a;
    br_actor* next_a;
    br_matrix34 mat;
    br_matrix34 mat2;
    br_matrix34 invmat;
    br_matrix34 box_to_actor;
    tBounds new_bounds;
    br_bounds br_bnds;

    i = 0;
    test_children = 1;
    if (ap->model != NULL) {
        this_model = ap->model;
    } else {
        this_model = model;
    }
    if (ap->material != NULL) {
        this_material = ap->material;
    } else {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return max_face;
    }
    if (ap->identifier != NULL && ap->identifier[0] == '&') {
        if (this_model == NULL) {
            return max_face;
        }
        if (ap->children == NULL && (ap->type != BR_ACTOR_MODEL || !BoundsTransformTest(&this_model->bounds, &bnds->real_bounds, &ap->t.t.mat))) {
            return max_face;
        }
        if (pMat == NULL) {
            pMat = &ap->t.t.mat;
        } else {
            BrMatrix34Mul(&mat, &ap->t.t.mat, pMat);
            pMat = &mat;
        }
        BrMatrix34LPInverse(&invmat, &ap->t.t.mat);
        BrMatrix34Mul(&mat2, bnds->mat, &invmat);
        new_bounds.mat = &mat2;
        BrVector3Copy(&new_bounds.original_bounds.min, &bnds->original_bounds.min);
        BrVector3Copy(&new_bounds.original_bounds.max, &bnds->original_bounds.max);
        BrMatrix34ApplyP(&new_bounds.box_centre, &bnds->box_centre, &invmat);
        new_bounds.radius = bnds->radius;
        GetNewBoundingBox(&new_bounds.real_bounds, &new_bounds.original_bounds, new_bounds.mat);
        if (ap->identifier[1] >= '0' && ap->identifier[1] <= '9') {
            if (!BoundsOverlapTest(&new_bounds.real_bounds, &this_model->bounds)) {
                return max_face;
            }
            BrMatrix34LPInverse(&invmat, bnds->mat);
            BrMatrix34Mul(&box_to_actor, &ap->t.t.mat, &invmat);
            GetNewBoundingBox(&br_bnds, &ap->model->bounds, &box_to_actor);
            if (!BoundsOverlapTest(&br_bnds, &bnds->original_bounds)) {
                return max_face;
            }
            if (pWorld_callbacks != NULL && pWorld_callbacks->pull_actor_from_world != NULL && pWorld_callbacks->pull_actor_from_world(ap) != NULL) {
                return max_face;
            }
        }
        bnds = &new_bounds;
    }
    if (ap->type == BR_ACTOR_MODEL) {
        if (this_model == NULL) {
            return max_face;
        }
        if (gActorBoxPick_StopGroovidelics && BoundsOverlapTest(&bnds->real_bounds, &this_model->bounds)) {
            n = ModelPickBox(ap, bnds, this_model, this_material, &face_list[i], max_face, pMat);
            if (pMat != NULL && max_face != n && pWorld_callbacks != NULL && pWorld_callbacks->stop_groovidelic != NULL) {
                pWorld_callbacks->stop_groovidelic(ap);
            }
            i = max_face - n;
            max_face = n;
        }
    } else if (ap->type == BR_ACTOR_BOUNDS || ap->type == BR_ACTOR_BOUNDS_CORRECT) {
        test_children = BoundsOverlapTest(&bnds->real_bounds, ap->type_data);
    }
    if (test_children) {
        for (a = ap->children; a != NULL; a = next_a) {
            next_a = a->next;
            n = ActorBoxPick(bnds, a, this_model, this_material, &face_list[i], max_face, pMat, pWorld_callbacks);
            i += max_face - n;
            max_face = n;
        }
    }
    return max_face;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0045ff90
// FUNCTION: CARMA2_SW 0x0045fa00
// FUNCTION: CARMA2_CHEAT 0x00460400
int C2_HOOK_FASTCALL ModelPickBox(br_actor* actor, tBounds* bnds, br_model* model, br_material* model_material, tFace_ref* face_list, int max_face, br_matrix34* pMat) {
    int f;
    int i;
    int n;
    int group;
    int v1;
    int v2;
    int v3;
    br_vector3 polygon[12];
    br_vector3 a;
    br_vector3 tv;
    br_scalar t;
    v11model* prepared;
    v11group* v11g;
    v11face* v11f;

    prepared = model->prepared;
    if (max_face <= 0) {
        return 0;
    }
    for (group = 0; group < prepared->ngroups; group++) {
        v11g = &prepared->groups[group];
        for (f = 0; f < v11g->nfaces; f++) {
            v11f = &v11g->faces[f];
            v1 = v11f->vertices[0];
            BrVector3Sub(&a, &v11g->vertices[v1].p, &bnds->box_centre);
            t = BrVector3Dot(&v11f->eqn, &a);
            if (fabs(t) > bnds->radius) {
                continue;
            }
            v2 = v11f->vertices[1];
            v3 = v11f->vertices[2];
            t = bnds->real_bounds.min.v[0];
            if (t > prepared->groups[group].vertices[v1].p.v[0] && t > prepared->groups[group].vertices[v2].p.v[0] && t > prepared->groups[group].vertices[v3].p.v[0]) {
                continue;
            }
            t = bnds->real_bounds.max.v[0];
            if (t < prepared->groups[group].vertices[v1].p.v[0] && t < prepared->groups[group].vertices[v2].p.v[0] && t < prepared->groups[group].vertices[v3].p.v[0]) {
                continue;
            }
            t = bnds->real_bounds.min.v[1];
            if (t > prepared->groups[group].vertices[v1].p.v[1] && t > prepared->groups[group].vertices[v2].p.v[1] && t > prepared->groups[group].vertices[v3].p.v[1]) {
                continue;
            }
            t = bnds->real_bounds.max.v[1];
            if (t < prepared->groups[group].vertices[v1].p.v[1] && t < prepared->groups[group].vertices[v2].p.v[1] && t < prepared->groups[group].vertices[v3].p.v[1]) {
                continue;
            }
            t = bnds->real_bounds.min.v[2];
            if (t > prepared->groups[group].vertices[v1].p.v[2] && t > prepared->groups[group].vertices[v2].p.v[2] && t > prepared->groups[group].vertices[v3].p.v[2]) {
                continue;
            }
            t = bnds->real_bounds.max.v[2];
            if (t < prepared->groups[group].vertices[v1].p.v[2] && t < prepared->groups[group].vertices[v2].p.v[2] && t < prepared->groups[group].vertices[v3].p.v[2]) {
                continue;
            }
            BrVector3Sub(&polygon[1], &V11MODEL(model)->groups[group].vertices[v1].p, (br_vector3*)bnds->mat->m[3]);
            BrVector3Sub(&polygon[2], &V11MODEL(model)->groups[group].vertices[v2].p, (br_vector3*)bnds->mat->m[3]);
            BrVector3Sub(&polygon[3], &V11MODEL(model)->groups[group].vertices[v3].p, (br_vector3*)bnds->mat->m[3]);
            BrMatrix34TApplyV(&polygon[0], &polygon[1], bnds->mat);
            BrMatrix34TApplyV(&polygon[1], &polygon[2], bnds->mat);
            BrMatrix34TApplyV(&polygon[2], &polygon[3], bnds->mat);
            n = 3;
            for (i = 0; i < 3; i++) {
                ClipToPlaneGE(bnds->original_bounds.min.v[i], polygon, &n, i);
                if (n < 3) {
                    break;
                }
                ClipToPlaneLE(bnds->original_bounds.max.v[i], polygon, &n, i);
                if (n < 3) {
                    break;
                }
            }
            if (n >= 3) {
                if (pMat != NULL) {
                    BrMatrix34ApplyP(&face_list->v[0], &V11MODEL(model)->groups[group].vertices[v1].p, pMat);
                    BrMatrix34ApplyP(&face_list->v[1], &V11MODEL(model)->groups[group].vertices[v2].p, pMat);
                    BrMatrix34ApplyP(&face_list->v[2], &V11MODEL(model)->groups[group].vertices[v3].p, pMat);
                    BrVector3Copy(&tv, &v11f->eqn);
                    BrMatrix34ApplyV(&face_list->normal, &tv, pMat);
                } else {
                    BrVector3Copy(&face_list->v[0], &V11MODEL(model)->groups[group].vertices[v1].p);
                    BrVector3Copy(&face_list->v[1], &V11MODEL(model)->groups[group].vertices[v2].p);
                    BrVector3Copy(&face_list->v[2], &V11MODEL(model)->groups[group].vertices[v3].p);
                    BrVector3Copy(&face_list->normal, &v11f->eqn);
                }
                if (V11MODEL(model)->groups[group].face_colours.materials[0] != NULL) {
                    face_list->material = V11MODEL(model)->groups[group].face_colours.materials[0];
                } else {
                    face_list->material = model_material;
                }
                if (pMat != NULL) {
                    face_list->d = BrVector3Dot(&face_list->v[0], &face_list->normal);
                } else {
                    face_list->d = v11f->eqn.v[3];
                }
                face_list->a = actor;
                if (model->flags & BR_MODF_UPDATEABLE) {
                    face_list->g = -1;
                    face_list->f = V11MODEL(model)->groups[group].face_user[f];
                    face_list->flags = model->faces[face_list->f].flags;
                    face_list->map[0] = &model->vertices[model->faces[face_list->f].vertices[0]].map;
                    face_list->map[1] = &model->vertices[model->faces[face_list->f].vertices[1]].map;
                    face_list->map[2] = &model->vertices[model->faces[face_list->f].vertices[2]].map;
                } else {
                    face_list->g = group;
                    face_list->f = f;
                    face_list->flags = 0;
                    if (v11f->edges[0] == 0) {
                        face_list->flags |= 0x1;
                    }
                    if (v11f->edges[1] == 0) {
                        face_list->flags |= 0x2;
                    }
                    if (v11f->edges[2] == 0) {
                        face_list->flags |= 0x4;
                    }
                    face_list->map[0] = &V11MODEL(model)->groups[group].vertices[v1].map;
                    face_list->map[1] = &V11MODEL(model)->groups[group].vertices[v2].map;
                    face_list->map[2] = &V11MODEL(model)->groups[group].vertices[v3].map;
                }
                if (face_list->material != NULL && face_list->material->identifier != NULL && face_list->material->identifier[0] == '!') {
                    gPling_face = face_list;
                }
                face_list++;
                max_face--;
                if (max_face == 0) {
                    break;
                }
            }
        }
        if (max_face == 0) {
            break;
        }
    }
    return max_face;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL ClipToPlaneGE(br_scalar limit, br_vector3* p, int* nv, int i) {
    int last_vertex;
    int j;
    int vertex;
    int k;
    br_vector3 p2[12];

    last_vertex = *nv - 1;
    j = 0;
    for (vertex = 0; vertex < *nv; vertex++) {
        if ((p[last_vertex].v[i] > limit) != (p[vertex].v[i] > limit)) {
            for (k = 0; k < 3; k++) {
                if (k != i) {
                    p2[j].v[k] = (p[vertex].v[k] - p[last_vertex].v[k]) * (limit - p[last_vertex].v[i]) / (p[vertex].v[i] - p[last_vertex].v[i]) + p[last_vertex].v[k];
                }
            }
            p2[j].v[i] = limit;
            j++;
        }
        if (p[vertex].v[i] >= limit) {
            BrVector3Copy(&p2[j], &p[vertex]);
            j++;
        }
        last_vertex = vertex;
    }
    *nv = j;
    for (k = 0; k < j; k++) {
        BrVector3Copy(&p[k], &p2[k]);
    }
}

inline void C2_HOOK_FASTCALL ClipToPlaneLE(br_scalar limit, br_vector3* p, int* nv, int i) {
    int last_vertex;
    int j;
    int vertex;
    int k;
    br_vector3 p2[12];

    last_vertex = *nv - 1;
    j = 0;
    for (vertex = 0; vertex < *nv; vertex++) {
        if ((p[last_vertex].v[i] > limit) != (p[vertex].v[i] > limit)) {
            for (k = 0; k < 3; k++) {
                if (k != i) {
                    p2[j].v[k] = (p[vertex].v[k] - p[last_vertex].v[k]) * (limit - p[last_vertex].v[i]) / (p[vertex].v[i] - p[last_vertex].v[i]) + p[last_vertex].v[k];
                }
            }
            p2[j].v[i] = limit;
            j++;
        }
        if (p[vertex].v[i] <= limit) {
            BrVector3Copy(&p2[j], &p[vertex]);
            j++;
        }
        last_vertex = vertex;
    }
    *nv = j;
    for (k = 0; k < j; k++) {
        BrVector3Copy(&p[k], &p2[k]);
    }
}

// BoundsOverlapTest

inline int C2_HOOK_FASTCALL BoundsTransformTest(br_bounds* b1, br_bounds* b2, br_matrix34* M) {
    br_scalar val;
    br_vector3 o;

    BrVector3Sub(&o, &b1->max, &b1->min);
    val = M->m[0][0] * b1->min.v[0] + M->m[1][0] * b1->min.v[1] + M->m[2][0] * b1->min.v[2] + M->m[3][0];
    if ((M->m[0][0] > 0.0f ? M->m[0][0] * o.v[0] : 0.0f)
            + (M->m[1][0] > 0.0f ? M->m[1][0] * o.v[1] : 0.0f)
            + (M->m[2][0] > 0.0f ? M->m[2][0] * o.v[2] : 0.0f)
            + val < b2->min.v[0]) {
        return 0;
    }
    if ((M->m[0][0] < 0.0f ? M->m[0][0] * o.v[0] : 0.0f)
            + (M->m[1][0] < 0.0f ? M->m[1][0] * o.v[1] : 0.0f)
            + (M->m[2][0] < 0.0f ? M->m[2][0] * o.v[2] : 0.0f)
            + val > b2->max.v[0]) {
        return 0;
    }
    val = M->m[0][2] * b1->min.v[0] + M->m[1][2] * b1->min.v[1] + M->m[2][2] * b1->min.v[2] + M->m[3][2];
    if ((M->m[0][2] > 0.0f ? M->m[0][2] * o.v[0] : 0.0f)
            + (M->m[1][2] > 0.0f ? M->m[1][2] * o.v[1] : 0.0f)
            + (M->m[2][2] > 0.0f ? M->m[2][2] * o.v[2] : 0.0f)
            + val < b2->min.v[2]) {
        return 0;
    }
    if ((M->m[0][2] < 0.0f ? M->m[0][2] * o.v[0] : 0.0f)
            + (M->m[1][2] < 0.0f ? M->m[1][2] * o.v[1] : 0.0f)
            + (M->m[2][2] < 0.0f ? M->m[2][2] * o.v[2] : 0.0f)
            + val > b2->max.v[2]) {
        return 0;
    }
    val = M->m[0][1] * b1->min.v[0] + M->m[1][1] * b1->min.v[1] + M->m[2][1] * b1->min.v[2] + M->m[3][1];
    if ((M->m[0][1] > 0.0f ? M->m[0][1] * o.v[0] : 0.0f)
            + (M->m[1][1] > 0.0f ? M->m[1][1] * o.v[1] : 0.0f)
            + (M->m[2][1] > 0.0f ? M->m[2][1] * o.v[2] : 0.0f)
            + val < b2->min.v[1]) {
        return 0;
    }
    if ((M->m[0][1] < 0.0f ? M->m[0][1] * o.v[0] : 0.0f)
            + (M->m[1][1] < 0.0f ? M->m[1][1] * o.v[1] : 0.0f)
            + (M->m[2][1] < 0.0f ? M->m[2][1] * o.v[2] : 0.0f)
            + val > b2->max.v[1]) {
        return 0;
    }
    return 1;
}

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


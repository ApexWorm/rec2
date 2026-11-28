/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "49-physics.h"

#include "00-car.h"
#include "09-funks.h"
#include "16-graphics1.h"
#include "22-replay.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "52-errors.h"
#include "65-hull.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "c2_math.h"
#include "c2_string.h"
#include "c2_stdlib.h"

#include <stddef.h>

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

// GLOBAL: CARMA2_HW 0x00679430
// GLOBAL: CARMA2_SW 0x00673ba0
// GLOBAL: CARMA2_CHEAT 0x00680ce0
tU32 gUINT_00679430;

// GLOBAL: CARMA2_HW 0x00679434
// GLOBAL: CARMA2_SW 0x00673ba4
// GLOBAL: CARMA2_CHEAT 0x00680ce4
int gINT_00679434;

// GLOBAL: CARMA2_HW 0x00679438
// GLOBAL: CARMA2_SW 0x00673ba8
// GLOBAL: CARMA2_CHEAT 0x00680ce8
int gINT_00679438;

// GLOBAL: CARMA2_HW 0x00692dc8
// GLOBAL: CARMA2_SW 0x0068ce20
// GLOBAL: CARMA2_CHEAT 0x0069a940
int gINT_00692dc8;

// GLOBAL: CARMA2_HW 0x00692dcc
// GLOBAL: CARMA2_SW 0x0068ce24
// GLOBAL: CARMA2_CHEAT 0x0069a944
int gPrepared_objects;

// GLOBAL: CARMA2_HW 0x00692dd0
// GLOBAL: CARMA2_SW 0x0068ce28
// GLOBAL: CARMA2_CHEAT 0x0069a948
br_scalar gPhysics_bounds_max_x;

// GLOBAL: CARMA2_HW 0x00692dd4
// GLOBAL: CARMA2_SW 0x0068ce2c
// GLOBAL: CARMA2_CHEAT 0x0069a94c
br_scalar gPhysics_bounds_min_x;

// GLOBAL: CARMA2_HW 0x00692dd8
// GLOBAL: CARMA2_SW 0x0068ce30
// GLOBAL: CARMA2_CHEAT 0x0069a950
int gINT_00692dd8;

// GLOBAL: CARMA2_HW 0x00692ddc
// GLOBAL: CARMA2_SW 0x0068ce34
// GLOBAL: CARMA2_CHEAT 0x0069a954
int gCount_friction_forces;

// GLOBAL: CARMA2_HW 0x00692de0
// GLOBAL: CARMA2_SW 0x0068ce38
// GLOBAL: CARMA2_CHEAT 0x0069a958
tPhysics_joint_force gFriction_forces[50];

// GLOBAL: CARMA2_HW 0x006940a0
// GLOBAL: CARMA2_SW 0x0068e0f8
// GLOBAL: CARMA2_CHEAT 0x0069bc18
int gPHIL_collision_group;

// GLOBAL: CARMA2_HW 0x006940a4
// GLOBAL: CARMA2_SW 0x0068e0fc
// GLOBAL: CARMA2_CHEAT 0x0069bc1c
int gINT_006940a4;

// GLOBAL: CARMA2_HW 0x006940a8
// GLOBAL: CARMA2_SW 0x0068e100
// GLOBAL: CARMA2_CHEAT 0x0069bc20
tPhysics_object** gReduced_object_list;

// GLOBAL: CARMA2_HW 0x006940ac
// GLOBAL: CARMA2_SW 0x0068e104
// GLOBAL: CARMA2_CHEAT 0x0069bc24
int gINT_006940ac;

// GLOBAL: CARMA2_HW 0x006940c0
// GLOBAL: CARMA2_SW 0x0068e118
// GLOBAL: CARMA2_CHEAT 0x0069bc38
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
// GLOBAL: CARMA2_SW 0x0073b2d8
// GLOBAL: CARMA2_CHEAT 0x0074beb8
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
// GLOBAL: CARMA2_SW 0x0073b2d4
// GLOBAL: CARMA2_CHEAT 0x0074beb4
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x0074480c
// GLOBAL: CARMA2_SW 0x0073b2cc
// GLOBAL: CARMA2_CHEAT 0x0074beac
int gTotal_count_smash_peds;

// GLOBAL: CARMA2_HW 0x00744810
// GLOBAL: CARMA2_SW 0x0073b2d0
// GLOBAL: CARMA2_CHEAT 0x0074beb0
int gImmortal_peds;

// GLOBAL: CARMA2_HW 0x00744804
// GLOBAL: CARMA2_SW 0x0073b2c4
// GLOBAL: CARMA2_CHEAT 0x0074bea4
int gBOOL_00744804;

// GLOBAL: CARMA2_HW 0x00744800
// GLOBAL: CARMA2_SW 0x0073b2c0
// GLOBAL: CARMA2_CHEAT 0x0074bea0
float gPed_head_scale;

// GLOBAL: CARMA2_HW 0x00744808
// GLOBAL: CARMA2_SW 0x0073b2c8
// GLOBAL: CARMA2_CHEAT 0x0074bea8
tPedestrian* gPedestrian_array;

// GLOBAL: CARMA2_HW 0x0074a5e0
// GLOBAL: CARMA2_SW 0x007410a0
// GLOBAL: CARMA2_CHEAT 0x00751c80
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x0074a5e4
// GLOBAL: CARMA2_SW 0x007410a4
// GLOBAL: CARMA2_CHEAT 0x00751c84
int gPHIL_mechanics_time_sync;

// GLOBAL: CARMA2_HW 0x0074a5e8
// GLOBAL: CARMA2_SW 0x007410a8
// GLOBAL: CARMA2_CHEAT 0x00751c88
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x006940c8
// GLOBAL: CARMA2_SW 0x0068e120
// GLOBAL: CARMA2_CHEAT 0x0069bc40
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x00694140
// GLOBAL: CARMA2_SW 0x0068e198
// GLOBAL: CARMA2_CHEAT 0x0069bcb8
tProximity_ray gProximity_rays[20];

// GLOBAL: CARMA2_HW 0x00694274
// GLOBAL: CARMA2_SW 0x0068e2cc
// GLOBAL: CARMA2_CHEAT 0x0069bdec
tPed_giblet_size_spec* gGiblet_sizes;

// GLOBAL: CARMA2_HW 0x00694278
// GLOBAL: CARMA2_SW 0x0068e2d0
// GLOBAL: CARMA2_CHEAT 0x0069bdf0
br_model* gPed_spurt_model;

// GLOBAL: CARMA2_HW 0x0069427c
// GLOBAL: CARMA2_SW 0x0068e2d4
// GLOBAL: CARMA2_CHEAT 0x0069bdf4
int gPed_spurty_lumps_count;

// GLOBAL: CARMA2_HW 0x00694268
// GLOBAL: CARMA2_SW 0x0068e2c0
// GLOBAL: CARMA2_CHEAT 0x0069bde0
int gPed_times[3];

// GLOBAL: CARMA2_HW 0x00694280
// GLOBAL: CARMA2_SW 0x0068e2d8
// GLOBAL: CARMA2_CHEAT 0x0069bdf8
br_actor* gPed_limbed_actors[30];

// GLOBAL: CARMA2_HW 0x006942f8
// GLOBAL: CARMA2_SW 0x0068e350
// GLOBAL: CARMA2_CHEAT 0x0069be70
tShrapnel_time_info gPed_spurt_min_max_time;

// GLOBAL: CARMA2_HW 0x006940c4
// GLOBAL: CARMA2_SW 0x0068e11c
// GLOBAL: CARMA2_CHEAT 0x0069bc3c
void* gPhysics_other_buffer;

// GLOBAL: CARMA2_HW 0x006940bc
// GLOBAL: CARMA2_SW 0x0068e114
// GLOBAL: CARMA2_CHEAT 0x0069bc34
void* gPhysics_scratch_ptr;

// GLOBAL: CARMA2_HW 0x006940b0
// GLOBAL: CARMA2_SW 0x0068e108
// GLOBAL: CARMA2_CHEAT 0x0069bc28
int gFace_count;

// GLOBAL: CARMA2_HW 0x00679438
// GLOBAL: CARMA2_SW 0x00673ba8
// GLOBAL: CARMA2_CHEAT 0x00680ce8
int gINT_00679438;

// GLOBAL: CARMA2_HW 0x0079ecbc
// GLOBAL: CARMA2_SW 0x00795248
// GLOBAL: CARMA2_CHEAT 0x007a635c
tPhysics_object* gObject_blocking_sensible_place;

// GLOBAL: CARMA2_HW 0x0065cf78
// GLOBAL: CARMA2_SW 0x006578b0
// GLOBAL: CARMA2_CHEAT 0x00664570
tWorld_callbacks gWorld_callbacks = {
    ProcessForcesCallback,
    ProcessJointForcesCallBack,
    NewFaceListCallBack,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
};

// count

// FUNCTION: CARMA2_HW 0x004225b0
// FUNCTION: CARMA2_SW 0x00422630
// FUNCTION: CARMA2_CHEAT 0x00422800
void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix) {
    br_vector3 relative;

    BrVector3Sub(&relative, source, (br_vector3*)matrix->m[3]);
    BrMatrix34TApplyV(dest, &relative, matrix);
}

// FUNCTION: CARMA2_HW 0x00422fe0
// FUNCTION: CARMA2_SW 0x00423060
// FUNCTION: CARMA2_CHEAT 0x00423160
int C2_HOOK_FASTCALL AddObjectWallForce(tPhysics_joint_force* pForces, const br_vector3* pPosition, br_scalar pT, const br_vector3* pNormal, br_scalar pUnused, int pCount, tPhysics_object* pObject, int pType, tFace_ref* pFace, const br_vector3* pReference_position) {
    br_bounds3 joint_bounds;
    br_scalar dx;
    br_scalar dy;
    br_scalar dz;
    br_scalar size;
    br_scalar d;
    br_scalar dot;
    int i;
    int j;
    tPhysics_joint_force* force;

    if (pObject->parent == NULL && pObject->physics_joint2 != NULL && pObject->physics_joint2->type != eJoint_none) {
        BrVector3Sub(&joint_bounds.max, &pObject->bb1.max, &pObject->physics_joint2->field_0x08);
        BrVector3Sub(&joint_bounds.min, &pObject->bb1.min, &pObject->physics_joint2->field_0x08);
        size = 0.0f;
        for (i = 0; i < 3; i++) {
            d = MIN((br_scalar)fabs(joint_bounds.max.v[i]), (br_scalar)fabs(joint_bounds.min.v[i]));
            if (d > size) {
                size = d;
            }
        }
        size = (br_scalar)(size + 0.01);
        BrVector3Sub(&joint_bounds.min, pPosition, &pObject->physics_joint2->field_0x08);
        if (fabs(joint_bounds.min.v[0]) < size) {
            d = (br_scalar)fabs(joint_bounds.min.v[1]);
            if (d < size
                && d + d < pObject->bb1.max.v[1] - pObject->bb1.min.v[1]
                && fabs(joint_bounds.min.v[2]) < size) {
                return pCount;
            }
        }
    }
    for (i = 0; i < pCount; i++) {
        dx = pForces[i].point_a.v[0] - (pPosition->v[0] - pObject->cmpos.v[0]);
        dy = pForces[i].point_a.v[1] - (pPosition->v[1] - pObject->cmpos.v[1]);
        dz = pForces[i].point_a.v[2] - (pPosition->v[2] - pObject->cmpos.v[2]);
        d = dx * dx + dy * dy + dz * dz;
        if (d < 0.0045) {
            dot = BrVector3Dot(&pForces[i].direction_a, pNormal);
            if (dot > 0.95 && d < 0.0005) {
                return pCount;
            }
            if (dot < -0.95) {
                return pCount;
            }
        }
    }
    if (pCount < 4) {
        pForces[pCount].field_0x0 = 2.0f;
        pCount++;
    }
    for (i = 0, force = pForces; i < pCount; i++, force++) {
        if (pT < force->field_0x0) {
            for (j = MIN(3, pCount); j >= i; j--) {
                pForces[j] = pForces[j - 1];
            }
            force->field_0x0 = pT;
            force->object_a = pObject;
            force->object_b = NULL;
            force->field_0xc = pFace;
            BrVector3Copy(&force->direction_a, pNormal);
            BrVector3Sub(&force->point_a, pPosition, &pObject->cmpos);
            if (pReference_position != NULL) {
                BrVector3Copy(&force->point_b, pReference_position);
            }
            force->type = pType;
            return pCount;
        }
    }
    return pCount;
}

// FUNCTION: CARMA2_HW 0x004268a0
// FUNCTION: CARMA2_SW 0x00426940
int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue) {
    int i;
    int j;
    tPhysics_joint_force* force;

    (void)pUnused;
    if (pCount < 4) {
        pForces[pCount].field_0x0 = 2.f;
        pCount++;
    }
    for (i = 0, force = pForces; i < pCount; i++, force++) {
        if (pT < force->field_0x0) {
            for (j = MIN(3, pCount); j >= i; j--) {
                pForces[j] = pForces[j - 1];
            }
            force->field_0x10 = pValue;
            force->object_a = pObject_a;
            force->object_b = pObject_b;
            BrVector3Sub(&force->point_a, pPoint_a, &pObject_a->cmpos);
            BrVector3Sub(&force->point_b, pPoint_b, &pObject_b->cmpos);
            BrMatrix34TApplyV(&force->direction_a, pNormal, &pObject_a->transform_matrix);
            BrMatrix34TApplyV(&force->direction_b, pNormal, &pObject_b->transform_matrix);
            force->field_0x0 = pT;
            force->type = pType;
            force->field_0xc = NULL;
            return pCount;
        }
    }
    return pCount;
}

// FUNCTION: CARMA2_HW 0x004225f0
// FUNCTION: CARMA2_SW 0x00422670
tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount) {
    br_vector3 normal;
    br_vector3 collision_position;
    br_scalar t;
    tFace_ref* best;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFace_ref, flags, 0x40);

    *pT = 2.f;
    for (i = 0; i < pCount; i++, pFaces++) {
        if ((pFaces->flags & 0x80) == 0) {
            CheckSingleFace(pFaces, pPosition, pDirection, &normal, &t, &collision_position);
            if (t < *pT) {
                *pT = t;
                BrVector3Copy(pNormal, &normal);
                best = pFaces;
                BrVector3Copy(pCollision_position, &collision_position);
            }
        }
    }
    if (*pT < 2.f) {
        return best;
    } else {
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004226d0
// FUNCTION: CARMA2_SW 0x00422750
// FUNCTION: CARMA2_CHEAT 0x00422920
tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList2(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount) {
    br_vector3 normal;
    br_vector3 collision_position;
    br_scalar t;
    tFace_ref* best;
    int i;

    *pT = 2.0f;
    for (i = 0; i < pCount; i++, pFaces++) {
        CheckSingleFace(pFaces, pPosition, pDirection, &normal, &t, &collision_position);
        if (t < *pT) {
            *pT = t;
            BrVector3Copy(pNormal, &normal);
            best = pFaces;
            BrVector3Copy(pCollision_position, &collision_position);
        }
    }
    if (*pT < 2.0f) {
        return best;
    } else {
        return NULL;
    }
}

inline tFace_ref* C2_HOOK_FASTCALL RayCastInBox(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tPhysics_object* pObject) {

    return RayCastThruFaceList(pPosition, pDirection, pNormal, pT, pCollision_position, &gFace_list__car[pObject->box_face_start], pObject->box_face_end - pObject->box_face_start);
}

inline void C2_HOOK_FASTCALL FaceOffCount2(void) {
}

inline void C2_HOOK_FASTCALL FaceOffCount(void) {
}

inline void C2_HOOK_FASTCALL FaceOnCount(void) {
}

// FUNCTION: CARMA2_HW 0x00423320
// FUNCTION: CARMA2_SW 0x004233a0
// FUNCTION: CARMA2_CHEAT 0x004234a0
int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pMat) {
    br_matrix34* mat;
    br_bounds3* bounds;
    br_vector3 corner;
    br_vector3 extents[3];
    br_vector3 tv;
    br_scalar low;
    br_scalar high;
    br_scalar d;
    br_scalar offset;
    br_scalar limit;
    int count;
    int i;
    int axis;
    tFace_ref* face;

    mat = &pObject->actor->t.t.mat;
    bounds = &pShape->common.field_0x1c;
    count = 0;
    GetNewBoundingBox(bounds, &pShape->common.bb, pMat);
    for (axis = 0; axis < 3; axis++) {
        if (pShape->common.bb.min.v[axis] < bounds->min.v[axis]) {
            bounds->min.v[axis] = pShape->common.bb.min.v[axis];
        }
        if (bounds->max.v[axis] < pShape->common.bb.max.v[axis]) {
            bounds->max.v[axis] = pShape->common.bb.max.v[axis];
        }
    }
    BrMatrix34ApplyV(&corner, &bounds->min, mat);
    BrVector3Accumulate(&corner, (br_vector3*)mat->m[3]);
    for (axis = 0; axis < 3; axis++) {
        BrVector3Scale(&extents[axis], (br_vector3*)mat->m[axis], bounds->max.v[axis] - bounds->min.v[axis]);
    }
    face = &gFace_list__car[pObject->box_face_start];
    for (i = 0; i < pObject->box_face_end - pObject->box_face_start; i++, face++) {
        BrVector3Sub(&tv, &corner, &face->v[0]);
        low = BrVector3Dot(&tv, &face->normal);
        high = low;
        for (axis = 0; axis < 3; axis++) {
            d = BrVector3Dot(&extents[axis], &face->normal);
            if (d >= 0.0f) {
                high += d;
            } else {
                low += d;
            }
        }
#ifdef REC2_FIX_BUGS
        if ((high > 0.005f && low > 0.005f)
            || (high < -0.005f && low < -0.005f)
            || (face->material != NULL && (face->material->identifier[0] == '!' || face->material->identifier[0] == '>'))) {
#else
        if ((high > 0.005f && low > 0.005f)
            || (high < -0.005f && low < -0.005f)
            || (face->material != NULL && face->material->identifier[0] == '!')
            || face->material->identifier[0] == '>') {
#endif
            face->flags |= 0x80;
            FaceOffCount();
        } else {
            for (axis = 0; axis < 3; axis++) {
                offset = BrVector3Dot((br_vector3*)mat->m[3], (br_vector3*)mat->m[axis]);
                d = BrVector3Dot(&face->v[0], (br_vector3*)mat->m[axis]);
                limit = bounds->min.v[axis] - 0.005f + offset;
                if (d >= limit) {
                    limit = bounds->max.v[axis] + offset + 0.005f;
                    if (d > limit
                        && BrVector3Dot(&face->v[1], (br_vector3*)mat->m[axis]) > limit
                        && BrVector3Dot(&face->v[2], (br_vector3*)mat->m[axis]) > limit) {
                        face->flags |= 0x80;
                        FaceOffCount2();
                    }
                } else if (BrVector3Dot(&face->v[1], (br_vector3*)mat->m[axis]) < limit
                    && BrVector3Dot(&face->v[2], (br_vector3*)mat->m[axis]) < limit) {
                    face->flags |= 0x80;
                    FaceOffCount2();
                }
            }
            FaceOnCount();
            count++;
            face->flags &= ~0x80;
        }
    }
    return count;
}

// GetShapePoint

// GetObjectObjectEdgeIntersections

// FUNCTION: CARMA2_HW 0x004236a0
// FUNCTION: CARMA2_SW 0x00423720
// FUNCTION: CARMA2_CHEAT 0x00423820
int C2_HOOK_FASTCALL GetFaceEdgeIntersection(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, int pMax_count, tPhysics_wall_workspace* pWorkspace, void* pUnused, int* pTouched) {
    br_vector3 centre;
    br_vector3 direction;
    br_vector3 reverse;
    br_vector3 normal;
    br_vector3 other_normal;
    br_vector3 intersection;
    br_vector3 other_intersection;
    br_vector3 tv;
    br_vector3 local_position;
    br_vector3 old_position;
    br_vector3* start;
    br_vector3* end;
    tFace_ref* face;
    tFace_ref* other_face;
    br_scalar t;
    br_scalar other_t;
    br_scalar dot;
    br_scalar other_dot;
    br_scalar depth;
    int start_index;
    int end_index;
    int found;
    int i;

    if (pShape->common.type != kCollisionShapeType_Sphere) {
        BrMatrix34ApplyP(&centre, &pObject->cmpos, &pObject->transform_matrix);
        for (i = 0; i < pWorkspace->count_edges; i++) {
            start_index = pWorkspace->edges[i].index1;
            end_index = pWorkspace->edges[i].index2;
            start = &pWorkspace->old_points[start_index];
            end = &pWorkspace->old_points[end_index];
            BrVector3Sub(&direction, end, start);
            face = RayCastInBox(start, &direction, &normal, &t, &intersection, pObject);
            BrVector3Sub(&reverse, start, end);
            found = 0;
            if (face == NULL) {
                face = RayCastInBox(end, &reverse, &normal, &t, &intersection, pObject);
                if (face != NULL) {
                    BrVector3Sub(&tv, &centre, &intersection);
                    BrVector3Normalise(&tv, &tv);
                    if (BrVector3Dot(&tv, &normal) >= 0.01f) {
                        pWorkspace->point_flags[start_index] = 1;
                        found = 1;
                    }
                }
            } else {
                BrVector3Sub(&tv, &centre, &intersection);
                BrVector3Normalise(&tv, &tv);
                dot = BrVector3Dot(&tv, &normal);
                other_face = RayCastInBox(end, &reverse, &other_normal, &other_t, &other_intersection, pObject);
                if (other_face == NULL) {
                    if (dot > 0.01f) {
                        pWorkspace->point_flags[end_index] = 1;
                        found = 1;
                    }
                } else {
                    BrVector3Sub(&tv, &centre, &other_intersection);
                    BrVector3Normalise(&tv, &tv);
                    other_dot = BrVector3Dot(&tv, &other_normal);
                    if (dot >= 0.05f || other_dot >= 0.05f) {
                        if (other_dot <= dot) {
                            pWorkspace->point_flags[end_index] = 1;
                        } else {
                            BrVector3Copy(&intersection, &other_intersection);
                            BrVector3Copy(&normal, &other_normal);
                            pWorkspace->point_flags[start_index] = 1;
                        }
                        found = 1;
                    }
                }
            }
            if (found && face != NULL) {
                DrMatrix34ApplyLPInverse(&local_position, &intersection, &pObject->transform_matrix);
                BrMatrix34ApplyP(&old_position, &local_position, &pObject->actor->t.t.mat);
                BrVector3Sub(&tv, &intersection, &old_position);
                depth = BrVector3Dot(&tv, &normal);
                if (depth > 0.0f) {
                    BrMatrix34TApplyV(&other_normal, &normal, &pObject->transform_matrix);
                    pCount = AddObjectWallForce(pForces, &local_position, 0.0f, &other_normal, depth, pCount, pObject, 1, face, NULL);
                }
                *pTouched = 1;
            }
        }
    }
    return pCount;
}

inline int C2_HOOK_FASTCALL GetSphereToFacePoints(tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_joint_force* pForces, int pCount, int pMax_count, br_vector3* pSphere_start, br_vector3* pSphere_end, br_matrix34* pMatrix) {
    tFace_ref* face;
    tFace_ref* best_face;
    br_vector3 point;
    br_vector3 normal;
    br_vector3 best_normal;
    br_vector3 old_normal;
    br_scalar t;
    br_scalar best_t;
    int i;

    best_t = 2.0f;
    for (i = pObject->box_face_start, face = &gFace_list__car[i]; i < pObject->box_face_end; i++, face++) {
        SphereFaceCollide(face, pSphere_start, pSphere_end, pSphere->radius, &point, &normal, &t);
        if (t < best_t) {
            best_t = t;
            best_face = face;
            BrVector3Copy(&best_normal, &normal);
        }
    }
    if (best_t <= 1.0f) {
        BrMatrix34TApplyV(&normal, &best_normal, &pObject->transform_matrix);
        BrMatrix34TApplyV(&old_normal, &best_normal, &pObject->actor->t.t.mat);
        BrVector3Scale(&point, &old_normal, -pSphere->radius);
        BrVector3Accumulate(&point, &pSphere->center);
        pCount = AddObjectWallForce(pForces, &point, best_t, &normal, 0.0f, pCount, pObject, 3, best_face, NULL);
    }
    return pCount;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00423ce0
// FUNCTION: CARMA2_SW 0x00423d60
// FUNCTION: CARMA2_CHEAT 0x00423e60
int C2_HOOK_FASTCALL CheckSingleEdgeWithSphere(br_vector3* pEdge_start, br_vector3* pEdge_end, tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_joint_force* pForces, int pCount, br_vector3* pSphere_start, br_vector3* pSphere_end, tFace_ref* pFace) {
    br_vector3 point;
    br_vector3 normal;
    br_vector3 trial_normal;
    br_vector3 old_normal;
    br_vector3 direction;
    br_vector3 surface_point;
    br_scalar t;

    SphereEdgeCollide(pEdge_start, pEdge_end, pSphere_start, pSphere_end, pSphere, &point, &normal, &t, 0, 0);
    if (t <= 1.0f) {
        BrMatrix34TApplyV(&trial_normal, &normal, &pObject->transform_matrix);
        BrMatrix34TApplyV(&old_normal, &normal, &pObject->actor->t.t.mat);
        BrVector3Scale(&old_normal, &old_normal, t);
        BrVector3Scale(&direction, &trial_normal, 1.0f - t);
        BrVector3Accumulate(&direction, &old_normal);
        BrVector3Normalise(&direction, &direction);
        BrVector3Scale(&direction, &direction, pSphere->radius);
        BrVector3Add(&surface_point, &pSphere->center, &direction);
        BrVector3Negate(&trial_normal, &trial_normal);
        return AddObjectWallForce(pForces, &surface_point, t, &trial_normal, 0.0f, pCount, pObject, 4, pFace, NULL);
    }
    return pCount;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00423c00
// FUNCTION: CARMA2_SW 0x00423c80
// FUNCTION: CARMA2_CHEAT 0x00423d80
int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_joint_force* pForces, int pCount, int pMax_count, br_vector3* pSphere_start, br_vector3* pSphere_end, br_matrix34* pMatrix) {
    int i;
    tFace_ref* face;

    i = pObject->box_face_start;
    face = &gFace_list__car[i];
    for (; i < pObject->box_face_end; i++, face++) {
        if (!(face->flags & 0x80)) {
            if (!(face->flags & 0x1)) {
                pCount = CheckSingleEdgeWithSphere(&face->v[0], &face->v[1], pObject, pSphere, pForces, pCount, pSphere_start, pSphere_end, face);
            }
            if (!(face->flags & 0x2)) {
                pCount = CheckSingleEdgeWithSphere(&face->v[1], &face->v[2], pObject, pSphere, pForces, pCount, pSphere_start, pSphere_end, face);
            }
            if (!(face->flags & 0x4)) {
                pCount = CheckSingleEdgeWithSphere(&face->v[2], &face->v[0], pObject, pSphere, pForces, pCount, pSphere_start, pSphere_end, face);
            }
        }
    }
    return pCount;
}

// FUNCTION: CARMA2_HW 0x00423ed0
// FUNCTION: CARMA2_SW 0x00423f50
// FUNCTION: CARMA2_CHEAT 0x00424050
int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, int pMax_count, tPhysics_wall_workspace* pWorkspace, void* pUnused) {
    br_vector3 direction;
    br_vector3 extension;
    br_vector3 normal;
    br_vector3 local_normal;
    br_vector3 intersection;
    br_vector3 local_position;
    br_scalar t;
    tFace_ref* face;
    int i;

    if (pMax_count < 4) {
        return 0;
    }
    gINT_00679434 = 0;
    for (i = 0; i < pWorkspace->count_points; i++) {
        if (pWorkspace->point_flags[i] == 0) {
            BrVector3Sub(&direction, &pWorkspace->trial_points[i], &pWorkspace->old_points[i]);
            BrVector3Normalise(&extension, &direction);
            BrVector3Scale(&extension, &extension, 0.0010434783f);
            BrVector3Accumulate(&direction, &extension);
            face = RayCastThruFaceList(&pWorkspace->old_points[i], &direction, &normal, &t, &intersection, &gFace_list__car[pObject->box_face_start], pObject->box_face_end - pObject->box_face_start);
            if (face != NULL) {
                BrMatrix34TApplyV(&local_normal, &normal, &pObject->transform_matrix);
                BrVector3Sub(&extension, &pWorkspace->old_points[i], (br_vector3*)pObject->transform_matrix.m[3]);
                BrMatrix34TApplyV(&local_position, &extension, &pObject->transform_matrix);
                pCount = AddObjectWallForce(pForces, &local_position, t, &local_normal, BrVector3Dot(&normal, &direction), pCount, pObject, 1, face, NULL);
            }
        }
    }
    if (pCount > 3) {
        pCount = 4;
    }
    return pCount;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

/* FIXME: pCount is never read; its type is a guess. */
// FUNCTION: CARMA2_HW 0x00425630
// FUNCTION: CARMA2_SW 0x004256d0
// FUNCTION: CARMA2_CHEAT 0x004257b0
int C2_HOOK_FASTCALL CylinderCheck(const br_vector3* pN0, const br_vector3* pN1, const br_vector3* pEdge_start, const br_vector3* pEdge_direction, const br_vector3* pOther_start, const br_vector3* pOther_direction, const br_vector3* pOther_velocity, const br_vector3* pOther_direction_change, tPhysics_joint_force* pForce, int pCount, tPhysics_object* pObject, tPhysics_object* pOther) {
    br_vector3 normal;
    br_vector3 delta;
    br_vector3 speed;
    br_vector3 cross;
    br_vector3 point;
    br_vector3 world;
    br_scalar length;
    br_scalar denominator;
    br_scalar t;
    br_scalar s;
    br_scalar distance;

    if (pForce->field_0x0 <= 1.0) {
        return 0;
    }
    BrVector3Add(&normal, pN0, pN1);
    length = BrVector3Length(&normal);
    if (length <= 1e-7) {
        return 0;
    }
    BrVector3Add(&delta, pOther_start, pOther_velocity);
    BrVector3Sub(&delta, &delta, pEdge_start);
    if (fabs(BrVector3Dot(&delta, &normal) / length) >= 0.002) {
        return 0;
    }
    normal.v[0] /= length;
    normal.v[1] /= length;
    normal.v[2] /= length;
    BrVector3Add(&speed, pOther_direction, pOther_direction_change);
    BrVector3Cross(&cross, pEdge_direction, &normal);
    denominator = BrVector3Dot(&cross, &speed);
    if (fabs(denominator) <= 1e-7) {
        return 0;
    }
    t = -(BrVector3Dot(&cross, &delta) / denominator);
    if (t < 0.0f || t > 1.0f) {
        return 0;
    }
    delta.v[0] += speed.v[0] * t;
    delta.v[1] += speed.v[1] * t;
    delta.v[2] += speed.v[2] * t;
    s = BrVector3Dot(pEdge_direction, &delta) / BrVector3LengthSquared(pEdge_direction);
    if (s < 0.0f || s > 1.0f) {
        return 0;
    }
    pForce->field_0x0 = 1.0f;
    BrVector3Sub(&delta, pOther_start, pEdge_start);
    if (BrVector3Dot(&delta, pN0) < 0.0f) {
        BrVector3Negate(&normal, &normal);
    }
    BrVector3Add(&delta, pOther_start, pOther_velocity);
    BrVector3Sub(&delta, &delta, pEdge_start);
    distance = BrVector3Dot(&delta, &normal);
    if (distance < 0.0f) {
        BrVector3Negate(&normal, &normal);
    }
    if (pOther != NULL) {
        BrVector3Scale(&world, pOther_direction, t);
        BrVector3Add(&point, pOther_start, &world);
        BrMatrix34ApplyP(&world, &point, &pOther->transform_matrix);
        DrMatrix34ApplyLPInverse(&pForce->point_a, &world, &pObject->transform_matrix);
        BrMatrix34ApplyV(&pForce->direction_a, &normal, &pOther->transform_matrix);
    } else {
        if (fabs(distance) < 0.001) {
            pForce->field_0x0 = 1.1f;
        }
        BrVector3Scale(&world, pOther_direction, t);
        BrVector3Add(&point, pOther_start, &world);
        DrMatrix34ApplyLPInverse(&pForce->point_a, &point, &pObject->transform_matrix);
        BrMatrix34TApplyV(&pForce->direction_a, &normal, &pObject->transform_matrix);
        BrVector3Copy(&pForce->direction_b, &normal);
    }
    BrVector3Scale(&pForce->point_b, pEdge_direction, s);
    BrVector3Accumulate(&pForce->point_b, pEdge_start);
    return 1;
}

/* FIXME: pCount is never read; its type is a guess. */
// FUNCTION: CARMA2_HW 0x00424990
// FUNCTION: CARMA2_SW 0x00424a10
// FUNCTION: CARMA2_CHEAT 0x00424b10
int C2_HOOK_FASTCALL CheckTwoEdges(const br_vector3* pEdge_start, const br_vector3* pEdge_direction, const br_vector3* pOther_start, const br_vector3* pOther_direction, const br_vector3* pOther_velocity, const br_vector3* pOther_direction_change, tPhysics_joint_force* pForce, int pCount, tPhysics_object* pObject, tPhysics_object* pOther) {
    br_vector3 n0;
    br_vector3 n1;
    br_vector3 delta;
    br_vector3 normal;
    br_vector3 edge;
    br_vector3 origin;
    br_vector3 projection;
    br_vector3 point;
    br_vector3 world;
    br_vector3 other_normal;
    br_scalar a;
    br_scalar b;
    br_scalar c;
    br_scalar ac4;
    br_scalar b2;
    br_scalar root;
    br_scalar two_a;
    br_scalar t;
    br_scalar dot;
    br_scalar length_squared;
    br_scalar s;
    br_scalar u;
    int axis;

    BrVector3Cross(&n0, pOther_direction, pEdge_direction);
    BrVector3Cross(&n1, pOther_direction_change, pEdge_direction);
    BrVector3Sub(&delta, pOther_start, pEdge_start);
    a = BrVector3Dot(pOther_velocity, &n1);
    b = BrVector3Dot(pOther_start, &n1) + BrVector3Dot(pOther_velocity, &n0)
        - (BrVector3Dot(pEdge_start, &n1) + BrVector3Dot(pEdge_direction, &n0));
    c = BrVector3Dot(&delta, &n0);
    if (a < 0.0f) {
        a = -a;
        b = -b;
        c = -c;
    }
    ac4 = c * a * 4.0f;
    b2 = b * b;
    if (b2 < fabs(ac4) * 1000.0f) {
        root = b2 - ac4;
        if (root < 0.0f) {
            return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
        }
        root = (br_scalar)sqrt(root);
        two_a = a + a;
        t = (-b - root) / two_a;
        if (t > 1.0f) {
            return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
        }
        if (t >= 0.0f) {
            BrVector3Scale(&normal, &n1, t);
            BrVector3Accumulate(&normal, &n0);
            if (BrVector3LengthSquared(&normal) < 1e-7) {
                return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
            }
        }
        if (t < 0.0f) {
            t = (root - b) / two_a;
        }
    } else {
        if (fabs(b) <= 1e-7) {
            return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
        }
        t = -(c / b) - c * c * a / (b2 * b) * (-4.0f / 3.0f);
        if (t < 0.0f || t > 1.0f || gINT_00679438 != 0) {
            t = -(b / a) - t;
        }
    }
    if (t < 0.0f || t > 1.0f) {
        return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
    }
    BrVector3Scale(&normal, &n1, t);
    BrVector3Accumulate(&normal, &n0);
    if (BrVector3LengthSquared(&normal) < 1e-7) {
        return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
    }
    if (t >= pForce->field_0x0) {
        return 0;
    }
    BrVector3Normalise(&normal, &normal);
    BrVector3Scale(&edge, pOther_direction_change, t);
    BrVector3Accumulate(&edge, pOther_direction);
    BrVector3Scale(&origin, pOther_velocity, t);
    BrVector3Accumulate(&origin, pOther_start);
    dot = BrVector3Dot(pEdge_direction, &edge);
    length_squared = BrVector3LengthSquared(pEdge_direction);
    projection.v[0] = pEdge_direction->v[0] * dot - edge.v[0] * length_squared;
    projection.v[1] = pEdge_direction->v[1] * dot - edge.v[1] * length_squared;
    projection.v[2] = pEdge_direction->v[2] * dot - edge.v[2] * length_squared;
    BrVector3Sub(&delta, pEdge_start, &origin);
    s = BrVector3Dot(&delta, &projection) / BrVector3Dot(&edge, &projection);
    if (s < 0.0f || s > 1.0f) {
        return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
    }
    if (fabs(pEdge_direction->v[0]) >= fabs(pEdge_direction->v[1])) {
        axis = 0;
    } else {
        axis = 1;
    }
    if (fabs(pEdge_direction->v[2]) > fabs(pEdge_direction->v[axis])) {
        axis = 2;
    }
    u = (edge.v[axis] * s + origin.v[axis] - pEdge_start->v[axis]) / pEdge_direction->v[axis];
    if (u < 0.0f || u > 1.0f) {
        return CylinderCheck(&n0, &n1, pEdge_start, pEdge_direction, pOther_start, pOther_direction, pOther_velocity, pOther_direction_change, pForce, pCount, pObject, pOther);
    }
    BrVector3Sub(&delta, pOther_start, pEdge_start);
    if (BrVector3Dot(&delta, &n0) < 0.0f) {
        BrVector3Negate(&normal, &normal);
    }
    if (pOther != NULL) {
        BrMatrix34ApplyV(&pForce->direction_a, &normal, &pOther->transform_matrix);
        BrMatrix34ApplyV(&other_normal, &normal, &pOther->actor->t.t.mat);
        BrVector3Scale(&pForce->direction_a, &pForce->direction_a, t);
        BrVector3Scale(&other_normal, &other_normal, 1.0f - t);
        BrVector3Accumulate(&pForce->direction_a, &other_normal);
        BrVector3Normalise(&pForce->direction_a, &pForce->direction_a);
        BrVector3Scale(&pForce->point_b, pEdge_direction, u);
        BrVector3Accumulate(&pForce->point_b, pEdge_start);
        BrVector3Scale(&point, pOther_direction, s);
        BrVector3Accumulate(&point, pOther_start);
        BrMatrix34ApplyP(&world, &point, &pOther->transform_matrix);
        DrMatrix34ApplyLPInverse(&pForce->point_a, &world, &pObject->transform_matrix);
    } else {
        BrMatrix34TApplyV(&pForce->direction_a, &normal, &pObject->transform_matrix);
        BrVector3Copy(&pForce->direction_b, &normal);
        BrVector3Scale(&point, pOther_direction, s);
        BrVector3Accumulate(&point, pOther_start);
        DrMatrix34ApplyLPInverse(&pForce->point_a, &point, &pObject->transform_matrix);
        BrVector3Scale(&pForce->point_b, pEdge_direction, u);
        BrVector3Accumulate(&pForce->point_b, pEdge_start);
    }
    pForce->field_0x0 = t;
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline int C2_HOOK_FASTCALL WorldPointCollision(const br_vector3* pPoint, tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, tFace_ref* pFace) {
    br_vector3 start;
    br_vector3 end;
    br_vector3 point;
    br_vector3 normal;
    br_scalar t;
    int result;

    DrMatrix34ApplyLPInverse(&start, pPoint, &pObject->transform_matrix);
    DrMatrix34ApplyLPInverse(&end, pPoint, &pObject->actor->t.t.mat);
    result = ShapeRayCastWithSphere(&start, &end, pShape, &point, &normal, &t);
    if (result > 0) {
        if (result > 64) {
            t = 1.1f;
        }
        pCount = AddObjectWallForce(pForces, &point, t, &normal, 0.0f, pCount, pObject, 2, pFace, NULL);
    }
    return pCount;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00424270
// FUNCTION: CARMA2_SW 0x004242f0
// FUNCTION: CARMA2_CHEAT 0x004243f0
int C2_HOOK_FASTCALL CheckSingleEdgeWithObjectEdges(br_vector3* pEdge_start, br_vector3* pEdge_end, tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, tFace_ref* pFace, tPhysics_wall_workspace* pWorkspace) {
    tPhysics_joint_force candidates[2];
    br_vector3 direction;
    br_vector3 velocity;
    br_vector3 edge_direction;
    br_vector3 change;
    br_vector3 start;
    br_vector3 end;
    br_vector3 point;
    br_vector3* first;
    br_vector3* second;
    br_vector3* trial_first;
    br_vector3* trial_second;
    br_matrix34* mat;
    br_scalar projected_start;
    br_scalar projected_end;
    br_scalar low;
    br_scalar high;
    br_scalar t;
    int check_start;
    int check_end;
    int i;
    int selector;
    int best_edge;

    check_start = 1;
    check_end = 1;
    mat = &pObject->actor->t.t.mat;
    for (i = 0; i < 3; i++) {
        projected_start = BrVector3Dot((br_vector3*)mat->m[i], pEdge_start);
        projected_end = BrVector3Dot((br_vector3*)mat->m[i], pEdge_end);
        low = (br_scalar)(pShape->common.field_0x1c.min.v[i] - 0.005);
        check_start |= projected_start < low;
        check_end |= projected_end < low;
        if (projected_start < low && projected_end < low) {
            return pCount;
        }
        high = (br_scalar)(pShape->common.field_0x1c.max.v[i] + 0.005);
        check_start |= projected_start < high;
        check_end |= projected_end < high;
        if (projected_start > high && projected_end > high) {
            return pCount;
        }
    }
    if (pObject->field_0xed == 0) {
        if (check_start) {
            pCount = WorldPointCollision(pEdge_start, pObject, pShape, pForces, pCount, pFace);
        }
        if (check_end) {
            pCount = WorldPointCollision(pEdge_end, pObject, pShape, pForces, pCount, pFace);
        }
    }
    BrVector3Sub(&direction, pEdge_end, pEdge_start);
    selector = 0;
    candidates[0].field_0xc = pFace;
    candidates[1].field_0xc = pFace;
    candidates[0].field_0x0 = 2.0f;
    candidates[1].field_0x0 = 2.0f;
    best_edge = -1;
    for (i = 0; i < pWorkspace->count_edges; i++) {
        first = &pWorkspace->old_points[pWorkspace->edges[i].index1];
        second = &pWorkspace->old_points[pWorkspace->edges[i].index2];
        trial_first = &pWorkspace->trial_points[pWorkspace->edges[i].index1];
        trial_second = &pWorkspace->trial_points[pWorkspace->edges[i].index2];
        BrVector3Sub(&velocity, trial_first, first);
        BrVector3Sub(&edge_direction, second, first);
        BrVector3Sub(&change, trial_second, trial_first);
        BrVector3Sub(&change, &change, &edge_direction);
        if (CheckTwoEdges(pEdge_start, &direction, first, &edge_direction, &velocity, &change, &candidates[selector], pCount, pObject, NULL) != 0) {
            best_edge = i;
            if (candidates[selector].field_0x0 < candidates[!selector].field_0x0) {
                selector = !selector;
            }
        }
    }
    if (candidates[0].field_0x0 >= 2.0f) {
        return pCount;
    }
    if (BrVector3Dot(&pFace->normal, &candidates[0].direction_b) < 0.0f) {
        DrMatrix34ApplyLPInverse(&start, pEdge_start, &pObject->transform_matrix);
        DrMatrix34ApplyLPInverse(&end, pEdge_end, &pObject->transform_matrix);
        if (ShapeRayCast(&start, &end, pShape, &point, &t, NULL) != 0) {
            return pCount;
        }
    }
    gUINT_00679430 |= 1 << best_edge;
    pCount = AddObjectWallForce(pForces, &candidates[0].point_a, candidates[0].field_0x0, &candidates[0].direction_a, 0.0f, pCount, pObject, 0, pFace, &candidates[0].point_b);
    if (candidates[1].field_0x0 < 2.0f) {
        pCount = AddObjectWallForce(pForces, &candidates[1].point_a, candidates[1].field_0x0, &candidates[1].direction_a, 0.0f, pCount, pObject, 0, pFace, &candidates[1].point_b);
    }
    return pCount;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

/* FIXME: pMax_count and pUnused are never read; their types are guesses. */
// FUNCTION: CARMA2_HW 0x00424140
// FUNCTION: CARMA2_SW 0x004241c0
// FUNCTION: CARMA2_CHEAT 0x004242c0
int C2_HOOK_FASTCALL GetEdgeToEdgePoints(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, int pMax_count, tPhysics_wall_workspace* pWorkspace, void* pUnused) {
    int i;
    tFace_ref* face;
    br_vector3 tv;

    gUINT_00679430 = 0;
    BrMatrix34TApplyV(&tv, &pObject->actor->t.t.translate.t, &pObject->actor->t.t.mat);
    BrVector3Accumulate(&pShape->common.field_0x1c.min, &tv);
    BrVector3Accumulate(&pShape->common.field_0x1c.max, &tv);
    i = pObject->box_face_start;
    face = &gFace_list__car[i];
    for (; i < pObject->box_face_end; i++, face++) {
        if (!(face->flags & 0x80)) {
            if (!(face->flags & 0x1)) {
                pCount = CheckSingleEdgeWithObjectEdges(&face->v[0], &face->v[1], pObject, pShape, pForces, pCount, face, pWorkspace);
            }
            if (!(face->flags & 0x2)) {
                pCount = CheckSingleEdgeWithObjectEdges(&face->v[1], &face->v[2], pObject, pShape, pForces, pCount, face, pWorkspace);
            }
            if (!(face->flags & 0x4)) {
                pCount = CheckSingleEdgeWithObjectEdges(&face->v[2], &face->v[0], pObject, pShape, pForces, pCount, face, pWorkspace);
            }
        }
    }
    return pCount;
}

inline void C2_HOOK_FASTCALL SetUpWorkspace(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_wall_workspace* pWorkspace) {
    br_matrix34* trial_matrix;
    br_vector3 corner;
    int i;

    if (gINT_00679438 != 0) {
        trial_matrix = &pObject->transform_matrix;
    } else {
        trial_matrix = &pObject->actor->t.t.mat;
    }
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        pWorkspace->count_points = 8;
        pWorkspace->count_edges = 12;
        for (i = 0; i < 8; i++) {
            if (i & 0x2) {
                corner.v[0] = pShape->common.bb.min.v[0];
            } else {
                corner.v[0] = pShape->common.bb.max.v[0];
            }
            if (i & 0x1) {
                corner.v[1] = pShape->common.bb.min.v[1];
            } else {
                corner.v[1] = pShape->common.bb.max.v[1];
            }
            if (i & 0x4) {
                corner.v[2] = pShape->common.bb.min.v[2];
            } else {
                corner.v[2] = pShape->common.bb.max.v[2];
            }
            BrMatrix34ApplyP(&pWorkspace->old_points[i], &corner, &pObject->transform_matrix);
            BrMatrix34ApplyP(&pWorkspace->trial_points[i], &corner, trial_matrix);
            pWorkspace->point_flags[i] = 0;
        }
        pWorkspace->edges = gDefault_box_collision_wireframe_edges;
        break;
    case kCollisionShapeType_Wireframe:
        pWorkspace->count_points = pShape->wireframe.wireframe.count_points;
        pWorkspace->count_edges = pShape->wireframe.wireframe.count_lines;
        for (i = 0; i < pWorkspace->count_points; i++) {
            BrMatrix34ApplyP(&pWorkspace->old_points[i], &pShape->wireframe.wireframe.points[i], &pObject->transform_matrix);
            BrMatrix34ApplyP(&pWorkspace->trial_points[i], &pShape->wireframe.wireframe.points[i], trial_matrix);
            pWorkspace->point_flags[i] = 0;
        }
        pWorkspace->edges = pShape->wireframe.wireframe.lines;
        break;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        pWorkspace->count_points = pShape->polyhedron.polyhedron.count_points;
        pWorkspace->count_edges = pShape->polyhedron.polyhedron.count_edges;
        for (i = 0; i < pWorkspace->count_points; i++) {
            BrMatrix34ApplyP(&pWorkspace->old_points[i], &pShape->polyhedron.polyhedron.points[i], &pObject->transform_matrix);
            BrMatrix34ApplyP(&pWorkspace->trial_points[i], &pShape->polyhedron.polyhedron.points[i], trial_matrix);
            pWorkspace->point_flags[i] = 0;
        }
        pWorkspace->edges = pShape->polyhedron.polyhedron.edges;
        break;
    case kCollisionShapeType_Sphere:
        pWorkspace->count_points = 1;
        pWorkspace->count_edges = 0;
        BrMatrix34ApplyP(&pWorkspace->old_points[0], &pShape->sphere.sphere.center, &pObject->transform_matrix);
        BrMatrix34ApplyP(&pWorkspace->trial_points[0], &pShape->sphere.sphere.center, trial_matrix);
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, "");
        break;
    }
    if (gINT_00679438 != 0) {
        BrMatrix34ApplyP(&pWorkspace->old_points[0], &pObject->cmpos, &pObject->transform_matrix);
        for (i = 1; i < pWorkspace->count_points; i++) {
            BrVector3Copy(&pWorkspace->old_points[i], &pWorkspace->old_points[0]);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004228b0
// FUNCTION: CARMA2_SW 0x00422930
// FUNCTION: CARMA2_CHEAT 0x00422a30
int C2_HOOK_FASTCALL GetObjectWallForceInfo(tPhysics_object* pObject, tPhysics_joint_force* pForces, int pCount) {
    tPhysics_wall_workspace workspace;
    br_matrix34 inverse;
    br_matrix34 delta;
    tPhysics_shape* shape;
    int count;
    int sphere_count;
    int touched;
    int touched_all;
    int i;

    count = 0;
    touched_all = 0;
    shape = pObject->shape;
    i = 1;
    if (pObject->box_face_start == pObject->box_face_end) {
        return 0;
    }
    if (pCount > 36) {
        return 0;
    }
    BrMatrix34LPInverse(&inverse, &pObject->actor->t.t.mat);
    BrMatrix34Mul(&delta, &pObject->transform_matrix, &inverse);
    for (; shape != NULL; shape = shape->common.next) {
        if (EliminateFaces(pObject, shape, &delta) != 0) {
            SetUpWorkspace(pObject, shape, &workspace);
            if (shape->common.type == kCollisionShapeType_Sphere) {
                sphere_count = GetSphereToFacePoints(pObject, &shape->sphere.sphere, &pForces[pCount], count, 40 - pCount, workspace.old_points, workspace.trial_points, &delta);
                if (GetSphereToEdgePoints(pObject, &shape->sphere.sphere, &pForces[pCount], sphere_count, 40 - pCount, workspace.old_points, workspace.trial_points, &delta) > count) {
                    count++;
                }
            } else {
                touched = 0;
                if (pObject->field_0xed != 0) {
                    count = GetFaceEdgeIntersection(pObject, shape, &pForces[pCount], count, 40 - pCount, &workspace, &delta, &touched);
                }
                touched_all |= touched;
                count = GetCornerToFacePoints(pObject, shape, &pForces[pCount], count, 40 - pCount, &workspace, &delta);
                if (touched == 0) {
                    count = GetEdgeToEdgePoints(pObject, shape, &pForces[pCount], count, 40 - pCount, &workspace, &delta);
                }
            }
        }
    }
    pObject->field_0xed = touched_all;
    if (count == 0) {
        return 0;
    }
    for (; i < count; i++) {
        if (BrVector3Dot(&pForces[pCount].direction_a, &pForces[pCount + i].direction_a) <= 0.95) {
            break;
        }
    }
    BrVector3Copy(&pObject->omega, &pObject->rotate_omega);
    return i;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetObjectCornerToObjectFacePoints

static int C2_HOOK_FASTCALL TestForNan_physics(const float* f) {

    /* FIXME: copy of TestForNan for inlining in BoundsOverlapTest.
     **/
    if (f != NULL) {
        return ((~*(tU32*)f) & 0x7f800000) == 0;
    } else {
        return 0;
    }
}
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00425ba0
// FUNCTION: CARMA2_SW 0x00425c40
// FUNCTION: CARMA2_CHEAT 0x00425d30
int C2_HOOK_FASTCALL BoundsOverlapTest(const br_bounds3* pBounds_1, const br_bounds3* pBounds_2) {
    int i;

    if (pBounds_1->min.v[2] > pBounds_2->max.v[2]
            || pBounds_2->min.v[2] > pBounds_1->max.v[2]
            || pBounds_1->min.v[1] > pBounds_2->max.v[1]
            || pBounds_2->min.v[1] > pBounds_1->max.v[1]
            || pBounds_1->min.v[0] > pBounds_2->max.v[0]
            || pBounds_2->min.v[0] > pBounds_1->max.v[0]) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (TestForNan_physics(&pBounds_1->min.v[i])
                || TestForNan_physics(&pBounds_1->max.v[i])
                || TestForNan_physics(&pBounds_2->min.v[i])
                || TestForNan_physics(&pBounds_2->max.v[i])) {
            return 0;
        }
    }
    return 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetObjectObjectEdgePoints

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004269c0
// STUB: CARMA2_SW 0x00426a60
// STUB: CARMA2_CHEAT 0x00426b50
int C2_HOOK_FASTCALL GetSphereNonSphereForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline int C2_HOOK_FASTCALL GetSphereSphereForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count) {
    br_vector3 normal;
    br_vector3 world_normal;
    br_vector3 new_normal;
    br_vector3 old_normal;
    br_vector3 point_a;
    br_vector3 point_b;
    br_scalar t;

    SphereSphereCollide(&pShape_a->sphere.sphere, &pShape_b->sphere.sphere, &pPoints_b[0], &pPoints_b[1], &normal, &t);
    if (t <= 1.0f) {
        BrVector3Scale(&point_a, &normal, -pShape_a->sphere.sphere.radius);
        BrVector3Accumulate(&point_a, &pShape_a->sphere.sphere.center);
        BrMatrix34ApplyV(&new_normal, &normal, &pObject_a->transform_matrix);
        BrMatrix34ApplyV(&old_normal, &normal, &pObject_a->actor->t.t.mat);
        BrVector3Scale(&new_normal, &new_normal, 1.0f - t);
        BrVector3Scale(&old_normal, &old_normal, t);
        BrVector3Add(&world_normal, &new_normal, &old_normal);
        BrMatrix34TApplyV(&new_normal, &world_normal, &pObject_b->transform_matrix);
        BrMatrix34TApplyV(&old_normal, &world_normal, &pObject_b->actor->t.t.mat);
        BrVector3Scale(&new_normal, &new_normal, 1.0f - t);
        BrVector3Scale(&old_normal, &old_normal, t);
        BrVector3Accumulate(&new_normal, &old_normal);
        BrVector3Scale(&point_b, &new_normal, pShape_b->sphere.sphere.radius);
        BrVector3Accumulate(&point_b, &pShape_b->sphere.sphere.center);
        pCount = AddObjectObjectForce(pForces, &point_a, t, &point_b, &world_normal, pCount, pObject_a, pObject_b, 7, NULL, 0.0f);
    }
    return pCount;
}

inline int C2_HOOK_FASTCALL GetSphereShapeForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count) {

    if (pShape_b->common.type == kCollisionShapeType_Sphere) {
        return GetSphereSphereForceInfo(pObject_a, pObject_b, pShape_a, pShape_b, pPoints_a, pPoints_b, pCount_a, pCount_b, pForces, pCount, pMax_count);
    } else {
        return GetSphereNonSphereForceInfo(pObject_a, pObject_b, pShape_a, pShape_b, pPoints_a, pPoints_b, pCount_a, pCount_b, pForces, pCount, pMax_count);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00427870
// STUB: CARMA2_SW 0x00427900
// STUB: CARMA2_CHEAT 0x00427a00
int C2_HOOK_FASTCALL GetNonSphereNonSphereForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline int C2_HOOK_FASTCALL GetNonSphereShapeForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count) {

    if (pShape_b->common.type == kCollisionShapeType_Sphere) {
        return GetSphereNonSphereForceInfo(pObject_b, pObject_a, pShape_b, pShape_a, pPoints_b, pPoints_a, pCount_b, pCount_a, pForces, pCount, pMax_count);
    } else {
        return GetNonSphereNonSphereForceInfo(pObject_a, pObject_b, pShape_a, pShape_b, pPoints_a, pPoints_b, pCount_a, pCount_b, pForces, pCount, pMax_count);
    }
}

/* FIXME: the sizes of the point and count arrays are guesses. */
// FUNCTION: CARMA2_HW 0x00425ca0
// FUNCTION: CARMA2_SW 0x00425d40
// FUNCTION: CARMA2_CHEAT 0x00425e30
int C2_HOOK_FASTCALL GetObjectObjectForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_joint_force* pForces, int pCount) {
    br_matrix34 new_matrix;
    br_matrix34 old_matrix;
    br_matrix34 inverse_new;
    br_matrix34 inverse_old;
    tU16 counts_b[17];
    tU16 counts_a[17];
    br_vector3 corner;
    br_vector3 points_b[256];
    br_vector3 points_a[256];
    br_vector3* points;
    br_vector3* other_points;
    tPhysics_shape* shape;
    tPhysics_shape* other_shape;
    tPhysics_joint_force* first;
    br_scalar dot;
    int count;
    int i;
    int j;

    count = 0;
    shape = pObject_b->shape;
    other_shape = pObject_a->shape;
    if (pCount > 36) {
        return 0;
    }
    BrMatrix34LPInverse(&inverse_new, &pObject_a->transform_matrix);
    BrMatrix34Mul(&new_matrix, &pObject_b->transform_matrix, &inverse_new);
    BrMatrix34LPInverse(&inverse_old, &pObject_a->actor->t.t.mat);
    BrMatrix34Mul(&old_matrix, &pObject_b->actor->t.t.mat, &inverse_old);
    points = points_b;
    j = 0;
    for (; shape != NULL; shape = shape->common.next) {
        switch (shape->common.type) {
        case kCollisionShapeType_Box:
            counts_b[j] = 8;
                for (i = 0; i < 8; i++) {
                    if (i & 0x2) {
                        corner.v[0] = shape->common.bb.min.v[0];
                    } else {
                        corner.v[0] = shape->common.bb.max.v[0];
                    }
                    if (i & 0x1) {
                        corner.v[1] = shape->common.bb.min.v[1];
                    } else {
                        corner.v[1] = shape->common.bb.max.v[1];
                    }
                    if (i & 0x4) {
                        corner.v[2] = shape->common.bb.min.v[2];
                    } else {
                        corner.v[2] = shape->common.bb.max.v[2];
                    }
                        BrMatrix34ApplyP(&points[2 * i], &corner, &new_matrix);
                        BrMatrix34ApplyP(&points[2 * i + 1], &corner, &old_matrix);
                }
            points += 16;
            break;
        case kCollisionShapeType_Wireframe:
            counts_b[j] = shape->wireframe.wireframe.count_points;
            for (i = 0; i < shape->wireframe.wireframe.count_points; i++) {
                BrMatrix34ApplyP(&points[2 * i], &shape->wireframe.wireframe.points[i], &new_matrix);
                BrMatrix34ApplyP(&points[2 * i + 1], &shape->wireframe.wireframe.points[i], &old_matrix);
            }
            points += 2 * shape->wireframe.wireframe.count_points;
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            counts_b[j] = shape->polyhedron.polyhedron.count_points;
            for (i = 0; i < shape->polyhedron.polyhedron.count_points; i++) {
                BrMatrix34ApplyP(&points[2 * i], &shape->polyhedron.polyhedron.points[i], &new_matrix);
                BrMatrix34ApplyP(&points[2 * i + 1], &shape->polyhedron.polyhedron.points[i], &old_matrix);
            }
            points += 2 * shape->polyhedron.polyhedron.count_points;
            break;
        case kCollisionShapeType_Sphere:
            BrMatrix34ApplyP(&points[0], &shape->sphere.sphere.center, &new_matrix);
            BrMatrix34ApplyP(&points[1], &shape->sphere.sphere.center, &old_matrix);
            points += 2;
            counts_b[j] = 1;
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, "");
            break;
        }
        j++;
    }
    shape = other_shape;
    points = points_a;
    j = 0;
    for (; shape != NULL; shape = shape->common.next) {
        switch (shape->common.type) {
        case kCollisionShapeType_Box:
            counts_a[j] = 8;
                for (i = 0; i < 8; i++) {
                    if (i & 0x2) {
                        corner.v[0] = shape->common.bb.min.v[0];
                    } else {
                        corner.v[0] = shape->common.bb.max.v[0];
                    }
                    if (i & 0x1) {
                        corner.v[1] = shape->common.bb.min.v[1];
                    } else {
                        corner.v[1] = shape->common.bb.max.v[1];
                    }
                    if (i & 0x4) {
                        corner.v[2] = shape->common.bb.min.v[2];
                    } else {
                        corner.v[2] = shape->common.bb.max.v[2];
                    }
                        DrMatrix34ApplyLPInverse(&points[2 * i], &corner, &new_matrix);
                        DrMatrix34ApplyLPInverse(&points[2 * i + 1], &corner, &old_matrix);
                }
            points += 16;
            break;
        case kCollisionShapeType_Wireframe:
            counts_a[j] = shape->wireframe.wireframe.count_points;
            for (i = 0; i < shape->wireframe.wireframe.count_points; i++) {
                DrMatrix34ApplyLPInverse(&points[2 * i], &shape->wireframe.wireframe.points[i], &new_matrix);
                DrMatrix34ApplyLPInverse(&points[2 * i + 1], &shape->wireframe.wireframe.points[i], &old_matrix);
            }
            points += 2 * shape->wireframe.wireframe.count_points;
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            counts_a[j] = shape->polyhedron.polyhedron.count_points;
            for (i = 0; i < shape->polyhedron.polyhedron.count_points; i++) {
                DrMatrix34ApplyLPInverse(&points[2 * i], &shape->polyhedron.polyhedron.points[i], &new_matrix);
                DrMatrix34ApplyLPInverse(&points[2 * i + 1], &shape->polyhedron.polyhedron.points[i], &old_matrix);
            }
            points += 2 * shape->polyhedron.polyhedron.count_points;
            break;
        case kCollisionShapeType_Sphere:
            DrMatrix34ApplyLPInverse(&points[0], &shape->sphere.sphere.center, &new_matrix);
            DrMatrix34ApplyLPInverse(&points[1], &shape->sphere.sphere.center, &old_matrix);
            points += 2;
            counts_a[j] = 1;
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, "");
            break;
        }
        j++;
    }
    points = points_a;
    i = 0;
    for (shape = pObject_a->shape; shape != NULL; shape = shape->common.next) {
        other_points = points_b;
        j = 0;
        if (shape->common.type == kCollisionShapeType_Sphere) {
            for (other_shape = pObject_b->shape; other_shape != NULL; other_shape = other_shape->common.next) {
                count = GetSphereShapeForceInfo(pObject_a, pObject_b, shape, other_shape, points, other_points, counts_a[i], counts_b[j], &pForces[pCount], count, 40 - pCount);
                other_points += 2 * counts_b[j];
                j++;
            }
        } else {
            for (other_shape = pObject_b->shape; other_shape != NULL; other_shape = other_shape->common.next) {
                count = GetNonSphereShapeForceInfo(pObject_a, pObject_b, shape, other_shape, points, other_points, counts_a[i], counts_b[j], &pForces[pCount], count, 40 - pCount);
                other_points += 2 * counts_b[j];
                j++;
            }
        }
        points += 2 * counts_a[i];
        i++;
    }
    if (count == 0) {
        return 0;
    }
    BrVector3Copy(&pObject_a->omega, &pObject_a->rotate_omega);
    BrVector3Copy(&pObject_b->omega, &pObject_b->rotate_omega);
    first = &pForces[pCount];
    for (i = 1; i < count; i++) {
        if (pForces[pCount + i].object_a == first->object_a) {
            dot = BrVector3Dot(&first->direction_a, &pForces[pCount + i].direction_a);
        } else {
            dot = -BrVector3Dot(&first->direction_b, &pForces[pCount + i].direction_a);
        }
        if (dot < 0.707) {
            break;
        }
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x00428480
// FUNCTION: CARMA2_SW 0x00428510
void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex) {
    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        BrVector3Set(pPoint,
            (pIndex & 0x2) ? pShape->common.bb.min.v[0] : pShape->common.bb.max.v[0],
            (pIndex & 0x1) ? pShape->common.bb.min.v[1] : pShape->common.bb.max.v[1],
            (pIndex & 0x4) ? pShape->common.bb.min.v[2] : pShape->common.bb.max.v[2]);
        break;
    case kCollisionShapeType_Wireframe:
        BrVector3Copy(pPoint, &pShape->wireframe.wireframe.points[pIndex]);
        return;
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        BrVector3Copy(pPoint, &pShape->polyhedron.polyhedron.points[pIndex]);
        return;
    default:
        return;
    }
}

// FUNCTION: CARMA2_HW 0x00428cf0
// FUNCTION: CARMA2_SW 0x00428d80
int C2_HOOK_FASTCALL AddOldForces(tPhysics_joint_force* pForces, int pCount, const tPhysics_joint_force* pOld_forces, int pOld_count) {
    int i;

    for (i = 0; i < pOld_count && pCount < 40; i++) {
        if (pOld_forces[i].type != 9) {
            pForces[pCount] = pOld_forces[i];
            pCount++;
        }
    }
    return pCount;
}

// FUNCTION: CARMA2_HW 0x00428d40
// FUNCTION: CARMA2_SW 0x00428dd0
int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;

    shape1 = pCollision_1->shape;
    shape2 = pCollision_2->shape;
    if (FlagsAreGo(pCollision_1, pCollision_2)
        && BoundsOverlapTest(&pCollision_1->field_0xf4, &pCollision_2->field_0xf4)) {
        while (shape1 != NULL) {
            while (shape2 != NULL) {
                if (TestShapeEdgeIntersection(pCollision_1, pCollision_2, shape1, shape2)
                    || TestShapeEdgeIntersection(pCollision_2, pCollision_1, shape2, shape1)) {
                    return 1;
                }
                shape2 = shape2->common.next;
            }
            shape1 = shape1->common.next;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00428dc0
// FUNCTION: CARMA2_SW 0x00428e50
// FUNCTION: CARMA2_CHEAT 0x00428f50
int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, tPhysics_shape* pCollision_shape_1, tPhysics_shape* pCollision_shape_2) {
    br_vector3 points[64];
    br_vector3 transformed;
    br_vector3 point;
    br_bounds bounds;
    br_scalar t;
    tPolyhedron_edge_indexes* edges;
    tPolyhedron_edge_indexes* edge;
    int count_edges;
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gDefault_box_collision_wireframe_edges) != 12);

    switch (pCollision_shape_1->common.type) {
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        for (i = 0; i < pCollision_shape_1->polyhedron.polyhedron.count_points; i++) {
            BrMatrix34ApplyP(&transformed, &pCollision_shape_1->polyhedron.polyhedron.points[i], &pCollision_info_1->transform_matrix);
            DrMatrix34ApplyLPInverse(&points[i], &transformed, &pCollision_info_2->transform_matrix);
        }
        edges = pCollision_shape_1->polyhedron.polyhedron.edges;
        count_edges = pCollision_shape_1->polyhedron.polyhedron.count_edges;
        break;
    case kCollisionShapeType_Box:
    case kCollisionShapeType_Wireframe:
    case kCollisionShapeType_Sphere:
        for (i = 0; i < 8; i++) {
            BrVector3Set(&points[i],
                (i & 0x2) ? pCollision_shape_1->common.bb.min.v[0] : pCollision_shape_1->common.bb.max.v[0],
                (i & 0x1) ? pCollision_shape_1->common.bb.min.v[1] : pCollision_shape_1->common.bb.max.v[1],
                (i & 0x4) ? pCollision_shape_1->common.bb.min.v[2] : pCollision_shape_1->common.bb.max.v[2]);
            BrMatrix34ApplyP(&transformed, &points[i], &pCollision_info_1->transform_matrix);
            DrMatrix34ApplyLPInverse(&points[i], &transformed, &pCollision_info_2->transform_matrix);
        }
        edges = gDefault_box_collision_wireframe_edges;
        count_edges = REC2_ASIZE(gDefault_box_collision_wireframe_edges);
        break;
    }
    switch (pCollision_shape_2->common.type) {
    case kCollisionShapeType_Polyhedron:
    case kCollisionShapeType_Wireframe_Polyhedron:
        for (i = 0, edge = edges; i < count_edges; i++, edge++) {
            if (LinePolyColl(&points[edge->index1], &points[edge->index2], &pCollision_shape_2->polyhedron.polyhedron, &point, 0.005f, &t)) {
                return 1;
            }
        }
        break;
    case kCollisionShapeType_Box:
    case kCollisionShapeType_Wireframe:
    case kCollisionShapeType_Sphere:
        BrVector3Set(&bounds.min,
            pCollision_shape_2->common.bb.min.v[0] - 0.005f,
            pCollision_shape_2->common.bb.min.v[1] - 0.005f,
            pCollision_shape_2->common.bb.min.v[2] - 0.005f);
        BrVector3Set(&bounds.max,
            pCollision_shape_2->common.bb.max.v[0] + 0.005f,
            pCollision_shape_2->common.bb.max.v[1] + 0.005f,
            pCollision_shape_2->common.bb.max.v[2] + 0.005f);
        for (i = 0, edge = edges; i < count_edges; i++, edge++) {
            if (LineBoxColl(&points[edge->index1], &points[edge->index2], &bounds, &point, &t)) {
                return 1;
            }
        }
        break;
    }
    return 0;
}

inline int C2_HOOK_FASTCALL TestObjectOverlapIgnoreCloseRelatives(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {

    if (pCollision_2 == pCollision_1 || pCollision_1->parent == pCollision_2 || pCollision_2->parent == pCollision_1) {
        return 0;
    }
    return TestObjectOverlap(pCollision_1, pCollision_2);
}

// FUNCTION: CARMA2_HW 0x004293c0
// FUNCTION: CARMA2_SW 0x00429450
// FUNCTION: CARMA2_CHEAT 0x00429550
int C2_HOOK_FASTCALL TestObjectOverlapRecurse(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {
    tPhysics_object* child;

    if (TestObjectOverlapIgnoreCloseRelatives(pCollision_1, pCollision_2)) {
        return 1;
    }
    for (child = pCollision_2->child; child != NULL; child = child->next) {
        if (TestObjectOverlapRecurse(pCollision_1, child)) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00429070
// FUNCTION: CARMA2_SW 0x00429100
// FUNCTION: CARMA2_CHEAT 0x00429200
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    tPhysics_joint_force forces[8];
    br_vector3 normal;
    int count;
    int i;
    tPhysics_object* object;

    gObject_blocking_sensible_place = NULL;
    GetNewBoundingBox(&pCollision_info_1->field_0xf4, &pCollision_info_1->bb1, &pCollision_info_1->transform_matrix);
    GetFacesNearObject(pCollision_info_1, pCar_callbacks);
    gINT_00679438 = 1;
    count = GetObjectWallForceInfo(pCollision_info_1, forces, 0);
    gINT_00679438 = 0;
    if (count != 0) {
        BrVector3Copy(&normal, &forces[0].direction_a);
        if (count > 1) {
            for (i = 1; i < count; i++) {
                BrVector3Accumulate(&normal, &forces[i].direction_a);
            }
            BrVector3Normalise(&normal, &normal);
        }
        BrMatrix34ApplyV(pVec3, &normal, &pCollision_info_1->transform_matrix);
        return 0;
    }
    object = pCollision_info_2;
    while (1) {
        if (object == NULL) {
            return -1;
        }
        if (TestObjectOverlapRecurse(pCollision_info_1, object)) {
            BrVector3Add(pVec3, &pCollision_info_1->field_0x10c.min, &pCollision_info_1->field_0x10c.max);
            BrVector3Sub(pVec3, pVec3, &object->field_0x10c.min);
            BrVector3Sub(pVec3, pVec3, &object->field_0x10c.max);
            BrVector3Normalise(pVec3, pVec3);
            gObject_blocking_sensible_place = object;
            return 0;
        }
        object = object->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b7290
// FUNCTION: CARMA2_SW 0x004b39b0
// FUNCTION: CARMA2_CHEAT 0x004bcdc0
br_scalar C2_HOOK_FASTCALL HierarchyCalcEnergy(tPhysics_object* pObject) {
    br_scalar energy;
    br_scalar object_energy;
    int i;

    energy = 0.0f;
    for (; pObject != NULL; pObject = pObject->next) {
        object_energy = 0.0f;
        for (i = 0; i < 3; i++) {
            object_energy += pObject->I.v[i] * REC2_SQR(pObject->omega.v[i]) + pObject->M * REC2_SQR(pObject->v.v[i]);
        }
        energy = HierarchyCalcEnergy(pObject->child) + (object_energy + energy);
    }
    return energy;
}

// CheckCollisionList

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004b77b0
// FUNCTION: CARMA2_SW 0x004b3ed0
// FUNCTION: CARMA2_CHEAT 0x004bd2e0
void C2_HOOK_FASTCALL MakeJointVelocitiesVector(br_scalar* pVelocities, tPhysics_joint_force** pForces, int pCount) {
    tPhysics_object* object;
    br_vector3 v;
    int i;

    for (i = 0; i < pCount; i++) {
        object = pForces[i]->object_a;
        if (pForces[i]->type == 11) {
            pVelocities[i] = -BrVector3Dot(&object->omega, &pForces[i]->direction_a);
            object = pForces[i]->object_b;
            if (object != NULL) {
                pVelocities[i] += BrVector3Dot(&object->omega, &pForces[i]->direction_b);
            }
        } else {
            BrVector3Cross(&v, &object->omega, &pForces[i]->point_a);
            BrVector3Accumulate(&v, &object->velocity_car_space);
            pVelocities[i] = -BrVector3Dot(&pForces[i]->direction_a, &v);
            object = pForces[i]->object_b;
            if (object != NULL) {
                BrVector3Cross(&v, &object->omega, &pForces[i]->point_b);
                BrVector3Accumulate(&v, &object->velocity_car_space);
                pVelocities[i] += BrVector3Dot(&pForces[i]->direction_b, &v);
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004b7960
// FUNCTION: CARMA2_SW 0x004b4080
// FUNCTION: CARMA2_CHEAT 0x004bd490
void C2_HOOK_FASTCALL ApplySingleForce(tPhysics_joint_force* pForce, br_scalar pAmount) {
    br_vector3 angular;
    br_vector3 linear;
    br_vector3 partner_angular;
    br_vector3 partner_linear;
    br_vector3 impulse;
    br_vector3 delta;
    br_vector3 tv;
    tPhysics_object* object_a;
    tPhysics_object* object_b;
    tPhysics_object* partner;

    if (pForce->type == 9 || pAmount == 0.0f) {
        return;
    }
    object_a = pForce->object_a;
    object_b = pForce->object_b;
    if (pForce->type == 11) {
        partner = SetUpTorqueHingePartners(&angular, &linear, &partner_angular, &partner_linear, &pForce->direction_a, object_a);
        BrVector3Scale(&angular, &angular, pAmount);
        BrVector3Accumulate(&object_a->omega, &angular);
        if (partner != NULL) {
            BrVector3Scale(&linear, &linear, pAmount);
            BrVector3Accumulate(&object_a->velocity_car_space, &linear);
            BrMatrix34ApplyV(&object_a->v, &object_a->velocity_car_space, &object_a->transform_matrix);
            BrVector3Scale(&partner_linear, &partner_linear, pAmount);
            BrVector3Accumulate(&partner->velocity_car_space, &partner_linear);
            BrMatrix34ApplyV(&partner->v, &partner->velocity_car_space, &partner->transform_matrix);
            BrVector3Scale(&partner_angular, &partner_angular, pAmount);
            BrVector3Accumulate(&partner->omega, &partner_angular);
        }
        if (object_b != NULL) {
            partner = SetUpTorqueHingePartners(&angular, &linear, &partner_angular, &partner_linear, &pForce->direction_b, object_b);
            BrVector3Scale(&angular, &angular, -pAmount);
            BrVector3Accumulate(&object_b->omega, &angular);
            if (partner != NULL) {
                BrVector3Scale(&linear, &linear, -pAmount);
                BrVector3Accumulate(&object_b->velocity_car_space, &linear);
                BrMatrix34ApplyV(&object_b->v, &object_b->velocity_car_space, &object_b->transform_matrix);
                BrVector3Scale(&partner_linear, &partner_linear, -pAmount);
                BrVector3Accumulate(&partner->velocity_car_space, &partner_linear);
                BrMatrix34ApplyV(&partner->v, &partner->velocity_car_space, &partner->transform_matrix);
                BrVector3Scale(&partner_angular, &partner_angular, -pAmount);
                BrVector3Accumulate(&partner->omega, &partner_angular);
            }
        }
        return;
    }
    if (object_a->field_0x1e4 != NULL) {
        SetUpForce(&angular, &linear, &pForce->point_a, &pForce->direction_a, object_a);
        partner = SetUpHingePartners(&angular, &linear, &partner_angular, &partner_linear, object_a);
        BrVector3Scale(&angular, &angular, pAmount);
        BrVector3Accumulate(&object_a->omega, &angular);
        BrVector3Scale(&partner_angular, &partner_angular, pAmount);
        BrVector3Accumulate(&partner->omega, &partner_angular);
        BrVector3Scale(&linear, &linear, pAmount);
        BrVector3Accumulate(&object_a->velocity_car_space, &linear);
        BrMatrix34ApplyV(&tv, &linear, &object_a->transform_matrix);
        BrVector3Accumulate(&object_a->v, &tv);
        BrVector3Scale(&partner_linear, &partner_linear, pAmount);
        BrVector3Accumulate(&partner->velocity_car_space, &partner_linear);
        BrMatrix34ApplyV(&tv, &partner_linear, &partner->transform_matrix);
        BrVector3Accumulate(&partner->v, &tv);
    } else {
        BrVector3Scale(&impulse, &pForce->direction_a, pAmount);
        BrVector3InvScale(&delta, &impulse, object_a->M);
        BrVector3Accumulate(&object_a->velocity_car_space, &delta);
        BrMatrix34ApplyV(&tv, &delta, &object_a->transform_matrix);
        BrVector3Accumulate(&object_a->v, &tv);
        if (!(object_a->flags & 0x800)) {
            BrVector3Cross(&tv, &pForce->point_a, &impulse);
            object_a->omega.v[0] += tv.v[0] / object_a->I.v[0];
            object_a->omega.v[1] += tv.v[1] / object_a->I.v[1];
            object_a->omega.v[2] += tv.v[2] / object_a->I.v[2];
        }
    }
    if (object_b != NULL && pForce->type != 14) {
        if (object_b->field_0x1e4 != NULL) {
            SetUpForce(&angular, &linear, &pForce->point_b, &pForce->direction_b, object_b);
            partner = SetUpHingePartners(&angular, &linear, &partner_angular, &partner_linear, object_b);
            BrVector3Scale(&angular, &angular, -pAmount);
            BrVector3Accumulate(&object_b->omega, &angular);
            BrVector3Scale(&partner_angular, &partner_angular, -pAmount);
            BrVector3Accumulate(&partner->omega, &partner_angular);
            BrVector3Scale(&linear, &linear, -pAmount);
            BrVector3Accumulate(&object_b->velocity_car_space, &linear);
            BrMatrix34ApplyV(&tv, &linear, &object_b->transform_matrix);
            BrVector3Accumulate(&object_b->v, &tv);
            BrVector3Scale(&partner_linear, &partner_linear, -pAmount);
            BrVector3Accumulate(&partner->velocity_car_space, &partner_linear);
            BrMatrix34ApplyV(&tv, &partner_linear, &partner->transform_matrix);
            BrVector3Accumulate(&partner->v, &tv);
        } else {
            BrVector3Scale(&impulse, &pForce->direction_b, -pAmount);
            BrVector3InvScale(&delta, &impulse, object_b->M);
            BrVector3Accumulate(&object_b->velocity_car_space, &delta);
            BrMatrix34ApplyV(&tv, &delta, &object_b->transform_matrix);
            BrVector3Accumulate(&object_b->v, &tv);
            if (!(object_b->flags & 0x800)) {
                BrVector3Cross(&tv, &pForce->point_b, &impulse);
                object_b->omega.v[0] += tv.v[0] / object_b->I.v[0];
                object_b->omega.v[1] += tv.v[1] / object_b->I.v[1];
                object_b->omega.v[2] += tv.v[2] / object_b->I.v[2];
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL ApplyJointForces(tPhysics_joint_force** pForces, br_scalar* pAmounts, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        ApplySingleForce(pForces[i], pAmounts[i]);
    }
}

inline void C2_HOOK_FASTCALL SkipForces(tPhysics_joint_force* pForce, int* pIndex) {

    if (pForce->object_a->physics_joint1 != NULL) {
        switch (pForce->object_a->physics_joint1->type) {
        case eJoint_hinge:
            *pIndex += 5;
            break;
        case eJoint_universal:
            *pIndex += 4;
            break;
        case eJoint_ball_n_socket:
            *pIndex += 3;
            break;
        case eJoint_quick_hinge:
            *pIndex += 2;
            break;
        default:
            PhysicsError(ePhysicsError_UnknownHingeType, "");
            break;
        }
    }
    if (pForce->object_a->physics_joint2 != NULL) {
        switch (pForce->object_a->physics_joint2->type) {
        case eJoint_hinge:
            *pIndex += 5;
            break;
        case eJoint_universal:
            *pIndex += 4;
            break;
        case eJoint_ball_n_socket:
            *pIndex += 3;
            break;
        case eJoint_translation:
            *pIndex += 2;
            break;
        default:
            PhysicsError(ePhysicsError_UnknownHingeType, "");
            break;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004bc8e0
// FUNCTION: CARMA2_SW 0x004b9000
// FUNCTION: CARMA2_CHEAT 0x004c2410
void C2_HOOK_FASTCALL ScaleUpObjectMI(tPhysics_object* pObject, br_scalar pFactor) {
    tPhysics_joint_force* force_list[40];
    tPhysics_object* partner;
    tPhysics_object* object;
    tPhysics_workspace* workspace;
    int i;

    BrVector3Scale(&pObject->I, &pObject->I, pFactor);
    pObject->field_0x21c *= pFactor;
    if (pObject->field_0x1e4 != NULL) {
        partner = pObject->field_0x1e4->partner;
        BrVector3Scale(&partner->I, &partner->I, pFactor);
        partner->field_0x21c *= pFactor;
        RedoQuickHingeVelocities(pObject);
    }
    object = pObject;
    while (object->field_0x218 == NULL && object->parent != NULL) {
        object = object->parent;
    }
    workspace = object->field_0x218;
    if (workspace != NULL) {
        for (i = 0; i < workspace->count; i++) {
            force_list[i] = &workspace->forces[i];
        }
        MakeMatrix(workspace->field_0x18, workspace->count, force_list, workspace->count);
        MatrixProcess(workspace->field_0x18, workspace->field_0x14, workspace->count);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004bc190
// FUNCTION: CARMA2_SW 0x004b88b0
// FUNCTION: CARMA2_CHEAT 0x004c1cc0
void C2_HOOK_FASTCALL CheckForTwang(tPhysics_joint_force** pForces, br_scalar* pImpulses, int pCount) {
    tPhysics_object* object;
    tPhysics_object* other;
    br_vector3 axis;
    br_vector3 omega_axis;
    br_vector3 rotated;
    br_vector3 direction;
    br_vector3 cross_before;
    br_vector3 cross_after;
    br_matrix34 rotation;
    br_scalar length;
    br_scalar cutoff;
    int i;
    int j;

    i = 0;
    while (i < pCount) {
        object = pForces[i]->object_a;
        if (object->child != NULL) {
            j = i;
            while (j < pCount) {
                if (pForces[j]->object_b == object && BrVector3LengthSquared(&object->rotate_omega) < BrVector3LengthSquared(&object->omega)) {
                    other = pForces[j]->object_a;
                    BrVector3Sub(&axis, &object->physics_joint1->field_0x08, &other->physics_joint1->field_0x14);
                    length = BrVector3Length(&axis);
                    axis.v[0] /= length;
                    axis.v[1] /= length;
                    axis.v[2] /= length;
                    cutoff = (br_scalar)(0.5 * object->M);
                    if (cutoff <= axis.v[0] * pImpulses[i] + axis.v[1] * pImpulses[i + 1] + axis.v[2] * pImpulses[i + 2]) {
                        if (cutoff <= pImpulses[j] * BrVector3Dot(&axis, &pForces[j]->direction_b)
                                + pImpulses[j + 1] * BrVector3Dot(&axis, &pForces[j + 1]->direction_b)
                                + pImpulses[j + 2] * BrVector3Dot(&axis, &pForces[j + 2]->direction_b)) {
                            length = BrVector3Length(&object->omega);
                            omega_axis.v[0] = object->omega.v[0] / length;
                            omega_axis.v[1] = object->omega.v[1] / length;
                            omega_axis.v[2] = object->omega.v[2] / length;
                            BrMatrix34Rotate(&rotation, BrRadianToAngle(0.04f * length), &omega_axis);
                            BrMatrix34ApplyV(&rotated, &axis, &rotation);
                            BrVector3Set(&direction, pImpulses[i], pImpulses[i + 1], pImpulses[i + 2]);
                            BrVector3Normalise(&direction, &direction);
                            BrVector3Cross(&cross_before, &axis, &direction);
                            BrVector3Cross(&cross_after, &rotated, &direction);
                            if (BrVector3Dot(&cross_before, &cross_after) < 0.0f) {
                                BrVector3InvScale(&object->omega, &object->omega, 10.0f);
                                ScaleUpObjectMI(object, 25.0f);
                            }
                        }
                    }
                }
                SkipForces(pForces[j], &j);
            }
        }
        SkipForces(pForces[i], &i);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004b8c20
void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0x0, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_a, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_b, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_a, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_b, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_a, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_b, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, type, 0x5c);

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    pForce->field_0xc = NULL;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, pDirection, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 10;
}

// FUNCTION: CARMA2_HW 0x004b8e10
void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, &pForce->direction_a, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 11;
}

inline tPhysics_workspace* C2_HOOK_FASTCALL AllocateObjectWorkspace(int pCount) {
    tPhysics_workspace* workspace;
    int size;

    size = pCount * sizeof(tPhysics_joint_force) + pCount * pCount * 2 * sizeof(br_scalar) + sizeof(tPhysics_workspace);
    workspace = GetPhysicsScratchPointer(size);
    IncreasePhysicsScratchPointer(size);
    workspace->forces = (tPhysics_joint_force*)((tU8*)workspace + sizeof(tPhysics_workspace));
    workspace->count = pCount;
    workspace->field_0x14 = (br_scalar*)(workspace->forces + pCount);
    workspace->field_0x18 = workspace->field_0x14 + pCount * pCount;
    return workspace;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004b8cf0
// FUNCTION: CARMA2_SW 0x004b5410
// FUNCTION: CARMA2_CHEAT 0x004be820
void C2_HOOK_FASTCALL RedoQuickHingeVelocities(tPhysics_object* pObject) {
    tPhysics_joint_force forces[3];
    tPhysics_joint_force* force_list[3];
    br_matrix34 matrix;
    br_vector3 velocities;
    br_vector3 amounts;
    tPhysics_object* partner;
    tPhysics_quick_hinge* hinge;
    tPhysics_quick_hinge* partner_hinge;
    int i;

    partner = pObject->field_0x230;
    if (pObject->field_0x1e4->partner != partner) {
        partner = pObject;
        pObject = pObject->field_0x1e4->partner;
    }
    CreateJointForce(pObject, partner, &forces[0], &pObject->physics_joint1->hinge_axis);
    CreateJointTorque(pObject, partner, &forces[1], &pObject->physics_joint1->hinge_axis2);
    CreateJointTorque(pObject, partner, &forces[2], &pObject->physics_joint1->hinge_axis3);
    for (i = 0; i < 3; i++) {
        force_list[i] = &forces[i];
    }
    partner_hinge = partner->field_0x1e4;
    hinge = pObject->field_0x1e4;
    pObject->field_0x1e4 = NULL;
    partner->field_0x1e4 = NULL;
    MakeMatrix(matrix.m[0], 3, force_list, 3);
    DRMatrix33Inverse(hinge->field_0x8.m[0], matrix.m[0]);
    MakeJointVelocitiesVector(velocities.v, force_list, 3);
    BrMatrix34ApplyV(&amounts, &velocities, &hinge->field_0x8);
    ApplyJointForces(force_list, amounts.v, 3);
    pObject->field_0x1e4 = hinge;
    partner->field_0x1e4 = partner_hinge;
}

// FUNCTION: CARMA2_HW 0x004b8350
// FUNCTION: CARMA2_SW 0x004b4a70
// FUNCTION: CARMA2_CHEAT 0x004bde80
int C2_HOOK_FASTCALL GetJointForces(tPhysics_object* pObject, tPhysics_joint_force* pForces) {
    tPhysics_object* partner;
    tPhysics_object* child;
    br_vector3 tv;
    br_vector3 tv2;
    br_vector3 velocity;
    br_scalar d;
    int count;
    int i;

    count = 0;
    if (pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none && !(pObject->flags & 0x400)) {
        partner = pObject->field_0x230;
        if (pObject->physics_joint1->type == eJoint_quick_hinge) {
            CreateJointForce(pObject, partner, &pForces[0], &pObject->physics_joint1->hinge_axis2);
            CreateJointForce(pObject, partner, &pForces[1], &pObject->physics_joint1->hinge_axis3);
            pForces += 2;
            count = 2;
        } else {
            count = 3;
            for (i = 0; i < 3; i++) {
                BrVector3Set(&tv, 0.0f, 0.0f, 0.0f);
                tv.v[i] = 1.0f;
                CreateJointForce(pObject, partner, pForces, &tv);
                pForces++;
            }
        }
#define SET_UP_JOINT_TORQUE(FORCE, AXIS)                                                                  \
    (FORCE)->field_0x0 = 0.0f;                                                                            \
    (FORCE)->object_a = pObject;                                                                          \
    (FORCE)->object_b = partner;                                                                          \
    BrVector3Sub(&(FORCE)->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);               \
    BrVector3Sub(&(FORCE)->point_b, &pObject->physics_joint1->field_0x14, &partner->cmpos);               \
    BrVector3Copy(&(FORCE)->direction_a, AXIS);                                                           \
    BrMatrix34ApplyV(&tv, &(FORCE)->direction_a, &pObject->transform_matrix);                             \
    BrMatrix34TApplyV(&(FORCE)->direction_b, &tv, &partner->transform_matrix);                            \
    (FORCE)->type = 11
        switch (pObject->physics_joint1->type) {
        case eJoint_quick_hinge:
            InitQuickHinge(pObject);
            CalcQuickHingeData(pObject);
            RedoQuickHingeVelocities(pObject);

            d = BrVector3Dot(&pObject->physics_joint1->parent_bone_axis, &partner->omega);
            BrVector3Scale(&tv, &pObject->physics_joint1->parent_bone_axis, d);
            BrVector3Sub(&tv, &partner->omega, &tv);
            BrMatrix34ApplyV(&tv2, &tv, &partner->transform_matrix);
            BrMatrix34TApplyV(&tv, &tv2, &pObject->transform_matrix);
            d = BrVector3Dot(&pObject->physics_joint1->hinge_axis, &pObject->omega);
            BrVector3Scale(&pObject->omega, &pObject->physics_joint1->hinge_axis, d);
            BrVector3Accumulate(&pObject->omega, &tv);

            BrVector3Sub(&tv, &pObject->physics_joint1->field_0x14, &partner->cmpos);
            BrVector3Cross(&tv2, &partner->omega, &tv);
            BrMatrix34ApplyV(&tv, &tv2, &partner->transform_matrix);
            BrVector3Accumulate(&tv, &partner->v);
            BrMatrix34TApplyV(&velocity, &tv, &pObject->transform_matrix);
            BrVector3Sub(&tv, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
            BrVector3Cross(&tv2, &pObject->omega, &tv);
            BrVector3Sub(&velocity, &velocity, &tv2);
            d = BrVector3Dot(&pObject->physics_joint1->hinge_axis, &velocity) - BrVector3Dot(&pObject->physics_joint1->hinge_axis, &pObject->velocity_car_space);
            BrVector3Scale(&tv, &pObject->physics_joint1->hinge_axis, d);
            BrVector3Accumulate(&pObject->velocity_car_space, &tv);
            BrMatrix34ApplyV(&pObject->v, &pObject->velocity_car_space, &pObject->transform_matrix);
            break;
        case eJoint_hinge:
            SET_UP_JOINT_TORQUE(&pForces[0], &pObject->physics_joint1->hinge_axis2);
            SET_UP_JOINT_TORQUE(&pForces[1], &pObject->physics_joint1->hinge_axis3);
            pForces += 2;
            count += 2;
            break;
        case eJoint_universal:
            SET_UP_JOINT_TORQUE(pForces, &pObject->physics_joint1->hinge_axis2);
            pForces++;
            count++;
            break;
        case eJoint_ball_n_socket:
            break;
        default:
            PhysicsError(ePhysicsError_UnknownHingeType, NULL);
        }
#undef SET_UP_JOINT_TORQUE
    }
    if (pObject->physics_joint2 != NULL) {
        if (pObject->physics_joint2->type == eJoint_translation) {
            pForces[0].field_0x0 = 0.0f;
            pForces[0].object_a = pObject;
            pForces[0].object_b = NULL;
            pForces[0].field_0xc = NULL;
            BrVector3Copy(&pForces[0].direction_a, &pObject->physics_joint2->hinge_axis2);
            pForces[0].type = 10;
            pForces[1].field_0x0 = 0.0f;
            pForces[1].object_a = pObject;
            pForces[1].object_b = NULL;
            pForces[1].field_0xc = NULL;
            BrVector3Copy(&pForces[1].direction_a, &pObject->physics_joint2->hinge_axis3);
            pForces[1].type = 10;
            pForces += 2;
            count += 2;
        } else {
            count += 3;
            for (i = 0; i < 3; i++) {
                pForces->field_0x0 = 0.0f;
                pForces->object_a = pObject;
                pForces->object_b = NULL;
                pForces->field_0xc = NULL;
                BrVector3Sub(&pForces->point_a, &pObject->physics_joint2->field_0x08, &pObject->cmpos);
                BrVector3Set(&pForces->direction_a, 0.0f, 0.0f, 0.0f);
                pForces->direction_a.v[i] = 1.0f;
                pForces->type = 10;
                pForces++;
            }
            switch (pObject->physics_joint2->type) {
            case eJoint_hinge:
                break;
            case eJoint_universal:
                pForces->field_0x0 = 0.0f;
                pForces->object_a = pObject;
                pForces->object_b = NULL;
                pForces->field_0xc = NULL;
                BrVector3Sub(&pForces->point_a, &pObject->physics_joint2->field_0x08, &pObject->cmpos);
                BrVector3Copy(&pForces->direction_a, &pObject->physics_joint2->hinge_axis2);
                pForces->type = 11;
                pForces++;
                count++;
                break;
            case eJoint_ball_n_socket:
                break;
            default:
                PhysicsError(ePhysicsError_UnknownHingeType, NULL);
            }
        }
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        i = GetJointForces(child, pForces);
        count += i;
        pForces += i;
    }
    return count;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004bd4f0
// FUNCTION: CARMA2_SW 0x004b9c00
// FUNCTION: CARMA2_CHEAT 0x004c3020
void C2_HOOK_FASTCALL GetUniversalRotationAngle(br_matrix34* pMat_1, br_matrix34* pMat_2, br_vector3* pV1, br_vector3* pV2, br_vector3* pV3, br_vector3* pV4, br_vector3* pOut, br_vector3* pAxis) {
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar length;
    br_scalar dot;

    BrMatrix34ApplyV(&tv, pV1, pMat_1);
    BrMatrix34TApplyV(&tv2, &tv, pMat_2);
    BrVector3Add(pAxis, &tv2, pV2);
    length = BrVector3Length(pAxis);
    if (length > 2.3841858e-7f) {
        BrVector3Scale(pAxis, pAxis, 1.0 / length);
    } else {
        BrVector3Set(pAxis, 1.0f, 0.0f, 0.0f);
    }
    BrMatrix34ApplyV(&tv, pV3, pMat_1);
    BrMatrix34TApplyV(pOut, &tv, pMat_2);
    dot = 2.0f * BrVector3Dot(pOut, pAxis);
    BrVector3Scale(&tv, pAxis, dot);
    BrVector3Sub(pOut, pOut, &tv);
    dot = 2.0f * BrVector3Dot(pOut, pV2);
    BrVector3Scale(&tv, pV2, dot);
    BrVector3Sub(pOut, pOut, &tv);
}

inline tPhysics_object* C2_HOOK_FASTCALL GetObjectsParent(tPhysics_object* pObject) {
    tPhysics_object* parent;

    parent = pObject->field_0x230;
    if (parent == NULL) {
        PhysicsError(11, "");
    }
    return parent;
}

// FUNCTION: CARMA2_HW 0x004bca10
// FUNCTION: CARMA2_SW 0x004b9130
// FUNCTION: CARMA2_CHEAT 0x004c2540
int C2_HOOK_FASTCALL GetJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pForces, int pCount, int pMax_count) {
    tPhysics_object* parent;
    tPhysics_joint_limit* limit;
    tPhysics_joint_force* force;
    br_vector3 tv;
    br_vector3 current;
    br_vector3 trial;
    br_vector3 cross;
    br_scalar current_dot;
    br_scalar trial_dot;
    br_scalar lower;
    br_scalar upper;
    br_scalar length;
    int i;
    int j;
    int k;

    if (pObject->physics_joint1 != NULL) {
        parent = GetObjectsParent(pObject);
        for (i = 0; i < pObject->physics_joint1->count_limits && pCount < pMax_count; i++) {
            limit = &pObject->physics_joint1->limits[i];
            switch (limit->type) {
            case eJoint_limit_plane:
                BrMatrix34ApplyV(&tv, &limit->child, &pObject->actor->t.t.mat);
                BrMatrix34TApplyV(&current, &tv, &parent->actor->t.t.mat);
                current_dot = BrVector3Dot(&current, &limit->parent);
                if (current_dot < 0.001) {
                    BrMatrix34ApplyV(&tv, &limit->child, &pObject->transform_matrix);
                    BrMatrix34TApplyV(&trial, &tv, &parent->transform_matrix);
                    trial_dot = BrVector3Dot(&trial, &limit->parent);
                    if (trial_dot >= 0.0f && (trial_dot >= 0.001 || pObject->field_0xf0 != 0)) {
                        force = &pForces[pCount];
                        force->field_0x0 = current_dot - trial_dot;
                        force->type = 11;
                        force->object_a = pObject;
                        force->object_b = parent;
                        BrVector3Cross(&cross, &trial, &limit->parent);
                        length = BrVector3Length(&cross);
                        if (length > 1e-6) {
                            BrVector3InvScale(&force->direction_b, &cross, length);
                        } else {
                            BrMatrix34ApplyV(&cross, &pObject->omega, &pObject->transform_matrix);
                            BrMatrix34TApplyV(&force->direction_b, &cross, &parent->transform_matrix);
                            BrVector3Sub(&force->direction_b, &force->direction_b, &parent->omega);
                            BrVector3Normalise(&force->direction_b, &force->direction_b);
                        }
                        BrMatrix34ApplyV(&cross, &force->direction_b, &parent->transform_matrix);
                        BrMatrix34TApplyV(&force->direction_a, &cross, &pObject->transform_matrix);
                        pCount++;
                        BrVector3Copy(&force->point_a, &pObject->physics_joint1->field_0x08);
                        BrVector3Copy(&force->point_b, &pObject->physics_joint1->field_0x14);
                    }
                }
                break;
            case eJoint_limit_universal:
                GetUniversalRotationAngle(&pObject->actor->t.t.mat, &parent->actor->t.t.mat,
                    &limit->child, &limit->parent,
                    &pObject->physics_joint1->limits[i + 1].child, &pObject->physics_joint1->limits[i + 1].parent,
                    &current, &cross);
                current_dot = BrVector3Dot(&current, &pObject->physics_joint1->limits[i + 1].parent);
                if (current_dot < limit->value + 0.001) {
                    force = &pForces[pCount];
                    GetUniversalRotationAngle(&pObject->transform_matrix, &parent->transform_matrix,
                        &limit->child, &limit->parent,
                        &pObject->physics_joint1->limits[i + 1].child, &pObject->physics_joint1->limits[i + 1].parent,
                        &trial, &force->direction_b);
                    trial_dot = BrVector3Dot(&trial, &pObject->physics_joint1->limits[i + 1].parent);
                    if (trial_dot > limit->value) {
                        BrVector3Cross(&cross, &current, &trial);
                        if (BrVector3Dot(&cross, &force->direction_b) < 0.0f) {
                            BrVector3Negate(&force->direction_b, &force->direction_b);
                        }
                        BrMatrix34ApplyV(&cross, &force->direction_b, &parent->transform_matrix);
                        BrMatrix34TApplyV(&force->direction_a, &cross, &pObject->transform_matrix);
                        pCount++;
                        force->field_0x0 = current_dot - trial_dot;
                        force->type = 11;
                        force->object_a = pObject;
                        force->object_b = parent;
                    }
                }
                i++;
                break;
            default:
                PhysicsError(8, "");
            }
        }
    }
    if (pObject->physics_joint2 != NULL) {
        for (i = 0; i < pObject->physics_joint2->count_limits; i++) {
            limit = &pObject->physics_joint2->limits[i];
            switch (limit->type) {
            case eJoint_limit_10:
                if (pObject->field_0x1e0 != 0) {
                    gINT_00692dd8 = 1;
                    for (j = 0; j < pCount; j++) {
                        force = &pForces[j];
                        if (force->object_a == pObject) {
                            if (force->object_b == NULL) {
                                for (k = j; k < pCount; k++) {
                                    pForces[k] = pForces[k + 1];
                                }
                                pCount--;
                                j--;
                            } else {
                                BrVector3Negate(&tv, &force->direction_a);
                                BrVector3Negate(&force->direction_a, &force->direction_b);
                                BrVector3Copy(&force->direction_b, &tv);
                                BrVector3Copy(&tv, &force->point_a);
                                BrVector3Copy(&force->point_a, &force->point_b);
                                BrVector3Copy(&force->point_b, &tv);
                                force->object_a = force->object_b;
                                force->object_b = pObject;
                                force->type = 14;
                            }
                        } else if (force->object_b == pObject) {
                            force->type = 14;
                        }
                    }
                }
                break;
            case eJoint_limit_plane:
                BrMatrix34ApplyV(&current, &limit->child, &pObject->actor->t.t.mat);
                current_dot = BrVector3Dot(&current, &limit->parent);
                if (current_dot < 0.001) {
                    if (pCount == pMax_count) {
                        return pCount;
                    }
                    force = &pForces[pCount];
                    force->field_0x0 = current_dot;
                    force->type = 11;
                    force->object_a = pObject;
                    force->object_b = NULL;
                    BrVector3Cross(&cross, &current, &limit->parent);
                    length = BrVector3Length(&cross);
                    if (length > 1e-6) {
                        BrVector3InvScale(&force->direction_b, &cross, length);
                    }
                }
                break;
            case eJoint_limit_11:
                BrMatrix34ApplyV(&tv, &pObject->physics_joint2->hinge_axis, &pObject->transform_matrix);
                lower = BrVector3Dot(&tv, &pObject->field_0x17c);
                current_dot = BrVector3Dot(&tv, &pObject->actor->t.t.translate.t);
                upper = lower + limit->value;
                trial_dot = BrVector3Dot(&tv, (br_vector3*)pObject->transform_matrix.m[3]);
                if (current_dot > upper && trial_dot < upper + 0.001) {
                    if (pCount == pMax_count) {
                        return pCount;
                    }
                    force = &pForces[pCount];
                    force->field_0x0 = (upper - trial_dot) / (current_dot - trial_dot);
                    force->field_0xc = NULL;
                    force->type = 2;
                    force->object_a = pObject;
                    force->object_b = NULL;
                    BrVector3Negate(&force->direction_a, &pObject->physics_joint2->hinge_axis);
                    BrVector3SetFloat(&force->point_a, 0.0f, 0.0f, 0.0f);
                    pCount++;
                } else if (current_dot < lower - 0.001 && trial_dot > lower - 0.002) {
                    if (pCount == pMax_count) {
                        return pCount;
                    }
                    force = &pForces[pCount];
                    force->field_0x0 = (lower - trial_dot) / (current_dot - trial_dot);
                    force->field_0xc = NULL;
                    force->type = 2;
                    force->object_a = pObject;
                    force->object_b = NULL;
                    BrVector3Copy(&force->direction_a, &pObject->physics_joint2->hinge_axis);
                    BrVector3SetFloat(&force->point_a, 0.0f, 0.0f, 0.0f);
                    pCount++;
                }
                break;
            default:
                PhysicsError(8, "");
            }
        }
    }
    return pCount;
}

// FUNCTION: CARMA2_HW 0x004bd6d0
// FUNCTION: CARMA2_SW 0x004b9df0
// FUNCTION: CARMA2_CHEAT 0x004c3200
int C2_HOOK_FASTCALL GetListJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pForces, int pCount, int pMax_count) {

    for (; pObject != NULL; pObject = pObject->next) {
        pCount = GetJointLimits(pObject, pForces, pCount, pMax_count);
        if (pObject->child != NULL) {
            pCount = GetListJointLimits(pObject->child, pForces, pCount, pMax_count);
        }
    }
    return pCount;
}

// GetGroupJointLimits

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004bd720
// FUNCTION: CARMA2_SW 0x004b9e40
// FUNCTION: CARMA2_CHEAT 0x004c3250
int C2_HOOK_FASTCALL LimitOmega(tPhysics_object* pObject) {
    br_scalar ts;
    br_scalar scale;
    tPhysics_object* partner;

    ts = BrVector3LengthSquared(&pObject->omega) * REC2_SQR(0.04f);
    if (ts > (br_scalar)REC2_SQR(1.57)) {
        ts = (br_scalar)sqrt(ts) * (2.0f / 1.57f);
        scale = 2.0f * ts;
        pObject->omega.v[0] /= scale;
        pObject->omega.v[1] /= scale;
        pObject->omega.v[2] /= scale;
        if (pObject->field_0x1e4 != NULL) {
            partner = pObject->field_0x1e4->partner;
            partner->omega.v[0] /= scale;
            partner->omega.v[1] /= scale;
            partner->omega.v[2] /= scale;
        }
        ScaleUpObjectMI(pObject, ts * ts);
        if (pObject->field_0x198 != NULL) {
            pObject->field_0x198(pObject);
        }
        return 1;
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c1840
// FUNCTION: CARMA2_SW 0x004bdf70
void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject) {
    br_scalar ts = BrVector3LengthSquared(&pObject->omega) * REC2_SQR(0.04f);
    if (ts > (br_scalar)REC2_SQR(1.57)) {
        ts = (br_scalar)sqrt(ts) * (2.0f / 3.14f);
        pObject->omega.v[0] = pObject->omega.v[0] / (2.f * ts);
        pObject->omega.v[1] = pObject->omega.v[1] / (2.f * ts);
        pObject->omega.v[2] = pObject->omega.v[2] / (2.f * ts);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004bd7f0
// FUNCTION: CARMA2_SW 0x004b9f10
// FUNCTION: CARMA2_CHEAT 0x004c3320
int C2_HOOK_FASTCALL ListLimitOmega(tPhysics_object* pObject) {
    int count;

    count = 0;
    for (; pObject != NULL; pObject = pObject->next) {
        count += LimitOmega(pObject);
        if (pObject->child != NULL) {
            count += ListLimitOmega(pObject->child);
        }
    }
    return count;
}

// GroupLimitOmega

// FUNCTION: CARMA2_HW 0x004b8fe0
// FUNCTION: CARMA2_SW 0x004b5700
void C2_HOOK_FASTCALL PositionWorldJointedObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    br_vector3 axis;
    br_vector3 tv;
    br_scalar ts;

    if (pObject->physics_joint2->type == eJoint_translation) {
        BrMatrix34ApplyV(&axis, &pObject->physics_joint2->hinge_axis, &pObject->transform_matrix);
        ts = BrVector3Dot(&pObject->actor->t.t.translate.t, &axis);
        BrMatrix34ApplyP(&tv, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        BrVector3Sub(&tv, &pObject->field_0x17c, &tv);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
        ts -= BrVector3Dot(&pObject->actor->t.t.translate.t, &axis);
        BrVector3Scale(&tv, &axis, ts);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
        ts = BrVector3Dot(&pObject->v, &axis);
        BrVector3Scale(&pObject->v, &axis, ts);
        TranslateObject(pObject, pDelta_time);
    } else {
        BrMatrix34ApplyP(&tv, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        BrVector3Sub(&tv, &pObject->field_0x17c, &tv);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b9230
// STUB: CARMA2_SW 0x004b5950
// STUB: CARMA2_CHEAT 0x004bed60
void C2_HOOK_FASTCALL TranslateAndRotatePartialObject(tPhysics_object* pObject, tPhysics_object** pRoot, br_scalar pDelta_time) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004b8ee0
// FUNCTION: CARMA2_SW 0x004b5600
// FUNCTION: CARMA2_CHEAT 0x004bea10
void C2_HOOK_FASTCALL TranslateAndRotateJointedObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    tPhysics_object* root;
    tPhysics_object* object;
    tPhysics_object* parent;
    br_vector3 pos;
    br_vector3 parent_pos;

    root = NULL;
    TranslateAndRotatePartialObject(pObject, &root, pDelta_time);
    if (root != NULL && root->field_0x230 != NULL) {
        object = root;
        PositionWorldJointedObject(object, pDelta_time);
        do {
            parent = object->field_0x230;
            BrMatrix34ApplyP(&pos, &object->physics_joint1->field_0x08, &object->actor->t.t.mat);
            BrMatrix34ApplyP(&parent_pos, &object->physics_joint1->field_0x14, &parent->actor->t.t.mat);
            BrVector3Sub(&pos, &pos, &parent_pos);
            BrVector3Accumulate(&object->parent->actor->t.t.translate.t, &pos);
            object = object->field_0x230;
        } while (object->field_0x230 != NULL);
        WriteNewPositions(pObject);
    }
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x004b9650
// FUNCTION: CARMA2_SW 0x004b5d70
// FUNCTION: CARMA2_CHEAT 0x004bf180
int C2_HOOK_FASTCALL CountJointForces(tPhysics_object* pObject, tPhysics_object* pParent) {
    tPhysics_joint* joint;
    tPhysics_object* child;
    int count;

    joint = pObject->physics_joint1;
    pObject->field_0x230 = pParent;
    pObject->field_0x1e4 = NULL;
    if (joint != NULL && !(pObject->flags & 0x400)) {
        switch (joint->type) {
        case eJoint_none:
            count = 0;
            break;
        case eJoint_universal:
            count = 4;
            break;
        case eJoint_ball_n_socket:
            count = 3;
            break;
        case eJoint_quick_hinge:
            if (pParent->field_0x1e4 == NULL) {
                count = 2;
                pObject->field_0x1e4 = (tPhysics_quick_hinge*)-1;
                pParent->field_0x1e4 = (tPhysics_quick_hinge*)-1;
                break;
            }
            joint->type = eJoint_hinge;
            /* fall through */
        case eJoint_hinge:
            count = 5;
            break;
        default:
            PhysicsError(8, "");
        }
    } else {
        count = 0;
    }
    if (pObject->physics_joint2 != NULL) {
        switch (pObject->physics_joint2->type) {
        case eJoint_universal:
            count += 4;
            break;
        case eJoint_ball_n_socket:
            count += 3;
            break;
        case eJoint_translation:
            count += 2;
            break;
        default:
            break;
        }
    }
    if (!(pObject->flags & 0x400)) {
        pParent = pObject;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        count += CountJointForces(child, pParent);
    }
    return count;
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// FUNCTION: CARMA2_HW 0x004b7510
// FUNCTION: CARMA2_SW 0x004b3c30
// FUNCTION: CARMA2_CHEAT 0x004bd040
void C2_HOOK_FASTCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time) {
    tPhysics_joint_force* force_list[40];
    br_scalar velocities[100];
    br_scalar amounts[100];
    tPhysics_workspace* workspace;
    tPhysics_joint_force* forces;
    int count;
    int i;

    count = CountJointForces(pObject, NULL);
    if (count != 0) {
        workspace = AllocateObjectWorkspace(count);
        forces = workspace->forces;
        count = GetJointForces(pObject, forces);
        pObject->field_0x218 = workspace;
        for (i = 0; i < count; i++) {
            force_list[i] = forces;
            forces++;
        }
        MakeMatrix(workspace->field_0x18, count, force_list, count);
        MakeJointVelocitiesVector(velocities, force_list, count);
        RQMatrixSolveWithNegatives(workspace->field_0x18, workspace->field_0x14, velocities, amounts, count);
        if (pDelta_time != 0.0f) {
            HierarchyCalcEnergy(pObject);
            ApplyJointForces(force_list, amounts, count);
            HierarchyCalcEnergy(pObject);
            if (pDelta_time > 0.0f) {
                TranslateAndRotateJointedObject(pObject, pDelta_time);
            }
        }
    } else if (pObject->child != NULL) {
        TranslateAndRotateJointedObject(pObject, pDelta_time);
    } else {
        TranslateObject(pObject, pDelta_time);
        RotateObject(pObject, pDelta_time);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004b9750
// FUNCTION: CARMA2_SW 0x004b5e70
// FUNCTION: CARMA2_CHEAT 0x004bf280
void C2_HOOK_FASTCALL TellPhysicsJointsHaveChanged(tPhysics_object* pCollision_info) {

    pCollision_info->field_0x218 = 0;
    MoveJointedObject(pCollision_info, 0.0f);
    gINT_00692dc8 = 1;
}

void C2_HOOK_FASTCALL CombineBounds(br_bounds* pBounds, br_bounds* pOther) {
    int i;

    for (i = 0; i < 3; i++) {
        if (pOther->min.v[i] < pBounds->min.v[i]) {
            pBounds->min.v[i] = pOther->min.v[i];
        }
        if (pOther->max.v[i] > pBounds->max.v[i]) {
            pBounds->max.v[i] = pOther->max.v[i];
        }
    }
}

extern void C2_HOOK_FASTCALL GetWorldBoundingBoxForObject(tPhysics_object* pObject, br_bounds* pBounds);

void C2_HOOK_FASTCALL GetWorldBoundingBoxForObjects(tPhysics_object* pObject, br_bounds* pBounds) {

    for (; pObject != NULL; pObject = pObject->next) {
        GetWorldBoundingBoxForObject(pObject, pBounds);
    }
}

// FUNCTION: CARMA2_HW 0x004b9ce0
// FUNCTION: CARMA2_SW 0x004b6400
// FUNCTION: CARMA2_CHEAT 0x004bf810
void C2_HOOK_FASTCALL GetWorldBoundingBoxForObject(tPhysics_object* pObject, br_bounds* pBounds) {
    br_bounds bounds;
    br_scalar extent;
    int i;

    GetNewBoundingBox(&pObject->field_0xf4, &pObject->bb1, &pObject->transform_matrix);
    GetNewBoundingBox(&bounds, &pObject->bb1, &pObject->actor->t.t.mat);
    for (i = 0; i < 3; i++) {
        if (pObject->field_0xf4.min.v[i] > bounds.min.v[i]) {
            pObject->field_0xf4.min.v[i] = bounds.min.v[i];
        }
        if (pObject->field_0xf4.max.v[i] < bounds.max.v[i]) {
            pObject->field_0xf4.max.v[i] = bounds.max.v[i];
        }
        pObject->field_0xf4.min.v[i] -= 0.0025;
        pObject->field_0xf4.max.v[i] -= -0.0025;
    }
    pObject->field_0x10c = pObject->field_0xf4;
    GetWorldBoundingBoxForObjects(pObject->child, &pObject->field_0x10c);
    if (pBounds != NULL) {
        CombineBounds(pBounds, &pObject->field_0x10c);
    } else {
        extent = pObject->field_0x10c.max.v[0] - pObject->transform_matrix.m[3][0];
        if (gPhysics_bounds_max_x <= extent) {
            gPhysics_bounds_max_x = extent;
        }
        extent = pObject->field_0x10c.min.v[0] - pObject->transform_matrix.m[3][0];
        if (gPhysics_bounds_min_x >= extent) {
            gPhysics_bounds_min_x = extent;
        }
    }
}

int C2_HOOK_FASTCALL MatricesAreIdentical(br_matrix34* pMat1, br_matrix34* pMat2) {
    int i;
    int j;

    for (i = 3; i >= 0; i--) {
        for (j = 0; j < 3; j++) {
            if (pMat1->m[i][j] != pMat2->m[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}

extern void C2_HOOK_FASTCALL PrepareForCollisions(tPhysics_object* pObject);

// FUNCTION: CARMA2_HW 0x004b99e0
// FUNCTION: CARMA2_SW 0x004b6100
// FUNCTION: CARMA2_CHEAT 0x004bf510
void C2_HOOK_FASTCALL InternalPrepareObject(tPhysics_object* pObject) {
    br_vector3 offset;

    if (pObject->M == 0.0f) {
        pObject->M = 0.01f;
        BrVector3SetFloat(&pObject->I, 0.01f, 0.01f, 0.01f);
    }
    pObject->field_0x1de = 0;
    pObject->collision_flag = 0;
    pObject->field_0x1dc = 0;
    pObject->field_0x1e4 = 0;
    pObject->flags &= ~0x1200;
    pObject->field_0x21c = 1.0f;
    pObject->field183_0x1d8 = pObject;
    pObject->field_0x218 = 0;
    pObject->field_0x1e0 = pObject->disable_move_rotate;
    BrMatrix34TApplyV(&pObject->velocity_car_space, &pObject->v, &pObject->transform_matrix);
    BrVector3Copy(&pObject->field_0x1b4, &pObject->v);
    BrVector3Copy(&pObject->field_0x1cc, &pObject->omega);
    if (pObject->parent != NULL) {
        pObject->disable_move_rotate = pObject->parent->disable_move_rotate;
    }
    if (!pObject->disable_move_rotate && pObject->field_0x1df) {
        pObject->field_0x21c *= 50.0f;
        BrVector3Scale(&pObject->I, &pObject->I, 50.0f);
    }
    if (pObject->disable_move_rotate & (pObject->field_0xf0 != 2)) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, &pObject->transform_matrix);
        PrepareForCollisions(pObject->child);
        if (pObject->parent == NULL) {
            GetWorldBoundingBoxForObject(pObject, NULL);
        }
    } else if (pObject->field_0xf0 == 2) {
        pObject->field_0x230 = pObject->parent;
        BrVector3Sub(&offset, (br_vector3*)pObject->actor->t.t.mat.m[3], (br_vector3*)pObject->transform_matrix.m[3]);
        if (BrVector3LengthSquared(&offset) > 1.0) {
            PhysicsWarning("Stupid trial position");
            BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
        }
        if (!MatricesAreIdentical(&pObject->transform_matrix, &pObject->actor->t.t.mat)) {
            pObject->disable_move_rotate = 0;
        }
        PrepareForCollisions(pObject->child);
        GetWorldBoundingBoxForObject(pObject, NULL);
    } else {
        if (pObject->field_0xf0 != 1) {
            BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
        } else {
            BrMatrix34Copy(&pObject->field_0xbc, &pObject->actor->t.t.mat);
        }
        PrepareForCollisions(pObject->child);
        if (pObject->parent == NULL) {
            MoveJointedObject(pObject, 0.04f);
            GetWorldBoundingBoxForObject(pObject, NULL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b9770
// FUNCTION: CARMA2_SW 0x004b5e90
// FUNCTION: CARMA2_CHEAT 0x004bf2a0
void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList) {

    InternalPrepareObject(pObject);
    gPrepared_objects = 1;
    CheckObjectsPostionInList(pObject, pList);
    if (gINT_006940ac != 0) {
        MakeObjectDoSomething(pObject, *pList);
    }
    if (gReduced_object_list != NULL) {
        AddObjectToReducedList(pObject, gReduced_object_list);
    }
}

void C2_HOOK_FASTCALL PrepareForCollisions(tPhysics_object* pObject) {

    for (; pObject != NULL; pObject = pObject->next) {
        InternalPrepareObject(pObject);
    }
}

inline int CheckPointVelocities(tPhysics_object* pObject, br_scalar pMax_velocity_squared) {
    int i;
    br_vector3 point;
    br_vector3 base;
    br_vector3 velocity;

    BrVector3Cross(&base, &pObject->omega, &pObject->cmpos);
    BrVector3Accumulate(&base, &pObject->velocity_car_space);
    for (i = 0; i < 8; i++) {
        if (i & 0x2) {
            point.v[0] = pObject->bb1.min.v[0];
        } else {
            point.v[0] = pObject->bb1.max.v[0];
        }
        if (i & 0x1) {
            point.v[1] = pObject->bb1.min.v[1];
        } else {
            point.v[1] = pObject->bb1.max.v[1];
        }
        if (i & 0x4) {
            point.v[2] = pObject->bb1.min.v[2];
        } else {
            point.v[2] = pObject->bb1.max.v[2];
        }
        BrVector3Cross(&velocity, &pObject->omega, &point);
        BrVector3Accumulate(&velocity, &base);
        if (BrVector3LengthSquared(&velocity) > pMax_velocity_squared) {
            return 0;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004baa00
// FUNCTION: CARMA2_SW 0x004b7120
// FUNCTION: CARMA2_CHEAT 0x004c0530
int C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject) {
    int all_doing_nothing;
    int doing_nothing;
    br_vector3 tv;
    br_scalar energy;
    br_scalar v_squared;
    br_scalar length;
    br_scalar normal_v;
    br_scalar normal_v2;

    all_doing_nothing = 1;
    for (; pObject != NULL; pObject = pObject->next) {
        if (pObject->field_0x21c != 1.0f) {
            pObject->I.v[0] /= pObject->field_0x21c;
            pObject->I.v[1] /= pObject->field_0x21c;
            pObject->I.v[2] /= pObject->field_0x21c;
        }
        if (pObject->field_0xf0) {
            pObject->field_0xf0 = 0;
            pObject->disable_move_rotate = ((tU32)pObject->flags >> 5) & 0x1;
        }
        doing_nothing = 0;
        pObject->flags = (pObject->flags & ~0x100) | (((tU32)pObject->flags >> 1) & 0x100);
        if (!pObject->disable_move_rotate) {
            if (pObject->field_0x1df && pObject->field183_0x1d8 == pObject) {
                pObject->field_0x1df = 0;
            }
            BrVector3Add(&tv, &pObject->field_0x1cc, &pObject->omega);
            energy = (pObject->I.v[0] * (tv.v[0] * tv.v[0])
                + pObject->I.v[1] * (tv.v[1] * tv.v[1])
                + pObject->I.v[2] * (tv.v[2] * tv.v[2])) / pObject->M;
            if (!pObject->collision_flag && pObject->field_0x1df) {
                v_squared = 0.0f;
            } else {
                v_squared = BrVector3LengthSquared(&pObject->v);
            }
            if (v_squared < 0.001 && energy < 0.03 * 0.003) {
                length = BrVector3Length(&pObject->field_0x54);
                tv.v[0] = pObject->field_0x54.v[0] / length;
                tv.v[1] = pObject->field_0x54.v[1] / length;
                tv.v[2] = pObject->field_0x54.v[2] / length;
                normal_v = -BrVector3Dot(&tv, &pObject->v);
                normal_v2 = -BrVector3Dot(&tv, &pObject->field_0x1b4);
                if (pObject->field_0x1df
                    || (normal_v < 0.012f && length * 0.9f < normal_v - normal_v2 && CheckPointVelocities(pObject, 0.0004f))) {
                    if (pObject->physics_joint2 == NULL || pObject->collision_flag || pObject->transform_matrix.m[1][1] > 0.999) {
                        doing_nothing = 1;
                        BrVector3SetFloat(&pObject->v, 0.0f, 0.0f, 0.0f);
                        BrVector3SetFloat(&pObject->omega, 0.0f, 0.0f, 0.0f);
                        BrVector3SetFloat(&pObject->rotate_omega, 0.0f, 0.0f, 0.0f);
                    }
                }
            }
        }
        pObject->field_0x1df = doing_nothing;
        BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
        BrMatrix34ApplyP(&pObject->pos, &pObject->cmpos, &pObject->transform_matrix);
        if (pObject->child != NULL) {
            doing_nothing = WriteNewPositions(pObject->child);
            if (pObject->field_0x1df) {
                pObject->field_0x1df += doing_nothing * 2;
            }
        } else {
            pObject->field_0x1df *= 3;
        }
        all_doing_nothing &= (pObject->field_0x1df >> 1) & 0x1;
    }
    return all_doing_nothing;
}

// FUNCTION: CARMA2_HW 0x004b9eb0
// FUNCTION: CARMA2_SW 0x004b65d0
// FUNCTION: CARMA2_CHEAT 0x004bf9e0
void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, int pFlag) {
    while (pObject != NULL) {
        pObject->disable_move_rotate = pFlag;
        pObject->field_0x1df = 0;
        if (pObject->child != NULL) {
            SetListDoingNothingFlag(pObject->child, pFlag);
        }
        pObject = pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b9e40
// FUNCTION: CARMA2_SW 0x004b6560
// FUNCTION: CARMA2_CHEAT 0x004bf970
void C2_HOOK_FASTCALL SetGroupDoingNothingFlag(tPhysics_object* pObject, int pFlag) {
    tPhysics_object* object;

    pObject->disable_move_rotate = pFlag;
    object = pObject;
    do {
        SetHierarchyDoingNothingFlag(object, pFlag);
        object = object->field183_0x1d8;
    } while (object != NULL && object != pObject);
}

// FUNCTION: CARMA2_HW 0x004b9ef0
// FUNCTION: CARMA2_SW 0x004b6610
// FUNCTION: CARMA2_CHEAT 0x004bfa20
void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, int pFlag) {
    pObject->disable_move_rotate = pFlag;
    pObject->field_0x1df = 0;
    if (pObject->child != NULL) {
        SetListDoingNothingFlag(pObject->child, pFlag);
    }
}

inline void CheckDoingNothingFlag(tPhysics_object* pObject) {
    tPhysics_object* object;
    tU8 flag;

    for (; pObject != NULL; pObject = pObject->next) {
        if (!pObject->disable_move_rotate && (pObject->field_0x1df & 0x2)) {
            flag = 0x2;
            for (object = pObject->field183_0x1d8; object != pObject; object = object->field183_0x1d8) {
                flag &= object->field_0x1df;
            }
            if (flag & 0x2) {
                SetGroupDoingNothingFlag(pObject, 1);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004bae80
// FUNCTION: CARMA2_SW 0x004b75a0
// FUNCTION: CARMA2_CHEAT 0x004c09b0
void C2_HOOK_FASTCALL SetGroup(tPhysics_object* pObject, int pGroup) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->field_0x1dc = (tU8)pGroup;
    for (; child != NULL; child = child->next) {
        if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
            SetGroup(child, pGroup);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004bddd0
// FUNCTION: CARMA2_SW 0x004ba4f0
// FUNCTION: CARMA2_CHEAT 0x004c3900
void C2_HOOK_FASTCALL GroupTogether(tPhysics_object* pObject_1, tPhysics_object* pObject_2) {
    tPhysics_object* object;

    while (pObject_1->parent != NULL && pObject_1->physics_joint1 != NULL && pObject_1->physics_joint1->type != eJoint_none) {
        pObject_1 = pObject_1->parent;
    }
    while (pObject_2->parent != NULL && pObject_2->physics_joint1 != NULL && pObject_2->physics_joint1->type != eJoint_none) {
        pObject_2 = pObject_2->parent;
    }
    if (pObject_1 != pObject_2) {
        if (pObject_2 == NULL) {
            if (pObject_1->field_0x1dc == 0) {
                SetGroup(pObject_1, gPHIL_collision_group);
                pObject_1->field183_0x1d8 = pObject_1;
                gPHIL_collision_group++;
            }
        } else if (pObject_1->field_0x1dc == 0) {
            if (pObject_2->field_0x1dc == 0) {
                SetGroup(pObject_1, gPHIL_collision_group);
                SetGroup(pObject_2, gPHIL_collision_group);
                pObject_1->field183_0x1d8 = pObject_2;
                pObject_2->field183_0x1d8 = pObject_1;
                gPHIL_collision_group++;
            } else {
                SetGroup(pObject_1, pObject_2->field_0x1dc);
                pObject_1->field183_0x1d8 = pObject_2->field183_0x1d8;
                pObject_2->field183_0x1d8 = pObject_1;
            }
        } else if (pObject_2->field_0x1dc == 0) {
            SetGroup(pObject_2, pObject_1->field_0x1dc);
            pObject_2->field183_0x1d8 = pObject_1->field183_0x1d8;
            pObject_1->field183_0x1d8 = pObject_2;
        } else if (pObject_1->field_0x1dc != pObject_2->field_0x1dc) {
            for (object = pObject_2; object->field183_0x1d8 != pObject_2; object = object->field183_0x1d8) {
                SetGroup(object, pObject_1->field_0x1dc);
            }
            SetGroup(object, pObject_1->field_0x1dc);
            object->field183_0x1d8 = pObject_1->field183_0x1d8;
            pObject_1->field183_0x1d8 = pObject_2;
        }
    }
}

// GetPreCalculatedJointForces

// FUNCTION: CARMA2_HW 0x004b9f40
// FUNCTION: CARMA2_SW 0x004b6660
// FUNCTION: CARMA2_CHEAT 0x004bfa70
void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject) {
    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->box_face_ref = gFace_num__car - 2;
            pObject->disable_move_rotate = 0;
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b9f90
// FUNCTION: CARMA2_SW 0x004b66b0
// FUNCTION: CARMA2_CHEAT 0x004bfac0
int C2_HOOK_FASTCALL CheckForObjectTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pOther, tPhysics_object* pList) {

    if (pObject->field_0xf4.min.v[0] - pOther->field_0xf4.max.v[0] > 0.01
            || pObject->field_0xf4.min.v[1] - pOther->field_0xf4.max.v[1] > 0.01
            || pObject->field_0xf4.min.v[2] - pOther->field_0xf4.max.v[2] > 0.01
            || pOther->field_0xf4.min.v[0] - pObject->field_0xf4.max.v[0] > 0.01
            || pOther->field_0xf4.min.v[1] - pObject->field_0xf4.max.v[1] > 0.01
            || pOther->field_0xf4.min.v[2] - pObject->field_0xf4.max.v[2] > 0.01) {
        return 0;
    } else {
        MakeObjectDoSomething(pOther, pList);
        return 1;
    }
}

void C2_HOOK_FASTCALL CheckForObjectListTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pOther, tPhysics_object* pList) {

    while (pObject != NULL && !CheckForObjectTouchingAnotherObject(pObject, pOther, pList)) {
        pObject = pObject->next;
    }
}

void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingAnotherObject(tPhysics_object* pObject, tPhysics_object* pOther, tPhysics_object* pList) {

    CheckForObjectTouchingAnotherObject(pObject, pOther, pList);
    if (pObject->child != NULL && pOther->disable_move_rotate) {
        CheckForObjectListTouchingAnotherObject(pObject->child, pOther, pList);
    }
}

// FUNCTION: CARMA2_HW 0x004ba190
// FUNCTION: CARMA2_SW 0x004b68b0
// FUNCTION: CARMA2_CHEAT 0x004bfcc0
void C2_HOOK_FASTCALL CheckForObjectHierachyTouchingObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object* pList_original) {
    tPhysics_object* list;

    for (list = pList; list != NULL; list = list->next) {
        if (list->disable_move_rotate) {
            CheckForObjectHierachyTouchingAnotherObject(pObject, list, pList_original);
            if (list->child != NULL) {
                CheckForObjectHierachyTouchingObjectList(pObject, list->child, pList_original);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004be0e0
// FUNCTION: CARMA2_SW 0x004ba800
// FUNCTION: CARMA2_CHEAT 0x004c3c10
void C2_HOOK_FASTCALL MakeObjectDoSomething(tPhysics_object* pObject, tPhysics_object* pList) {

    if (pObject->disable_move_rotate) {
        while (pObject->parent != NULL && pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none) {
            pObject = pObject->parent;
        }
        pObject->box_face_ref = gFace_num__car - 2;
        pObject->disable_move_rotate = 0;
        if (pObject->child != NULL || pObject->physics_joint1 != NULL || pObject->physics_joint2 != NULL) {
            if (pObject->field_0x218 == 0) {
                MoveJointedObject(pObject, 0.0f);
            }
            MakeObjectListDoSomething(pObject->child);
        }
        CheckForObjectHierachyTouchingObjectList(pObject, pList, pList);
    }
}

// GetReducedList

inline tPhysics_object** C2_HOOK_FASTCALL ReduceReducedList(tPhysics_object* pObject, tPhysics_object** pList) {
    tPhysics_object* reduced;
    tPhysics_object** reduced_end;
    tPhysics_object* current;

    reduced_end = &reduced;
    for (current = *pList; current != NULL; current = *pList) {
        if (current == pObject || BoundsOverlapTest(&pObject->field_0x10c, &current->field_0x10c)) {
            *pList = current->field_0x234;
            *reduced_end = current;
            reduced_end = &current->field_0x234;
        } else {
            pList = &current->field_0x234;
        }
    }
    *reduced_end = NULL;
    *pList = reduced;
    return pList;
}

// FUNCTION: CARMA2_HW 0x004bd9b0
// FUNCTION: CARMA2_SW 0x004ba0d0
// FUNCTION: CARMA2_CHEAT 0x004c34e0
void C2_HOOK_FASTCALL SetObjectTrialPositionType(tPhysics_object* pObject, int pType) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->field_0xf0 = pType;
    for (; child != NULL; child = child->next) {
        SetObjectTrialPositionType(child, pType);
    }
}

inline void C2_HOOK_FASTCALL FillInTrialWorkspace(tPhysics_object* pObject, tPhysics_object* pOther, tWorld_callbacks* pCallbacks) {

    while (pObject->parent != NULL && pObject->physics_joint1 != NULL && pObject->physics_joint1->type != eJoint_none) {
        pObject = pObject->parent;
    }
    SetObjectTrialPositionType(pObject, 0);
    if (pCallbacks != NULL && pCallbacks->activate_passive != NULL) {
        pCallbacks->activate_passive(pObject, pOther);
    }
    if ((pObject->child != NULL || pObject->physics_joint2 != NULL) && pObject->field_0x218 == 0) {
        MoveJointedObject(pObject, 0.0f);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004ba4f0
// FUNCTION: CARMA2_SW 0x004b6c10
// FUNCTION: CARMA2_CHEAT 0x004c0020
int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {
    int blocked;

    blocked = (pCollision_1->field_0x1a0 & pCollision_2->field_0x1a4)
        || (pCollision_2->field_0x1a0 & pCollision_1->field_0x1a4)
        || (pCollision_1->flags & 4)
        || (pCollision_2->flags & 4);
    return !blocked;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline int C2_HOOK_FASTCALL CollideObjectWithReducedObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_joint_force* pForces, int pCount, tPhysics_object* pDo_something_list, tWorld_callbacks* pCallbacks) {
    int count;

    for (; pList != NULL; pList = pList->field_0x234) {
        if (pList != pObject
            && (pObject->field_0x1dc == 0 || pList->field_0x1dc != pObject->field_0x1dc || pObject < pList)
            && pObject->parent != pList && pList->parent != pObject
            && FlagsAreGo(pObject, pList)) {
            count = GetObjectObjectForceInfo(pObject, pList, pForces, pCount);
            if (count != 0) {
                if (pObject->field_0xf0 == 2) {
                    FillInTrialWorkspace(pObject, pList, pCallbacks);
                }
                if (pList->field_0xf0 == 2) {
                    FillInTrialWorkspace(pList, pObject, pCallbacks);
                }
                if (pList->disable_move_rotate != 0) {
                    MakeObjectDoSomething(pList, pDo_something_list);
                }
                GroupTogether(pObject, pList);
                pCount += count;
            }
        }
    }
    return pCount;
}

inline void C2_HOOK_FASTCALL AddObjectToReducedList(tPhysics_object* pObject, tPhysics_object** pList) {

    pObject->field_0x234 = *pList;
    *pList = pObject;
}

// FUNCTION: CARMA2_HW 0x004bd9e0
// FUNCTION: CARMA2_SW 0x004ba100
// FUNCTION: CARMA2_CHEAT 0x004c3510
int C2_HOOK_FASTCALL ReducedFindObjectCollisions(tPhysics_object* pObject, tPhysics_object** pList, tPhysics_joint_force* pForces, int pCount, tWorld_callbacks* pCallbacks, tPhysics_object* pDo_something_list) {
    tPhysics_object* child;
    int count;

    gReduced_object_list = pList;
    if (BrVector3LengthSquared(&pObject->field_0x1b4) == 0.0f) {
        gINT_006940ac = 1;
    } else {
        gINT_006940ac = 0;
    }
    GetFacesNearObject(pObject, pCallbacks);
    gReduced_object_list = NULL;
    gINT_006940ac = 0;
    count = GetObjectWallForceInfo(pObject, pForces, pCount);
    if (count != 0) {
        if (pObject->field_0xf0 == 2) {
            FillInTrialWorkspace(pObject, NULL, pCallbacks);
        }
        pCount += count;
    }
    pCount = CollideObjectWithReducedObjectList(pObject, *pList, pForces, pCount, pDo_something_list, pCallbacks);
    for (child = pObject->child; child != NULL; child = child->next) {
        if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
            pCount = ReducedFindObjectCollisions(child, ReduceReducedList(child, pList), pForces, pCount, pCallbacks, pDo_something_list);
        }
    }
    return pCount;
}

// FindObjectCollisions

// DoRigidJoints

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004baec0
// STUB: CARMA2_SW 0x004b75e0
// STUB: CARMA2_CHEAT 0x004c09f0
int C2_HOOK_FASTCALL DoInterGroupCollisions(tPhysics_object** pObject_list, tPhysics_object* pObject, tPhysics_joint_force* pForces, tPhysics_joint_force* pOld_forces, int pCount_old_forces, tWorld_callbacks* pCallbacks) {
    /* FIXME: the types of pForces, pOld_forces and pCount_old_forces are guesses */
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ba550
// FUNCTION: CARMA2_SW 0x004b6c70
// FUNCTION: CARMA2_CHEAT 0x004c0080
int C2_HOOK_FASTCALL ReduceEffectOfForces(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount, const int* pIndices, int pIndex_count, br_scalar pThreshold) {
    int i;
    br_scalar total;

    total = 0.0f;
    for (i = 0; i < pIndex_count; i++) {
        total += pImpulses[pIndices[i]];
    }
    if (total <= pThreshold) {
        return 0;
    }
    pThreshold /= total;
    gINT_00692dc8 = 1;
    for (i = 0; i < pCount; i++) {
        pImpulses[i] = -pImpulses[i];
    }
    for (i = 0; i < pIndex_count; i++) {
        pImpulses[pIndices[i]] *= -pThreshold;
        if (pForces[pIndices[i]].type & 0x100) {
            BrVector3Scale(&pForces[pIndices[i]].field_0x44, &pForces[pIndices[i]].field_0x44,pThreshold);
            if (pForces[pIndices[i]].object_b != NULL) {
                BrVector3Scale(&pForces[pIndices[i]].field_0x44, &pForces[pIndices[i]].field_0x44, pThreshold);
            }
        }
    }
    for (i = 0; i < pCount; i++) {
        if (pImpulses[i] < 0.0f) {
            pImpulses[i] = 0.0f;
        }
        pForces[i].type = 9;
    }
    return 1;
    // }
    // return 0;
}

// FUNCTION: CARMA2_HW 0x004ba6b0
// FUNCTION: CARMA2_SW 0x004b6dd0
// FUNCTION: CARMA2_CHEAT 0x004c01e0
void C2_HOOK_FASTCALL DoCollisions(tPhysics_object** pObject_list, tWorld_callbacks* pCallbacks) {
    tPhysics_joint_force forces_1[40];
    tPhysics_joint_force forces_2[40];
    tPhysics_joint_force* forces;
    tPhysics_joint_force* old_forces;
    tPhysics_joint_force* tmp;
    tPhysics_object* object;
    int count;
    int loops;

    forces = forces_1;
    old_forces = forces_2;
    ClearPhysicsScratchSpace();
    gPHIL_collision_group = 1;
    gCount_friction_forces = 0;
    gPhysics_bounds_max_x = 0.0f;
    gPhysics_bounds_min_x = 0.0f;
    PrepareForCollisions(*pObject_list);
    OrderObjectListOnX(pObject_list);
    do {
        gPrepared_objects = 0;
        for (object = *pObject_list; object != NULL; object = object->next) {
            if (object->field_0x1dc || object->disable_move_rotate) {
                continue;
            }
            if (object->child != NULL && object->child->physics_joint1 != NULL && object->child->physics_joint1->type != eJoint_none) {
                SetGroup(object, gPHIL_collision_group);
                gPHIL_collision_group++;
            }
            loops = 0;
            gINT_006940a4 = 0;
            count = 0;
            for (;;) {
                count = DoInterGroupCollisions(pObject_list, object, forces, old_forces, count, pCallbacks);
                if (count == 0) {
                    break;
                }
                loops++;
                tmp = forces;
                forces = old_forces;
                old_forces = tmp;
                if (loops >= 30 || gINT_006940a4 >= 3) {
                    break;
                }
            }
            if (loops >= 30) {
                PhysicsWarning("Too many loops in DoCollisions");
            }
        }
    } while (gPrepared_objects != 0);
    PostAddFriction(pCallbacks);
    WriteNewPositions(*pObject_list);
    CheckDoingNothingFlag(*pObject_list);
    ListDoJointFriction(*pObject_list);
}

#define FORCE_RESPONSE(DIRECTION, POINT, ANGULAR, LINEAR) \
    BrVector3Cross(&tv, ANGULAR, POINT);                   \
    BrVector3Accumulate(&tv, LINEAR);                      \
    response = BrVector3Dot(DIRECTION, &tv)

inline void C2_HOOK_FASTCALL AddDoubleTorqueToMatrix(br_scalar* pMatrix, int pRows, int pIndex, tPhysics_joint_force** pForces, int pColumns) {
    br_vector3 angular_a;
    br_vector3 linear_a;
    br_vector3 partner_angular_a;
    br_vector3 partner_linear_a;
    br_vector3 angular_b;
    br_vector3 linear_b;
    br_vector3 partner_angular_b;
    br_vector3 partner_linear_b;
    tPhysics_object* object_a;
    tPhysics_object* object_b;
    tPhysics_object* partner_a;
    tPhysics_object* partner_b;
    tPhysics_joint_force* force;
    br_vector3 tv;
    br_scalar response;
    br_scalar* value;
    int j;

    force = pForces[pIndex];
    object_a = force->object_a;
    object_b = force->object_b;
    partner_a = SetUpTorqueHingePartners(&angular_a, &linear_a, &partner_angular_a, &partner_linear_a, &force->direction_a, object_a);
    if (object_b != NULL) {
        partner_b = SetUpTorqueHingePartners(&angular_b, &linear_b, &partner_angular_b, &partner_linear_b, &force->direction_b, object_b);
    }
    for (j = 0; j <= pIndex; j++) {
        value = &pMatrix[j * pRows + pIndex];
        force = pForces[j];
        if (force->type == 11) {
            if (force->object_a == object_a) {
                *value = BrVector3Dot(&force->direction_a, &angular_a);
            } else if (force->object_b == object_a) {
                *value = -BrVector3Dot(&force->direction_b, &angular_a);
            } else {
                *value = 0.0f;
            }
            if (partner_a != NULL) {
                force = pForces[j];
                if (force->object_a == partner_a) {
                    *value += BrVector3Dot(&force->direction_a, &partner_angular_a);
                } else if (force->object_b == partner_a) {
                    *value -= BrVector3Dot(&force->direction_b, &partner_angular_a);
                }
            }
            if (object_b != NULL) {
                force = pForces[j];
                if (force->object_a == object_b) {
                    *value -= BrVector3Dot(&force->direction_a, &angular_b);
                } else if (force->object_b == object_b) {
                    *value += BrVector3Dot(&force->direction_b, &angular_b);
                }
                if (partner_b != NULL) {
                    force = pForces[j];
                    if (force->object_a == partner_b) {
                        *value -= BrVector3Dot(&force->direction_a, &partner_angular_b);
                    } else if (force->object_b == partner_b) {
                        *value += BrVector3Dot(&force->direction_b, &partner_angular_b);
                    }
                }
            }
        } else {
            if (force->object_a == object_a) {
                FORCE_RESPONSE(&force->direction_a, &force->point_a, &angular_a, &linear_a);
                *value = response;
            } else if (force->object_b == object_a) {
                FORCE_RESPONSE(&force->direction_b, &force->point_b, &angular_a, &linear_a);
                *value = -response;
            } else {
                *value = 0.0f;
            }
            if (partner_a != NULL) {
                force = pForces[j];
                if (force->object_a == partner_a) {
                    FORCE_RESPONSE(&force->direction_a, &force->point_a, &partner_angular_a, &partner_linear_a);
                    *value += response;
                } else if (force->object_b == partner_a) {
                    FORCE_RESPONSE(&force->direction_b, &force->point_b, &partner_angular_a, &partner_linear_a);
                    *value -= response;
                }
            }
            if (object_b != NULL) {
                force = pForces[j];
                if (force->object_a == object_b) {
                    FORCE_RESPONSE(&force->direction_a, &force->point_a, &angular_b, &linear_b);
                    *value -= response;
                } else if (force->object_b == object_b) {
                    FORCE_RESPONSE(&force->direction_b, &force->point_b, &angular_b, &linear_b);
                    *value += response;
                }
                if (partner_b != NULL) {
                    force = pForces[j];
                    if (force->object_a == partner_b) {
                        FORCE_RESPONSE(&force->direction_a, &force->point_a, &partner_angular_b, &partner_linear_b);
                        *value -= response;
                    } else if (force->object_b == partner_b) {
                        FORCE_RESPONSE(&force->direction_b, &force->point_b, &partner_angular_b, &partner_linear_b);
                        *value += response;
                    }
                }
            }
        }
        pMatrix[pIndex * pRows + j] = *value;
    }
}

// FUNCTION: CARMA2_HW 0x004be350
// FUNCTION: CARMA2_SW 0x004baa70
// FUNCTION: CARMA2_CHEAT 0x004c3e80
void C2_HOOK_FASTCALL DRMatrix33Inverse(br_scalar* pResult, const br_scalar* pInput) {
    br_scalar inverse_determinant;

    pResult[0] = pInput[4] * pInput[8] + (-pInput[5]) * pInput[7];
    pResult[1] = -(pInput[1] * pInput[8] + (-pInput[2]) * pInput[7]);
    pResult[2] = pInput[1] * pInput[5] + (-pInput[2]) * pInput[4];
    inverse_determinant = (br_scalar)(1.0 / (pInput[0] * pResult[0] + pInput[1] * pResult[1] + pInput[2] * pResult[2]));
    pResult[0] = pResult[0] * inverse_determinant;
    pResult[3] = -(inverse_determinant * (pInput[3] * pInput[8] + (-pInput[5]) * pInput[6]));
    pResult[6] = inverse_determinant * (pInput[3] * pInput[7] + (-pInput[4]) * pInput[6]);
    pResult[1] = pResult[1] * inverse_determinant;
    pResult[4] = inverse_determinant * (pInput[0] * pInput[8] + (-pInput[2]) * pInput[6]);
    pResult[7] = -(inverse_determinant * (pInput[0] * pInput[7] + (-pInput[1]) * pInput[6]));
    pResult[2] = pResult[2] * inverse_determinant;
    pResult[5] = -(inverse_determinant * (pInput[0] * pInput[5] + (-pInput[2]) * pInput[3]));
    pResult[8] = inverse_determinant * (pInput[0] * pInput[4] + (-pInput[1]) * pInput[3]);
}

inline void C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object* pObject, tPhysics_object* pPartner, br_vector3* pAxis) {

    pObject->field_0x1e4->field_0x0 = 0;
    pObject->field_0x1e4->partner = pPartner;
}

inline void C2_HOOK_FASTCALL InitQuickHinge(tPhysics_object* pObject) {
    tPhysics_object* partner;
    tPhysics_quick_hinge* hinges;

    partner = pObject->field_0x230;
    hinges = GetPhysicsScratchPointer(2 * sizeof(tPhysics_quick_hinge));
    IncreasePhysicsScratchPointer(2 * sizeof(tPhysics_quick_hinge));
    pObject->field_0x1e4 = &hinges[0];
    partner->field_0x1e4 = &hinges[1];
    SetUpQuickHingeData(pObject, partner, &pObject->physics_joint1->hinge_axis);
    SetUpQuickHingeData(partner, pObject, &pObject->physics_joint1->parent_bone_axis);
}

inline void C2_HOOK_FASTCALL CalcQuickHingeData(tPhysics_object* pObject) {
    tPhysics_object* partner;
    br_matrix34 inverse;

    partner = pObject->field_0x1e4->partner;
    BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
    BrMatrix34Mul(&pObject->field_0x1e4->field_0x38, &partner->transform_matrix, &inverse);
    BrMatrix34LPInverse(&partner->field_0x1e4->field_0x38, &pObject->field_0x1e4->field_0x38);
    pObject->field_0x1e4->field_0x0 = 1;
    partner->field_0x1e4->field_0x0 = 1;
}

// FUNCTION: CARMA2_HW 0x004be4b0
// FUNCTION: CARMA2_SW 0x004babd0
// FUNCTION: CARMA2_CHEAT 0x004c3fe0
void C2_HOOK_FASTCALL SetUpForce(br_vector3* pAngular, br_vector3* pLinear, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject) {
    br_scalar inverse_mass;
    int i;

    if (!(pObject->flags & 0x800)) {
        BrVector3Cross(pAngular, pPoint, pDirection);
        for (i = 0; i < 3; i++) {
            pAngular->v[i] /= pObject->I.v[i];
        }
    } else {
        BrVector3Set(pAngular, 0.0f, 0.0f, 0.f);
    }
    inverse_mass = (br_scalar)(1.0 / pObject->M);
    BrVector3Scale(pLinear, pDirection, inverse_mass);
}

// FUNCTION: CARMA2_HW 0x004be550
// FUNCTION: CARMA2_SW 0x004bac70
// FUNCTION: CARMA2_CHEAT 0x004c4080
tPhysics_object* C2_HOOK_FASTCALL SetUpTorqueHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, const br_vector3* pTorque, tPhysics_object* pObject) {
    tPhysics_object* partner;
    tPhysics_joint* joint;
    br_vector3 lever;
    br_vector3 cross;
    br_vector3 rhs;
    br_vector3 response;
    br_vector3 rotated;
    br_vector3 force;
    br_vector3 temp;
    br_scalar factor;
    int i;

    if (!(pObject->flags & 0x800)) {
        for (i = 0; i < 3; i++) {
            pAngular->v[i] = pTorque->v[i] / pObject->I.v[i];
        }
    } else {
        BrVector3Set(pAngular, 0.0f, 0.0f, 0.0f);
    }
    if (pObject->field_0x1e4 == NULL) {
        BrVector3SetFloat(pLinear, 0.0f, 0.0f, 0.0f);
        return NULL;
    }
    partner = pObject->field_0x1e4->partner;
    if (partner == pObject->field_0x230) {
        joint = pObject->physics_joint1;
        BrVector3Sub(&lever, &joint->field_0x08, &pObject->cmpos);
        BrVector3Cross(&cross, pAngular, &lever);
        rhs.v[0] = -BrVector3Dot(&cross, &joint->hinge_axis);
        rhs.v[1] = -BrVector3Dot(&joint->hinge_axis2, pAngular);
        rhs.v[2] = -BrVector3Dot(&joint->hinge_axis3, pAngular);
        BrMatrix34ApplyV(&response, &rhs, &pObject->field_0x1e4->field_0x8);
        factor = response.v[0] / pObject->M;
        BrVector3Scale(pLinear, &joint->hinge_axis, factor);
        factor = -(response.v[0] / partner->M);
        BrVector3Scale(pPartner_linear, &joint->parent_bone_axis, factor);
        for (i = 0; i < 3; i++) {
            rotated.v[i] = joint->hinge_axis2.v[i] * response.v[1] + joint->hinge_axis3.v[i] * response.v[2];
        }
        BrVector3Scale(&force, &joint->hinge_axis, response.v[0]);
        BrVector3Cross(&temp, &lever, &force);
        BrVector3Accumulate(&temp, &rotated);
        for (i = 0; i < 3; i++) {
            pAngular->v[i] += temp.v[i] / pObject->I.v[i];
        }
        BrMatrix34ApplyV(&temp, &rotated, &pObject->transform_matrix);
        BrMatrix34TApplyV(pPartner_angular, &temp, &partner->transform_matrix);
        BrVector3Scale(&force, &joint->parent_bone_axis, response.v[0]);
        BrVector3Sub(&lever, &joint->field_0x14, &partner->cmpos);
        BrVector3Cross(&temp, &lever, &force);
        BrVector3Accumulate(pPartner_angular, &temp);
        for (i = 0; i < 3; i++) {
            pPartner_angular->v[i] = -(pPartner_angular->v[i] / partner->I.v[i]);
        }
        return partner;
    }
    joint = partner->physics_joint1;
    BrVector3Sub(&lever, &joint->field_0x14, &pObject->cmpos);
    BrVector3Cross(&cross, pAngular, &lever);
    rhs.v[0] = BrVector3Dot(&cross, &joint->parent_bone_axis);
    BrMatrix34ApplyV(&rotated, pAngular, &pObject->transform_matrix);
    BrMatrix34TApplyV(&temp, &rotated, &partner->transform_matrix);
    rhs.v[1] = BrVector3Dot(&joint->hinge_axis2, &temp);
    rhs.v[2] = BrVector3Dot(&joint->hinge_axis3, &temp);
    BrMatrix34ApplyV(&response, &rhs, &partner->field_0x1e4->field_0x8);
    factor = response.v[0] / partner->M;
    BrVector3Scale(pPartner_linear, &joint->hinge_axis, factor);
    factor = -(response.v[0] / pObject->M);
    BrVector3Scale(pLinear, &joint->parent_bone_axis, factor);
    for (i = 0; i < 3; i++) {
        pPartner_angular->v[i] = joint->hinge_axis2.v[i] * response.v[1] + joint->hinge_axis3.v[i] * response.v[2];
    }
    BrMatrix34ApplyV(&temp, pPartner_angular, &partner->transform_matrix);
    BrMatrix34TApplyV(&rotated, &temp, &pObject->transform_matrix);
    BrVector3Scale(&force, &joint->parent_bone_axis, response.v[0]);
    BrVector3Cross(&temp, &lever, &force);
    BrVector3Accumulate(&rotated, &temp);
    BrVector3Scale(&force, &joint->hinge_axis, response.v[0]);
    BrVector3Sub(&lever, &joint->field_0x08, &partner->cmpos);
    BrVector3Cross(&temp, &lever, &force);
    BrVector3Accumulate(pPartner_angular, &temp);
    for (i = 0; i < 3; i++) {
        pAngular->v[i] -= rotated.v[i] / pObject->I.v[i];
    }
    for (i = 0; i < 3; i++) {
        pPartner_angular->v[i] /= partner->I.v[i];
    }
    return partner;
}

// FUNCTION: CARMA2_HW 0x004becf0
// FUNCTION: CARMA2_SW 0x004bb400
// FUNCTION: CARMA2_CHEAT 0x004c4820
tPhysics_object* C2_HOOK_FASTCALL SetUpHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, tPhysics_object* pObject) {
    tPhysics_object* partner;
    tPhysics_joint* joint;
    br_vector3 lever;
    br_vector3 velocity;
    br_vector3 rhs;
    br_vector3 response;
    br_vector3 rotated;
    br_vector3 force;
    br_vector3 temp;
    br_scalar factor;
    int i;

    partner = pObject->field_0x1e4->partner;
    if (partner == pObject->field_0x230) {
        joint = pObject->physics_joint1;
        BrVector3Sub(&lever, &joint->field_0x08, &pObject->cmpos);
        BrVector3Cross(&velocity, pAngular, &lever);
        BrVector3Accumulate(&velocity, pLinear);
        rhs.v[0] = -BrVector3Dot(&joint->hinge_axis, &velocity);
        rhs.v[1] = -BrVector3Dot(&joint->hinge_axis2, pAngular);
        rhs.v[2] = -BrVector3Dot(&joint->hinge_axis3, pAngular);
        BrMatrix34ApplyV(&response, &rhs, &pObject->field_0x1e4->field_0x8);
        factor = response.v[0] / pObject->M;
        for (i = 0; i < 3; i++) {
            pLinear->v[i] += joint->hinge_axis.v[i] * factor;
        }
        factor = -(response.v[0] / partner->M);
        BrVector3Scale(pPartner_linear, &joint->parent_bone_axis, factor);
        for (i = 0; i < 3; i++) {
            rotated.v[i] = joint->hinge_axis2.v[i] * response.v[1] + joint->hinge_axis3.v[i] * response.v[2];
        }
        BrVector3Scale(&force, &joint->hinge_axis, response.v[0]);
        BrVector3Cross(&temp, &lever, &force);
        BrVector3Accumulate(&temp, &rotated);
        for (i = 0; i < 3; i++) {
            pAngular->v[i] += temp.v[i] / pObject->I.v[i];
        }
        BrMatrix34ApplyV(&temp, &rotated, &pObject->transform_matrix);
        BrMatrix34TApplyV(pPartner_angular, &temp, &partner->transform_matrix);
        BrVector3Scale(&force, &joint->parent_bone_axis, response.v[0]);
        BrVector3Sub(&lever, &joint->field_0x14, &partner->cmpos);
        BrVector3Cross(&temp, &lever, &force);
        BrVector3Accumulate(pPartner_angular, &temp);
        for (i = 0; i < 3; i++) {
            pPartner_angular->v[i] = -(pPartner_angular->v[i] / partner->I.v[i]);
        }
        return partner;
    }
    joint = partner->physics_joint1;
    BrVector3Sub(&lever, &joint->field_0x14, &pObject->cmpos);
    BrVector3Cross(&velocity, pAngular, &lever);
    BrVector3Accumulate(&velocity, pLinear);
    rhs.v[0] = BrVector3Dot(&joint->parent_bone_axis, &velocity);
    BrMatrix34ApplyV(&rotated, pAngular, &pObject->transform_matrix);
    BrMatrix34TApplyV(&temp, &rotated, &partner->transform_matrix);
    rhs.v[1] = BrVector3Dot(&joint->hinge_axis2, &temp);
    rhs.v[2] = BrVector3Dot(&joint->hinge_axis3, &temp);
    BrMatrix34ApplyV(&response, &rhs, &partner->field_0x1e4->field_0x8);
    factor = response.v[0] / partner->M;
    BrVector3Scale(pPartner_linear, &joint->hinge_axis, factor);
    factor = -(response.v[0] / pObject->M);
    for (i = 0; i < 3; i++) {
        pLinear->v[i] += joint->parent_bone_axis.v[i] * factor;
    }
    for (i = 0; i < 3; i++) {
        pPartner_angular->v[i] = joint->hinge_axis2.v[i] * response.v[1] + joint->hinge_axis3.v[i] * response.v[2];
    }
    BrMatrix34ApplyV(&temp, pPartner_angular, &partner->transform_matrix);
    BrMatrix34TApplyV(&rotated, &temp, &pObject->transform_matrix);
    BrVector3Scale(&force, &joint->parent_bone_axis, response.v[0]);
    BrVector3Cross(&temp, &lever, &force);
    BrVector3Accumulate(&rotated, &temp);
    BrVector3Scale(&force, &joint->hinge_axis, response.v[0]);
    BrVector3Sub(&lever, &joint->field_0x08, &partner->cmpos);
    BrVector3Cross(&temp, &lever, &force);
    BrVector3Accumulate(pPartner_angular, &temp);
    for (i = 0; i < 3; i++) {
        pAngular->v[i] -= rotated.v[i] / pObject->I.v[i];
    }
    for (i = 0; i < 3; i++) {
        pPartner_angular->v[i] /= partner->I.v[i];
    }
    return partner;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004bf490
// FUNCTION: CARMA2_SW 0x004bbbb0
// FUNCTION: CARMA2_CHEAT 0x004c4fc0
void C2_HOOK_FASTCALL MakeMatrix(br_scalar* pMatrix, int pRows, tPhysics_joint_force** pForces, int pColumns) {
    br_vector3 angular_a;
    br_vector3 linear_a;
    br_vector3 partner_angular_a;
    br_vector3 partner_linear_a;
    br_vector3 angular_b;
    br_vector3 linear_b;
    br_vector3 partner_angular_b;
    br_vector3 partner_linear_b;
    tPhysics_object* object_a;
    tPhysics_object* object_b;
    tPhysics_object* partner_a;
    tPhysics_object* partner_b;
    tPhysics_joint_force* force;
    br_vector3 tv;
    br_scalar response;
    br_scalar* value;
    int i;
    int j;

    for (i = 0; i < pColumns; i++) {
        force = pForces[i];
        object_b = force->object_b;
        object_a = force->object_a;
        if (object_b != NULL && force->type == 14) {
            object_b = NULL;
        }
        if (force->type == 11) {
            AddDoubleTorqueToMatrix(pMatrix, pRows, i, pForces, pColumns);
            continue;
        }
        SetUpForce(&angular_a, &linear_a, &force->point_a, &force->direction_a, object_a);
        if (object_a->field_0x1e4 != NULL) {
            partner_a = SetUpHingePartners(&angular_a, &linear_a, &partner_angular_a, &partner_linear_a, object_a);
        } else {
            partner_a = NULL;
        }
        partner_b = NULL;
        if (object_b != NULL) {
            SetUpForce(&angular_b, &linear_b, &force->point_b, &force->direction_b, object_b);
            if (object_b->field_0x1e4 != NULL) {
                partner_b = SetUpHingePartners(&angular_b, &linear_b, &partner_angular_b, &partner_linear_b, object_b);
            }
        }
        for (j = 0; j <= i; j++) {
            value = &pMatrix[j * pRows + i];
            *value = 0.0f;
            force = pForces[j];
            if (force->type == 11) {
                if (force->object_a == object_a) {
                    *value += BrVector3Dot(&force->direction_a, &angular_a);
                } else if (force->object_b == object_a) {
                    *value -= BrVector3Dot(&force->direction_b, &angular_a);
                }
                if (object_b != NULL) {
                    if (force->object_a == object_b) {
                        *value -= BrVector3Dot(&force->direction_a, &angular_b);
                    } else if (force->object_b == object_b) {
                        *value += BrVector3Dot(&force->direction_b, &angular_b);
                    }
                }
                if (partner_a != NULL) {
                    if (force->object_a == partner_a) {
                        *value += BrVector3Dot(&force->direction_a, &partner_angular_a);
                    } else if (force->object_b == partner_a) {
                        *value -= BrVector3Dot(&force->direction_b, &partner_angular_a);
                    }
                }
                if (partner_b != NULL) {
                    if (force->object_a == partner_b) {
                        *value -= BrVector3Dot(&force->direction_a, &partner_angular_b);
                    } else if (force->object_b == partner_b) {
                        *value += BrVector3Dot(&force->direction_b, &partner_angular_b);
                    }
                }
            } else {
                if (force->object_a == object_a) {
                    FORCE_RESPONSE(&force->direction_a, &force->point_a, &angular_a, &linear_a);
                    *value += response;
                } else if (force->object_b == object_a && force->type != 14) {
                    FORCE_RESPONSE(&force->direction_b, &force->point_b, &angular_a, &linear_a);
                    *value -= response;
                }
                if (object_b != NULL) {
                    if (force->object_a == object_b) {
                        FORCE_RESPONSE(&force->direction_a, &force->point_a, &angular_b, &linear_b);
                        *value -= response;
                    } else if (force->object_b == object_b && force->type != 14) {
                        FORCE_RESPONSE(&force->direction_b, &force->point_b, &angular_b, &linear_b);
                        *value += response;
                    }
                }
                if (partner_a != NULL) {
                    if (force->object_a == partner_a) {
                        FORCE_RESPONSE(&force->direction_a, &force->point_a, &partner_angular_a, &partner_linear_a);
                        *value += response;
                    } else if (force->object_b == partner_a) {
                        FORCE_RESPONSE(&force->direction_b, &force->point_b, &partner_angular_a, &partner_linear_a);
                        *value -= response;
                    }
                }
                if (partner_b != NULL) {
                    if (force->object_a == partner_b) {
                        FORCE_RESPONSE(&force->direction_a, &force->point_a, &partner_angular_b, &partner_linear_b);
                        *value -= response;
                    } else if (force->object_b == partner_b) {
                        FORCE_RESPONSE(&force->direction_b, &force->point_b, &partner_angular_b, &partner_linear_b);
                        *value += response;
                    }
                }
            }
            pMatrix[i * pRows + j] = *value;
        }
    }
}

#undef FORCE_RESPONSE

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c03f0
void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, rotate_omega, 0x1c0);

    while (pObject != NULL) {
        BrVector3Copy(&pObject->omega, &pObject->rotate_omega);
        if (pObject->child != NULL) {
            RestoreListOmega(pObject->child);
        }
        pObject = pObject->next;
    }
}

// RestoreGroupOmegas

// MakeVelocitiesVector2

// MakeVelocitiesVector

inline void C2_HOOK_FASTCALL NewAddFrictionObjectObject(tPhysics_joint_force* pForce) {
    tPhysics_object* object_a;
    tPhysics_object* object_b;
    br_vector3 velocity;
    br_vector3 velocity_b;
    br_vector3 normal;
    br_vector3 direction_a;
    br_vector3 direction_b;
    br_vector3 angular_a;
    br_vector3 angular_b;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar friction;
    br_scalar max_impulse;
    br_scalar speed;
    br_scalar d;
    br_scalar impulse;

    object_a = pForce->object_a;
    friction = pForce->field_0x0;
    object_b = pForce->object_b;
    object_a->collision_flag |= 0x2;
    object_b->collision_flag |= 0x2;
    BrVector3Cross(&tv, &object_b->omega, &pForce->point_b);
    BrVector3Add(&velocity_b, &tv, &object_b->velocity_car_space);
    BrMatrix34ApplyV(&tv, &velocity_b, &object_b->actor->t.t.mat);
    BrMatrix34TApplyV(&velocity_b, &tv, &object_a->actor->t.t.mat);
    BrVector3Cross(&tv, &object_a->omega, &pForce->point_a);
    BrVector3Accumulate(&tv, &object_a->velocity_car_space);
    BrVector3Sub(&velocity, &tv, &velocity_b);
    BrMatrix34ApplyV(&tv, &pForce->direction_a, &object_a->transform_matrix);
    BrMatrix34TApplyV(&normal, &tv, &object_a->actor->t.t.mat);
    d = BrVector3Dot(&velocity, &normal);
    BrVector3Scale(&tv, &normal, d);
    BrVector3Sub(&velocity, &velocity, &tv);
    max_impulse = 0.35f * friction * object_a->object_friction + 0.35f * friction;
    max_impulse = max_impulse * object_b->object_friction + max_impulse;
    speed = BrVector3Length(&velocity);
    if (speed < 0.001f) {
        return;
    }
    BrVector3Scale(&direction_a, &velocity, 1.0f / -speed);
    BrMatrix34ApplyV(&tv, &direction_a, &object_a->actor->t.t.mat);
    BrMatrix34TApplyV(&direction_b, &tv, &object_b->actor->t.t.mat);
    if (!(object_a->flags & 0x800)) {
        BrVector3Cross(&tv, &pForce->point_a, &direction_a);
        Vector3Div(&angular_a, &tv, &object_a->I);
    } else {
        BrVector3Set(&angular_a, 0.0f, 0.0f, 0.0f);
    }
    if (!(object_b->flags & 0x800)) {
        BrVector3Cross(&tv, &pForce->point_b, &direction_b);
        Vector3Div(&angular_b, &tv, &object_b->I);
    } else {
        BrVector3Set(&angular_b, 0.0f, 0.0f, 0.0f);
    }
    BrVector3Cross(&tv, &angular_b, &pForce->point_b);
    BrVector3Cross(&tv2, &angular_a, &pForce->point_a);
    d = 1.0f / object_a->M + 1.0f / object_b->M + BrVector3Dot(&tv2, &direction_a) + BrVector3Dot(&tv, &direction_b);
    if (fabs(d) <= 0.0001f) {
        return;
    }
    impulse = speed / d;
    if (impulse > max_impulse) {
        impulse = max_impulse;
    }
    BrVector3Scale(&direction_a, &direction_a, impulse / object_a->M);
    BrVector3Scale(&direction_b, &direction_b, -(impulse / object_b->M));
    BrVector3Scale(&pForce->field_0x44, &direction_a, object_a->M);
    BrVector3Scale(&pForce->field_0x50, &direction_b, object_b->M);
    pForce->type |= 0x100;
    BrVector3Scale(&angular_a, &angular_a, impulse);
    BrVector3Accumulate(&object_a->omega, &angular_a);
    BrVector3Scale(&angular_b, &angular_b, -impulse);
    BrVector3Accumulate(&object_b->omega, &angular_b);
    BrMatrix34ApplyV(&tv, &direction_a, &object_a->actor->t.t.mat);
    BrVector3Accumulate(&object_a->v, &tv);
    BrVector3Accumulate(&object_a->velocity_car_space, &direction_a);
    BrMatrix34ApplyV(&tv, &direction_b, &object_b->actor->t.t.mat);
    BrVector3Accumulate(&object_b->v, &tv);
    BrVector3Accumulate(&object_b->velocity_car_space, &direction_b);
}

// FUNCTION: CARMA2_HW 0x004c0ac0
// FUNCTION: CARMA2_SW 0x004bd1f0
// FUNCTION: CARMA2_CHEAT 0x004c65e0
void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* pForce, tWorld_callbacks* pCallbacks) {
    tPhysics_object* object;
    br_vector3 point;
    br_vector3 velocity;
    br_vector3 normal;
    br_vector3 direction;
    br_vector3 angular;
    br_vector3 tv;
    br_scalar friction;
    br_scalar max_impulse;
    br_scalar speed;
    br_scalar d;
    br_scalar impulse;

    object = pForce->object_a;
    if (pForce->object_b != NULL && pForce->type != 14) {
        NewAddFrictionObjectObject(pForce);
        CapOmega(pForce->object_a);
        CapOmega(pForce->object_b);
        return;
    }
    friction = pForce->field_0x0;
    object->collision_flag |= 0x1;
    if (pForce->type == 1) {
        BrVector3Scale(&point, &pForce->point_a, -0.005f);
        BrVector3Accumulate(&point, &pForce->point_a);
        BrVector3Cross(&velocity, &object->omega, &point);
    } else {
        BrVector3Cross(&velocity, &object->omega, &pForce->point_a);
    }
    BrVector3Accumulate(&velocity, &object->velocity_car_space);
    BrMatrix34ApplyV(&tv, &pForce->direction_a, &object->transform_matrix);
    BrMatrix34TApplyV(&normal, &tv, &object->actor->t.t.mat);
    d = BrVector3Dot(&velocity, &normal);
    BrVector3Scale(&tv, &normal, d);
    BrVector3Sub(&velocity, &velocity, &tv);
    if (pCallbacks != NULL && pCallbacks->get_friction_from_face != NULL && pForce->field_0xc != NULL) {
        friction = pCallbacks->get_friction_from_face(pForce->field_0xc) * friction;
    }
    max_impulse = 0.35f * friction * object->world_friction + 0.35f * friction;
    speed = BrVector3Length(&velocity);
    if (speed < 0.001f) {
        return;
    }
    BrVector3Scale(&direction, &velocity, 1.0f / -speed);
    if (!(object->flags & 0x800)) {
        BrVector3Cross(&tv, &pForce->point_a, &direction);
        Vector3Div(&angular, &tv, &object->I);
    } else {
        BrVector3Set(&angular, 0.0f, 0.0f, 0.0f);
    }
    d = BrVector3Dot(&object->omega, &pForce->direction_a);
    if (fabs(d) < 0.1f && BrVector3Dot(&angular, &object->omega) < 0.0f && BrVector3LengthSquared(&object->v) < 0.001) {
        BrVector3Scale(&tv, &pForce->direction_a, d);
        BrVector3Sub(&object->omega, &object->omega, &tv);
        BrVector3SetFloat(&angular, 0.0f, 0.0f, 0.0f);
        speed = BrVector3Length(&object->velocity_car_space);
        if (speed < 0.001f) {
            return;
        }
        BrVector3InvScale(&direction, &object->velocity_car_space, -speed);
    }
    BrVector3Cross(&tv, &angular, &pForce->point_a);
    d = 1.0f / object->M + BrVector3Dot(&tv, &direction);
    if (fabs(d) <= 0.0001f) {
        return;
    }
    impulse = speed / d;
    if (impulse > max_impulse) {
        impulse = max_impulse;
    }
    BrVector3Scale(&angular, &angular, impulse);
    BrVector3Scale(&direction, &direction, impulse / object->M);
    BrVector3Scale(&pForce->field_0x44, &direction, object->M);
    pForce->type |= 0x100;
    BrVector3Accumulate(&object->omega, &angular);
    BrMatrix34ApplyV(&tv, &direction, &object->actor->t.t.mat);
    BrVector3Accumulate(&object->v, &tv);
    BrVector3Accumulate(&object->velocity_car_space, &direction);
    CapOmega(object);
}

// ApplyForces

// FUNCTION: CARMA2_HW 0x004c18d0
// FUNCTION: CARMA2_SW 0x004be000
// FUNCTION: CARMA2_CHEAT 0x004c73f0
void C2_HOOK_FASTCALL DoJointFriction(tPhysics_object* pObject) {
    br_scalar total_sq;
    br_vector3 impulse;
    int j;
    br_vector3 tv;
    int i;
    br_scalar max_impulse_sq;
    br_scalar s;
    tPhysics_object* child;
    br_vector3 rel[3];

    if (pObject->physics_joint1 != NULL && pObject->physics_joint1->friction != 0.0f && pObject->field_0x230 != NULL) {
        total_sq = 0.0f;
        max_impulse_sq = pObject->physics_joint1->friction * 0.04f;
        max_impulse_sq = max_impulse_sq * max_impulse_sq;
        for (i = 0; i < 3; i++) {
            BrMatrix34TApplyV(&tv, (br_vector3*)pObject->transform_matrix.m[i], &pObject->field_0x230->transform_matrix);
            impulse.v[i] = BrVector3Dot(&tv, &pObject->field_0x230->omega) - pObject->omega.v[i];
            for (j = 0; j < 3; j++) {
                rel[i].v[j] = tv.v[j] / pObject->field_0x230->I.v[j];
            }
            impulse.v[i] /= (br_scalar)(1.0 / pObject->I.v[i] + BrVector3Dot(&rel[i], &tv));
            total_sq += impulse.v[i] * impulse.v[i];
        }
        if (total_sq > max_impulse_sq) {
            s = (br_scalar)sqrt(max_impulse_sq / total_sq);
            BrVector3Scale(&impulse, &impulse, s);
        }
        for (i = 0; i < 3; i++) {
            pObject->omega.v[i] += impulse.v[i] / pObject->I.v[i];
            BrVector3Scale(&rel[i], &rel[i], -impulse.v[i]);
            BrVector3Accumulate(&pObject->field_0x230->omega, &rel[i]);
        }
    }
    if (pObject->physics_joint2 != NULL && pObject->physics_joint2->friction != 0.0f) {
        max_impulse_sq = pObject->physics_joint2->friction * 0.04f;
        total_sq = 0.0f;
        max_impulse_sq = max_impulse_sq * max_impulse_sq;
        for (i = 0; i < 3; i++) {
            impulse.v[i] = pObject->omega.v[i] * pObject->I.v[i];
            total_sq += impulse.v[i] * impulse.v[i];
        }
        if (total_sq > max_impulse_sq) {
            s = (br_scalar)sqrt(max_impulse_sq / total_sq);
            BrVector3Scale(&impulse, &impulse, s);
        }
        for (i = 0; i < 3; i++) {
            pObject->omega.v[i] -= impulse.v[i] / pObject->I.v[i];
        }
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        if (!child->disable_move_rotate) {
            DoJointFriction(child);
        }
    }
}

inline void ListDoJointFriction(tPhysics_object* pObject) {

    for (; pObject != NULL; pObject = pObject->next) {
        if (!pObject->disable_move_rotate) {
            DoJointFriction(pObject);
        }
    }
}

inline void PostAddFriction(tWorld_callbacks* pCallbacks) {
    int i;

    for (i = 0; i < gCount_friction_forces; i++) {
        NewAddFriction(&gFriction_forces[i], pCallbacks);
    }
}

// RotateObjectSecondOrder

// FUNCTION: CARMA2_HW 0x004c1f10
void C2_HOOK_FASTCALL RotateObjectFirstOrder(tPhysics_object* pObject, br_scalar pTime) {
    br_matrix34 rotation;
    br_vector3 axis;
    br_vector3 momentum;
    br_vector3 rotated;
    br_scalar length;
    br_scalar angle;

    length = BrVector3Length(&pObject->omega);
    angle = length * pTime;
    if (angle < 0.0001) {
        return;
    }
    axis.v[0] = pObject->omega.v[0] / length;
    axis.v[1] = pObject->omega.v[1] / length;
    axis.v[2] = pObject->omega.v[2] / length;
    momentum.v[0] = pObject->I.v[0] * pObject->omega.v[0];
    momentum.v[1] = pObject->I.v[1] * pObject->omega.v[1];
    momentum.v[2] = pObject->I.v[2] * pObject->omega.v[2];
    BrMatrix34Rotate(&rotation, BrRadianToAngle(angle), &axis);
    BrMatrix34TApplyV(&rotated, &momentum, &rotation);
    BrMatrix34PreTranslate(&rotation, -pObject->cmpos.v[0], -pObject->cmpos.v[1], -pObject->cmpos.v[2]);
    BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
    BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
    pObject->omega.v[0] = rotated.v[0] / pObject->I.v[0];
    pObject->omega.v[1] = rotated.v[1] / pObject->I.v[1];
    pObject->omega.v[2] = rotated.v[2] / pObject->I.v[2];
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004c1be0
// STUB: CARMA2_SW 0x004be310
// STUB: CARMA2_CHEAT 0x004c7700
void C2_HOOK_FASTCALL RotateObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    br_vector3 tv;

    BrVector3Scale(&tv, &pObject->v, pDelta_time);
    BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004c2060
// STUB: CARMA2_SW 0x004be790
// STUB: CARMA2_CHEAT 0x004c7b80
void C2_HOOK_FASTCALL GetFacesNearObject(tPhysics_object *pObject, tWorld_callbacks *pCallbacks) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c25a0
// FUNCTION: CARMA2_SW 0x004becd0
// FUNCTION: CARMA2_CHEAT 0x004c80c0
void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject) {
    tPhysics_object* child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xbc, 0xbc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xf0, 0xf0);

    if (pObject->field_0xf0 == 1) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, &pObject->field_0xbc);
    } else {
        BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        ResetObjectAndChildren(child);
    }
}

// FUNCTION: CARMA2_HW 0x004c2600
// FUNCTION: CARMA2_SW 0x004bed30
// FUNCTION: CARMA2_CHEAT 0x004c8120
void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects) {

    while (pObjects != NULL) {
        ResetObjectAndChildren(pObjects);
        pObjects = pObjects->next;
    }
}

// RepositionObjectGroup

// SimpleRotate

// InterpolateSingleObject

// InterpolateObjects

// FUNCTION: CARMA2_HW 0x004c2910
// FUNCTION: CARMA2_SW 0x004bf040
void C2_HOOK_FASTCALL ApplyGravitySingleObject(tPhysics_object* pObject) {

    if (!pObject->disable_move_rotate) {
        BrVector3Accumulate(&pObject->v, &pObject->field_0x54);
    }
    if (pObject->child != NULL) {
        ApplyGravity(pObject->child);
    }
}

// FUNCTION: CARMA2_HW 0x004c2970
// FUNCTION: CARMA2_SW 0x004bf0a0
// FUNCTION: CARMA2_CHEAT 0x004c8490
void C2_HOOK_FASTCALL ApplyGravity(tPhysics_object* pObject) {

    for (; pObject != NULL; pObject = pObject->next) {
        ApplyGravitySingleObject(pObject);
    }
}

// FUNCTION: CARMA2_HW 0x004c29d0
// FUNCTION: CARMA2_SW 0x004bf100
void C2_HOOK_FASTCALL SetGravityDeltaV(tPhysics_object* pObject, br_vector3* pImpulse) {
    tPhysics_object* pChild;

    pObject->field_0x54 = *pImpulse;
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        SetGravityDeltaV(pChild, pImpulse);
    }
}

// FUNCTION: CARMA2_HW 0x004c2a10
// FUNCTION: CARMA2_SW 0x004bf140
// FUNCTION: CARMA2_CHEAT 0x004c8530
void C2_HOOK_FASTCALL TrialPositionChildren(tPhysics_object* pObject) {
    tPhysics_object* child;
    tPhysics_object* root;
    br_vector3 parent_point;
    br_vector3 child_point;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1e8, 0x1e8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x230, 0x230);

    root = pObject;
    child = pObject->child;
    if (pObject->flags & 0x400) {
        root = pObject->field_0x230;
    }
    for (; child != NULL; child = child->next) {
        if (!(pObject->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pObject->actor->t.t.mat);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->actor->t.t.mat);
            BrVector3Sub(&parent_point, &parent_point, &child_point);
            BrVector3Accumulate((br_vector3*)child->actor->t.t.mat.m[3], &parent_point);
        } else {
            BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->field_0x1e8, &root->actor->t.t.mat);
        }
        TrialPositionChildren(child);
    }
}

// FUNCTION: CARMA2_HW 0x004c2b20
// FUNCTION: CARMA2_SW 0x004bf250
// FUNCTION: CARMA2_CHEAT 0x004c8640
void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot) {
    tPhysics_object* child;
    br_vector3 parent_point;
    br_vector3 child_point;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1e8, 0x1e8);

    child = pParent->child;
    if (!(pParent->flags & 0x400)) {
        pRoot = pParent;
    }
    for (; child != NULL; child = child->next) {
        if (!(pParent->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pParent->transform_matrix);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->transform_matrix);
            BrVector3Sub(&parent_point, &parent_point, &child_point);
            BrVector3Accumulate((br_vector3*)child->transform_matrix.m[3], &parent_point);
        } else {
            BrMatrix34Mul(&pParent->transform_matrix, &pParent->field_0x1e8, &pRoot->transform_matrix);
        }
        InternalPositionChildren(child, pRoot);
    }
}

// FUNCTION: CARMA2_HW 0x004c2b10
// FUNCTION: CARMA2_SW 0x004bf240
// FUNCTION: CARMA2_CHEAT 0x004c8630
void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info) {

    InternalPositionChildren(pCollision_info, pCollision_info);
}

// FUNCTION: CARMA2_HW 0x004c2c30
// FUNCTION: CARMA2_SW 0x004bf360
void C2_HOOK_FASTCALL ApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque) {
    br_vector3 torque;
    br_vector3 inertia;

    BrVector3Scale(&torque, pTorque, 1.0f / REC2_SQR(WORLD_SCALE));
    BrVector3Copy(&inertia, &pObject->I);
    pObject->omega.v[0] += torque.v[0] / inertia.v[0];
    pObject->omega.v[1] += torque.v[1] / inertia.v[1];
    pObject->omega.v[2] += torque.v[2] / inertia.v[2];
}

// FUNCTION: CARMA2_HW 0x004c2c90
// FUNCTION: CARMA2_SW 0x004bf3c0
// FUNCTION: CARMA2_CHEAT 0x004c87b0
void C2_HOOK_FASTCALL ApplyDiminishingTorque(tPhysics_object* pObject, const br_vector3* pV) {
    br_vector3 original_omega;
    br_vector3 tv;
    br_vector3 inertia;

    original_omega = pObject->omega;
    BrVector3Scale(&tv, pV, (br_scalar)(1.0 / REC2_SQR(WORLD_SCALE_D)));
    BrVector3Copy(&inertia, &pObject->I);
    pObject->omega.v[0] -= tv.v[0] / inertia.v[0];
    pObject->omega.v[1] -= tv.v[1] / inertia.v[1];
    pObject->omega.v[2] -= tv.v[2] / inertia.v[2];
    if (original_omega.v[0] * pObject->omega.v[0] <= 0.0f) {
        pObject->omega.v[0] = 0.0f;
    }
    if (original_omega.v[1] * pObject->omega.v[1] <= 0.0f) {
        pObject->omega.v[1] = 0.0f;
    }
    if (original_omega.v[2] * pObject->omega.v[2] <= 0.0f) {
        pObject->omega.v[2] = 0.0f;
    }
}

inline void OrderObjectListOnX(tPhysics_object** pObject_list) {
    tPhysics_object* first;
    tPhysics_object* last;
    tPhysics_object* current;
    tPhysics_object* position;
    tPhysics_object* next;

    first = *pObject_list;
    first->prev = NULL;
    last = first;
    current = first->next;
    while (current != NULL) {
        position = last;
        while (position != NULL && current->transform_matrix.m[3][0] < position->transform_matrix.m[3][0]) {
            position = position->prev;
        }
        if (position == last) {
            position->next = current;
            current->prev = position;
            last = current;
            current = current->next;
        } else if (position == NULL) {
            first->prev = current;
            next = current->next;
            current->next = first;
            current->prev = NULL;
            first = current;
            current = next;
        } else {
            next = current->next;
            current->next = position->next;
            current->next->prev = current;
            position->next = current;
            current->prev = position;
            current = next;
        }
    }
    last->next = NULL;
    *pObject_list = first;
}

inline void C2_HOOK_FASTCALL SwapPair(tPhysics_object* pObject_1, tPhysics_object* pObject_2) {
    tPhysics_object* prev;
    tPhysics_object* next;

    prev = pObject_1->prev;
    next = pObject_2->next;
    if (prev != NULL) {
        prev->next = pObject_2;
    }
    if (next != NULL) {
        next->prev = pObject_1;
    }
    pObject_1->prev = pObject_2;
    pObject_2->prev = prev;
    pObject_1->next = next;
    pObject_2->next = pObject_1;
}

inline void C2_HOOK_FASTCALL CheckObjectsPostionInList(tPhysics_object* pObject, tPhysics_object** pList) {
    tPhysics_object* other;

    other = pObject->prev;
    if (other != NULL && pObject->transform_matrix.m[3][0] < other->transform_matrix.m[3][0]) {
        do {
            SwapPair(other, pObject);
            other = pObject->prev;
        } while (other != NULL && pObject->transform_matrix.m[3][0] < other->transform_matrix.m[3][0]);
        if (other == NULL) {
            *pList = pObject;
        }
    } else {
        other = pObject->next;
        while (other != NULL && other->transform_matrix.m[3][0] < pObject->transform_matrix.m[3][0]) {
            SwapPair(pObject, other);
            other = pObject->next;
        }
    }
}

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

// FUNCTION: CARMA2_HW 0x004c5830
// FUNCTION: CARMA2_SW 0x004c1f60
int C2_HOOK_FASTCALL RQDcmpV(br_scalar* pMatrix, int pDim, br_scalar* pQ) {
    int i;
    int j;
    int k;
    int singular;
    br_scalar scale;
    br_scalar sum;
    br_scalar norm;
    br_scalar divisor;
    br_scalar matrix_sum;
    br_scalar orthogonal_sum;
    br_scalar tau;
    br_scalar tau2;
    br_scalar* row;

    /* Using householder transformations, apply RQ decomposition on pMatrix:
     * - pMatrix is in-place converted into a lower-triangular matrix
     * - pQ is a orthonormal matrix */

    singular = 0;

    /* pQ starts as a identity matrix */
    for (i = 0; i < pDim; i++) {
        for (j = 0; j < pDim; j++) {
            pQ[i * pDim + j] = 0.0f;
        }
        pQ[i * pDim + i] = 1.0f;
    }

    for (i = 0; i < pDim; i++) {
        // Rotate i-th upper-diagonal row onto base vector

        row = &pMatrix[i * pDim];
        scale = 0.0f;
        for (j = i; j < pDim; j++) {
            scale = (br_scalar)MAX(scale, fabs(row[j]));
        }
        if (scale == 0.0) {
            singular = 1;
            continue;
        }
        // Only normalize the upper-vector that must be rotated onto the base vector
        // This is done for numerical stability reasons.
        for (j = i; j < pDim; j++) {
            row[j] /= scale;
        }
        // Calculate norm of the upper-diagonal row
        sum = 0.0f;
        for (j = i; j < pDim; j++) {
            sum += row[j] * row[j];
        }
        // Apply Householder reflection on pQ and the row+matrix
        norm = (br_scalar)(row[i] > 0.0 ? fabs(sqrt(sum)) : -fabs(sqrt(sum)));
        divisor = norm * (norm + row[i]);
        row[i] += norm;
        for (j = 0; j <= i; j++) {
            sum = 0.0f;
            for (k = i; k < pDim; k++) {
                sum += row[k] * pQ[k * pDim + j];
            }
            tau = sum / divisor;
            for (k = i; k < pDim; k++) {
                pQ[k * pDim + j] -= tau * row[k];
            }
        }
        for (j = i + 1; j < pDim; j++) {
            matrix_sum = 0.0f;
            for (k = i; k < pDim; k++) {
                matrix_sum += pMatrix[j * pDim + k] * row[k];
            }
            tau2 = matrix_sum / divisor;
            orthogonal_sum = 0.0f;
            for (k = i; k < pDim; k++) {
                orthogonal_sum += pQ[k * pDim + j] * pMatrix[i * pDim + k];
            }
            tau = orthogonal_sum / divisor;
            for (k = i; k < pDim; k++) {
                pMatrix[j * pDim + k] -= tau2 * row[k];
                pQ[k * pDim + j] -= tau * row[k];
            }
        }
        row[i] = -(norm * scale);
        // Zero upper-dialog values so what remains is lower-diagonal
        for (j = i + 1; j < pDim; j++) {
            row[j] = 0.0f;
        }
    }
    return singular;
}

// MakeIdentity

// SparseRQDcmpV

// MakeMatrixFromIndices

// HasBeenDoneBefore

// RemoveRQSingularities

// AddToMatrix

// RQRemovedOneTooManyForces

// GetInverseM

// RQMatrixSolveNoNegatives

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004c53b0
// STUB: CARMA2_SW 0x004c1ae0
// STUB: CARMA2_CHEAT 0x004caed0
void C2_HOOK_FASTCALL RQMatrixSolveWithNegatives(br_scalar* pR, br_scalar* pQ, br_scalar* pB, br_scalar* pX, int pCount) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c5b50
// FUNCTION: CARMA2_SW 0x004c2280
int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim) {
    return RQDcmpV(pR, pDim, pQ);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c5b60
void C2_HOOK_FASTCALL RqBackSub(const br_scalar* pR, const br_scalar* pQ, const br_scalar* pRhs, br_scalar* pX, int pDim) {
    br_scalar intermediate[100];
    br_scalar sum;
    int i;
    int j;

    /* Solve Ax = b, with A represented by its RQ decomposition, where:
     * - R is a lower-diagonal matrix
     * - Q is an orthonormal matrix
     * with:
     * - all matrices (A, Q, R) are square of size N x N (with N = pDim)
     * - vectors x, t are of size N x 1
     * A, Q, R matrices are of size [pDim x pDim]
     */

    C2_HOOK_ASSERT(pDim > 0 && pDim <= (int)REC2_ASIZE(intermediate));

    /* Use back substitution to solve Ry = b, with R a lower-diagonal matrix */
    intermediate[0] = pRhs[0] / pR[0 * pDim + 0];             /*  T[0] = B[0] / R[0, 0] */
    for (i = 1; i < pDim; i++) {
        sum = 0.0f;
        for (j = 0; j < i; j++) {
            sum += intermediate[j] * pR[i * pDim + j];        /*  sum += T[j] * R[i, j] */
        }
        intermediate[i] = (pRhs[i] - sum) / pR[i * pDim + i]; /*  T[i] = (B[i] - sum) / R[i, i] */
    }
    /* Solve Qx = T, by multiplying both sides with Q transposed */
    for (i = 0; i < pDim; i++) {
        pX[i] = pQ[0 * pDim + i] * intermediate[0];           /*  X[i]  = Q[0, i] * T[0] */
        for (j = 1; j < pDim; j++) {
            pX[i] += intermediate[j] * pQ[j * pDim + i];      /*  X[i] += Q[j, i] * T[j] */
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c5c70
// FUNCTION: CARMA2_SW 0x004c23a0
// FUNCTION: CARMA2_CHEAT 0x004cb790
void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    return gPhysics_scratch_ptr;
}

// FUNCTION: CARMA2_HW 0x004c5cb0
// FUNCTION: CARMA2_SW 0x004c23e0
// FUNCTION: CARMA2_CHEAT 0x004cb7d0
void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    gPhysics_scratch_ptr = (tU8*)gPhysics_scratch_ptr + pSize;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c5d00
// FUNCTION: CARMA2_SW 0x004c2430
// FUNCTION: CARMA2_CHEAT 0x004cb820
void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void) {

    gPhysics_scratch_ptr = gPhysics_other_buffer;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c5d10
// FUNCTION: CARMA2_SW 0x004c2440
// FUNCTION: CARMA2_CHEAT 0x004cb830
tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType) {
    tCollision_shape_sphere* result;

    result = BrMemAllocate(sizeof(tCollision_shape_sphere), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5d70
// FUNCTION: CARMA2_SW 0x004c24a0
// FUNCTION: CARMA2_CHEAT 0x004cb890
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;
    int count_edges;
    int count_planes;

    count_planes = (pCount_points - 2) * 2;
    count_edges = (pCount_points - 2) * 3;
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) + pCount_points * sizeof(br_vector3)
        + count_edges * sizeof(tPolyhedron_edge_indexes) + count_planes * sizeof(br_vector4), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_polyhedron);
    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->polyhedron.edges = (tPolyhedron_edge_indexes*)raw_memory;
    raw_memory += count_edges * sizeof(tPolyhedron_edge_indexes);
    result->polyhedron.planes = (br_vector4*)raw_memory;
    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5dc0
tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3)
        + pCount_lines * sizeof(tPolyhedron_edge_indexes), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_wireframe);
    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->wireframe.count_lines = pCount_lines;
    result->wireframe.lines = (tPolyhedron_edge_indexes*)raw_memory;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5e00
// FUNCTION: CARMA2_SW 0x004c2530
// FUNCTION: CARMA2_CHEAT 0x004cb920
tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType) {
    tCollision_shape_box* result;

    result = BrMemAllocate(sizeof(tCollision_shape_box), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}

// GetJointSize

// FUNCTION: CARMA2_HW 0x004c5e20
// FUNCTION: CARMA2_SW 0x004c2550
// FUNCTION: CARMA2_CHEAT 0x004cb940
tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType) {
    return BrMemAllocate(offsetof(tPhysics_joint, limits) + pCount_limits * sizeof(tPhysics_joint_limit), (br_uint_8)pType);
}

// FUNCTION: CARMA2_HW 0x004c5e40
tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType) {
    size_t size;
    tPhysics_joint* clone;

    size = offsetof(tPhysics_joint, limits) + pJoint->count_limits * sizeof(tPhysics_joint_limit);
    clone = BrMemAllocate(size, (br_uint_8)pType);
    memcpy(clone, pJoint, size);
    return clone;
}

// FUNCTION: CARMA2_HW 0x004c5e70
// FUNCTION: CARMA2_SW 0x004c25a0
// FUNCTION: CARMA2_CHEAT 0x004cb990
void C2_HOOK_FASTCALL FreeShape(void* pData) {
    BrMemFree(pData);
}

// FUNCTION: CARMA2_HW 0x004c5e80
// FUNCTION: CARMA2_SW 0x004c25b0
// FUNCTION: CARMA2_CHEAT 0x004cb9a0
void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape) {

    if (pShape->common.next != NULL) {
        FreeShapeList(pShape->common.next);
    }
    BrMemFree(pShape);
}

// FUNCTION: CARMA2_HW 0x004c5ea0
// FUNCTION: CARMA2_SW 0x004c25d0
// FUNCTION: CARMA2_CHEAT 0x004cb9c0
void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint) {

    BrMemFree(pJoint);
}

// FUNCTION: CARMA2_HW 0x004c5eb0
// FUNCTION: CARMA2_SW 0x004c25e0
// FUNCTION: CARMA2_CHEAT 0x004cb9d0
void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds) {
    int i;

    BrVector3Copy(&pBounds->min, &pVertices[0]);
    BrVector3Copy(&pBounds->max, &pVertices[0]);
    for (i = 1; i < pCount_vertices; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (pVertices[i].v[j] < pBounds->min.v[j]) {
                pBounds->min.v[j] = pVertices[i].v[j];
            } else if (pVertices[i].v[j] > pBounds->max.v[j]) {
                pBounds->max.v[j] = pVertices[i].v[j];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c5f20
// FUNCTION: CARMA2_SW 0x004c2650
// FUNCTION: CARMA2_CHEAT 0x004cba40
void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape) {
    int i;
    tPhysicsError error;

    C2_HOOK_BUG_ON(REC2_ASIZE(pShape->common.bb.min.v) != 3);

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        break;
    case kCollisionShapeType_Wireframe:
        if (pShape->wireframe.wireframe.count_points == 0) {
            PhysicsError(ePhysicsError_WireFrameHasNoPoints, NULL);
        } else {
            GetBoundsFromPointList(pShape->wireframe.wireframe.points, pShape->wireframe.wireframe.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 4) {
            PhysicsError(ePhysicsError_PolyhedronHasNoPoints, NULL);
        } else {
            error = ConvexHull3D(&pShape->polyhedron.polyhedron);
            if (error != ePhysicsError_Ok) {
                if (error == ePhysicsError_WireFrameHasNoPoints) {
                    pShape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
                } else {
                    PDFatalError("This polyhedron is too wierd for me");
                }
            }
            GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Wireframe_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 3) {
            PhysicsError(4, NULL);
        } else {
            ConvexHull2D(&pShape->polyhedron.polyhedron);
            GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Sphere:
        BrVector3Copy(&pShape->common.bb.min, &pShape->sphere.sphere.center);
        BrVector3Copy(&pShape->common.bb.max, &pShape->sphere.sphere.center);
        for (i = 0; i < REC2_ASIZE(pShape->common.bb.min.v); i++) {
            pShape->common.bb.min.v[i] -= pShape->sphere.sphere.radius;
            pShape->common.bb.max.v[i] += pShape->sphere.sphere.radius;
        }
        pShape->sphere.sphere.radius_squared = REC2_SQR(pShape->sphere.sphere.radius);
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, NULL);
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004c60a0
// FUNCTION: CARMA2_SW 0x004c27d0
// FUNCTION: CARMA2_CHEAT 0x004cbbc0
void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info) {
    tPhysics_shape* shape;
    tPhysics_shape* current;
    br_scalar radius;

    shape = pCollision_info->shape;
    if (shape == NULL) {
        return;
    }
    pCollision_info->bb1 = shape->common.bb;

    for (current = shape->common.next; current != NULL; current = current->common.next) {
        int i;

        for (i = 0; i < 3; i++) {

            pCollision_info->bb1.min.v[i] = pCollision_info->bb1.min.v[i] < current->common.bb.min.v[i] ? pCollision_info->bb1.min.v[i] : current->common.bb.min.v[i];
            pCollision_info->bb1.max.v[i] = pCollision_info->bb1.max.v[i] > current->common.bb.max.v[i] ? pCollision_info->bb1.max.v[i] : current->common.bb.max.v[i];
        }
    }
    pCollision_info->bb2 = pCollision_info->bb1;

    pCollision_info->radius_squared = 0.0f
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[0]), REC2_SQR(pCollision_info->bb2.max.v[0]))
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[1]), REC2_SQR(pCollision_info->bb2.max.v[1]))
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[2]), REC2_SQR(pCollision_info->bb2.max.v[2]));
    radius = (br_scalar)sqrt(pCollision_info->radius_squared);
    pCollision_info->radius = radius;
}

// FUNCTION: CARMA2_HW 0x004c61c0
// FUNCTION: CARMA2_SW 0x004c28f0
// FUNCTION: CARMA2_CHEAT 0x004cbce0
void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage) {

    if (gPhysics_error_cb != NULL) {
        gPhysics_error_cb(pError, pMessage);
    }
    exit(pError);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c61e0
// FUNCTION: CARMA2_SW 0x004c2910
// FUNCTION: CARMA2_CHEAT 0x004cbd00
void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage) {
    (void)pMessage;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c61f0
// FUNCTION: CARMA2_SW 0x004c2920
// FUNCTION: CARMA2_CHEAT 0x004cbd10
void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn) {

    gPhysics_error_cb = pError_cbfn;
}

// FUNCTION: CARMA2_HW 0x004c62e0
// FUNCTION: CARMA2_SW 0x004c2a10
// FUNCTION: CARMA2_CHEAT 0x004cbe00
void C2_HOOK_FASTCALL RepositionAndOrientateChildren(tPhysics_object* pObject, br_matrix34* pParent_matrix) {
    br_matrix34 inverse;
    br_matrix34 parent_matrix;
    br_matrix34 transform;
    tPhysics_object* child;

    child = pObject->child;
    if (pParent_matrix != NULL) {
        BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->transform_matrix, pParent_matrix);
    } else {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&transform, &inverse, &pObject->actor->t.t.mat);
        BrMatrix34LPNormalise(&parent_matrix, &transform);
        pParent_matrix = &parent_matrix;
    }
    if (child != NULL) {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&parent_matrix, &inverse, &pObject->actor->t.t.mat);
        for (; child != NULL; child = child->next) {
            RepositionAndOrientateChildren(child, pParent_matrix);
        }
    }
    BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c63b0
// FUNCTION: CARMA2_SW 0x004c2ae0
// FUNCTION: CARMA2_CHEAT 0x004cbed0
void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->next = pParent->child;
    pParent->child = pChild;
    pChild->parent = pParent;
}

// FUNCTION: CARMA2_HW 0x004c63d0
// FUNCTION: CARMA2_SW 0x004c2b00
// FUNCTION: CARMA2_CHEAT 0x004cbef0
void C2_HOOK_FASTCALL PhysicsAddObjectList(tPhysics_object* pParent, tPhysics_object* pChild) {
    tPhysics_object* last;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->parent = pParent;
    last = pChild;
    while (last->next != NULL) {
        last = last->next;
        last->parent = pParent;
    }
    last->next = pParent->child;
    pParent->child = pChild;
}

// FUNCTION: CARMA2_HW 0x004c6410
// FUNCTION: CARMA2_SW 0x004c2b40
// FUNCTION: CARMA2_CHEAT 0x004cbf30
void C2_HOOK_FASTCALL PhysicsRemoveObject(tPhysics_object* pObject) {
    tPhysics_object* parent;
    tPhysics_object* sibling;

    parent = pObject->parent;
    sibling = parent->child;
    if (sibling == pObject) {
        parent->child = pObject->next;
    } else {
        while (sibling->next != pObject) {
            sibling = sibling->next;
        }
        sibling->next = pObject->next;
    }
    pObject->parent = NULL;
    pObject->next = NULL;
}

// FUNCTION: CARMA2_HW 0x004c6470
// FUNCTION: CARMA2_SW 0x004c2ba0
// FUNCTION: CARMA2_CHEAT 0x004cbf90
int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    int result;
    tPhysics_object* child;

    result = pCallback(pObject, pContext);
    if (result != 0) {
        return result;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c64b0
int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    tPhysics_object* child;

    for (child = pObject->child; child != NULL; child = child->next) {
        int result;

        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c6510
// FUNCTION: CARMA2_SW 0x004c2c40
// FUNCTION: CARMA2_CHEAT 0x004cc030
void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {

    gPhysics_buffer1_ptr = pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = pBuffer + PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer3_ptr = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;
    pBuffer += 3 * PHYSICS_BUFFER_PART_SIZE;
    if (pSize < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer, "Physics workspace too small");
    }
    gPhysics_other_buffer_capacity = pSize;
    gPhysics_other_buffer = pBuffer;
}

// FUNCTION: CARMA2_HW 0x004b57d0
// FUNCTION: CARMA2_SW 0x004b1ef0
// FUNCTION: CARMA2_CHEAT 0x004bb0a0
int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks) {
    tU8 cx_min;
    tU8 cz_min;
    tU8 cx_max;
    tU8 cz_max;
    int x;
    int z;
    int emitted;
    tTrack_spec* track_spec;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_x, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_z, 0x1);

    emitted = 0;
    track_spec = &gProgram_state.track_spec;
    FillInBounds(pBounds);
    XZToColumnXZ(&cx_min, &cz_min, pBounds->real_bounds.min.v[0], pBounds->real_bounds.min.v[2], track_spec);
    XZToColumnXZ(&cx_max, &cz_max, pBounds->real_bounds.max.v[0], pBounds->real_bounds.max.v[2], track_spec);
    if (cx_min > 0) {
        cx_min--;
    }
    if (cz_min > 0) {
        cz_min--;
    }
    if (cx_max + 1 < track_spec->ncolumns_x) {
        cx_max++;
    }
    if (cz_max + 1 < track_spec->ncolumns_z) {
        cz_max++;
    }
    for (x = cx_min; x <= cx_max; x++) {
        for (z = cz_min; z <= cz_max; z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                emitted = pMax_face - ActorBoxPick(pBounds, track_spec->columns[z][x].actor_0x0, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
            }
        }
    }
    if (gAdditional_actors != NULL) {
        emitted = pMax_face - ActorBoxPick(pBounds, gAdditional_actors, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
    }
    return emitted;
}

// FUNCTION: CARMA2_HW 0x004b5970
float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace) {
    int material_index;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCurrent_race.material_modifiers) != 11);

    material_index = pFace->material->identifier[0] - '/';
    if (material_index < 0 || material_index >= (int)REC2_ASIZE(gCurrent_race.material_modifiers)) {
        material_index = 0;
    }
    return gCurrent_race.material_modifiers[material_index].car_wall_friction;
}

// FUNCTION: CARMA2_HW 0x004b5990
// FUNCTION: CARMA2_SW 0x004b20b0
// FUNCTION: CARMA2_CHEAT 0x004bb260
void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage) {
    char s[256];

    (void)pMessage;
    switch (pError) {
    case ePhysicsError_UnknownShapeType:
        strcpy(s, "Unknown shape type");
        break;
    case ePhysicsError_WireFrameHasNoPoints:
        strcpy(s, "Wire frame shape has no points");
        break;
    case ePhysicsError_PolyhedronHasNoPoints:
        strcpy(s, "Polyhedron shape has no points");
        break;
    case ePhysicsError_UnknownHingeType:
        strcpy(s, "Unknown hinge type");
        /* The original overwrites this message with the code. */
#ifndef REC2_MATCHING
        break;
#endif
        /* fall through */
    default:
        sprintf(s, "%d ", pError);
        break;
    }
    FatalError(1000 + pError, s);
}

// ResetMan2

// FUNCTION: CARMA2_HW 0x004b5aa0
// FUNCTION: CARMA2_CHEAT 0x004bb370
void C2_HOOK_FASTCALL SetUpTestObjects(void) {

}

// ResetObjectList

// ResetMan

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x004b5ca0
// FUNCTION: CARMA2_SW 0x004b23c0
// FUNCTION: CARMA2_CHEAT 0x004bb570
void C2_HOOK_FASTCALL InitPhysics(void) {

    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// PointOutOfSightNotAR (moved to 51-smash for inlining purposes)

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

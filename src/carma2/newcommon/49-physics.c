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

// GLOBAL: CARMA2_HW 0x00692dc8
// GLOBAL: CARMA2_SW 0x0068ce20
// GLOBAL: CARMA2_CHEAT 0x0069a940
int gINT_00692dc8;

// GLOBAL: CARMA2_HW 0x006940b8
// GLOBAL: CARMA2_SW 0x0068e110
// GLOBAL: CARMA2_CHEAT 0x0069bc30
int gINT_006940b8;

// GLOBAL: CARMA2_HW 0x006940c0
// GLOBAL: CARMA2_SW 0x0068e118
// GLOBAL: CARMA2_CHEAT 0x0069bc38
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
// GLOBAL: CARMA2_SW 0x0073b2d8
// GLOBAL: CARMA2_CHEAT 0x0074beb8
br_scalar* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
// GLOBAL: CARMA2_SW 0x0073b2d4
// GLOBAL: CARMA2_CHEAT 0x0074beb4
br_scalar* gPhysics_buffer2_ptr;

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
void* gPhysics_scratch_ptr;

// GLOBAL: CARMA2_HW 0x006940b0
// GLOBAL: CARMA2_SW 0x0068e108
// GLOBAL: CARMA2_CHEAT 0x0069bc28
int gFace_count;

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

// AddObjectWallForce

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

// RayCastInBox

// FaceOffCount2

// FaceOffCount

// FaceOnCount

// EliminateFaces

// GetShapePoint

// GetObjectObjectEdgeIntersections

// GetFaceEdgeIntersection

// GetSphereToFacePoints

// CheckSingleEdgeWithSphere

// GetSphereToEdgePoints

// GetCornerToFacePoints

// CylinderCheck

// CheckTwoEdges

// WorldPointCollision

// CheckSingleEdgeWithObjectEdges

// GetEdgeToEdgePoints

// SetUpWorkspace

// GetObjectWallForceInfo

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

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

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

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

// STUB: CARMA2_HW 0x00429070
// STUB: CARMA2_SW 0x00429100
// STUB: CARMA2_CHEAT 0x00429200
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    NOT_IMPLEMENTED();
}

// HierarchyCalcEnergy

// CheckCollisionList

// MakeJointVelocitiesVector

// ApplySingleForce

// ApplyJointForces

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

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

// AllocateObjectWorkspace

// RedoQuickHingeVelocities

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

// GetJointLimits

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

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

// ListLimitOmega

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

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b7510
// STUB: CARMA2_SW 0x004b3c30
// STUB: CARMA2_CHEAT 0x004bd040
void C2_HOOK_FASTCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time) {
    NOT_IMPLEMENTED();
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

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

// FUNCTION: CARMA2_HW 0x004c2a10
// FUNCTION: CARMA2_SW 0x004bf140
void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject) {
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
        WriteNewPositions(child);
    }
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

// SetGroupDoingNothingFlag

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

// CheckDoingNothingFlag

// FUNCTION: CARMA2_HW 0x004bae80
// FUNCTION: CARMA2_SW 0x004b75a0
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

// GroupTogether

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

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// FUNCTION: CARMA2_HW 0x004bd9b0
// FUNCTION: CARMA2_SW 0x004ba0d0
void C2_HOOK_FASTCALL SetObjectTrialPositionType(tPhysics_object* pObject, int pType) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->field_0xf0 = pType;
    for (; child != NULL; child = child->next) {
        SetObjectTrialPositionType(child, pType);
    }
}

// FillInTrialWorkspace

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

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

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

// DoCollisions

// AddDoubleTorqueToMatrix

// DRMatrix33Inverse

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

// FUNCTION: CARMA2_HW 0x004be4b0
// FUNCTION: CARMA2_SW 0x004babd0
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

// SetUpTorqueHingePartners

// SetUpHingePartners

// MakeMatrix

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

// NewAddFrictionObjectObject

// NewAddFriction

// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

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

// RotateObject

void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    br_vector3 tv;

    BrVector3Scale(&tv, &pObject->v, pDelta_time);
    BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
}

// STUB: CARMA2_HW 0x004c2060
// STUB: CARMA2_SW 0x004be790
void C2_HOOK_FASTCALL GetFacesNearObject(tPhysics_object *pObject, tWorld_callbacks *pCallbacks) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c25a0
// FUNCTION: CARMA2_SW 0x004becd0
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

// TrialPositionChildren

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

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer
inline int C2_HOOK_FASTCALL TestForWrongAnswer(const br_scalar* pOriginal, const br_scalar* pRhs, const br_scalar* pResult, int pSize) {
    br_scalar product[100];
    int wrong;
    int i;
    int j;

    wrong = 0;
    for (i = 0; i < pSize; i++) {
        product[i] = 0.0f;
        for (j = 0; j < pSize; j++) {
            product[i] += pOriginal[i * 100 + j] * pResult[j];
        }
        if ((pResult[i] != 0.0f && fabs(product[i] - pRhs[i]) > 0.01) || product[i] - pRhs[i] < -0.01) {
            wrong = 1;
        }
    }
    return wrong;
}

// RQIterativeImprovement
inline void C2_HOOK_FASTCALL RQIterativeImprovement(br_scalar* pForces, const br_scalar* pMatrix, const br_scalar* pOrthogonal, const br_scalar* pRhs, const br_scalar* pOriginal, const tU8* pIndices, int pSize) {
    br_scalar residual[100];
    br_scalar correction[100];
    br_scalar sum;
    int i;
    int j;

    for (i = 0; i < pSize; i++) {
        sum = -pRhs[i];
        for (j = 0; j < pSize; j++) {
            sum += pOriginal[pIndices[i] * 100 + pIndices[j]] * pForces[j];
        }
        residual[i] = sum;
    }
    correction[0] = residual[0] / pMatrix[0];
    for (i = 1; i < pSize; i++) {
        sum = 0.0f;
        for (j = 0; j < i; j++) {
            sum += correction[j] * pMatrix[i * 100 + j];
        }
        correction[i] = (residual[i] - sum) / pMatrix[i * 101];
    }
    for (i = 0; i < pSize; i++) {
        for (j = 0; j < pSize; j++) {
            pForces[i] += pOrthogonal[j * 100 + i] * correction[j];
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c4850
// FUNCTION: CARMA2_SW 0x004c0f80
// FUNCTION: CARMA2_CHEAT 0x004ca370
void C2_HOOK_FASTCALL RQExpand(br_scalar* pMatrix, const br_scalar* pOriginal, int pOld_size, int pSize, br_scalar* pOrthogonal, const tU8* pIndices) {
    br_scalar row[100];
    br_scalar value;
    br_scalar maximum;
    br_scalar scale;
    br_scalar diagonal;
    br_scalar sum;
    br_scalar length;
    br_scalar head;
    br_scalar denominator;
    br_scalar coefficient;
    const br_scalar* selected;
    int i;
    int j;
    int k;

    for (j = pOld_size; j < pSize; j++) {
        selected = &pOriginal[100 * pIndices[j]];
        for (i = 0; i < pOld_size; i++) {
            row[i] = selected[pIndices[i]];
            pMatrix[i * 100 + j] = row[i];
        }
        for (i = 0; i < pOld_size; i++) {
            sum = 0.0f;
            for (k = 0; k < pOld_size; k++) {
                sum += row[k] * pOrthogonal[i * 100 + k];
            }
            pMatrix[j * 100 + i] = sum;
        }
    }

    for (i = pOld_size; i < pSize; i++) {
        selected = &pOriginal[100 * pIndices[i]];
        for (j = pOld_size; j < pSize; j++) {
            pMatrix[i * 100 + j] = selected[pIndices[j]];
            pOrthogonal[i * 100 + j] = 0.0f;
        }
        for (j = 0; j < pOld_size; j++) {
            pOrthogonal[i * 100 + j] = 0.0f;
            pOrthogonal[j * 100 + i] = 0.0f;
        }
        pOrthogonal[i * 101] = 1.0f;
    }

    for (i = 0; i < pOld_size; i++) {
        diagonal = pMatrix[i * 101];
        maximum = 0.0f;
        for (k = pOld_size; k < pSize; k++) {
            value = (br_scalar)fabs(pMatrix[i * 100 + k]);
            if (maximum <= value) {
                maximum = value;
            }
        }
        if (maximum == 0.0f) {
            continue;
        }
        scale = (br_scalar)fabs(diagonal);
        if (maximum > scale) {
            scale = maximum;
        }
        for (k = pOld_size; k < pSize; k++) {
            pMatrix[i * 100 + k] /= scale;
        }
        diagonal /= scale;
        sum = 0.0f;
        for (k = pOld_size; k < pSize; k++) {
            sum += pMatrix[i * 100 + k] * pMatrix[i * 100 + k];
        }
        length = (br_scalar)fabs(sqrt(diagonal * diagonal + sum));
        if (diagonal <= 0.0f) {
            length = -length;
        }
        head = diagonal + length;
        denominator = head * length;
        for (j = 0; j < pSize; j++) {
            sum = head * pOrthogonal[i * 100 + j];
            for (k = pOld_size; k < pSize; k++) {
                sum += pOrthogonal[k * 100 + j] * pMatrix[i * 100 + k];
            }
            coefficient = sum / denominator;
            for (k = pOld_size; k < pSize; k++) {
                pOrthogonal[k * 100 + j] -= coefficient * pMatrix[i * 100 + k];
            }
            pOrthogonal[i * 100 + j] -= head * coefficient;
        }
        for (j = i + 1; j < pSize; j++) {
            sum = head * pMatrix[j * 100 + i];
            for (k = pOld_size; k < pSize; k++) {
                sum += pMatrix[j * 100 + k] * pMatrix[i * 100 + k];
            }
            coefficient = sum / denominator;
            for (k = pOld_size; k < pSize; k++) {
                pMatrix[j * 100 + k] -= coefficient * pMatrix[i * 100 + k];
            }
            pMatrix[j * 100 + i] -= head * coefficient;
        }
        pMatrix[i * 101] = -(length * scale);
        for (k = pOld_size; k < pSize; k++) {
            pMatrix[i * 100 + k] = 0.0f;
        }
    }

    for (i = pOld_size; i < pSize; i++) {
        maximum = 0.0f;
        for (k = i; k < pSize; k++) {
            value = (br_scalar)fabs(pMatrix[i * 100 + k]);
            if (maximum <= value) {
                maximum = value;
            }
        }
        if (maximum == 0.0f) {
            continue;
        }
        for (k = i; k < pSize; k++) {
            pMatrix[i * 100 + k] /= maximum;
        }
        sum = 0.0f;
        for (k = i; k < pSize; k++) {
            sum += pMatrix[i * 100 + k] * pMatrix[i * 100 + k];
        }
        length = (br_scalar)fabs(sqrt(sum));
        if (pMatrix[i * 101] <= 0.0f) {
            length = -length;
        }
        head = length + pMatrix[i * 101];
        denominator = length * head;
        pMatrix[i * 101] = head;
        for (j = 0; j < pSize; j++) {
            sum = 0.0f;
            for (k = i; k < pSize; k++) {
                sum += pMatrix[i * 100 + k] * pOrthogonal[k * 100 + j];
            }
            coefficient = sum / denominator;
            for (k = i; k < pSize; k++) {
                pOrthogonal[k * 100 + j] -= coefficient * pMatrix[i * 100 + k];
            }
        }
        for (j = i + 1; j < pSize; j++) {
            sum = 0.0f;
            for (k = i; k < pSize; k++) {
                sum += pMatrix[j * 100 + k] * pMatrix[i * 100 + k];
            }
            coefficient = sum / denominator;
            for (k = i; k < pSize; k++) {
                pMatrix[j * 100 + k] -= coefficient * pMatrix[i * 100 + k];
            }
        }
        pMatrix[i * 101] = -(length * maximum);
        for (k = i + 1; k < pSize; k++) {
            pMatrix[i * 100 + k] = 0.0f;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c4f40
// FUNCTION: CARMA2_SW 0x004c1670
// FUNCTION: CARMA2_CHEAT 0x004caa60
void C2_HOOK_FASTCALL RQSingleExpand(br_scalar* pMatrix, const br_scalar* pOriginal, int pSize, br_scalar* pOrthogonal, const tU8* pIndices) {
    int i;
    int k;
    br_scalar maximum;
    br_scalar head;
    br_scalar denominator;
    br_scalar length;
    br_scalar second_coefficient;
    br_scalar row[100];
    br_scalar diagonal;
    br_scalar sum;
    br_scalar magnitude;
    int j;
    br_scalar second;
    br_scalar first_coefficient;

    for (i = 0; i <= pSize; i++) {
        row[i] = pOriginal[100 * pIndices[pSize] + pIndices[i]];
    }
    for (k = 0; k < pSize; k++) {
        sum = 0.0f;
        for (j = 0; j < pSize; j++) {
            sum += pOrthogonal[j + k * 100] * row[j];
        }
        pMatrix[100 * pSize + k] = sum;
    }
    pOrthogonal[101 * pSize] = 1.0f;
    for (i = 0; i < pSize; i++) {
        pOrthogonal[i * 100 + pSize] = 0.0f;
        pOrthogonal[100 * pSize + i] = 0.0f;
    }
    for (i = 0; i < pSize; i++) {
        magnitude = (br_scalar)fabs(row[i]);
        pMatrix[i * 100 + pSize] = 0.0f;
        if (magnitude == 0.0f) {
            continue;
        }
        maximum = (br_scalar)fabs(pMatrix[i * 101]);
        if (magnitude > maximum) {
            maximum = magnitude;
        }
        second = row[i] / maximum;
        diagonal = pMatrix[i * 101] / maximum;
        length = (br_scalar)fabs(sqrt((diagonal * diagonal) + second * second));
        row[i] = second;
        if (diagonal <= 0.0f) {
            length = -length;
        }
        head = diagonal + length;
        denominator = head * length;
        second_coefficient = second / denominator;
        first_coefficient = head / denominator;
        for (j = 0; j <= pSize; j++) {
            sum = (second * pOrthogonal[pSize * 100 + j]) + (pOrthogonal[i * 100 + j] * head);
            pOrthogonal[pSize * 100 + j] -= second_coefficient * sum;
            pOrthogonal[i * 100 + j] -= first_coefficient * sum;
        }
        for (j = i + 1; j <= pSize; j++) {
            sum = (row[j] * second) + (pMatrix[100 * j + i] * head);
            row[j] -= sum * second_coefficient;
            pMatrix[i + j * 100] -= sum * first_coefficient;
        }
        pMatrix[i * 101] = -(length * maximum);
    }
    pMatrix[pSize * 101] = row[pSize];
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// FUNCTION: CARMA2_HW 0x004c5220
// FUNCTION: CARMA2_SW 0x004c1950
// FUNCTION: CARMA2_CHEAT 0x004cad40
void C2_HOOK_FASTCALL MakeMatrixFromIndices(br_scalar* pMatrix, br_scalar* pOrthogonal, const br_scalar* pOriginal, br_scalar* pResult, const br_scalar* pRhs, const tU8* pIndices, int pSize, tForce_group** pGroups, int pCount_groups) {
    int total;
    int g;
    int count;
    int i;
    int j;
    int k;

    total = 0;
    for (g = 0; g < pCount_groups; g++) {
        count = pGroups[g]->count;
        for (i = 0; i < count; i++) {
            for (j = 0; j < count; j++) {
                pMatrix[(total + i) * 100 + total + j] = pGroups[g]->matrix[i * count + j];
                pOrthogonal[(total + i) * 100 + total + j] = pGroups[g]->orthogonal[i * count + j];
            }
            for (k = 0; k < total; k++) {
                pMatrix[k * 100 + total + i] = 0.0f;
                pMatrix[(total + i) * 100 + k] = 0.0f;
                pOrthogonal[k * 100 + total + i] = 0.0f;
                pOrthogonal[(total + i) * 100 + k] = 0.0f;
            }
        }
        total += count;
    }
    if (pSize - total == 1) {
        RQSingleExpand(pMatrix, pOriginal, total, pOrthogonal, pIndices);
    } else if (pSize > total) {
        RQExpand(pMatrix, pOriginal, total, pSize, pOrthogonal, pIndices);
    }
    for (i = 0; i < pSize; i++) {
        pResult[i] = pRhs[pIndices[i]];
    }
}

// HasBeenDoneBefore
inline int C2_HOOK_FASTCALL HasBeenDoneBefore(tU32 pHistory[][2], int pCount, int pToggle) {
    tU32 mask[2];
    int i;
    int j;

    mask[0] = pHistory[pCount][0];
    mask[1] = pHistory[pCount][1];
    if (pToggle >= 0) {
        mask[pToggle >> 5] ^= 1 << (pToggle & 0x1f);
    }
    for (i = 0; i < pCount; i++) {
        for (j = 0; j <= 1; j++) {
            if (mask[j] != pHistory[i][j]) {
                break;
            }
        }
        if (j > 1) {
            return 1;
        }
    }
    return 0;
}

// RemoveRQSingularities
inline int C2_HOOK_FASTCALL RemoveRQSingularities(int pLast, const br_scalar* pMatrix, const br_scalar* pOrthogonal, const br_scalar* pRhs, const br_scalar* pForward, int pFirst, tU32 pHistory[][2], int pCount, const tU8* pIndices) {
    br_scalar biggest;
    br_scalar value;
    int chosen;
    int i;

    chosen = pLast;
    biggest = 0.0f;
    for (i = pFirst; i < pLast; i++) {
        value = (br_scalar)fabs(pOrthogonal[pLast * 100 + i]);
        if (value > biggest) {
            biggest = value;
            chosen = i;
        }
    }
    if (chosen == pLast) {
        return pFirst;
    }
    if (HasBeenDoneBefore(pHistory, pCount, pIndices[chosen] - pFirst) != 0) {
        biggest = 0.0f;
        for (i = pFirst; i < pLast; i++) {
            value = (br_scalar)fabs(pOrthogonal[pLast * 100 + i]);
            if (value > biggest) {
                if (HasBeenDoneBefore(pHistory, pCount, pIndices[i] - pFirst) == 0) {
                    biggest = value;
                    chosen = i;
                }
            }
        }
        if (chosen == pLast) {
            return pFirst;
        }
    }
    return chosen;
}

// AddToMatrix
inline void C2_HOOK_FASTCALL AddToMatrix(tU8 pIndex, br_scalar* pMatrix, const br_scalar* pOriginal, br_scalar* pResult, const br_scalar* pRhs, br_scalar* pOrthogonal, tU8* pIndices, int* pSize) {

    pIndices[*pSize] = pIndex;
    *pSize += 1;
    if (gINT_006940b8 == 0) {
        RQSingleExpand(pMatrix, pOriginal, *pSize - 1, pOrthogonal, pIndices);
    } else {
        RQExpand(pMatrix, pOriginal, *pSize - 1, *pSize, pOrthogonal, pIndices);
    }
    pResult[*pSize - 1] = pRhs[pIndices[*pSize - 1]];
}

// RQRemovedOneTooManyForces
inline int C2_HOOK_FASTCALL RQRemovedOneTooManyForces(int* pCount_excluded, tU8* pExcluded, int pSize, const br_scalar* pResult, const br_scalar* pRhs, const br_scalar* pOriginal, tU32 pHistory[][2], int pCount, int pFirst) {
    br_scalar sum;
    br_scalar scaled;
    br_scalar lowest;
    int repeated;
    int chosen;
    int i;
    int j;
    tU8 index;

    repeated = HasBeenDoneBefore(pHistory, pCount, -1);
    chosen = -1;
    lowest = 0.0f;
    for (i = 0; i < *pCount_excluded; i++) {
        index = pExcluded[i];
        sum = 0.0f;
        for (j = 0; j < pSize; j++) {
            sum += pOriginal[index * 100 + j] * pResult[j];
        }
        sum -= pRhs[index];
        scaled = sum / pOriginal[index * 101];
        if (sum < -0.001 && scaled < lowest) {
            if (repeated != 0 && HasBeenDoneBefore(pHistory, pCount, index - pFirst) != 0) {
                if (chosen == -1) {
                    chosen = i;
                }
            } else {
                lowest = scaled;
                chosen = i;
            }
        }
    }
    if (chosen < 0) {
        return 0;
    }
    index = pExcluded[chosen];
    for (i = chosen + 1; i < *pCount_excluded; i++) {
        pExcluded[i - 1] = pExcluded[i];
    }
    *pCount_excluded -= 1;
    pExcluded[*pCount_excluded] = index;
    return 1;
}

// GetInverseM
inline void C2_HOOK_FASTCALL GetInverseM(const br_scalar* pMatrix, const br_scalar* pOrthogonal, br_scalar* pResult, int pSize, int pFirst) {
    br_scalar column[100];
    br_scalar sum;
    int i;
    int j;
    int k;

    for (i = pFirst; i < pSize; i++) {
        for (j = pSize - 1; j >= i; j--) {
            sum = 0.0f;
            for (k = pSize - 1; k > j; k--) {
                sum += column[k] * pMatrix[k * 100 + j];
            }
            column[j] = (pOrthogonal[j * 100 + i] - sum) / pMatrix[j * 101];
        }
        pResult[i] = column[i];
    }
}

#define PYTHAGORAS(RESULT, A, B)                                                         \
    do {                                                                                  \
        abs_a = (br_scalar)fabs(A);                                                       \
        abs_b = (br_scalar)fabs(B);                                                       \
        if (abs_a > abs_b) {                                                              \
            (RESULT) = (br_scalar)(abs_a * sqrt(1.0f + abs_b / abs_a * abs_b / abs_a));   \
        } else if (abs_b == 0.0) {                                                        \
            (RESULT) = (br_scalar)0.0;                                                    \
        } else {                                                                          \
            (RESULT) = (br_scalar)(abs_b * sqrt(1.0f + abs_a / abs_b * abs_a / abs_b));   \
        }                                                                                 \
    } while (0)

// FUNCTION: CARMA2_HW 0x004c2db0
// FUNCTION: CARMA2_SW 0x004bf4e0
// FUNCTION: CARMA2_CHEAT 0x004c88d0
int C2_HOOK_FASTCALL FUN_004c2db0(br_scalar** pA, int pCount_rows, int pCount_columns, br_scalar* pW, br_scalar** pV) {
    br_scalar rv1[256];
    br_scalar anorm;
    br_scalar scale;
    br_scalar abs_a;
    br_scalar abs_b;
    br_scalar c;
    br_scalar f;
    br_scalar g;
    br_scalar h;
    br_scalar s;
    br_scalar x;
    br_scalar y;
    br_scalar z;
    int flag;
    int its;
    int i;
    int column;
    int j;
    int jj;
    int k;
    int l;
    int nm;

    /* Singular value decomposition A = U W V^T (Householder bidiagonalisation and QR iteration):
     * pA (pCount_rows x pCount_columns, rows by pointer) is replaced by U, pW gets the singular values,
     * pV (pCount_columns x pCount_columns) gets V. Returns -1 when an iteration does not converge. */
    g = 0.0f;
    scale = 0.0f;
    anorm = 0.0f;
    for (i = 0; i < pCount_columns; i++) {
        l = i + 1;
        rv1[i] = scale * g;
        g = 0.0f;
        s = 0.0f;
        scale = 0.0f;
        if (i <= pCount_rows) {
            for (k = i; k < pCount_rows; k++) {
                scale += (br_scalar)fabs(pA[k][i]);
            }
            if (scale != 0.0f) {
                for (k = i; k < pCount_rows; k++) {
                    pA[k][i] /= scale;
                    s += pA[k][i] * pA[k][i];
                }
                f = pA[i][i];
                g = -(br_scalar)(f > 0.0 ? fabs(sqrt(s)) : -fabs(sqrt(s)));
                h = f * g - s;
                pA[i][i] = f - g;
                for (j = l; j < pCount_columns; j++) {
                    s = 0.0f;
                    for (k = i; k < pCount_rows; k++) {
                        s += pA[k][i] * pA[k][j];
                    }
                    f = s / h;
                    for (k = i; k < pCount_rows; k++) {
                        pA[k][j] += f * pA[k][i];
                    }
                }
                for (k = i; k < pCount_rows; k++) {
                    pA[k][i] *= scale;
                }
            }
        }
        pW[i] = scale * g;
        g = 0.0f;
        s = 0.0f;
        scale = 0.0f;
        if (i < pCount_rows && i != pCount_columns - 1) {
            for (k = l; k < pCount_columns; k++) {
                scale += (br_scalar)fabs(pA[i][k]);
            }
            if (scale != 0.0f) {
                for (k = l; k < pCount_columns; k++) {
                    pA[i][k] /= scale;
                    s += pA[i][k] * pA[i][k];
                }
                f = pA[i][l];
                g = -(br_scalar)(f > 0.0 ? fabs(sqrt(s)) : -fabs(sqrt(s)));
                h = f * g - s;
                pA[i][l] = f - g;
                for (k = l; k < pCount_columns; k++) {
                    rv1[k] = pA[i][k] / h;
                }
                for (j = l; j < pCount_rows; j++) {
                    s = 0.0f;
                    for (k = l; k < pCount_columns; k++) {
                        s += pA[j][k] * pA[i][k];
                    }
                    for (k = l; k < pCount_columns; k++) {
                        pA[j][k] += s * rv1[k];
                    }
                }
                for (k = l; k < pCount_columns; k++) {
                    pA[i][k] *= scale;
                }
            }
        }
        anorm = (br_scalar)MAX(anorm, fabs(pW[i]) + fabs(rv1[i]));
    }
    for (column = pCount_columns - 1; column >= 0; column--) {
        if (column < pCount_columns - 1) {
            if (g != 0.0f) {
                for (j = l; j < pCount_columns; j++) {
                    pV[j][column] = pA[column][j] / pA[column][l] / g;
                }
                for (j = l; j < pCount_columns; j++) {
                    s = 0.0f;
                    for (k = l; k < pCount_columns; k++) {
                        s += pA[column][k] * pV[k][j];
                    }
                    for (k = l; k < pCount_columns; k++) {
                        pV[k][j] += s * pV[k][column];
                    }
                }
            }
            for (j = l; j < pCount_columns; j++) {
                pV[j][column] = 0.0f;
                pV[column][j] = 0.0f;
            }
        }
        pV[column][column] = 1.0f;
        g = rv1[column];
        l = column;
    }
    for (i = MIN(pCount_rows, pCount_columns) - 1; i >= 0; i--) {
        l = i + 1;
        g = pW[i];
        for (j = l; j < pCount_columns; j++) {
            pA[i][j] = 0.0f;
        }
        if (g != 0.0f) {
            g = (br_scalar)(1.0 / g);
            for (j = l; j < pCount_columns; j++) {
                s = 0.0f;
                for (k = l; k < pCount_rows; k++) {
                    s += pA[k][i] * pA[k][j];
                }
                f = s / pA[i][i] * g;
                for (k = i; k < pCount_rows; k++) {
                    pA[k][j] += f * pA[k][i];
                }
            }
            for (j = i; j < pCount_rows; j++) {
                pA[j][i] *= g;
            }
        } else {
            for (j = i; j < pCount_rows; j++) {
                pA[j][i] = 0.0f;
            }
        }
        pA[i][i] += 1.0f;
    }
    for (k = pCount_columns - 1; k >= 0; k--) {
        for (its = 1; its <= 30; its++) {
            flag = 1;
            for (l = k; l >= 0; l--) {
                nm = l - 1;
                if ((br_scalar)fabs(rv1[l]) + anorm == anorm) {
                    flag = 0;
                    break;
                }
                if ((br_scalar)fabs(pW[nm]) + anorm == anorm) {
                    break;
                }
            }
            if (flag != 0) {
                c = 0.0f;
                s = 1.0f;
                for (i = l; i <= k; i++) {
                    f = s * rv1[i];
                    rv1[i] = c * rv1[i];
                    if ((br_scalar)fabs(f) + anorm == anorm) {
                        break;
                    }
                    g = pW[i];
                    PYTHAGORAS(h, f, g);
                    pW[i] = h;
                    h = (br_scalar)(1.0 / h);
                    c = g * h;
                    s = -(f * h);
                    for (j = 0; j < pCount_rows; j++) {
                        y = pA[j][nm];
                        z = pA[j][i];
                        pA[j][nm] = y * c + z * s;
                        pA[j][i] = z * c - y * s;
                    }
                }
            }
            z = pW[k];
            if (l == k) {
                if (z < 0.0) {
                    pW[k] = -z;
                    for (j = 0; j < pCount_columns; j++) {
                        pV[j][k] = -pV[j][k];
                    }
                }
                break;
            }
            if (its == 30) {
                return -1;
            }
            x = pW[l];
            nm = k - 1;
            y = pW[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0f * h * y);
            PYTHAGORAS(g, f, 1.0);
            f = ((x - z) * (x + z) + h * (y / (f + (f > 0.0 ? fabs(g) : -fabs(g))) - h)) / x;
            c = 1.0f;
            s = 1.0f;
            for (j = l; j <= nm; j++) {
                i = j + 1;
                g = rv1[i];
                y = pW[i];
                h = s * g;
                g = c * g;
                PYTHAGORAS(z, f, h);
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y *= c;
                for (jj = 0; jj < pCount_columns; jj++) {
                    x = pV[jj][j];
                    z = pV[jj][i];
                    pV[jj][j] = x * c + z * s;
                    pV[jj][i] = z * c - x * s;
                }
                PYTHAGORAS(z, f, h);
                pW[j] = z;
                if (z != 0.0f) {
                    z = (br_scalar)(1.0 / z);
                    c = f * z;
                    s = h * z;
                }
                f = c * g + s * y;
                x = c * y - s * g;
                for (jj = 0; jj < pCount_rows; jj++) {
                    y = pA[jj][j];
                    z = pA[jj][i];
                    pA[jj][j] = y * c + z * s;
                    pA[jj][i] = z * c - y * s;
                }
            }
            rv1[l] = 0.0f;
            rv1[k] = f;
            pW[k] = x;
        }
    }
    return 0;
}

#undef PYTHAGORAS

// FUNCTION: CARMA2_HW 0x004c3b60
// FUNCTION: CARMA2_SW 0x004c0290
// FUNCTION: CARMA2_CHEAT 0x004c9680
int C2_HOOK_FASTCALL RQMatrixSolveNoNegatives(const br_scalar* pOriginal, br_scalar* pRhs, br_scalar* pResult, int pSize, int pRow_size, tU8* pExcluded, int* pCount_excluded, int pFirst, int pUnused, tForce_group** pGroups, int pCount_groups) {
    tU8 indices[100];
    br_scalar forward[100];
    br_scalar active_rhs[100];
    br_scalar forces[100];
    tU32 history[60][2];
    tU32 mask[2];
    br_scalar sum;
    br_scalar limit;
    br_scalar lowest;
    br_scalar biggest;
    int count;
    int matrix_count;
    int iteration;
    int last;
    int removed;
    int negative;
    int boundary;
    int bit;
    int i;
    int j;

    removed = -1;
#ifdef REC2_FIX_BUGS
    negative = -1;
#endif
    *pCount_excluded = 0;
    if (pSize == 1) {
        if (pOriginal[0] > 0.0001) {
            pResult[0] = pRhs[0] / pOriginal[0];
        } else {
            pResult[0] = 0.0f;
        }
        return 0;
    }
    for (i = 0; i < pSize; i++) {
        indices[i] = i;
    }
    count = pFirst + 1;
    matrix_count = count;
    *pCount_excluded = pSize - pFirst - 1;
    for (i = 0; i < *pCount_excluded; i++) {
        pExcluded[i] = i + pFirst + 1;
    }
    MakeMatrixFromIndices(gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, pOriginal, active_rhs, pRhs, indices, count, pGroups, pCount_groups);
    mask[1] = 0;
    mask[0] = 0;
    mask[0] |= 1;
    for (iteration = 0;; iteration++) {
        if (iteration >= 50 && negative == -1) {
            PhysicsWarning("Matrix solve loop overflow");
            return -1;
        }
        history[iteration][0] = mask[0];
        history[iteration][1] = mask[1];
        forward[0] = active_rhs[0] / gPhysics_buffer1_ptr[0];
        for (i = 1; i < count; i++) {
            sum = 0.0f;
            for (j = 0; j < i; j++) {
                sum += forward[j] * gPhysics_buffer1_ptr[i * 100 + j];
            }
            forward[i] = (active_rhs[i] - sum) / gPhysics_buffer1_ptr[i * 101];
        }
        last = count - 1;
        limit = (br_scalar)fabs(gPhysics_buffer1_ptr[last * 101] * 100000.0f);
        for (j = 0; j < last; j++) {
            if (fabs(gPhysics_buffer1_ptr[last * 100 + j]) > limit) {
                break;
            }
        }
        if (j != last) {
            removed = RemoveRQSingularities(last, gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, active_rhs, forward, pFirst, history, iteration, indices);
            pExcluded[*pCount_excluded] = indices[removed];
            bit = indices[removed] - pFirst;
            mask[bit >> 5] &= ~(1 << (bit & 0x1f));
            *pCount_excluded += 1;
            for (i = removed; i < last; i++) {
                indices[i] = indices[i + 1];
            }
            count = last;
            matrix_count = last;
            MakeMatrixFromIndices(gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, pOriginal, active_rhs, pRhs, indices, count, pGroups, pCount_groups);
        } else {
            for (i = 0; i < count; i++) {
                forces[i] = 0.0f;
                for (j = 0; j < count; j++) {
                    forces[i] += gPhysics_buffer2_ptr[j * 100 + i] * forward[j];
                }
            }
            negative = -1;
            if (removed >= 0) {
                removed = -1;
                if (pRhs[pExcluded[*pCount_excluded - 1]] == 0.0f) {
                    pRhs[pExcluded[*pCount_excluded - 1]] = -10000.0f;
                }
            }
            lowest = -0.0001f;
            for (i = pFirst; i < count; i++) {
                if (forces[i] < lowest) {
                    lowest = forces[i];
                    negative = i;
                }
            }
            if (negative >= 0) {
                if (forces[count - 1] < 0.0f) {
                    boundary = 0;
                } else {
                    for (boundary = last; boundary >= matrix_count; boundary--) {
                        if (forces[boundary] > 0.0f) {
                            boundary--;
                            break;
                        }
                    }
                    boundary++;
                }
                if (negative < boundary) {
                    pExcluded[*pCount_excluded] = indices[negative];
                    bit = indices[negative] - pFirst;
                    mask[bit >> 5] &= ~(1 << (bit & 0x1f));
                    *pCount_excluded += 1;
                    indices[negative] = 0xff;
                } else {
                    biggest = 0.0f;
                    negative = pFirst;
                    GetInverseM(gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, forward, count, pFirst);
                    for (i = pFirst; i < count; i++) {
                        if (forces[i] / forward[i] < 0.0f && fabs(forward[i]) > biggest) {
                            biggest = (br_scalar)fabs(forward[i]);
                            negative = i;
                        }
                    }
                    pExcluded[*pCount_excluded] = indices[negative];
                    bit = indices[negative] - pFirst;
                    mask[bit >> 5] &= ~(1 << (bit & 0x1f));
                    *pCount_excluded += 1;
                    indices[negative] = 0xff;
                }
                matrix_count = 0;
                for (i = 0; i < count; i++) {
                    if (indices[i] != 0xff) {
                        indices[matrix_count] = indices[i];
                        active_rhs[matrix_count] = active_rhs[i];
                        matrix_count++;
                    }
                }
                count = matrix_count;
                MakeMatrixFromIndices(gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, pOriginal, active_rhs, pRhs, indices, count, pGroups, pCount_groups);
            } else {
                for (i = 0; i < pSize; i++) {
                    pResult[i] = 0.0f;
                }
                for (i = 0; i < count; i++) {
                    pResult[indices[i]] = forces[i];
                }
                if (RQRemovedOneTooManyForces(pCount_excluded, pExcluded, pSize, pResult, pRhs, pOriginal, history, iteration, pFirst) == 0) {
                    if (TestForWrongAnswer(pOriginal, pRhs, pResult, pSize) != 0) {
                        RQIterativeImprovement(forces, gPhysics_buffer1_ptr, gPhysics_buffer2_ptr, active_rhs, pOriginal, indices, count);
                    }
                    return 0;
                }
                bit = pExcluded[*pCount_excluded] - pFirst;
                mask[bit >> 5] |= 1 << (bit & 0x1f);
                AddToMatrix(pExcluded[*pCount_excluded], gPhysics_buffer1_ptr, pOriginal, active_rhs, pRhs, gPhysics_buffer2_ptr, indices, &count);
            }
        }
    }
}

// RQMatrixSolveWithNegatives

// FUNCTION: CARMA2_HW 0x004c5b50
// FUNCTION: CARMA2_SW 0x004c2280
int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim) {
    return RQDcmpV(pR, pDim, pQ);
}

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

// FUNCTION: CARMA2_HW 0x004c5c70
void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    return gPhysics_scratch_ptr;
}

// FUNCTION: CARMA2_HW 0x004c5cb0
void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    gPhysics_scratch_ptr = (tU8*)gPhysics_scratch_ptr + pSize;
}

// FUNCTION: CARMA2_HW 0x004c5d00
// FUNCTION: CARMA2_SW 0x004c2430
void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void) {

    gPhysics_scratch_ptr = gPhysics_other_buffer;
}

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

    gPhysics_buffer1_ptr = (br_scalar*)pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = (br_scalar*)(pBuffer + PHYSICS_BUFFER_PART_SIZE);
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

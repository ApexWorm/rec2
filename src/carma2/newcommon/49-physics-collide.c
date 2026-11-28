/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "49-physics-collide.h"

#include "49-physics.h"
#include "00-car.h"
#include "09-funks.h"
#include "16-graphics1.h"
#include "22-replay.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "49-physics-support.h"
#include "52-errors.h"
#include "65-hull.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "c2_math.h"
#include "c2_string.h"
#include "c2_stdlib.h"

#include <stddef.h>

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

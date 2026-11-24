/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "65-hull.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x0067942c
// GLOBAL: CARMA2_SW 0x00673b9c
tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// GLOBAL: CARMA2_HW 0x00679420
// GLOBAL: CARMA2_SW 0x00673b90
// GLOBAL: CARMA2_CHEAT 0x00680cd0
br_vector3 gPhysics_reference_normal_comparison;

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00420910
// STUB: CARMA2_SW 0x00420990
// STUB: CARMA2_CHEAT 0x00420b70
tPhysicsError C2_HOOK_FASTCALL ConvexHull2D(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00420d60
// FUNCTION: CARMA2_CHEAT 0x00420fc0
br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2) {
    br_scalar x;
    br_scalar y;
    br_scalar z;

    y = pV1->v[2] * pV2->v[0] - pV1->v[0] * pV2->v[2];
    x = pV1->v[1] * pV2->v[2] - pV1->v[2] * pV2->v[1];
    z = pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
    return x * gPhysics_reference_normal_comparison.v[0] + y * gPhysics_reference_normal_comparison.v[1] + z * gPhysics_reference_normal_comparison.v[2];
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x00420dc0
// FUNCTION: CARMA2_SW 0x00420e40
// FUNCTION: CARMA2_CHEAT 0x00421010
int C2_HOOK_CDECL CompareAngle(const void *pLhs, const void *pRhs) {
    tCollision_shape_polyhedron_data* polyhedron;
    br_scalar s1, s2;
    br_vector3 tv1, tv2;
    const int* const pIndex1 = pLhs;
    const int* const pIndex2 = pRhs;

    polyhedron = gPolyhedron_to_sort;
    BrVector3Sub(&tv1, &polyhedron->points[*pIndex1], &polyhedron->points[0]);
    BrVector3Sub(&tv2, &polyhedron->points[*pIndex2], &polyhedron->points[0]);
    s1 = SignedTwiceAreaOf3DTriangle(&tv1, &tv2);
    if (s1 > 1e-6f) {
        return -1;
    }
    if (s1 < -1e-6f) {
        return 1;
    }
    s1 = BrVector3Length(&tv1);
    s2 = BrVector3Length(&tv2);
    if (s1 < s2) {
        return -1;
    }
    if (s1 > s2) {
        return 1;
    }
    return 0;
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// FUNCTION: CARMA2_HW 0x00420ef0
// FUNCTION: CARMA2_SW 0x00420f70
// FUNCTION: CARMA2_CHEAT 0x00421140
int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3) {

    return (float)fabs((pV3->v[2] - pV1->v[2]) * (pV2->v[1] - pV1->v[1]) - (pV3->v[1] - pV1->v[1]) * (pV2->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV3->v[0] - pV1->v[0]) * (pV2->v[2] - pV1->v[2]) - (pV2->v[0] - pV1->v[0]) * (pV3->v[2] - pV1->v[2])) < 1e-6f
        && (float)fabs((pV2->v[0] - pV1->v[0]) * (pV3->v[1] - pV1->v[1]) - (pV3->v[0] - pV1->v[0]) * (pV2->v[1] - pV1->v[1])) < 1e-6f;
}

// Left

// DRVector4Negate

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004211f0
// STUB: CARMA2_SW 0x00421270
// STUB: CARMA2_CHEAT 0x00421440
tPhysicsError C2_HOOK_FASTCALL ConstructTetrahedron(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdge_planes) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CheckSignOfNormal

// AddNewEdge

// PointOutsidePlane

// AdjustEdgePlanes

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00421790
// STUB: CARMA2_SW 0x00421810
// STUB: CARMA2_CHEAT 0x004219e0
int C2_HOOK_FASTCALL AddPointToPolyhedron(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pPoint, tU8 pPoint_index, tPolyhedron_edge_indexes* pEdge_planes) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING

// FUNCTION: CARMA2_HW 0x00421fd0
// FUNCTION: CARMA2_SW 0x00422050
// FUNCTION: CARMA2_CHEAT 0x00422220
tFUN_00421fd0* C2_HOOK_FASTCALL FUN_00421fd0(int pCount_points) {
    /* Unused function */
    tFUN_00421fd0* result;
    int count_faces;
    int count_indices;

    count_faces = pCount_points - 2;
    count_indices = count_faces * 3;

    /* FIXME: replace "32" with sizeof(some_struct) */
    result = BrMemAllocate(sizeof(tU16) * count_indices + 32 * count_faces + sizeof(br_vector3) * pCount_points + sizeof(tFUN_00421fd0), BR_MEMORY_APPLICATION);
    if (result == NULL) {
        return NULL;
    }
    result->field_0x0c = (br_vector3*)(result + 1);
    result->field_0x10 = (tU16*)(result->field_0x0c + pCount_points);
    result->field_0x14 = (undefined*)(result->field_0x10 + count_indices);
    result->field_0x00 = 0;
    result->field_0x04 = 0;
    result->field_0x08 = 0;
    return result;
}
#endif

static void C2_HOOK_FASTCALL AdjustEdges(tCollision_shape_polyhedron_data* pPolyhedron, tU8 pOld_index, tU8 pNew_index) {
    int i;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (pPolyhedron->edges[i].index1 == pOld_index) {
            pPolyhedron->edges[i].index1 = pNew_index;
        } else if (pPolyhedron->edges[i].index2 == pOld_index) {
            pPolyhedron->edges[i].index2 = pNew_index;
        }
    }
}

static int C2_HOOK_FASTCALL PointOnShape(tCollision_shape_polyhedron_data* pPolyhedron, tU8 pIndex) {
    int i;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (pPolyhedron->edges[i].index1 == pIndex) {
            return 1;
        }
        if (pPolyhedron->edges[i].index2 == pIndex) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00420fb0
// FUNCTION: CARMA2_SW 0x00421030
// FUNCTION: CARMA2_CHEAT 0x00421200
tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron) {
    tU8 count;
    tU8 i;
    int original_count_points;
    tPolyhedron_edge_indexes edge_planes[894];
    br_vector3 original_points[300];
    tPhysicsError error;

    if (pPolyhedron->count_points > 300) {
        pPolyhedron->count_points = 300;
    }
    memcpy(original_points, pPolyhedron->points, pPolyhedron->count_points * sizeof(br_vector3));
    original_count_points = pPolyhedron->count_points;
    error = ConstructTetrahedron(pPolyhedron, edge_planes);
    if (error != ePhysicsError_Ok) {
        if (error == ePhysicsError_UnknownShapeType) {
            return ePhysicsError_UnknownShapeType;
        }
        pPolyhedron->count_points = original_count_points;
        memcpy(pPolyhedron->points, original_points, pPolyhedron->count_points * sizeof(br_vector3));
        error = ConvexHull2D(pPolyhedron);
        if (error == ePhysicsError_Ok) {
            return ePhysicsError_WireFrameHasNoPoints;
        }
        return error;
    }
    count = 4;
    for (i = 0; i < original_count_points; i++) {
        if (AddPointToPolyhedron(pPolyhedron, &original_points[i], count, edge_planes)) {
            if (count != i) {
                BrVector3Copy(&pPolyhedron->points[count], &original_points[i]);
            }
            count++;
        }
    }
    pPolyhedron->count_points = count;
    count = 0;
    for (i = 0; i < pPolyhedron->count_points; i++) {
        if (PointOnShape(pPolyhedron, i)) {
            if (count != i) {
                BrVector3Copy(&pPolyhedron->points[count], &pPolyhedron->points[i]);
                AdjustEdges(pPolyhedron, i, count);
            }
            count++;
        }
    }
    pPolyhedron->count_points = count;
    return ePhysicsError_Ok;
}

// GetPointSample

// SampledConvexHull

// NPointConvexHullUsingSample


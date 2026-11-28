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

// FUNCTION: CARMA2_HW 0x004211f0
// FUNCTION: CARMA2_SW 0x00421270
// FUNCTION: CARMA2_CHEAT 0x00421440
tPhysicsError C2_HOOK_FASTCALL ConstructTetrahedron(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdge_planes) {
    int i;
    br_vector3 tv1;
    br_vector3 tv2;

    i = 0;
    while (Collinear(&pPolyhedron->points[i], &pPolyhedron->points[i + 1], &pPolyhedron->points[i + 2])) {
        i++;
        if (i == pPolyhedron->count_points - 3) {
            return ePhysicsError_UnknownShapeType;
        }
    }
    if (i != 0) {
        BrVector3Copy(&pPolyhedron->points[0], &pPolyhedron->points[i]);
        BrVector3Copy(&pPolyhedron->points[1], &pPolyhedron->points[i + 1]);
        BrVector3Copy(&pPolyhedron->points[2], &pPolyhedron->points[i + 2]);
    }
    pPolyhedron->edges[0].index1 = 0;
    pPolyhedron->edges[0].index2 = 1;
    pPolyhedron->edges[1].index1 = 1;
    pPolyhedron->edges[1].index2 = 2;
    pPolyhedron->edges[2].index1 = 0;
    pPolyhedron->edges[2].index2 = 2;
    BrVector3Sub(&tv1, &pPolyhedron->points[2], &pPolyhedron->points[1]);
    BrVector3Sub(&tv2, &pPolyhedron->points[1], &pPolyhedron->points[0]);
    BrVector3Cross(&pPolyhedron->planes[0], &tv2, &tv1);
    BrVector3Normalise(&pPolyhedron->planes[0], &pPolyhedron->planes[0]);
    pPolyhedron->planes[0].v[3] = -BrVector3Dot(&pPolyhedron->planes[0], &pPolyhedron->points[0]);
    pEdge_planes[2].index1 = 0;
    pEdge_planes[1].index1 = 0;
    pEdge_planes[0].index1 = 0;
    for (i = 0; i < pPolyhedron->count_points; i++) {
        if ((br_scalar)fabs(BrVector3Dot(&pPolyhedron->planes[0], &pPolyhedron->points[i]) + pPolyhedron->planes[0].v[3]) > 1e-6f) {
            break;
        }
    }
    if (i == pPolyhedron->count_points) {
        return ePhysicsError_error7;
    }
    BrVector3Copy(&pPolyhedron->points[3], &pPolyhedron->points[i]);
    if (BrVector3Dot(&pPolyhedron->planes[0], &pPolyhedron->points[i]) + pPolyhedron->planes[0].v[3] < 1e-6f) {
        BrVector4Negate(&pPolyhedron->planes[0], &pPolyhedron->planes[0]);
    }
    for (i = 0; i < 3; i++) {
        pPolyhedron->edges[3 + i].index1 = i;
        pPolyhedron->edges[3 + i].index2 = 3;
        BrVector3Sub(&tv1, &pPolyhedron->points[3], &pPolyhedron->points[i]);
        BrVector3Sub(&tv2, &pPolyhedron->points[(i + 1) % 3], &pPolyhedron->points[i]);
        BrVector3Cross(&pPolyhedron->planes[i + 1], &tv1, &tv2);
        BrVector3Normalise(&pPolyhedron->planes[i + 1], &pPolyhedron->planes[i + 1]);
        pPolyhedron->planes[i + 1].v[3] = -BrVector3Dot(&pPolyhedron->points[3], &pPolyhedron->planes[i + 1]);
        pEdge_planes[i].index2 = i + 1;
        pEdge_planes[3 + i].index1 = i + 1;
        pEdge_planes[3 + i].index2 = (i + 2) % 3 + 1;
    }
    for (i = 0; i < 3; i++) {
        if (BrVector3Dot(&pPolyhedron->planes[i + 1], &pPolyhedron->points[(i + 2) % 3]) + pPolyhedron->planes[i + 1].v[3] < 1e-6f) {
            BrVector4Negate(&pPolyhedron->planes[i + 1], &pPolyhedron->planes[i + 1]);
        }
    }
    pPolyhedron->count_points = 4;
    pPolyhedron->count_planes = 4;
    pPolyhedron->count_edges = 6;
    return ePhysicsError_Ok;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL CheckSignOfNormal(br_vector4* pNormal, const br_vector4* pPlane1, const br_vector4* pPlane2) {

    if (BrVector3Dot(pPlane1, pPlane1) * BrVector3Dot(pNormal, pPlane2) - BrVector3Dot(pPlane1, pPlane2) * BrVector3Dot(pPlane1, pNormal) < 0.0f) {
        BrVector4Negate(pNormal, pNormal);
    }
}

inline void C2_HOOK_FASTCALL AddNewEdge(tPolyhedron_edge_indexes* pEdges, int* pCount_edges, tU8 pIndex1, tU8 pIndex2, tU8 pPlane, tPolyhedron_edge_indexes* pEdge_planes) {
    int i;

    for (i = 0; i < *pCount_edges; i++) {
        if (pEdges[i].index1 == pIndex1 && pEdges[i].index2 == pIndex2) {
            pEdge_planes[i].index2 = pPlane;
            return;
        }
    }
    pEdges[*pCount_edges].index1 = pIndex1;
    pEdges[*pCount_edges].index2 = pIndex2;
    pEdge_planes[*pCount_edges].index1 = pPlane;
    *pCount_edges += 1;
}

inline tPolyhedron_plane_side C2_HOOK_FASTCALL PointOutsidePlane(const br_vector4* pPlane, const br_vector3* pPoint) {
    br_scalar d;

    d = BrVector3Dot(pPlane, pPoint) + pPlane->v[3];
    if (d < -1e-6f) {
        return ePolyhedron_plane_side_outside;
    } else if (d < 1e-6f) {
        return ePolyhedron_plane_side_on;
    } else {
        return ePolyhedron_plane_side_inside;
    }
}

inline void C2_HOOK_FASTCALL AdjustEdgePlanes(tCollision_shape_polyhedron_data* pPolyhedron, tU8 pOld_index, tU8 pNew_index, tPolyhedron_edge_indexes* pEdge_planes) {
    int i;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (pOld_index == pEdge_planes[i].index1) {
            pEdge_planes[i].index1 = pNew_index;
        } else if (pOld_index == pEdge_planes[i].index2) {
            pEdge_planes[i].index2 = pNew_index;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00421790
// FUNCTION: CARMA2_SW 0x00421810
// FUNCTION: CARMA2_CHEAT 0x004219e0
int C2_HOOK_FASTCALL AddPointToPolyhedron(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pPoint, tU8 pPoint_index, tPolyhedron_edge_indexes* pEdge_planes) {
    tPolyhedron_plane_side sides[596];
    int keep_edges[894];
    br_vector4 new_planes[596];
    tPolyhedron_edge_indexes new_edges[894];
    tPolyhedron_edge_indexes new_edge_planes[894];
    int all_inside;
    int count_new_planes;
    int count_new_edges;
    tU8 i;
    tU8 count;
    int plane1;
    int plane2;
    tPolyhedron_plane_side side1;
    tPolyhedron_plane_side side2;
    tPolyhedron_edge_indexes* edge_planes;
    br_vector4* new_plane;
    br_vector3* v1;
    br_vector3* v2;
    br_vector3 tv1;
    br_vector3 tv2;

    all_inside = 1;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        sides[i] = PointOutsidePlane(&pPolyhedron->planes[i], pPoint);
        all_inside &= (sides[i] & ePolyhedron_plane_side_outside) == 0;
    }
    if (all_inside) {
        return 0;
    }
    count_new_planes = 0;
    count_new_edges = 0;
    for (i = 0; i < pPolyhedron->count_edges; i++) {
        edge_planes = &pEdge_planes[i];
        plane1 = edge_planes->index1;
        plane2 = edge_planes->index2;
        side1 = sides[plane1];
        side2 = sides[plane2];
        if (side1 != ePolyhedron_plane_side_inside && side2 != ePolyhedron_plane_side_inside) {
            keep_edges[i] = 0;
        } else {
            keep_edges[i] = 1;
        }
        if ((side1 & ePolyhedron_plane_side_outside) != (side2 & ePolyhedron_plane_side_outside)) {
            if (!(side1 & ePolyhedron_plane_side_on) && !(side2 & ePolyhedron_plane_side_on)) {
                new_plane = &new_planes[count_new_planes];
                v1 = &pPolyhedron->points[pPolyhedron->edges[i].index1];
                v2 = &pPolyhedron->points[pPolyhedron->edges[i].index2];
                BrVector3Sub(&tv1, pPoint, v1);
                BrVector3Sub(&tv2, v2, v1);
                BrVector3Cross(new_plane, &tv1, &tv2);
                BrVector3Normalise(new_plane, new_plane);
                CheckSignOfNormal(new_plane, &pPolyhedron->planes[edge_planes->index1], &pPolyhedron->planes[edge_planes->index2]);
                new_plane->v[3] = -BrVector3Dot(pPoint, new_plane);
                if (side1 == ePolyhedron_plane_side_inside) {
                    edge_planes->index2 = count_new_planes + pPolyhedron->count_planes;
                } else {
                    edge_planes->index1 = count_new_planes + pPolyhedron->count_planes;
                }
                AddNewEdge(new_edges, &count_new_edges, pPolyhedron->edges[i].index1, pPoint_index, count_new_planes + pPolyhedron->count_planes, new_edge_planes);
                AddNewEdge(new_edges, &count_new_edges, pPolyhedron->edges[i].index2, pPoint_index, count_new_planes + pPolyhedron->count_planes, new_edge_planes);
                count_new_planes++;
            } else {
                if (side1 & ePolyhedron_plane_side_on) {
                    plane2 = plane1;
                }
                AddNewEdge(new_edges, &count_new_edges, pPolyhedron->edges[i].index1, pPoint_index, plane2, new_edge_planes);
                AddNewEdge(new_edges, &count_new_edges, pPolyhedron->edges[i].index2, pPoint_index, plane2, new_edge_planes);
            }
        }
    }
    count = 0;
    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (keep_edges[i]) {
            if (count != i) {
                pPolyhedron->edges[count] = pPolyhedron->edges[i];
                pEdge_planes[count] = pEdge_planes[i];
            }
            count++;
        }
    }
    for (i = 0; i < count_new_edges; i++) {
        pPolyhedron->edges[count] = new_edges[i];
        pEdge_planes[count] = new_edge_planes[i];
        count++;
    }
    pPolyhedron->count_edges = count;
    count = 0;
    for (i = 0; i < pPolyhedron->count_planes; i++) {
        if ((sides[i] & ePolyhedron_plane_side_outside) == 0) {
            if (count != i) {
                BrVector4Copy(&pPolyhedron->planes[count], &pPolyhedron->planes[i]);
                AdjustEdgePlanes(pPolyhedron, i, count, pEdge_planes);
            }
            count++;
        }
    }
    for (i = 0; i < count_new_planes; i++) {
        BrVector4Copy(&pPolyhedron->planes[count], &new_planes[i]);
        AdjustEdgePlanes(pPolyhedron, i + pPolyhedron->count_planes, count, pEdge_planes);
        count++;
    }
    pPolyhedron->count_planes = count;
    return 1;
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

inline void C2_HOOK_FASTCALL AdjustEdges(tCollision_shape_polyhedron_data* pPolyhedron, tU8 pOld_index, tU8 pNew_index) {
    int i;

    for (i = 0; i < pPolyhedron->count_edges; i++) {
        if (pPolyhedron->edges[i].index1 == pOld_index) {
            pPolyhedron->edges[i].index1 = pNew_index;
        } else if (pPolyhedron->edges[i].index2 == pOld_index) {
            pPolyhedron->edges[i].index2 = pNew_index;
        }
    }
}

inline int C2_HOOK_FASTCALL PointOnShape(tCollision_shape_polyhedron_data* pPolyhedron, tU8 pIndex) {
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


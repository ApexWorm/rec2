/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "65-hull.h"

// GLOBAL: CARMA2_HW 0x0067942c
// GLOBAL: CARMA2_SW 0x00673b9c
tCollision_shape_polyhedron_data* gPolyhedron_to_sort;

// GLOBAL: CARMA2_HW 0x00679420
// GLOBAL: CARMA2_SW 0x00673b90
// GLOBAL: CARMA2_CHEAT 0x00680cd0
br_vector3 gPhysics_reference_normal_comparison;

// STUB: CARMA2_HW 0x00420910
// STUB: CARMA2_SW 0x00420990
// STUB: CARMA2_CHEAT 0x00420b70
tPhysicsError C2_HOOK_FASTCALL ConvexHull2D(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

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

// ConstructTetrahedron

// CheckSignOfNormal

// AddNewEdge

// PointOutsidePlane

// AdjustEdgePlanes

// AddPointToPolyhedron

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

// AdjustEdges

// PointOnShape

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00420fb0
// STUB: CARMA2_SW 0x00421030
// STUB: CARMA2_CHEAT 0x00421200
tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00422030
// FUNCTION: CARMA2_SW 0x004220b0
// FUNCTION: CARMA2_CHEAT 0x00422280
tPhysicsError C2_HOOK_FASTCALL GetPointSample(br_model* pModel, int pCount, int* pIndices) {
    int count;
    int remaining;
    int i;
    int j;
    int k;
    int index;
    int best_i;
    int best_j;
    br_scalar best;
    br_scalar sum_i;
    br_scalar sum_j;
    br_scalar* distances;
    int removed[512];

    count = pModel->nvertices;
    if (count > 512) {
        count = 512;
    }
    if (pCount >= count) {
        return ePhysicsError_PolyhedronHasNoPoints;
    }
    distances = BrMemAllocate(count * count / 2 * sizeof(br_scalar), BR_MEMORY_APPLICATION);
    if (distances == NULL) {
        /* FIXME: tPhysicsError has no name for 4 yet */
        return 4;
    }
    k = 0;
    for (i = 0; i < count; i++) {
        for (j = 0; j < i; j++) {
            distances[k] = BR_SQR3(pModel->vertices[i].p.v[0] - pModel->vertices[j].p.v[0], pModel->vertices[i].p.v[1] - pModel->vertices[j].p.v[1], pModel->vertices[i].p.v[2] - pModel->vertices[j].p.v[2]);
            k++;
        }
    }
    for (i = 0; i < count; i++) {
        removed[i] = 0;
    }
    for (remaining = count - pCount; remaining != 0; remaining--) {
        best = BR_SCALAR_MAX;
        for (i = 0; i < count; i++) {
            if (!removed[i]) {
                for (j = 0; j < i; j++) {
                    if (!removed[j]) {
                        if (j < i) {
                            index = ((i - 1) * i >> 1) + j;
                        } else {
                            index = ((j - 1) * j >> 1) + i;
                        }
                        if (distances[index] < best) {
                            best = distances[index];
                            best_i = i;
                            best_j = j;
                        }
                    }
                }
            }
        }
        sum_i = 0.0f;
        sum_j = 0.0f;
        for (k = 0; k < count; k++) {
            if (!removed[k] && k != best_i && k != best_j) {
                if (k < best_i) {
                    index = ((best_i - 1) * best_i >> 1) + k;
                } else {
                    index = ((k - 1) * k >> 1) + best_i;
                }
                sum_i += (br_scalar)sqrt(distances[index]);
                if (k < best_j) {
                    index = ((best_j - 1) * best_j >> 1) + k;
                } else {
                    index = ((k - 1) * k >> 1) + best_j;
                }
                sum_j += (br_scalar)sqrt(distances[index]);
            }
        }
        if (sum_j <= sum_i) {
            removed[best_j] = 1;
        } else {
            removed[best_i] = 1;
        }
    }
    for (i = 0; i < count; i++) {
        if (!removed[i]) {
            *pIndices = i;
            pIndices++;
        }
    }
    BrMemFree(distances);
    return ePhysicsError_Ok;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00422400
// FUNCTION: CARMA2_SW 0x00422480
// FUNCTION: CARMA2_CHEAT 0x00422650
tPhysicsError C2_HOOK_FASTCALL SampledConvexHull(tCollision_shape_polyhedron_data* pPolyhedron, int* pIndices, br_model* pModel, int pCount) {
    int i;
    tPhysicsError error;

    if (pModel->nvertices > pCount) {
        error = GetPointSample(pModel, pCount, pIndices);
        if (error != ePhysicsError_Ok) {
            return error;
        }
        pPolyhedron->count_points = pCount;
        for (i = 0; i < pCount; i++) {
            BrVector3Copy(&pPolyhedron->points[i], &pModel->vertices[pIndices[i]].p);
        }
    } else {
        pPolyhedron->count_points = pModel->nvertices;
        for (i = 0; i < pModel->nvertices; i++) {
            pIndices[i] = i;
            BrVector3Copy(&pPolyhedron->points[i], &pModel->vertices[i].p);
        }
    }
    return ConvexHull3D(pPolyhedron);
}

// FUNCTION: CARMA2_HW 0x00422500
// FUNCTION: CARMA2_SW 0x00422580
// FUNCTION: CARMA2_CHEAT 0x00422750
tPhysicsError C2_HOOK_FASTCALL NPointConvexHullUsingSample(tCollision_shape_polyhedron_data* pPolyhedron, br_model* pModel, int pMin_points, int pMax_points, int* pIndices) {
    tPhysicsError error;
    int count;

    if (pModel == NULL) {
        return ePhysicsError_UnknownHingeType;
    }
    if (pMax_points < 3) {
        return ePhysicsError_OutOfMemory;
    }
    if (pMax_points > pModel->nvertices) {
        pMax_points = pModel->nvertices;
    }
    if (pMin_points < 3) {
        pMin_points = 3;
    } else if (pMin_points > pMax_points) {
        pMin_points = pMax_points;
    }
    count = pMax_points;
    do {
        error = SampledConvexHull(pPolyhedron, pIndices, pModel, count);
        if (pPolyhedron->count_points == 0 || (error != ePhysicsError_Ok && error != ePhysicsError_WireFrameHasNoPoints)) {
            break;
        }
        count = count * pMin_points / pPolyhedron->count_points;
        if (pPolyhedron->count_points == pMin_points) {
            break;
        }
    } while (count < pMax_points);
    return error;
}


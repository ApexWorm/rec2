/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_65_HULL_H
#define GUARD_65_HULL_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tPhysicsError C2_HOOK_FASTCALL ConvexHull2D(tCollision_shape_polyhedron_data* pPolyhedron);

extern br_scalar C2_HOOK_FASTCALL SignedTwiceAreaOf3DTriangle(const br_vector3* pV1, const br_vector3* pV2);

extern int C2_HOOK_CDECL CompareAngle(const void *pLhs, const void *pRhs);

extern int C2_HOOK_FASTCALL Collinear(const br_vector3* pV1, const br_vector3* pV2, const br_vector3* pV3);

// Left

// DRVector4Negate

extern tPhysicsError C2_HOOK_FASTCALL ConstructTetrahedron(tCollision_shape_polyhedron_data* pPolyhedron, tPolyhedron_edge_indexes* pEdge_planes);

// CheckSignOfNormal

// AddNewEdge

// PointOutsidePlane

// AdjustEdgePlanes

extern int C2_HOOK_FASTCALL AddPointToPolyhedron(tCollision_shape_polyhedron_data* pPolyhedron, br_vector3* pPoint, tU8 pPoint_index, tPolyhedron_edge_indexes* pEdge_planes);

#ifdef REC2_MATCHING
typedef struct {
    undefined4 field_0x00;
    undefined4 field_0x04;
    undefined4 field_0x08;
    br_vector3* field_0x0c;
    tU16* field_0x10;
    undefined* field_0x14;
} tFUN_00421fd0;

extern tFUN_00421fd0* C2_HOOK_FASTCALL FUN_00421fd0(int pCount_points);
#endif

// AdjustEdges

// PointOnShape

extern tPhysicsError C2_HOOK_FASTCALL ConvexHull3D(tCollision_shape_polyhedron_data* pPolyhedron);

// GetPointSample

// SampledConvexHull

// NPointConvexHullUsingSample

#endif // GUARD_65_HULL_H

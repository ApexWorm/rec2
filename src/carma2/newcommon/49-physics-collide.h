/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_49_PHYSICS_COLLIDE_H
#define GUARD_49_PHYSICS_COLLIDE_H

#include "c2_hooks.h"
#include "rec2_types.h"

// count

void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix);

// AddObjectWallForce

extern int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList2(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

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

extern int C2_HOOK_FASTCALL BoundsOverlapTest(const br_bounds3* pBounds_1, const br_bounds3* pBounds_2);

// GetObjectObjectEdgePoints

// GetSphereNonSphereForceInfo

// GetSphereSphereForceInfo

// GetSphereShapeForceInfo

// GetNonSphereNonSphereForceInfo

// GetNonSphereShapeForceInfo

// GetObjectObjectForceInfo

extern void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex);

extern int C2_HOOK_FASTCALL AddOldForces(tPhysics_joint_force* pForces, int pCount, const tPhysics_joint_force* pOld_forces, int pOld_count);

extern int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

extern int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, tPhysics_shape* pCollision_shape_1, tPhysics_shape* pCollision_shape_2);

// TestObjectOverlapIgnoreCloseRelatives

// TestObjectOverlapRecurse

extern int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks);

#endif // GUARD_49_PHYSICS_COLLIDE_H

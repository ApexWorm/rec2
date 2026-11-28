/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_49_PHYSICS_SUPPORT_H
#define GUARD_49_PHYSICS_SUPPORT_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize);

extern void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize);

extern void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void);

extern tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType);

extern tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType);

extern tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType);

extern tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType);

// GetJointSize

extern tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType);

extern tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType);

extern void C2_HOOK_FASTCALL FreeShape(void* pData);

extern void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape);

extern void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint);

extern void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds);

extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);

extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);

extern void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage);

extern void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage);

extern void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn);

extern void C2_HOOK_FASTCALL RepositionAndOrientateChildren(tPhysics_object* pObject, br_matrix34* pParent_matrix);

extern void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild);

extern void C2_HOOK_FASTCALL PhysicsAddObjectList(tPhysics_object* pParent, tPhysics_object* pChild);

extern void C2_HOOK_FASTCALL PhysicsRemoveObject(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext);

extern void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize);

#endif // GUARD_49_PHYSICS_SUPPORT_H

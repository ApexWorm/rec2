/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_60_PHIL_H
#define GUARD_60_PHIL_H

#include "c2_hooks.h"
#include "rec2_types.h"

#define NET_OBJECT_HIERARCHY_DEFAULT_SIZE 72

extern int gPHIL_disabled;
extern tPhysics_object* gPHIL_list_collision_infos;
extern tU32 gPHIL_last_physics_tick;
extern int gPHIL_count_list_collision_infos;
extern int gPHIL_object_added;
extern tPHIL_queued_objects gPhil_queued_objects;
extern int gPHIL_count_queued_objects_for_removal;
extern int gPHIL_count_queued_objects;

extern tU16 C2_HOOK_FASTCALL ScalarToFraction(br_scalar pValue, br_scalar pMin, br_scalar pMax);

extern br_scalar C2_HOOK_FASTCALL FractionToScalar(tU16 pFixed, br_scalar pMin, br_scalar pMax);

extern void C2_HOOK_FASTCALL GetOrientationFromMatrix(tFixed_quat* pOrientation, const br_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tFixed_quat* pOrientation);

extern tU16 C2_HOOK_FASTCALL OmegaCompScalarTo16(br_scalar pOmega);

extern br_scalar C2_HOOK_FASTCALL OmegaComp16ToScalar(tU16 pOmega16);

extern void C2_HOOK_FASTCALL GetOmega16FromOmega(tFixed_vec3* pOmega16, const br_vector3* pOmega);

extern void C2_HOOK_FASTCALL GetOmegaFromOmega16(br_vector3* pOmega, const tFixed_vec3* pOmega16);

extern int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, tU32 pSize);

extern void C2_HOOK_FASTCALL GetVelocityAndPosition(tPhysics_object* pObject, br_matrix34* pMatrix, br_matrix34* pParent_matrix);

extern int C2_HOOK_FASTCALL SizeOfObjectNetworkStuff(tPhysics_joint_type pType);

extern int C2_HOOK_FASTCALL WriteObjectNetworkStuff(const tU8* pBuffer, tPhysics_object* pObject, int pMode);

extern int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize);

// FindObject

extern int C2_HOOK_FASTCALL WriteHierarchyNetworkStuff(tU8* pBuffer, tPhysics_object* pObject, int pArg3);

extern int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data);

// GetSingleMatrixFromNetworkData

extern int C2_HOOK_FASTCALL PHILInit(void);

extern void C2_HOOK_FASTCALL PHILDisable(void);

extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);

#ifdef REC2_CHEAT
extern void C2_HOOK_FASTCALL PHILVerify(void);
#else
#define PHILVerify() do {} while(0)
#endif

extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject);

extern tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);

extern int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, const br_matrix34* pArg2, const br_vector3* pArg3, const br_vector3* pArg4);

extern int C2_HOOK_FASTCALL PHILAddObjectImmediately(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pOmega);

extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pCollision_info, int pParam, ...);

extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pObject, int pProperty);

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag2

// ProcessDrag

extern void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject);

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// PHILDoPhysics

extern void C2_HOOK_FASTCALL PHILApplyPHILObject(tPhysics_object* pObject, const br_matrix34* pMatrix);

#endif // GUARD_60_PHIL_H

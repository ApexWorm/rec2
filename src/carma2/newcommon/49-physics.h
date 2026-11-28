/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_49_PHYSICS_H
#define GUARD_49_PHYSICS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gINT_00679434;

extern int gINT_00679438;

extern int gINT_00692dc8;

extern int gPrepared_objects;

extern int gJoint_limit_10_hit;

extern int gCount_friction_forces;
extern tPhysics_joint_force gFriction_forces[50];
extern int gPHIL_collision_group;
extern int gINT_006940a4;
extern tPhysics_object** gReduced_object_list;
extern int gINT_006940ac;
extern undefined4 gDAT_006940b8;

extern int gINT_0065d014;

extern tPhysicsError_cbfn* gPhysics_error_cb;
extern br_scalar* gPhysics_buffer1_ptr;
extern br_scalar* gPhysics_buffer2_ptr;
extern int gImmortal_peds;
extern int gTotal_count_smash_peds;
extern int gBOOL_00744804;
extern float gPed_head_scale;
extern tPedestrian* gPedestrian_array;
extern void* gPhysics_buffer3_ptr;
extern int gPHIL_mechanics_time_sync;
extern int gPHIL_munging_objects;
extern int gPhysics_other_buffer_capacity;
extern tProximity_ray gProximity_rays[20];
extern tPed_giblet_size_spec* gGiblet_sizes;
extern br_model* gPed_spurt_model;
extern int gPed_spurty_lumps_count;
extern int gPed_times[3];
extern br_actor* gPed_limbed_actors[30];
extern tShrapnel_time_info gPed_spurt_min_max_time;
extern void* gPhysics_other_buffer;
extern void* gPhysics_scratch_ptr;
extern int gFace_count;
extern int gINT_00679438;
extern tPhysics_object* gObject_blocking_sensible_place;
extern tWorld_callbacks gWorld_callbacks;

// count

void C2_HOOK_FASTCALL DrMatrix34ApplyLPInverse(br_vector3* dest, const br_vector3* source, const br_matrix34* matrix);

extern int C2_HOOK_FASTCALL AddObjectWallForce(tPhysics_joint_force* pForces, const br_vector3* pPosition, br_scalar pT, const br_vector3* pNormal, br_scalar pUnused, int pCount, tPhysics_object* pObject, int pType, tFace_ref* pFace, const br_vector3* pReference_position);

extern int C2_HOOK_FASTCALL AddObjectObjectForce(tPhysics_joint_force* pForces, const br_vector3* pPoint_a, br_scalar pT, const br_vector3* pPoint_b, const br_vector3* pNormal, int pCount, tPhysics_object* pObject_a, tPhysics_object* pObject_b, int pType, void* pUnused, br_scalar pValue);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

extern tFace_ref* C2_HOOK_FASTCALL RayCastThruFaceList2(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tFace_ref* pFaces, int pCount);

extern tFace_ref* C2_HOOK_FASTCALL RayCastInBox(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pT, br_vector3* pCollision_position, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL FaceOffCount2(void);

extern void C2_HOOK_FASTCALL FaceOffCount(void);

extern void C2_HOOK_FASTCALL FaceOnCount(void);

extern int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, tPhysics_shape* pShape, br_matrix34* pMat);

// GetShapePoint

// GetObjectObjectEdgeIntersections

extern int C2_HOOK_FASTCALL GetFaceEdgeIntersection(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, int pMax_count, tPhysics_wall_workspace* pWorkspace, void* pUnused, int* pTouched);

extern int C2_HOOK_FASTCALL GetSphereToFacePoints(tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_joint_force* pForces, int pCount, int pMax_count, br_vector3* pSphere_start, br_vector3* pSphere_end, br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL CheckSingleEdgeWithSphere(br_vector3* pEdge_start, br_vector3* pEdge_end, tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_joint_force* pForces, int pCount, br_vector3* pSphere_start, br_vector3* pSphere_end, tFace_ref* pFace);

extern int C2_HOOK_FASTCALL GetSphereToEdgePoints(tPhysics_object* pObject, tCollision_shape_sphere_data* pSphere, tPhysics_joint_force* pForces, int pCount, int pMax_count, br_vector3* pSphere_start, br_vector3* pSphere_end, br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, int pMax_count, tPhysics_wall_workspace* pWorkspace, void* pUnused);

extern int C2_HOOK_FASTCALL CylinderCheck(const br_vector3* pN0, const br_vector3* pN1, const br_vector3* pEdge_start, const br_vector3* pEdge_direction, const br_vector3* pOther_start, const br_vector3* pOther_direction, const br_vector3* pOther_velocity, const br_vector3* pOther_direction_change, tPhysics_joint_force* pForce, int pCount, tPhysics_object* pObject, tPhysics_object* pOther);

extern int C2_HOOK_FASTCALL CheckTwoEdges(const br_vector3* pEdge_start, const br_vector3* pEdge_direction, const br_vector3* pOther_start, const br_vector3* pOther_direction, const br_vector3* pOther_velocity, const br_vector3* pOther_direction_change, tPhysics_joint_force* pForce, int pCount, tPhysics_object* pObject, tPhysics_object* pOther);

extern int C2_HOOK_FASTCALL WorldPointCollision(const br_vector3* pPoint, tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, tFace_ref* pFace);

extern int C2_HOOK_FASTCALL CheckSingleEdgeWithObjectEdges(br_vector3* pEdge_start, br_vector3* pEdge_end, tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, tFace_ref* pFace, tPhysics_wall_workspace* pWorkspace);

extern int C2_HOOK_FASTCALL GetEdgeToEdgePoints(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_joint_force* pForces, int pCount, int pMax_count, tPhysics_wall_workspace* pWorkspace, void* pUnused);

extern void C2_HOOK_FASTCALL SetUpWorkspace(tPhysics_object* pObject, tPhysics_shape* pShape, tPhysics_wall_workspace* pWorkspace);

extern int C2_HOOK_FASTCALL GetObjectWallForceInfo(tPhysics_object* pObject, tPhysics_joint_force* pForces, int pCount);

// GetObjectCornerToObjectFacePoints

extern int C2_HOOK_FASTCALL BoundsOverlapTest(const br_bounds3* pBounds_1, const br_bounds3* pBounds_2);

// GetObjectObjectEdgePoints

extern int C2_HOOK_FASTCALL GetSphereNonSphereForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL GetSphereSphereForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL GetSphereShapeForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL GetNonSphereNonSphereForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL GetNonSphereShapeForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_shape* pShape_a, tPhysics_shape* pShape_b, br_vector3* pPoints_a, br_vector3* pPoints_b, int pCount_a, int pCount_b, tPhysics_joint_force* pForces, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL GetObjectObjectForceInfo(tPhysics_object* pObject_a, tPhysics_object* pObject_b, tPhysics_joint_force* pForces, int pCount);

extern void C2_HOOK_FASTCALL GetShapePoint(br_vector3* pPoint, tPhysics_shape* pShape, tU32 pIndex);

extern int C2_HOOK_FASTCALL AddOldForces(tPhysics_joint_force* pForces, int pCount, const tPhysics_joint_force* pOld_forces, int pOld_count);

extern int C2_HOOK_FASTCALL TestObjectOverlap(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

extern int C2_HOOK_FASTCALL TestShapeEdgeIntersection(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, tPhysics_shape* pCollision_shape_1, tPhysics_shape* pCollision_shape_2);

extern int C2_HOOK_FASTCALL TestObjectOverlapIgnoreCloseRelatives(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

extern int C2_HOOK_FASTCALL TestObjectOverlapRecurse(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

extern int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks);

extern br_scalar C2_HOOK_FASTCALL HierarchyCalcEnergy(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL CheckCollisionList(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL MakeJointVelocitiesVector(br_scalar* pVelocities, tPhysics_joint_force** pForces, int pCount);

extern void C2_HOOK_FASTCALL ApplySingleForce(tPhysics_joint_force* pForce, br_scalar pAmount);

extern void C2_HOOK_FASTCALL ApplyJointForces(tPhysics_joint_force** pForces, br_scalar* pAmounts, int pCount);

extern void C2_HOOK_FASTCALL SkipForces(tPhysics_joint_force* pForce, int* pIndex);

extern void C2_HOOK_FASTCALL ScaleUpObjectMI(tPhysics_object* pObject, br_scalar pFactor);

extern void C2_HOOK_FASTCALL CheckForTwang(tPhysics_joint_force** pForces, br_scalar* pImpulses, int pCount, tWorld_callbacks* pCallbacks);

extern void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

extern void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection);

extern tForce_group* C2_HOOK_FASTCALL AllocateObjectWorkspace(int pCount);

extern void C2_HOOK_FASTCALL RedoQuickHingeVelocities(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL GetJointForces(tPhysics_object* pObject, tPhysics_joint_force* pForces);

extern void C2_HOOK_FASTCALL GetUniversalRotationAngle(br_matrix34* pMat_1, br_matrix34* pMat_2, br_vector3* pV1, br_vector3* pV2, br_vector3* pV3, br_vector3* pV4, br_vector3* pOut, br_vector3* pAxis);

extern tPhysics_object* C2_HOOK_FASTCALL GetObjectsParent(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL GetJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pForces, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL GetListJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pForces, int pCount, int pMax_count);

extern int C2_HOOK_FASTCALL GetGroupJointLimits(tPhysics_object* pObject, tPhysics_joint_force* pForces, int pCount, int pMax_count, tWorld_callbacks* pCallbacks);

extern int C2_HOOK_FASTCALL LimitOmega(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL ListLimitOmega(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL GroupLimitOmega(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL PositionWorldJointedObject(tPhysics_object* pObject, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL TranslateAndRotatePartialObject(tPhysics_object* pObject, tPhysics_object** pRoot, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL TranslateAndRotateJointedObject(tPhysics_object* pObject, br_scalar pDelta_time);

extern int C2_HOOK_FASTCALL CountJointForces(tPhysics_object* pObject, tPhysics_object* pParent);

extern void C2_HOOK_FASTCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time);

extern void C2_HOOK_FASTCALL TellPhysicsJointsHaveChanged(tPhysics_object* pCollision_info);

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

extern void C2_HOOK_FASTCALL PrepareObject(tPhysics_object* pObject, tPhysics_object** pList);

// PrepareForCollisions

extern int CheckPointVelocities(tPhysics_object* pObject, br_scalar pMax_velocity_squared);

extern int C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, int pFlag);

// SetGroupDoingNothingFlag

extern void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, int pFlag);

extern void CheckDoingNothingFlag(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetGroup(tPhysics_object* pObject, int pGroup);

// GroupTogether

extern int C2_HOOK_FASTCALL GetPreCalculatedJointForces(tPhysics_joint_force** pForces, tPhysics_object* pObject, tForce_group** pGroups, int* pCount_groups);

extern void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject);

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

extern void C2_HOOK_FASTCALL MakeObjectDoSomething(tPhysics_object* pObject, tPhysics_object* pList);

extern tPhysics_object** C2_HOOK_FASTCALL GetReducedList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_object** pTail);

extern tPhysics_object** C2_HOOK_FASTCALL ReduceReducedList(tPhysics_object* pObject, tPhysics_object** pList);

extern void C2_HOOK_FASTCALL SetObjectTrialPositionType(tPhysics_object* pObject, int pType);

extern void C2_HOOK_FASTCALL FillInTrialWorkspace(tPhysics_object* pObject, tPhysics_object* pOther, tWorld_callbacks* pCallbacks);

extern int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2);

extern int C2_HOOK_FASTCALL CollideObjectWithReducedObjectList(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_joint_force* pForces, int pCount, tPhysics_object* pDo_something_list, tWorld_callbacks* pCallbacks);

extern void C2_HOOK_FASTCALL AddObjectToReducedList(tPhysics_object* pObject, tPhysics_object** pList);

extern int C2_HOOK_FASTCALL ReducedFindObjectCollisions(tPhysics_object* pObject, tPhysics_object** pList, tPhysics_joint_force* pForces, int pCount, tWorld_callbacks* pCallbacks, tPhysics_object* pDo_something_list);

extern int C2_HOOK_FASTCALL FindObjectCollisions(tPhysics_object* pObject, tPhysics_object* pList, tPhysics_joint_force* pForces, int pCount, tWorld_callbacks* pCallbacks);

extern void C2_HOOK_FASTCALL DoRigidJoints(tPhysics_joint_force* pForces, int pCount);

extern int C2_HOOK_FASTCALL DoInterGroupCollisions(tPhysics_object** pObject_list, tPhysics_object* pObject, tPhysics_joint_force* pForces, tPhysics_joint_force* pOld_forces, int pCount_old_forces, tWorld_callbacks* pCallbacks);

extern int C2_HOOK_FASTCALL ReduceEffectOfForces(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount, const int* pIndices, int pIndex_count, br_scalar pThreshold);

extern void C2_HOOK_FASTCALL DoCollisions(tPhysics_object** pObject_list, tWorld_callbacks* pCallbacks);

extern void C2_HOOK_FASTCALL AddDoubleTorqueToMatrix(br_scalar* pMatrix, int pRows, int pIndex, tPhysics_joint_force** pForces, int pColumns);

extern void C2_HOOK_FASTCALL DRMatrix33Inverse(br_scalar* pResult, const br_scalar* pInput);

extern void C2_HOOK_FASTCALL SetUpQuickHingeData(tPhysics_object* pObject, tPhysics_object* pPartner, br_vector3* pAxis);

extern void C2_HOOK_FASTCALL InitQuickHinge(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL CalcQuickHingeData(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetUpForce(br_vector3* pAngular, br_vector3* pLinear, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject);

extern tPhysics_object* C2_HOOK_FASTCALL SetUpTorqueHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, const br_vector3* pTorque, tPhysics_object* pObject);

extern tPhysics_object* C2_HOOK_FASTCALL SetUpHingePartners(br_vector3* pAngular, br_vector3* pLinear, br_vector3* pPartner_angular, br_vector3* pPartner_linear, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL MakeMatrix(br_scalar* pMatrix, int pRows, tPhysics_joint_force** pForces, int pColumns);

extern void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL RestoreGroupOmegas(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL MakeVelocitiesVector2(br_scalar* pOutput, tPhysics_joint_force** pForces, int pCapacity, int pCount);

extern int C2_HOOK_FASTCALL MakeVelocitiesVector(br_scalar* pVelocities, tPhysics_joint_force** pForces, int pCapacity, int pCount);

extern void C2_HOOK_FASTCALL NewAddFrictionObjectObject(tPhysics_joint_force* pForce);

extern void C2_HOOK_FASTCALL NewAddFriction(tPhysics_joint_force* pForce, tWorld_callbacks* pCallbacks);

extern void C2_HOOK_FASTCALL ApplyForces(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount);

// DoJointFriction

extern void ListDoJointFriction(tPhysics_object* pObject);

extern void PostAddFriction(tWorld_callbacks* pCallbacks);

// RotateObjectSecondOrder

extern void C2_HOOK_FASTCALL RotateObjectFirstOrder(tPhysics_object* pObject, br_scalar pTime);

extern void C2_HOOK_FASTCALL RotateObject(tPhysics_object* pObject, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL GetFacesNearObject(tPhysics_object *pObject, tWorld_callbacks *pCallbacks);

extern void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects);

extern int C2_HOOK_FASTCALL RepositionObjectGroup(tPhysics_object** pObject_list, tPhysics_object* pObject, tWorld_callbacks* pCallbacks);

// SimpleRotate

// InterpolateSingleObject

// InterpolateObjects

extern void C2_HOOK_FASTCALL ApplyGravitySingleObject(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ApplyGravity(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetGravityDeltaV(tPhysics_object* pObject, br_vector3* pImpulse);

extern void C2_HOOK_FASTCALL TrialPositionChildren(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot);

extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);

extern void C2_HOOK_FASTCALL ApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque);

extern void C2_HOOK_FASTCALL ApplyDiminishingTorque(tPhysics_object* pObject, const br_vector3 *pTorque);

extern void OrderObjectListOnX(tPhysics_object** pObject_list);

extern void C2_HOOK_FASTCALL SwapPair(tPhysics_object* pObject_1, tPhysics_object* pObject_2);

extern void C2_HOOK_FASTCALL CheckObjectsPostionInList(tPhysics_object* pObject, tPhysics_object** pList);

extern int C2_HOOK_FASTCALL TestForWrongAnswer(const br_scalar* pOriginal, const br_scalar* pRhs, const br_scalar* pResult, int pSize);

extern void C2_HOOK_FASTCALL RQIterativeImprovement(br_scalar* pForces, const br_scalar* pMatrix, const br_scalar* pOrthogonal, const br_scalar* pRhs, const br_scalar* pOriginal, const tU8* pIndices, int pSize);

extern void C2_HOOK_FASTCALL RQExpand(br_scalar* pMatrix, const br_scalar* pOriginal, int pOld_size, int pSize, br_scalar* pOrthogonal, const tU8* pIndices);

extern void C2_HOOK_FASTCALL RQSingleExpand(br_scalar* pMatrix, const br_scalar* pOriginal, int pSize, br_scalar* pOrthogonal, const tU8* pIndices);

extern int C2_HOOK_FASTCALL RQDcmpV(br_scalar* pMatrix, int pDim, br_scalar* pQ);

// MakeIdentity

// SparseRQDcmpV

extern void C2_HOOK_FASTCALL MakeMatrixFromIndices(br_scalar* pMatrix, br_scalar* pOrthogonal, const br_scalar* pOriginal, br_scalar* pResult, const br_scalar* pRhs, const tU8* pIndices, int pSize, tForce_group** pGroups, int pCount_groups);

extern int C2_HOOK_FASTCALL HasBeenDoneBefore(tU32 pHistory[][2], int pCount, int pToggle);

extern int C2_HOOK_FASTCALL RemoveRQSingularities(int pLast, const br_scalar* pMatrix, const br_scalar* pOrthogonal, const br_scalar* pRhs, const br_scalar* pForward, int pFirst, tU32 pHistory[][2], int pCount, const tU8* pIndices);

extern void C2_HOOK_FASTCALL AddToMatrix(tU8 pIndex, br_scalar* pMatrix, const br_scalar* pOriginal, br_scalar* pResult, const br_scalar* pRhs, br_scalar* pOrthogonal, tU8* pIndices, int* pSize);

extern int C2_HOOK_FASTCALL RQRemovedOneTooManyForces(int* pCount_excluded, tU8* pExcluded, int pSize, const br_scalar* pResult, const br_scalar* pRhs, const br_scalar* pOriginal, tU32 pHistory[][2], int pCount, int pFirst);

extern void C2_HOOK_FASTCALL GetInverseM(const br_scalar* pMatrix, const br_scalar* pOrthogonal, br_scalar* pResult, int pSize, int pFirst);

extern int C2_HOOK_FASTCALL RQMatrixSolveNoNegatives(const br_scalar* pOriginal, br_scalar* pRhs, br_scalar* pResult, int pSize, int pRow_size, tU8* pExcluded, int* pCount_excluded, int pFirst, int pUnused, tForce_group** pGroups, int pCount_groups);

extern void C2_HOOK_FASTCALL RQMatrixSolveWithNegatives(br_scalar* pR, br_scalar* pQ, br_scalar* pB, br_scalar* pX, int pCount);

extern int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim);

extern void C2_HOOK_FASTCALL RqBackSub(const br_scalar* pR, const br_scalar* pQ, const br_scalar* pRhs, br_scalar* pX, int pDim);

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

extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);

extern float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace);

extern void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage);

// ResetMan2

extern void C2_HOOK_FASTCALL SetUpTestObjects(void);

// ResetObjectList

// ResetMan

extern void C2_HOOK_FASTCALL InitPhysics(void);

// PointOutOfSightNotAR (moved to 51-smash for inlining purposes)

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

#endif // GUARD_49_PHYSICS_H

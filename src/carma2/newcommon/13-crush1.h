/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_13_CRUSH1_H
#define GUARD_13_CRUSH1_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <stdio.h>

extern undefined4 gCrush_array_0067a190[10];
extern int gCount_net_crush_reattach_bit_list;
extern tCrush_net_reattach_bit_list_item gNet_crush_reattach_bit_list[8];
extern undefined4 gDAT_00679440;
extern tCrush_info_buffer gDetached_bit_crush_info_buffer;
extern tPhysics_object* gDetached_bit_collision_infos;
extern tDriver gDetached_bit_driver;
extern tPhysics_joint* gTrack_crush_joints[32];
extern tCrush_info_buffer gSplit_car_crush_info_buffer;
extern tPhysics_object* gSplit_car_collision_infos;
extern tDriver gSplit_car_driver;
extern tPhysics_shape* gGonad_sphere_collision_shape;
extern int gCount_toggled_doors;
extern tCrush_list_item gCrush_lists[8];
extern int gCount_queued_drone_crushes;
extern int gCount_crush_detach_list;
extern int gINT_0067bdfc;
extern tU8 gCrush_data_entry_counter;
extern int gCount_net_crush_detach_list;
extern int gCount_net_crush_semi_detach_bit_list;
extern tU16 gBend_counter;
extern int gCount_net_crush_full_detach_bit_list;
extern int gCount_car_damage_crush_list;
extern tToggled_door gToggled_doors[16];
extern tCrush_detach_list_item gCrush_detach_list[16];
extern tU32 gUINT_0067be84;

extern void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF);

extern void C2_HOOK_FASTCALL ClearCrushLists(void);

extern void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL InitCrushSystems(void);

extern void C2_HOOK_FASTCALL ResetCrushSystems(void);

extern void C2_HOOK_FASTCALL ReadCrushData(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage);

extern intptr_t C2_HOOK_CDECL CreateDefaultCrushDataCB(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL CreateDefaultCrushData(br_actor* pActor, tCar_spec* pCar_spec);

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

extern void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor* pActor, tCar_crush_buffer* pBuffer);

extern intptr_t C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* pData);

extern intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* pActor, void* pData);

// InitModelMasterCrushData

extern void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush);

extern int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2);

extern int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2);

extern tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);

// SoftnessOfNearestPointCB

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* pCallback, void* pContext);

// SoftnessOfNearestPoint

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

// InitPhysMasterCrushData

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex

// SetUpSemiDetachJointStuff

// InitPhysCrushDataCB

// InitPhysCrushData

// CalculateReferencePoints

extern intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* pActor, void* pData);

// InitModelCrushData

// PointEdgeDistSq

extern void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data);

extern void C2_HOOK_FASTCALL InitModelVertexData(tModel_detail_vertex_data* pVertex_data, br_model* pModel, int pDetail_level, tCar_crush_buffer_entry* pCrush_data, tCar_crush_spec* pCar_crush_spec);

extern intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* pData);

// InitVertexData

// CheckWheelPositions

// InitPhysModCrushData

extern void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data);

extern void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data);

// BuckleTheBugger

// OutsideEdgeLine

// PointTriangleDistSq

// PointRectDistSq

// PointSimplexDistSq

extern float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices);

extern void C2_HOOK_FASTCALL LimitImpactScale(float* pScale, int pAxis, float pPos, float pDirection, tU16 pLimit_number, const tCar_crush_limits* pLimits, int pMax_index);

extern float C2_HOOK_FASTCALL GetImpactScale(const br_vector3* pP, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

extern int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector);

// CrushModel

// PossibleShockSmash

extern intptr_t C2_HOOK_FASTCALL CrushModels(br_actor* pActor, br_matrix34* pMat, void* pContext);

extern void C2_HOOK_FASTCALL CrushPoint(br_vector3* pPoint, int pCount, const br_vector3* pVertices, const br_vector3* pDirection, br_scalar pForce, tU16 pLimit_number, const tCar_crush_limits* pLimits, int pMax_index);

extern void C2_HOOK_FASTCALL ClipPointToLimits(br_vector3* pPoint, tU16 pLimit_bits, const tCar_crush_limits* pLimits);

extern void C2_HOOK_FASTCALL CrushCollisionShapes(tCar_spec* pCar, tSpam_userdata* pContext);

extern intptr_t C2_HOOK_FASTCALL SimpleCrushCarModelsCB(br_actor* pActor, br_matrix34* pMat, void* pContext);

extern void C2_HOOK_FASTCALL SimpleCrushCar(tCar_spec* pCar, tSpam_userdata* pContext);

extern void C2_HOOK_FASTCALL CrushCar(tCar_spec* pCar, br_actor* pActor, tCar_crush* pCrush);

// SendCrush

extern void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc);

extern void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc);

// RemoveDuplicatePoints

extern void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, const br_vector3* pPoints, const br_vector3* pForce, const br_bounds3* pBounds);

extern void C2_HOOK_FASTCALL CrushDrone(tDrone_crush_data* pCrush_data, int pCount, const br_vector3* pPoints, const br_vector3* pForce);

extern void C2_HOOK_FASTCALL CrushDroneObject(tPhysics_object* pObject, int pCount, const br_vector3* pPoints, const br_vector3* pForce);

// DoDroneCrushing

extern void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints);

// QueueDroneCrush

// DoCrushing

extern void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue);

// QueueCarCrush

extern void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);

extern void C2_HOOK_FASTCALL SetFlappage(br_actor* pActor, tCar_spec* pCar, int pFlags, float pScalar);

// GetUniqueID

// InitializeCollisionInfo

// RemoveBit

// RemoveNonCarChildren

// RemoveNonCar

// GetNonCar

// GetNonCarObject

// SetRelationsDoingNothingFlags

// SetUpDetachedBitObject

// SetUpSemiDetachedBitJoint

extern int C2_HOOK_FASTCALL BitIsInBentPartOfCar(br_actor* pActor, br_scalar pBend_z, br_scalar pPivot_z);

// SetUpFlapJoint

extern void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel);

// CheckFlapAngles

extern void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel);

extern void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar_spec, br_actor* pActor_arg2, br_actor* pActor_arg3, tPhysics_object* pObject);

// CentreModelOnPoint

extern tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pP1, br_bounds3* pBounds, br_vector3* pP2, br_vector3* pP3);

extern int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP3, br_bounds3* pBounds, br_actor* pActor);

extern void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL SendDetachBit(tCar_spec* pCar, br_actor* pActor);

extern void C2_HOOK_FASTCALL SendAllSemiDetachedBits(tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL AddNonCar(tNon_car_spec* pNon_car, const tDriver* pDriver);

extern tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);

extern void C2_HOOK_FASTCALL DoDetaching(void);

// FlapBit

// FlapBits

// StartFlapping

extern void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush);

extern void C2_HOOK_FASTCALL SendReAttachBit(tCar_spec* pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec);

// DoFlapping

extern intptr_t C2_HOOK_CDECL ToggleFlapsCB2(br_actor* pActor, void* pCar);

extern int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* pCar);

extern void C2_HOOK_FASTCALL SendToggleFlaps(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ToggleFlaps(void);

extern int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL DoFullyDetaching(void);

// CalculateCollisionObjectStuffForSplitHalves

// SetUpCollisionInfoStuffForSplitHalf

// SetFace

// AddSplitVertexIndex

// CreateSplitVertex

// GetSplitVertex

// SplitModel

// CreateSplitPoints

// SplitCollisionShape

// SplitCarCollisionShapes

// QuantizeWithinBounds

// SaveVitalBitsOfObject

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg);

extern void C2_HOOK_FASTCALL SwapCarShapes(tCar_spec* pCar, tPhysics_shape* pShape);

extern int C2_HOOK_FASTCALL IsAWheelOrPivotActor(br_actor* pActor, tCar_spec* pCar);

extern intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);

// IsOnNonCarHalf

extern void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, const tSplit_info* pSplit_info, int pArg3);

// QuantizePlane

// WithinBounds

extern int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pArg2, const br_vector3* pPlane, float pArg4, tSplit_info* pSplit_info);

// FindForceEdge

// DoXZSplitting

// DoYSplitting

// DoSplitting

// DoJointSnapping

extern int C2_HOOK_FASTCALL PointIsOutsideShape(const br_vector3* pPoint, const tCollision_shape_polyhedron_data* pPolyhedron);

extern int C2_HOOK_FASTCALL PointIsOutsideLimits(const tCar_crush_shape_info* pShape, int pPoint_index, const tCar_crush_limits* pLimits);

extern void C2_HOOK_FASTCALL ClipPointToPolyhedron(br_vector3* pPoint, const tCollision_shape_polyhedron_data* pPolyhedron);

extern void C2_HOOK_FASTCALL QuantizeVector3(br_vector3* pV);

extern int C2_HOOK_FASTCALL MoveCarToSensiblePlace(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec* pCar_spec);

// DoShapeSwapping

extern void C2_HOOK_FASTCALL BendPoint(br_vector3* pPoint, const tBend_point_context* pContext);

extern int C2_HOOK_FASTCALL EdgeCrossesZPlane(br_scalar pZ, const br_vector3* pP1, const br_vector3* pP2);

// BendCollisionShape

extern void C2_HOOK_FASTCALL BendCollisionShapes(tCar_spec* pCar, const tBend_point_context* pContext);

// BendModel

extern intptr_t C2_HOOK_FASTCALL BendModels(br_actor* pActor, br_vector3* pTranslation, void* pContext);

extern br_scalar C2_HOOK_FASTCALL BrAngleToSignedDegree(br_angle pAngle);

// BendWheels

extern void C2_HOOK_FASTCALL BendActor(tCar_spec* pCar, br_actor* pActor, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pPivot, br_scalar pZ, tBend_point_spec* pSpec);

extern void C2_HOOK_FASTCALL BendOtherThings(tCar_spec* pCar, tBend_point_spec* pSpec);

extern void C2_HOOK_FASTCALL ReAttachOrFullyDetachDifficultBits(tCar_spec* pCar, br_scalar pBend_z, br_scalar pPivot_z);

extern void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pPivot, br_scalar pZ, int pArg7);

extern br_angle C2_HOOK_FASTCALL QuantizeAngle(br_angle pAngle, br_angle pMin, br_angle pMax, br_angle pStep);

extern int C2_HOOK_FASTCALL BendCarOneForce(tCar_spec* pCar, tCar_crush* pCar_crush);

extern void C2_HOOK_FASTCALL DoBending(void);

extern void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

extern void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel);

extern void C2_HOOK_FASTCALL RandomlyXZSplitCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL RandomlyBendCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL TotallySpamTheCar(tCar_spec* pCar, br_scalar pDamage);

extern intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseUltraPlus(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg);

extern intptr_t C2_HOOK_CDECL FireOffBitsCB(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL FireSomeBitsOffCar(tCar_spec* pCar);

// DoSpams

// CrushBendFlapRend

// SphericizeModel

// SphericizePhysics

extern void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar);

extern intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data);

extern void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape);

// CompletelyUnBendCollisionShapes

extern void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec);

// CompletelyUnBendOtherThings

extern intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData);

// DoCompletelyUnBentThings

extern void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL TotallyRepairModel(br_model* pModel);

extern intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser);

extern int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data);

extern void C2_HOOK_FASTCALL SendReAttachAllBits(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL TotallyRepairCar(void);

// AddTotalDeflectionOfModel

// AddTotalDeflection

// RepairModel

// RepairModels

// UnBendModel

// UnBendModels

// UnBendActor

extern void C2_HOOK_FASTCALL UnBendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context* pContext);

// UnBendCollisionShapes

// UnBendWheels

// UnBendOtherThings

extern br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle);

// CalculateAmountRepairedFraction

extern br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount);

extern void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec);

// ReAttachBits

// RepairObject

extern void C2_HOOK_FASTCALL RepairCarCollisionShapes(tCar_spec* pCar, float pStep);

// RepairCar2

// RepairCar

// RestoreVitalBitsOfObject

// WeldCar

// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

extern void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, tCompressed_vector3* pSrc);

extern void C2_HOOK_FASTCALL ARApplyFlap(br_actor* pActor, br_angle pAngle);

extern void C2_HOOK_FASTCALL ARApplySplitWeld(tCar_spec* pCar, const br_vector4* pPlane);

extern void C2_HOOK_FASTCALL ARUndoSplitWeld(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL ReceivedCrush(tNet_message_chunk* pContents);

extern intptr_t C2_HOOK_CDECL FindBitWithID(br_actor* pActor, void* pContext);

extern br_actor* C2_HOOK_FASTCALL AttachedBitFromID(tCar_spec* pCar, tU8 pID);

extern void C2_HOOK_FASTCALL ReceivedSemiDetachBit(tNet_message_chunk* pContents);

// ReceivedDetachBit

extern tPhysics_object* C2_HOOK_FASTCALL SemiDetachedBitObjectFromID(tCar_spec* pCar, tU8 pID);

extern void C2_HOOK_FASTCALL ReceivedFullyDetachBit(tNet_message_chunk* pContents);

extern br_actor* C2_HOOK_FASTCALL DetachedBitFromID(tCar_spec* pCar, tU8 pID);

extern br_actor* C2_HOOK_FASTCALL SemiDetachedBitFromID(tCar_spec* pCar, tU8 pID);

extern void C2_HOOK_FASTCALL ReceivedReAttachBit(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedReAttachAllBits(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL DoQueuedNetDetaching(void);

// DBNonCarOwner

// SendDBNonCar

// ReceivedDBNonCar

// SendAllDBNonCars

// ReceivedDBNonCarPosition

extern void C2_HOOK_FASTCALL ReceivedToggleFlaps(tNet_message_chunk* pContents);

// SHNonCarOwner

// SendSHNonCar

// SendAllSHNonCars

// ReceivedSHNonCar

extern int C2_HOOK_FASTCALL GetNetShapeDataLengthFromShape(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL GetNetShapeDataLengthFromData(tU8* pData);

extern void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar);

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

extern void C2_HOOK_FASTCALL CompressPlane(tCompressed_vector4* pDest, const br_vector4* pSrc);

extern void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tCompressed_vector4* pSrc);

// CopySplitInMessage

extern void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes);

#endif // GUARD_13_CRUSH1_H

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_00_CAR_H
#define GUARD_00_CAR_H

#include "rec2_types.h"

extern int gTesting_car_for_sensible_place;
extern int gCunning_stunt_bonus[3];
extern float gArmour_starting_value[100];
extern int gExplosion_sound_id;
extern float gPower_starting_value[100];
extern float gOffensive_starting_value[100];
extern tFloat_bunch_info gCar_softness;
extern tFloat_bunch_info gCar_car_damage_multiplier;
extern br_vector3 gZero_v__car;
extern int gFreeze_mechanics;
extern int gNum_active_non_cars;
extern tNon_car_spec* gActive_non_car_list[99];
extern int gEliminate_faces;
extern int gIn_the_sea;
extern int gFace_num__car;
extern tFace_ref gFace_list__car[300];
extern br_scalar gMin_world_y;

extern void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageEngine(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageTrans(int pAmount);

extern void C2_HOOK_FASTCALL DamageSteering(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageLFWheel(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageLFBrake(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageLRBrake(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageLRWheel(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageRFWheel(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageRFBrake(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageRRBrake(int pDamage_amount);

extern void C2_HOOK_FASTCALL DamageRRWheel(int pDamage_amount);

extern intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex);

extern void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag);

extern void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race);

extern void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, tU32 pGrid_index);

extern void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race);

// InitialiseNonCar

extern void C2_HOOK_FASTCALL NewFaceListCallBack(tPhysics_object* pObject, tFace_ref* pFace);

extern int C2_HOOK_FASTCALL IsCarInTheSea(void);

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

extern void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar);

// FinishCars

// GetNonCars

extern void C2_HOOK_FASTCALL GetNetPos(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL MungeCarsMass(tCar_spec* pCar, br_scalar pFactor);

extern void C2_HOOK_FASTCALL AddDrag(const tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDelta_time);

extern void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL DoCarStuff(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DoNonCarStuff(tNon_car_spec* pNon_car);

extern void C2_HOOK_FASTCALL RemoveFlyingCar(tCar_spec* pCar);

// RestoreFlyingCar

extern void C2_HOOK_FASTCALL GetDrivableOnList(tPhysics_object *pObjects);

extern void C2_HOOK_FASTCALL APTCPreCollision(void);

extern void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tPhysics_object* pObject);

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

extern void C2_HOOK_FASTCALL MoveAndCollideCar(tCar_spec* pCar, br_scalar pDt);

extern void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar2(tCar_spec* pCar, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar3(tCar_spec* pCar, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar4(tCar_spec* pCar, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar5(tCar_spec* pCar, br_scalar pDt);

extern void C2_HOOK_FASTCALL ControlCar1(tCar_spec* pCar, br_scalar pDt);

extern int C2_HOOK_FASTCALL SteeringSelfCentre(tCar_spec* c, br_scalar dt, br_vector3* n);

// NonCarSnapOff

// TestNonCarSnapOff

extern void C2_HOOK_FASTCALL TumbleObjectWithV(tPhysics_object* pObject, br_vector3* pV, float pArg3, float pArg4);

extern void C2_HOOK_FASTCALL TumbleObject(tPhysics_object* pObject, float pTumble_factor, float pTumble_threshold);

extern void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car);

extern void C2_HOOK_FASTCALL NonCarCalcForce(tNon_car_spec* pNon_car, br_scalar pDt);

extern void C2_HOOK_FASTCALL DoBumpiness(tCar_spec* c, br_vector3* wheel_pos, br_vector3* norm, br_scalar* d, int n);

extern void C2_HOOK_FASTCALL SmashFacesWithWheels(tCar_spec* c, tFace_ref** pFace_refs, br_scalar* pForces, br_vector3* pWheel_pos);

extern void C2_HOOK_FASTCALL ConditionallyNoteSkid(tCar_spec* pCar, tFace_ref** pFace_refs, int pWheel);

extern void C2_HOOK_FASTCALL NudgeObject(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL CalcForce(tCar_spec* c, br_scalar dt);

// DoRevs

extern void C2_HOOK_FASTCALL ScrapeNoise(br_scalar pVelocity, br_vector3* pPosition, int pMaterial);

extern void C2_HOOK_FASTCALL SkidNoise(tCar_spec* pC, int pWheel_num, br_scalar pV, int pMaterial);

extern void C2_HOOK_FASTCALL StopSkid(tCar_spec* pC);

// CrashNoise

extern void C2_HOOK_FASTCALL CrushAndDamageCar(tCar_spec* pCar, br_vector3* pPosition, br_vector3* pForce_car_space, tCar_spec* pCar2, int pCount_points, br_vector3* pPoints, br_actor* pActor);

extern int C2_HOOK_FASTCALL PointInFaceByQuiteABitActually(br_scalar pD, br_vector3* pPoint, br_vector3* pNormal, tFace_ref* pFace, br_vector3* pV0, br_vector3* pV1, br_vector3* pV2);

extern int C2_HOOK_FASTCALL DoEnvironmentSmashes(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount);

extern int C2_HOOK_FASTCALL ProcessForcesCallback(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount);

extern int C2_HOOK_FASTCALL ProcessJointForcesCallBack(void* pArg1, void* pArg2, void* pArg3);

extern void C2_HOOK_FASTCALL MultiFindFloorInBoxM(int pNum_rays, br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d, tCar_spec* c, tFace_ref** pFace_refs, tPhysics_object** pObjects);

extern void C2_HOOK_FASTCALL MultiRayCastOnObjects(int pNum_rays, br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d, tCar_spec* c, tFace_ref** pFace_refs, tPhysics_object** pObjects);

extern void C2_HOOK_FASTCALL MultiFindFloorInBoxBU(int pNum_rays, br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d, tCar_spec* c, tFace_ref** pFace_refs);

extern void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material);

extern void C2_HOOK_FASTCALL findfloor(br_vector3* a, br_vector3* b, br_vector3* nor, br_scalar* d);

extern int C2_HOOK_FASTCALL FindFloorInBoxBU(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pDistance, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL CancelPendingCunningStunt(void);

// SetAmbientPratCam

extern void C2_HOOK_FASTCALL SetTextureBits(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void);

extern void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period);

extern void C2_HOOK_FASTCALL DoLODCarModels(void);

extern intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL DoComplexCarModels(void);

extern void C2_HOOK_FASTCALL ResetCarScreens(void);

extern void C2_HOOK_FASTCALL ToggleCarToCarCollisions(void);

extern void C2_HOOK_FASTCALL SwapCar(void);

extern void C2_HOOK_FASTCALL AdjustDownForce(void);

extern void C2_HOOK_FASTCALL FreezeMechanics(void);

// FlyCar

extern float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(const tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);

extern int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3);

extern tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* pActor);

extern tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* pActor);

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

extern void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs);

extern void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex);

#endif // GUARD_00_CAR_H

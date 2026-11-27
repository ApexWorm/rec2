/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_57_DRONE_H
#define GUARD_57_DRONE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern br_vector3 gRender_bounds_centre;
extern int gDrones_unmodified;
extern int gINT_006820d4;
extern int gINT_006844fc;
extern int gINT_00681fb0;
extern int gFrame;
extern tDrone_state_function* gDrone_state_functions[6];
extern int gCurrent_selected_drone;
extern int gCount_drones;
extern tDrone_form gDrone_forms[64];
extern int gCount_drone_forms;
extern int gCount_drone_path_nodes;
extern tDrone_spec* gDrone_specs;
extern tDrone_path_node* gDrone_path_nodes;
extern int gCount_drones;
extern int gCurrent_selected_drone;

extern int gCount_drones;

extern int gCurrent_selected_drone;

extern void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...);

extern void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pName, br_vector3* pVector);

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

extern FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name);

// LoadDroneTypeInfo

extern void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL AddDroneToPHIL(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL PreprocessDronePaths(void);

extern int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData);

// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

extern br_actor* C2_HOOK_CDECL MrFindy(br_actor* pActor, const char* pName);

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

// CalculateDrivingInfo

extern void C2_HOOK_FASTCALL PauseDroneState(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL UnPauseDroneState(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone, int pNew_state);

// ResetDroneCrushyModel

// ResetDroneModel

extern int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos);

// SemiInitDroneSpec

// InitDroneSpec

// ChooseNextNode

extern int C2_HOOK_FASTCALL DroneOrientationChanged(tDrone_spec* pDrone, float pDot);

extern int C2_HOOK_FASTCALL DroneyObjectInSensiblePlace(tPhysics_object* pObject, tPhysics_object* pList);

extern void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce);

extern void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL StartProcessingThisDrone(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos);

extern void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void);

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

extern void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex);

extern void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState pState);

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

extern void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);

// PossiblyPipeDroneMovement

extern void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState pState);

// DoDroneHaltingStuff

extern void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState pState);

extern void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState pState);

// GetNewCrushyModelForDrone

extern void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on);

extern int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void);

extern void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

extern void C2_HOOK_FASTCALL InitDrones(void);

extern void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void);

extern void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void);

// GetDroneCrushData

// AllocateAndInitDrones

extern void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF);

extern void C2_HOOK_FASTCALL DronePreCollisionStuff(void);

// DoDronePerGameFrameStuff

extern void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone);

// MyDroneHathCollideth

// MyDroneHathHalteth

// MoveMyDroneBaby

extern void C2_HOOK_FASTCALL DroneLookWhereYoureGoingYouBlindCunt(void);

extern void C2_HOOK_FASTCALL SelectNextDrone(void);

extern void C2_HOOK_FASTCALL SelectPreviousDrone(void);

extern int C2_HOOK_FASTCALL OKToViewDrones(void);

extern br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void);

// GetCurrentViewDroneDirection

// ApplyDroneRender2

extern void C2_HOOK_FASTCALL ApplyDroneStraightPos2(int pDrone_id, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance);

extern void C2_HOOK_FASTCALL ApplyDroneCornerPos2(int pDrone_id, tU16 pSpeed, tU16 pField_0x74, tU16 pRadius, const tCompressed_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL ReallocModelFacesAndVertices(br_model* pModel, int pCount_faces, int pCount_vertices);

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

#endif // GUARD_57_DRONE_H

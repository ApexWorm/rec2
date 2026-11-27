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

extern const char* gGroove_funk_type_names[3];
extern const char* gAxis_names[3];
extern const char* gFunk_speed_control_names[2];
extern const char* gReverseness_type_names[2];
extern br_vector3 gRender_bounds_centre;
extern int gINT_00681fb0;
extern int gFrame;
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

// CrappyLittleVector3DPrintf

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

extern FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name);

// LoadDroneTypeInfo

extern void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone);

// InitDroneCollisionObject

extern int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone);

// AddDroneToPHIL

extern void C2_HOOK_FASTCALL PreprocessDronePaths(void);

extern int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL LinkDroneActorsToModelsAndSetRenderStyle(tDrone_spec* pDrone_spec, int pModel_start, int pModel_end);

extern void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone_spec);

extern int C2_HOOK_FASTCALL ReadPastBoundsShapesHeader(FILE* pF);

extern int C2_HOOK_FASTCALL ReadPastThisLine(FILE* pF, const char* pLine);

extern int C2_HOOK_FASTCALL MatchFGType(const char* pName);

extern int C2_HOOK_FASTCALL GetAxisFromString(const char* pName);

extern int C2_HOOK_FASTCALL GetSpeedControlFromString(const char* pName);

extern br_actor* C2_HOOK_CDECL MrFindy(br_actor* pActor, const char* pName);

extern br_actor* C2_HOOK_FASTCALL FindDroneChildActor(tDrone_spec* pDrone_spec, const char* pName);

extern int C2_HOOK_FASTCALL GetReversenessFromString(const char* pName);

extern void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone_spec, FILE* pF);

extern void C2_HOOK_FASTCALL LoadDrone(int pIndex);

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

extern void C2_HOOK_FASTCALL ResetDroneCrushyModel(br_model* pSource, br_model* pDest);

// ResetDroneModel

extern int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos);

// SemiInitDroneSpec

// InitDroneSpec

// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

// StopRenderingThisDrone

// StopProcessingThisDrone

// StartRenderingThisDrone

// CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing

// StartProcessingThisDrone

extern int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos);

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// DroneStateFuncReset

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

extern void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);

// PossiblyPipeDroneMovement

// DroneStateFuncControlledMovement

// DoDroneHaltingStuff

// DroneStateFuncPhysicsActive

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

extern void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on);

extern int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void);

extern void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

// InitDrones

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

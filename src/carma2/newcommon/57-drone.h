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
extern br_model* gStatic_drone_path_model;
extern br_model* gElasticatey_drone_path_model;
extern br_actor* gStatic_drone_path_model_actor;
extern br_actor* gDrone_path_model_actor;
extern int gDrone_paths_elasticating;
extern int gStatic_drone_path_model_max_vertices;
extern int gStatic_drone_path_model_max_faces;
extern int gSelected_edit_drone_path;
extern int gSelected_drone_path_node_index;
extern int gSelected_drone_form_index;
extern char gDrone_edit_message[256];
extern int gSelected_drone_path_index;
extern tU32 gNext_drone_edit_path_munge;
extern int gShow_drone_paths;
extern tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4];
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

extern void C2_HOOK_FASTCALL PauseDroneState(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL UnPauseDroneState(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone, int pNew_state);

extern void C2_HOOK_FASTCALL ResetDroneCrushyModel(br_model* pSource, br_model* pDest);

extern void C2_HOOK_FASTCALL ResetDroneModel(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos);

extern void C2_HOOK_FASTCALL SemiInitDroneSpec(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode);

// ChooseNextNode

// DroneOrientationChanged

extern int C2_HOOK_FASTCALL DroneyObjectInSensiblePlace(tPhysics_object* pObject, tPhysics_object* pList);

extern void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce);

extern void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone);

extern void C2_HOOK_FASTCALL StartProcessingThisDrone(tDrone_spec* pDrone);

extern int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos);

extern int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos);

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

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

extern void C2_HOOK_FASTCALL CalcNFacesAndVerticesInEditModel(int* pCount_faces, int* pCount_vertices);

extern int C2_HOOK_FASTCALL InitialiseEditModelsEtc(void);

// UpdateEditModels

extern void C2_HOOK_FASTCALL WriteOutDronePaths(void);

extern void C2_HOOK_FASTCALL SelectClosestDrone(void);

// FUN_00453460

extern void C2_HOOK_FASTCALL ToggleDronePaths(void);

extern void C2_HOOK_FASTCALL FUN_00453720(void);

extern int C2_HOOK_FASTCALL FindNearestNodeDrone(const br_vector3* pPosition, br_scalar pMax_distance);

extern int C2_HOOK_FASTCALL FUN_004538a0(int pReport);

extern void C2_HOOK_FASTCALL FUN_00453940(void);

extern int C2_HOOK_FASTCALL FUN_00453ab0(void);

extern void C2_HOOK_FASTCALL FUN_00453b80(void);

extern void C2_HOOK_FASTCALL FUN_00453d60(void);

extern void C2_HOOK_FASTCALL FUN_00453d80(void);

extern void C2_HOOK_FASTCALL FUN_00453ed0(void);

extern void C2_HOOK_FASTCALL FUN_00453ee0(void);

extern void C2_HOOK_FASTCALL FUN_00453ef0(void);

extern void C2_HOOK_FASTCALL FUN_00453f00(void);

extern void C2_HOOK_FASTCALL FUN_00453f10(void);

extern void C2_HOOK_FASTCALL FUN_00453f20(void);

extern void C2_HOOK_FASTCALL FUN_00453f30(void);

extern void C2_HOOK_FASTCALL FUN_00453f40(void);

extern int C2_HOOK_FASTCALL FUN_00454360(int pNode);

extern void C2_HOOK_FASTCALL FUN_004543d0(void);

extern void C2_HOOK_FASTCALL FUN_00454470(int pNode);

extern void C2_HOOK_FASTCALL FUN_00454640(void);

extern void C2_HOOK_FASTCALL FUN_00454650(void);

extern void C2_HOOK_FASTCALL FUN_00454660(void);

extern void C2_HOOK_FASTCALL FUN_00454670(void);

extern void C2_HOOK_FASTCALL FUN_00454680(void);

extern void C2_HOOK_FASTCALL FUN_00454690(void);

extern void C2_HOOK_FASTCALL FUN_00454710(void);

extern void C2_HOOK_FASTCALL DeleteNodeSectionsAndOrphan(void);

extern void C2_HOOK_FASTCALL DeleteNodeAndSections(int pNodenum);

extern void C2_HOOK_FASTCALL FUN_00454bb0(void);

// SelectNextDroneForm

// SelectPreviousDroneForm

// CreateNewDrone

#endif // GUARD_57_DRONE_H

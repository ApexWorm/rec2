/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_59_CAMERA_H
#define GUARD_59_CAMERA_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gNet_player_to_view_index;
extern int gOpponent_viewing_mode;
extern int gAction_replay_manual_camera_target_type;
extern int gCamera_frozen;
extern tSave_camera gSave_camera[2];
extern br_actor* gPed_actor;

extern float gCamera_cockpit_hither;
extern tActionReplayCameraMode gAction_replay_camera_mode;

// DoCameraControls

extern void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime);

extern void C2_HOOK_FASTCALL CheckCameraHither(void);

extern tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void);

extern void C2_HOOK_FASTCALL AmIGettingBoredWatchingCameraSpin(void);

extern void C2_HOOK_FASTCALL ViewNetPlayer(void);

extern void C2_HOOK_FASTCALL ViewOpponent(void);

extern void C2_HOOK_FASTCALL ReallyViewOpponent(void);

extern void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL SetPanningFieldOfView(void);

extern int C2_HOOK_FASTCALL IsCameraTypeAllowed(int pCamera_mode);

extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);

extern void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);

extern void C2_HOOK_FASTCALL ChangeCameraType(void);

extern void C2_HOOK_FASTCALL ToggleCockpit(void);

extern void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime);

extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime);

extern int C2_HOOK_FASTCALL PossibleRemoveNonCarFromWorld(br_actor* pActor);

extern void C2_HOOK_FASTCALL PutNonCarBackInWorld(br_actor* pActor);

extern int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime);

extern int C2_HOOK_FASTCALL MoveCamToIncident(tCar_spec* pCar, tIncident_type* pType, float* pSeverity, tIncident_info* pInfo, int* pNext_incident_time);

extern void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* pCar, tU32 pTime);

extern int C2_HOOK_FASTCALL CheckForWall(br_vector3* pStart, br_vector3* pEnd);

extern void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

extern void C2_HOOK_FASTCALL SaveCameraPosition(int pIndex);

extern void C2_HOOK_FASTCALL RestoreCameraPosition(int pIndex);

// DrVector3RotateY

extern void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime);

extern void C2_HOOK_FASTCALL NormalPositionExternalCamera(tCar_spec* pCar, tU32 pTime);

extern void C2_HOOK_FASTCALL PositionPedCam(tPed_character_instance* pPed, tU32 pTime);

extern void C2_HOOK_FASTCALL PositionDroneCam(tU32 pTime);

// MoveWithWheels

// SwingCamera

extern void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor);

extern void C2_HOOK_FASTCALL PointCamera(const br_vector3* pPos, br_matrix34* pMat);

extern int C2_HOOK_FASTCALL CollideCamera2(br_vector3* pCar_pos, br_vector3* pCam_pos, br_vector3* pOld_camera_pos, int pManual_move, tPhysics_object* pCollision_info);

extern int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos);

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

extern void C2_HOOK_FASTCALL FreezeCamera(void);

extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

// FindACar

// FindAPed

// MouseClickInActionReplay

#endif // GUARD_59_CAMERA_H

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
extern br_vector3 gAction_replay_manual_camera_target_pos;
extern tCar_spec* gAction_replay_manual_camera_target_car;
extern tPedestrian* gAction_replay_manual_camera_target_ped;
extern br_actor* gAction_replay_manual_camera_target_actor;
extern int gCamera_frozen;
extern tSave_camera gSave_camera[2];
extern int gCamera_mode;
extern float gFLOAT_006792fc;
extern br_angle gOld_yaw__car;
extern br_angle gOld_zoom;
extern br_vector3 gCamera_pos_before_collide;
extern br_actor* gPed_actor;

extern float gCamera_cockpit_hither;
extern tActionReplayCameraMode gAction_replay_camera_mode;

extern void C2_HOOK_FASTCALL DoCameraControls(tCamera_key_flags* pCamera_controls, tU32 pTime);

extern void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime);

// CheckCameraHither

extern tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void);

// AmIGettingBoredWatchingCameraSpin

extern void C2_HOOK_FASTCALL ViewNetPlayer(void);

extern void C2_HOOK_FASTCALL ViewOpponent(void);

extern void C2_HOOK_FASTCALL ReallyViewOpponent(void);

extern void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar);

// SetPanningFieldOfView

extern int C2_HOOK_FASTCALL IsCameraTypeAllowed(int pCamera_mode);

extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);

extern void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);

extern void C2_HOOK_FASTCALL ChangeCameraType(void);

extern void C2_HOOK_FASTCALL ToggleCockpit(void);

// PositionCarMountedCamera

extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime);

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

extern int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime);

// MoveCamToIncident

extern void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* pCar, tU32 pTime);

extern int C2_HOOK_FASTCALL CheckForWall(br_vector3* pStart, br_vector3* pEnd);

extern void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c);

extern void C2_HOOK_FASTCALL SaveCameraPosition(int pIndex);

extern void C2_HOOK_FASTCALL RestoreCameraPosition(int pIndex);

extern void C2_HOOK_FASTCALL DrVector3RotateY(br_vector3* v, br_angle t);

extern void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime);

extern void C2_HOOK_FASTCALL NormalPositionExternalCamera(tCar_spec* pCar, tU32 pTime);

// PositionPedCam

// PositionDroneCam

extern void C2_HOOK_FASTCALL MoveWithWheels(tCar_spec* pCar, br_vector3* pDir, int pManual_swing);

extern void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime, tCar_spec* pCar);

extern void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor);

extern void C2_HOOK_FASTCALL PointCamera(const br_vector3* pPos, br_matrix34* pMat);

extern int C2_HOOK_FASTCALL CollideCamera2(br_vector3* pCar_pos, br_vector3* pCam_pos, br_vector3* pOld_camera_pos, int pManual_move, tPhysics_object* pCollision_info);

extern int C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos);

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

extern void C2_HOOK_FASTCALL FreezeCamera(void);

extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

extern tCar_spec* C2_HOOK_FASTCALL FindACar(br_vector3* pPos, br_vector3* pDir, br_scalar* pT);

extern tPedestrian* C2_HOOK_FASTCALL FindAPed(br_vector3* pPos, br_vector3* pDir, br_scalar* pT, br_actor** pActor);

extern void C2_HOOK_FASTCALL MouseClickInActionReplay(int pX_coord, int pY_coord, int pUnknown);

#endif // GUARD_59_CAMERA_H

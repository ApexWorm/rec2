/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_61_PEDESTRN_PHYS_H
#define GUARD_61_PEDESTRN_PHYS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gAnimalsOn;
extern int gFlameThrowerOn;
extern int gExplosives_on;
extern int gGoreLevel;

extern float gPed_tumble_spin;
extern const char* gForm_non_humanoid_names[1];
extern const float gFlamed_ped_flame_scales[7];
extern int gGhost_pedestrians;

extern int C2_HOOK_FASTCALL OrientationChanged(const br_matrix34* pOrientation_1, const br_matrix34* pOrientation_2, float pCos_angle);

extern void C2_HOOK_FASTCALL AssertRootObjectsMatrix(tPed_character_instance* pPed);

extern void C2_HOOK_FASTCALL InitFaceCaches(void);

extern void C2_HOOK_FASTCALL ResetPedFaceCache(void);

extern void C2_HOOK_FASTCALL SetPedCurrentFace(tPedestrian* pPed, br_vector3* pPos);

extern void C2_HOOK_FASTCALL RecachePedestrian(tPedestrian* pPed, br_vector3* pPos, tU32 pTime);

extern void C2_HOOK_FASTCALL SetCharacterDirectionAR(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);

extern void C2_HOOK_FASTCALL PedApplyPedDir(tU32 pId, const br_vector3* pDir);

extern void C2_HOOK_FASTCALL SetCharacterMoveAR(tPed_character_instance* pCharacter, int pMove_action, float pSpeed, int pArg4, int pArg5, tU32 pTime);

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

extern void C2_HOOK_FASTCALL PedApplyPedFPChange(tPedestrian* pPed, float pSpeed);

// PedApplyStatus

// PedUndoStatus

extern void C2_HOOK_CDECL SetCharacterPhysicsLevelAR(tPed_character_instance* pCharacter, tU32 pLevel, ...);

// PedApplyPedPhysics

extern void C2_HOOK_FASTCALL PedApplyPedPos(tPedestrian* pPedestrian, br_vector3* pPos);

extern void C2_HOOK_FASTCALL PedApplyVanishDismembered(tPedestrian* pPedestrian, tU32 pValue);

// SetCharacterPositionAR

extern void C2_HOOK_FASTCALL SetCharacterBoneModelAR(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set);

// PedApplyPedModelChangeP

// PedApplyPedModelChange

extern int C2_HOOK_FASTCALL DismemberCharacterAR(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pAngular_velocity);

// PedApplyDismember

extern void C2_HOOK_FASTCALL PedUndoDismember(tPedestrian* pPed, int pBone_index, tS8 pValue);

extern void C2_HOOK_FASTCALL ReadPedSpecs(FILE* pF);

extern br_vector3* C2_HOOK_FASTCALL GetPedPos(tPedestrian* pPed);

extern void C2_HOOK_FASTCALL SetUpGibSlicks(void);

extern void C2_HOOK_FASTCALL ReadInGiblets(FILE* pFile);

extern void C2_HOOK_FASTCALL ReadSettingsFile(void);

extern void C2_HOOK_FASTCALL InitOtherPedStuff(void);

extern void C2_HOOK_FASTCALL InitNapalmNolts(void);

extern void C2_HOOK_FASTCALL InitPolyPedSystem(void);

extern void C2_HOOK_FASTCALL DisposePedSpawnSpec(void);

extern void C2_HOOK_FASTCALL DisposeCharacterInstances(void);

extern void C2_HOOK_FASTCALL DisposePedStuff(void);

extern FILE* C2_HOOK_FASTCALL BonerOpenCharacterForm(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenDefaultMoves(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenPersonality(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenMoves(const char* pName);

extern FILE* C2_HOOK_FASTCALL BonerOpenRemaps(const char* pFile_name);

extern void C2_HOOK_FASTCALL BonerReadPersonalityModels(const char* pName);

extern void C2_HOOK_FASTCALL MakeActorRenderable(br_actor* pActor, int pBone_index, tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL StopActorBeingRenderable(br_actor* pActor);

extern int C2_HOOK_FASTCALL IsActorRenderworthy(br_actor* pActor);

extern void C2_HOOK_FASTCALL SetPedXZDirection(tPedestrian* pPed, br_vector3* pDir, float pSpeed, tU32 pTime);

extern void C2_HOOK_FASTCALL StopPedNoise(tPedestrian* pPed);

// MakePedNoise

extern void C2_HOOK_FASTCALL SetPedMove(tPedestrian* pPed, int pMove_action, int pWalk_speed_factor, int pArg4, int pArg5, tU32 pTime, tPed_action pAction);

extern void C2_HOOK_FASTCALL StartPedRunning(tPedestrian* pPed, tU32 pTime, int pArg3);

// AcceleratePed

// MungePedHeadAnim

extern void C2_HOOK_FASTCALL SetPedHeadAnim(tPedestrian* pPed, const tPed_anim_seq* pSeq, tU32 pTime);

// ClearPedHeadAnim

extern void C2_HOOK_FASTCALL ScarePedestrian(tPedestrian* pPed, tU32 pTime, int pPanic, int pForce);

// PedAnimCausesMovement

// StopSmoothTurning

extern void C2_HOOK_FASTCALL SetKnockedOverMove(tPedestrian* pPed, tU32 pTime);

extern void C2_HOOK_FASTCALL DoPostElectricution(tPedestrian* pPed, tU32 pTime, float pSpeed, float pSpin);

extern int C2_HOOK_FASTCALL CBMoveCompleted(tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL CBLoadForm(tPed_form* pPed_form, FILE* pF);

extern void C2_HOOK_FASTCALL CBDisposeForm(tPed_form* pForm);

extern void C2_HOOK_FASTCALL CBLoadPersonality(tPed_personality* pPersonality, FILE* pF);

extern void C2_HOOK_FASTCALL CBDisposePersonality(tPed_personality* pPersonality);

extern void C2_HOOK_FASTCALL CBFillInObject(tPhysics_object* pObject, undefined4 pType);

extern void C2_HOOK_FASTCALL DoGibShower(tU32 pId, tU32 pTime, unsigned int pSeed, float pForce, int pStart_size, br_vector3* pPosition, br_vector3* pAlternate_position, br_vector3* pImpactee_velocity);

extern void C2_HOOK_FASTCALL PedApplyGibShower(tU32 pId, tU32 pTime, unsigned int pSeed, float pForce, int pStart_size, br_vector3* pPosition, br_vector3* pAlternate_position, br_vector3* pImpactee_velocity);

// PedUndoGibShower

extern void C2_HOOK_FASTCALL DoGiblets(tPedestrian* pPed, tPhysics_object* pObject, tPhysics_object* pOther_object, float pForce, br_vector3* pPos, int pStart_size);

extern void C2_HOOK_FASTCALL DoBloodSpurt(tU32 pId, tU32 pTime, unsigned int pSeed, br_vector3* pPosition, br_vector3* pImpactee_velocity, br_vector3* pNormal);

// DoSpurt

extern void C2_HOOK_FASTCALL PedApplyBloodSpurt(tU32 pId, tU32 pTime, unsigned int pSeed, br_vector3* pPosition, br_vector3* pImpactee_velocity, br_vector3* pNormal);

extern void C2_HOOK_FASTCALL PedUndoBloodSpurt(tU32 pCurrent_time, tU32 pId, tU32 pTime, unsigned int pSeed, br_vector3* pPosition, br_vector3* pImpactee_velocity, br_vector3* pNormal);

extern void C2_HOOK_FASTCALL OneLessPed(tPedestrian* pPed);

extern void C2_HOOK_FASTCALL ScoreForKilledPedestrian(tPedestrian* pPed, float pHeight);

extern void C2_HOOK_FASTCALL KillPedestrian(tPedestrian* pPed, tPhysics_object* pCollision_info);

extern int C2_HOOK_FASTCALL DismemberPed(tPedestrian* pPed, int pBone_index);

extern int C2_HOOK_FASTCALL DamagePedestrian(tPedestrian* pPed, int pDamage, int pArg3, int pArg4, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL SetLastHitter(tPedestrian* pPed, tPhysics_object* pObject);

// MakePedVanish

extern int C2_HOOK_FASTCALL CBPassiveCollision(tPed_character_instance* pCharacter, tPhysics_object* pObject, tPhysics_object* pOther_object);

// BastardIt

extern int C2_HOOK_FASTCALL MungeProxRay(tPedestrian* pPed, tPhysics_object* pObject, float pDistance_squared, tU32 pTime);

// DoToPeds

// DoPedAnnihilator

// RepulseIt

// DoPedestrianRepulsifictor

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

extern void C2_HOOK_FASTCALL KillNapalmBolt(tNapalm_bolt* pBolt);

extern void C2_HOOK_FASTCALL MungeNapalm(void);

extern int C2_HOOK_FASTCALL PedHeadingTowardsCollisionObject(tPedestrian* pPed, tPhysics_object** pObject, float* pDistance_squared, br_vector3* pPos, tU32 pTime);

extern int C2_HOOK_FASTCALL OutsideFaceXZ(const br_vector3* pPos, const tFace_ref* pFace);

extern br_scalar C2_HOOK_FASTCALL RescanPedProximity(tPedestrian* pPed, const br_vector3* pDir);

extern tPed_face_cache_0x34* C2_HOOK_FASTCALL RecacheAndSetFace(tPedestrian* pPed, int* pOn_face, int* pFace_changed, br_vector3* pPos, int pForce, tU32 pTime);

extern void C2_HOOK_FASTCALL PedScanForObjects(tPedestrian* pPed, tU32 pTime);

extern void C2_HOOK_FASTCALL CatAndMouseBonus(tPedestrian* pPed);

extern void C2_HOOK_FASTCALL StillifyCorpse(tPedestrian* pPed, tU32 pTime, int pArg3);

extern void C2_HOOK_FASTCALL StraightenOutPed(tPedestrian* pPed, tU32 pTime);

extern int C2_HOOK_FASTCALL ActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject, tFace_ref* pFace);

extern int C2_HOOK_FASTCALL CBActiveHalted(tPed_character_instance* pCharacter, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL RecordPedSlide(tPedestrian* pPed, tFace_ref* pFace, br_vector3* pPos, br_vector3* pVelocity);

extern void C2_HOOK_FASTCALL PedProcessForces(tPhysics_object* pObject, tPhysics_object* pOther_object, tFace_ref* pFace, float pForce, br_vector3* pPos);

extern void C2_HOOK_FASTCALL CBMovedByPhysics(tPed_character_instance* pCharacter, tPhysics_object* pObject, tU32 pTime);

extern int C2_HOOK_FASTCALL GetPedCount(void);

extern int C2_HOOK_FASTCALL GetPedPosition(int pIndex, br_vector3* pPos);

extern void C2_HOOK_CDECL TurnLimbsOnAndOff(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

extern void C2_HOOK_FASTCALL SetModelCallBacks(tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL BuildPedestrian(tPedestrian* pPed, const char* pGroup_name, const br_vector3* pPos, br_vector3* pDir, tPed_movement_spec* pMovement_spec, tRace_ped_exclusion_spec* pExclusion_spec);

extern br_scalar C2_HOOK_FASTCALL AreaOfTriangle(const br_vector3* pV0, const br_vector3* pV1, const br_vector3* pV2);

extern void C2_HOOK_FASTCALL MakeRandomPointInTriangle(br_vector3* pPos, const br_vector3* pV0, const br_vector3* pV1, const br_vector3* pV2);

extern void C2_HOOK_FASTCALL SpawnPedsOnFace(br_face* pFace, br_model* pModel);

extern void C2_HOOK_FASTCALL FinishUpLoadingPeds(void);

extern void C2_HOOK_FASTCALL MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo(void);

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

extern void C2_HOOK_FASTCALL SetThisPedPhysicing(tPedestrian* pPed);

#ifdef REC2_MATCHING
extern void C2_HOOK_FASTCALL FUN_004d3600(void);
#endif

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

extern void C2_HOOK_FASTCALL RenderLimbs(void);

// MungePedestrians

// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

extern void C2_HOOK_FASTCALL ResetPedSystem(void);

extern void C2_HOOK_FASTCALL ResetNapalmBolts(void);

extern void C2_HOOK_FASTCALL ARResetNapalmBolts(void);

extern void C2_HOOK_FASTCALL AdjustNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions);

extern void C2_HOOK_FASTCALL InitPedsForRace(void);

// RenderElectroBastardRays

extern void C2_HOOK_FASTCALL AdjustProxRay(int pRay_index, tU16 pCar_ID, tU16 pPed_index, tU32 pTime);

// ResetProxRay

extern tPed_character_instance* C2_HOOK_FASTCALL GetTestPed(void);

extern void C2_HOOK_FASTCALL NextPedCam(void);

extern void C2_HOOK_FASTCALL PrevPedCam(void);

extern void C2_HOOK_FASTCALL FindNearestPed(void);

extern void C2_HOOK_FASTCALL ResetPedNearness(void);

extern void C2_HOOK_FASTCALL GetPedestrianDebugInfo(char* pBuffer, int pPed_index);

extern void C2_HOOK_FASTCALL ShowActivePedCount(void);

extern void C2_HOOK_FASTCALL PedPreCollisionStuff(void);

extern void C2_HOOK_FASTCALL UpdateIfBackwardsAR(void);

extern void C2_HOOK_FASTCALL ChangePedScaling(const br_vector3* pScale);

// ChangeHeadScaling

extern void C2_HOOK_FASTCALL MakeMaterialBlendy(br_material* pMaterial);

extern void C2_HOOK_FASTCALL UnmakeMaterialBlendy(br_material* pMaterial);

extern void C2_HOOK_FASTCALL TurnOffCollisions(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL TurnOnCollisions(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL UpdateModels(br_model* pModel);

extern void C2_HOOK_FASTCALL TurnOnGhostPeds(void);

extern void C2_HOOK_FASTCALL TurnOffGhostPeds(void);

extern void C2_HOOK_FASTCALL MutatePed(tPed_character_instance* pCharacter, void* pUnused, tU8* pAngle_x, tU8* pAngle_y, tU8* pAngle_z);

extern void C2_HOOK_FASTCALL FlushAllPedCaches(void);

extern void C2_HOOK_FASTCALL SavePedStuff(void);

extern void C2_HOOK_FASTCALL ComparePedStuff(void);

extern void C2_HOOK_FASTCALL AdjustPedDiagnostics(tU32 pSubject_index, void* pUnused2, void* pUnused3, void* pUnused4);

extern int C2_HOOK_FASTCALL CalmDownAllPeds(void);

extern void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetZombiePedFolderNames(void);

extern void C2_HOOK_FASTCALL SetBloodPedFolderNames(void);

extern void C2_HOOK_FASTCALL SetAlienPedFolderNames(void);

extern void C2_HOOK_FASTCALL AddChangedPoint(br_vector3* pPoint);

extern int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void);

extern int C2_HOOK_FASTCALL AreWeGoingToOutrageAnimalLoversAroundTheGlobe(void);

extern int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void);

extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);

extern void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn);

extern void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn);

extern void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on);

extern void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel);

#endif // GUARD_61_PEDESTRN_PHYS_H

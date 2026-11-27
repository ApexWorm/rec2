/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_61_PEDESTRN_H
#define GUARD_61_PEDESTRN_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPed_count_limbed_actors;
extern br_actor* gLimbs_actor;
extern tSmashable_initial_position_spec gGiblet_initial_position;
extern float gGiblet_scrape_start;
extern tSlick_spec gPed_slick;
extern int gPed_spurting_period;
extern br_material* gPed_smear_material;
extern tExplosion_animation gPed_explosion_small_blood_cloud;
extern tExplosion_animation gPed_medium_blood_cloud;
extern tExplosion_animation gPed_large_blood_cloud;
extern tMovement_spec gGiblet_initial_speed;
extern tShrapnel_time_info gGiblet_min_max_time;
extern int gGiblet_size_count;

extern int gPed_move_fsm[118][7];

extern tPedForms_vtable* gPed_vtable;

extern int gPed_retain_root_mode;

extern br_material* gPedestrian_character_cloned_materials[50];
extern int gCount_pedestrian_personality_cloned_materials;

extern tPed_remap* gPed_remaps[10];

extern tPed_personality* gPed_personalities[50];
extern float gPed_sin_table[256];
extern float gPedestrian_speed_factor;
extern int gSelected_ped;
extern int gCount_active_peds;
extern int gPeds_suicidal;
extern int gExploding_pedestrians;
extern tNapalm_bolt gNapalm_bolts[5];
extern int gPed_severing_chance;

extern int gPed_severing_damage;
extern br_material* gPed_slick_material;
extern br_material* gNapalm_bolt_materials[7];
extern br_model* gNapalm_bolt_models[7];
extern const char* gNapalm_bolt_texture_names[7];
extern int gCount_changed_points;
extern br_vector3 gChanged_points[10];
extern int gPed_dismember_spurting_member_count;
extern int gPed_nearness;
extern tTWTVFS gTwtPeds;
extern tU32 gDAT_006a0418;
extern tPedestrian* gPTR_00694120;
extern int gPed_points[3];
extern int gPed_score_multiplier;
extern int gPed_recent_points;
extern float gMutant_speed;
extern tMovement_spec gPed_spurt_initial_speed;
extern tSmashable_initial_position_spec gPed_spurt_initial_position;
extern int gDancing_peds;
extern int gDrunk_pedestrians;
extern int gPeds_already_munged;
extern int gPed_cache_sizes_1[4];
extern tPed_face_cache_0x34* gPed_face_cache_heads[4];
extern int gPed_cache_notify_flags;
extern int gCount_race_pedestrian_specs;
extern tPed_group_spec* gPed_groups;
extern int gPed_groups_count;
extern tPed_movement_spec* gPed_movements;
extern int gPed_movements_count;
extern tRace_ped_spec* gRace_pedestrian_specs;
extern br_vector3 gPed_scale;
extern float gAverage_ped_scale;
extern tPed_cache_006944c0 gPed_cache_006944c0[100];
extern tPed_face_cache_0x34* gPed_face_caches[4];
extern int gPipe_halted_ped_status;
extern int gPed_valium_left;

// ClearOutMorphs

// InitBoner

// DRVector3SafeCross

// DRVector3SafeCrossXZ

extern void C2_HOOK_FASTCALL ScaleModelXYZ(br_model* pModel, br_scalar pScale_x, br_scalar pScale_y, br_scalar pScale_z);

extern void C2_HOOK_FASTCALL ScaleModel(br_model* pModel, float pScale);

// DisposeMove

// DisposeAllLoadedMoves

// ReadRemap

extern void C2_HOOK_FASTCALL DisposeRemap(tPed_remap* pRemap);

extern void C2_HOOK_FASTCALL DisposeAllLoadedRemaps(void);

// FindOrOpenRemap

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

extern tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName);

extern void C2_HOOK_FASTCALL DisposeCharacterForm(tPed_form* pForm);

// DisposeAllLoadedForms

extern void C2_HOOK_FASTCALL DisposeCharacterInstance(tPed_character_instance* pCharacter);

extern tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName);

// MakeRotationMatrix

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

extern tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName);

// DisposePersonality

// DisposeAllLoadedPersonalities

extern tPed_personality* C2_HOOK_FASTCALL FindOrOpenPersonality(const char* pName);

extern tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pGroup_name, br_matrix34* pMat34);

extern br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance* pCharacter);

extern br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index);

extern br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(const tPed_character_instance* pCharacter);

extern void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, int pArg2, int pArg3);

extern int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex);

extern int C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter);

// CharacterNoLongerRenderable

extern void C2_HOOK_FASTCALL CalcCollisionObjectThings(tPhysics_object* pObject, br_bounds3* pFrame, int pNull, br_vector3* pBoneVec, br_vector3* pBonePtr);

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, int pArg3, int pArg4);

extern int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance* pCharacter, int pArg2, int pArg3);

extern tPhysics_object* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance* pPed);

// CharacterNoLongerCollideworthy

extern void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, const br_vector3* pV);

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

// MakeCharacterPhysicworthy

extern void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pCharacter);

extern int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2);

extern int C2_HOOK_FASTCALL CharacterNoLongerPhysicworthy(tPed_character_instance* pCharacter);

extern void C2_HOOK_CDECL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, tU32 pLevel, ...);

extern void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter);

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

extern int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pPed, int pMove_action, float pSpeed, int pArg4, int pArg5, tU32 pTime);

// DRMatrix34RotateCos

extern void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest);

extern void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp);

extern void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3);

// DisposeClonedMaterials

extern void C2_HOOK_FASTCALL DisposeAllBonerData(void);

extern int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set);

extern void C2_HOOK_FASTCALL SetCharacterBoneModelP(tPed_character_instance* pCharacter, int pBone_index, br_model* pModel, int pArg4);

extern void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, int pModel_index, int pModel_set);

extern br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index);

extern int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter);

// BonerPassiveCollision

extern int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter);

// BonerPedMovedByPhysics

extern int C2_HOOK_FASTCALL DismemberCharacter(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pAngular_velocity);

extern float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum);

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

extern void C2_HOOK_FASTCALL ScaleBone(int pBone_index, br_vector3* pScale, int pArg3);

extern void C2_HOOK_FASTCALL MungeAllCharacterMaterials(material_cbfn* pCallback);

extern void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL *pCallback)(tPhysics_object*));

extern void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL *pCallback)(br_model*));

extern void C2_HOOK_FASTCALL DisableOverallMovement(void);

extern void C2_HOOK_FASTCALL EnableOverallMovement(void);

extern void C2_HOOK_FASTCALL EnableRetainRootMode(void);

extern void C2_HOOK_FASTCALL DisableRetainRootMode(void);

#endif // GUARD_61_PEDESTRN_H

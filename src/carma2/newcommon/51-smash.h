/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_51_SMASH_H
#define GUARD_51_SMASH_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gAllow_material_adapt;
extern br_material* gMaterials_to_adapt[200];
extern tSmashable_item_spec gUnknown_smashable_006a3660;
extern tSmashable_item_spec gPowerup_pickup_smashable;
extern int gMaterials_to_adapt_count;
extern int gShrapnel_child_count;

extern tSmashable_item_spec* gTrack_smashable_environment_specs;
extern tSmashable_item_spec* gSmashable_track_indexable_triggers[100];
extern br_actor* gSmashable_noncar_shrapnel_actors[100];
extern int gCount_smashable_noncar_shrapnel_actors;
extern const char* gSmashable_track_environment_path;
extern int gCount_track_smashable_environment_specs;
extern int gCapacity_track_smashable_environment_specs;
extern int gCount_track_smashable_environment_specs_2;
extern tStored_smash* gNet_host_smashes;
extern tQueued_net_smash gQueued_net_smashes[50];
extern int gINT_006a8290;
extern int gDoing_net_smash;
extern tPowerup_queue_item gPowerup_queue[50];
extern int gSize_powerup_queue;

extern tExplosion gExplosions[50];
extern tRepair_animation gRepair_animations[25];
extern br_vector3 gZero_vector__smash;
extern tDecal gDecals[50];
extern int gCount_queued_smashes;

extern int gINT_006a829c;
extern tSmash_explosion gSmash_explosions[20];

typedef struct {
    br_vector3 origin;
    br_vector2 map_origin;
    br_vector2 map_other;
    br_scalar map_from_geometry[2][2];
    br_face* face;
} tSmash_triangle_mapping;

typedef struct {
    int axis_0;
    int axis_1;
    int axis_2;
    br_vector3 normal;
    br_scalar d;
    br_vector2 map_edge;
    tSmash_triangle_mapping triangles[2];
} tSmash_quad_mapping;

extern int C2_HOOK_FASTCALL PointOutOfSightNotAR(const br_vector3* pPos, br_scalar pMax_distance);

extern void C2_HOOK_FASTCALL InitFogificateMaterials(void);

extern void C2_HOOK_FASTCALL FogificateMaterials(tRendererShadingType pShading_type);

extern void C2_HOOK_FASTCALL AutoFogificateMaterials(tRendererShadingType pShading_type);

extern void C2_HOOK_FASTCALL AddFogificateMaterial(br_material* pMaterial);

extern void C2_HOOK_FASTCALL EnableAutoFogification(void);

extern void C2_HOOK_FASTCALL DisableAutoFogification(void);

extern void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial);

extern void C2_HOOK_FASTCALL InitExplosions(void);

extern void C2_HOOK_FASTCALL ResetExplosions(void);

extern void C2_HOOK_FASTCALL RemoveAllExplosionettes(void);

extern void C2_HOOK_FASTCALL MungeExplosions(void);

extern void C2_HOOK_FASTCALL CopyExplosionData(int pIndex, tExplosion* pExplosion);

// DoExplosionette

extern void C2_HOOK_FASTCALL Explode(tExplosion_animation *pExplosion_animation,br_actor *pActor, const br_bounds3* pBounds, const br_vector3* pPos);

extern void C2_HOOK_FASTCALL RemoveExplosions(tCar_spec* pCar_spec);

// CalcRandomPointBetween

extern void C2_HOOK_FASTCALL ApplyInitialMovement(tMovement_spec* pSpeed_info, br_vector3* pImpactee_velocity, br_vector3* pVelocity, br_vector3* pOmega, float pScale, br_vector3* pNormal, br_vector3* pSource_position, br_vector3* pPosition);

// ApplyTimeSpec

extern tSmash_vertex* C2_HOOK_FASTCALL FindShrapnelFragment(br_model* pModel, tU32 pId, int pPriority);

extern void C2_HOOK_FASTCALL SetShrapnelFragment(tSmash_vertex* pFragment, tMovement_spec* pSpeed_info, br_vector3* pImpactee_velocity, float pScale, tShrapnel_time_info* pTime_spec, tU32 pTime, br_vector3* pNormal, br_vector3* pSource_position, br_vector3* pPosition);

// ShatterObject

extern void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec);

extern void C2_HOOK_FASTCALL ActuallySmashPart(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmash, int pLevel);

extern void C2_HOOK_FASTCALL MakeSmashSound(tConnotations* pConnotations, br_vector3* pPos);

extern void C2_HOOK_FASTCALL MungeSmokeFire(br_actor* pActor, tNon_car_spec* pNon_car, tSmoke_fire_chance* pSmoke_fire_chance);

// Random34Matrix

extern br_scalar C2_HOOK_FASTCALL SetInitPosition(br_actor* pActor, tSmashable_initial_position_spec* pInitial_position, br_actor* pSource_actor, br_vector3* pPosition, br_vector3* pAlternate_position, br_actor* pReference_actor, br_bounds3* pBounds, br_matrix34* pMatrix);

// DoGhostPartShrapnel

// DoNonCarShrapnel

extern void C2_HOOK_FASTCALL DoShrapnel(tCar_crush_smashable_part* pSmashable, tShrapnel_spec* pShrapnel, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, br_actor* pActor, tU32 pTime, uintptr_t pId, br_matrix34* pMat, br_bounds3* pBounds, br_vector3* pArg12, float pArg13, br_vector3* pArg14, int pArg15);

extern void C2_HOOK_FASTCALL DoAward(tPhysics_object* pObject, tAward_info* pAward, float pForce_magnitude);

extern void C2_HOOK_FASTCALL DoSpillage(tSlick_spec* pSlick, br_actor* pActor);

extern void C2_HOOK_FASTCALL DoSpecial(tSpecial_effects_spec* pSpecial, br_actor* pActor, br_bounds3* pBounds);

extern intptr_t C2_HOOK_CDECL GiveNonCarsAShove(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL QueueDelayedSideEffect(int pType, void* pData, int pSize, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL DoDelayedNonCar(tU32 pTime, tDelayed_non_car* pDelayed_non_car);

extern void C2_HOOK_FASTCALL DoDelayedSmash(tDelayed_smash* pDelayed_smash);

extern void C2_HOOK_FASTCALL MungeDelayedSideEffects(void);

extern void C2_HOOK_FASTCALL ActivateNonCarSideEffects(br_actor* pActor, tNon_car_cuboid_activations* pActivations, tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ActivateSmashSideEffects(tSmashable_item_spec* pSpec, tPhysics_object* pObject, tShrapnel_side_effects* pSide_effects, br_vector3* pForce, float pForce_magnitude);

extern void C2_HOOK_FASTCALL DoSideEffects(br_actor* pActor, tSmashable_item_spec* pSpec, tPhysics_object* pObject, tSide_effects* pSide_effects, br_vector3* pPosition, float pForce_magnitude, br_vector3* pForce);

extern void C2_HOOK_FASTCALL DoVariableEffects(tVariable_changes* pChanges);

extern void C2_HOOK_FASTCALL DoConnotations(tPhysics_object* pObject, tSmashable_item_spec* pSpec, tCar_crush_smashable_part* pSmashable, tConnotations* pConnotations, int pSeed, br_vector3* pVelocity, br_vector3* pPosition, br_vector3* pSound_position, br_actor* pActor, tU32 pTime, uintptr_t pId, br_matrix34* pMat, br_bounds3* pBounds, br_vector3* pArg14, float pForce_magnitude, br_vector3* pForce, int pNo_side_effects);

extern void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor* pActor);

extern void C2_HOOK_FASTCALL DoConnotationsMultiLevel(tPhysics_object* pObject, tSmashable_item_spec* pSpec, tCar_crush_smashable_part* pSmashable, tConnotations* pConnotations, int pSeed, br_vector3* pVelocity, br_vector3* pPosition, br_vector3* pSound_position, br_actor* pActor, tU32 pTime, uintptr_t pId, br_matrix34* pMat, int pFrom_level, int pTo_level, br_vector3* pForce, int pNo_side_effects);

extern void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tCar_spec* pCar, tPhysics_object* pObject, br_actor* pActor, tSmashable_item_spec* pSpec, tCar_crush_smashable_part* pSmashable, br_vector3* pVelocity, br_vector3* pPosition, int pLevel, br_vector3* pForce, int pNo_side_effects);

extern void C2_HOOK_FASTCALL InitSmashQueue(void);

extern void C2_HOOK_FASTCALL FlushPowerupQueue(void);

extern void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups);

extern tSmashable_level* C2_HOOK_FASTCALL QueueSmashIfNecessary3(tCar_spec* pCar, tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_actor* pActor, tSmashable_item_spec* pSpec, tCar_crush_smashable_part* pSmashable, float pMagnitude, int pArg9, int pArg10);

extern void C2_HOOK_FASTCALL QueueSmashIfNecessary2(tCar_spec* pCar, tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_actor* pActor, tSmashable_item_spec* pSpec, tCar_crush_smashable_part* pSmashable, float pMagnitude, int pArg9, int pArg10);

extern void C2_HOOK_FASTCALL AllocateSmashLevels(tSmashable_level** pSmashable_levels, int pCount_smashable_levels);

extern void C2_HOOK_FASTCALL ReadSmashSounds(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL ReadShrapnelMovementSpec(FILE* pFile, tMovement_spec* pInitial_speed);

extern void C2_HOOK_FASTCALL ReadShrapnelInitialPosition(FILE* pF, tSmashable_initial_position_spec* pInitial_pos);

extern void C2_HOOK_FASTCALL ReadShrapnelTimeInfo(FILE* pF, tShrapnel_time_info* pMin_max_time);

extern intptr_t C2_HOOK_CDECL NoteChildren(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL ReadSmokeFireChance(FILE* pF, tSmoke_fire_chance* pSmoke_fire_chance);

extern br_actor* C2_HOOK_FASTCALL LoadActorFromRace(const char *pName);

extern void C2_HOOK_FASTCALL ReadSpillData(FILE* pF, tSlick_spec* pSlick_spec);

extern void C2_HOOK_FASTCALL ReadExplosion(FILE* pF, tExplosion_animation* pAnimation);

extern void C2_HOOK_FASTCALL FreeExplosionAnimation(tExplosion_animation* pAnimation);

extern void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count);

extern void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds);

#ifdef REC2_MATCHING
extern void C2_HOOK_FASTCALL FUN_004ef830(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, undefined4 pArg3, int pLevel);
#endif

// FreeExplosion

extern void C2_HOOK_FASTCALL ReadSpecialEffectsSpec(FILE* pF, tSpecial_effects_spec* pSpecial_effects);

// ReadBounds

extern void C2_HOOK_FASTCALL ReadNoncarSideEffects(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);

extern void C2_HOOK_FASTCALL ReadSmashSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);

extern void C2_HOOK_FASTCALL ReadSideEffects(FILE* pF, tSide_effects* pSide_effects);

extern void C2_HOOK_FASTCALL ReadLightingInformation(FILE* pF, int* pLighting_information);

extern void C2_HOOK_FASTCALL ReadAward(FILE* pF, tAward_info* pAward_info);

extern void C2_HOOK_FASTCALL ReadVariableChanges(FILE* pF, tVariable_changes* pVariable_changes);

extern void C2_HOOK_FASTCALL ReadConnotations(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL ReadSmashLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage);

extern void C2_HOOK_FASTCALL ReadSmashableData(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage);

extern void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel);

// KillFragment

extern void C2_HOOK_FASTCALL AddRepairAnimationToQueue(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable);

extern void C2_HOOK_FASTCALL RepairSmash(tCar_spec* pCar_spec, tCar_crush_buffer_entry* pSmash_data);

extern void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec* pCar_Spec, tCar_crush_buffer_entry* pSmash_data);

// InitGlassFragments

//static void C2_HOOK_FASTCALL InitDecals(void);

extern void C2_HOOK_FASTCALL RemoveAllDecals(void);

extern void C2_HOOK_FASTCALL ReadPowerupSmash(FILE* pF);

extern void C2_HOOK_FASTCALL InitSmashing(void);

extern void C2_HOOK_FASTCALL VerySimpleRotate(br_scalar pDt, br_matrix34* pMatrix, br_vector3* pOmega);

extern void C2_HOOK_FASTCALL DoFragMovement(tSmash_vertex* pFragment, tU32 pTime, float pDelta_time);

extern void C2_HOOK_FASTCALL MungeGlassFragments2(int pEnd_race);

extern void C2_HOOK_FASTCALL MungeAnimationRepairs(void);

// MungeGlassFragments

// KillAllGlassFragments

extern void C2_HOOK_FASTCALL CleanUpSmashStuff(void);

extern void C2_HOOK_FASTCALL DisposeSmashData(tCar_crush_buffer_entry* pCrush_data);

extern void C2_HOOK_FASTCALL DoBackwardShrapnel(tU32 pTime_now, tCar_crush_smashable_part* pSmashable, tShrapnel_spec* pShrapnel, int pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, br_actor* pActor, tU32 pTime, uintptr_t pId, br_matrix34* pMat, br_bounds3* pBounds, br_vector3* pArg12, float pArg13, br_vector3* pArg14);

extern void C2_HOOK_FASTCALL InitRepairAnimations(void);

extern void C2_HOOK_FASTCALL SplondificatalizeIdentifier(br_actor* pActor, char **pIdentifier, int pIndex);

extern void C2_HOOK_FASTCALL ReadSmashReplace(tSmashReplace* pReplace, FILE* pF);

extern void C2_HOOK_FASTCALL ReadSmashTexture(tSmashable_item_spec* pSpec, tSmashable_item_spec_texture_change* pChange, FILE* pF);

extern void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath);

extern void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void);

extern void C2_HOOK_FASTCALL DisposeSmashableEnvironment(void);

// CloneMemory

// CloneEnvSmash

extern void C2_HOOK_FASTCALL FreeEnvSmash(tSmashable_item_spec* pSmash);

extern void C2_HOOK_FASTCALL FreeConnotations(tConnotations* pConnotations, int pKeep_actors);

// MungeSmashMaterialNames

// ExtrapolateTextureCoordinates

extern void C2_HOOK_FASTCALL ExtrapolateGeometryCoordinates(tSmash_quad_mapping* pMapping, br_vector2* pMap, br_vertex* pVertex);

extern void C2_HOOK_FASTCALL ExtrapolateGeometryCoordinates2(tSmash_quad_mapping* pMapping, tSmash_triangle_mapping* pTriangle, br_vector2* pMap, br_vertex* pVertex);

// ExtractDeTiledFaces

extern br_actor* C2_HOOK_FASTCALL AddDecal(tFace_ref* pFace, br_vertex* pVertices, br_material* pMaterial, tU16* pFace_points, tU16* pPoint_indices);

extern int C2_HOOK_FASTCALL InsertQuad(tSmash_quad_mapping* pMapping, br_actor* pActor, br_vertex* pVertices, tU8* pOn_edge, br_vertex** pCorners, br_material* pMaterial, tU16* pFace_points, tU16* pPoint_indices);

// SpawnSmashAndMungeFaces

// DistanceFromFaceND

// PointInFace

extern int C2_HOOK_FASTCALL FindFaceWithPoint(tFace_ref* pFace, br_vector3* pPoint, tFace_ref* pNew_face, int pInclude_same_face);

extern float C2_HOOK_FASTCALL ProcessTexture(tPhysics_object* pObject, br_material* pMaterial, br_actor* pActor, tSmashable_item_spec* pSpec, tSmashable_item_spec* pSpec2, tFace_ref* pFace, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pArg11, int pArg12, int* pSmashed, int pNetwork);

extern void C2_HOOK_FASTCALL ProcessMesh(tPhysics_object* pObject, br_material* pMaterial, br_actor* pActor, tSmashable_item_spec* pSpec, int* pSmashed);

// ProcessModel

extern float C2_HOOK_FASTCALL BashModel(tSmashable_item_spec* pSpec, tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int* pSmashed, int pNetwork);

extern int C2_HOOK_FASTCALL CalcCollObjIDFromCarSpec(tCar_spec* pCar);

extern int C2_HOOK_FASTCALL CalcCollObjID(tPhysics_object* pObject);

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromCollObjID(int pID);

extern tPhysics_object* C2_HOOK_FASTCALL GetCollObjFromID(int pID);

extern void C2_HOOK_FASTCALL SendNetSmash(tPhysics_object* pObject, tFace_ref* pFace, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap);

extern void C2_HOOK_FASTCALL SendAllSmashes(tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL HitPowerup(tPhysics_object* pObject, int pIndex, br_actor* pActor);

extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, tFace_ref* pFace, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);

extern float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);

extern void C2_HOOK_FASTCALL NonCarSnapSmashCallback(tPhysics_object* pObject, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce);

extern int C2_HOOK_FASTCALL SmashFaceMustBeUpdateable(br_material* pMaterial);

extern intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* pData);

extern void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal);

extern void C2_HOOK_FASTCALL ReinitSmashing(void);

extern void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data);

extern intptr_t C2_HOOK_CDECL RepairModels(br_actor* pActor, void* pUser);

extern intptr_t C2_HOOK_FASTCALL RepairAllSmashes(tCar_spec* pCar_spec);

extern intptr_t C2_HOOK_CDECL SmashEverythingCB(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL SmashEverything(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DetermineTilitudinality(br_model* pModel, int* pArea_1, int* pArea_2);

extern void C2_HOOK_FASTCALL GetGlassFragmentCounts(int* pVisible_count, int* pActive_count);

extern void C2_HOOK_FASTCALL SetSmashableModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL LoadWastedExplosion(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups);

extern void C2_HOOK_FASTCALL ChangeSmashModel(br_actor* pActor, br_model* pOld_model, br_model* pNew_model);

extern void C2_HOOK_FASTCALL TurnOnSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL TurnOffSmashModel(br_actor* pActor);

extern void C2_HOOK_FASTCALL SmashRemoveSomeFaces(br_model* pModel, int pCount);

extern void C2_HOOK_FASTCALL SmashRestoreSomeFaces(br_model* pModel, int pFace_count, const tPipe_remove_face* pFace_data, br_material* pMaterial);

extern void C2_HOOK_FASTCALL TurnOnSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL TurnOffSmashDecal(int pIndex);

extern void C2_HOOK_FASTCALL ChangeSmashTexture(br_material* pMaterial, tCar_spec* pCar_spec, br_pixelmap* pOld_texture, br_pixelmap* pNew_texture);

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromCollObjID(int pID);

extern tPhysics_object* C2_HOOK_FASTCALL GetCollObjFromID(int pID);

extern void C2_HOOK_FASTCALL DoNetSmash(tQueued_net_smash* pSmash);

// ReceivedSmashEnvironment

extern void C2_HOOK_FASTCALL DoQueuedNetSmashes(void);

extern br_actor* C2_HOOK_FASTCALL FindTheBastard(const char* pShrapnel_id);

extern void C2_HOOK_FASTCALL ReceivedSmashShraps(tNet_message_chunk_smash_shraps* pContents);

// MungeSmashEdgeTriggers

#endif // GUARD_51_SMASH_H

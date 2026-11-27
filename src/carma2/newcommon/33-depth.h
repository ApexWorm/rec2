/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_33_DEPTH_H
#define GUARD_33_DEPTH_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern int gHas_sky_texture;
extern br_pixelmap* gDepth_shade_table;
extern int gDepth_shade_table_power;
extern int gFog_shade_table_power;
extern int gAcid_shade_table_power;
extern br_pixelmap* gWater_shade_table;
extern int gWater_shade_table_power;
extern br_actor* gForward_sky_actor;
extern tSpecial_volume* gLast_camera_special_volume;

extern int C2_HOOK_CDECL Log2(int pNumber);

extern br_scalar C2_HOOK_FASTCALL CalculateWrappingMultiplier(br_scalar pValue, br_scalar pYon);

extern br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance);

extern void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

extern void C2_HOOK_FASTCALL FogCars(void);

// extern void C2_HOOK_FASTCALL FrobFog(void);

extern void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);

extern br_scalar C2_HOOK_FASTCALL Tan(br_scalar pAngle);

extern br_scalar C2_HOOK_FASTCALL EdgeU(br_angle pSky, br_angle pView, br_angle pPerfect);

extern void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel);

extern br_model* C2_HOOK_FASTCALL CreateHorizonModel(br_actor* pCamera);

extern void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower);

extern void C2_HOOK_FASTCALL InitDepthEffects(void);

extern void C2_HOOK_FASTCALL DoHorizon(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);

// DoDepthCue

// DoFog

// DepthEffect

extern void C2_HOOK_FASTCALL RenderLollipops(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);

extern void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world);

extern void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera);

extern void C2_HOOK_FASTCALL DoDrugWobbleCamera(br_actor* pCamera);

extern int C2_HOOK_FASTCALL DoSubaquaCam(void);

extern void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world);

extern void C2_HOOK_FASTCALL ReduceDepthStart(void);

extern void C2_HOOK_FASTCALL IncreaseDepthStart(void);

extern void C2_HOOK_FASTCALL ReduceDepthEnd(void);

extern void C2_HOOK_FASTCALL IncreaseDepthEnd(void);

extern void C2_HOOK_FASTCALL AssertYons(void);

extern void C2_HOOK_FASTCALL IncreaseYon(void);

extern void C2_HOOK_FASTCALL DecreaseYon(void);

extern void C2_HOOK_STDCALL SetYon(br_scalar pYon);

extern br_scalar C2_HOOK_STDCALL GetYon(void);

extern void C2_HOOK_STDCALL IncreaseAngle(void);

extern void C2_HOOK_STDCALL DecreaseAngle(void);

extern int C2_HOOK_FASTCALL GetSkyTextureOn(void);

extern void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn);

extern void C2_HOOK_FASTCALL ToggleSkyQuietly(void);

extern void C2_HOOK_FASTCALL ToggleSky(void);

extern int C2_HOOK_FASTCALL GetDepthCueingOn(void);

extern void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn);

extern void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void);

extern void C2_HOOK_FASTCALL ToggleDepthCueing(void);

extern void C2_HOOK_FASTCALL ChangeDepthEffect(void);

extern void C2_HOOK_FASTCALL MungeForwardSky(void);

extern void C2_HOOK_FASTCALL MungeRearviewSky(void);

extern void C2_HOOK_FASTCALL SkyTextureChanged(void);

extern void C2_HOOK_FASTCALL SetSkyColour(br_colour c);

#endif // GUARD_33_DEPTH_H

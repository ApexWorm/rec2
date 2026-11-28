/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "57-drone.h"

#include "08-loading1.h"
#include "15-displays.h"
#include "23-piping.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "56-aiworld.h"
#include "60-phil.h"
#include "c2_string.h"
#include "globvars.h"
#include "c2_stdio.h"
#include "platform.h"
#include "rec2_macros.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
// GLOBAL: CARMA2_CHEAT 0x00689968
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x006820d0
// GLOBAL: CARMA2_SW 0x0067c810
// GLOBAL: CARMA2_CHEAT 0x00689978
int gCount_drones;

// GLOBAL: CARMA2_HW 0x00682178
// GLOBAL: CARMA2_SW 0x0067c8b8
tDrone_form gDrone_forms[64];

// GLOBAL: CARMA2_HW 0x0068450c
// GLOBAL: CARMA2_SW 0x0067ec4c
// GLOBAL: CARMA2_CHEAT 0x0068bdb4
int gCount_drone_forms;

// GLOBAL: CARMA2_HW 0x00684510
// GLOBAL: CARMA2_SW 0x0067ec50
// GLOBAL: CARMA2_CHEAT 0x0068bdb8
int gCount_drone_path_nodes;

// GLOBAL: CARMA2_HW 0x006844f8
// GLOBAL: CARMA2_SW 0x0067ec38
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x00684504
// GLOBAL: CARMA2_SW 0x0067ec44
// GLOBAL: CARMA2_CHEAT 0x0068bdac
tDrone_spec* gDrone_specs;

// GLOBAL: CARMA2_HW 0x00684508
// GLOBAL: CARMA2_SW 0x0067ec48
// GLOBAL: CARMA2_CHEAT 0x0068bdb0
tDrone_path_node* gDrone_path_nodes;

/* FIXME: name is a guess (the model is called "StaticDronePathModel") */
// GLOBAL: CARMA2_HW 0x0068451c
// GLOBAL: CARMA2_SW 0x0067ec5c
// GLOBAL: CARMA2_CHEAT 0x0068bdc4
br_model* gStatic_drone_path_model;

// GLOBAL: CARMA2_HW 0x00684520
// GLOBAL: CARMA2_SW 0x0067ec60
// GLOBAL: CARMA2_CHEAT 0x0068bdc8
br_model* gElasticatey_drone_path_model;

/* FIXME: name is a guess */
// GLOBAL: CARMA2_HW 0x00684524
// GLOBAL: CARMA2_SW 0x0067ec64
// GLOBAL: CARMA2_CHEAT 0x0068bdcc
br_actor* gStatic_drone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684528
// GLOBAL: CARMA2_SW 0x0067ec68
// GLOBAL: CARMA2_CHEAT 0x0068bdd0
br_actor* gDrone_path_model_actor;

// GLOBAL: CARMA2_HW 0x00684530
// GLOBAL: CARMA2_SW 0x0067ec70
// GLOBAL: CARMA2_CHEAT 0x0068bdd8
int gDrone_paths_elasticating;

/* FIXME: name is a guess */
// GLOBAL: CARMA2_HW 0x00684538
// GLOBAL: CARMA2_SW 0x0067ec78
// GLOBAL: CARMA2_CHEAT 0x0068bde0
int gStatic_drone_path_model_max_vertices;

/* FIXME: name is a guess */
// GLOBAL: CARMA2_HW 0x0068453c
// GLOBAL: CARMA2_SW 0x0067ec7c
// GLOBAL: CARMA2_CHEAT 0x0068bde4
int gStatic_drone_path_model_max_faces;

// GLOBAL: CARMA2_HW 0x005947b4
// GLOBAL: CARMA2_SW 0x0058f714
// GLOBAL: CARMA2_CHEAT 0x0059b10c
int gSelected_edit_drone_path = -1;

// GLOBAL: CARMA2_HW 0x005947b0
// GLOBAL: CARMA2_SW 0x0058f710
// GLOBAL: CARMA2_CHEAT 0x0059b108
int gSelected_drone_path_node_index = -1;

// GLOBAL: CARMA2_HW 0x005947bc
// GLOBAL: CARMA2_SW 0x0058f71c
// GLOBAL: CARMA2_CHEAT 0x0059b114
int gSelected_drone_form_index = -1;

/* FIXME: name is a guess */
// GLOBAL: CARMA2_HW 0x00684378
// GLOBAL: CARMA2_SW 0x0067eab8
// GLOBAL: CARMA2_CHEAT 0x0068bc20
char gDrone_edit_message[256];

// GLOBAL: CARMA2_HW 0x005947b8
// GLOBAL: CARMA2_SW 0x0058f718
// GLOBAL: CARMA2_CHEAT 0x0059b110
int gSelected_drone_path_index = -1;

// GLOBAL: CARMA2_HW 0x0068452c
// GLOBAL: CARMA2_SW 0x0067ec6c
// GLOBAL: CARMA2_CHEAT 0x0068bdd4
int gShow_drone_paths;

// GLOBAL: CARMA2_HW 0x00684518
// GLOBAL: CARMA2_SW 0x0067ec58
// GLOBAL: CARMA2_CHEAT 0x0068bdc0
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x00681fb0
// GLOBAL: CARMA2_SW 0x0067c6f0
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00684514
// GLOBAL: CARMA2_SW 0x0067ec54
// GLOBAL: CARMA2_CHEAT 0x0068bdbc
int gFrame;

// FUNCTION: CARMA2_HW 0x0044cfc0
// FUNCTION: CARMA2_SW 0x0044cd70
// FUNCTION: CARMA2_CHEAT 0x0044d510
void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...) {

}

// CrappyLittleVector3DPrintf

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

// FUNCTION: CARMA2_HW 0x0044f640
// FUNCTION: CARMA2_SW 0x0044f3e0
FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name) {
    tPath_name the_path;
    FILE* f;

    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    PathCat(the_path, the_path, pDrone_name);
    strcat(the_path, ".TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    return f;
}

// LoadDroneTypeInfo

void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone) {
    PHILRemoveObject(&pDrone->collision_info);
}

void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone) {
    ReallyRemoveDroneFromPHIL(pDrone);
}

// InitDroneCollisionObject

// FUNCTION: CARMA2_HW 0x00451650
// FUNCTION: CARMA2_SW 0x004513e0
// FUNCTION: CARMA2_CHEAT 0x00451ba0
int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone) {

    if (PHILAddObject(&pDrone->collision_info)) {
        return 0;
    }
    PHILSetObjectProperty(&pDrone->collision_info, 0, 1.0);
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 7, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 6, 1);
    return 1;
}

// AddDroneToPHIL

// FUNCTION: CARMA2_HW 0x00451070
// FUNCTION: CARMA2_SW 0x00450e00
// FUNCTION: CARMA2_CHEAT 0x004515c0
void C2_HOOK_FASTCALL PreprocessDronePaths(void) {
    int i;
    int j;

    for (i = 0; i < gCount_drone_path_nodes; i++) {
        for (j = 0; j < gDrone_path_nodes[i].count_sections; j++) {
            BrVector3Sub(&gDrone_path_nodes[i].sections[j].field_0x04, &gDrone_path_nodes[gDrone_path_nodes[i].sections[j].node1].position, &gDrone_path_nodes[i].position);
            gDrone_path_nodes[i].sections[j].field_0x1c = BrVector3Length(&gDrone_path_nodes[i].sections[j].field_0x04);
            BrVector3Normalise(&gDrone_path_nodes[i].sections[j].field_0x10, &gDrone_path_nodes[i].sections[j].field_0x04);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00450600
// FUNCTION: CARMA2_SW 0x00450390
// FUNCTION: CARMA2_CHEAT 0x00450b50
int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData) {
    tLinkyCallback_context* context;
    int i;

    context = pData;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    pActor->render_style = BR_RSTYLE_DEFAULT;
    for (i = context->model_start; i < context->model_start + context->model_end; i++) {
        if (DRStricmp(gDroneStorage.models[i]->identifier, pActor->model->identifier) == 0) {
            pActor->model = gDroneStorage.models[i];
            return 0;
        }
    }
    PDFatalError("No drone model");
    return 0;
}

// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

// FUNCTION: CARMA2_HW 0x00450b30
// FUNCTION: CARMA2_SW 0x004508c0
br_actor* C2_HOOK_CDECL MrFindy(br_actor* pActor, const char* pName) {

    if (DRStricmp(pActor->identifier, pName) == 0) {
        return pActor;
    } else {
        return NULL;
    }
}

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

// FUNCTION: CARMA2_HW 0x0044ca90
// FUNCTION: CARMA2_SW 0x0044c840
// FUNCTION: CARMA2_CHEAT 0x0044cfe0
int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 15.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 12.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 15.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cae0
// FUNCTION: CARMA2_SW 0x0044c890
// FUNCTION: CARMA2_CHEAT 0x0044d030
int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 40.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 30.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 40.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cb30
// FUNCTION: CARMA2_SW 0x0044c8e0
// FUNCTION: CARMA2_CHEAT 0x0044d080
int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 50.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 20.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 50.f) {
        return 1;
    } else {
        return 0;
    }
}

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

// FUNCTION: CARMA2_HW 0x0044cb80
// FUNCTION: CARMA2_SW 0x0044c930
// FUNCTION: CARMA2_CHEAT 0x0044d0d0
int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 18.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 13.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 18.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cbd0
// FUNCTION: CARMA2_SW 0x0044c980
// FUNCTION: CARMA2_CHEAT 0x0044d120
int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cc20
// FUNCTION: CARMA2_SW 0x0044c9d0
// FUNCTION: CARMA2_CHEAT 0x0044d170
int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f) {
        return 1;
    } else {
        return 0;
    }
}

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// DroneStateFuncReset

// InitDroneDrivingInfo

// MoveThisDronePlane

// MoveThisDroneCar

// FUNCTION: CARMA2_HW 0x0044cfd0
// FUNCTION: CARMA2_SW 0x0044cd80
// FUNCTION: CARMA2_CHEAT 0x0044d520
void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone) {
    tCompressed_matrix34 compressed_mat;
    br_scalar radius;
    br_scalar speed;

    BrVector3Set(&compressed_mat.m[0],
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[0][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[1],
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[1][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[2],
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][0], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][1], -1.f, 1.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[2][2], -1.f, 1.f));
    BrVector3Set(&compressed_mat.m[3],
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][0], -1000.f, 1000.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][1], -1000.f, 1000.f),
        DRScalarToU16(pDrone->actor->t.t.mat.m[3][2], -1000.f, 1000.f));

    radius = MIN(pDrone->h_radius, 100.f);
    speed = pDrone->left_turn ? pDrone->field_0x48 : -pDrone->field_0x48;
    PipeSingleDroneCornerPos(pDrone, DRScalarToU16(speed, -500.f, 500.f), DRScalarToU16(pDrone->field_0x74, 0.f, 1.f), DRScalarToU16(radius, 0.f, 100.f), &compressed_mat);
}

// PossiblyPipeDroneMovement

// DroneStateFuncControlledMovement

// DoDroneHaltingStuff

// DroneStateFuncPhysicsActive

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

// FUNCTION: CARMA2_HW 0x0044ecf0
// FUNCTION: CARMA2_SW 0x0044ea90
// FUNCTION: CARMA2_CHEAT 0x0044f240
void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on) {

    gTraffic_disabled = !pTraffic_on;
}

// FUNCTION: CARMA2_HW 0x0044ed00
// FUNCTION: CARMA2_SW 0x0044eaa0
// FUNCTION: CARMA2_CHEAT 0x0044f250
int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void) {
    return !gTraffic_disabled;
}

// STUB: CARMA2_HW 0x0044ed10
// STUB: CARMA2_SW 0x0044eab0
// STUB: CARMA2_CHEAT 0x0044f260
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
    NOT_IMPLEMENTED();
}

// InitDrones

// FUNCTION: CARMA2_HW 0x0044fc10
// FUNCTION: CARMA2_SW 0x0044f9b0
// FUNCTION: CARMA2_CHEAT 0x00450160
void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    int i;

    if (gCount_drones == 0) {
        return;
    }

    for (i = 0; i < gCount_drones; i++) {

        if (gDrone_specs[i].actor->parent != NULL) {
            BrActorRemove(gDrone_specs[i].actor);
        }
        BrActorFree(gDrone_specs[i].actor);
        RemoveDroneFromPHIL(&gDrone_specs[i]);
        if (gDrone_specs[i].field_0x46) {
            FreeShape(gDrone_specs[i].collision_info.shape);
            gDrone_specs[i].field_0x46 = 0;
        }

        if (gDrone_specs[i].funk_grooves != NULL) {
            BrMemFree(gDrone_specs[i].funk_grooves);
            gDrone_specs[i].funk_grooves = NULL;
        }
    }

    for (i = 0; i < gCount_drone_forms; i++) {

        if (gDrone_forms[i].field_0x80 != NULL) {
            int j;

            BrMemFree(gDrone_forms[i].field_0x80);
            gDrone_forms[i].field_0x80 = NULL;
            for (j = 0; j < gDrone_forms[i].count_models; j++) {
                BrModelRemove(gDrone_forms[i].models[j]);
                gDrone_forms[i].models[j]->nfaces = 0;
                gDrone_forms[i].models[j]->faces = NULL;
                BrModelFree(gDrone_forms[i].models[j]);
            }
            if (gDrone_forms[i].models != NULL) {
                BrMemFree(gDrone_forms[i].models);
                gDrone_forms[i].models = NULL;
            }
        }
    }
    ClearOutStorageSpace(&gDroneStorage);
    BrMemFree(gDrone_specs);
    gDrone_specs = NULL;
    gCount_drones = 0;
    BrMemFree(gDrone_path_nodes);
    gDrone_path_nodes = NULL;
    gCount_drone_path_nodes = 0;
}

// STUB: CARMA2_HW 0x0044fda0
// STUB: CARMA2_SW 0x0044fb40
// STUB: CARMA2_CHEAT 0x004502f0
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {
    NOT_IMPLEMENTED();
}

// GetDroneCrushData

// AllocateAndInitDrones

// STUB: CARMA2_HW 0x00450bf0
// STUB: CARMA2_SW 0x00450980
// STUB: CARMA2_CHEAT 0x00451140
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004512d0
// FUNCTION: CARMA2_SW 0x00451060
void C2_HOOK_FASTCALL DronePreCollisionStuff(void) {
    gINT_00681fb0 = 0;
    gFrame++;
}

// DoDronePerGameFrameStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00451e70
// STUB: CARMA2_SW 0x00451c00
// STUB: CARMA2_CHEAT 0x004523c0
void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// MyDroneHathCollideth

// MyDroneHathHalteth

// MoveMyDroneBaby

// FUNCTION: CARMA2_HW 0x004525c0
// FUNCTION: CARMA2_SW 0x00452350
void C2_HOOK_FASTCALL DroneLookWhereYoureGoingYouBlindCunt(void) {
}

// FUNCTION: CARMA2_HW 0x004525d0
// FUNCTION: CARMA2_SW 0x00452360
void C2_HOOK_FASTCALL SelectNextDrone(void) {
    char s[64];

    gCurrent_selected_drone++;
    if (gCurrent_selected_drone >= gCount_drones) {
        gCurrent_selected_drone = 0;
    }
    sprintf(s, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, s);
}

// FUNCTION: CARMA2_HW 0x00452620
// FUNCTION: CARMA2_SW 0x004523b0
void C2_HOOK_FASTCALL SelectPreviousDrone(void) {
    char s[64];

    if (gCount_drones != 0) {
        gCurrent_selected_drone--;
        if (gCurrent_selected_drone < 0) {
            gCurrent_selected_drone = gCount_drones - 1;
        }
    }
    sprintf(s, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, s);
}

// FUNCTION: CARMA2_HW 0x00452680
// FUNCTION: CARMA2_SW 0x00452410
// FUNCTION: CARMA2_CHEAT 0x00452bd0
int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452690
// FUNCTION: CARMA2_SW 0x00452420
br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void) {
    return &gDrone_specs[gCurrent_selected_drone].actor->t.t.mat;
}

// GetCurrentViewDroneDirection

// ApplyDroneRender2

// FUNCTION: CARMA2_HW 0x00452880
// FUNCTION: CARMA2_SW 0x00452610
// FUNCTION: CARMA2_CHEAT 0x00452de0
void C2_HOOK_FASTCALL ApplyDroneStraightPos2(int pDrone_id, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance) {
    br_vector3 direction;
    tDrone_spec* drone;

    drone = &gDrone_specs[pDrone_id];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.0f, 500.0f);
    drone->field_0xe0 = 0.0f;
    BrVector3Scale(&direction, &pSection->field_0x10, pSection->field_0x1c - DRU16ToScalar(pDistance, 0.0f, 255.0f));
    BrVector3Negate(&direction, &direction);
    BrVector3Add(&drone->actor->t.t.translate.t, &gDrone_path_nodes[pSection->node1].position, &direction);
    BrVector3Copy(&direction, &pSection->field_0x10);
    if (drone->form->flags & 0x10) {
        direction.v[1] = 0.0f;
    }
    PointActorAlongThisBloodyVector(drone->actor, &direction);
    DoDroneFunkyGroovyThings(drone);
}

// FUNCTION: CARMA2_HW 0x004529d0
// FUNCTION: CARMA2_SW 0x00452760
// FUNCTION: CARMA2_CHEAT 0x00452f30
void C2_HOOK_FASTCALL ApplyDroneCornerPos2(int pDrone_id, tU16 pSpeed, tU16 pField_0x74, tU16 pRadius, const tCompressed_matrix34* pMatrix) {
    tDrone_spec* drone;

    drone = &gDrone_specs[pDrone_id];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.0f, 500.0f);
    drone->field_0xe0 = DRU16ToScalar(pField_0x74, 0.0f, 1.0f);
    drone->field_0xe8 = DRU16ToScalar(pRadius, 0.0f, 100.0f);
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[0],
        DRU16ToScalar(pMatrix->m[0].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[0].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[0].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[1],
        DRU16ToScalar(pMatrix->m[1].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[1].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[1].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[2],
        DRU16ToScalar(pMatrix->m[2].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[2].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[2].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[3],
        DRU16ToScalar(pMatrix->m[3].v[0], -1000.0f, 1000.0f),
        DRU16ToScalar(pMatrix->m[3].v[1], -1000.0f, 1000.0f),
        DRU16ToScalar(pMatrix->m[3].v[2], -1000.0f, 1000.0f));
    DoDroneFunkyGroovyThings(drone);
}

// FUNCTION: CARMA2_HW 0x00452f40
// FUNCTION: CARMA2_SW 0x00452cd0
// FUNCTION: CARMA2_CHEAT 0x004534a0
void C2_HOOK_FASTCALL ReallocModelFacesAndVertices(br_model* pModel, int pCount_faces, int pCount_vertices) {
    int i;
    br_vertex* vertices;
    br_face* faces;

    vertices = BrResAllocate(pModel, pCount_vertices * sizeof(br_vertex), BR_MEMORY_VERTICES);
    BrBlockFill(vertices, 0, pCount_vertices * sizeof(br_vertex) / sizeof(br_uint_32));
    if (pModel->nvertices != 0) {
        for (i = 0; i < (pModel->nvertices < pCount_vertices ? pModel->nvertices : pCount_vertices); i++) {
            vertices[i] = pModel->vertices[i];
        }
        BrResRemove(pModel->vertices);
        BrResFree(pModel->vertices);
    }
    pModel->nvertices = pCount_vertices;
    pModel->vertices = vertices;
    faces = BrResAllocate(pModel, pCount_faces * sizeof(br_face), BR_MEMORY_FACES);
    BrBlockFill(faces, 0, pCount_faces * sizeof(br_face) / sizeof(br_uint_32));
    if (pModel->nfaces != 0) {
        for (i = 0; i < (pModel->nfaces < pCount_faces ? pModel->nfaces : pCount_faces); i++) {
            faces[i] = pModel->faces[i];
        }
        BrResRemove(pModel->faces);
        BrResFree(pModel->faces);
    }
    pModel->nfaces = pCount_faces;
    pModel->faces = faces;
}

inline void C2_HOOK_FASTCALL CalcNFacesAndVerticesInEditModel(int* pCount_faces, int* pCount_vertices) {
    int i;
    int j;

    *pCount_faces = gCount_drone_path_nodes * 12;
    *pCount_vertices = gCount_drone_path_nodes * 8;
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        for (j = 0; j < gDrone_path_nodes[i].count_sections; j++) {
            *pCount_faces += 2;
            *pCount_vertices += 4;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00452ba0
// FUNCTION: CARMA2_SW 0x00452930
// FUNCTION: CARMA2_CHEAT 0x00453100
int C2_HOOK_FASTCALL InitialiseEditModelsEtc(void) {
    int count_faces;
    int count_vertices;
    int i;
    int j;
    tDrone_path_node* node;

    if (gStatic_drone_path_model_actor == NULL) {
        gStatic_drone_path_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gNon_track_actor, gStatic_drone_path_model_actor);
    }
    if (gDrone_path_model_actor == NULL) {
        gDrone_path_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        BrActorAdd(gNon_track_actor, gDrone_path_model_actor);
    }
    if (gStatic_drone_path_model == NULL) {
        gStatic_drone_path_model = BrModelAllocate("StaticDronePathModel", 3, 1);
        gStatic_drone_path_model->flags |= BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL | BR_MODF_DONT_WELD;
        BrModelAdd(gStatic_drone_path_model);
    }
    if (gElasticatey_drone_path_model == NULL) {
        gElasticatey_drone_path_model = BrModelAllocate("ElasticateyDronePathModel", 3, 1);
        gElasticatey_drone_path_model->flags |= BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL | BR_MODF_DONT_WELD;
        BrModelAdd(gElasticatey_drone_path_model);
    }
    if (gStatic_drone_path_model_actor == NULL || gDrone_path_model_actor == NULL || gStatic_drone_path_model == NULL || gElasticatey_drone_path_model == NULL) {
        return 0;
    }
    gStatic_drone_path_model_actor->model = gStatic_drone_path_model;
    gStatic_drone_path_model_actor->type = BR_ACTOR_MODEL;
    gStatic_drone_path_model_actor->render_style = gCount_drone_path_nodes != 0 ? BR_RSTYLE_FACES : BR_RSTYLE_NONE;
    gDrone_path_model_actor->model = gElasticatey_drone_path_model;
    gDrone_path_model_actor->type = BR_ACTOR_MODEL;
    gDrone_path_model_actor->render_style = gDrone_paths_elasticating ? BR_RSTYLE_FACES : BR_RSTYLE_NONE;
    ReallocModelFacesAndVertices(gElasticatey_drone_path_model, 2, 4);
    CalcNFacesAndVerticesInEditModel(&count_faces, &count_vertices);
    if (count_vertices > gStatic_drone_path_model_max_vertices || count_faces > gStatic_drone_path_model_max_faces) {
        ReallocModelFacesAndVertices(gStatic_drone_path_model, count_faces, count_vertices);
        gStatic_drone_path_model_max_vertices = count_vertices;
        gStatic_drone_path_model_max_faces = count_faces;
    } else {
        gStatic_drone_path_model->nvertices = count_vertices;
        gStatic_drone_path_model->nfaces = count_faces;
    }
    count_faces = 0;
    count_vertices = 0;
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        if (i == gSelected_edit_drone_path) {
            MakeAIEditCubeHere(gStatic_drone_path_model, count_vertices, count_faces, &gDrone_path_nodes[i].position, gMat_dk_turq, gMat_lt_turq, gMat_lt_turq);
        } else if (gDrone_path_nodes[i].field_0x12e & 0x1) {
            MakeAIEditCubeHere(gStatic_drone_path_model, count_vertices, count_faces, &gDrone_path_nodes[i].position, gMat_dk_yel, gMat_md_yel, gMat_lt_yel);
        } else {
            MakeAIEditCubeHere(gStatic_drone_path_model, count_vertices, count_faces, &gDrone_path_nodes[i].position, gMat_dk_grn, gMat_md_grn, gMat_lt_grn);
        }
        count_faces += 12;
        count_vertices += 8;
    }
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        node = &gDrone_path_nodes[i];
        for (j = 0; j < ((node->field_0x12e & 0x1) ? 1 : node->count_sections); j++) {
            if (node->field_0x12e & 0x1) {
                if (node->sections[j].node1 >= 0) {
                    MakeAISimpleEditSectionHere(gStatic_drone_path_model, count_vertices, count_faces, &node->position, &gDrone_path_nodes[node->sections[j].node1].position, gMat_dk_yel, gMat_lt_yel);
                }
            } else {
                MakeAISimpleEditSectionHere(gStatic_drone_path_model, count_vertices, count_faces, &node->position, &gDrone_path_nodes[node->sections[j].node1].position, gMat_dk_grn, gMat_lt_grn);
            }
            count_faces += 2;
            count_vertices += 4;
        }
    }
    if (count_vertices != 0) {
        BrModelUpdate(gStatic_drone_path_model, BR_MODU_ALL);
    } else {
        gStatic_drone_path_model_actor->render_style = BR_RSTYLE_NONE;
    }
    WriteOutDronePaths();
    return 1;
}

// UpdateEditModels

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// STUB: CARMA2_HW 0x00453050
// STUB: CARMA2_SW 0x00452de0
// STUB: CARMA2_CHEAT 0x004535b0
void C2_HOOK_FASTCALL WriteOutDronePaths(void) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004532e0
// FUNCTION: CARMA2_SW 0x00453070
// FUNCTION: CARMA2_CHEAT 0x00453840
void C2_HOOK_FASTCALL SelectClosestDrone(void) {
    int i;
    int nearest;
    br_scalar nearest_distance_squared;
    br_scalar distance_squared;
    br_vector3 v;

    nearest_distance_squared = BR_SCALAR_MAX;
    for (i = 0; i < gCount_drones; i++) {
        BrVector3Sub(&v, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &gDrone_specs[i].actor->t.t.translate.t);
        distance_squared = BrVector3LengthSquared(&v);
        if (distance_squared < nearest_distance_squared) {
            nearest_distance_squared = distance_squared;
            nearest = i;
        }
    }
    if (nearest_distance_squared < 2.0f) {
        if (gSelected_drone_path_index >= 0) {
            gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_FACES;
        }
        gSelected_drone_path_index = nearest;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-extra-args"
#endif
        sprintf(gDrone_edit_message, "Selected drone #%d", nearest, nearest_distance_squared);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
        NewTextHeadupSlot(4, 0, 3000, -1, gDrone_edit_message);
        for (i = 0; i < gCount_drone_forms; i++) {
            if (gDrone_specs[gSelected_drone_path_index].form == &gDrone_forms[i]) {
                gSelected_drone_form_index = i;
                return;
            }
        }
    } else {
        NewTextHeadupSlot(4, 0, 3000, -1, "No drones close enough");
    }
}

// FUN_00453460

// FUNCTION: CARMA2_HW 0x00453680
// FUNCTION: CARMA2_SW 0x00453410
// FUNCTION: CARMA2_CHEAT 0x00453be0
void C2_HOOK_FASTCALL ToggleDronePaths(void) {

    if (gSelected_drone_path_index >= 0) {
        gDrone_specs[gSelected_drone_path_index].actor->render_style = BR_RSTYLE_FACES;
        gSelected_drone_path_index = -1;
    }
    if (gShow_drone_paths) {
        if (gStatic_drone_path_model_actor != NULL) {
            gStatic_drone_path_model_actor->render_style = BR_RSTYLE_NONE;
        }
        if (gDrone_path_model_actor != NULL) {
            gDrone_path_model_actor->render_style = BR_RSTYLE_NONE;
        }
        WriteOutDronePaths();
        gShow_drone_paths = 0;
        NewTextHeadupSlot(4, 0, 3000, -1, "Hiding Drone Paths");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -1, "Drone Paths Already Hidden");
    }
}

// FUNCTION: CARMA2_HW 0x00453720
// FUNCTION: CARMA2_SW 0x004534b0
// FUNCTION: CARMA2_CHEAT 0x00453c80
void C2_HOOK_FASTCALL FUN_00453720(void) {
    int node;
    int selected;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
    } else if (FUN_004538a0(1)) {
        node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
        if (node >= 0 && node != gSelected_drone_path_node_index) {
            selected = gSelected_drone_path_node_index;
            if (gDrone_path_nodes[selected].count_sections < 8) {
                gDrone_path_nodes[selected].sections[gDrone_path_nodes[selected].count_sections].node1 = node;
                gDrone_path_nodes[selected].count_sections++;
            }
            InitialiseEditModelsEtc();
            gDrone_paths_elasticating = 0;
            if (gDrone_path_model_actor != NULL) {
                gDrone_path_model_actor->render_style = BR_RSTYLE_NONE;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004537d0
// FUNCTION: CARMA2_SW 0x00453560
// FUNCTION: CARMA2_CHEAT 0x00453d30
int C2_HOOK_FASTCALL FindNearestNodeDrone(const br_vector3* pPosition, br_scalar pMax_distance) {
    int i;
    int nearest;
    br_scalar max_distance_squared;
    br_scalar nearest_distance_squared;
    br_scalar distance_squared;
    br_vector3 v;

    nearest_distance_squared = BR_SCALAR_MAX;
    max_distance_squared = pMax_distance * pMax_distance;
    nearest = -1;
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        BrVector3Sub(&v, pPosition, &gDrone_path_nodes[i].position);
        distance_squared = BrVector3LengthSquared(&v);
        if (distance_squared < nearest_distance_squared && distance_squared < max_distance_squared) {
            nearest_distance_squared = distance_squared;
            nearest = i;
        }
    }
    if (nearest < 0) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Get nearer to a drone node");
    }
    return nearest;
}

// FUNCTION: CARMA2_HW 0x004538a0
// FUNCTION: CARMA2_SW 0x00453630
// FUNCTION: CARMA2_CHEAT 0x00453e00
int C2_HOOK_FASTCALL FUN_004538a0(int pReport) {
    br_vector3 v;

    if (gDrone_paths_elasticating) {
        BrVector3Sub(&v, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &gDrone_path_nodes[gSelected_drone_path_node_index].position);
        if (pReport && BrVector3Length(&v) > 255.0f) {
            NewTextHeadupSlot(4, 0, 3000, -1, "New section too long");
            return 0;
        }
        return 1;
    }
    if (pReport) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must be elasticating to do that");
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00453940
// FUNCTION: CARMA2_SW 0x004536d0
// FUNCTION: CARMA2_CHEAT 0x00453ea0
void C2_HOOK_FASTCALL FUN_00453940(void) {
    int i;
    int node;
    tDrone_path_node* path_node;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
    } else if (gDrone_paths_elasticating) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Stop elasticating first");
    } else if (FUN_00453ab0()) {
        path_node = &gDrone_path_nodes[gCount_drone_path_nodes];
        node = gCount_drone_path_nodes++;
        BrVector3Copy(&path_node->position, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
        for (i = 0; i < REC2_ASIZE(path_node->sections); i++) {
            path_node->sections[i].node1 = -1;
        }
        path_node->field_0x12e = 0;
        path_node->count_sections = 0;
        path_node->type = -1;
        InitialiseEditModelsEtc();
        if (gDrone_path_model_actor != NULL && gElasticatey_drone_path_model != NULL) {
            gDrone_paths_elasticating = 1;
            gDrone_path_model_actor->render_style = BR_RSTYLE_FACES;
            gSelected_drone_path_node_index = node;
            MakeAISimpleEditSectionHere(gElasticatey_drone_path_model, 0, 0,
                &gDrone_path_nodes[node].position,
                &gProgram_state.current_car.car_master_actor->t.t.translate.t,
                gMat_dk_grn, gMat_lt_grn);
            BrModelUpdate(gElasticatey_drone_path_model, BR_MODU_ALL);
        }
        sprintf(gDrone_edit_message, "Shat out drone path node #%d", node);
        NewTextHeadupSlot(4, 0, 2000, -1, gDrone_edit_message);
    }
}

// FUNCTION: CARMA2_HW 0x00453ab0
// FUNCTION: CARMA2_SW 0x00453840
// FUNCTION: CARMA2_CHEAT 0x00454010
int C2_HOOK_FASTCALL FUN_00453ab0(void) {
    int i;
    br_vector3 pos;
    br_vector3 v;

    if (gCount_drone_path_nodes >= 200) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Out of spare nodes. Sorry.");
        return 0;
    }
    BrVector3Copy(&pos, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        BrVector3Sub(&v, &pos, &gDrone_path_nodes[i].position);
        if (BrVector3LengthSquared(&v) < 0.5f) {
            NewTextHeadupSlot(4, 0, 3000, -1, "Too close. Move a bit.");
            return 0;
        }
    }
    return 1;
}

// FUN_00453b80

// FUNCTION: CARMA2_HW 0x00453d60
// FUNCTION: CARMA2_SW 0x00453af0
// FUNCTION: CARMA2_CHEAT 0x004542c0
void C2_HOOK_FASTCALL FUN_00453d60(void) {

    gDrone_paths_elasticating = 0;
    if (gDrone_path_model_actor != NULL) {
        gDrone_path_model_actor->render_style = BR_RSTYLE_NONE;
    }
}

// FUN_00453d80

// FUNCTION: CARMA2_HW 0x00453ed0
// FUNCTION: CARMA2_SW 0x00453c60
// FUNCTION: CARMA2_CHEAT 0x00454430
void C2_HOOK_FASTCALL FUN_00453ed0(void) {
}

// FUNCTION: CARMA2_HW 0x00453ee0
// FUNCTION: CARMA2_SW 0x00453c70
// FUNCTION: CARMA2_CHEAT 0x00454440
void C2_HOOK_FASTCALL FUN_00453ee0(void) {
}

// FUNCTION: CARMA2_HW 0x00453ef0
// FUNCTION: CARMA2_SW 0x00453c80
// FUNCTION: CARMA2_CHEAT 0x00454450
void C2_HOOK_FASTCALL FUN_00453ef0(void) {
}

// FUNCTION: CARMA2_HW 0x00453f00
// FUNCTION: CARMA2_SW 0x00453c90
// FUNCTION: CARMA2_CHEAT 0x00454460
void C2_HOOK_FASTCALL FUN_00453f00(void) {
}

// FUNCTION: CARMA2_HW 0x00453f10
// FUNCTION: CARMA2_SW 0x00453ca0
// FUNCTION: CARMA2_CHEAT 0x00454470
void C2_HOOK_FASTCALL FUN_00453f10(void) {
}

// FUNCTION: CARMA2_HW 0x00453f20
// FUNCTION: CARMA2_SW 0x00453cb0
// FUNCTION: CARMA2_CHEAT 0x00454480
void C2_HOOK_FASTCALL FUN_00453f20(void) {
}

// FUNCTION: CARMA2_HW 0x00453f30
// FUNCTION: CARMA2_SW 0x00453cc0
// FUNCTION: CARMA2_CHEAT 0x00454490
void C2_HOOK_FASTCALL FUN_00453f30(void) {
}

// FUN_00453f40

// FUN_00454360

// FUN_004543d0

// FUN_00454470

// FUNCTION: CARMA2_HW 0x00454640
// FUNCTION: CARMA2_SW 0x004543d0
// FUNCTION: CARMA2_CHEAT 0x00454ba0
void C2_HOOK_FASTCALL FUN_00454640(void) {

    WriteOutDronePaths();
}

// FUNCTION: CARMA2_HW 0x00454650
// FUNCTION: CARMA2_SW 0x004543e0
// FUNCTION: CARMA2_CHEAT 0x00454bb0
void C2_HOOK_FASTCALL FUN_00454650(void) {

    WriteOutDronePaths();
}

// FUNCTION: CARMA2_HW 0x00454660
// FUNCTION: CARMA2_SW 0x004543f0
// FUNCTION: CARMA2_CHEAT 0x00454bc0
void C2_HOOK_FASTCALL FUN_00454660(void) {

    WriteOutDronePaths();
}

// FUNCTION: CARMA2_HW 0x00454670
// FUNCTION: CARMA2_SW 0x00454400
// FUNCTION: CARMA2_CHEAT 0x00454bd0
void C2_HOOK_FASTCALL FUN_00454670(void) {

    WriteOutDronePaths();
}

// FUNCTION: CARMA2_HW 0x00454680
// FUNCTION: CARMA2_SW 0x00454410
// FUNCTION: CARMA2_CHEAT 0x00454be0
void C2_HOOK_FASTCALL FUN_00454680(void) {

    WriteOutDronePaths();
}

// FUN_00454690

// FUN_00454710

// DeleteNodeSectionsAndOrphan

// DeleteNodeAndSections

// FUN_00454bb0

// SelectNextDroneForm

// SelectPreviousDroneForm

// CreateNewDrone


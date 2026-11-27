/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "57-drone.h"

#include "00-car.h"

#include "08-loading1.h"
#include "15-displays.h"
#include "23-piping.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "34-finteray.h"
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

// GLOBAL: CARMA2_HW 0x006820b8
// GLOBAL: CARMA2_SW 0x0067c7f8
// GLOBAL: CARMA2_CHEAT 0x00689960
int gDrones_unmodified;

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
// GLOBAL: CARMA2_CHEAT 0x00689968
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x006820d0
// GLOBAL: CARMA2_SW 0x0067c810
// GLOBAL: CARMA2_CHEAT 0x00689978
int gCount_drones;

// GLOBAL: CARMA2_HW 0x006820d4
// GLOBAL: CARMA2_SW 0x0067c814
// GLOBAL: CARMA2_CHEAT 0x0068997c
int gINT_006820d4;

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
// GLOBAL: CARMA2_CHEAT 0x0068bda0
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x006844fc
// GLOBAL: CARMA2_SW 0x0067ec3c
// GLOBAL: CARMA2_CHEAT 0x0068bda4
int gINT_006844fc;

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

// GLOBAL: CARMA2_HW 0x00684518
// GLOBAL: CARMA2_SW 0x0067ec58
// GLOBAL: CARMA2_CHEAT 0x0068bdc0
int gTraffic_disabled;

extern float gDrone_delta_time;

// GLOBAL: CARMA2_HW 0x00681fb0
// GLOBAL: CARMA2_SW 0x0067c6f0
// GLOBAL: CARMA2_CHEAT 0x00689858
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00594738
// GLOBAL: CARMA2_SW 0x0058f698
// GLOBAL: CARMA2_CHEAT 0x0059b090
tDrone_state_function* gDrone_state_functions[6] = {
    NULL,
    DroneStateFuncReset,
    DroneStateFuncControlledMovement,
    DroneStateFuncPhysicsActive,
    DroneStateFuncStationaryPassive,
    NULL,
};

// GLOBAL: CARMA2_HW 0x00681fb4
// GLOBAL: CARMA2_SW 0x0067c6f4
// GLOBAL: CARMA2_CHEAT 0x0068985c
int gCount_active_drones;

// GLOBAL: CARMA2_HW 0x00684500
// GLOBAL: CARMA2_SW 0x0067ec40
// GLOBAL: CARMA2_CHEAT 0x0068bda8
int gCount_rendered_drones;

// GLOBAL: CARMA2_HW 0x0068452c
// GLOBAL: CARMA2_SW 0x0067ec6c
// GLOBAL: CARMA2_CHEAT 0x0068bdd4
int gShow_drone_paths;

// GLOBAL: CARMA2_HW 0x00684514
// GLOBAL: CARMA2_SW 0x0067ec54
// GLOBAL: CARMA2_CHEAT 0x0068bdbc
int gFrame;

// FUNCTION: CARMA2_HW 0x0044cfc0
// FUNCTION: CARMA2_SW 0x0044cd70
// FUNCTION: CARMA2_CHEAT 0x0044d510
void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...) {

}

// FUNCTION: CARMA2_HW 0x00451620
// FUNCTION: CARMA2_SW 0x004513b0
// FUNCTION: CARMA2_CHEAT 0x00451b70
inline void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pName, br_vector3* pVector) {
    DoNotDprintf_drone("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pName, pVector->v[0], pVector->v[1], pVector->v[2]);
}

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

// FUNCTION: CARMA2_HW 0x0044f980
// FUNCTION: CARMA2_SW 0x0044f720
// FUNCTION: CARMA2_CHEAT 0x0044fed0
inline void C2_HOOK_FASTCALL InitDroneCollisionObject(tDrone_spec* pDrone) {
    br_bounds3 bnds;
    br_vector3 size;
    tPhysics_shape* original_shape;
    tPhysics_object* object;

    object = &pDrone->collision_info;
    BrActorToBounds(&bnds, pDrone->actor);
    bnds.min.v[1] -= -.001f;
    if (pDrone->field_0x46) {
        original_shape = object->shape;
    }
    memset(object, 0, sizeof(*object));
    if (pDrone->field_0x46) {
        object->shape = original_shape;
    } else {
        object->shape = (tPhysics_shape*)AllocateShapeBox(kMem_drone_collision_stuff);
        object->shape->box.common.bb = bnds;
        object->shape->box.common.field_0x1c = bnds;
        object->shape->box.common.next = NULL;
        pDrone->field_0x46 = 1;
    }
    object->flags_0x238 = 0x10;
    object->owner = pDrone;
    object->actor = pDrone->actor;
    object->M = pDrone->form->M;
    BrVector3Set(&object->cmpos, 0.0f, .09f, 0.0f);
    BrVector3Set(&object->field_0x54, 0.0f, -.4f / WORLD_SCALE, 0.0f);
    BrMatrix34Copy(&object->transform_matrix, &pDrone->actor->t.t.mat);
    object->box_face_ref = gFace_num__car - 2;
    object->field_0x1a0 = 0x100000;
    FillInShape(object->shape);
    UpdateCollisionObject(object);
    GetNewBoundingBox(&object->field_0xf4, &object->bb1, &object->transform_matrix);
    BrVector3Sub(&size, &bnds.max, &bnds.min);
    object->I.v[0] = (size.v[2] * size.v[2] + size.v[1] * size.v[1]) * (object->M / 12.0f);
    object->I.v[1] = (size.v[2] * size.v[2] + size.v[0] * size.v[0]) * (object->M / 12.0f);
    object->I.v[2] = (size.v[1] * size.v[1] + size.v[0] * size.v[0]) * (object->M / 12.0f);
    object->cmpos.v[0] = bnds.min.v[0] - pDrone->form->center.v[1] * size.v[0] * -0.01;
    object->cmpos.v[1] = bnds.min.v[1] - pDrone->form->center.v[2] * size.v[1] * -0.01;
    object->cmpos.v[2] = bnds.min.v[2] - pDrone->form->center.v[0] * size.v[2] * -0.01;
    if (pDrone->form->type == kDroneType_train) {
        object->world_friction = -.75f;
    }
    object->disable_move_rotate = 1;
    object->field_0xed = 1;
    object->flags |= 0x40;
    object->drivable_on = (pDrone->form->flags >> 3) & 0x1;
    BrVector3SetFloat(&object->v, 0.0f, 0.0f, 0.0f);
    BrVector3SetFloat(&object->omega, 0.0f, 0.0f, 0.0f);
    BrVector3SetFloat(&object->rotate_omega, 0.0f, 0.0f, 0.0f);
    BrVector3SetFloat(&object->velocity_car_space, 0.0f, 0.0f, 0.0f);
}

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

inline int C2_HOOK_FASTCALL AddDroneToPHIL(tDrone_spec* pDrone) {

    return ReallyAddDroneToPHIL(pDrone);
}

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

inline void C2_HOOK_FASTCALL PauseDroneState(tDrone_spec* pDrone) {
    if (gDrone_state_functions[pDrone->current_state] != NULL) {
        gDrone_state_functions[pDrone->current_state](pDrone, 2);
    }
}

inline void C2_HOOK_FASTCALL UnPauseDroneState(tDrone_spec* pDrone) {

    if (gDrone_state_functions[pDrone->current_state] != NULL) {
        gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_DEFAULT);
    }
}

inline void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone, int pNew_state) {
    int old_state;

    DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, pNew_state);
    if (pDrone->current_state != pNew_state) {
        if (gDrone_state_functions[pDrone->current_state] != NULL) {
            gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
        }
        old_state = pDrone->current_state;
        pDrone->time_last_munge = gTime_stamp_for_this_munging;
        pDrone->prev_states[1] = pDrone->prev_states[0];
        pDrone->prev_states[0] = old_state;
        pDrone->current_state = pNew_state;
        if (gDrone_state_functions[pNew_state] != NULL) {
            gDrone_state_functions[pNew_state](pDrone, eDrone_state_START);
        }
    }
}

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

inline int C2_HOOK_FASTCALL DroneOrientationChanged(tDrone_spec* pDrone, float pDot) {
    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[0], (br_vector3*)pDrone->actor->t.t.mat.m[0]) < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[1], (br_vector3*)pDrone->actor->t.t.mat.m[1]) < pDot) {
        return 1;
    }
    if (BrVector3Dot((br_vector3*)pDrone->collision_info.transform_matrix.m[2], (br_vector3*)pDrone->actor->t.t.mat.m[2]) < pDot) {
        return 1;
    }
    return 0;
}

inline int C2_HOOK_FASTCALL DroneyObjectInSensiblePlace(tPhysics_object* pObject, tPhysics_object* pList) {
    tPhysics_object* object;

    for (object = pList; object != NULL; object = object->next) {
        if (pObject != object && TestObjectOverlap(pObject, object)) {
            return 0;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004516d0
// FUNCTION: CARMA2_SW 0x00451460
// FUNCTION: CARMA2_CHEAT 0x00451c20
inline void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone) {
    if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
        DoNotDprintf_drone("STOP RENDERING: %d", pDrone->id);
        gCount_rendered_drones--;
        pDrone->actor->render_style = BR_RSTYLE_NONE;
        PipeSingleDroneRender(pDrone, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00451710
// FUNCTION: CARMA2_SW 0x004514a0
// FUNCTION: CARMA2_CHEAT 0x00451c60
void C2_HOOK_FASTCALL StopProcessingThisDrone(tDrone_spec* pDrone, int pForce) {

    if (pDrone->field_0x44) {
        if (pForce || (!(pDrone->form->flags & 0x4) && pDrone->current_state != 3 && gCount_active_drones > 0)) {
            pDrone->field_0x44 = 0;
            gCount_active_drones--;
            StopRenderingThisDrone(pDrone);
            RemoveDroneFromPHIL(pDrone);
            PauseDroneState(pDrone);
            DoNotDprintf_drone("PROCESSING OFF: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("     Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00451810
// FUNCTION: CARMA2_SW 0x004515a0
// FUNCTION: CARMA2_CHEAT 0x00451d60
void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone) {
    tU8 render_style;

    render_style = pDrone->actor->render_style;
    if (render_style != BR_RSTYLE_FACES && (!gShow_drone_paths || render_style != BR_RSTYLE_BOUNDING_FACES)) {
        if (!gShow_drone_paths && (!pDrone->field_0x44 || gCount_rendered_drones >= 10)) {
            NewDroneState(pDrone, 1);
        } else {
            DoNotDprintf_drone("START RENDERING: %d", pDrone->id);
            gCount_rendered_drones++;
            pDrone->actor->render_style = BR_RSTYLE_FACES;
            PipeSingleDroneRender(pDrone, 1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004518e0
// FUNCTION: CARMA2_SW 0x00451670
// FUNCTION: CARMA2_CHEAT 0x00451e30
int C2_HOOK_FASTCALL CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(tDrone_spec* pDrone) {

    if (gTime_stamp_for_this_munging > pDrone->last_collide_check + 1000) {
        pDrone->last_collide_check = gTime_stamp_for_this_munging;
        InitDroneCollisionObject(pDrone);
        return DroneyObjectInSensiblePlace(&pDrone->collision_info, gPHIL_list_collision_infos);
    } else {
        return 0;
    }
}

inline void C2_HOOK_FASTCALL StartProcessingThisDrone(tDrone_spec* pDrone) {

    if (!gShow_drone_paths && pDrone->field_0x44 == 0 && ((pDrone->form->flags & 0x4) || gCount_active_drones < 12)) {
        if (CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing(pDrone) && AddDroneToPHIL(pDrone)) {
            BrMatrix34Copy(&pDrone->collision_info.transform_matrix, &pDrone->actor->t.t.mat);
            pDrone->field_0x44 = 1;
            if (!(pDrone->form->flags & 0x4)) {
                gCount_active_drones++;
            }
            UnPauseDroneState(pDrone);
            DoNotDprintf_drone("PROCESSING ON: Frame %d, Drone %d, state %d", gFrame, pDrone->id, pDrone->current_state);
            CrappyLittleVector3DPrintf("    Pos", &pDrone->actor->t.t.translate.t);
        }
    }
}

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

// FUNCTION: CARMA2_HW 0x00451bd0
// FUNCTION: CARMA2_SW 0x00451960
// FUNCTION: CARMA2_CHEAT 0x00452120
void C2_HOOK_FASTCALL CalcRenderBoundsCentre(void) {
    br_vector3 direction;

    BrVector3Negate(&direction, (br_vector3*)gCamera_to_world.m[2]);
    BrVector3Normalise(&direction, &direction);
    BrVector3Scale(&direction, &direction, 5.0f);
    BrVector3Add(&gRender_bounds_centre, &direction, (br_vector3*)gCamera_to_world.m[3]);
}

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// FUNCTION: CARMA2_HW 0x00451ca0
// FUNCTION: CARMA2_SW 0x00451a30
// FUNCTION: CARMA2_CHEAT 0x004521f0
void C2_HOOK_FASTCALL ProcessThisDrone(int pIndex) {
    tDrone_spec* drone;
    br_scalar length;

    drone = &gDrone_specs[pIndex];
    BrVector3Copy(&drone->pos, &drone->actor->t.t.translate.t);
    if (gDrone_state_functions[drone->current_state] != NULL) {
        gDrone_state_functions[drone->current_state](drone, eDrone_state_RUN);
    }
    if (drone->field_0x45) {
        DoDroneFunkyGroovyThings(drone);
    }
    BrVector3Sub(&drone->field_0x18, &drone->actor->t.t.translate.t, &drone->pos);
    if ((drone->field_0x44 && DroneOrientationChanged(drone, .92f)) || (length = BrVector3Length(&drone->field_0x18)) > 1.) {
        BrMatrix34Copy(&drone->collision_info.transform_matrix, &drone->actor->t.t.mat);
        DoNotDprintf_drone("REASSERTING OBJECT MATRIX: v length %f, Frame %d, Drone %d, state %d: ",
            length, gFrame, drone->id, drone->current_state);
    }
    drone->field_0x18.v[0] /= gDrone_delta_time;
    drone->field_0x18.v[1] /= gDrone_delta_time;
    drone->field_0x18.v[2] /= gDrone_delta_time;
}

// STUB: CARMA2_HW 0x0044cc70
// STUB: CARMA2_SW 0x0044ca20
// STUB: CARMA2_CHEAT 0x0044d1c0
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState pState) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x0044d1d0
// STUB: CARMA2_SW 0x0044cf80
// STUB: CARMA2_CHEAT 0x0044d720
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState pState) {
    NOT_IMPLEMENTED();
}

// DoDroneHaltingStuff

// FUNCTION: CARMA2_HW 0x0044eb70
// FUNCTION: CARMA2_SW 0x0044e910
// FUNCTION: CARMA2_CHEAT 0x0044f0c0
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState pState) {

    switch (pState) {
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncPhysicsActive() RUN");
        if ((!pDrone->field_0x44 || ((pDrone->form->flags & 0x4) && !pDrone->field_0x45)) && (pDrone->form->flags & 0x2)) {
            RemoveDroneFromPHIL(pDrone);
            pDrone->collision_info.field_0x1a0 = 0x100000;
            PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
            pDrone->field_0x5d4 = 0;
            NewDroneState(pDrone, 1);
        }
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x0044ec50
// FUNCTION: CARMA2_SW 0x0044e9f0
// FUNCTION: CARMA2_CHEAT 0x0044f1a0
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState pState) {

    switch (pState) {
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncStationaryPassive() RUN");
        if ((!pDrone->field_0x44 || ((pDrone->form->flags & 0x4) && !pDrone->field_0x45)) && (pDrone->form->flags & 0x2)) {
            NewDroneState(pDrone, 1);
        }
        break;
    default:
        break;
    }
}

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

// FUNCTION: CARMA2_HW 0x0044f700
// FUNCTION: CARMA2_SW 0x0044f4a0
// FUNCTION: CARMA2_CHEAT 0x0044fc50
void C2_HOOK_FASTCALL InitDrones(void) {
    int i;

    gDrones_unmodified = 1;
    gCount_rendered_drones = 0;
    gCurrent_selected_drone = 0;
    gINT_006820d4 = 0;
    gINT_006844fc = 0;
    gINT_00681fb0 = 0;
    for (i = 0; i < gCount_drones; i++) {
        NewDroneState(&gDrone_specs[i], 1);
        if (gDrone_specs[i].form->type == kDroneType_plane) {
            StartProcessingThisDrone(&gDrone_specs[i]);
        }
    }
}

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

// STUB: CARMA2_HW 0x004512f0
// STUB: CARMA2_SW 0x00451080
// STUB: CARMA2_CHEAT 0x00451840
void C2_HOOK_FASTCALL DoDronePerGameFrameStuff(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00451e70
// FUNCTION: CARMA2_SW 0x00451c00
// FUNCTION: CARMA2_CHEAT 0x004523c0
void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone) {
    tFunk_groove* groove;
    float position;
    float speed;
    float f;
    int left_turn;
    int i;
    float delta_time;
    float angle;

    delta_time = gAction_replay_mode ? gFrame_period / 1000. : gDrone_delta_time;
    position = gAction_replay_mode ? pDrone->field_0xe0 : pDrone->field_0x74;
    speed = gAction_replay_mode ? pDrone->field_0xe4 : pDrone->field_0x48;
    if (gAction_replay_mode) {
        if (speed < 0.0f) {
            speed = -speed;
            left_turn = 0;
        } else {
            left_turn = 1;
        }
    } else {
        left_turn = pDrone->left_turn;
    }
    if (pDrone->funk_grooves == NULL) {
        return;
    }
    if (!gAction_replay_mode && (pDrone->current_state == 3 || pDrone->current_state == 4)) {
        return;
    }
    for (i = 0; i < pDrone->funk_grooves->count; i++) {
        groove = &pDrone->funk_grooves->items[i];

        switch (groove->type) {
        case eFunk_groove_type_spinny_groove:
            if (groove->spinny.speed_control) {
                angle = speed * delta_time / groove->spinny.omega;
            } else {
                angle = 0.0f;
            }
            if (!groove->spinny.reverse) {
                angle = -angle;
            }
            switch (groove->spinny.axis) {
            case tFunk_groove_axis_x:
                BrMatrix34PreRotateX(&groove->actor->t.t.mat, (br_angle)(angle * 65536.0f));
                break;
            case tFunk_groove_axis_y:
                BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.0f));
                break;
            case tFunk_groove_axis_z:
                BrMatrix34PreRotateZ(&groove->actor->t.t.mat, (br_angle)(angle * 65536.0f));
                break;
            }
            break;
        case eFunk_groove_type_steering_groove:
            if (gAction_replay_mode) {
                f = (10.0f - MIN(10.0f, MAX(0.0f, pDrone->field_0xe8))) / 70.0f;
            } else if (pDrone->field_0xdc == 2) {
                f = (10.0f - MIN(10.0f, MAX(0.0f, pDrone->h_radius))) / 70.0f;
            } else {
                f = 0.0f;
            }
            if (position < .5) {
                angle = position;
            } else {
                angle = 1.0f - position;
            }
            angle = angle * f * 2.0f;
            if (groove->steering.reverse) {
                angle = -angle;
            }
            if (left_turn) {
                angle = -angle;
            }
            BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[0], 1.0f, 0.0f, 0.0f);
            BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[1], 0.0f, 1.0f, 0.0f);
            BrVector3Set((br_vector3*)groove->actor->t.t.mat.m[2], 0.0f, 0.0f, 1.0f);
            BrMatrix34PreRotateY(&groove->actor->t.t.mat, (br_angle)(angle * 65536.0f));
            if (pDrone->field_0xdc == 2) {
                DoNotDprintf_drone("STEERING GROOVE: rot %f, h_radius %f, reverse %d, left_turn %d",
                    angle, pDrone->h_radius, groove->steering.reverse, pDrone->left_turn);
            }
            break;
        }
    }
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


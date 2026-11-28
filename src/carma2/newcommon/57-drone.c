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
#include "10-loading2.h"
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
#include "70-packfile.h"
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
// GLOBAL: CARMA2_CHEAT 0x00689a20
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

// GLOBAL: CARMA2_HW 0x00594760
// GLOBAL: CARMA2_SW 0x0058f6c0
// GLOBAL: CARMA2_CHEAT 0x0059b0b8
tDrone_form_within_rendering_distance_cbfn* gDrone_form_within_rendering_distance_functions[4] = {
    DroneCarWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
    DroneTrainWithinRenderingDistance,
    DronePlaneWithinRenderingDistance,
};

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

// GLOBAL: CARMA2_HW 0x006820cc
// GLOBAL: CARMA2_SW 0x0067c80c
// GLOBAL: CARMA2_CHEAT 0x00689974
tU32 gNext_drone_edit_path_munge;

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00450150
// STUB: CARMA2_SW 0x0044fef0
// STUB: CARMA2_CHEAT 0x004506a0
void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline int C2_HOOK_FASTCALL ReadPastBoundsShapesHeader(FILE* pF) {
    char s[256];

    while (!PFfeof(pF)) {
        GetALineAndDontArgue(pF, s);
        if (strcmp(s, "START OF BOUNDING SHAPES") == 0) {
            DoNotDprintf_drone("ReadPastBoundsShapesHeader() returning TRUE");
            return 1;
        }
    }
    return 0;
}

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00450680
// STUB: CARMA2_SW 0x00450410
// STUB: CARMA2_CHEAT 0x00450bd0
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone_spec, FILE* pF) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL LoadDrone(int pIndex) {
    tPath_name the_path;
    tDrone_spec* drone;
    tTWTVFS twt;
    FILE* f;

    drone = &gDrone_specs[pIndex];
    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, drone->form->name);
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadDroneActorsModels(drone);
    f = OpenDroneFile(drone->form->name);
    if (ReadPastBoundsShapesHeader(f)) {
        DoNotDprintf_drone("DRONE.C: Reading mechanics data for drone of type %s", drone->form->name);
        ReadMechanicsShapes(&drone->collision_info.shape, f);
        if (drone->collision_info.shape != NULL) {
            drone->field_0x46 = 1;
        }
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, drone->form->name);
    }
    LoadFunksAndGrooves(drone, f);
    PFfclose(f);
    ClosePackFileAndSetTiffLoading(twt);
}

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

inline void C2_HOOK_FASTCALL ResetDroneCrushyModel(br_model* pSource, br_model* pDest) {

    pDest->faces = pSource->faces;
    pDest->nfaces = pSource->nfaces;
    memcpy(pDest->vertices, pSource->vertices, pDest->nvertices * sizeof(br_vertex));
}

inline void C2_HOOK_FASTCALL ResetDroneModel(tDrone_spec* pDrone) {

    if (pDrone->field_0xf4 >= 0) {
        ResetDroneCrushyModel(gDroneStorage.models[pDrone->form->model_index], pDrone->form->models[pDrone->field_0xf4]);
        pDrone->form->field_0x80[pDrone->field_0xf4] = 0;
        pDrone->field_0xf4 = -1;
        pDrone->model_actor->model = gDroneStorage.models[pDrone->form->model_index];
    }
}

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

inline void C2_HOOK_FASTCALL SemiInitDroneSpec(tDrone_spec* pDrone) {

    DoNotDprintf_drone("SemiInitDroneSpec() - REINITIALISING DRONE SPEC");
    pDrone->field_0xa_pathnode_id = pDrone->field_0x8_pathnode_id;
    pDrone->field_0xe = -1;
    pDrone->field_0xc = -1;
    pDrone->field_0x14 = 0;
    pDrone->field_0x10 = 0;
    BrVector3Set(&pDrone->field_0x18, 0.0f, 0.0f, 0.0f);
    BrVector3Copy(&pDrone->pos, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position);
    pDrone->field_0xdc = 0;
    pDrone->field_0xe4 = 1.0f;
    pDrone->field_0x48 = 1.0f;
    ResetDroneModel(pDrone);
    pDrone->field_0x45 = gDrone_form_within_rendering_distance_functions[pDrone->form->type](&pDrone->actor->t.t.translate.t);
    pDrone->field_0x5d4 = 0;
    PipeDroneMatrix(pDrone);
}

// InitDroneSpec

// ChooseNextNode

// DroneOrientationChanged

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

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// FUNCTION: CARMA2_HW 0x0044cc70
// FUNCTION: CARMA2_SW 0x0044ca20
// FUNCTION: CARMA2_CHEAT 0x0044d1c0
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState pState) {

    switch (pState) {
    case eDrone_state_START:
        DoNotDprintf_drone("DroneStateFuncReset() START");
        StopRenderingThisDrone(pDrone);
        StopProcessingThisDrone(pDrone, 1);
        SemiInitDroneSpec(pDrone);
        BrVector3Copy(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0x8_pathnode_id].position);
        break;
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncReset() RUN");
        if (!gShow_drone_paths) {
            NewDroneState(pDrone, 2);
        }
        break;
    case eDrone_state_STOP:
        DoNotDprintf_drone("DroneStateFuncReset() STOP");
        break;
    default:
        break;
    }
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

// FUNCTION: CARMA2_HW 0x0044fda0
// FUNCTION: CARMA2_SW 0x0044fb40
// FUNCTION: CARMA2_CHEAT 0x004502f0
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {
    int i;
    int j;
    int count;
    tDrone_form* form;
    br_model* model;

    for (i = 0; i < gCount_drone_forms; i++) {
        form = &gDrone_forms[i];
        form->field_0x6c = 0;
        form->model_start = -1;
        form->model_end = 0;
        form->model_index = -1;
        form->count_models = 0;
        form->field_0x80 = NULL;
        form->models = NULL;
    }
    for (i = 0; i < gCount_drones; i++) {
        gDrone_specs[i].form->field_0x6c++;
        LoadDrone(i);
    }
    for (i = 0; i < gCount_drone_forms; i++) {
        form = &gDrone_forms[i];
        if (form->field_0x6c != 0) {
            if (form->crushability != 0.0f) {
                if ((int)form->field_0x6c > 10) {
                    count = 10;
                } else {
                    count = form->field_0x6c;
                }
            } else {
                count = 0;
            }
            form->count_models = count;
            if (form->count_models != 0) {
                form->models = BrMemCalloc(form->count_models, sizeof(br_model*), kMem_drone_model_dup);
                form->field_0x80 = BrMemCalloc(form->count_models, sizeof(tU8), kMem_drone_model_dup);
                for (j = 0; j < form->count_models; j++) {
                    model = gDroneStorage.models[form->model_index];
                    form->models[j] = BrModelAllocate(model->identifier, model->nvertices, 0);
                    form->models[j]->flags |= BR_MODF_UPDATEABLE;
                    ResetDroneCrushyModel(model, form->models[j]);
                    BrModelAdd(form->models[j]);
                }
            }
        }
    }
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

// FUNCTION: CARMA2_HW 0x00453b80
// FUNCTION: CARMA2_SW 0x00453910
// FUNCTION: CARMA2_CHEAT 0x004540e0
void C2_HOOK_FASTCALL FUN_00453b80(void) {
    int i;
    int node;
    int selected;
    tDrone_path_node* path_node;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
        return;
    }
    if (FUN_004538a0(0)) {
        path_node = &gDrone_path_nodes[gCount_drone_path_nodes];
        node = gCount_drone_path_nodes++;
        BrVector3Copy(&path_node->position, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
        for (i = 0; i < REC2_ASIZE(path_node->sections); i++) {
            path_node->sections[i].node1 = -1;
        }
        path_node->field_0x12e = 0;
        path_node->count_sections = 0;
        path_node->type = -1;
        selected = gSelected_drone_path_node_index;
        if (gDrone_path_nodes[selected].count_sections < 8) {
            gDrone_path_nodes[selected].sections[gDrone_path_nodes[selected].count_sections].node1 = node;
            gDrone_path_nodes[selected].count_sections++;
        }
        sprintf(gDrone_edit_message, "Shat out drone path node #%d", node);
        NewTextHeadupSlot(4, 0, 2000, -1, gDrone_edit_message);
    } else {
        node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
        if (node >= 0 && gDrone_path_nodes[node].count_sections >= 8) {
            NewTextHeadupSlot(4, 0, 3000, -1, "No more free links on this node");
            node = -1;
        }
    }
    InitialiseEditModelsEtc();
    if (node >= 0 && gDrone_path_model_actor != NULL && gElasticatey_drone_path_model != NULL) {
        gDrone_paths_elasticating = 1;
        gDrone_path_model_actor->render_style = BR_RSTYLE_FACES;
        gSelected_drone_path_node_index = node;
        MakeAISimpleEditSectionHere(gElasticatey_drone_path_model, 0, 0,
            &gDrone_path_nodes[node].position,
            &gProgram_state.current_car.car_master_actor->t.t.translate.t,
            gMat_dk_grn, gMat_lt_grn);
        BrModelUpdate(gElasticatey_drone_path_model, BR_MODU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x00453d60
// FUNCTION: CARMA2_SW 0x00453af0
// FUNCTION: CARMA2_CHEAT 0x004542c0
void C2_HOOK_FASTCALL FUN_00453d60(void) {

    gDrone_paths_elasticating = 0;
    if (gDrone_path_model_actor != NULL) {
        gDrone_path_model_actor->render_style = BR_RSTYLE_NONE;
    }
}

// FUNCTION: CARMA2_HW 0x00453d80
// FUNCTION: CARMA2_SW 0x00453b10
// FUNCTION: CARMA2_CHEAT 0x004542e0
void C2_HOOK_FASTCALL FUN_00453d80(void) {
    int i;
    int node;
    int selected;
    tDrone_path_node* path_node;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
        return;
    }
    if (FUN_004538a0(1) && FUN_00453ab0()) {
        path_node = &gDrone_path_nodes[gCount_drone_path_nodes];
        node = gCount_drone_path_nodes++;
        BrVector3Copy(&path_node->position, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
        for (i = 0; i < REC2_ASIZE(path_node->sections); i++) {
            path_node->sections[i].node1 = -1;
        }
        path_node->field_0x12e = 0;
        path_node->count_sections = 0;
        path_node->type = -1;
        selected = gSelected_drone_path_node_index;
        if (gDrone_path_nodes[selected].count_sections < 8) {
            gDrone_path_nodes[selected].sections[gDrone_path_nodes[selected].count_sections].node1 = node;
            gDrone_path_nodes[selected].count_sections++;
        }
        InitialiseEditModelsEtc();
        FUN_00453d60();
        sprintf(gDrone_edit_message, "Shat out drone path node #%d", gCount_drone_path_nodes);
        NewTextHeadupSlot(4, 0, 2000, -1, gDrone_edit_message);
    }
}

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

// FUNCTION: CARMA2_HW 0x00453f40
// FUNCTION: CARMA2_SW 0x00453cd0
// FUNCTION: CARMA2_CHEAT 0x004544a0
void C2_HOOK_FASTCALL FUN_00453f40(void) {
    int i;
    int node;
    int current;
    int found;
    int looped;
    int start;
    int previous;
    tDrone_path_node* path_node;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
        return;
    }
    if (gDrone_paths_elasticating) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Stop elasticating first");
        return;
    }
    node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
    if (node < 0) {
        return;
    }
    path_node = &gDrone_path_nodes[node];
    if (path_node->field_0x12e & 0x1) {
        current = node;
        looped = 0;
        do {
            found = 0;
            for (i = 0; i < gCount_drone_path_nodes; i++) {
                if (gDrone_path_nodes[i].sections[0].node1 == current) {
                    current = i;
                    if (i == node) {
                        looped = 1;
                    }
                    found = 1;
                    break;
                }
            }
        } while (found && !looped);
        if (looped) {
            start = -1;
        } else {
            start = current;
        }
        if (start < 0) {
            i = node;
            do {
                gDrone_path_nodes[i].sections[1].node1 = -1;
                gDrone_path_nodes[i].count_sections = 1;
                gDrone_path_nodes[i].field_0x12e &= ~0x1;
                i = gDrone_path_nodes[i].sections[0].node1;
            } while (i != node);
            InitialiseEditModelsEtc();
            return;
        } else {
            i = start;
            do {
                gDrone_path_nodes[i].sections[1].node1 = -1;
                if (gDrone_path_nodes[i].sections[0].node1 >= 0) {
                    gDrone_path_nodes[i].count_sections = 1;
                } else {
                    gDrone_path_nodes[i].count_sections = 0;
                }
                gDrone_path_nodes[i].field_0x12e &= ~0x1;
                i = gDrone_path_nodes[i].sections[0].node1;
            } while (i >= 0);
        }
        InitialiseEditModelsEtc();
        return;
    }
    do {
        i = path_node->sections[0].node1;
        path_node = &gDrone_path_nodes[i];
    } while (path_node->count_sections == 1 && i != node);
    if (gDrone_path_nodes[i].count_sections > 1) {
        NewTextHeadupSlot(4, 0, 2000, -1, "NOT a valid path");
        return;
    }
    current = node;
    do {
        found = -1;
        for (i = 0; i < gCount_drone_path_nodes; i++) {
            if (gDrone_path_nodes[i].count_sections != 0 && gDrone_path_nodes[i].sections[0].node1 == current) {
                if (found >= 0) {
                    NewTextHeadupSlot(4, 0, 2000, -1, "NOT a valid path");
                    return;
                }
                found = i;
            }
        }
        current = found;
    } while (found >= 0 && found != node);
    start = FUN_00454360(node);
    if (start < 0) {
        gDrone_path_nodes[node].field_0x12e |= 0x1;
        gDrone_path_nodes[node].count_sections = 2;
        gDrone_path_nodes[node].sections[1].node1 = -1;
        previous = node;
        i = gDrone_path_nodes[node].sections[0].node1;
        do {
            gDrone_path_nodes[i].field_0x12e |= 0x1;
            gDrone_path_nodes[i].count_sections = 2;
            gDrone_path_nodes[i].sections[1].node1 = previous;
            previous = i;
            i = gDrone_path_nodes[i].sections[0].node1;
        } while (previous != node);
        InitialiseEditModelsEtc();
        NewTextHeadupSlot(4, 0, 2000, -1, "OK, valid path");
        return;
    }
    if (gDrone_path_nodes[start].count_sections != 0) {
        gDrone_path_nodes[start].field_0x12e |= 0x1;
        gDrone_path_nodes[start].count_sections = 2;
        gDrone_path_nodes[start].sections[1].node1 = -1;
        previous = start;
        i = gDrone_path_nodes[start].sections[0].node1;
        while (gDrone_path_nodes[i].count_sections != 0) {
            gDrone_path_nodes[i].field_0x12e |= 0x1;
            gDrone_path_nodes[i].count_sections = 2;
            gDrone_path_nodes[i].sections[1].node1 = previous;
            previous = i;
            i = gDrone_path_nodes[i].sections[0].node1;
        }
        gDrone_path_nodes[i].field_0x12e |= 0x1;
        gDrone_path_nodes[i].count_sections = 2;
        gDrone_path_nodes[i].sections[0].node1 = -1;
        gDrone_path_nodes[i].sections[1].node1 = previous;
    }
    InitialiseEditModelsEtc();
    NewTextHeadupSlot(4, 0, 2000, -1, "OK, valid path");
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00454360
// FUNCTION: CARMA2_SW 0x004540f0
// FUNCTION: CARMA2_CHEAT 0x004548c0
int C2_HOOK_FASTCALL FUN_00454360(int pNode) {
    int i;
    int current;
    int found;
    int looped;
    int result;

    current = pNode;
    looped = 0;
    do {
        found = 0;
        for (i = 0; i < gCount_drone_path_nodes; i++) {
            if (gDrone_path_nodes[i].sections[0].node1 == current) {
                current = i;
                if (i == pNode) {
                    looped = 1;
                }
                found = 1;
                break;
            }
        }
    } while (found && !looped);
    if (looped) {
        result = -1;
    } else {
        result = current;
    }
    return result;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004543d0
// FUNCTION: CARMA2_SW 0x00454160
// FUNCTION: CARMA2_CHEAT 0x00454930
void C2_HOOK_FASTCALL FUN_004543d0(void) {
    int node;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
    } else if (gDrone_paths_elasticating) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Stop elasticating first");
    } else {
        node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
        if (node >= 0) {
            if (gDrone_path_nodes[node].field_0x12e & 0x10) {
                gDrone_path_nodes[node].field_0x12e &= ~0x10;
            } else {
                gDrone_path_nodes[node].field_0x12e |= 0x10;
            }
            FUN_00454470(node);
            gSelected_edit_drone_path = node;
            gNext_drone_edit_path_munge = gTime_stamp_for_this_munging + 1000;
            InitialiseEditModelsEtc();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00454470
// FUNCTION: CARMA2_SW 0x00454200
// FUNCTION: CARMA2_CHEAT 0x004549d0
void C2_HOOK_FASTCALL FUN_00454470(int pNode) {
    char s[256];
    tU8* flags;

    sprintf(s, "Drone node #%d, ", pNode);
    flags = &gDrone_path_nodes[pNode].field_0x12e;
    if (*flags & 0x10) {
        strcat(s, "SMALL vehicles");
    } else {
        strcat(s, "ALL sizes");
    }
    strcat(s, ", ");
    if (*flags & 0x2) {
        strcat(s, "PRIVATE");
    } else {
        strcat(s, "ANY vehicle");
    }
    strcat(s, ", ");
    if (*flags & 0x4) {
        strcat(s, "PARKING SPACE");
    } else {
        strcat(s, "-");
    }
    strcat(s, ", ");
    if (*flags & 0x8) {
        strcat(s, "LAYBY");
    } else {
        strcat(s, "-");
    }
    NewTextHeadupSlot(4, 0, 1000, -1, s);
}

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

// FUNCTION: CARMA2_HW 0x00454690
// FUNCTION: CARMA2_SW 0x00454420
// FUNCTION: CARMA2_CHEAT 0x00454bf0
void C2_HOOK_FASTCALL FUN_00454690(void) {
    int node;
    char s[256];

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
        return;
    }
    node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
    if (node >= 0) {
        sprintf(s, "Drone node %d", node);
        NewTextHeadupSlot(4, 0, 1000, -1, s);
    }
}

// FUNCTION: CARMA2_HW 0x00454710
// FUNCTION: CARMA2_SW 0x004544a0
// FUNCTION: CARMA2_CHEAT 0x00454c70
void C2_HOOK_FASTCALL FUN_00454710(void) {
    int node;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
    } else if (gDrone_paths_elasticating) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Stop elasticating first");
    } else {
        node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
        if (node >= 0) {
            FUN_00454470(node);
        }
    }
}

static void DeleteDrone(int pIndex) {

    dr_dprintf("DeleteDrone(): DELETING drone %d", pIndex);
    memmove(&gDrone_specs[pIndex], &gDrone_specs[pIndex + 1], (gCount_drones - pIndex - 1) * sizeof(tDrone_spec));
    gCount_drones--;
    dr_dprintf("DeleteDrone(): New number of drones %d", gCount_drones);
}

static int NodeIsReferenced(int pNode) {
    int i;
    int j;

    for (i = 0; i < gCount_drone_path_nodes; i++) {
        if (i != pNode) {
            for (j = 0; j < gDrone_path_nodes[i].count_sections; j++) {
                if (gDrone_path_nodes[i].sections[j].node1 == pNode) {
                    DoNotDprintf_drone("NodeIsReferenced(): Node %d referenced by node %d section %d", pNode, i, j);
                    return 1;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00454760
// FUNCTION: CARMA2_SW 0x004544f0
// FUNCTION: CARMA2_CHEAT 0x00454cc0
void C2_HOOK_FASTCALL DeleteNodeSectionsAndOrphan(void) {
    int i;
    int node;
    int orphan;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
        return;
    }
    if (gDrone_paths_elasticating) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Stop elasticating first");
        return;
    }
    node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
    if (node < 0) {
        return;
    }
    DoNotDprintf_drone("DeleteNodeSectionsAndOrphans(): DELETING %d, current number of nodes %d", node, gCount_drone_path_nodes);
    for (i = 0; i < gCount_drones; i++) {
        if (gDrone_specs[i].field_0x8_pathnode_id == node) {
            DoNotDprintf_drone("DeleteNodeSectionsAndOrphans(): Drone %d owns node %d, so deleting drone", i, node);
            DeleteDrone(i);
        }
    }
    DeleteNodeAndSections(node);
    orphan = -1;
    do {
        if (orphan >= 0) {
            DoNotDprintf_drone("DeleteNodeSectionsAndOrphans(): Orphan phase, DELETING orphan node %d", orphan);
            DeleteNodeAndSections(orphan);
            DoNotDprintf_drone("DeleteNodeSectionsAndOrphans(): Orphan phase, New number of nodes %d", gCount_drone_path_nodes);
        }
        orphan = -1;
        for (i = 0; i < gCount_drone_path_nodes; i++) {
            if (gDrone_path_nodes[i].count_sections == 0 && !NodeIsReferenced(i)) {
                DoNotDprintf_drone("DeleteNodeSectionsAndOrphans(): Orphan phase, node %d is NOT referenced by any others and has no sections", i);
                orphan = i;
                break;
            }
        }
    } while (orphan >= 0);
    DoNotDprintf_drone("DeleteNodeSectionsAndOrphans(): END. New number of nodes %d, number of drones %d", gCount_drone_path_nodes, gCount_drones);
    gDrones_unmodified = 0;
    InitialiseEditModelsEtc();
    sprintf(gDrone_edit_message, "DELETED node #%d. All nodes have been re-numbered.", node);
    NewTextHeadupSlot(4, 0, 3500, -1, gDrone_edit_message);
}

// FUNCTION: CARMA2_HW 0x004549f0
// FUNCTION: CARMA2_SW 0x00454780
// FUNCTION: CARMA2_CHEAT 0x00454f50
void C2_HOOK_FASTCALL DeleteNodeAndSections(int pNodenum) {
    int i;
    int j;
    int link_node;

    DoNotDprintf_drone("DeleteNodeAndSections(): node %d", pNodenum);
    for (i = 0; i < gCount_drone_path_nodes; i++) {
        if (i != pNodenum) {
            for (j = 0; j < gDrone_path_nodes[i].count_sections; j++) {
                link_node = gDrone_path_nodes[i].sections[j].node1;
                if (link_node > pNodenum) {
                    DoNotDprintf_drone("DeleteNodeAndSections(): node %d, section %d, link_node %d, being decremented", i, j, link_node);
                    gDrone_path_nodes[i].sections[j].node1--;
                } else if (link_node == pNodenum) {
                    DoNotDprintf_drone("DeleteNodeAndSections(): node %d, section %d, link_node %d, being deleted", i, j, link_node);
                    memmove(&gDrone_path_nodes[i].sections[j], &gDrone_path_nodes[i].sections[j + 1],
                        (gDrone_path_nodes[i].count_sections - j - 1) * sizeof(tDrone_path_node_section));
                    gDrone_path_nodes[i].count_sections--;
                    j--;
                }
            }
        }
    }
    DoNotDprintf_drone("DeleteNodeAndSections(): shifting nodes down 1 into node %d position, sizeof a node is %d, number of nodes to shift is %d, total mem block size %d",
        pNodenum, sizeof(tDrone_path_node), gCount_drone_path_nodes - pNodenum - 1,
        (gCount_drone_path_nodes - (pNodenum + 1)) * sizeof(tDrone_path_node));
    DoNotDprintf_drone("INFO: sizeofs: tDrone_section %d, tDrone_node %d, tDrone_spec %d, gDrone_nodes[pNodenum] %d",
        sizeof(tDrone_path_node_section), sizeof(tDrone_path_node), sizeof(tDrone_spec), sizeof(gDrone_path_nodes[pNodenum]));
    memmove(&gDrone_path_nodes[pNodenum], &gDrone_path_nodes[pNodenum + 1], (gCount_drone_path_nodes - (pNodenum + 1)) * sizeof(tDrone_path_node));
    gCount_drone_path_nodes--;
    DoNotDprintf_drone("DeleteNodeAndSections(): END");
}

// FUNCTION: CARMA2_HW 0x00454bb0
// FUNCTION: CARMA2_SW 0x00454940
// FUNCTION: CARMA2_CHEAT 0x00455110
void C2_HOOK_FASTCALL FUN_00454bb0(void) {
    int node;

    if (!gShow_drone_paths) {
        NewTextHeadupSlot(4, 0, 3000, -1, "You must show Drone Paths first");
        return;
    }
    if (gDrone_paths_elasticating) {
        NewTextHeadupSlot(4, 0, 3000, -1, "Stop elasticating first");
        return;
    }
    node = FindNearestNodeDrone(&gProgram_state.current_car.car_master_actor->t.t.translate.t, 2.0f);
    if (node >= 0) {
        BrVector3Copy(&gDrone_path_nodes[node].position, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
        PreprocessDronePaths();
        gDrones_unmodified = 0;
        InitialiseEditModelsEtc();
        NewTextHeadupSlot(4, 0, 2000, -1, "BOINK!");
    }
}

// SelectNextDroneForm

// SelectPreviousDroneForm

// CreateNewDrone


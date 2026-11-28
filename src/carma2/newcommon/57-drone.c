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

// GLOBAL: CARMA2_HW 0x00684518
// GLOBAL: CARMA2_SW 0x0067ec58
// GLOBAL: CARMA2_CHEAT 0x0068bdc0
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x00681fb0
// GLOBAL: CARMA2_SW 0x0067c6f0
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

inline int C2_HOOK_FASTCALL DroneStraightestArcForCorner2D(br_vector2* pCent, br_scalar* pRadius, br_scalar* pEntry_length, int* pLeft_not_right, const br_vector2* p1, const br_vector2* p2, const br_vector2* p3, br_scalar pWidth) {
    br_vector2 rel1;
    br_vector2 rel3;
    br_vector2 rot1;
    br_vector2 rot1u;
    br_scalar len12_squared;
    br_scalar len23_squared;
    br_scalar c;
    br_scalar numerator;
    br_scalar x;

    BrVector2Sub(&rel1, p1, p2);
    BrVector2Sub(&rel3, p3, p2);
    len12_squared = BrVector2LengthSquared(&rel1);
    len23_squared = BrVector2LengthSquared(&rel3);
    if (len12_squared == 0.0f || len23_squared == 0.0f) {
        return 0;
    }
    rot1.v[0] = rel1.v[0] * rel3.v[0] + rel1.v[1] * rel3.v[1];
    rot1.v[1] = rel1.v[0] * -rel3.v[1] + rel1.v[1] * rel3.v[0];
    BrVector2Normalise(&rot1u, &rot1);
    if (rot1u.v[1] > 0.0f) {
        *pLeft_not_right = 1;
    } else {
        rot1u.v[1] = -rot1u.v[1];
        *pLeft_not_right = 0;
    }
    if (rot1u.v[1] < 0.04f) {
        return 0;
    }
    numerator = (rot1u.v[0] * pWidth + pWidth) / rot1u.v[1];
    x = rot1u.v[1] / (rot1u.v[0] + 1.0f);
    *pEntry_length = x * pWidth + numerator + (br_scalar)sqrt((x * x * pWidth + 2.0f * x * numerator - pWidth) * pWidth);
    if (len12_squared < 9.0f * *pEntry_length * *pEntry_length) {
        *pEntry_length = (br_scalar)sqrt(len12_squared) / 3.0f;
    }
    if (len23_squared < 9.0f * *pEntry_length * *pEntry_length) {
        *pEntry_length = (br_scalar)sqrt(len23_squared) / 3.0f;
    }
    *pRadius = *pEntry_length * x;
    if (*pLeft_not_right) {
        pCent->v[0] = *pEntry_length * rel3.v[0] + *pRadius * -rel3.v[1];
        pCent->v[1] = *pEntry_length * rel3.v[1] + *pRadius * rel3.v[0];
    } else {
        pCent->v[0] = *pEntry_length * rel3.v[0] + *pRadius * rel3.v[1];
        pCent->v[1] = *pEntry_length * rel3.v[1] + -*pRadius * rel3.v[0];
    }
    c = (br_scalar)sqrt(len23_squared);
    pCent->v[0] /= c;
    pCent->v[1] /= c;
    pCent->v[0] += p2->v[0];
    pCent->v[1] += p2->v[1];
    return 1;
}

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

// FUNCTION: CARMA2_HW 0x0044dac0
// FUNCTION: CARMA2_SW 0x0044d870
// FUNCTION: CARMA2_CHEAT 0x0044e010
void C2_HOOK_FASTCALL CalculateDrivingInfo(tDrone_spec* pDrone) {
    br_vector3 v1;
    br_vector3 v2;
    br_vector3 up;
    br_vector3 normal;
    br_vector3 axis;
    br_vector3 rotated_v1;
    br_vector3 rotated_v2;
    br_vector2 p1;
    br_vector2 p2;
    br_vector2 p3;
    br_vector2 dir;
    br_vector2 ref;
    br_angle angle;
    br_scalar angle_rad;
    br_scalar slope;
    br_scalar width;
    br_scalar min_width;
    br_scalar cos_angle;
    int left_not_right;

    if (pDrone->form->type != kDroneType_plane) {
        min_width = 0.02f;
        width = 0.5f;
    } else {
        min_width = 0.1f;
        width = 3.0f;
    }
    pDrone->field_0xe0 = 0.0f;
    pDrone->field_0x74 = 0.0f;
    if (pDrone->field_0x14 != NULL && (pDrone->form->flags & 0x1)) {
        BrVector3Sub(&v1, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position, &gDrone_path_nodes[pDrone->field_0xc].position);
        BrVector3Sub(&v2, &gDrone_path_nodes[pDrone->field_0xe].position, &gDrone_path_nodes[pDrone->field_0xc].position);
        BrVector3Cross(&normal, &v2, &v1);
        if (normal.v[1] < 0.0f) {
            BrVector3Negate(&normal, &normal);
        }
        BrVector3Normalise(&normal, &normal);
        BrVector3Set(&up, 0.0f, 1.0f, 0.0f);
        BrVector3Cross(&axis, &normal, &up);
        BrVector3Normalise(&axis, &axis);
        angle = BrRadianToAngle(acos(normal.v[1]));
        angle_rad = BrAngleToRadian(angle);
        slope = angle_rad;
        if (slope > PI) {
            slope = 0.0f;
        }
        if (slope > PI / 2.0) {
            slope = (br_scalar)(PI - slope);
        }
        if (pDrone->form->type != kDroneType_plane && pDrone->form->type != kDroneType_chopper) {
            width = (br_scalar)((1.0 - slope / (PI / 2.0)) * (width - min_width) + min_width);
        }
        BrMatrix34Rotate(&pDrone->field_0x78, angle, &axis);
        BrMatrix34LPInverse(&pDrone->field_0xa8, &pDrone->field_0x78);
        BrMatrix34ApplyV(&rotated_v1, &v1, &pDrone->field_0x78);
        BrMatrix34ApplyV(&rotated_v2, &v2, &pDrone->field_0x78);
        BrVector2Set(&p1, rotated_v1.v[0], rotated_v1.v[2]);
        BrVector2Set(&p2, 0.0f, 0.0f);
        BrVector2Set(&p3, rotated_v2.v[0], rotated_v2.v[2]);
        if (DroneStraightestArcForCorner2D(&pDrone->field_0x54, &pDrone->field_0x68, &pDrone->field_0x6c, &left_not_right, &p1, &p2, &p3, width)) {
            pDrone->left_turn = left_not_right;
            pDrone->field_0xe8 = pDrone->h_radius = BrVector2Length(&pDrone->field_0x54) * ((br_scalar)sin(angle_rad) * 7.0f + 1.0f);
            BrVector2Set(&ref, pDrone->field_0x68, 0.0f);
            BrVector2Sub(&p1, &p1, &pDrone->field_0x54);
            BrVector2Sub(&p2, &p2, &pDrone->field_0x54);
            BrVector2Sub(&p3, &p3, &pDrone->field_0x54);

            BrVector2Sub(&dir, &p1, &p2);
            BrVector2Normalise(&dir, &dir);
            BrVector2Scale(&dir, &dir, pDrone->field_0x6c);
            BrVector2Add(&dir, &dir, &p2);
            cos_angle = (br_scalar)MIN(1.0, MAX(-1.0, BrVector2Dot(&dir, &ref) / (pDrone->field_0x68 * pDrone->field_0x68)));
            pDrone->field_0x5c = (br_scalar)acos(cos_angle);
            if (dir.v[1] < 0.0f) {
                pDrone->field_0x5c = (br_scalar)(TAU - pDrone->field_0x5c);
            }

            BrVector2Sub(&dir, &p3, &p2);
            BrVector2Normalise(&dir, &dir);
            BrVector2Scale(&dir, &dir, pDrone->field_0x6c);
            BrVector2Add(&dir, &dir, &p2);
            cos_angle = (br_scalar)MIN(1.0, MAX(-1.0, BrVector2Dot(&dir, &ref) / (pDrone->field_0x68 * pDrone->field_0x68)));
            pDrone->field_0x64 = (br_scalar)acos(cos_angle);
            if (dir.v[1] < 0.0f) {
                pDrone->field_0x64 = (br_scalar)(TAU - pDrone->field_0x64);
            }

            if (left_not_right) {
                while (pDrone->field_0x64 < pDrone->field_0x5c) {
                    pDrone->field_0x64 = (br_scalar)(pDrone->field_0x64 + TAU);
                }
            } else {
                while (pDrone->field_0x5c < pDrone->field_0x64) {
                    pDrone->field_0x5c = (br_scalar)(pDrone->field_0x5c + TAU);
                }
            }
            pDrone->field_0x60 = pDrone->field_0x5c;
            pDrone->field_0xdc = 1;
        } else {
            pDrone->field_0x68 = 1000.0f;
            pDrone->field_0x6c = 0.0f;
        }
    } else {
        pDrone->field_0x68 = 1000.0f;
        pDrone->field_0x6c = 0.0f;
    }
    if (pDrone->form->speed < 0.0f) {
        pDrone->field_0x50 = pDrone->field_0x68 * 0.5f;
        pDrone->field_0x50 = MIN(pDrone->form->max_speed, MAX(pDrone->form->min_speed, pDrone->field_0x50));
    } else {
        pDrone->field_0x50 = pDrone->form->speed;
    }
    DoNotDprintf_drone("CalculateDrivingInfo called - centre node %d, radius %f, section after corner %p", pDrone->field_0xc, pDrone->field_0x68, pDrone->field_0x10);
}

// PauseDroneState

// UnPauseDroneState

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

// FUNCTION: CARMA2_HW 0x0044e410
// FUNCTION: CARMA2_SW 0x0044e1b0
// FUNCTION: CARMA2_CHEAT 0x0044e960
int C2_HOOK_FASTCALL ChooseNextNode(tDrone_spec* pDrone) {

    if (pDrone->field_0xc < 0) {
        if (gDrone_path_nodes[pDrone->field_0xa_pathnode_id].count_sections != 0) {
            pDrone->field_0x10 = &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].sections[IRandomBetween(0, MAX(0, gDrone_path_nodes[pDrone->field_0xa_pathnode_id].count_sections - 1))];
        } else {
            pDrone->field_0x14 = NULL;
            pDrone->field_0x10 = NULL;
            pDrone->field_0xc = -1;
            pDrone->field_0xa_pathnode_id = -1;
        }
    } else {
        pDrone->field_0xa_pathnode_id = pDrone->field_0xc;
        pDrone->field_0x10 = pDrone->field_0x14;
    }
    if (pDrone->field_0x10 != NULL) {
        pDrone->field_0xc = pDrone->field_0x10->node1;
    } else {
        pDrone->field_0xc = -1;
    }
    if (pDrone->field_0xc < 0) {
        return 0;
    }
    if (gDrone_path_nodes[pDrone->field_0xc].count_sections != 0) {
        pDrone->field_0x14 = &gDrone_path_nodes[pDrone->field_0xc].sections[IRandomBetween(0, MAX(0, gDrone_path_nodes[pDrone->field_0xc].count_sections - 1))];
        pDrone->field_0xe = pDrone->field_0x14->node1;
    } else {
        pDrone->field_0x14 = NULL;
        pDrone->field_0xe = -1;
    }
    return 1;
}

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

// STUB: CARMA2_HW 0x0044cc70
// STUB: CARMA2_SW 0x0044ca20
// STUB: CARMA2_CHEAT 0x0044d1c0
void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState pState) {
    NOT_IMPLEMENTED();
}

inline void C2_HOOK_FASTCALL InitDroneDrivingInfo(tDrone_spec* pDrone) {
    pDrone->field_0xdc = 0;
    pDrone->field_0xe4 = 0.0f;
    pDrone->field_0x48 = 0.0f;
    pDrone->field_0x4c = 0.0f;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0044d2a0
// FUNCTION: CARMA2_SW 0x0044d050
// FUNCTION: CARMA2_CHEAT 0x0044d7f0
void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone) {
    br_vector3 offset;
    br_vector3 direction;
    br_vector3 tmp;
    br_scalar time_left;
    br_scalar time_used;
    br_scalar angle_step;
    br_scalar new_angle;
    br_scalar new_distance;
    br_scalar end_distance;
    br_scalar target_speed;
    br_scalar brake_distance;
    br_scalar deceleration;
    br_scalar time_to_target_speed;
    int finished;

    DoNotDprintf_drone("MoveThisDronePlane()");
    time_left = gDrone_delta_time;
    while (time_left > 0.0f) {
        switch (pDrone->field_0xdc) {
        case 2:
            if (pDrone->field_0x68 > 0.0f) {
                angle_step = time_left * pDrone->field_0x48 / pDrone->field_0x68;
                finished = 0;
                if (pDrone->left_turn) {
                    new_angle = pDrone->field_0x60 + angle_step;
                    if (new_angle >= pDrone->field_0x64) {
                        time_used = (pDrone->field_0x64 - pDrone->field_0x60) * pDrone->field_0x68 / pDrone->field_0x48;
                        finished = 1;
                        pDrone->field_0x60 = pDrone->field_0x64;
                    } else {
                        time_used = gDrone_delta_time;
                        pDrone->field_0x60 = new_angle;
                    }
                } else {
                    new_angle = pDrone->field_0x60 - angle_step;
                    if (new_angle <= pDrone->field_0x64) {
                        time_used = (pDrone->field_0x60 - pDrone->field_0x64) * pDrone->field_0x68 / pDrone->field_0x48;
                        finished = 1;
                        pDrone->field_0x60 = pDrone->field_0x64;
                    } else {
                        time_used = gDrone_delta_time;
                        pDrone->field_0x60 = new_angle;
                    }
                }
                offset.v[0] = (br_scalar)(pDrone->field_0x68 * cos(pDrone->field_0x60) + pDrone->field_0x54.v[0]);
                offset.v[1] = 0.0f;
                offset.v[2] = (br_scalar)(pDrone->field_0x68 * sin(pDrone->field_0x60) + pDrone->field_0x54.v[1]);
                BrMatrix34ApplyV(&pDrone->actor->t.t.translate.t, &offset, &pDrone->field_0xa8);
                BrVector3Accumulate(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0xc].position);
                pDrone->field_0xe0 = (pDrone->field_0x60 - pDrone->field_0x5c) / (pDrone->field_0x64 - pDrone->field_0x5c);
                pDrone->field_0x74 = pDrone->field_0xe0;
                BrVector3Scale(&direction, &pDrone->field_0x14->field_0x10, pDrone->field_0x74);
                BrVector3Scale(&tmp, &pDrone->field_0x10->field_0x10, 1.0f - pDrone->field_0x74);
                BrVector3Accumulate(&direction, &tmp);
                PointActorAlongThisBloodyVector(pDrone->actor, &direction);
                time_left -= time_used;
                if (finished) {
                    pDrone->field_0x4c = pDrone->field_0x6c;
                    pDrone->field_0xdc = 0;
                }
            } else {
                pDrone->field_0x4c = 0.0f;
                pDrone->field_0xdc = 0;
            }
            pDrone->field_0xe4 = pDrone->field_0x48;
            break;
        case 1:
            new_distance = time_left * pDrone->field_0x48 + pDrone->field_0x4c;
            switch (pDrone->field_0x10->type) {
            case 6:
                target_speed = pDrone->form->min_speed;
                DoNotDprintf_drone("eDDS_straight_driving: TAXI/LANDING %f %f", target_speed, pDrone->field_0x48);
                break;
            case 4:
            case 5:
                target_speed = pDrone->form->max_speed * 0.66f;
                DoNotDprintf_drone("eDDS_straight_driving: APPROACH %f %f", target_speed, pDrone->field_0x48);
                break;
            default:
                target_speed = pDrone->form->max_speed;
                DoNotDprintf_drone("eDDS_straight_driving: TAKEOFF/INFLIGHT %f %f", target_speed, pDrone->field_0x48);
                break;
            }
            end_distance = pDrone->field_0x10->field_0x1c - pDrone->field_0x6c;
            if (pDrone->field_0x4c >= end_distance) {
                time_used = 0.0f;
                pDrone->field_0xdc = 2;
            } else if (new_distance > end_distance) {
                if (pDrone->field_0x48 > target_speed) {
                    DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 2a %f %f", target_speed, pDrone->field_0x48);
                    time_used = (pDrone->field_0x48 - target_speed) / pDrone->form->max_acceleration;
                    pDrone->field_0x48 = target_speed;
                } else {
                    DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 2b %f %f", target_speed, pDrone->field_0x48);
                    time_used = time_left;
                }
            } else {
                brake_distance = end_distance - (pDrone->field_0x48 * pDrone->field_0x48 - target_speed * target_speed) * 0.5f;
                DoNotDprintf_drone("eDDS_straight_driving: decel_envelope %f %f %f", brake_distance, target_speed, pDrone->field_0x48);
                if (pDrone->field_0x10->type != 2 && pDrone->field_0x10->type != 3
                    && pDrone->field_0x48 > target_speed && pDrone->field_0x4c >= brake_distance) {
                    DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 4 %f %f", target_speed, pDrone->field_0x48);
                    deceleration = -((pDrone->field_0x48 * pDrone->field_0x48 - target_speed * target_speed)
                        / (2.0f * (pDrone->field_0x10->field_0x1c - pDrone->field_0x6c - pDrone->field_0x4c)));
                    time_to_target_speed = (pDrone->field_0x48 - target_speed) / -deceleration;
                    DoNotDprintf_drone("eDDS_straight_driving: a %f, time_to_hit_max_corner_speed %f %f ", deceleration, time_to_target_speed, target_speed, pDrone->field_0x48);
                    if (time_to_target_speed <= time_left) {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 4a %f %f", target_speed, pDrone->field_0x48);
                        pDrone->field_0x48 = target_speed;
                        time_used = time_to_target_speed;
                    } else {
                        DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 4b %f %f", target_speed, pDrone->field_0x48);
                        pDrone->field_0x48 = deceleration * time_left + pDrone->field_0x48;
                        time_used = time_left;
                    }
                } else if (pDrone->field_0x10->type != 2 && pDrone->field_0x10->type != 3
                    && pDrone->field_0x48 > target_speed && new_distance > brake_distance) {
                    DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 5 %f %f", target_speed, pDrone->field_0x48);
                    time_used = time_left;
                } else if (pDrone->field_0x10->type != 2 && pDrone->field_0x10->type != 3
                    && pDrone->field_0x48 > target_speed
                    && pDrone->form->max_acceleration * time_left * time_left * 0.5f + pDrone->field_0x48 * time_left + pDrone->field_0x4c > brake_distance) {
                    DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 6 %f %f", target_speed, pDrone->field_0x48);
                    time_used = time_left;
                } else {
                    DoNotDprintf_drone("eDDS_straight_driving: CLAUSE 7 %f %f", target_speed, pDrone->field_0x48);
                    if (pDrone->field_0x48 > target_speed) {
                        pDrone->field_0x48 = pDrone->field_0x48 - pDrone->form->max_acceleration * time_left;
                        if (pDrone->field_0x48 < pDrone->form->min_speed) {
                            pDrone->field_0x48 = pDrone->form->min_speed;
                        }
                    } else {
                        pDrone->field_0x48 = pDrone->form->max_acceleration * time_left + pDrone->field_0x48;
                        if (pDrone->field_0x48 > pDrone->form->max_speed) {
                            pDrone->field_0x48 = pDrone->form->max_speed;
                        }
                    }
                    time_used = gDrone_delta_time;
                }
            }
            if (pDrone->field_0xdc == 2) {
                pDrone->field_0x4c = pDrone->field_0x10->field_0x1c - pDrone->field_0x6c;
                BrVector3Scale(&pDrone->actor->t.t.translate.t, &pDrone->field_0x10->field_0x10, pDrone->field_0x4c);
                BrVector3Accumulate(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position);
            } else {
                pDrone->field_0x4c = pDrone->field_0x48 * time_used + pDrone->field_0x4c;
                BrVector3Scale(&pDrone->actor->t.t.translate.t, &pDrone->field_0x10->field_0x10, pDrone->field_0x4c);
                BrVector3Accumulate(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position);
            }
            time_left -= time_used;
            pDrone->field_0xe4 = pDrone->field_0x48;
            break;
        case 0:
            DoNotDprintf_drone("eDDS_straight_start");
            if (!ChooseNextNode(pDrone)) {
                NewDroneState(pDrone, 4);
                return;
            }
            CalculateDrivingInfo(pDrone);
            PointActorAlongThisBloodyVector(pDrone->actor, &pDrone->field_0x10->field_0x04);
            pDrone->field_0xdc = 1;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0044e540
// FUNCTION: CARMA2_SW 0x0044e2e0
// FUNCTION: CARMA2_CHEAT 0x0044ea90
void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone) {
    br_vector3 offset;
    br_vector3 direction;
    br_vector3 tmp;
    br_scalar time_left;
    br_scalar time_used;
    br_scalar angle_step;
    br_scalar new_angle;
    br_scalar new_distance;
    br_scalar end_distance;
    br_scalar speed_diff_squared;
    br_scalar brake_distance;
    br_scalar deceleration;
    br_scalar time_to_corner_speed;
    int finished;

    DoNotDprintf_drone("MoveThisDroneCar()");
    time_left = gDrone_delta_time;
    while (time_left > 0.0f) {
        switch (pDrone->field_0xdc) {
        case 2:
            if (pDrone->field_0x68 > 0.0f) {
                angle_step = time_left * pDrone->field_0x48 / pDrone->field_0x68;
                finished = 0;
                if (pDrone->left_turn) {
                    new_angle = pDrone->field_0x60 + angle_step;
                    if (new_angle >= pDrone->field_0x64) {
                        time_used = (pDrone->field_0x64 - pDrone->field_0x60) * pDrone->field_0x68 / pDrone->field_0x48;
                        finished = 1;
                        pDrone->field_0x60 = pDrone->field_0x64;
                    } else {
                        time_used = gDrone_delta_time;
                        pDrone->field_0x60 = new_angle;
                    }
                } else {
                    new_angle = pDrone->field_0x60 - angle_step;
                    if (new_angle <= pDrone->field_0x64) {
                        time_used = (pDrone->field_0x60 - pDrone->field_0x64) * pDrone->field_0x68 / pDrone->field_0x48;
                        finished = 1;
                        pDrone->field_0x60 = pDrone->field_0x64;
                    } else {
                        time_used = gDrone_delta_time;
                        pDrone->field_0x60 = new_angle;
                    }
                }
                offset.v[0] = (br_scalar)(pDrone->field_0x68 * cos(pDrone->field_0x60) + pDrone->field_0x54.v[0]);
                offset.v[1] = 0.0f;
                offset.v[2] = (br_scalar)(pDrone->field_0x68 * sin(pDrone->field_0x60) + pDrone->field_0x54.v[1]);
                BrMatrix34ApplyV(&pDrone->actor->t.t.translate.t, &offset, &pDrone->field_0xa8);
                BrVector3Accumulate(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0xc].position);
                pDrone->field_0xe0 = (pDrone->field_0x60 - pDrone->field_0x5c) / (pDrone->field_0x64 - pDrone->field_0x5c);
                pDrone->field_0x74 = pDrone->field_0xe0;
                BrVector3Scale(&direction, &pDrone->field_0x14->field_0x10, pDrone->field_0x74);
                BrVector3Scale(&tmp, &pDrone->field_0x10->field_0x10, 1.0f - pDrone->field_0x74);
                BrVector3Accumulate(&direction, &tmp);
                if (pDrone->form->flags & 0x10) {
                    direction.v[1] = 0.0f;
                }
                PointActorAlongThisBloodyVector(pDrone->actor, &direction);
                time_left -= time_used;
                if (finished) {
                    pDrone->field_0x4c = pDrone->field_0x6c;
                    pDrone->field_0xdc = 0;
                }
            } else {
                pDrone->field_0x4c = 0.0f;
                pDrone->field_0xdc = 0;
            }
            pDrone->field_0xe4 = pDrone->field_0x48;
            break;
        case 1:
            if (pDrone->form->speed >= 0.0f) {
                pDrone->field_0x48 = pDrone->form->speed;
                pDrone->field_0x50 = pDrone->form->speed;
            }
            end_distance = pDrone->field_0x10->field_0x1c - pDrone->field_0x6c;
            new_distance = time_left * pDrone->field_0x48 + pDrone->field_0x4c;
            if (pDrone->field_0x4c >= end_distance) {
                time_used = 0.0f;
                pDrone->field_0xdc = 2;
                if (pDrone->field_0x48 > pDrone->field_0x50) {
                    pDrone->field_0x48 = pDrone->field_0x50;
                }
            } else if (new_distance > end_distance) {
                if (pDrone->field_0x48 > pDrone->field_0x50) {
                    time_used = pDrone->field_0x48 - pDrone->field_0x50;
                    pDrone->field_0x48 = pDrone->field_0x50;
                } else {
                    time_used = time_left;
                }
            } else if (pDrone->form->speed < 0.0f) {
                speed_diff_squared = pDrone->field_0x48 * pDrone->field_0x48 - pDrone->field_0x50 * pDrone->field_0x50;
                brake_distance = end_distance - speed_diff_squared * 0.5f;
                if (pDrone->field_0x48 > pDrone->field_0x50 && pDrone->field_0x4c >= brake_distance) {
                    deceleration = -(speed_diff_squared / (2.0f * (end_distance - pDrone->field_0x4c)));
                    time_to_corner_speed = (pDrone->field_0x48 - pDrone->field_0x50) / -deceleration;
                    if (time_to_corner_speed <= time_left) {
                        pDrone->field_0x48 = pDrone->field_0x50;
                        time_used = time_to_corner_speed;
                    } else {
                        pDrone->field_0x48 = deceleration * time_left + pDrone->field_0x48;
                        time_used = time_left;
                    }
                } else if ((pDrone->field_0x48 <= pDrone->field_0x50 || new_distance <= brake_distance)
                    && (pDrone->field_0x48 <= pDrone->field_0x50
                        || pDrone->form->max_acceleration * time_left * time_left * 0.5f + pDrone->field_0x48 * time_left + pDrone->field_0x4c <= brake_distance)) {
                    pDrone->field_0x48 = pDrone->form->max_acceleration * time_left + pDrone->field_0x48;
                    if (pDrone->field_0x48 > pDrone->form->max_speed) {
                        pDrone->field_0x48 = pDrone->form->max_speed;
                    }
                    time_used = gDrone_delta_time;
                } else {
                    time_used = time_left;
                }
            } else {
                time_used = time_left;
            }
            if (pDrone->field_0xdc == 2) {
                pDrone->field_0x4c = pDrone->field_0x10->field_0x1c - pDrone->field_0x6c;
                BrVector3Scale(&pDrone->actor->t.t.translate.t, &pDrone->field_0x10->field_0x10, pDrone->field_0x4c);
                BrVector3Accumulate(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position);
            } else {
                pDrone->field_0x4c = pDrone->field_0x48 * time_used + pDrone->field_0x4c;
                BrVector3Scale(&pDrone->actor->t.t.translate.t, &pDrone->field_0x10->field_0x10, pDrone->field_0x4c);
                BrVector3Accumulate(&pDrone->actor->t.t.translate.t, &gDrone_path_nodes[pDrone->field_0xa_pathnode_id].position);
            }
            time_left -= time_used;
            pDrone->field_0xe4 = pDrone->field_0x48;
            break;
        case 0:
            DoNotDprintf_drone("eDDS_straight_start");
            if (!ChooseNextNode(pDrone)) {
                NewDroneState(pDrone, 4);
                return;
            }
            CalculateDrivingInfo(pDrone);
            BrVector3Copy(&direction, &pDrone->field_0x10->field_0x04);
            if (pDrone->form->flags & 0x10) {
                direction.v[1] = 0.0f;
            }
            PointActorAlongThisBloodyVector(pDrone->actor, &direction);
            pDrone->field_0xdc = 1;
            break;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

inline void C2_HOOK_FASTCALL PossiblyPipeDroneMovement(tDrone_spec* pDrone) {
    if (pDrone->field_0xdc == 1) {
        PipeSingleDroneStraightPos(pDrone, pDrone->field_0x10,
            DRScalarToU16(pDrone->field_0x48, -500.0f, 500.0f),
            DRScalarToU16(pDrone->field_0x4c, 0.0f, 255.0f));
    } else if (pDrone->field_0xdc == 2) {
        PipeDroneMatrix(pDrone);
    } else {
        PDEnterDebugger("Keith");
    }
}

// FUNCTION: CARMA2_HW 0x0044d1d0
// FUNCTION: CARMA2_SW 0x0044cf80
// FUNCTION: CARMA2_CHEAT 0x0044d720
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState pState) {

    switch (pState) {
    case eDrone_state_START:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() START");
        InitDroneDrivingInfo(pDrone);
        break;
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() RUN");
        if (pDrone->field_0x44) {
            switch (pDrone->form->type) {
            case kDroneType_plane:
                MoveThisDronePlane(pDrone);
                break;
            default:
                MoveThisDroneCar(pDrone);
                break;
            }
            PossiblyPipeDroneMovement(pDrone);
        }
        break;
    default:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() DEFAULT");
        break;
    }
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

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths


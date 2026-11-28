/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "49-physics.h"

#include "00-car.h"
#include "09-funks.h"
#include "16-graphics1.h"
#include "22-replay.h"
#include "28-world3.h"
#include "32-spark.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "49-physics-support.h"
#include "52-errors.h"
#include "65-hull.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "c2_math.h"
#include "c2_string.h"
#include "c2_stdlib.h"

#include <stddef.h>

// GLOBAL: CARMA2_HW 0x00692dc8
// GLOBAL: CARMA2_SW 0x0068ce20
// GLOBAL: CARMA2_CHEAT 0x0069a940
int gINT_00692dc8;

// GLOBAL: CARMA2_HW 0x006940c0
// GLOBAL: CARMA2_SW 0x0068e118
// GLOBAL: CARMA2_CHEAT 0x0069bc38
tPhysicsError_cbfn* gPhysics_error_cb;

// GLOBAL: CARMA2_HW 0x00744818
// GLOBAL: CARMA2_SW 0x0073b2d8
// GLOBAL: CARMA2_CHEAT 0x0074beb8
void* gPhysics_buffer1_ptr;

// GLOBAL: CARMA2_HW 0x00744814
// GLOBAL: CARMA2_SW 0x0073b2d4
// GLOBAL: CARMA2_CHEAT 0x0074beb4
void* gPhysics_buffer2_ptr;

// GLOBAL: CARMA2_HW 0x0074480c
// GLOBAL: CARMA2_SW 0x0073b2cc
// GLOBAL: CARMA2_CHEAT 0x0074beac
int gTotal_count_smash_peds;

// GLOBAL: CARMA2_HW 0x00744810
// GLOBAL: CARMA2_SW 0x0073b2d0
// GLOBAL: CARMA2_CHEAT 0x0074beb0
int gImmortal_peds;

// GLOBAL: CARMA2_HW 0x00744804
// GLOBAL: CARMA2_SW 0x0073b2c4
// GLOBAL: CARMA2_CHEAT 0x0074bea4
int gBOOL_00744804;

// GLOBAL: CARMA2_HW 0x00744800
// GLOBAL: CARMA2_SW 0x0073b2c0
// GLOBAL: CARMA2_CHEAT 0x0074bea0
float gPed_head_scale;

// GLOBAL: CARMA2_HW 0x00744808
// GLOBAL: CARMA2_SW 0x0073b2c8
// GLOBAL: CARMA2_CHEAT 0x0074bea8
tPedestrian* gPedestrian_array;

// GLOBAL: CARMA2_HW 0x0074a5e0
// GLOBAL: CARMA2_SW 0x007410a0
// GLOBAL: CARMA2_CHEAT 0x00751c80
void* gPhysics_buffer3_ptr;

// GLOBAL: CARMA2_HW 0x0074a5e4
// GLOBAL: CARMA2_SW 0x007410a4
// GLOBAL: CARMA2_CHEAT 0x00751c84
int gPHIL_mechanics_time_sync;

// GLOBAL: CARMA2_HW 0x0074a5e8
// GLOBAL: CARMA2_SW 0x007410a8
// GLOBAL: CARMA2_CHEAT 0x00751c88
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x006940c8
// GLOBAL: CARMA2_SW 0x0068e120
// GLOBAL: CARMA2_CHEAT 0x0069bc40
int gPhysics_other_buffer_capacity;

// GLOBAL: CARMA2_HW 0x00694140
// GLOBAL: CARMA2_SW 0x0068e198
// GLOBAL: CARMA2_CHEAT 0x0069bcb8
tProximity_ray gProximity_rays[20];

// GLOBAL: CARMA2_HW 0x00694274
// GLOBAL: CARMA2_SW 0x0068e2cc
// GLOBAL: CARMA2_CHEAT 0x0069bdec
tPed_giblet_size_spec* gGiblet_sizes;

// GLOBAL: CARMA2_HW 0x00694278
// GLOBAL: CARMA2_SW 0x0068e2d0
// GLOBAL: CARMA2_CHEAT 0x0069bdf0
br_model* gPed_spurt_model;

// GLOBAL: CARMA2_HW 0x0069427c
// GLOBAL: CARMA2_SW 0x0068e2d4
// GLOBAL: CARMA2_CHEAT 0x0069bdf4
int gPed_spurty_lumps_count;

// GLOBAL: CARMA2_HW 0x00694268
// GLOBAL: CARMA2_SW 0x0068e2c0
// GLOBAL: CARMA2_CHEAT 0x0069bde0
int gPed_times[3];

// GLOBAL: CARMA2_HW 0x00694280
// GLOBAL: CARMA2_SW 0x0068e2d8
// GLOBAL: CARMA2_CHEAT 0x0069bdf8
br_actor* gPed_limbed_actors[30];

// GLOBAL: CARMA2_HW 0x006942f8
// GLOBAL: CARMA2_SW 0x0068e350
// GLOBAL: CARMA2_CHEAT 0x0069be70
tShrapnel_time_info gPed_spurt_min_max_time;

// GLOBAL: CARMA2_HW 0x006940c4
// GLOBAL: CARMA2_SW 0x0068e11c
// GLOBAL: CARMA2_CHEAT 0x0069bc3c
void* gPhysics_other_buffer;

// GLOBAL: CARMA2_HW 0x006940bc
// GLOBAL: CARMA2_SW 0x0068e114
void* gPhysics_scratch_ptr;

// GLOBAL: CARMA2_HW 0x006940b0
// GLOBAL: CARMA2_SW 0x0068e108
// GLOBAL: CARMA2_CHEAT 0x0069bc28
int gFace_count;

// GLOBAL: CARMA2_HW 0x0065cf78
// GLOBAL: CARMA2_SW 0x006578b0
// GLOBAL: CARMA2_CHEAT 0x00664570
tWorld_callbacks gWorld_callbacks = {
    ProcessForcesCallback,
    ProcessJointForcesCallBack,
    NewFaceListCallBack,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
};

// HierarchyCalcEnergy

// CheckCollisionList

// MakeJointVelocitiesVector

// ApplySingleForce

// ApplyJointForces

// SkipForces

// ScaleUpObjectMI

// CheckForTwang

// FUNCTION: CARMA2_HW 0x004b8c20
void C2_HOOK_FASTCALL CreateJointForce(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0x0, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_a, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, object_b, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_a, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, point_b, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_a, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, direction_b, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint_force, type, 0x5c);

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    pForce->field_0xc = NULL;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, pDirection, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 10;
}

// FUNCTION: CARMA2_HW 0x004b8e10
void C2_HOOK_FASTCALL CreateJointTorque(tPhysics_object* pObject, tPhysics_object* pPartner, tPhysics_joint_force* pForce, const br_vector3* pDirection) {
    br_vector3 direction;

    pForce->field_0x0 = 0.f;
    pForce->object_a = pObject;
    pForce->object_b = pPartner;
    BrVector3Sub(&pForce->point_a, &pObject->physics_joint1->field_0x08, &pObject->cmpos);
    BrVector3Sub(&pForce->point_b, &pObject->physics_joint1->field_0x14, &pPartner->cmpos);
    BrVector3Copy(&pForce->direction_a, pDirection);
    BrMatrix34ApplyV(&direction, &pForce->direction_a, &pObject->transform_matrix);
    BrMatrix34TApplyV(&pForce->direction_b, &direction, &pPartner->transform_matrix);
    pForce->type = 11;
}

// AllocateObjectWorkspace

// RedoQuickHingeVelocities

// GetJointForces

// GetUniversalRotationAngle

// GetObjectsParent

// GetJointLimits

// GetListJointLimits

// GetGroupJointLimits

// LimitOmega

// FUNCTION: CARMA2_HW 0x004c1840
// FUNCTION: CARMA2_SW 0x004bdf70
void C2_HOOK_FASTCALL CapOmega(tPhysics_object* pObject) {
    br_scalar ts = BrVector3LengthSquared(&pObject->omega) * REC2_SQR(0.04f);
    if (ts > (br_scalar)REC2_SQR(1.57)) {
        ts = (br_scalar)sqrt(ts) * (2.0f / 3.14f);
        pObject->omega.v[0] = pObject->omega.v[0] / (2.f * ts);
        pObject->omega.v[1] = pObject->omega.v[1] / (2.f * ts);
        pObject->omega.v[2] = pObject->omega.v[2] / (2.f * ts);
    }
}

// ListLimitOmega

// GroupLimitOmega

// FUNCTION: CARMA2_HW 0x004b8fe0
// FUNCTION: CARMA2_SW 0x004b5700
void C2_HOOK_FASTCALL PositionWorldJointedObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    br_vector3 axis;
    br_vector3 tv;
    br_scalar ts;

    if (pObject->physics_joint2->type == eJoint_translation) {
        BrMatrix34ApplyV(&axis, &pObject->physics_joint2->hinge_axis, &pObject->transform_matrix);
        ts = BrVector3Dot(&pObject->actor->t.t.translate.t, &axis);
        BrMatrix34ApplyP(&tv, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        BrVector3Sub(&tv, &pObject->field_0x17c, &tv);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
        ts -= BrVector3Dot(&pObject->actor->t.t.translate.t, &axis);
        BrVector3Scale(&tv, &axis, ts);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
        ts = BrVector3Dot(&pObject->v, &axis);
        BrVector3Scale(&pObject->v, &axis, ts);
        TranslateObject(pObject, pDelta_time);
    } else {
        BrMatrix34ApplyP(&tv, &pObject->physics_joint2->field_0x08, &pObject->actor->t.t.mat);
        BrVector3Sub(&tv, &pObject->field_0x17c, &tv);
        BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
    }
}

// TranslateAndRotatePartialObject

// TranslateAndRotateJointedObject

// CountJointForces

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b7510
// STUB: CARMA2_SW 0x004b3c30
// STUB: CARMA2_CHEAT 0x004bd040
void C2_HOOK_FASTCALL MoveJointedObject(tPhysics_object* pObject, float pDelta_time) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004b9750
// FUNCTION: CARMA2_SW 0x004b5e70
// FUNCTION: CARMA2_CHEAT 0x004bf280
void C2_HOOK_FASTCALL TellPhysicsJointsHaveChanged(tPhysics_object* pCollision_info) {

    pCollision_info->field_0x218 = 0;
    MoveJointedObject(pCollision_info, 0.0f);
    gINT_00692dc8 = 1;
}

// CombineBounds

// GetWorldBoundingBoxForObjects

// GetWorldBoundingBoxForObject

// MatricesAreIdentical

// InternalPrepareObject

// PrepareObject

// PrepareForCollisions

// CheckPointVelocities

// FUNCTION: CARMA2_HW 0x004c2a10
// FUNCTION: CARMA2_SW 0x004bf140
void C2_HOOK_FASTCALL WriteNewPositions(tPhysics_object* pObject) {
    tPhysics_object* child;
    tPhysics_object* root;
    br_vector3 parent_point;
    br_vector3 child_point;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1e8, 0x1e8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x230, 0x230);

    root = pObject;
    child = pObject->child;
    if (pObject->flags & 0x400) {
        root = pObject->field_0x230;
    }
    for (; child != NULL; child = child->next) {
        if (!(pObject->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pObject->actor->t.t.mat);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->actor->t.t.mat);
            BrVector3Sub(&parent_point, &parent_point, &child_point);
            BrVector3Accumulate((br_vector3*)child->actor->t.t.mat.m[3], &parent_point);
        } else {
            BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->field_0x1e8, &root->actor->t.t.mat);
        }
        WriteNewPositions(child);
    }
}

// FUNCTION: CARMA2_HW 0x004b9eb0
// FUNCTION: CARMA2_SW 0x004b65d0
// FUNCTION: CARMA2_CHEAT 0x004bf9e0
void C2_HOOK_FASTCALL SetListDoingNothingFlag(tPhysics_object* pObject, int pFlag) {
    while (pObject != NULL) {
        pObject->disable_move_rotate = pFlag;
        pObject->field_0x1df = 0;
        if (pObject->child != NULL) {
            SetListDoingNothingFlag(pObject->child, pFlag);
        }
        pObject = pObject->next;
    }
}

// SetGroupDoingNothingFlag

// FUNCTION: CARMA2_HW 0x004b9ef0
// FUNCTION: CARMA2_SW 0x004b6610
// FUNCTION: CARMA2_CHEAT 0x004bfa20
void C2_HOOK_FASTCALL SetHierarchyDoingNothingFlag(tPhysics_object* pObject, int pFlag) {
    pObject->disable_move_rotate = pFlag;
    pObject->field_0x1df = 0;
    if (pObject->child != NULL) {
        SetListDoingNothingFlag(pObject->child, pFlag);
    }
}

// CheckDoingNothingFlag

// FUNCTION: CARMA2_HW 0x004bae80
// FUNCTION: CARMA2_SW 0x004b75a0
void C2_HOOK_FASTCALL SetGroup(tPhysics_object* pObject, int pGroup) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->field_0x1dc = (tU8)pGroup;
    for (; child != NULL; child = child->next) {
        if (child->physics_joint1 != NULL && child->physics_joint1->type != eJoint_none) {
            SetGroup(child, pGroup);
        }
    }
}

// GroupTogether

// GetPreCalculatedJointForces

// FUNCTION: CARMA2_HW 0x004b9f40
// FUNCTION: CARMA2_SW 0x004b6660
// FUNCTION: CARMA2_CHEAT 0x004bfa70
void C2_HOOK_FASTCALL MakeObjectListDoSomething(tPhysics_object* pObject) {
    while (pObject != NULL) {
        if (pObject->disable_move_rotate) {
            pObject->box_face_ref = gFace_num__car - 2;
            pObject->disable_move_rotate = 0;
        }
        if (pObject->child != NULL) {
            MakeObjectListDoSomething(pObject->child);
        }
        pObject = pObject->next;
    }
}

// CheckForObjectTouchingAnotherObject

// CheckForObjectListTouchingAnotherObject

// CheckForObjectHierachyTouchingAnotherObject

// CheckForObjectHierachyTouchingObjectList

// MakeObjectDoSomething

// GetReducedList

// ReduceReducedList

// FUNCTION: CARMA2_HW 0x004bd9b0
// FUNCTION: CARMA2_SW 0x004ba0d0
void C2_HOOK_FASTCALL SetObjectTrialPositionType(tPhysics_object* pObject, int pType) {
    tPhysics_object* child;

    child = pObject->child;
    pObject->field_0xf0 = pType;
    for (; child != NULL; child = child->next) {
        SetObjectTrialPositionType(child, pType);
    }
}

// FillInTrialWorkspace

// FUNCTION: CARMA2_HW 0x004ba4f0
// FUNCTION: CARMA2_SW 0x004b6c10
// FUNCTION: CARMA2_CHEAT 0x004c0020
int C2_HOOK_FASTCALL FlagsAreGo(tPhysics_object* pCollision_1, tPhysics_object* pCollision_2) {
    int blocked;

    blocked = (pCollision_1->field_0x1a0 & pCollision_2->field_0x1a4)
        || (pCollision_2->field_0x1a0 & pCollision_1->field_0x1a4)
        || (pCollision_1->flags & 4)
        || (pCollision_2->flags & 4);
    return !blocked;
}

// CollideObjectWithReducedObjectList

// AddObjectToReducedList

// ReducedFindObjectCollisions

// FindObjectCollisions

// DoRigidJoints

// DoInterGroupCollisions

// FUNCTION: CARMA2_HW 0x004ba550
// FUNCTION: CARMA2_SW 0x004b6c70
// FUNCTION: CARMA2_CHEAT 0x004c0080
int C2_HOOK_FASTCALL ReduceEffectOfForces(tPhysics_joint_force* pForces, br_scalar* pImpulses, int pCount, const int* pIndices, int pIndex_count, br_scalar pThreshold) {
    int i;
    br_scalar total;

    total = 0.0f;
    for (i = 0; i < pIndex_count; i++) {
        total += pImpulses[pIndices[i]];
    }
    if (total <= pThreshold) {
        return 0;
    }
    pThreshold /= total;
    gINT_00692dc8 = 1;
    for (i = 0; i < pCount; i++) {
        pImpulses[i] = -pImpulses[i];
    }
    for (i = 0; i < pIndex_count; i++) {
        pImpulses[pIndices[i]] *= -pThreshold;
        if (pForces[pIndices[i]].type & 0x100) {
            BrVector3Scale(&pForces[pIndices[i]].field_0x44, &pForces[pIndices[i]].field_0x44,pThreshold);
            if (pForces[pIndices[i]].object_b != NULL) {
                BrVector3Scale(&pForces[pIndices[i]].field_0x44, &pForces[pIndices[i]].field_0x44, pThreshold);
            }
        }
    }
    for (i = 0; i < pCount; i++) {
        if (pImpulses[i] < 0.0f) {
            pImpulses[i] = 0.0f;
        }
        pForces[i].type = 9;
    }
    return 1;
    // }
    // return 0;
}

// DoCollisions

// AddDoubleTorqueToMatrix

// DRMatrix33Inverse

// SetUpQuickHingeData

// InitQuickHinge

// CalcQuickHingeData

// FUNCTION: CARMA2_HW 0x004be4b0
// FUNCTION: CARMA2_SW 0x004babd0
void C2_HOOK_FASTCALL SetUpForce(br_vector3* pAngular, br_vector3* pLinear, const br_vector3* pPoint, const br_vector3* pDirection, tPhysics_object* pObject) {
    br_scalar inverse_mass;
    int i;

    if (!(pObject->flags & 0x800)) {
        BrVector3Cross(pAngular, pPoint, pDirection);
        for (i = 0; i < 3; i++) {
            pAngular->v[i] /= pObject->I.v[i];
        }
    } else {
        BrVector3Set(pAngular, 0.0f, 0.0f, 0.f);
    }
    inverse_mass = (br_scalar)(1.0 / pObject->M);
    BrVector3Scale(pLinear, pDirection, inverse_mass);
}

// SetUpTorqueHingePartners

// SetUpHingePartners

// MakeMatrix

// FUNCTION: CARMA2_HW 0x004c03f0
void C2_HOOK_FASTCALL RestoreListOmega(tPhysics_object* pObject) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, rotate_omega, 0x1c0);

    while (pObject != NULL) {
        BrVector3Copy(&pObject->omega, &pObject->rotate_omega);
        if (pObject->child != NULL) {
            RestoreListOmega(pObject->child);
        }
        pObject = pObject->next;
    }
}

// RestoreGroupOmegas

// MakeVelocitiesVector2

// MakeVelocitiesVector

// NewAddFrictionObjectObject

// NewAddFriction

// ApplyForces

// DoJointFriction

// ListDoJointFriction

// PostAddFriction

// RotateObjectSecondOrder

// FUNCTION: CARMA2_HW 0x004c1f10
void C2_HOOK_FASTCALL RotateObjectFirstOrder(tPhysics_object* pObject, br_scalar pTime) {
    br_matrix34 rotation;
    br_vector3 axis;
    br_vector3 momentum;
    br_vector3 rotated;
    br_scalar length;
    br_scalar angle;

    length = BrVector3Length(&pObject->omega);
    angle = length * pTime;
    if (angle < 0.0001) {
        return;
    }
    axis.v[0] = pObject->omega.v[0] / length;
    axis.v[1] = pObject->omega.v[1] / length;
    axis.v[2] = pObject->omega.v[2] / length;
    momentum.v[0] = pObject->I.v[0] * pObject->omega.v[0];
    momentum.v[1] = pObject->I.v[1] * pObject->omega.v[1];
    momentum.v[2] = pObject->I.v[2] * pObject->omega.v[2];
    BrMatrix34Rotate(&rotation, BrRadianToAngle(angle), &axis);
    BrMatrix34TApplyV(&rotated, &momentum, &rotation);
    BrMatrix34PreTranslate(&rotation, -pObject->cmpos.v[0], -pObject->cmpos.v[1], -pObject->cmpos.v[2]);
    BrMatrix34PostTranslate(&rotation, pObject->cmpos.v[0], pObject->cmpos.v[1], pObject->cmpos.v[2]);
    BrMatrix34Pre(&pObject->actor->t.t.mat, &rotation);
    pObject->omega.v[0] = rotated.v[0] / pObject->I.v[0];
    pObject->omega.v[1] = rotated.v[1] / pObject->I.v[1];
    pObject->omega.v[2] = rotated.v[2] / pObject->I.v[2];
}

// RotateObject

void C2_HOOK_FASTCALL TranslateObject(tPhysics_object* pObject, br_scalar pDelta_time) {
    br_vector3 tv;

    BrVector3Scale(&tv, &pObject->v, pDelta_time);
    BrVector3Accumulate(&pObject->actor->t.t.translate.t, &tv);
}

// STUB: CARMA2_HW 0x004c2060
// STUB: CARMA2_SW 0x004be790
void C2_HOOK_FASTCALL GetFacesNearObject(tPhysics_object *pObject, tWorld_callbacks *pCallbacks) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c25a0
// FUNCTION: CARMA2_SW 0x004becd0
void C2_HOOK_FASTCALL ResetObjectAndChildren(tPhysics_object* pObject) {
    tPhysics_object* child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xbc, 0xbc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xf0, 0xf0);

    if (pObject->field_0xf0 == 1) {
        BrMatrix34Copy(&pObject->actor->t.t.mat, &pObject->field_0xbc);
    } else {
        BrMatrix34LPNormalise(&pObject->actor->t.t.mat, &pObject->transform_matrix);
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        ResetObjectAndChildren(child);
    }
}

// FUNCTION: CARMA2_HW 0x004c2600
// FUNCTION: CARMA2_SW 0x004bed30
void C2_HOOK_FASTCALL ResetObjectList(tPhysics_object* pObjects) {

    while (pObjects != NULL) {
        ResetObjectAndChildren(pObjects);
        pObjects = pObjects->next;
    }
}

// RepositionObjectGroup

// SimpleRotate

// InterpolateSingleObject

// InterpolateObjects

// FUNCTION: CARMA2_HW 0x004c2910
// FUNCTION: CARMA2_SW 0x004bf040
void C2_HOOK_FASTCALL ApplyGravitySingleObject(tPhysics_object* pObject) {

    if (!pObject->disable_move_rotate) {
        BrVector3Accumulate(&pObject->v, &pObject->field_0x54);
    }
    if (pObject->child != NULL) {
        ApplyGravity(pObject->child);
    }
}

// FUNCTION: CARMA2_HW 0x004c2970
// FUNCTION: CARMA2_SW 0x004bf0a0
void C2_HOOK_FASTCALL ApplyGravity(tPhysics_object* pObject) {

    for (; pObject != NULL; pObject = pObject->next) {
        ApplyGravitySingleObject(pObject);
    }
}

// FUNCTION: CARMA2_HW 0x004c29d0
// FUNCTION: CARMA2_SW 0x004bf100
void C2_HOOK_FASTCALL SetGravityDeltaV(tPhysics_object* pObject, br_vector3* pImpulse) {
    tPhysics_object* pChild;

    pObject->field_0x54 = *pImpulse;
    for (pChild = pObject->child; pChild != NULL; pChild = pChild->next) {
        SetGravityDeltaV(pChild, pImpulse);
    }
}

// TrialPositionChildren

// FUNCTION: CARMA2_HW 0x004c2b20
// FUNCTION: CARMA2_SW 0x004bf250
// FUNCTION: CARMA2_CHEAT 0x004c8640
void C2_HOOK_FASTCALL InternalPositionChildren(tPhysics_object* pParent, tPhysics_object* pRoot) {
    tPhysics_object* child;
    br_vector3 parent_point;
    br_vector3 child_point;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x1e8, 0x1e8);

    child = pParent->child;
    if (!(pParent->flags & 0x400)) {
        pRoot = pParent;
    }
    for (; child != NULL; child = child->next) {
        if (!(pParent->flags & 0x400)) {
            BrMatrix34ApplyP(&parent_point, &child->physics_joint1->field_0x14, &pParent->transform_matrix);
            BrMatrix34ApplyP(&child_point, &child->physics_joint1->field_0x08, &child->transform_matrix);
            BrVector3Sub(&parent_point, &parent_point, &child_point);
            BrVector3Accumulate((br_vector3*)child->transform_matrix.m[3], &parent_point);
        } else {
            BrMatrix34Mul(&pParent->transform_matrix, &pParent->field_0x1e8, &pRoot->transform_matrix);
        }
        InternalPositionChildren(child, pRoot);
    }
}

// FUNCTION: CARMA2_HW 0x004c2b10
// FUNCTION: CARMA2_SW 0x004bf240
// FUNCTION: CARMA2_CHEAT 0x004c8630
void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info) {

    InternalPositionChildren(pCollision_info, pCollision_info);
}

// FUNCTION: CARMA2_HW 0x004c2c30
// FUNCTION: CARMA2_SW 0x004bf360
void C2_HOOK_FASTCALL ApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque) {
    br_vector3 torque;
    br_vector3 inertia;

    BrVector3Scale(&torque, pTorque, 1.0f / REC2_SQR(WORLD_SCALE));
    BrVector3Copy(&inertia, &pObject->I);
    pObject->omega.v[0] += torque.v[0] / inertia.v[0];
    pObject->omega.v[1] += torque.v[1] / inertia.v[1];
    pObject->omega.v[2] += torque.v[2] / inertia.v[2];
}

// FUNCTION: CARMA2_HW 0x004c2c90
// FUNCTION: CARMA2_SW 0x004bf3c0
// FUNCTION: CARMA2_CHEAT 0x004c87b0
void C2_HOOK_FASTCALL ApplyDiminishingTorque(tPhysics_object* pObject, const br_vector3* pV) {
    br_vector3 original_omega;
    br_vector3 tv;
    br_vector3 inertia;

    original_omega = pObject->omega;
    BrVector3Scale(&tv, pV, (br_scalar)(1.0 / REC2_SQR(WORLD_SCALE_D)));
    BrVector3Copy(&inertia, &pObject->I);
    pObject->omega.v[0] -= tv.v[0] / inertia.v[0];
    pObject->omega.v[1] -= tv.v[1] / inertia.v[1];
    pObject->omega.v[2] -= tv.v[2] / inertia.v[2];
    if (original_omega.v[0] * pObject->omega.v[0] <= 0.0f) {
        pObject->omega.v[0] = 0.0f;
    }
    if (original_omega.v[1] * pObject->omega.v[1] <= 0.0f) {
        pObject->omega.v[1] = 0.0f;
    }
    if (original_omega.v[2] * pObject->omega.v[2] <= 0.0f) {
        pObject->omega.v[2] = 0.0f;
    }
}

// OrderObjectListOnX

// SwapPair

// CheckObjectsPostionInList

// TestForWrongAnswer

// RQIterativeImprovement

// RQExpand

// RQSingleExpand

// FUNCTION: CARMA2_HW 0x004c5830
// FUNCTION: CARMA2_SW 0x004c1f60
int C2_HOOK_FASTCALL RQDcmpV(br_scalar* pMatrix, int pDim, br_scalar* pQ) {
    int i;
    int j;
    int k;
    int singular;
    br_scalar scale;
    br_scalar sum;
    br_scalar norm;
    br_scalar divisor;
    br_scalar matrix_sum;
    br_scalar orthogonal_sum;
    br_scalar tau;
    br_scalar tau2;
    br_scalar* row;

    /* Using householder transformations, apply RQ decomposition on pMatrix:
     * - pMatrix is in-place converted into a lower-triangular matrix
     * - pQ is a orthonormal matrix */

    singular = 0;

    /* pQ starts as a identity matrix */
    for (i = 0; i < pDim; i++) {
        for (j = 0; j < pDim; j++) {
            pQ[i * pDim + j] = 0.0f;
        }
        pQ[i * pDim + i] = 1.0f;
    }

    for (i = 0; i < pDim; i++) {
        // Rotate i-th upper-diagonal row onto base vector

        row = &pMatrix[i * pDim];
        scale = 0.0f;
        for (j = i; j < pDim; j++) {
            scale = (br_scalar)MAX(scale, fabs(row[j]));
        }
        if (scale == 0.0) {
            singular = 1;
            continue;
        }
        // Only normalize the upper-vector that must be rotated onto the base vector
        // This is done for numerical stability reasons.
        for (j = i; j < pDim; j++) {
            row[j] /= scale;
        }
        // Calculate norm of the upper-diagonal row
        sum = 0.0f;
        for (j = i; j < pDim; j++) {
            sum += row[j] * row[j];
        }
        // Apply Householder reflection on pQ and the row+matrix
        norm = (br_scalar)(row[i] > 0.0 ? fabs(sqrt(sum)) : -fabs(sqrt(sum)));
        divisor = norm * (norm + row[i]);
        row[i] += norm;
        for (j = 0; j <= i; j++) {
            sum = 0.0f;
            for (k = i; k < pDim; k++) {
                sum += row[k] * pQ[k * pDim + j];
            }
            tau = sum / divisor;
            for (k = i; k < pDim; k++) {
                pQ[k * pDim + j] -= tau * row[k];
            }
        }
        for (j = i + 1; j < pDim; j++) {
            matrix_sum = 0.0f;
            for (k = i; k < pDim; k++) {
                matrix_sum += pMatrix[j * pDim + k] * row[k];
            }
            tau2 = matrix_sum / divisor;
            orthogonal_sum = 0.0f;
            for (k = i; k < pDim; k++) {
                orthogonal_sum += pQ[k * pDim + j] * pMatrix[i * pDim + k];
            }
            tau = orthogonal_sum / divisor;
            for (k = i; k < pDim; k++) {
                pMatrix[j * pDim + k] -= tau2 * row[k];
                pQ[k * pDim + j] -= tau * row[k];
            }
        }
        row[i] = -(norm * scale);
        // Zero upper-dialog values so what remains is lower-diagonal
        for (j = i + 1; j < pDim; j++) {
            row[j] = 0.0f;
        }
    }
    return singular;
}

// MakeIdentity

// SparseRQDcmpV

// MakeMatrixFromIndices

// HasBeenDoneBefore

// RemoveRQSingularities

// AddToMatrix

// RQRemovedOneTooManyForces

// GetInverseM

// RQMatrixSolveNoNegatives

// RQMatrixSolveWithNegatives

// FUNCTION: CARMA2_HW 0x004c5b50
// FUNCTION: CARMA2_SW 0x004c2280
int C2_HOOK_FASTCALL MatrixProcess(br_scalar* pR, br_scalar* pQ, int pDim) {
    return RQDcmpV(pR, pDim, pQ);
}

// FUNCTION: CARMA2_HW 0x004c5b60
void C2_HOOK_FASTCALL RqBackSub(const br_scalar* pR, const br_scalar* pQ, const br_scalar* pRhs, br_scalar* pX, int pDim) {
    br_scalar intermediate[100];
    br_scalar sum;
    int i;
    int j;

    /* Solve Ax = b, with A represented by its RQ decomposition, where:
     * - R is a lower-diagonal matrix
     * - Q is an orthonormal matrix
     * with:
     * - all matrices (A, Q, R) are square of size N x N (with N = pDim)
     * - vectors x, t are of size N x 1
     * A, Q, R matrices are of size [pDim x pDim]
     */

    C2_HOOK_ASSERT(pDim > 0 && pDim <= (int)REC2_ASIZE(intermediate));

    /* Use back substitution to solve Ry = b, with R a lower-diagonal matrix */
    intermediate[0] = pRhs[0] / pR[0 * pDim + 0];             /*  T[0] = B[0] / R[0, 0] */
    for (i = 1; i < pDim; i++) {
        sum = 0.0f;
        for (j = 0; j < i; j++) {
            sum += intermediate[j] * pR[i * pDim + j];        /*  sum += T[j] * R[i, j] */
        }
        intermediate[i] = (pRhs[i] - sum) / pR[i * pDim + i]; /*  T[i] = (B[i] - sum) / R[i, i] */
    }
    /* Solve Qx = T, by multiplying both sides with Q transposed */
    for (i = 0; i < pDim; i++) {
        pX[i] = pQ[0 * pDim + i] * intermediate[0];           /*  X[i]  = Q[0, i] * T[0] */
        for (j = 1; j < pDim; j++) {
            pX[i] += intermediate[j] * pQ[j * pDim + i];      /*  X[i] += Q[j, i] * T[j] */
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b57d0
// FUNCTION: CARMA2_SW 0x004b1ef0
// FUNCTION: CARMA2_CHEAT 0x004bb0a0
int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks) {
    tU8 cx_min;
    tU8 cz_min;
    tU8 cx_max;
    tU8 cz_max;
    int x;
    int z;
    int emitted;
    tTrack_spec* track_spec;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_x, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tTrack_spec, ncolumns_z, 0x1);

    emitted = 0;
    track_spec = &gProgram_state.track_spec;
    FillInBounds(pBounds);
    XZToColumnXZ(&cx_min, &cz_min, pBounds->real_bounds.min.v[0], pBounds->real_bounds.min.v[2], track_spec);
    XZToColumnXZ(&cx_max, &cz_max, pBounds->real_bounds.max.v[0], pBounds->real_bounds.max.v[2], track_spec);
    if (cx_min > 0) {
        cx_min--;
    }
    if (cz_min > 0) {
        cz_min--;
    }
    if (cx_max + 1 < track_spec->ncolumns_x) {
        cx_max++;
    }
    if (cz_max + 1 < track_spec->ncolumns_z) {
        cz_max++;
    }
    for (x = cx_min; x <= cx_max; x++) {
        for (z = cz_min; z <= cz_max; z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                emitted = pMax_face - ActorBoxPick(pBounds, track_spec->columns[z][x].actor_0x0, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
            }
        }
    }
    if (gAdditional_actors != NULL) {
        emitted = pMax_face - ActorBoxPick(pBounds, gAdditional_actors, NULL, gBlack_material, &pFace_list[emitted], pMax_face - emitted, NULL, pWorld_callbacks);
    }
    return emitted;
}

// FUNCTION: CARMA2_HW 0x004b5970
float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace) {
    int material_index;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCurrent_race.material_modifiers) != 11);

    material_index = pFace->material->identifier[0] - '/';
    if (material_index < 0 || material_index >= (int)REC2_ASIZE(gCurrent_race.material_modifiers)) {
        material_index = 0;
    }
    return gCurrent_race.material_modifiers[material_index].car_wall_friction;
}

// FUNCTION: CARMA2_HW 0x004b5990
// FUNCTION: CARMA2_SW 0x004b20b0
// FUNCTION: CARMA2_CHEAT 0x004bb260
void C2_HOOK_FASTCALL DoPhysicsError(tPhysicsError pError, const char* pMessage) {
    char s[256];

    (void)pMessage;
    switch (pError) {
    case ePhysicsError_UnknownShapeType:
        strcpy(s, "Unknown shape type");
        break;
    case ePhysicsError_WireFrameHasNoPoints:
        strcpy(s, "Wire frame shape has no points");
        break;
    case ePhysicsError_PolyhedronHasNoPoints:
        strcpy(s, "Polyhedron shape has no points");
        break;
    case ePhysicsError_UnknownHingeType:
        strcpy(s, "Unknown hinge type");
        /* The original overwrites this message with the code. */
#ifndef REC2_MATCHING
        break;
#endif
        /* fall through */
    default:
        sprintf(s, "%d ", pError);
        break;
    }
    FatalError(1000 + pError, s);
}

// ResetMan2

// FUNCTION: CARMA2_HW 0x004b5aa0
// FUNCTION: CARMA2_CHEAT 0x004bb370
void C2_HOOK_FASTCALL SetUpTestObjects(void) {

}

// ResetObjectList

// ResetMan

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x004b5ca0
// FUNCTION: CARMA2_SW 0x004b23c0
// FUNCTION: CARMA2_CHEAT 0x004bb570
void C2_HOOK_FASTCALL InitPhysics(void) {

    PhysicsSetErrorHandler(DoPhysicsError);
    InitPhysicsWorkspace(gTemporary_physics_render_buffer, sizeof(gTemporary_physics_render_buffer));
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// PointOutOfSightNotAR (moved to 51-smash for inlining purposes)

// CalcCollObjIDFromCarSpec

// CalcCollObjID

// GetCarSpecFromCollObjID

// GetCollObjFromID

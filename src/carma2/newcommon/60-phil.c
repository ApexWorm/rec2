/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "60-phil.h"

#include "00-car.h"
#include "18-graphics2.h"
#include "49-physics.h"
#include "55-volume.h"
#include "rec2_macros.h"

#include <stdarg.h>

// GLOBAL: CARMA2_HW 0x0065d004
// GLOBAL: CARMA2_SW 0x0065793c
// GLOBAL: CARMA2_CHEAT 0x006645fc
int gPHIL_disabled = 1;

// GLOBAL: CARMA2_HW 0x0074a5f0
// GLOBAL: CARMA2_SW 0x007410b0
// GLOBAL: CARMA2_CHEAT 0x00751c90
tPhysics_object* gPHIL_list_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5ec
// GLOBAL: CARMA2_SW 0x007410ac
// GLOBAL: CARMA2_CHEAT 0x00751c8c
tU32 gPHIL_last_physics_tick;

// GLOBAL: CARMA2_HW 0x006923e0
// GLOBAL: CARMA2_SW 0x0068c438
int gPHIL_count_list_collision_infos;

// GLOBAL: CARMA2_HW 0x006923e4
// GLOBAL: CARMA2_SW 0x0068c43c
// GLOBAL: CARMA2_CHEAT 0x00699dcc
int gPHIL_object_added;

// GLOBAL: CARMA2_HW 0x006923f0
// GLOBAL: CARMA2_SW 0x0068c448
tPHIL_queued_objects gPhil_queued_objects = { 0 };

// GLOBAL: CARMA2_HW 0x00692db8
// GLOBAL: CARMA2_SW 0x0068ce10
tWorld_callback_active_passive_cbfn* gPHIL_original_activate_passive = NULL;

// GLOBAL: CARMA2_HW 0x00692dbc
// GLOBAL: CARMA2_SW 0x0068ce14
int gPHIL_count_queued_objects_for_removal;

// GLOBAL: CARMA2_HW 0x00692dc0
// GLOBAL: CARMA2_SW 0x0068ce18
int gPHIL_count_queued_objects;

// FUNCTION: CARMA2_HW 0x0049c5e0
// FUNCTION: CARMA2_SW 0x00498760
// FUNCTION: CARMA2_CHEAT 0x0049c990
tU16 C2_HOOK_FASTCALL ScalarToFraction(br_scalar pValue, br_scalar pMin, br_scalar pMax) {

    if (pValue > pMax) {
        pValue = pMax;
    }
    if (pValue < pMin) {
        pValue = pMin;
    }
    pValue = ((pValue - pMin) / (pMax - pMin)) * 65534.0f;
    return (tU16)(pValue + 0.5f);
}

br_scalar C2_HOOK_FASTCALL FractionToScalar(tU16 pFixed, br_scalar pMin, br_scalar pMax) {

    return ((pMax - pMin) * (((br_scalar)pFixed / 65534.0f))) + pMin;
}

// FUNCTION: CARMA2_HW 0x0049be90
// FUNCTION: CARMA2_SW 0x00498010
// FUNCTION: CARMA2_CHEAT 0x0049c240
void C2_HOOK_FASTCALL GetOrientationFromMatrix(tFixed_quat* pOrientation, const br_matrix34* pMatrix) {
    br_quat quaternion;

    BrMatrix34ToQuat(&quaternion, pMatrix);

    pOrientation->v[0] = ScalarToFraction(quaternion.x, -1.0f, 1.0f);
    pOrientation->v[1] = ScalarToFraction(quaternion.y, -1.0f, 1.0f);
    pOrientation->v[2] = ScalarToFraction(quaternion.z, -1.0f, 1.0f);
    pOrientation->v[3] = ScalarToFraction(quaternion.w, -1.0f, 1.0f);
}

// FUNCTION: CARMA2_HW 0x0049c000
// FUNCTION: CARMA2_SW 0x00498180
// FUNCTION: CARMA2_CHEAT 0x0049c3b0
void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tFixed_quat* pOrientation) {
    br_quat quaternion;

    quaternion.x = FractionToScalar(pOrientation->v[0], -1.0f, 1.0f);
    quaternion.y = FractionToScalar(pOrientation->v[1], -1.0f, 1.0f);
    quaternion.z = FractionToScalar(pOrientation->v[2], -1.0f, 1.0f);
    quaternion.w = FractionToScalar(pOrientation->v[3], -1.0f, 1.0f);
    BrQuatNormalise(&quaternion, &quaternion);
    BrQuatToMatrix34(pMatrix, &quaternion);
}

tU16 C2_HOOK_FASTCALL OmegaCompScalarTo16(br_scalar pOmega) {
    /* Range is [-12.5 * PI, 12.5 * PI] */
    return ScalarToFraction(pOmega, -39.269908f, 39.269908f);
}

// OmegaComp16ToScalar

void C2_HOOK_FASTCALL GetOmega16FromOmega(tFixed_vec3* pOmega16, const br_vector3* pOmega) {
    int i;

    for (i = 0; i < 3; i++) {
        pOmega16->v[i] = OmegaCompScalarTo16(pOmega->v[i]);
    }
}

// GetOmegaFromOmega16

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0049c0d0
// FUNCTION: CARMA2_SW 0x00498250
// FUNCTION: CARMA2_CHEAT 0x0049c480
int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, tU32 pSize) {
    tObject_network_stuff* stuff = (tObject_network_stuff*)pBuffer;

    C2_HOOK_BUG_ON(sizeof(stuff->hinge) != 0xc);
    C2_HOOK_BUG_ON(sizeof(stuff->universal) != 0x10);
    C2_HOOK_BUG_ON(sizeof(stuff->none) != 0x28);

    if (pObject->physics_joint1 != NULL) {
        switch (pObject->physics_joint1->type) {
        case eJoint_hinge:
        case eJoint_quick_hinge:
            if (pSize < sizeof(stuff->hinge)) {
                return -1;
            }
            stuff->hinge.type = (tU8)pObject->physics_joint1->type;
            stuff->hinge.uid = pObject->uid;
            stuff->hinge.rotation = OmegaCompScalarTo16(BrVector3Dot(&pObject->omega, &pObject->physics_joint1->hinge_axis));
            GetOrientationFromMatrix(&stuff->hinge.orientation, &pObject->transform_matrix);
            return sizeof(stuff->hinge);
        case eJoint_universal:
        case eJoint_ball_n_socket:
            if (pSize < sizeof(stuff->universal)) {
                return -1;
            }
            stuff->universal.type = (tU8)pObject->physics_joint1->type;
            stuff->universal.uid = pObject->uid;
            GetOmega16FromOmega(&stuff->universal.omega, &pObject->omega);
            GetOrientationFromMatrix(&stuff->universal.orientation, &pObject->transform_matrix);
            return sizeof(stuff->universal);
        case eJoint_none:
            break;
        default:
            PhysicsError(ePhysicsError_error9, "netphys got messed up");
            break;
        }
    }
    if (pSize < sizeof(stuff->none)) {
        return -1;
    }
    stuff->none.type = eJoint_none;
    stuff->none.uid = pObject->uid;
    GetOmega16FromOmega(&stuff->none.omega, &pObject->omega);
    BrVector3Copy(&stuff->none.v, &pObject->v);
    BrVector3Copy(&stuff->none.pos, (br_vector3*)pObject->transform_matrix.m[3]);
    GetOrientationFromMatrix(&stuff->none.orientation, &pObject->transform_matrix);
    return sizeof(stuff->none);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetVelocityAndPosition

int C2_HOOK_FASTCALL SizeOfObjectNetworkStuff(tPhysics_joint_type pType) {

    // FIXME: add structs and use sizeof(struct) + add static assertion making sure sizeof(struct) == {0x28,0xc,0x10}
    switch (pType & 0xf) {
    case eJoint_none:
        return 0x28;
    case eJoint_hinge:
    case eJoint_quick_hinge:
        return 0xc;
    case eJoint_universal:
    case eJoint_ball_n_socket:
        return 0x10;
    default:
        PhysicsError(ePhysicsError_error9, "netphys got messed up");
        return 0;
    }
}

// WriteObjectNetworkStuff

// FUNCTION: CARMA2_HW 0x0049cd00
// FUNCTION: CARMA2_SW 0x00498e80
// FUNCTION: CARMA2_CHEAT 0x0049d0b0
int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize) {
    int size;
    int child_size;
    tPhysics_object* child;
    tU8* child_buffer;

    size = GetObjectNetworkStuff(pObject, pBuffer, pSize);
    if (size < 0) {
        return -1;
    }
    child = pObject->child;
    pSize -= size;
    if (child != NULL) {
        *pBuffer |= 0x80;
        for (; child != NULL; child = child->next) {
            child_buffer = pBuffer + size;
            child_size = GetHierarchyNetworkStuff(child, child_buffer, pSize);
            if (child_size < 0) {
                return -1;
            }
            pSize -= child_size;
            size += child_size;
            *child_buffer |= 0x40;
        }
        *child_buffer &= ~0x40;
    }
    return size;
}

// FindObject

// STUB: CARMA2_HW 0x0049cd90
// STUB: CARMA2_SW 0x00498f10
// STUB: CARMA2_CHEAT 0x0049d140
void C2_HOOK_FASTCALL WriteHierarchyNetworkStuff(tU8* pBuffer, tPhysics_object* pObject, int pSize) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049ce60
// FUNCTION: CARMA2_SW 0x00498fe0
// FUNCTION: CARMA2_CHEAT 0x0049d210
int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject) {
    int size;
    tPhysics_object* child;

    size = SizeOfObjectNetworkStuff(pObject->physics_joint1 != NULL ? pObject->physics_joint1->type : eJoint_none);
    for (child = pObject->child; child != NULL; child = child->next) {
        size += GetHierarchyNetworkSize(child);
    }
    return size;
}

// FUNCTION: CARMA2_HW 0x0049cee0
// FUNCTION: CARMA2_SW 0x00499060
// FUNCTION: CARMA2_CHEAT 0x0049d290
int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data) {
    int size;
    tU8* next;
    tU8 more;

    size = SizeOfObjectNetworkStuff(*pNet_data);
    more = *pNet_data & 0x80;
    while (more != 0) {
        next = pNet_data + size;
        size += GetNetworkDataSize(next);
        more = *next & 0x40;
    }
    return size;
}

// GetSingleMatrixFromNetworkData

// FUNCTION: CARMA2_HW 0x004b5cc0
// FUNCTION: CARMA2_SW 0x004b23e0
int C2_HOOK_FASTCALL PHILInit(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPhil_queued_objects.headers) != 100);
    C2_HOOK_BUG_ON(sizeof(gPhil_queued_objects.headers[0]) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPHIL_queued_objects, headers, 0x68);

    for (i = 0; i < (int)REC2_ASIZE(gPhil_queued_objects.headers); i++) {
        gPhil_queued_objects.headers[i].collision_info = NULL;
    }
    gPHIL_count_list_collision_infos = 0;
    gPHIL_count_queued_objects = 0;
    gPHIL_count_queued_objects_for_removal = 0;
    gPHIL_list_collision_infos = NULL;
    gPHIL_munging_objects = 0;
    gPHIL_doing_physics = 0;
    gPHIL_object_added = 0;
    gPHIL_last_physics_tick = 0;
    gPHIL_mechanics_time_sync = 1;
    gPHIL_disabled = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5d20
// FUNCTION: CARMA2_SW 0x004b2440
// FUNCTION: CARMA2_CHEAT 0x004bb5f0
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_disabled = 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b5d40
// STUB: CARMA2_SW 0x004b2460
// STUB: CARMA2_CHEAT 0x004bb610
int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_CHEAT
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_CHEAT 0x004bb790
void C2_HOOK_FASTCALL PHILVerify(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b5ea0
// STUB: CARMA2_SW 0x004b25c0
// STUB: CARMA2_CHEAT 0x004bb890
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5fd0
// FUNCTION: CARMA2_SW 0x004b26f0
// FUNCTION: CARMA2_CHEAT 0x004bb9f0
tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void) {
    if (gPHIL_disabled) {
        return NULL;
    } else {
        return gPHIL_list_collision_infos;
    }
}

// FUNCTION: CARMA2_HW 0x004b5ff0
// FUNCTION: CARMA2_SW 0x004b2710
// FUNCTION: CARMA2_CHEAT 0x004bba10
tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject) {
    if (gPHIL_disabled) {
        return NULL;
    } else {
        return pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b6010
// FUNCTION: CARMA2_SW 0x004b2730
tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x239, 0x239);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x240, 0x240);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPHIL_queued_header, field_0x8, 0x8);

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        return info->field_0x8;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        return info->field_0x8;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004b6040
// FUNCTION: CARMA2_SW 0x004b2760
int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 1;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 1;
        pObject->flags |= 0x20;
        return 0;
    } else {
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004b6090
// FUNCTION: CARMA2_SW 0x004b27b0
// FUNCTION: CARMA2_CHEAT 0x004bbad0
int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag) {
    tPHIL_queued_object_info* info;

    if (gPHIL_disabled) {
        return 0;
    }
    PHILVerify();
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 2;
        if (pVelocity != NULL) {
            info->field_0x18 = *pVelocity;
            info->flags |= 0x1;
        }
        if (pOmega != NULL) {
            info->field_0x24 = *pOmega;
            info->flags |= 0x2;
        }
        if (pFlag) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        PHILVerify();
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 2;
        pObject->flags &= ~0x20;
        if (pFlag) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        if (pVelocity != NULL) {
            pObject->v = *pVelocity;
        }
        if (pOmega != NULL) {
            pObject->omega = *pOmega;
        }
        ResetCarSpecialVolume(pObject);
        PHILVerify();
        return 0;
    } else {
        PHILVerify();
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004b61a0
// FUNCTION: CARMA2_SW 0x004b28c0
// FUNCTION: CARMA2_cHEAT 0x004bbc00
inline int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix) {
    tPHIL_queued_object_info* queued;
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    PHILVerify();
    if (pObject->field_0x239 == 2) {
        queued = pObject->field_0x240;
        queued->field_0x30 = *pMatrix;
        queued->flags |= 0x4;
        PHILVerify();
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        if (info->field_0x8 != 1) {
            PHILVerify();
            return 4;
        }
        info->collision_info->actor->t.t.mat = *pMatrix;
        PHILVerify();
        return 0;
    } else {
        PHILVerify();
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004b6210
// FUNCTION: CARMA2_SW 0x004b2930
// FUNCTION: CARMA2_CHEAT 0x004bbc90
int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, const br_matrix34* pArg2, const br_vector3* pArg3, const br_vector3* pArg4) {
    int result;

    if (gPHIL_disabled) {
        return 0;
    }
    result = PHILAddObject(pInfo);
    if (result == 0 && (pArg2 == NULL || (result = PHILSetPassiveObjectsMatrix(pInfo, pArg2)) == 0)) {
        result = PHILMakeObjectActive(pInfo, pArg3, pArg4, 0);
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x004b62b0
// FUNCTION: CARMA2_SW 0x004b29d0
// FUNCTION: CARMA2_CHEAT 0x004bbd40
int C2_HOOK_FASTCALL PHILAddObjectImmediately(tPhysics_object* pObject) {
    int result;

    if (gPHIL_disabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddObject(pObject);
    gPHIL_object_added = 0;
    return result;
}

// FUNCTION: CARMA2_HW 0x004b62e0
// FUNCTION: CARMA2_SW 0x004b2a00
// FUNCTION: CARMA2_CHEAT 0x004bbd70
int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pObject, const br_matrix34* pMatrix, const br_vector3* pVelocity, const br_vector3* pOmega) {
    int result;

    if (gPHIL_disabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddObject(pObject);
    if (result == 0 && (pMatrix == NULL || (result = PHILSetPassiveObjectsMatrix(pObject, pMatrix)) == 0)) {
        result = PHILMakeObjectActive(pObject, pVelocity, pOmega, 0);
    }
    gPHIL_object_added = 0;
    return result;
}

// FUNCTION: CARMA2_HW 0x004b63b0
// FUNCTION: CARMA2_SW 0x004b2ad0
// FUNCTION: CARMA2_CHEAT 0x004bbe50
int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pCollision_info, int pParam, ...) {
    tCollision_info_owner* owner;
    va_list ap;

    if (gPHIL_disabled) {
        return 0;
    }
#ifdef REC2_CHEAT
    PHILVerify();
#endif
    if (pCollision_info->field_0x239 == 2) {
        owner = pCollision_info->field_0x240;
    } else {
        owner = pCollision_info->field_0x240;
    }
    if (owner != NULL) {
        if (pParam < 0 || pParam >= 9) {
            return 5;
        }
        va_start(ap, pParam);
        switch (pParam) {
        case 0:
            owner->field_0x10 = (float)va_arg(ap, double);
            break;
        case 1:
            if (va_arg(ap, int)) {
                owner->field_0x04 |= 0x1;
                PositionChildren(pCollision_info);
            } else {
                owner->field_0x04 &= 0x1;
            }
            break;
        case 2:
            owner->field_0x0c = (float)va_arg(ap, double);
            break;
        case 3:
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 6)) | (((va_arg(ap, int) != 0) & 0x1) << 6);
            if (!(pCollision_info->flags & 0x40)) {
                pCollision_info->box_face_ref = gFace_num__car - 2;
            }
            break;
        case 4:
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 3)) | (((va_arg(ap, int) != 0) & 0x1) << 3);
            break;
        case 5:
            pCollision_info->flags = (pCollision_info->flags & ~(1 << 4)) | (((va_arg(ap, int) != 0) & 0x1) << 4);
            break;
        case 6:
            if (va_arg(ap, int)) {
                owner->field_0x04 |= 0x4;
            } else {
                owner->field_0x04 &= 0x4;
            }
            break;
        case 7:
            if (va_arg(ap, int)) {
                owner->field_0x04 |= 0x8;
                pCollision_info->water_d = 10000.f;
            } else {
                owner->field_0x04 &= 0x8;
            }
            break;
        case 8:
            owner->field_0x14 = (float)va_arg(ap, double);
            break;
        }
#ifdef REC2_CHEAT
        PHILVerify();
#endif
        va_end(ap);
        return 0;
    }
#ifdef REC2_CHEAT
    PHILVerify();
#endif
    return 3;
}

// FUNCTION: CARMA2_HW 0x004b6530
// FUNCTION: CARMA2_SW 0x004b2c50
// FUNCTION: CARMA2_CHEAT 0x004bc010
float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pObject, int pProperty) {
    tCollision_info_owner* owner;

    if (!gPHIL_disabled) {
        if (pObject->field_0x239 == 2) {
            owner = pObject->field_0x240;
        } else {
            owner = pObject->field_0x240;
        }
        if (owner != NULL) {
            if (pProperty < 0 || pProperty >= 9) {
                return 5.0f;
            }
            switch (pProperty) {
            case 0:
                return owner->field_0x10;
            case 1:
                return (float)(owner->field_0x04 & 0x1);
            case 2:
                return owner->field_0x0c;
            case 3:
                return (float)(int)(((tU32)pObject->flags >> 6) & 0x1);
            case 4:
                return (float)(int)(((tU32)pObject->flags >> 3) & 0x1);
            case 5:
                return (float)(int)(((tU32)pObject->flags >> 4) & 0x1);
            case 6:
                return (float)((owner->field_0x04 >> 2) & 0x1);
            case 7:
                return (float)((owner->field_0x04 >> 3) & 0x1);
            case 8:
                return owner->field_0x14;
            }
        }
    }
    return 0.0f;
}

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag2

// ProcessDrag

// FUNCTION: CARMA2_HW 0x004b6e90
void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject) {
    tPhysics_object* child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xf0, 0xf0);

    pObject->field_0xf0 = 2;
    for (child = pObject->child; child != NULL; child = child->next) {
        MarkObjectAndChildrenAsPassive(child);
    }
}

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// PHILDoPhysics

// FUNCTION: CARMA2_HW 0x004b7220
// FUNCTION: CARMA2_SW 0x004b3940
// FUNCTION: CARMA2_CHEAT 0x004bcd50
void C2_HOOK_FASTCALL PHILApplyPHILObject(tPhysics_object* pObject, const br_matrix34* pMatrix) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, actor, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, transform_matrix, 0x8c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_actor, t, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_transform, t, 0x4);

    BrMatrix34Copy(&pObject->transform_matrix, pMatrix);
    BrMatrix34Copy(&pObject->actor->t.t.mat, pMatrix);
}


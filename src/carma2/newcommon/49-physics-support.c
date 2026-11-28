/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "49-physics-support.h"

#include "49-physics.h"
#include "65-hull.h"
#include "platform.h"
#include "rec2_macros.h"
#include "c2_math.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#define PHYSICS_BUFFER_PART_SIZE 40000
#define PHYSICS_BUFFER_OTHER_SIZE 50000

// FUNCTION: CARMA2_HW 0x004c5c70
void* C2_HOOK_FASTCALL GetPhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    return gPhysics_scratch_ptr;
}

// FUNCTION: CARMA2_HW 0x004c5cb0
void C2_HOOK_FASTCALL IncreasePhysicsScratchPointer(int pSize) {

    if ((tU8*)gPhysics_scratch_ptr + pSize > (tU8*)gPhysics_other_buffer + gPhysics_other_buffer_capacity) {
        PhysicsError(ePhysicsError_OutOfMemory, NULL);
    }
    gPhysics_scratch_ptr = (tU8*)gPhysics_scratch_ptr + pSize;
}

// FUNCTION: CARMA2_HW 0x004c5d00
// FUNCTION: CARMA2_SW 0x004c2430
void C2_HOOK_FASTCALL ClearPhysicsScratchSpace(void) {

    gPhysics_scratch_ptr = gPhysics_other_buffer;
}

// FUNCTION: CARMA2_HW 0x004c5d10
// FUNCTION: CARMA2_SW 0x004c2440
// FUNCTION: CARMA2_CHEAT 0x004cb830
tCollision_shape_sphere* C2_HOOK_FASTCALL AllocateShapeSphere(int pType) {
    tCollision_shape_sphere* result;

    result = BrMemAllocate(sizeof(tCollision_shape_sphere), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Sphere;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5d70
// FUNCTION: CARMA2_SW 0x004c24a0
// FUNCTION: CARMA2_CHEAT 0x004cb890
tCollision_shape_polyhedron* C2_HOOK_FASTCALL AllocateShapePolyhedron(int pCount_points, int pType) {
    tCollision_shape_polyhedron* result;
    tU8* raw_memory;
    int count_edges;
    int count_planes;

    count_planes = (pCount_points - 2) * 2;
    count_edges = (pCount_points - 2) * 3;
    result = BrMemAllocate(sizeof(tCollision_shape_polyhedron) + pCount_points * sizeof(br_vector3)
        + count_edges * sizeof(tPolyhedron_edge_indexes) + count_planes * sizeof(br_vector4), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_polyhedron);
    result->polyhedron.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->polyhedron.edges = (tPolyhedron_edge_indexes*)raw_memory;
    raw_memory += count_edges * sizeof(tPolyhedron_edge_indexes);
    result->polyhedron.planes = (br_vector4*)raw_memory;
    result->polyhedron.count_points = 0;
    result->polyhedron.count_edges = 0;
    result->polyhedron.count_planes = 0;
    result->common.type = kCollisionShapeType_Polyhedron;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5dc0
tCollision_shape_wireframe* C2_HOOK_FASTCALL AllocateShapeWireFrame(int pCount_points, int pCount_lines, int pType) {
    tCollision_shape_wireframe* result;
    tU8* raw_memory;

    result = BrMemAllocate(sizeof(tCollision_shape_wireframe) + pCount_points * sizeof(br_vector3)
        + pCount_lines * sizeof(tPolyhedron_edge_indexes), (br_uint_8)pType);
    raw_memory = (tU8*)result + sizeof(tCollision_shape_wireframe);
    result->wireframe.points = (br_vector3*)raw_memory;
    raw_memory += pCount_points * sizeof(br_vector3);
    result->wireframe.count_lines = pCount_lines;
    result->wireframe.lines = (tPolyhedron_edge_indexes*)raw_memory;
    result->wireframe.count_points = pCount_points;
    result->common.type = kCollisionShapeType_Wireframe;
    return result;
}

// FUNCTION: CARMA2_HW 0x004c5e00
// FUNCTION: CARMA2_SW 0x004c2530
// FUNCTION: CARMA2_CHEAT 0x004cb920
tCollision_shape_box* C2_HOOK_FASTCALL AllocateShapeBox(int pType) {
    tCollision_shape_box* result;

    result = BrMemAllocate(sizeof(tCollision_shape_box), (br_uint_8)pType);
    result->common.type = kCollisionShapeType_Box;
    return result;
}

// GetJointSize

// FUNCTION: CARMA2_HW 0x004c5e20
// FUNCTION: CARMA2_SW 0x004c2550
// FUNCTION: CARMA2_CHEAT 0x004cb940
tPhysics_joint* C2_HOOK_FASTCALL AllocatePhysicsJoint(int pCount_limits, int pType) {
    return BrMemAllocate(offsetof(tPhysics_joint, limits) + pCount_limits * sizeof(tPhysics_joint_limit), (br_uint_8)pType);
}

// FUNCTION: CARMA2_HW 0x004c5e40
tPhysics_joint* C2_HOOK_FASTCALL ClonePhysicsJoint(const tPhysics_joint* pJoint, int pType) {
    size_t size;
    tPhysics_joint* clone;

    size = offsetof(tPhysics_joint, limits) + pJoint->count_limits * sizeof(tPhysics_joint_limit);
    clone = BrMemAllocate(size, (br_uint_8)pType);
    memcpy(clone, pJoint, size);
    return clone;
}

// FUNCTION: CARMA2_HW 0x004c5e70
// FUNCTION: CARMA2_SW 0x004c25a0
// FUNCTION: CARMA2_CHEAT 0x004cb990
void C2_HOOK_FASTCALL FreeShape(void* pData) {
    BrMemFree(pData);
}

// FUNCTION: CARMA2_HW 0x004c5e80
// FUNCTION: CARMA2_SW 0x004c25b0
// FUNCTION: CARMA2_CHEAT 0x004cb9a0
void C2_HOOK_FASTCALL FreeShapeList(tPhysics_shape *pShape) {

    if (pShape->common.next != NULL) {
        FreeShapeList(pShape->common.next);
    }
    BrMemFree(pShape);
}

// FUNCTION: CARMA2_HW 0x004c5ea0
// FUNCTION: CARMA2_SW 0x004c25d0
// FUNCTION: CARMA2_CHEAT 0x004cb9c0
void C2_HOOK_FASTCALL FreePhysicsJoint(tPhysics_joint* pJoint) {

    BrMemFree(pJoint);
}

// FUNCTION: CARMA2_HW 0x004c5eb0
// FUNCTION: CARMA2_SW 0x004c25e0
// FUNCTION: CARMA2_CHEAT 0x004cb9d0
void C2_HOOK_FASTCALL GetBoundsFromPointList(const br_vector3* pVertices, int pCount_vertices, br_bounds3* pBounds) {
    int i;

    BrVector3Copy(&pBounds->min, &pVertices[0]);
    BrVector3Copy(&pBounds->max, &pVertices[0]);
    for (i = 1; i < pCount_vertices; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (pVertices[i].v[j] < pBounds->min.v[j]) {
                pBounds->min.v[j] = pVertices[i].v[j];
            } else if (pVertices[i].v[j] > pBounds->max.v[j]) {
                pBounds->max.v[j] = pVertices[i].v[j];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c5f20
// FUNCTION: CARMA2_SW 0x004c2650
// FUNCTION: CARMA2_CHEAT 0x004cba40
void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape) {
    int i;
    tPhysicsError error;

    C2_HOOK_BUG_ON(REC2_ASIZE(pShape->common.bb.min.v) != 3);

    switch (pShape->common.type) {
    case kCollisionShapeType_Box:
        break;
    case kCollisionShapeType_Wireframe:
        if (pShape->wireframe.wireframe.count_points == 0) {
            PhysicsError(ePhysicsError_WireFrameHasNoPoints, NULL);
        } else {
            GetBoundsFromPointList(pShape->wireframe.wireframe.points, pShape->wireframe.wireframe.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 4) {
            PhysicsError(ePhysicsError_PolyhedronHasNoPoints, NULL);
        } else {
            error = ConvexHull3D(&pShape->polyhedron.polyhedron);
            if (error != ePhysicsError_Ok) {
                if (error == ePhysicsError_WireFrameHasNoPoints) {
                    pShape->common.type = kCollisionShapeType_Wireframe_Polyhedron;
                } else {
                    PDFatalError("This polyhedron is too wierd for me");
                }
            }
            GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Wireframe_Polyhedron:
        if (pShape->polyhedron.polyhedron.count_points < 3) {
            PhysicsError(4, NULL);
        } else {
            ConvexHull2D(&pShape->polyhedron.polyhedron);
            GetBoundsFromPointList(pShape->polyhedron.polyhedron.points, pShape->polyhedron.polyhedron.count_points, &pShape->common.bb);
        }
        break;
    case kCollisionShapeType_Sphere:
        BrVector3Copy(&pShape->common.bb.min, &pShape->sphere.sphere.center);
        BrVector3Copy(&pShape->common.bb.max, &pShape->sphere.sphere.center);
        for (i = 0; i < REC2_ASIZE(pShape->common.bb.min.v); i++) {
            pShape->common.bb.min.v[i] -= pShape->sphere.sphere.radius;
            pShape->common.bb.max.v[i] += pShape->sphere.sphere.radius;
        }
        pShape->sphere.sphere.radius_squared = REC2_SQR(pShape->sphere.sphere.radius);
        break;
    default:
        PhysicsError(ePhysicsError_UnknownShapeType, NULL);
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004c60a0
// FUNCTION: CARMA2_SW 0x004c27d0
// FUNCTION: CARMA2_CHEAT 0x004cbbc0
void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info) {
    tPhysics_shape* shape;
    tPhysics_shape* current;
    br_scalar radius;

    shape = pCollision_info->shape;
    if (shape == NULL) {
        return;
    }
    pCollision_info->bb1 = shape->common.bb;

    for (current = shape->common.next; current != NULL; current = current->common.next) {
        int i;

        for (i = 0; i < 3; i++) {

            pCollision_info->bb1.min.v[i] = pCollision_info->bb1.min.v[i] < current->common.bb.min.v[i] ? pCollision_info->bb1.min.v[i] : current->common.bb.min.v[i];
            pCollision_info->bb1.max.v[i] = pCollision_info->bb1.max.v[i] > current->common.bb.max.v[i] ? pCollision_info->bb1.max.v[i] : current->common.bb.max.v[i];
        }
    }
    pCollision_info->bb2 = pCollision_info->bb1;

    pCollision_info->radius_squared = 0.0f
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[0]), REC2_SQR(pCollision_info->bb2.max.v[0]))
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[1]), REC2_SQR(pCollision_info->bb2.max.v[1]))
            + MAX(REC2_SQR(pCollision_info->bb2.min.v[2]), REC2_SQR(pCollision_info->bb2.max.v[2]));
    radius = (br_scalar)sqrt(pCollision_info->radius_squared);
    pCollision_info->radius = radius;
}

// FUNCTION: CARMA2_HW 0x004c61c0
// FUNCTION: CARMA2_SW 0x004c28f0
// FUNCTION: CARMA2_CHEAT 0x004cbce0
void C2_NORETURN C2_HOOK_FASTCALL PhysicsError(tPhysicsError pError, const char* pMessage) {

    if (gPhysics_error_cb != NULL) {
        gPhysics_error_cb(pError, pMessage);
    }
    exit(pError);
}

// FUNCTION: CARMA2_HW 0x004c61e0
// FUNCTION: CARMA2_SW 0x004c2910
// FUNCTION: CARMA2_CHEAT 0x004cbd00
void C2_HOOK_FASTCALL PhysicsWarning(const char* pMessage) {
    (void)pMessage;
}

// FUNCTION: CARMA2_HW 0x004c61f0
// FUNCTION: CARMA2_SW 0x004c2920
// FUNCTION: CARMA2_CHEAT 0x004cbd10
void C2_HOOK_FASTCALL PhysicsSetErrorHandler(tPhysicsError_cbfn* pError_cbfn) {

    gPhysics_error_cb = pError_cbfn;
}

// FUNCTION: CARMA2_HW 0x004c62e0
// FUNCTION: CARMA2_SW 0x004c2a10
// FUNCTION: CARMA2_CHEAT 0x004cbe00
void C2_HOOK_FASTCALL RepositionAndOrientateChildren(tPhysics_object* pObject, br_matrix34* pParent_matrix) {
    br_matrix34 inverse;
    br_matrix34 parent_matrix;
    br_matrix34 transform;
    tPhysics_object* child;

    child = pObject->child;
    if (pParent_matrix != NULL) {
        BrMatrix34Mul(&pObject->actor->t.t.mat, &pObject->transform_matrix, pParent_matrix);
    } else {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&transform, &inverse, &pObject->actor->t.t.mat);
        BrMatrix34LPNormalise(&parent_matrix, &transform);
        pParent_matrix = &parent_matrix;
    }
    if (child != NULL) {
        BrMatrix34LPInverse(&inverse, &pObject->transform_matrix);
        BrMatrix34Mul(&parent_matrix, &inverse, &pObject->actor->t.t.mat);
        for (; child != NULL; child = child->next) {
            RepositionAndOrientateChildren(child, pParent_matrix);
        }
    }
    BrMatrix34Copy(&pObject->transform_matrix, &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c63b0
// FUNCTION: CARMA2_SW 0x004c2ae0
// FUNCTION: CARMA2_CHEAT 0x004cbed0
void C2_HOOK_FASTCALL PhysicsAddObject(tPhysics_object* pParent, tPhysics_object* pChild) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->next = pParent->child;
    pParent->child = pChild;
    pChild->parent = pParent;
}

// FUNCTION: CARMA2_HW 0x004c63d0
// FUNCTION: CARMA2_SW 0x004c2b00
// FUNCTION: CARMA2_CHEAT 0x004cbef0
void C2_HOOK_FASTCALL PhysicsAddObjectList(tPhysics_object* pParent, tPhysics_object* pChild) {
    tPhysics_object* last;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, next, 0x220);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, child, 0x224);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, parent, 0x228);

    pChild->parent = pParent;
    last = pChild;
    while (last->next != NULL) {
        last = last->next;
        last->parent = pParent;
    }
    last->next = pParent->child;
    pParent->child = pChild;
}

// FUNCTION: CARMA2_HW 0x004c6410
// FUNCTION: CARMA2_SW 0x004c2b40
// FUNCTION: CARMA2_CHEAT 0x004cbf30
void C2_HOOK_FASTCALL PhysicsRemoveObject(tPhysics_object* pObject) {
    tPhysics_object* parent;
    tPhysics_object* sibling;

    parent = pObject->parent;
    sibling = parent->child;
    if (sibling == pObject) {
        parent->child = pObject->next;
    } else {
        while (sibling->next != pObject) {
            sibling = sibling->next;
        }
        sibling->next = pObject->next;
    }
    pObject->parent = NULL;
    pObject->next = NULL;
}

// FUNCTION: CARMA2_HW 0x004c6470
// FUNCTION: CARMA2_SW 0x004c2ba0
// FUNCTION: CARMA2_CHEAT 0x004cbf90
int C2_HOOK_FASTCALL PhysicsObjectRecurse(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    int result;
    tPhysics_object* child;

    result = pCallback(pObject, pContext);
    if (result != 0) {
        return result;
    }
    for (child = pObject->child; child != NULL; child = child->next) {
        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c64b0
int C2_HOOK_FASTCALL PhysicsObjectRecurseChildren(tPhysics_object* pObject, tEnumCollision_cbfn* pCallback, void* pContext) {
    tPhysics_object* child;

    for (child = pObject->child; child != NULL; child = child->next) {
        int result;

        result = PhysicsObjectRecurse(child, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004c6510
// FUNCTION: CARMA2_SW 0x004c2c40
// FUNCTION: CARMA2_CHEAT 0x004cc030
void C2_HOOK_FASTCALL InitPhysicsWorkspace(tU8* pBuffer, int pSize) {

    gPhysics_buffer1_ptr = pBuffer;
    pSize -= 3 * PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer2_ptr = pBuffer + PHYSICS_BUFFER_PART_SIZE;
    gPhysics_buffer3_ptr = pBuffer + 2 * PHYSICS_BUFFER_PART_SIZE;
    pBuffer += 3 * PHYSICS_BUFFER_PART_SIZE;
    if (pSize < PHYSICS_BUFFER_OTHER_SIZE) {
        PhysicsError(ePhysicsError_InsufficientSizedBuffer, "Physics workspace too small");
    }
    gPhysics_other_buffer_capacity = pSize;
    gPhysics_other_buffer = pBuffer;
}

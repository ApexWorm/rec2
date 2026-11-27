/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "renderfuncs.h"

#include "backend.h"
#include "match.h"

// STUB: D3D 0x100080e0
void C2_HOOK_CDECL PointRender_d3d_RGB_555(brp_block* block, brp_vertex* vertex) {
    NOT_IMPLEMENTED();
}

// STUB: D3D 0x10007f80
void C2_HOOK_CDECL LineRender_d3d(brp_block* block, brp_vertex* vertex) {
    NOT_IMPLEMENTED();
}

// STUB: D3D 0x10007030
void C2_HOOK_CDECL FUN_10007030(brp_block_d3d* block) {
    NOT_IMPLEMENTED();
}

// FUNCTION: D3D 0x10007080
void C2_HOOK_CDECL FUN_10007080(brp_block_d3d* block) {
    QUEUE_RENDER_STATE(1, gDWORD_1001bbb0);
    QUEUE_RENDER_STATE(0x15, gINT_1001bbf0);
    QUEUE_RENDER_STATE(4, 0);
}

// FUNCTION: D3D 0x100070e0
void C2_HOOK_CDECL FUN_100070e0(brp_block_d3d* block) {
    QUEUE_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE, gDWORD_1001bbb0);
    QUEUE_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, gINT_1001bbf0);
#ifdef REC2_FIX_BUGS
    if (gRegister_flags & 0x8) {
        QUEUE_RENDER_STATE(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);
    }
#else
    /* The original's queue macro is not a single statement, so only the type store depends on the flag */
    if (gRegister_flags & 0x8) {
        gQueued_render_states[gCount_queued_render_states].type = D3DRENDERSTATE_TEXTUREPERSPECTIVE;
    }
    gQueued_render_states[gCount_queued_render_states].value = 1;
    gCount_queued_render_states += 1;
#endif
}

// STUB: D3D 0x10007690
void C2_HOOK_CDECL FUN_10007690(brp_block* block, brp_vertex* vertex) {
    NOT_IMPLEMENTED();
}

// STUB: D3D 0x10007850
void C2_HOOK_CDECL FUN_10007850(brp_block* block, brp_vertex* vertex) {
    NOT_IMPLEMENTED();
}

// FUNCTION: D3D 0x10007a00
void C2_HOOK_CDECL FUN_10007a00(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2) {
    gINT_1001bbc4 += 1;
    gQueued_vertices_indices[gCount_queued_vertices_indices] = gCount_queued_vertices;
    gCount_queued_vertices_indices += 1;
    gQueued_vertices[gCount_queued_vertices].sx = v0->comp_f[C_SX];
    gQueued_vertices[gCount_queued_vertices].sy = v0->comp_f[C_SY];
    gQueued_vertices[gCount_queued_vertices].sz = v0->comp_f[C_SZ] / 65536.0f;
    gQueued_vertices[gCount_queued_vertices].tu = v0->comp_f[C_U];
    gQueued_vertices[gCount_queued_vertices].tv = v0->comp_f[C_V];
    gQueued_vertices[gCount_queued_vertices].rhw = (float)(1.0 / v0->comp_f[C_W]);
    gQueued_vertices[gCount_queued_vertices].color = gCOLOUR_100156e8 | 0xffffff;
    gQueued_vertices[gCount_queued_vertices].specular = 0;
    gCount_queued_vertices += 1;

    gQueued_vertices_indices[gCount_queued_vertices_indices] = gCount_queued_vertices;
    gCount_queued_vertices_indices += 1;
    gQueued_vertices[gCount_queued_vertices].sx = v1->comp_f[C_SX];
    gQueued_vertices[gCount_queued_vertices].sy = v1->comp_f[C_SY];
    gQueued_vertices[gCount_queued_vertices].sz = v1->comp_f[C_SZ] / 65536.0f;
    gQueued_vertices[gCount_queued_vertices].tu = v1->comp_f[C_U];
    gQueued_vertices[gCount_queued_vertices].tv = v1->comp_f[C_V];
    gQueued_vertices[gCount_queued_vertices].rhw = (float)(1.0 / v1->comp_f[C_W]);
    gQueued_vertices[gCount_queued_vertices].color = gCOLOUR_100156e8 | 0xffffff;
    gQueued_vertices[gCount_queued_vertices].specular = 0;
    gCount_queued_vertices += 1;

    gQueued_vertices_indices[gCount_queued_vertices_indices] = gCount_queued_vertices;
    gCount_queued_vertices_indices += 1;
    gQueued_vertices[gCount_queued_vertices].sx = v2->comp_f[C_SX];
    gQueued_vertices[gCount_queued_vertices].sy = v2->comp_f[C_SY];
    gQueued_vertices[gCount_queued_vertices].sz = v2->comp_f[C_SZ] / 65536.0f;
    gQueued_vertices[gCount_queued_vertices].tu = v2->comp_f[C_U];
    gQueued_vertices[gCount_queued_vertices].tv = v2->comp_f[C_V];
    gQueued_vertices[gCount_queued_vertices].rhw = (float)(1.0 / v2->comp_f[C_W]);
    gQueued_vertices[gCount_queued_vertices].color = gCOLOUR_100156e8 | 0xffffff;
    gQueued_vertices[gCount_queued_vertices].specular = 0;
    gCount_queued_vertices += 1;

    gPrimitive_type = D3DPT_TRIANGLELIST;
    if (gCount_queued_vertices > 100) {
        FUN_10007140();
    }
}

// STUB: D3D 0x10007ba0
void C2_HOOK_CDECL FUN_10007ba0(brp_block* block, brp_vertex* vertex) {
    NOT_IMPLEMENTED();
}

// STUB: D3D 0x10007da0
void C2_HOOK_CDECL FUN_10007da0(brp_block* block, brp_vertex* vertex) {
    NOT_IMPLEMENTED();
}

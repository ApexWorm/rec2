/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "37-brucetrk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

br_scalar gYon_factor = 0.25f;
int gRender_alternative_track_actors = 1;
static br_actor primary[9], alternate[9], world, camera_actor;
static br_camera camera;
static br_matrix34 matrix;
static tTrack_square squares[3][3];
static tTrack_square* rows[3];
static tTrack_spec track;
static int rendered[9];
static int renders, wrong_renderer, projections, transforms, failures;
static tU8 mock_column_x, mock_column_z;

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); failures++; } } while (0)

void C2_HOOK_FASTCALL XZToColumnXZ(tU8* x, tU8* z, br_scalar px, br_scalar pz, tTrack_spec* spec) {
    (void)px;
    (void)pz;
    CHECK(spec == &track);
    projections++;
    *x = mock_column_x;
    *z = mock_column_z;
}

void C2_HOOK_CDECL BrMatrix34ApplyV(br_vector3* dest, const br_vector3* src, const br_matrix34* transform) {
    CHECK(transform == &matrix);
    transforms++;
    *dest = *src;
}

void C2_HOOK_CDECL BrZbSceneRenderAdd(br_actor* actor) {
    (void)actor;
    wrong_renderer++;
}

void C2_HOOK_CDECL BrZbsSceneRenderAdd(br_actor* actor) {
    int i, id = -1;
    if (actor == &world) id = 1000;
    for (i = 0; i < 9; i++) {
        if (actor == &primary[i]) id = i;
        if (actor == &alternate[i]) id = 100 + i;
    }
    CHECK(id >= 0);
    CHECK(renders < 9);
    if (renders >= 9 || id < 0) exit(2);
    rendered[renders++] = id;
}

static void setup(float x_direction, float z_direction) {
    int z, x;
    memset(&track, 0, sizeof(track));
    memset(&camera, 0, sizeof(camera));
    memset(&matrix, 0, sizeof(matrix));
    for (z = 0; z < 3; z++) {
        rows[z] = squares[z];
        for (x = 0; x < 3; x++) {
            squares[z][x].actor_0x0 = &primary[z * 3 + x];
            squares[z][x].actor_0x4 = &alternate[z * 3 + x];
        }
    }
    track.ncolumns_x = track.ncolumns_z = 3;
    track.columns = rows;
    camera_actor.type_data = &camera;
    camera.field_of_view = 16384;
    camera.aspect = 1.0f;
    camera.yon_z = 100.0f;
    matrix.m[2][0] = x_direction;
    matrix.m[2][2] = z_direction;
    gRender_alternative_track_actors = 1;
    mock_column_x = mock_column_z = 1;
    renders = wrong_renderer = projections = transforms = 0;
}

static void run_expected(const int* expected, int count) {
    int i;
    RenderTrack(&world, &track, &camera_actor, &matrix);
    CHECK(wrong_renderer == 0);
    CHECK(renders == count);
    for (i = 0; i < renders && i < count; i++) CHECK(rendered[i] == expected[i]);
    if (track.columns != NULL) {
        CHECK(projections == 5);
        CHECK(transforms == 4);
    } else {
        CHECK(projections == 0 && transforms == 0);
    }
}

int main(void) {
    int i;
    const float directions[8][2] = {
        {2,1}, {-2,1}, {2,-1}, {-2,-1},
        {1,2}, {-1,2}, {1,-2}, {-1,-2}
    };
    const int orders[8][9] = {
        {0,3,6,1,4,7,2,5,8},
        {2,5,8,1,4,7,0,3,6},
        {6,3,0,7,4,1,8,5,2},
        {8,5,2,7,4,1,6,3,0},
        {0,1,2,3,4,5,6,7,8},
        {2,1,0,5,4,3,8,7,6},
        {6,7,8,3,4,5,0,1,2},
        {8,7,6,5,4,3,2,1,0}
    };
    const int alternate_order[] = {100,101,102,103,104,105,106,107,108};
    const int without_middle[] = {100,101,102,103,105,106,107,108};
    const int at_low_edge[] = {0,1,3,4};
    const int at_high_edge[] = {4,5,7,8};
    const int fallback[] = {1000};

    setup(1,2);
    track.columns = NULL;
    run_expected(fallback, 1);

    for (i = 0; i < 8; i++) {
        setup(directions[i][0], directions[i][1]);
        run_expected(orders[i], 9);
    }

    setup(1,2);
    gRender_alternative_track_actors = 0;
    run_expected(alternate_order, 9);

    setup(1,2);
    gRender_alternative_track_actors = 0;
    squares[1][1].actor_0x0 = NULL;
    run_expected(without_middle, 8);

    setup(1,2);
    mock_column_x = mock_column_z = 0;
    run_expected(at_low_edge, 4);

    setup(1,2);
    mock_column_x = mock_column_z = 2;
    run_expected(at_high_edge, 4);

    setup(1,1);
    run_expected(orders[4], 9);

    if (failures) return 1;
    puts("PASS: 14 RenderTrack selection/traversal cases (projection and transform mocked)");
    return 0;
}

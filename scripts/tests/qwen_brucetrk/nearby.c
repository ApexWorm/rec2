/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "37-brucetrk.h"
#include "41-utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static br_actor actors[3][6];
static tTrack_square squares[3][6];
static tTrack_square* rows[3];
static tTrack_spec track;
static br_vector3 position;
static float radius;
static int visited[18];
static int visits;
static int callbacks;
static int stop_after;
static int cookie;
static int failures;

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #c); failures++; } } while (0)

static intptr_t C2_HOOK_FASTCALL user_callback(br_actor* actor, void* arg) {
    CHECK(arg == &cookie);
    CHECK(actor == &actors[visited[visits - 1] / 6][visited[visits - 1] % 6]);
    callbacks++;
    return callbacks == stop_after ? 7 : 0;
}

intptr_t C2_HOOK_CDECL FoundAnActor(br_actor* actor, void* arg) {
    tFoundAnActor_context* context = arg;
    CHECK(context->match_type == 2);
    CHECK(context->identifier_value == 'Z');
    CHECK(context->identifier_index == -2);
    CHECK(context->match_flags == 0x1004);
    CHECK(context->max_dist_axis == radius);
    CHECK(context->max_dist_squared == radius * radius);
    CHECK(context->pos == &position);
    CHECK(context->callback == user_callback);
    CHECK(context->callback_context == &cookie);
    return context->callback(actor, context->callback_context);
}

intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* actor, br_actor_enum_cbfn* callback, void* arg) {
    int x, z, found = -1;
    CHECK(callback == FoundAnActor);
    for (z = 0; z < 3; z++) {
        for (x = 0; x < 6; x++) {
            if (actor == &actors[z][x]) {
                found = 6 * z + x;
            }
        }
    }
    CHECK(found >= 0);
    CHECK(visits < 18);
    if (visits >= 18 || found < 0) {
        exit(2);
    }
    visited[visits++] = found;
    return callback(actor, arg);
}

static void setup(float x, float z, float r) {
    int i, j;
    memset(&track, 0, sizeof(track));
    for (i = 0; i < 3; i++) {
        rows[i] = squares[i];
        for (j = 0; j < 6; j++) {
            squares[i][j].actor_0x0 = &actors[i][j];
        }
    }
    track.ncolumns_x = 6;
    track.ncolumns_z = 3;
    track.column_size_x = 10.0f;
    track.column_size_z = 10.0f;
    track.columns = rows;
    position.v[0] = x;
    position.v[1] = 0.0f;
    position.v[2] = z;
    radius = r;
    visits = callbacks = 0;
    stop_after = -1;
}

static int query(void) {
    return ProcessNearbyActors(&track, &position, radius, 2, 'Z', -2, 0x1004, user_callback, &cookie);
}

int main(void) {
    int i;
    const int expanded[] = {1, 7, 13, 2, 8, 14, 3, 9, 15};
    const int with_null[] = {1, 7, 13, 2, 14, 3, 9, 15};

    setup(15, 25, 1);
    CHECK(query() == 0);
    CHECK(visits == 1 && callbacks == 1 && visited[0] == 13);

    setup(25, 15, 6);
    CHECK(query() == 0);
    CHECK(visits == 9 && callbacks == 9);
    for (i = 0; i < visits; i++) CHECK(visited[i] == expanded[i]);

    setup(55, 5, 1);
    CHECK(query() == 0);
    CHECK(visits == 1 && visited[0] == 5);

    setup(100, 100, 1);
    CHECK(query() == 0);
    CHECK(visits == 1 && visited[0] == 17);

    setup(-20, -20, 1);
    CHECK(query() == 0);
    CHECK(visits == 1 && visited[0] == 0);

    setup(25, 15, 6);
    squares[1][2].actor_0x0 = NULL;
    CHECK(query() == 0);
    CHECK(visits == 8 && callbacks == 8);
    for (i = 0; i < visits; i++) CHECK(visited[i] == with_null[i]);

    setup(25, 15, 6);
    stop_after = 2;
    CHECK(query() == 7);
    CHECK(visits == 2 && callbacks == 2);
    CHECK(visited[0] == 1 && visited[1] == 7);

    if (failures != 0) return 1;
    puts("PASS: 7 nearby-actor traversal cases and callback metadata checks");
    return 0;
}

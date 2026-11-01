/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "41-utility.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char* name;
    float point[3];
    float normal[3];
    float plane_offset;
    float expected;
} Fixture;

int main(void) {
    const Fixture fixtures[] = {
        {"on axis plane", {10, 3, -10}, {0, 1, 0}, 3, 0},
        {"positive axis offset", {0, 5, 0}, {0, 1, 0}, 3, 2},
        {"negative axis offset", {0, -1, 0}, {0, 1, 0}, 3, -4},
        {"on oblique plane", {1, 1, 0}, {1, 2, -2}, 3, 0},
        {"positive oblique residual", {2, 3, -1}, {1, 2, -2}, 3, 7},
        {"negative oblique residual", {-1, -2, 1}, {1, 2, -2}, 3, -10},
        {"scaled nonunit normal", {2, 3, -1}, {3, 6, -6}, 9, 21},
        {"fractional coefficients", {4, 2, -0.25f}, {0.5f, -0.25f, 2}, 1.5f, -0.5f},
        {"reversed normal", {0, 5, 0}, {0, -1, 0}, -3, -2}
    };
    unsigned i;
    for (i = 0; i < sizeof(fixtures) / sizeof(fixtures[0]); i++) {
        const Fixture* f = &fixtures[i];
        tFace_ref face;
        br_vector3 point;
        br_scalar actual;
        memset(&face, 0, sizeof(face));
        memcpy(point.v, f->point, sizeof(point.v));
        memcpy(face.normal.v, f->normal, sizeof(face.normal.v));
        face.d = f->plane_offset;
        actual = DistanceFromFace(&point, &face);
        if (fabsf(actual - f->expected) > 0.000001f) {
            fprintf(stderr, "%s: expected %.8g, got %.8g\n", f->name, f->expected, actual);
            return 1;
        }
    }
    puts("PASS: 9 DistanceFromFace plane-residual fixtures");
    return 0;
}

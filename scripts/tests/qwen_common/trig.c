/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "36-trig.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
C2_NORETURN void rec2_error(const char* reason, const char* function, const char* file, int line) {
    (void)reason; (void)function; (void)file; (void)line;
    abort();
}
int main(void) {
    int a, i;
    for (a = -1080; a <= 1080; a++) {
        double radians = a * 3.14159265358979323846 / 180.0;
        if (fabs(FastScalarSin(a) - sin(radians)) > 0.000002 || fabs(FastScalarCos(a) - cos(radians)) > 0.000002) {
            fprintf(stderr, "quadrant/wrap mismatch at %d\n", a);
            return 1;
        }
    }
    for (i = -100; i <= 100; i++) {
        float value = i / 100.0f;
        volatile float actual = FastScalarArcSin(value);
        volatile float expected = FastFloatArcSin(value);
        if (actual != expected || fabs(actual - asin(value) * 180.0 / 3.14159265358979323846) > 1.1) {
            fprintf(stderr, "arcsine mismatch at %g: %.9g vs %.9g\n", value, FastScalarArcSin(value), FastFloatArcSin(value));
            return 1;
        }
    }
    puts("PASS: 2161 signed/wrapped angles; 201 arcsine forwarding values");
    return 0;
}

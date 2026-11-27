/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface.h"
#include "72-interface-main.h"
#include "72-interface-network.h"
#include "globvars.h"
#include "c2_string.h"

// FUNCTION: CARMA2_HW 0x004677d0
// FUNCTION: CARMA2_SW 0x00467550
// FUNCTION: CARMA2_CHEAT 0x00467c20
int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY) {
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {

        if (pX >= gFrontend_brender_items[i].model->vertices[1].p.v[0]
                && pX <= gFrontend_brender_items[i].model->vertices[3].p.v[0]
                && pY >= -gFrontend_brender_items[i].model->vertices[0].p.v[1]
                && pY <= -gFrontend_brender_items[i].model->vertices[1].p.v[1]
                && gCurrent_frontend_spec->items[i].enabled > 0 /* default or disabled */
                && gCurrent_frontend_spec->items[i].visible) {
            return i;
                }
    }
    return -1;
}

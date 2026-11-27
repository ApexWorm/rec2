/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_68_TIMER_H
#define GUARD_68_TIMER_H

#include "c2_hooks.h"
#include "platform.h"
#include "rec2_types.h"

#define TIMER_MNG 0
#define TIMER_ROB 1
#define TIMER_RWL 2
#define TIMER_RQQ 3
#define TIMER_PHS 4
#define TIMER_PED 5
#define TIMER_DRN 6
#define TIMER_OPP 7
#define TIMER_CRS 8
#define TIMER_LLR 9
#define TIMER_CRC 10
#define TIMER_SRC 11
#define TIMER_PRC 12
#define TIMER_CHK 13
#define TIMER_MAN 14
#define TIMER_GUA 15
#define TIMER_REC 16
#define TIMER_SER 17
#define TIMER_OQQ 18

extern int gTimers_stack_size;
extern tTimer gTimers[19];
extern int gTimers_stack[8];
extern int gTimers_max_index;

static inline void Timers_Push(int pType) {

    gTimers_stack[gTimers_stack_size] = pType;
    if (gTimers_stack_size != 0) {
#define timer_index gTimers_stack[gTimers_stack_size - 1]
        gTimers[timer_index].durations[gTimers[timer_index].index] += PDGetMicroseconds() - gTimers[timer_index].start_time;
#undef timer_index
    } else {
#define timer_index gTimers_max_index
        gTimers[timer_index].durations[gTimers[timer_index].index] += PDGetMicroseconds() - gTimers[timer_index].start_time;
#undef timer_index
    }
    gTimers[pType].start_time = PDGetMicroseconds();
    gTimers_stack_size += 1;
}

static inline void Timers_Pop(int pType) {

    gTimers[pType].durations[gTimers[pType].index] += PDGetMicroseconds() - gTimers[pType].start_time;
    if (gTimers_stack_size > 1) {
#define timer_index gTimers_stack[gTimers_stack_size - 1]
        gTimers[timer_index].start_time = PDGetMicroseconds();
#undef timer_index
    } else {
#define timer_index gTimers_max_index
        gTimers[timer_index].start_time = PDGetMicroseconds();
#undef timer_index
    }
    gTimers_stack_size -= 1;
}

extern void C2_HOOK_FASTCALL Timers_Init(void);

extern void C2_HOOK_FASTCALL Timers_StartFrame(void);

extern void C2_HOOK_FASTCALL Timers_EndFrame(void);

extern void C2_HOOK_FASTCALL Timers_Draw(br_pixelmap* pScreen);

#endif // GUARD_68_TIMER_H

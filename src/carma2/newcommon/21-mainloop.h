/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_21_MAINLOOP_H
#define GUARD_21_MAINLOOP_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */
extern int gAuto_increase_credits_dt[3];
extern tU32 gNet_auto_increase_credits_dt[8];
extern tU32 gCamera_period;
#ifdef REC2_SW
extern br_pixelmap* gSaved_depth_buffer;
#endif

extern tU32 gOld_camera_time;
extern tU32 gActual_last_tick_count;
extern tU32 gAverage_frame_period;
extern tU32 gLast_tick_count;
extern int gPed_count;
extern int gQueued_wasted_messages[5];
extern int gQueued_wasted_messages_count;
extern tU32 gLast_wasted_message_start;
extern tU32 gWasted_last_flash;
extern int gLast_credit_headup__mainloop;
extern int gWasted_flash_state;
extern int gLast_time_headup;
extern tU32 gLast_net_auto_increase_credits;
extern int gTotal_bonus_post_race;
extern float gMr_odo;
extern int gTime_bonus;
extern tTime_bonus_state gTime_bonus_state;
extern tU32 gTime_bonus_start;
extern int gTime_bonus_headup;
extern int gCredits_post_race;
extern int gRace_bonus_headup;
extern tU32 gPrev_task_switched_message;
extern int gWindow_inactive;

extern void C2_HOOK_FASTCALL CalculateFrameRate(void);

extern void C2_HOOK_FASTCALL LoseOldestWastedMassage(void);

extern void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex);

extern void C2_HOOK_FASTCALL MungeHeadups(void);

extern void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period);

#ifdef REC2_MATCHING
extern tU32 C2_HOOK_FASTCALL GetLastTickCount(void);
#endif

extern void C2_HOOK_FASTCALL CheckTimer(void);

extern int C2_HOOK_FASTCALL MungeRaceFinished(void);

// CalculateCameraStuff

extern void C2_HOOK_FASTCALL CheckActiveCarList(void);

extern tRace_result C2_HOOK_FASTCALL MainGameLoop(void);

extern tRace_result C2_HOOK_FASTCALL DoRace(void);

#endif // GUARD_21_MAINLOOP_H

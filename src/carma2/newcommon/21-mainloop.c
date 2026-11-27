/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "21-mainloop.h"

#include "00-car.h"
#include "01-network.h"
#include "04-netgame.h"
#include "05-drmem.h"
#include "07-structur.h"
#include "09-funks.h"
#include "11-world1.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "20-skidmark.h"
#include "22-replay.h"
#include "23-piping.h"
#include "27-powerup.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "33-depth.h"
#include "36-trig.h"
#include "38-flicplay.h"
#include "39-oil.h"
#include "40-main.h"
#include "41-utility.h"
#include "42-input.h"
#include "44-mainmenu.h"
#include "49-physics.h"
#include "51-smash.h"
#include "53-controls.h"
#include "54-runtime.h"
#include "56-aiworld.h"
#include "58-crush2.h"
#include "59-camera.h"
#include "60-phil.h"
#include "61-pedestrn-phys.h"
#include "67-lighting.h"
#include "68-timer.h"
#include "69-sound.h"
#include "c2_math.h"
#include "c2_stdio.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "joystick.h"
#include "platform.h"
#include "rec2_macros.h"
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x0068c814
// GLOBAL: CARMA2_SW 0x00686868
// GLOBAL: CARMA2_CHEAT 0x006940b4
tU32 gOld_camera_time;

// GLOBAL: CARMA2_HW 0x0068c820
// GLOBAL: CARMA2_SW 0x00686874
// GLOBAL: CARMA2_CHEAT 0x006940c0
tU32 gActual_last_tick_count;

// GLOBAL: CARMA2_HW 0x0068c824
// GLOBAL: CARMA2_SW 0x00686878
// GLOBAL: CARMA2_CHEAT 0x006940c4
tU32 gAverage_frame_period;

// GLOBAL: CARMA2_HW 0x00761ef0
// GLOBAL: CARMA2_SW 0x00758450
// GLOBAL: CARMA2_CHEAT 0x00769590
int gCredits_per_second_time_bonus[3];  /* FIXME: rename to gPoints_per_second */

// GLOBAL: CARMA2_HW 0x0074b740
// GLOBAL: CARMA2_SW 0x00741e00
// GLOBAL: CARMA2_CHEAT 0x00752de0
int gAuto_increase_credits_dt[3];

// GLOBAL: CARMA2_HW 0x0074b720
// GLOBAL: CARMA2_SW 0x00741de0
// GLOBAL: CARMA2_CHEAT 0x00752dc0
tU32 gNet_auto_increase_credits_dt[8];

// GLOBAL: CARMA2_HW 0x0074abf0
// GLOBAL: CARMA2_SW 0x0074129c
// GLOBAL: CARMA2_CHEAT 0x00752290
tU32 gCamera_period;

#ifdef REC2_SW
// GLOBAL: CARMA2_SW 0x007412a8
br_pixelmap* gSaved_depth_buffer;
#endif

// GLOBAL: CARMA2_HW 0x007447d4
// GLOBAL: CARMA2_SW 0x0073b294
// GLOBAL: CARMA2_CHEAT 0x0074be74
int gPed_count;

// GLOBAL: CARMA2_HW 0x0068c738
// GLOBAL: CARMA2_SW 0x00686788
// GLOBAL: CARMA2_CHEAT 0x00693fd8
br_vector3 gPrev_camera_position;

// GLOBAL: CARMA2_HW 0x00704e44
// GLOBAL: CARMA2_SW 0x006fbb24
// GLOBAL: CARMA2_CHEAT 0x0070c4e4
br_scalar gCamera_to_horiz_angle;

// GLOBAL: CARMA2_HW 0x0068c748
// GLOBAL: CARMA2_SW 0x00686798
// GLOBAL: CARMA2_CHEAT 0x00693fe8
int gQueued_wasted_messages[5] = { 0 };

// GLOBAL: CARMA2_HW 0x0068c75c
// GLOBAL: CARMA2_SW 0x006867ac
// GLOBAL: CARMA2_CHEAT 0x00693ffc
int gCompletion_bonus_post_race = 0;

// GLOBAL: CARMA2_HW 0x0068c760
// GLOBAL: CARMA2_SW 0x006867b0
// GLOBAL: CARMA2_CHEAT 0x00694000
int gQueued_wasted_messages_count;

// GLOBAL: CARMA2_HW 0x0068c77c
// GLOBAL: CARMA2_SW 0x006867d0
// GLOBAL: CARMA2_CHEAT 0x0069401c
tU32 gLast_wasted_message_start;

// GLOBAL: CARMA2_HW 0x0068c730
// GLOBAL: CARMA2_SW 0x00686780
// GLOBAL: CARMA2_CHEAT 0x00693fd0
tU32 gWasted_last_flash;

// GLOBAL: CARMA2_HW 0x0068c764
// GLOBAL: CARMA2_SW 0x006867b8
// GLOBAL: CARMA2_CHEAT 0x00694004
int gLast_credit_headup__mainloop;

// GLOBAL: CARMA2_HW 0x0068c768
// GLOBAL: CARMA2_SW 0x006867bc
// GLOBAL: CARMA2_CHEAT 0x00694008
int gWasted_flash_state;

// GLOBAL: CARMA2_HW 0x0068c76c
// GLOBAL: CARMA2_SW 0x006867c0
// GLOBAL: CARMA2_CHEAT 0x0069400c
int gLast_time_headup;

// GLOBAL: CARMA2_HW 0x0068c770
// GLOBAL: CARMA2_SW 0x006867c4
// GLOBAL: CARMA2_CHEAT 0x00694010
tU32 gLast_net_auto_increase_credits;

// GLOBAL: CARMA2_HW 0x0068c774
// GLOBAL: CARMA2_SW 0x006867c8
// GLOBAL: CARMA2_CHEAT 0x00694014
int gTotal_bonus_post_race;

// GLOBAL: CARMA2_HW 0x0068c778
// GLOBAL: CARMA2_SW 0x006867cc
// GLOBAL: CARMA2_CHEAT 0x00694018
float gMr_odo;

// GLOBAL: CARMA2_HW 0x0068c780
// GLOBAL: CARMA2_SW 0x006867d4
// GLOBAL: CARMA2_CHEAT 0x00694020
int gTime_bonus;

// GLOBAL: CARMA2_HW 0x0068c784
// GLOBAL: CARMA2_SW 0x006867d8
// GLOBAL: CARMA2_CHEAT 0x00694024
tTime_bonus_state gTime_bonus_state;

// GLOBAL: CARMA2_HW 0x0068c788
// GLOBAL: CARMA2_SW 0x006867dc
// GLOBAL: CARMA2_CHEAT 0x00694028
tU32 gTime_bonus_start;

// GLOBAL: CARMA2_HW 0x0068c808
// GLOBAL: CARMA2_SW 0x00686858
// GLOBAL: CARMA2_CHEAT 0x006940a8
int gTime_bonus_headup;

// GLOBAL: CARMA2_HW 0x0068c80c
// GLOBAL: CARMA2_SW 0x00686860
// GLOBAL: CARMA2_CHEAT 0x006940ac
int gCredits_post_race;

// GLOBAL: CARMA2_HW 0x0068c810
// GLOBAL: CARMA2_SW 0x00686864
// GLOBAL: CARMA2_CHEAT 0x006940b0
int gRace_bonus_headup;

// GLOBAL: CARMA2_HW 0x0068c834
// GLOBAL: CARMA2_SW 0x00686888
// GLOBAL: CARMA2_CHEAT 0x006940d4
tU32 gPrev_task_switched_message;

// GLOBAL: CARMA2_HW 0x0068c838
// GLOBAL: CARMA2_SW 0x0068688c
// GLOBAL: CARMA2_CHEAT 0x006940d8
int gWindow_inactive;

inline void C2_HOOK_FASTCALL CalculateFrameRate(void) {
    // GLOBAL: CARMA2_HW 0x0068c734
    // GLOBAL: CARMA2_SW 0x00686784
    // GLOBAL: CARMA2_CHEAT 0x00693fd4
    static tU32 last_time;
    tU32 new_time;
    // GLOBAL: CARMA2_HW 0x0068c790
    // GLOBAL: CARMA2_SW 0x006867e0
    // GLOBAL: CARMA2_CHEAT 0x00694030
    static int last_rates[30];
    int new_rate;
    int i;

    new_time = PDGetTotalTime();
    if (new_time != last_time) {
        new_rate = 10000 / (new_time - last_time);
        gFrame_rate = new_rate;
        for (i = 0; i < (int)REC2_ASIZE(last_rates); i++) {
            gFrame_rate += last_rates[i];
        }
        gFrame_rate /= (int)REC2_ASIZE(last_rates) + 1;
        for (i = 0; i < (int)REC2_ASIZE(last_rates) - 1; i++) {
            last_rates[i] = last_rates[i + 1];
        }
        last_rates[REC2_ASIZE(last_rates) - 1] = new_rate;
        last_time = new_time;
    }
}

void C2_HOOK_FASTCALL LoseOldestWastedMassage(void) {
    int i;

    for (i = 1; i < gQueued_wasted_messages_count; i++) {
        gQueued_wasted_messages[i - 1] = gQueued_wasted_messages[i];
    }
    gQueued_wasted_messages_count--;
    gLast_wasted_message_start = GetTotalTime();
}

// FUNCTION: CARMA2_HW 0x00492620
// FUNCTION: CARMA2_SW 0x0048f840
// FUNCTION: CARMA2_CHEAT 0x004925c0
void C2_HOOK_FASTCALL QueueWastedMassage(int pIndex) {

    if (gQueued_wasted_messages_count == (int)REC2_ASIZE(gQueued_wasted_messages)) {
        LoseOldestWastedMassage();
    }
    if (gQueued_wasted_messages_count == 0) {
        gLast_wasted_message_start = GetTotalTime();
    }
    gQueued_wasted_messages[gQueued_wasted_messages_count] = pIndex;
    gQueued_wasted_messages_count++;
}

inline void C2_HOOK_FASTCALL MungeHeadups(void) {
    char the_text[256];
    int new_countdown;
    int net_credits;
    int previous_gtimer;
    int previous_time_bonus;
    int effective_timer;
    int bonus;
    int oppo_count;
    tU32 the_time;
    // GLOBAL: CARMA2_HW 0x0068c818
    // GLOBAL: CARMA2_SW 0x0068686c
    // GLOBAL: CARMA2_CHEAT 0x006940b8
    static tU32 last_rattle_time;

    ClearHeadupSlot(3);
    MungeJoystickHeadups();
    net_credits = gProgram_state.credits;
    if (fabs((float)net_credits - (float)gLast_credit_headup__mainloop) / (double)gFrame_period > 1.2) {
        if (net_credits - gLast_credit_headup__mainloop > 0) {
            net_credits = (int)((net_credits - gLast_credit_headup__mainloop + 1000.0) * (double)gFrame_period * 1.2 / 1000.0 + (double)gLast_credit_headup__mainloop);
        } else {
            net_credits = (int)((double)gLast_credit_headup__mainloop - ((double)(gLast_credit_headup__mainloop - net_credits) + 1000.0) * (double)gFrame_period * 1.2 / 1000.0);
        }
    }
    if (gNet_mode != eNet_mode_none
        && gCountdown == 0
        && gNet_auto_increase_credits_dt[gCurrent_net_game->type] != 0
        && net_credits < gRecovery_cost.initial_network[gCurrent_net_game->type]
        && PDGetTotalTime() - gLast_net_auto_increase_credits >= gNet_auto_increase_credits_dt[gCurrent_net_game->type]) {
        gLast_net_auto_increase_credits = PDGetTotalTime();
        gProgram_state.credits += 50;
        if (net_credits + 50 > (gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
            gProgram_state.credits -= net_credits - (gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]) + 50.0f;
            net_credits = (int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]);
        } else {
            net_credits += 50;
        }
    }
    gLast_credit_headup__mainloop = net_credits;
    if (gCountdown != 0) {
        new_countdown = (int)(7.5 - (double)GetRaceTime() / 1000.0);
        if (new_countdown < 0) {
            new_countdown = 0;
        }
        if (new_countdown != gCountdown && new_countdown <= 5) {
            gCountdown = new_countdown;
            NewImageHeadupSlot(5, 0, 800, new_countdown + 4);
            DRS3StartSound(gPedestrians_outlet, gCountdown + eSoundId_Countdown_Start);
            if (new_countdown == 0) {
                MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo();
            }
        }
    }
    if (fabs((float)gTimer - (float)gLast_time_headup) / (double)gFrame_period > 10.0) {
        if (gTimer - gLast_time_headup > 0) {
            effective_timer = (int)(gFrame_period * 10.0 + gLast_time_headup);
        } else {
            effective_timer = gTimer;
        }
    } else {
        effective_timer = gTimer;
    }
    gLast_time_headup = effective_timer;
    if (gNet_mode != eNet_mode_none) {
        DoNetworkHeadups(net_credits);
    } else {
        if (net_credits >= 0) {
            sprintf(the_text, "\xf8%d\xfa %s", net_credits, GetMiscString(net_credits >= 100000 ? eMiscString_credits_abbrev : eMiscString_credits_gained));
        } else {
            sprintf(the_text, "\xf8%d\xfa %s", -net_credits, GetMiscString(eMiscString_credits_lost));
        }
        ChangeHeadupText(gCredits_won_headup, the_text);
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
            sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", gCount_killed_peds, gPed_count, GetMiscString(eMiscString_kills));
            ChangeHeadupText(gPed_kill_count_headup, the_text);
        } else if (gCurrent_race.race_spec->race_type == kRaceType_Peds) {
            sprintf(the_text, "\xf8%d\xfa/\xf8%d\xfa %s", gCount_killed_peds, gTotal_count_smash_peds, GetMiscString(eMiscString_kills));
            ChangeHeadupText(gPed_kill_count_headup, the_text);
        }
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Cars) {
            if (gQueued_wasted_messages_count != 0 && GetTotalTime() > gLast_wasted_message_start + 5000) {
                LoseOldestWastedMassage();
            }
            if (gQueued_wasted_messages_count != 0) {
                if (Flash(150, &gWasted_last_flash, &gWasted_flash_state)) {
                    sprintf(the_text, "\xf9%s %s", gOpponents[gQueued_wasted_messages[0]].abbrev_name, GetMiscString(eMiscString_wasted_46));
                } else {
                    sprintf(the_text, " ");
                }
            } else {
                if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                    oppo_count = GetCarCount(eVehicle_opponent);
                } else {
                    oppo_count = gCount_opponents;
                }
                sprintf(the_text, "%s \xf8%d\xfa/\xf8%d", GetMiscString(eMiscString_wasted), oppo_count - NumberOfOpponentsLeft(), oppo_count);
            }
            ChangeHeadupText(gCar_kill_count_headup, the_text);
        }
        TimerString(effective_timer, the_text, 1, 0, 0);
        ChangeHeadupText(gTimer_headup, the_text);
        if (gHeadup_detail_level == 1 || gHeadup_detail_level == 4) {
            sprintf(gHeadup_oppo_ped_text, "%i  %i  %i", GetCarCount(eVehicle_opponent) - NumberOfOpponentsLeft(), net_credits, gCount_killed_peds);
        }
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Checkpoints) {
            sprintf(the_text, "%s \xf8%d\xfa/\xf8%d \xfa%s \xf8%d\xfa/\xf8%d",
                GetMiscString(eMiscString_cp), gCheckpoint, gCheckpoint_count,
                GetMiscString(eMiscString_lap), gLap, gTotal_laps);
            ChangeHeadupText(gLaps_headup, the_text);
        }
        if (gCurrent_race.race_spec->race_type >= kRaceType_Smash) {
            sprintf(the_text, "%s \xf8%d\xfa/\xf8%d",
                GetMiscString(eMiscString_targets),
                GetRuntimeVariable(gCurrent_race.race_spec->options.smash.var_smash_number),
                gCurrent_race.race_spec->options.smash.var_smash_target);
            ChangeHeadupText(gLaps_headup, the_text);
        }
        the_time = GetTotalTime() - gTime_bonus_start;
        switch (gTime_bonus_state) {
        case eTime_bonus_none:
            break;
        case eTime_bonus_initial_pause:
            if (the_time >= 500) {
                bonus = gCompletion_bonus_post_race;
                sprintf(the_text, "%s %d", GetMiscString(eMiscString_completion_bonus_colon), bonus);
                DRS3StartSound(gPedestrians_outlet, eSoundId_Clap);
                ChangeHeadupText(gRace_bonus_headup, the_text);
                gProgram_state.credits += gCompletion_bonus_post_race;
                gTime_bonus_state = eTime_bonus_race_bonus;
                gTime_bonus_start = GetTotalTime();
            }
            gRace_finished = 10000;
            break;
        case eTime_bonus_race_bonus:
            if (the_time >= 2000) {
                gTime_bonus_state = eTime_bonus_tb_up;
                gTime_bonus_start = GetTotalTime();
                last_rattle_time = 0;
            }
            gRace_finished = 10000;
            break;
        case eTime_bonus_tb_up:
            if (gTimer != 0) {
                if (the_time - last_rattle_time > 15) {
                    previous_gtimer = gTimer;
                    gTimer -= 1000 * ((the_time - last_rattle_time) / 15);
                    if (gTimer < 0) {
                        gTimer = 0;
                    }
                    gTime_bonus += gCredits_per_second_time_bonus[gProgram_state.skill_level] * ((previous_gtimer - gTimer) / 1000);
                    last_rattle_time += 15 * ((the_time - last_rattle_time) / 15);
                }
                sprintf(the_text, "%s %d", GetMiscString(eMiscString_time_bonus_colon), gTime_bonus);
                ChangeHeadupText(gTime_bonus_headup, the_text);
            } else {
                gTime_bonus_state = eTime_bonus_tb_pause;
                gTime_bonus_start = GetTotalTime();
                last_rattle_time = 0;
            }
            gRace_finished = 10000;
            break;
        case eTime_bonus_tb_pause:
            if (the_time >= 1000) {
                gTime_bonus_state = eTime_bonus_tb_down;
                gTime_bonus_start = GetTotalTime();
                last_rattle_time = 0;
            }
            gRace_finished = 10000;
            break;
        case eTime_bonus_tb_down:
            if (gTime_bonus != 0) {
                if (the_time - last_rattle_time > 15) {
                    previous_time_bonus = gTime_bonus;
                    gTime_bonus -= (the_time - last_rattle_time) * gCredits_per_second_time_bonus[gProgram_state.skill_level] / 15;
                    if (gTime_bonus < 0) {
                        gTime_bonus = 0;
                    }
                    gProgram_state.credits += previous_time_bonus - gTime_bonus;
                    last_rattle_time += 15 * ((the_time - last_rattle_time) / 15);
                }
                sprintf(the_text, "%s %d", GetMiscString(eMiscString_time_bonus_colon), gTime_bonus);
                ChangeHeadupText(gTime_bonus_headup, the_text);
            } else {
                gTime_bonus_state = eTime_bonus_end_pause;
                gTime_bonus_start = GetTotalTime();
            }
            gRace_finished = 10000;
            break;
        case eTime_bonus_end_pause:
            if (the_time >= 2000 && gRace_finished > 1) {
                gRace_finished = 1;
            }
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00492680
// FUNCTION: CARMA2_SW 0x0048f8a0
// FUNCTION: CARMA2_CHEAT 0x00492620
void C2_HOOK_FASTCALL UpdateFramePeriod(tU32* pCamera_period) {
    tU32 new_tick_count;
    tU32 new_camera_tick_count;
    int error;

    if (gAction_replay_mode) {
        tU32 last_tick_count;
        tU32 camera_period;

        last_tick_count = gLast_tick_count;
        gFrame_period = abs((int)(gLast_replay_frame_time - last_tick_count));
        gLast_tick_count = gLast_replay_frame_time;
        new_camera_tick_count = PDGetTotalTime();
        new_tick_count = GetTotalTime();
        if (gOld_camera_time != 0) {
            camera_period = new_camera_tick_count - gOld_camera_time;
        } else {
            camera_period = 0;
        }
        *pCamera_period = camera_period;
        gOld_camera_time = new_camera_tick_count;
        if (gFrame_period != 0) {
            *pCamera_period = gFrame_period;
        }
    } else {
        int new_period;

        new_tick_count = PDGetTotalTime();
        gLast_tick_count += gFrame_period;
        error = new_tick_count - gLast_tick_count;
        gFrame_period = new_tick_count - gActual_last_tick_count;
        if (gFrame_period > 500 && new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
            gLast_tick_count = new_tick_count;
            if (gNet_mode != eNet_mode_none) {
                switch (gNet_mode) {
                case eNet_mode_client:
                    gProgram_state.current_car.collision_info->field_0x49c = 0;
                    break;
                case eNet_mode_host:
                    break;
                default:
                    break;
                }
            }
        }
        gAverage_frame_period = 9 * gAverage_frame_period / 10 + gFrame_period;
        if (new_tick_count - gRace_start > 2000) {
            gFrame_period = gAverage_frame_period / 10;
        }
        new_period = gFrame_period + error;
        if (new_period <= 0) {
            gLast_tick_count = new_tick_count;
        } else {
            gFrame_period = new_period;
        }
        *pCamera_period = gFrame_period;
        gActual_last_tick_count = new_tick_count;
    }
    if (gFrame_period < 10) {
        gFrame_period = 10;
        gLast_tick_count = new_tick_count;
    } else if (gFrame_period > 1000) {
        gFrame_period = 1000;
        gLast_tick_count = new_tick_count;
    }
    if (*pCamera_period < 10) {
        *pCamera_period = 10;
    } else if (*pCamera_period > 1000) {
        *pCamera_period = 1000;
    }
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00492830
// FUNCTION: CARMA2_SW 0x0048fa50
tU32 C2_HOOK_FASTCALL GetLastTickCount(void) {

    return gLast_tick_count;
}
#endif

inline void C2_HOOK_FASTCALL CheckTimer(void) {
    tS32 time_in_seconds;
    // GLOBAL: CARMA2_HW 0x0068c828
    // GLOBAL: CARMA2_SW 0x0068687c
    // GLOBAL: CARMA2_CHEAT 0x006940c8
    static tS32 last_time_in_seconds;

    if (gFreeze_timer || gCountdown || gRace_finished) {
        return;
    }
    if ((tU32)gTimer <= gFrame_period) {
        gTimer = 0;
        RaceCompleted(eRace_over_out_of_time);
    } else {
        if (gNet_mode == eNet_mode_none) {
            gTimer -= gFrame_period;
        }
        time_in_seconds = (gTimer + 500) / 1000;
        if (time_in_seconds != last_time_in_seconds && time_in_seconds <= 10) {
            DRS3StartSound(gPedestrians_outlet, eSoundId_warning_buzzer_noise);
        }
        last_time_in_seconds = time_in_seconds;
    }
}

inline int C2_HOOK_FASTCALL MungeRaceFinished(void) {

    if (!gRace_finished || gAction_replay_mode || (gNet_mode != eNet_mode_none && gRace_over_reason == eRace_not_over_yet)) {
        return 0;
    }
    if (gNet_mode == eNet_mode_none && gRace_over_reason <= eRace_over_3 && gCredits_post_race < 0) {
        gCredits_post_race = gProgram_state.credits;
        gCompletion_bonus_post_race = gCurrent_race.completion_bonus[gCurrent_race.race_spec->race_type == kRaceType_Carma1 ? gRace_over_reason : 0][gProgram_state.skill_level];
        gTotal_bonus_post_race = gCredits_per_second_time_bonus[gProgram_state.skill_level] * (gTimer / 1000) + gCompletion_bonus_post_race;
        if (!GetRuntimeVariable(99)) {
            NewTextHeadupSlot(4, 0, 3000, -4, GetMiscString(eMiscString_game_auto_saved));
        }
    }
    if (gRace_finished <= gFrame_period) {
        if (gTimer == 0 || gNet_mode != eNet_mode_none || gRace_over_reason == eRace_over_6 || GetRuntimeVariable(99)) {
            gRace_finished = 0;
            return 1;
        }
        gRace_finished = 15 * (gTimer + 300);
        gRace_bonus_headup = NewTextHeadupSlot(9, 0, 0, -4, "");
        gTime_bonus_headup = NewTextHeadupSlot(10, 0, 0, -4, "");
        gTime_bonus = 0;
        gTime_bonus_start = GetTotalTime();
        gTime_bonus_state = eTime_bonus_initial_pause;
    } else {
        gRace_finished -= gFrame_period;
    }
    return PDKeyDown(51) || PDKeyDown(52);
}

// FUNCTION: CARMA2_HW 0x004940e0
// FUNCTION: CARMA2_SW 0x004913e0
// FUNCTION: CARMA2_CHEAT 0x00494310
void C2_HOOK_FASTCALL CalculateCameraStuff(tU32 pCamera_period) {

    gOur_pos = &gPlayer_car_master_actor->t.t.translate.t;
    PositionExternalCamera(&gProgram_state.current_car, pCamera_period);
    BrActorToActorMatrix34(&gCamera_to_world, gCamera, gUniverse_actor);
    BrActorToActorMatrix34(&gRearview_camera_to_world, gRearview_camera, gUniverse_actor);
    gCamera_to_horiz_angle = FastScalarArcTan2(gCamera_to_world.m[2][1], gCamera_to_world.m[1][1]);
    gYon_squared = gYon_multiplier * gYon_multiplier * ((br_camera*)gCamera->type_data)->yon_z * ((br_camera*)gCamera->type_data)->yon_z;
    if ((gCamera_to_world.m[3][2] - gPrev_camera_position.v[2]) * (gCamera_to_world.m[3][2] - gPrev_camera_position.v[2])
            + ((gCamera_to_world.m[3][0] - gPrev_camera_position.v[0]) * (gCamera_to_world.m[3][0] - gPrev_camera_position.v[0])
                + (gCamera_to_world.m[3][1] - gPrev_camera_position.v[1]) * (gCamera_to_world.m[3][1] - gPrev_camera_position.v[1]))
        > 400.0) {
        ResetPedNearness();
    }
    BrVector3Copy(&gPrev_camera_position, (br_vector3*)gCamera_to_world.m[3]);
}

inline void C2_HOOK_FASTCALL CheckActiveCarList(void) {
    int i;
    int new_count;
    tCar_spec* car;

    new_count = 0;
    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_5) {
            if (new_count != i) {
                gActive_car_list[new_count] = car;
            }
            new_count++;
        }
    }
    gNum_active_cars = new_count;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00492980
// FUNCTION: CARMA2_SW 0x0048fbf0
// FUNCTION: CARMA2_CHEAT 0x00492920
tRace_result C2_HOOK_FASTCALL MainGameLoop(void) {
    tU32 start_menu_time;
    tU32 frame_start_time;
    tRace_result result;
    int tried_to_allocate_AR;
    int window_inactive;
    int i;

    tried_to_allocate_AR = 0;
    gCar_to_view = &gProgram_state.current_car;
    gCar_to_view_id = 0;
    gOld_camera_time = 0;
    gLast_net_auto_increase_credits = 0;
    gLast_credit_headup__mainloop = 0;
    gLast_time_headup = gTimer;
    gRace_bonus_headup = -1;
    gTime_bonus_headup = -1;
    gTime_bonus_start = 0;
    gTime_bonus_state = eTime_bonus_none;
    ARMainLoopStart();
    gLast_tick_count = PDGetTotalTime();
    gActual_last_tick_count = gLast_tick_count;
    gFrame_period = 100;
    gQueued_wasted_messages_count = 0;
    gWasted_flash_state = 0;
    gWasted_last_flash = 0;
    RevertPalette();
    gHost_abandon_game = 0;
    gNo_races_yet = 0;
    gRace_over_reason = eRace_not_over_yet;
    gMr_odo = 0.0f;
    gNo_credits_APO_restore = 0;
    gReceived_game_scores = 0;
    if (gNet_mode == eNet_mode_none) {
        gProgram_state.field_0x2c = 1;
    }
    gCredits_post_race = -1;
    ShowSpecialVolumesIfRequ();
    gProgram_state.racing = 1;
    if (gNet_mode == eNet_mode_host) {
        gCurrent_net_game->status.stage = eNet_game_playing;
    }
    NetPlayerStatusChanged(ePlayer_status_racing);
    GetAverageGridPosition(&gCurrent_race);
    ForceRebuildActiveCarList();
    PrintMemoryDump(0, "ABOUT TO ENTER MAINLOOP");
    UpdateFramePeriod(&gCamera_period);
    BrVector3Set(&gPrev_camera_position, 1000.0f, 1000.0f, 1000.0f);
    CalculateCameraStuff(gCamera_period);
    PDPageInProcessMemory();
    gRace_start = PDGetTotalTime();
    Timers_Init();
    if (gWait_for_it != 0) {
        gWait_for_it += GetTotalTime();
    }
    ClearEntireScreen();
    ResetPalette();
#ifdef REC2_SW
    gSaved_depth_buffer = gDepth_buffer;
    gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
    ScreenSmaller();
    ScreenLarger();
#endif
    do {
        Timers_StartFrame();
        EdgeTriggerModeOff();
        if (!gAction_replay_mode) {
            UpdateFramePeriod(&gCamera_period);
        }
        frame_start_time = GetTotalTime();
        CyclePollKeys();
        InitSmashQueue();
        CheckSystemKeys(1);
        InitialiseExtraRenders();
        NetReceiveAndProcessMessages();
        if (gHost_abandon_game || gProgram_state.prog_status == eProg_idling) {
            break;
        }
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox && gMap_view == 2) {
            ToggleMap();
        }
        ResetGrooveFlags();
        ServiceGameInRace();
        EnterUserMessage();
        ARService();
        ResetLollipopQueue();
        MungePalette();
        window_inactive = PDIsWindowInactive();
        if (window_inactive) {
            if (PDGetTotalTime() > gPrev_task_switched_message + 1000) {
                gPrev_task_switched_message = GetTotalTime();
                NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_task_switched));
            }
        } else if (gWindow_inactive) {
            ClearHeadupSlot(4);
            ClearQueuedHeadups();
        }
        gWindow_inactive = window_inactive;
        if (!gAction_replay_mode) {
            MungeAIWorld(gFrame_period);
            PollCarControls(gFrame_period);
        }
        if (!gAction_replay_mode) {
            CheckActiveCarList();
            Timers_Push(TIMER_PED);
            MungePedestrians(gFrame_period);
            Timers_Pop(TIMER_PED);
            ControlOurCar(gFrame_period);
            Timers_Push(TIMER_PHS);
            ApplyPhysicsToCars(gLast_tick_count - gRace_start, gFrame_period);
            Timers_Pop(TIMER_PHS);
            MungeSomeOtherCarGraphics();
            PipeCarPositions();
            Timers_Push(TIMER_CRS);
            CrushBendFlapRend();
            Timers_Pop(TIMER_CRS);
            DoNetGameManagement();
            CheckRecoveryOfCars(gLast_tick_count + gFrame_period - gRace_start);
            NetSendMessageStacks();
        } else {
            DoActionReplay(gFrame_period);
        }
        PollCameraControls(gCamera_period);
        MungeEngineNoise();
        CalculateCameraStuff(gCamera_period);
        MungeNapalm();
        if (!gAction_replay_mode) {
            DoPowerupPeriodics(gFrame_period);
        }
        if (!gAction_replay_mode) {
            CheckCheckpoints();
            MungeEnvironmentalSound();
        }
        ChangingView();
        MungeCarGraphics(gFrame_period);
        FunkThoseTronics();
        GrooveThoseDelics();
        DoWheelDamage(gFrame_period);
        CalculateFrameRate();
        CameraBugFix(&gProgram_state.current_car, gCamera_period);
        if (!gAction_replay_mode) {
            MungeHeadups();
            ProcessOilSpills(gFrame_period);
            MungeSmashEdgeTriggers(GetTotalTime());
        }
        MungeDelayedSideEffects();
        MungeShrapnel(gFrame_period);
        if (!gAction_replay_mode) {
            MungePowerupStuff(gFrame_period);
        }
        ChangeDepthEffect();
        ServiceGameInRace();
        EnterUserMessage();
        SkidsPerFrame();
        FlushSmashQueue(0);
        MungeGlassFragments();
        AnimateSky();
        MungeExplosions();
        LastChanceForPedEffects();
        DoLODCarModels();
        MungeLightingEffects();
        if (gWait_for_it != 0 && GetTotalTime() >= gWait_for_it) {
            gWait_for_it = 0;
        }
        if (gWait_for_it == 0) {
            Timers_Force(TIMER_RQQ);
            RenderAFrame(1);
            Timers_Force(TIMER_OQQ);
        }
        DoComplexCarModels();
        CheckReplayTurnOn();
        if (!gRecover_car
            && gProgram_state.prog_status == eProg_game_ongoing
            && gPalette_fade_time == 0
            && (gNet_mode == eNet_mode_none
                || !gAction_replay_mode
                || gProgram_state.current_car.car_master_actor->t.t.translate.t.v[0] < 500.0f)) {
            EnsureRenderPalette();
            EnsurePaletteUp();
        }
        if (KeyIsDown(0) && !gEntering_message) {
            WaitForNoKeys();
            if (gAction_replay_mode) {
                ToggleReplay(NULL, NULL);
            } else {
                if (gMap_view == 2) {
                    ToggleMap();
                }
                start_menu_time = PDGetTotalTime();
#ifdef REC2_SW
                if (gDepth_buffer != NULL) {
                    BrPixelmapFree(gDepth_buffer);
                    gDepth_buffer = gSaved_depth_buffer;
                }
#endif
                FadePaletteDown();
                ClearEntireScreen();
                GoingToInterfaceFromRace();
                DoOptionsMenu();
                GoingBackToRaceFromInterface();
                AddLostTime(PDGetTotalTime() - start_menu_time);
#ifdef REC2_SW
                AdjustRenderScreenSize();
                gSaved_depth_buffer = gDepth_buffer;
                gDepth_buffer = BrPixelmapMatch(gBack_screen, BR_PMMATCH_DEPTH_16);
#endif
            }
        }
        if (!gAction_replay_mode) {
            CheckTimer();
        } else {
            PollActionReplayControls(&gFrame_period, &gAverage_frame_period);
        }
        if (!gAction_replay_mode && gKnobbled_frame_period != 0) {
            while (GetTotalTime() - frame_start_time < gKnobbled_frame_period) {
            }
        }
        if (!tried_to_allocate_AR) {
            tried_to_allocate_AR = 1;
            PrintMemoryDump(0, "JUST RENDERED 1ST STUFF");
            InitialiseActionReplay();
            PrintMemoryDump(0, "JUST ALLOCATED ACTION REPLAY BUFFER");
        }
        if (gNet_mode == eNet_mode_client && gAbandon_game) {
            gProgram_state.prog_status = eProg_idling;
            gAbandon_game = 0;
        }
        Timers_EndFrame();
    } while (gProgram_state.prog_status == eProg_game_ongoing && !MungeRaceFinished() && !gAbandon_game && !gHost_abandon_game);
#ifdef REC2_SW
    if (gDepth_buffer != NULL) {
        BrPixelmapFree(gDepth_buffer);
        gDepth_buffer = gSaved_depth_buffer;
    }
#endif
    if (gCredits_post_race >= 0) {
        gProgram_state.credits = gTotal_bonus_post_race + gCredits_post_race;
    }
    PHILDisable();
    PrintMemoryDump(0, "JUST EXITED MAINLOOP");
    FadePaletteDown();
    ClearEntireScreen();
    SuspendPendingFlic();
    RevertPalette();
    if (gMap_view == 2) {
        ToggleMap();
    }
    EnsureSpecialVolumesHidden();
    FadePaletteDown();
    NetPlayerStatusChanged(ePlayer_status_loading);
    if (gAdditional_actors != NULL && gAdditional_actors->children != NULL) {
        AutoSaveAdditionalStuff();
    }
    if (gProgram_state.prog_status == eProg_game_ongoing) {
        ResetCarScreens();
    }
    if (gAbandon_game && gNet_mode == eNet_mode_host && gRace_over_reason < eRace_over_8) {
        NetFinishRace(gCurrent_net_game, eRace_over_abandoned);
    }
    if (gAction_replay_mode) {
        ToggleReplay(NULL, NULL);
    }
    if (!gHost_abandon_game && gProgram_state.prog_status == eProg_game_ongoing) {
        if (gAbandon_game == 1) {
            result = eRace_aborted;
        } else if (gRace_over_reason == eRace_over_out_of_time || gRace_over_reason == eRace_over_6 || gRace_over_reason == eRace_over_7) {
            result = eRace_timed_out;
        } else {
            result = eRace_completed;
        }
    } else {
        result = eRace_game_abandonned;
    }
    if (result < eRace_completed) {
        gProgram_state.redo_race_index = gProgram_state.current_race_index;
    } else {
        gProgram_state.redo_race_index = -1;
    }
    if (gNet_mode != eNet_mode_none && result == eRace_game_abandonned && !gAbandon_game && gProgram_state.prog_status != eProg_idling) {
        gProgram_state.prog_status = eProg_game_starting;
    }
    gAbandon_game = 0;
    gStart_race_sent = 0;
    gInitialised_grid = 0;
    gHost_abandon_game = 0;
    DRS3StopAllOutletSoundsExceptCDA();
    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            StopCarBeingIt(gNet_players[i].car);
        }
    }
    gProgram_state.racing = 0;
    if (gNet_mode == eNet_mode_host) {
        gCurrent_net_game->status.stage = eNet_game_stage0;
    }
    WaitForNoKeys();
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00492950
// FUNCTION: CARMA2_SW 0x0048fbc0
// FUNCTION: CARMA2_CHEAT 0x004928f0
tRace_result C2_HOOK_FASTCALL DoRace(void) {
    tRace_result result;

    if (PercentageChance(50)) {
        StartMusicTrack(9998);
    } else {
        StartMusicTrack(9997);
    }
    gRace_start = PDGetTotalTime();
    result = MainGameLoop();
    return result;
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "07-structur.h"

#include "00-car.h"
#include "01-network.h"
#include "02-init.h"
#include "03-racestrt.h"
#include "04-netgame.h"
#include "05-drmem.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "18b-graphics2.h"
#include "21-mainloop.h"
#include "23-piping.h"
#include "24-loadsave.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "31-racesumm.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "44-mainmenu.h"
#include "45-cutscene.h"
#include "51-smash.h"
#include "52-errors.h"
#include "53-controls.h"
#include "56-aiworld.h"
#include "66-tintedpoly.h"
#include "69-sound.h"
#include "72-interface.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "c2_stdio.h"

// GLOBAL: CARMA2_HW 0x00655e48
// GLOBAL: CARMA2_SW 0x00650d30
// GLOBAL: CARMA2_CHEAT 0x0065c6d0
int gHud_tinted1 = -1;

// GLOBAL: CARMA2_HW 0x00655e4c
// GLOBAL: CARMA2_SW 0x00650d34
// GLOBAL: CARMA2_CHEAT 0x0065c6d4
int gHud_tinted2 = -1;

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x00655e50
// GLOBAL: CARMA2_CHEAT 0x0065c6d8
int gHud_tinted3 = -1;
#endif

// GLOBAL: CARMA2_HW 0x00762140
// GLOBAL: CARMA2_SW 0x007586a0
// GLOBAL: CARMA2_CHEAT 0x007697e0
int gCredits_checkpoint[3];

// GLOBAL: CARMA2_HW 0x006aaa34
// GLOBAL: CARMA2_SW 0x006a4484
// GLOBAL: CARMA2_CHEAT 0x006b25ac
tU32 gLast_checkpoint_time;

// GLOBAL: CARMA2_HW 0x006aaa38
// GLOBAL: CARMA2_SW 0x006a4488
// GLOBAL: CARMA2_CHEAT 0x006b25b0
int gHud_tinted1_visible;

// GLOBAL: CARMA2_HW 0x006aaa3c
// GLOBAL: CARMA2_SW 0x006a448c
// GLOBAL: CARMA2_CHEAT 0x006b25b4
int gHud_tinted2_visible;

// GLOBAL: CARMA2_HW 0x00660cdc
// GLOBAL: CARMA2_SW 0x0065b650
// GLOBAL: CARMA2_CHEAT 0x00668524
int gLast_wrong_checkpoint = -1;

// GLOBAL: CARMA2_HW 0x00660ce0
// GLOBAL: CARMA2_SW 0x0065b654
// GLOBAL: CARMA2_CHEAT 0x00668528
int gMirror_on__structur = 1;

// GLOBAL: CARMA2_HW 0x00660ce8
// GLOBAL: CARMA2_SW 0x0065b65c
// GLOBAL: CARMA2_CHEAT 0x00668530
int gLoad_last_save_game = 1;

// FUNCTION: CARMA2_HW 0x00503050
// FUNCTION: CARMA2_SW 0x004fe9a0
// FUNCTION: CARMA2_CHEAT 0x00508cf0
int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int result;
    int count;
    int i;
    tCar_spec* car_spec;

    result = 0;
    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        car_spec = GetCarSpec(eVehicle_opponent, i);

        if (!car_spec->knackered && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || (gCurrent_race.race_spec->race_type == kRaceType_Cars && car_spec->is_race_goal))) {
            result += 1;
        }
    }
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x005030b0
// FUNCTION: CARMA2_SW 0x004fea00
// FUNCTION: CARMA2_CHEAT 0x00508d50
void C2_HOOK_FASTCALL RaceCompleted(tRace_over_reason pReason) {

    if (!gRace_finished) {
        if (gNet_mode == eNet_mode_host && pReason < eRace_over_8) {
            NetFinishRace(gCurrent_net_game, pReason);
        }
        if (pReason == eRace_over_out_of_time || pReason == eRace_over_7) {
            ChangeAmbientPratcam(35);
        } else if (pReason < eRace_over_abandoned) {
            ChangeAmbientPratcam(34);
        }
        gRace_over_reason = pReason;
        if (gMap_view == 2) {
            ToggleMap();
        }
        switch (gRace_over_reason) {
        case eRace_over_8:
            ChangeAmbientPratcam(34);
            DoFancyHeadup(29);
            break;
        case eRace_over_9:
            ChangeAmbientPratcam(36);
            DoFancyHeadup(26);
            break;
        case eRace_over_out_of_time:
            ChangeAmbientPratcam(35);
            if (gCurrent_race.race_spec->is_boundary) {
                DoFancyHeadup(33);
            } else {
                DoFancyHeadup(22);
            }
            DRS3StartSound(gPedestrians_outlet, eSoundId_OutOfTime2);
            break;
        case eRace_over_7:
            ChangeAmbientPratcam(35);
            DoFancyHeadup(35);
            break;
        case eRace_over_0:
        case eRace_over_1:
        case eRace_over_2:
        case eRace_over_3:
            ChangeAmbientPratcam(34);
            if (gCurrent_race.race_spec->is_boundary) {
                DoFancyHeadup(34);
            } else {
                DoFancyHeadup(23);
            }
            DRS3StartSound(gPedestrians_outlet, eSoundId_RaceComplete);
            break;
        case eRace_over_abandoned:
            if (gNet_mode == eNet_mode_client) {
                gHost_abandon_game = 1;
                NetFullScreenMessage(eMiscString_host_abandoned_race, 0);
            }
            break;
        case eRace_over_6:
            ChangeAmbientPratcam(36);
            if (gCurrent_race.race_spec->is_boundary) {
                DoFancyHeadup(33);
            } else {
                DoFancyHeadup(24);
            }
            break;
        default:
            break;
        }
        if (gNet_mode != eNet_mode_none) {
            gRace_finished = 8000;
        } else {
            gRace_finished = 10000;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00503270
// FUNCTION: CARMA2_SW 0x004febc0
// FUNCTION: CARMA2_CHEAT 0x00508f10
void C2_HOOK_FASTCALL Checkpoint(int pCheckpoint_index, int pDo_sound) {
    (void)pCheckpoint_index;

    PratcamEvent(33);
    DoFancyHeadup(21);
    if (pDo_sound) {
        DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
    }
}

// FUNCTION: CARMA2_HW 0x005032a0
// FUNCTION: CARMA2_SW 0x004febf0
// FUNCTION: CARMA2_CHEAT 0x00508f40
void C2_HOOK_FASTCALL IncrementCheckpoint(void) {
    int done_voice;

    done_voice = 0;
    if (gRace_finished) {
        return;
    }
    if (gCurrent_race.race_spec->race_type != kRaceType_Carma1
            && gCurrent_race.race_spec->race_type != kRaceType_Checkpoints) {
        return;
    }
    gLast_checkpoint_time = GetTotalTime();
    if (gCheckpoint >= gCheckpoint_count) {
        gCheckpoint = 1;
        if (gLap++ >= gTotal_laps) {
            gLap = gTotal_laps;
            gCheckpoint = gCheckpoint_count;
            RaceCompleted(eRace_over_0);
        } else if (gLap == gTotal_laps) {
            PratcamEvent(33);
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_final_lap));
            DRS3StartSound(gPedestrians_outlet, eSoundId_FinalLap);
            done_voice = 1;
        }
    } else {
        gCheckpoint += 1;
    }
    if (!gRace_finished) {
        PratcamEvent(33);
        DoFancyHeadup(21);
        if (!done_voice) {
            DRS3StartSound(gPedestrians_outlet, eSoundId_Checkpoint);
        }
        if (gCredits_checkpoint[gProgram_state.skill_level] != 0) {
            EarnCredits(gCredits_checkpoint[gProgram_state.skill_level]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x005033b0
// FUNCTION: CARMA2_SW 0x004fed00
// FUNCTION: CARMA2_CHEAT 0x00509050
void C2_HOOK_FASTCALL IncrementLap(void) {
    int i;

    for (i = gCheckpoint; i <= gCheckpoint_count; i++) {
        IncrementCheckpoint();
    }
}

// RayHitFace

// FUNCTION: CARMA2_HW 0x005033e0
// FUNCTION: CARMA2_SW 0x004fed30
// FUNCTION: CARMA2_CHEAT 0x00509080
void C2_HOOK_FASTCALL WrongCheckpoint(int pCheckpoint_index) {
    int next_checkpoint;

    if (gRace_finished) {
        return;
    }
    if (gCurrent_race.race_spec->race_type != kRaceType_Carma1
            && gCurrent_race.race_spec->race_type != kRaceType_Checkpoints) {
        return;
    }
    if ((gLast_wrong_checkpoint == pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 20000)
            || (gLast_wrong_checkpoint != pCheckpoint_index && GetTotalTime() - gLast_checkpoint_time > 2000)) {
        if (gNet_mode == eNet_mode_none) {
            next_checkpoint = pCheckpoint_index + 2;
            if (next_checkpoint > gCurrent_race.check_point_count) {
                if (gLap == 1) {
                    next_checkpoint = -1;
                } else {
                    next_checkpoint = 1;
                }
            }
            if (next_checkpoint == gCheckpoint) {
                return;
            }
        }
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_wrong_checkpoint));
        DRS3StartSound(gPedestrians_outlet, eSoundId_WrongCheckpoint);
        gLast_checkpoint_time = GetTotalTime();
        gLast_wrong_checkpoint = pCheckpoint_index;
    }
}

// CheckCheckpoints

// FUNCTION: CARMA2_HW 0x005039b0
// FUNCTION: CARMA2_SW 0x004ff300
void C2_HOOK_FASTCALL TotalRepair(void) {

    TotallyRepairCar();
    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_instant_repair));
}

inline void C2_HOOK_FASTCALL DoLogos(void) {

    ClearEntireScreen();
    DoSCILogo();
    DoOpeningAnimation();
    DoStainlessLogo();
    gProgram_state.prog_status = eProg_opening;
}

inline void C2_HOOK_FASTCALL DoProgOpeningAnimation(void) {

    gProgram_state.prog_status = eProg_idling;
    DRS3StopOutletSound(gEffects_outlet);
}

inline void C2_HOOK_FASTCALL DoProgramDemo(void) {

    DoLogos();
    gProgram_state.prog_status = eProg_idling;
    DRS3StopOutletSound(gEffects_outlet);
}

// FUNCTION: CARMA2_HW 0x00503b00
// FUNCTION: CARMA2_SW 0x004ff450
// FUNCTION: CARMA2_CHEAT 0x005097a0
int C2_HOOK_FASTCALL ChooseOpponent(int pNastiness) {
    int i;
    int count;
    int temp_array[40];

    while (1) {
        count = 0;
        for (i = 0; i < gNumber_of_racers; i++) {
            if ((pNastiness == 0 || gOpponents[i].strength_rating == pNastiness)
                && gProgram_state.current_car_index != i
                && !gOpponents[i].picked
                && gOpponents[i].strength_rating <= 5
                && gOpponents[i].strength_rating >= 1) {
                temp_array[count++] = i;
            }
        }
        if (count != 0) {
            break;
        }
        if (pNastiness < 5) {
            pNastiness++;
        } else {
            pNastiness = 0;
        }
    }
    i = temp_array[IRandomBetween(0, count - 1)];
    gOpponents[i].picked = 1;
    return i;
}

// FUNCTION: CARMA2_HW 0x005039e0
// FUNCTION: CARMA2_SW 0x004ff330
// FUNCTION: CARMA2_CHEAT 0x00509680
void C2_HOOK_FASTCALL SelectOpponents(tRace_info* pRace_info) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        pRace_info->number_of_racers = pRace_info->race_spec->count_explicit_opponents;
        for (i = 0; i < gNumber_of_racers; i++) {
            gOpponents[i].picked = 0;
        }
        for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
            if (pRace_info->race_spec->explicit_opponents[i] >= 0) {
                gOpponents[pRace_info->race_spec->explicit_opponents[i]].picked = 1;
                pRace_info->opponent_list[i].index = pRace_info->race_spec->explicit_opponents[i];
                pRace_info->opponent_list[i].ranking = -(i + 1);
            }
        }
        for (i = 0; i < pRace_info->race_spec->count_explicit_opponents; i++) {
            if (pRace_info->race_spec->explicit_opponents[i] < 0) {
                pRace_info->opponent_list[i].index = ChooseOpponent(-pRace_info->race_spec->explicit_opponents[i]);
                pRace_info->opponent_list[i].ranking = IRandomBetween(0, 99);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00503ba0
// FUNCTION: CARMA2_SW 0x004ff4f0
// FUNCTION: CARMA2_CHEAT 0x00509840
int C2_HOOK_FASTCALL PickNetRace(int pCurrent_race, tNet_sequence_type pNet_race_sequence) {
    int i;
    int new_index;
    int races_count;
    int most_seldom_seen;
    int races_to_pick_from[50];

    if (pNet_race_sequence == eNet_sequence_sequential) {
        pCurrent_race++;
        if (pCurrent_race >= gNumber_of_races) {
            pCurrent_race = 0;
        }
        return pCurrent_race;
    } else {
        most_seldom_seen = 10000;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents < most_seldom_seen) {
                most_seldom_seen = gRace_list[i].count_opponents;
            }
        }
        races_count = 0;
        for (i = 0; i < gNumber_of_races; i++) {
            if (gRace_list[i].count_opponents == most_seldom_seen && i != pCurrent_race) {
                races_to_pick_from[races_count] = i;
                races_count++;
            }
        }
        new_index = races_to_pick_from[IRandomBetween(0, races_count - 1)];
        gRace_list[new_index].count_opponents++;
        return new_index;
    }
}

inline void C2_HOOK_FASTCALL SwapNetCarsLoad(void) {
    int i;

    DisableNetService();
    AboutToLoadFirstCar();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].next_car_index >= 0) {
            gNet_players[i].car_index = gNet_players[i].next_car_index;
        }
        gNet_players[i].next_car_index = -1;
        LoadCar(gOpponents[gNet_players[i].car_index].car_file_name,
            (i == gThis_net_player_index) ? eDriver_local_human : eDriver_net_human,
            gNet_players[i].car, gNet_players[i].car_index, gNet_players[i].player_name,
            &gNet_cars_storage_space);
    }
    ReenableNetService();
}

inline void C2_HOOK_FASTCALL SwapNetCarsDispose(void) {
    int i;

    DisableNetService();
    for (i = 0; i < gNumber_of_net_players; i++) {
        DisposeCar(gNet_players[i].car, gNet_players[i].car_index);
        if (gNet_players[i].car_index >= 0) {
            gCar_details[gNet_players[i].car_index].ownership = eCar_owner_none;
        }
    }
    ClearOutStorageSpace(&gOur_car_storage_space);
    ClearOutStorageSpace(&gNet_cars_storage_space);
    ReenableNetService();
}

inline void C2_HOOK_FASTCALL DoGame(void) {
    tRace_result race_result;
    int game_completed;
    int i;

    PrintMemoryDump(0, "START OF DoGame()");
    gAbandon_game = 0;
    gDisallow_abandon_race = 0;
    gCar_to_view = &gProgram_state.current_car;
    StartLoadingScreen();
    gProgram_state.prog_status = eProg_game_ongoing;
    if (gNet_mode_of_last_game != gNet_mode) {
        gProgram_state.prog_status = eProg_idling;
    } else {
        PrintMemoryDump(0, "BEFORE START RACE SCREEN");
        LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
        SelectOpponents(&gCurrent_race);
        if (gNet_mode != eNet_mode_none) {
            FillInRaceInfo(&gCurrent_race);
            DisposeRaceInfo(&gCurrent_race);
        } else {
            SelectOpponents(&gCurrent_race);
            LoadRaceInfo(gProgram_state.current_race_index, &gCurrent_race);
            FillInRaceInfo(&gCurrent_race);
        }
        PrintMemoryDump(0, "AFTER START RACE SCREEN");
        if (gNet_mode != eNet_mode_none
            && (gCurrent_net_game->options.car_choice == eNet_car_all || gCurrent_net_game->options.car_choice == eNet_car_novelty)
            && !gNo_races_yet) {
            SwapNetCarsLoad();
        }
        PrintMemoryDump(0, "AFTER LOADING OPPONENTS IN");
        InitRace();
        if (gNet_mode_of_last_game != gNet_mode) {
            gProgram_state.prog_status = eProg_idling;
        } else {
            if (gNet_mode != eNet_mode_none) {
                FRONTEND_Main(kFrontend_menu_netsync);
                dr_dprintf("Returned from NetSync screen");
                SwitchToLoresMode();
            } else {
                SortOpponents();
                SetInitialPositions(&gCurrent_race);
            }
            if (gProgram_state.prog_status == eProg_idling) {
                dr_dprintf("Calling DisposeRace()");
                DisposeRace();
                dr_dprintf("Calling DisposeTrack()");
                DisposeTrack();
                dr_dprintf("Returning from DoGame()");
            } else {
                InitialiseAIWorldForRace(&gCurrent_race);
                InitialiseCarsEtc(&gCurrent_race);
                InitSoundSources();
                InitLastDamageArrayEtc();
                FogificateMaterials(kRendererShadingType_Default);
                EnableAutoFogification();
                RecordAPOAndCredits();
                race_result = DoRace();
                DisableAutoFogification();
                DisposeRace();
                PrintMemoryDump(0, "AFTER DISPOSING RACE");
                if (gNet_mode != eNet_mode_none) {
                    gProgram_state.current_race_index = gPending_race;
                    gCurrent_net_game->start_race = gProgram_state.current_race_index;
                    gPending_race = -1;
                    if (race_result != eRace_game_abandonned && race_result != eRace_aborted) {
                        DoEndRaceSummary(race_result, 0);
                    }
                } else {
                    game_completed = MungeRaceProgression(race_result);
                    if (race_result == eRace_completed) {
                        gSave_game_out_of_sync = 1;
                        DoEndRaceAnimation();
                        DoEndRaceSummary(race_result, game_completed);
                        DoSaveGame();
                    }
                }
                if (gNet_mode != eNet_mode_none) {
                    for (i = 0; i < gNumber_of_net_players; i++) {
                        TotallyRepairACar(gNet_players[i].car);
                    }
                } else {
                    TotallyRepairCar();
                }
                if (gNet_mode == eNet_mode_none) {
                    DisposeOpponentsCars(&gCurrent_race);
                } else if ((gCurrent_net_game->options.car_choice == eNet_car_all || gCurrent_net_game->options.car_choice == eNet_car_novelty)
                           && !gNo_races_yet) {
                    SwapNetCarsDispose();
                }
                PrintMemoryDump(0, "AFTER DISPOSING CARS");
                DisposeTrack();
                PrintMemoryDump(0, "AFTER DISPOSING TRACK");
                if (gProgram_state.prog_status == eProg_game_ongoing) {
                    if (gNet_mode == eNet_mode_none) {
                        gProgram_state.prog_status = eProg_idling;
                    } else {
                        gProgram_state.prog_status = eProg_game_starting;
                    }
                }
            }
        }
    }
}

inline void C2_HOOK_FASTCALL InitialiseProgramState(void) {

    gProgram_state.prog_status = eProg_intro;
    gProgram_state.last_slot = 0;
    gProgram_state.view_type = eVT_Scene;
    gProgram_state.frank_or_anniness = eFrankie;
    gProgram_state.racing = 0;
    gProgram_state.cut_scene = 0;
    gProgram_state.saving = 0;
    gProgram_state.loading = 0;
    gProgram_state.dont_save_or_load = 0;
    gProgram_state.dont_load = 0;
    gProgram_state.mirror_on = gMirror_on__structur;
    gProgram_state.field_0x2c = 0;
    if (gAusterity_mode) {
        gPratcam_on = 0;
        gProgram_state.prat_cam_on = 0;
    } else {
        gProgram_state.prat_cam_on = gPratcam_on;
    }
    gProgram_state.cockpit_on = gCockpit_on;
    gProgram_state.car_name[0] = '\0';
    SetSoundVolumes(0);
}

// FUNCTION: CARMA2_HW 0x00503c50
// FUNCTION: CARMA2_SW 0x004ff5a0
// FUNCTION: CARMA2_CHEAT 0x005098f0
void C2_HOOK_FASTCALL DoProgram(void) {

    InitialiseProgramState();
    do {
        switch (gProgram_state.prog_status) {
        case eProg_intro:
            DisposeGameIfNecessary();
            DoLogos();
            break;
        case eProg_opening:
            DisposeGameIfNecessary();
            DoProgOpeningAnimation();
            break;
        case eProg_idling:
            DisposeGameIfNecessary();
            if (gNo_current_game) {
                LoadRaces(gRace_list, &gNumber_of_races, -1);
                InitGame(gDev_initial_race);
                MaybeRestoreSavedGame();
            }
            if (gAuto_load && gLoad_last_save_game) {
                DoLoadMostRecentGame();
                gLoad_last_save_game = 0;
            }
            DoMainScreen();
            break;
        case eProg_demo:
            DoProgramDemo();
            break;
        case eProg_game_starting:
            DoGame();
            break;
        default:
            break;
        }
    } while (gProgram_state.prog_status != eProg_quit);
}

// FUNCTION: CARMA2_HW 0x00504170
// FUNCTION: CARMA2_SW 0x004ffac0
// FUNCTION: CARMA2_CHEAT 0x00509e10
void C2_HOOK_FASTCALL JumpTheStart(void) {
    char s[256];

    if (gNet_mode != eNet_mode_none && gProgram_state.credits < gJump_start_fine[gProgram_state.skill_level]) {
        return;
    }
    WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(gCountdown);
    gCountdown = 0;
    DRS3StopOutletSound(gPedestrians_outlet);
    DRS3StartSound(gPedestrians_outlet, eSoundId_HeyYou);
    SpendCredits(gJump_start_fine[gProgram_state.skill_level]);
    sprintf(s, "%s %d %s", GetMiscString(eMiscString_bad_boy), gJump_start_fine[gProgram_state.skill_level], GetMiscString(eMiscString_credit_fine));
    NewTextHeadupSlot(4, 0, 1000, -4, s);
}

// FUNCTION: CARMA2_HW 0x00504230
// FUNCTION: CARMA2_SW 0x004ffb80
// FUNCTION: CARMA2_CHEAT 0x00509ed0
void C2_HOOK_FASTCALL GoingToInterfaceFromRace(void) {

    gInterface_within_race_mode = 1;
    gHud_tinted1_visible = !!TintedPolyIsOn(gHud_tinted1);
    gHud_tinted2_visible = !!TintedPolyIsOn(gHud_tinted2);
    TurnTintedPolyOff(gHud_tinted1);
    TurnTintedPolyOff(gHud_tinted2);
    PlayFlicsFromDisk();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x005042a0
// FUNCTION: CARMA2_SW 0x004ffbf0
// FUNCTION: CARMA2_CHEAT 0x00509f40
void C2_HOOK_FASTCALL GoingBackToRaceFromInterface(void) {

    gInterface_within_race_mode = 0;
    if (gHud_tinted1_visible) {
        TurnTintedPolyOn(gHud_tinted1);
    }
    if (gHud_tinted2_visible) {
        TurnTintedPolyOn(gHud_tinted2);
    }
    SwitchToLoresMode();
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
}


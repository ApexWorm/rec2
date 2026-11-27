/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "01-network.h"

#include "13-crush1.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "28-world3.h"
#include "32-spark.h"
#include "47-win95net.h"
#include "52-errors.h"
#include "53-controls.h"
#include "68-timer.h"
#include "platform.h"
#include "globvars.h"
#include "globvrpb.h"
#include "04-netgame.h"
#include "41-utility.h"

#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"
#include <stddef.h>

#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <wsipx.h>

// GLOBAL: CARMA2_HW 0x0068d95c
// GLOBAL: CARMA2_SW 0x006879b4
// GLOBAL: CARMA2_CHEAT 0x006951fc
int gJoin_poll_index;

// GLOBAL: CARMA2_HW 0x0068d964
// GLOBAL: CARMA2_SW 0x006879bc
// GLOBAL: CARMA2_CHEAT 0x00695204
int gReceiving_batch_number;

// GLOBAL: CARMA2_HW 0x0068d974
// GLOBAL: CARMA2_SW 0x006879cc
// GLOBAL: CARMA2_CHEAT 0x00695214
tU32 gLast_player_list_received;

// GLOBAL: CARMA2_HW 0x0068d978
// GLOBAL: CARMA2_SW 0x006879d0
// GLOBAL: CARMA2_CHEAT 0x00695218
int gBastard_has_answered;

// GLOBAL: CARMA2_HW 0x0068d984
// GLOBAL: CARMA2_SW 0x006879dc
// GLOBAL: CARMA2_CHEAT 0x00695224
tU32 gAsk_time;

// GLOBAL: CARMA2_HW 0x0068d988
// GLOBAL: CARMA2_SW 0x006879e0
// GLOBAL: CARMA2_CHEAT 0x00695228
int gJoin_request_denied;

// GLOBAL: CARMA2_HW 0x0068d98c
// GLOBAL: CARMA2_SW 0x006879e4
// GLOBAL: CARMA2_CHEAT 0x0069522c
int gMessage_header_size;

// GLOBAL: CARMA2_HW 0x00690c80
// GLOBAL: CARMA2_SW 0x0068acd8
// GLOBAL: CARMA2_CHEAT 0x00698520
tU32 gLast_status_broadcast;

// GLOBAL: CARMA2_HW 0x00690238
// GLOBAL: CARMA2_SW 0x0068a290
// GLOBAL: CARMA2_CHEAT 0x00697ad8
int gTime_for_next_one;

// GLOBAL: CARMA2_HW 0x00690234
// GLOBAL: CARMA2_SW 0x0068a28c
// GLOBAL: CARMA2_CHEAT 0x00697ad4
int gCar_was_taken;

// GLOBAL: CARMA2_HW 0x00690240
// GLOBAL: CARMA2_SW 0x0068a298
// GLOBAL: CARMA2_CHEAT 0x00697ae0
tNet_game_player_info gNew_net_players[12];

// GLOBAL: CARMA2_HW 0x00690c30
// GLOBAL: CARMA2_SW 0x0068ac88
// GLOBAL: CARMA2_CHEAT 0x006984d0
int gReceiving_new_players;

// GLOBAL: CARMA2_HW 0x00690c34
// GLOBAL: CARMA2_SW 0x0068ac8c
// GLOBAL: CARMA2_CHEAT 0x006984d4
tNet_game_details* gCurrent_join_poll_game;

// GLOBAL: CARMA2_HW 0x00690c38
// GLOBAL: CARMA2_SW 0x0068ac90
// GLOBAL: CARMA2_CHEAT 0x006984d8
int gNet_service_disable;

// GLOBAL: CARMA2_HW 0x00690c3c
// GLOBAL: CARMA2_SW 0x0068ac94
// GLOBAL: CARMA2_CHEAT 0x006984dc
int gIn_net_service;

// GLOBAL: CARMA2_HW 0x00690c40
// GLOBAL: CARMA2_SW 0x0068ac98
// GLOBAL: CARMA2_CHEAT 0x006984e0
int gPlayer_list_batch_number;

// GLOBAL: CARMA2_HW 0x00690c48
// GLOBAL: CARMA2_SW 0x0068aca0
// GLOBAL: CARMA2_CHEAT 0x006984e8
tNet_message_memory* gDynamic_messages;

// GLOBAL: CARMA2_HW 0x00690c4c
// GLOBAL: CARMA2_SW 0x0068aca4
// GLOBAL: CARMA2_CHEAT 0x006984ec
tU32 gLast_flush_message;

// GLOBAL: CARMA2_HW 0x00690c54
// GLOBAL: CARMA2_SW 0x0068acac
// GLOBAL: CARMA2_CHEAT 0x006984f4
int gJoin_list_mode;

// GLOBAL: CARMA2_HW 0x0074b7dc
// GLOBAL: CARMA2_SW 0x00741e9c
// GLOBAL: CARMA2_CHEAT 0x00752e7c
tPlayer_ID gLocal_net_ID;

// GLOBAL: CARMA2_HW 0x00690c44
// GLOBAL: CARMA2_SW 0x0068ac9c
// GLOBAL: CARMA2_CHEAT 0x006984e4
int gOnly_receive_guarantee_replies;

// GLOBAL: CARMA2_HW 0x0068d97c
// GLOBAL: CARMA2_SW 0x006879d4
// GLOBAL: CARMA2_CHEAT 0x0069521c
tMin_message* gMin_messages;

// GLOBAL: CARMA2_HW 0x0068d970
// GLOBAL: CARMA2_SW 0x006879c8
// GLOBAL: CARMA2_CHEAT 0x00695210
tMid_message* gMid_messages;

// GLOBAL: CARMA2_HW 0x0068d980
// GLOBAL: CARMA2_SW 0x006879d8
// GLOBAL: CARMA2_CHEAT 0x00695220
tMax_message* gMax_messages;

// GLOBAL: CARMA2_HW 0x0068d96c
// GLOBAL: CARMA2_SW 0x006879c4
// GLOBAL: CARMA2_CHEAT 0x0069520c
int gNet_initialised;

// GLOBAL: CARMA2_HW 0x0068d960
// GLOBAL: CARMA2_SW 0x006879b8
// GLOBAL: CARMA2_CHEAT 0x00695200
int gDont_allow_joiners;

// GLOBAL: CARMA2_HW 0x0068d968
// GLOBAL: CARMA2_SW 0x006879c0
// GLOBAL: CARMA2_CHEAT 0x00695208
int gHost_died;

// GLOBAL: CARMA2_HW 0x0074a690
// GLOBAL: CARMA2_SW 0x00741150
// GLOBAL: CARMA2_CHEAT 0x00751d30
tU32 gUINT_0074a690;

// GLOBAL: CARMA2_HW 0x0074a694
// GLOBAL: CARMA2_SW 0x00741154
// GLOBAL: CARMA2_CHEAT 0x00751d34
int gINT_0074a694;

// GLOBAL: CARMA2_HW 0x0074a718
// GLOBAL: CARMA2_SW 0x007411d8
// GLOBAL: CARMA2_CHEAT 0x00751db8
tU32 gUINT_0074a718;

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x0074a71c
br_actor* gNetscore_box_universe;

// GLOBAL: CARMA2_HW 0x0074a720
br_material* gNetscore_box_materials[4] = { 0 };

// GLOBAL: CARMA2_HW 0x0074a730
br_actor* gNetscore_box_camera = NULL;
#endif

// GLOBAL: CARMA2_HW 0x0074a734
// GLOBAL: CARMA2_SW 0x007411f4
// GLOBAL: CARMA2_CHEAT 0x00751dd4
int gIt_or_fox;

// GLOBAL: CARMA2_HW 0x0074a738
// GLOBAL: CARMA2_SW 0x007411f8
// GLOBAL: CARMA2_CHEAT 0x00751dd8
int gInitialised_grid;

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x0074a73c
br_model* gNetScore_box_model;

// GLOBAL: CARMA2_HW 0x0074a740
br_actor* gNetscore_box_actors[12] = { 0 };
#endif

// GLOBAL: CARMA2_HW 0x0074a770
int gINT_0074a770 = 0;

// GLOBAL: CARMA2_HW 0x00659d30
// GLOBAL: CARMA2_SW 0x00654648
// GLOBAL: CARMA2_CHEAT 0x00660650
tU16 gGuarantee_number = 1;

// GLOBAL: CARMA2_HW 0x00659d34
// GLOBAL: CARMA2_SW 0x0065464c
// GLOBAL: CARMA2_CHEAT 0x00660654
int INT_00659d34 = -1;

// GLOBAL: CARMA2_HW 0x0068d958
// GLOBAL: CARMA2_SW 0x006879b0
// GLOBAL: CARMA2_CHEAT 0x006951f8
int gNext_guarantee;

// GLOBAL: CARMA2_HW 0x0068d990
// GLOBAL: CARMA2_SW 0x006879e8
tGuaranteed_message gGuarantee_list[200] = { 0 };

// GLOBAL: CARMA2_HW 0x00690230
// GLOBAL: CARMA2_SW 0x0068a288
// GLOBAL: CARMA2_CHEAT 0x00697ad0
tAddToJoinListProc* gAdd_proc = NULL;

// GLOBAL: CARMA2_HW 0x00690c60
// GLOBAL: CARMA2_SW 0x0068acb8
// GLOBAL: CARMA2_CHEAT 0x00698500
int rr_min;

// GLOBAL: CARMA2_HW 0x00690c64
// GLOBAL: CARMA2_SW 0x0068acbc
// GLOBAL: CARMA2_CHEAT 0x00698504
int rr_mid;

// GLOBAL: CARMA2_HW 0x00690c68
// GLOBAL: CARMA2_SW 0x0068acc0
// GLOBAL: CARMA2_CHEAT 0x00698508
int rr_max;

// FUNCTION: CARMA2_HW 0x0049d210
// FUNCTION: CARMA2_SW 0x00499390
// FUNCTION: CARMA2_CHEAT 0x0049d5c0
int C2_HOOK_FASTCALL NetInitialise(void) {
    int i;

    InitAbuseomatic();
    gNet_service_disable = 0;
    gIn_net_service = 0;
    gMessage_header_size = PDNetGetHeaderSize();
    gOnly_receive_guarantee_replies = 0;
    gMin_messages = BrMemAllocate(MIN_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMin_message)), kMem_net_min_messages);
    gMid_messages = BrMemAllocate(MID_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMid_message)), kMem_net_mid_messages);
    gMax_messages = BrMemAllocate(MAX_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMax_message)), kMem_net_max_messages);

    for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMin_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMid_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMax_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    gNet_initialised = PDNetInitialise() == 0;
    if (gNet_initialised) {
        InitNetHeadups();
    }
    GenerateItFoxShadeTable();
    gDont_allow_joiners = 0;
    gUINT_0074a690 = PDGetTotalTime();
    gUINT_0074a718 = PDGetTotalTime();
    gNext_guarantee = 0;
    gDynamic_messages = NULL;
    gGuarantee_number = ((tU16)PDGetTotalTime() & 0x3ff) + 1;
    return !gNet_initialised;
}

int C2_HOOK_FASTCALL NetShutdown(void) {
    int err;

    err = PDNetShutdown();
    DisposeAbuseomatic();
    BrMemFree(gMin_messages);
    BrMemFree(gMid_messages);
    BrMemFree(gMax_messages);
    DisposeExcessMemory();
    DisposeNetHeadups();
    return err;
}

// FUNCTION: CARMA2_HW 0x0049d370
// FUNCTION: CARMA2_SW 0x004994f0
// FUNCTION: CARMA2_CHEAT 0x0049d720
void C2_HOOK_FASTCALL ShutdownNetIfRequired(void) {

    if (gNet_initialised) {
        NetShutdown();
        gNet_initialised = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0049d3e0
// FUNCTION: CARMA2_SW 0x00499560
// FUNCTION: CARMA2_CHEAT 0x0049d790
void C2_HOOK_FASTCALL DisableNetService(void) {

    gNet_service_disable = 1;
}

// FUNCTION: CARMA2_HW 0x0049d3f0
// FUNCTION: CARMA2_SW 0x00499570
// FUNCTION: CARMA2_CHEAT 0x0049d7a0
void C2_HOOK_FASTCALL ReenableNetService(void) {

    gNet_service_disable = 0;
}

// FUNCTION: CARMA2_HW 0x0049d400
// FUNCTION: CARMA2_SW 0x00499580
// FUNCTION: CARMA2_CHEAT 0x0049d7b0
int C2_HOOK_FASTCALL PermitNetServiceReentrancy(void) {

    if (gIn_net_service) {
        gIn_net_service = 0;
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049d420
// FUNCTION: CARMA2_SW 0x004995a0
// FUNCTION: CARMA2_CHEAT 0x0049d7d0
void C2_HOOK_FASTCALL HaltNetServiceReentrancy(void) {

    gIn_net_service = 1;
}

// FUNCTION: CARMA2_HW 0x0049d430
// FUNCTION: CARMA2_SW 0x004995b0
// FUNCTION: CARMA2_CHEAT 0x0049d7e0
void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage) {
    tNet_message_chunk* the_contents;

    if (gNet_mode != eNet_mode_none) {
        the_contents = NetStartBroadcastContents(eNet_message_chunk_type_headup, 0);
        if (the_contents != NULL) {
            strcpy(the_contents->headup.text, pMessage);
            NetBroadcastContents(the_contents);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049d780
// FUNCTION: CARMA2_SW 0x00499900
void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage) {
    tNet_message_chunk* the_contents;

    if (gNet_mode != eNet_mode_none) {
        if (gProgram_state.racing) {
            NewTextHeadupSlot(4, 0, 3000, -4, pMessage);
        }
        the_contents = NetStartBroadcastContents(eNet_message_chunk_type_headup, 0);
        if (the_contents != NULL) {
            strcpy(the_contents->headup.text, pMessage);
            NetBroadcastContents(the_contents);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049daf0
// FUNCTION: CARMA2_SW 0x00499c70
// FUNCTION: CARMA2_CHEAT 0x0049e030
void C2_HOOK_FASTCALL NetSendHeadupToPlayer(char* pMessage, tPlayer_ID pPlayer) {
    tNet_message* message;

    if (gNet_mode != eNet_mode_none) {
        if (pPlayer == gLocal_net_ID) {
            if (gProgram_state.racing) {
                NewTextHeadupSlot(4, 0, 3000, -4, pMessage);
            }
        } else {
            message = NetBuildGuaranteedMessage(eNet_message_chunk_type_headup, 0);
            strcpy(message->contents.headup.text, pMessage);
            NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, pPlayer, NULL);
        }
    }
}

inline void C2_HOOK_FASTCALL InitialisePlayerStati(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gNet_players); i++) {
        gNet_players[i].last_heard_from_him = PDGetTotalTime();
        gNet_players[i].player_status = ePlayer_status_loading;
    }
}

void C2_HOOK_FASTCALL LeaveTempGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        NetLeaveGameLowLevel(gCurrent_join_poll_game);
    }
    gTime_for_next_one = 1;
    gCurrent_join_poll_game = NULL;
}

void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        BrMemFree(gCurrent_join_poll_game);
        gCurrent_join_poll_game = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0049ec70
// FUNCTION: CARMA2_SW 0x0049adf0
// FUNCTION: CARMA2_CHEAT 0x0049f1c0
void C2_HOOK_FASTCALL DoNextJoinPoll(void) {

    if (gTime_for_next_one) {
        gCurrent_join_poll_game = NetAllocatePIDGameDetails();
        if (gCurrent_join_poll_game == NULL) {
            return;
        }
        if (PDNetGetNextJoinGame(gCurrent_join_poll_game, gJoin_poll_index)) {
            if (!NetJoinGameLowLevel(gCurrent_join_poll_game, "!TEMP!")) {
                tNet_message* message;
                gTime_for_next_one = 0;
                message = NetBuildMessage(0, 0);
                NetSendMessageToAddress(gCurrent_join_poll_game, message, &gCurrent_join_poll_game->pd_net_info);
                gBastard_has_answered = 0;
                gAsk_time = PDGetTotalTime();
            } else {
                DisposeCurrentJoinPollGame();
            }
            gJoin_poll_index += 1;
            return;
        } else {
            gJoin_poll_index = 0;
            DisposeCurrentJoinPollGame();
            return;
        }
    } else {
        if (gBastard_has_answered) {
            gAdd_proc(gCurrent_join_poll_game);
            LeaveTempGame();
        } else {
            if (PDGetTotalTime() - gAsk_time > 10000) {
                LeaveTempGame();
                DisposeCurrentJoinPollGame();
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049dbd0
// FUNCTION: CARMA2_SW 0x00499d50
// FUNCTION: CARMA2_CHEAT 0x0049e110
void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc* pAdd_proc) {

    gAdd_proc = pAdd_proc;
    gJoin_list_mode = 1;
    gBastard_has_answered = 0;
    gTime_for_next_one = 1;
    gJoin_poll_index = 0;
    gCurrent_join_poll_game = NULL;
    PDNetStartProducingJoinList();
}

// FUNCTION: CARMA2_HW 0x0049dc00
// FUNCTION: CARMA2_SW 0x00499d80
// FUNCTION: CARMA2_CHEAT 0x0049e140
void C2_HOOK_FASTCALL NetEndJoinList(void) {

    if (gJoin_list_mode) {
        gJoin_list_mode = 0;
        DisposeCurrentJoinPollGame();
        LeaveTempGame();
        PDNetEndJoinList();
    }
}

// FUNCTION: CARMA2_HW 0x0049dc70
// FUNCTION: CARMA2_SW 0x00499df0
void C2_HOOK_FASTCALL NetDisposePIDGameInfo(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

// FUNCTION: CARMA2_HW 0x0049dc80
// FUNCTION: CARMA2_SW 0x00499e00
// FUNCTION: CARMA2_CHEAT 0x0049e1c0
void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);

    return BrMemAllocate(sizeof(tNet_game_details), kMem_net_pid_details);
}

void C2_HOOK_FASTCALL NetLeaveGameLowLevel(tNet_game_details* pDetails) {

    if (gNet_mode == eNet_mode_host) {
        PDNetHostFinishGame(gCurrent_net_game);
    } else {
        PDNetLeaveGame(pDetails);
    }
}

// FUNCTION: CARMA2_HW 0x0049dc90
// FUNCTION: CARMA2_SW 0x00499e10
// FUNCTION: CARMA2_CHEAT 0x0049e1d0
void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
    tNet_message* the_message;
    char s[256];
    const char* s2;
    int i;
    int must_revert_reentrancy;

    dr_dprintf("NetLeaveGame() START");
    if (gNet_mode != eNet_mode_none) {
        gOnly_receive_guarantee_replies = 1;
        if (gNet_mode == eNet_mode_host) {
            dr_dprintf("NetLeaveGame() Point 1");
            gDont_allow_joiners = 1;
            NetSendMessageStacks();
            dr_dprintf("NetLeaveGame() Point 2");
            the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_hosticide, 0);
            dr_dprintf("NetLeaveGame() Point 3");
            must_revert_reentrancy = PermitNetServiceReentrancy();
            dr_dprintf("NetLeaveGame() Point 4");
            NetGuaranteedSendMessageToAllPlayers(pNet_game, the_message, NULL);
            dr_dprintf("NetLeaveGame() Point 5");
            if (must_revert_reentrancy) {
                HaltNetServiceReentrancy();
            }
            dr_dprintf("NetLeaveGame() Point 6");
        } else if (!gHost_died) {
            the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_leave, 0);
            NetGuaranteedSendMessageToHost(pNet_game, the_message, NULL);
            strcpy(s, gProgram_state.player_name);
            s2 = GetMiscString(eMiscString_has_left_the_game);
            strcat(s, " ");
            strcat(s, s2);
            NetSendHeadupToAllPlayers(s);
        }
        dr_dprintf("NetLeaveGame() Point 7");
        for (i = 0; i < gNumber_of_net_players; i++) {
            DisposeCarN(i);
        }
        dr_dprintf("NetLeaveGame() Point 8");
        ClearOutStorageSpace(&gOur_car_storage_space);
        dr_dprintf("NetLeaveGame() Point 9");
        ClearOutStorageSpace(&gNet_cars_storage_space);
        dr_dprintf("NetLeaveGame() Point 10");
        must_revert_reentrancy = PermitNetServiceReentrancy();
        NetSendMessageStacks();
        NetWaitForGuaranteeReplies();
        NetLeaveGameLowLevel(gCurrent_net_game);
        if (must_revert_reentrancy) {
            HaltNetServiceReentrancy();
        }
        gCurrent_net_game = NULL;
        gNet_mode = eNet_mode_none;
        gNumber_of_net_players = 0;
        gProgram_state.prog_status = eProg_idling;
        gOnly_receive_guarantee_replies = 0;
        dr_dprintf("NetLeaveGame() Point 11");
    }
}

inline void C2_HOOK_FASTCALL NetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address) {

    PDNetSetPlayerSystemInfo(pPlayer, pSender_address);
}

// FUNCTION: CARMA2_HW 0x0049e3a0
// FUNCTION: CARMA2_SW 0x0049a520
// FUNCTION: CARMA2_CHEAT 0x0049e8e0
void C2_HOOK_FASTCALL FillInThisPlayer(tNet_game_details* pGame, tNet_game_player_info* pPlayer, int pCar_index, int pHost) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, field_0x58, 0x58);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNet_game_player_info, field_0x58, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, field_0x74, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNet_game_player_info, field_0x74, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, field_0x84, 0x84);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNet_game_player_info, field_0x84, 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, field_0xd0, 0xd0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNet_game_player_info, field_0xd0, 4);

    pPlayer->host = pHost;
    pPlayer->ID = NetExtractPlayerID(pGame);
    strcpy(pPlayer->player_name, gProgram_state.player_name);
    pPlayer->this_players_time_stamp = PDGetTotalTime();
    pPlayer->car_index = pCar_index;
    pPlayer->reposition_time = 0;
    pPlayer->last_waste_message = 0;
    pPlayer->wasteage_attributed = 0;
    pPlayer->field_0x58 = 0;
    pPlayer->field_0x74 = 1;
    pPlayer->field_0x7c = 0;
    pPlayer->field_0x88 = 0;
    pPlayer->score = 0;
    pPlayer->field_0x80 = 0;
    pPlayer->field_0x84 = 0;
    pPlayer->field_0xcc = NULL;
    pPlayer->field_0xd0 = 0;
    if (pGame->options.car_choice == eNet_car_all || pGame->options.car_choice == eNet_car_novelty) {
        pPlayer->next_car_index = -1;
    }
    InitialisePlayerScore(pPlayer);
}

// LoadCarN

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049e100
// STUB: CARMA2_SW 0x0049a280
// STUB: CARMA2_CHEAT 0x0049e640
void C2_HOOK_FASTCALL DisposeCarN(int pIndex) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// PlayerHasLeft

// RemoveReferencesToCar

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049e450
// STUB: CARMA2_SW 0x0049a5d0
// STUB: CARMA2_CHEAT 0x0049e990
void C2_HOOK_FASTCALL NetPlayersChanged(int pNew_count, tNet_game_player_info* pNew_players) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0049e210
// FUNCTION: CARMA2_SW 0x0049a390
// FUNCTION: CARMA2_CHEAT 0x0049e750
tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pGame_type, tNet_game_options* pOptions, int pStart_rank, char* pHost_name, int pCar_index) {
    tNet_game_details* game;
    void* host_address;
    tNet_game_player_info me;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, host_name, 0xe);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, host_ID, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, type, 0x74);

    game = NetAllocatePIDGameDetails();
    if (pHost_name[0] == '\0') {
        sprintf(pHost_name, "%s", "HOST");
    }
    DisableNetService();
    if (!PDNetHostGame(game, pHost_name, &host_address)) {
        NetDisposeGameDetails(game);
        return NULL;
    }
    gCurrent_net_game = game;
    gSynch_race_start = 0;
    strcpy(game->host_name, pHost_name);
    game->host_ID = NetExtractPlayerID(game);
    game->num_players = 1;
    memcpy(&game->options, pOptions, sizeof(tNet_game_options));
    game->status.stage = eNet_game_stage0;
    game->type = pGame_type;
    game->start_race = pStart_rank;
    game->no_races_yet = 1;
    gReceiving_new_players = 0;
    gHost_died = 0;
    gNumber_of_net_players = 0;
    gThis_net_player_index = 0;
    gLocal_net_ID = game->host_ID;
    FillInThisPlayer(game, &me, pCar_index, 1);
    NetSetPlayerSystemInfo(&me, host_address);
    NetPlayersChanged(1, &me);
    InitialisePlayerStati();
    gNet_mode = eNet_mode_host;
    InitNetworkShapesStuff(gNet_players[0].car->car_crush_spec);
    gDont_allow_joiners = 0;
    return game;
}

int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName) {

    return PDNetJoinGame(pGame_details, pName);
}

// FUNCTION: CARMA2_HW 0x0049e970
// FUNCTION: CARMA2_SW 0x0049aaf0
// FUNCTION: CARMA2_CHEAT 0x0049eeb0
int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details* pDetails, char* pPlayer_name, int pCar_index) {
    int result;
    tNet_message* the_message;
    tU32 start_time;

    result = NetJoinGameLowLevel(pDetails, pPlayer_name);
    if (result == 0) {
        DisableNetService();
        gReceiving_new_players = 0;
        gNet_mode = eNet_mode_client;
        gCurrent_net_game = pDetails;
        gLocal_net_ID = NetExtractPlayerID(pDetails);
        gNumber_of_net_players = 0;
        gLast_player_list_received = 0;
        gJoin_request_denied = 0;
        gCar_was_taken = 0;
        gHost_died = 0;
        the_message = NetBuildGuaranteedMessage(2, 0);
        FillInThisPlayer(pDetails, &the_message->contents.join.player_info, pCar_index, 0);
        ReenableNetService();
        NetGuaranteedSendMessageToAddress(pDetails, the_message, pDetails, NULL);
        start_time = PDGetTotalTime();
        do {
            NetService(0);
        } while (gNumber_of_net_players == 0
            && PDGetTotalTime() - start_time < 30000
            && !gJoin_request_denied
            && !gHost_died);
        DisableNetService();
        InitialisePlayerStati();
        if (gNumber_of_net_players == 0) {
            ReenableNetService();
            if (gJoin_request_denied && gCar_was_taken) {
                result = -4;
            } else {
                gNet_mode = eNet_mode_none;
                NetDisposeGameDetails(gCurrent_net_game);
                gCurrent_net_game = NULL;
                if (gJoin_request_denied) {
                    result = -2;
                } else {
                    result = -1;
                }
            }
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x0049ee20
// FUNCTION: CARMA2_SW 0x0049afa0
// FUNCTION: CARMA2_CHEAT 0x0049f3c0
void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length) {

    PDNetObtainSystemUserName(pName, pMax_length);
    pName[9] = '\0';
}

// FUNCTION: CARMA2_HW 0x0049ee30
// FUNCTION: CARMA2_SW 0x0049afb0
// FUNCTION: CARMA2_CHEAT 0x0049f3d0
tU32 C2_HOOK_FASTCALL NetExtractGameID(tNet_game_details* pDetails) {

    return PDNetExtractGameID(pDetails);
}

inline tPlayer_ID C2_HOOK_FASTCALL NetExtractPlayerID(tNet_game_details* pDetails) {

    return PDNetExtractPlayerID();
}

// FUNCTION: CARMA2_HW 0x0049ee50
// FUNCTION: CARMA2_SW 0x0049afd0
// FUNCTION: CARMA2_CHEAT 0x0049f3f0
int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress) {

    if (gNet_mode == eNet_mode_none && !gJoin_list_mode) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    return PDNetSendMessageToAddress(pDetails, pMessage, pAddress);
}

// FUNCTION: CARMA2_HW 0x0049eea0
// FUNCTION: CARMA2_SW 0x0049b020
// FUNCTION: CARMA2_CHEAT 0x0049f490
int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer_id) {
            return PDNetSendMessageToAddress(pNet_game, pMessage, &gNet_players[i].pd_net_info);
        }
    }
    return -3;
}

// FUNCTION: CARMA2_HW 0x0049ef20
// FUNCTION: CARMA2_SW 0x0049b0a0
// FUNCTION: CARMA2_CHEAT 0x0049f560
int C2_HOOK_FASTCALL NetSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer) {
    int i;
    int size;
    void* contents;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            size = pMessage->header.field_0x16 - sizeof(tNet_message_header);
            pMessage->contents.raw.header.size = size;
            contents = NetGetToPlayerContentsSize(&gNet_players[i], size);
            if (contents != NULL) {
                memcpy(contents, &pMessage->contents, size);
                NetDisposeMessage(pDetails, pMessage);
                return 0;
            }
        }
    }
    NetDisposeMessage(pDetails, pMessage);
    return -3;
}

// FUNCTION: CARMA2_HW 0x0049f0d0
// FUNCTION: CARMA2_SW 0x0049b250
// FUNCTION: CARMA2_CHEAT 0x0049f7d0
int C2_HOOK_FASTCALL NetSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (gNet_mode != eNet_mode_none) {
        pMessage->header.field_0x8 = gLocal_net_ID;
        pMessage->header.field_0x10 = PDGetTotalTime();
        return PDNetSendMessageToAddress(pDetails, pMessage, &pDetails->pd_net_info);
    } else {
        return -3;
    }
}

// FUNCTION: CARMA2_HW 0x0049f2c0
// FUNCTION: CARMA2_SW 0x0049b440
// FUNCTION: CARMA2_CHEAT 0x0049fad0
int C2_HOOK_FASTCALL NetSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage) {
    int i;
    int size;
    void* contents;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    size = pMessage->header.field_0x16 - sizeof(tNet_message_header);
    pMessage->contents.raw.header.size = size;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            contents = NetGetToPlayerContentsSize(&gNet_players[i], size);
            if (contents != NULL) {
                memcpy(contents, &pMessage->contents, size);
            }
        }
    }
    NetDisposeMessage(pDetails, pMessage);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049f440
// STUB: CARMA2_SW 0x0049b5c0
// STUB: CARMA2_CHEAT 0x0049fce0
tU32 C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, tU32 pSize_decider) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// NetGetMessageSize

// FUNCTION: CARMA2_HW 0x0049f650
// FUNCTION: CARMA2_SW 0x0049b7d0
// FUNCTION: CARMA2_CHEAT 0x0049fef0
tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2) {
    tNet_message* the_message;
    tU32 the_size;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    the_size = NetGetContentsSize(pArg1, pArg2) + offsetof(tNet_message, contents);
    the_message = NetAllocateMessage(the_size);
    if (the_message != NULL) {
        the_message->header.field_0x14 = 1;
        the_message->header.field_0x16 = (tU16)the_size;
        the_message->contents.raw.header.type = pArg1;
    }
    return the_message;
}

// FUNCTION: CARMA2_HW 0x0049f680
// FUNCTION: CARMA2_SW 0x0049b800
// FUNCTION: CARMA2_CHEAT 0x0049ff20
tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption) {
    tNet_message* the_message;
    tU32 contents_size;
    tU32 guarantee_size;
    tU32 the_size;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    contents_size = NetGetContentsSize(pNet_message_type, pOption);
    guarantee_size = NetGetContentsSize(eNet_message_chunk_type_5, 0);
    the_size = NetGetContentsSize(pNet_message_type, pOption) + offsetof(tNet_message, contents) + guarantee_size;
    the_message = NetAllocateMessage(the_size);
    if (the_message != NULL) {
        the_message->contents.raw.header.size = (tU8)contents_size;
        the_message->header.field_0x14 = 0xff;
        the_message->header.field_0x16 = (tU16)the_size;
        the_message->contents.raw.header.type = pNet_message_type;
        ((tNet_message_guaranteed*)((tU8*)&the_message->contents + the_message->contents.raw.header.size))->header.type = 5;
        ((tNet_message_guaranteed*)((tU8*)&the_message->contents + the_message->contents.raw.header.size))->header.size = (tU8)guarantee_size;
    }
    return the_message;
}

// FUNCTION: CARMA2_HW 0x004a5140
// FUNCTION: CARMA2_SW 0x004a12c0
// FUNCTION: CARMA2_CHEAT 0x004a5fb0
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pPlayer, int pContents_size) {
    tNet_message* message;
    tNet_message_chunk* contents;

    if (pPlayer == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = pPlayer->field_0xcc;
    if (message != NULL && (tS32)(message->header.field_0x16 + pContents_size) > (tS32)(sizeof(tMax_message) - sizeof(void*))) {
        NetReallySendMessageToPlayer(gCurrent_net_game, message, pPlayer->ID);
        pPlayer->field_0xcc = NULL;
        message = NULL;
    }
    if (message == NULL) {
        message = pPlayer->field_0xcc = NetAllocateMessage(sizeof(tMax_message) - sizeof(void*));
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    contents = (tNet_message_chunk*)((tU8*)message + message->header.field_0x16);
    message->header.field_0x16 += (tU16)pContents_size;
    message->header.field_0x14++;
    return contents;
}

// FUNCTION: CARMA2_HW 0x0049f6f0
// FUNCTION: CARMA2_SW 0x0049b870
// FUNCTION: CARMA2_CHEAT 0x0049ff90
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tNet_message_type pType, tU32 pSize_decider, tPlayer_ID pPlayer) {
    tNet_message_chunk* contents;
    tU32 content_size;
    int i;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);
    C2_HOOK_BUG_ON(sizeof(tMax_message) - sizeof(void*) != 0x200);

    content_size = NetGetContentsSize(pType, pSize_decider);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            contents = NetGetToPlayerContentsSize(&gNet_players[i], content_size);
            contents->raw.header.type = pType;
            contents->raw.header.size = (tU8)content_size;
            return contents;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049f860
// FUNCTION: CARMA2_SW 0x0049b9e0
// FUNCTION: CARMA2_CHEAT 0x004a0150
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToHostContents(tNet_message_type pType, tU32 pSize_decider) {

    return NetGetToPlayerContents(pType, pSize_decider, gNet_players[0].ID);
}

// FUNCTION: CARMA2_HW 0x0049f9c0
// FUNCTION: CARMA2_SW 0x0049bb40
// FUNCTION: CARMA2_CHEAT 0x004a0310
tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tU32 pSize_decider) {
    int i;

    if (INT_00659d34 >= 0) {
        PDFatalError("Broadcast stacks messed up");
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            INT_00659d34 = i;
            return NetGetToPlayerContents(pType, pSize_decider, gNet_players[i].ID);
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049fb70
// FUNCTION: CARMA2_SW 0x0049bcf0
// FUNCTION: CARMA2_CHEAT 0x004a0520
void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage) {
    tNet_message_chunk* contents;
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index && i != INT_00659d34) {
            contents = NetGetToPlayerContentsSize(&gNet_players[i], pMessage->raw.header.size);
            memcpy(contents, pMessage, pMessage->raw.header.size);
        }
    }
    INT_00659d34 = -1;
}

// FUNCTION: CARMA2_HW 0x0049fcf0
// FUNCTION: CARMA2_SW 0x0049be70
// FUNCTION: CARMA2_CHEAT 0x004a0700
void C2_HOOK_FASTCALL NetSendMessageStacks(void) {
    int i;

    gLast_flush_message = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].field_0xcc != NULL) {
            NetReallySendMessageToPlayer(gCurrent_net_game, gNet_players[i].field_0xcc, gNet_players[i].ID);
            gNet_players[i].field_0xcc = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049fdb0
// FUNCTION: CARMA2_SW 0x0049bf30
// FUNCTION: CARMA2_CHEAT 0x004a0820
tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize) {
    void* pointer;
    tNet_message_memory* new_message;
    tNet_message_memory* last_message;
    tNet_message* message;
    tU8* test;
    int i;

    C2_HOOK_BUG_ON(sizeof(tMin_message) - sizeof(void*) != 0x58);
    C2_HOOK_BUG_ON(sizeof(tMid_message) - sizeof(void*) != 0x80);
    C2_HOOK_BUG_ON(sizeof(tMax_message) - sizeof(void*) != 0x200);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_memory, message, 0x4);

    pointer = NULL;
    if (pSize <= sizeof(tMin_message) - sizeof(void*)) {
        for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
            test = gMin_messages[rr_min].buffer;
            test += gMessage_header_size;
            if (((tNet_message*)test)->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMin_messages[rr_min];
                break;
            }
            rr_min++;
            if (rr_min >= MIN_MESSAGES_CAPACITY) {
                rr_min = 0;
            }
        }
    }
    if (pointer == NULL && pSize <= (int)(sizeof(tMid_message) - sizeof(void*))) {
        for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
            test = gMid_messages[rr_mid].buffer;
            test += gMessage_header_size;
            if (((tNet_message*)test)->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMid_messages[rr_mid];
                break;
            }
            rr_mid++;
            if (rr_mid >= MID_MESSAGES_CAPACITY) {
                rr_mid = 0;
            }
        }
    }
    if (pointer == NULL && pSize <= (int)(sizeof(tMax_message) - sizeof(void*))) {
        for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
            test = gMax_messages[rr_max].buffer;
            test += gMessage_header_size;
            if (((tNet_message*)test)->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMax_messages[rr_max];
                break;
            }
            rr_max++;
            if (rr_max >= MAX_MESSAGES_CAPACITY) {
                rr_max = 0;
            }
        }
    }
    if (pointer == NULL) {
        pointer = new_message = BrMemAllocate(pSize + offsetof(tNet_message_memory, message) + gMessage_header_size, kMem_dynamic_message);
        if (new_message != NULL) {
            new_message->next = NULL;
            if (gDynamic_messages != NULL) {
                for (last_message = gDynamic_messages; last_message->next != NULL; last_message = last_message->next) {
                }
                last_message->next = new_message;
            } else {
                gDynamic_messages = new_message;
            }
            pointer = &new_message->message;
        }
    }
    if (pointer != NULL) {
        message = (tNet_message*)((tU8*)pointer + gMessage_header_size);
        message->header.field_0xc = 1;
        message->header.field_0x4 = 0x763a5059;
        return message;
    } else {
        return NULL;
    }
}

void C2_HOOK_FASTCALL NetFreeExcessMemory(void) {

    while (gDynamic_messages != NULL && *(&gDynamic_messages->message.contents.raw.header.type + gMessage_header_size) == eNetMsg_none) {
        tNet_message_memory* next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

void C2_HOOK_FASTCALL DisposeExcessMemory(void) {
    tNet_message_memory* next;

    while (gDynamic_messages != NULL) {
        next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

// FUNCTION: CARMA2_HW 0x0049ff50
// FUNCTION: CARMA2_SW 0x0049c0d0
// FUNCTION: CARMA2_CHEAT 0x004a0a40
int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (pMessage->header.field_0x14 == 0xff) {
        return -1;
    }
    pMessage->contents.raw.header.type = eNetMsg_none;
    return 0;
}

void C2_HOOK_FASTCALL NetReallyDisposeMessage(tNet_message* pMessage) {

    pMessage->contents.raw.header.type = eNetMsg_none;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0049ff70
// FUNCTION: CARMA2_SW 0x0049c0f0
// FUNCTION: CARMA2_CHEAT 0x004a0aa0
tNet_message* C2_HOOK_FASTCALL NetGetNextMessage(tNet_game_details* pDetails, void** pSender_address) {
    tNet_message* message;

    Timers_Push(TIMER_LLR);
    message = PDNetGetNextMessage(pDetails, pSender_address);
    Timers_Pop(TIMER_LLR);
    return message;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// ReceivedSendMeDetails

// ReceivedDetails

inline void C2_HOOK_FASTCALL SendOutPlayerList(void) {
    tNet_message* message;
    int i;

    gCurrent_net_game->num_players = gNumber_of_net_players;
    for (i = 0; i < gNumber_of_net_players; i++) {
        message = NetBuildGuaranteedMessage(3, 1);
        message->contents.player_list.number_of_players = gNumber_of_net_players;
        message->contents.player_list.this_index = i;
        message->contents.player_list.batch_number = gPlayer_list_batch_number;
        memcpy(&message->contents.player_list.player, &gNet_players[i], sizeof(gNet_players[i]));
        message->contents.player_list.player.field_0xcc = NULL;
        NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, message, NULL);
    }
    gPlayer_list_batch_number++;
    if (gInitialised_grid) {
        SetUpNetCarPositions();
        if (gINT_0074b780) {
            gSynch_race_start = 1;
            for (i = 1; i < gNumber_of_net_players; i++) {
                gNet_players[i].awaiting_confirmation = 1;
            }
        }
    }
}

// ReceivedJoin

// FUNCTION: CARMA2_HW 0x004a4660
// FUNCTION: CARMA2_SW 0x004a07e0
// FUNCTION: CARMA2_CHEAT 0x004a5350
void C2_HOOK_FASTCALL KickPlayerOut(tPlayer_ID pID) {
    int i;
    int j;
    int new_player_count;
    tNet_game_player_info* new_players;

    new_player_count = gNumber_of_net_players;
    new_players = BrMemAllocate(new_player_count * sizeof(tNet_game_player_info), kMem_player_list_leave);
    memcpy(new_players, gNet_players, new_player_count * sizeof(tNet_game_player_info));
    for (i = 0; i < new_player_count; i++) {
        if (pID == new_players[i].ID) {
            if (new_players[i].car_index >= 0) {
                gCar_details[new_players[i].car_index].ownership = eCar_owner_none;
            }
            if (new_players[i].next_car_index >= 0
                    && (gCurrent_net_game->options.car_choice == eNet_car_all || gCurrent_net_game->options.car_choice == eNet_car_novelty)) {
                gCar_details[new_players[i].next_car_index].ownership = eCar_owner_none;
            }
            for (j = i; j < new_player_count - 1; j++) {
                memcpy(&new_players[j], &new_players[j + 1], sizeof(tNet_game_player_info));
            }
            new_player_count--;
            break;
        }
    }
    NetPlayersChanged(new_player_count, new_players);
    BrMemFree(new_players);
    SendOutPlayerList();
}

// ReceivedLeave

// FUNCTION: CARMA2_HW 0x004a0080
// FUNCTION: CARMA2_SW 0x0049c200
// FUNCTION: CARMA2_CHEAT 0x004a0bb0
void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there) {
    tU32 start_time;
    const char* s;
    int restore_screen;
    int pixel_buffer_size;
    char* pixels_copy;
    char* palette_copy;

    if (!(pLeave_it_up_there || (gProgram_state.racing && !gInterface_within_race_mode))) {
        pixel_buffer_size = gBack_screen->height * gBack_screen->row_bytes;
        pixels_copy = BrMemAllocate(pixel_buffer_size, kMem_quit_vfy_pixels);
        palette_copy = BrMemAllocate(256 * sizeof(br_colour), kMem_quit_vfy_pal);
        memcpy(pixels_copy, gBack_screen->pixels, pixel_buffer_size);
        memcpy(palette_copy, gCurrent_palette_pixels, 256 * sizeof(br_colour));
        restore_screen = 1;
    } else {
        restore_screen = 0;
    }
    FadePaletteDown();
    LoadFont(4);
    ClearEntireScreen();
    s = GetMiscString(pStr_index);
    OoerrIveGotTextInMeBoxMissus(4, s, gBack_screen,
        gGraf_specs[gGraf_spec_index].total_width / 10,
        gGraf_specs[gGraf_spec_index].total_height / 2 - gFonts[4].height,
        gGraf_specs[gGraf_spec_index].total_width * 9 / 10,
        gGraf_specs[gGraf_spec_index].total_height - 64, 1);
    PDScreenBufferSwap(0);
    EnsureRenderPalette();
    EnsurePaletteUp();
    if (!pLeave_it_up_there) {
        start_time = PDGetTotalTime();
        while (PDGetTotalTime() - start_time < 3000) {
            /* FIXME: add sleep */
        }
        FadePaletteDown();
        if (restore_screen) {
            memcpy(gBack_screen->pixels, pixels_copy, pixel_buffer_size);
            memcpy(gCurrent_palette_pixels, palette_copy, 256 * sizeof(br_colour));
            if (pixels_copy != NULL) {
                BrMemFree(pixels_copy);
            }
            if (palette_copy != NULL) {
                BrMemFree(palette_copy);
            }
            PDScreenBufferSwap(0);
            FadePaletteUp();
        } else {
            ClearEntireScreen();
        }
    }
}

// HostHasBittenTheDust

// ReceivedHosticide

// FUNCTION: CARMA2_HW 0x004a48d0
// FUNCTION: CARMA2_SW 0x004a0a50
// FUNCTION: CARMA2_CHEAT 0x004a55c0
void C2_HOOK_FASTCALL ReceivedNewPlayerList(tNet_message_chunk* pContents, tNet_message* pM) {
    int i;

    if (gCurrent_net_game == NULL) {
        return;
    }
    if (pContents->player_list.number_of_players <= 0) {
        gJoin_request_denied = 1;
        if (pContents->player_list.number_of_players < 0) {
            gCar_was_taken = 1;
        }
        return;
    }
    if (gReceiving_batch_number > pContents->player_list.batch_number) {
        return;
    }
    if (!gReceiving_new_players) {
        gLast_player_list_received = pM->header.field_0x10;
        for (i = 0; i < (int)REC2_ASIZE(gNew_net_players); i++) {
            gNew_net_players[i].car_index = -1;
        }
        gReceiving_new_players = 1;
        gReceiving_batch_number = pContents->player_list.batch_number;
    }
    if (gReceiving_batch_number < pContents->player_list.batch_number) {
        gReceiving_new_players = 0;
        ReceivedNewPlayerList(pContents, pM);
        return;
    }
    memcpy(&gNew_net_players[pContents->player_list.this_index], &pContents->player_list.player, sizeof(tNet_game_player_info));
    for (i = 0; i < pContents->player_list.number_of_players; i++) {
        if (gNew_net_players[i].car_index < 0) {
            return;
        }
    }
    gReceiving_new_players = 0;
    NetPlayersChanged(pContents->player_list.number_of_players, gNew_net_players);
    gThis_net_player_index = -1;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == gLocal_net_ID) {
            gThis_net_player_index = i;
            break;
        }
    }
    if (gThis_net_player_index < 0) {
        FatalError(0x69);
    }
    gNet_players[0].last_heard_from_him = PDGetTotalTime();
    gCurrent_race.number_of_racers = gNumber_of_net_players;
    if (gStart_race_sent) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            gCurrent_race.opponent_list[gNet_players[i].opponent_list_index].net_player_index = i;
        }
    }
}

// ReceivedRaceOver

// ReceivedStatusReport

// ReceivedGroupStatus

// ReceivedRepositionCar

// ReceivedStartRace

// ReceivedGuaranteeReply

// ReceivedHeadup

// ReceivedHostQuery

// ReceivedHostReply

// ReceivedGuaranteedMessage

// PlayerIsInList

// ReceivedTimeSync

// ReceivedConfirm

// ReceivedDisableCar

// ReceivedEnableCar

// ReceivedActivateLift

// ReceivedScores

// ReceivedWasted

// ReceivedCarDetailsReq

// ReceivedCarDetails

// ReceivedGameScores

// ProcessGuaranteeStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a0260
// STUB: CARMA2_SW 0x0049c3e0
// STUB: CARMA2_CHEAT 0x004a0d90
void C2_HOOK_FASTCALL ReceivedMessage(tNet_message* pMessage, void* pSender_address, tU32 pReceive_time) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a4a40
// FUNCTION: CARMA2_SW 0x004a0bc0
// FUNCTION: CARMA2_CHEAT 0x004a5730
void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void) {
    tNet_message* message;
    void* sender_address;
    tU32 receive_time;
    int old_net_service;

    if (gNet_mode != eNet_mode_none || gJoin_list_mode) {
        old_net_service = gIn_net_service;
        gIn_net_service = 1;
        do {
            message = NetGetNextMessage(gCurrent_net_game, &sender_address);
            if (message != NULL) {
                if (message->header.field_0x14 != 0) {
                    receive_time = GetRaceTime();
                    if (message->header.field_0x4 == 0x763a5059) {
                        CheckCheckSum(message);
                        StatReceivePacket();
                        ReceivedMessage(message, sender_address, receive_time);
                    }
                    NetReallyDisposeMessage(message);
                }
            }
        } while (message != NULL);
        gIn_net_service = old_net_service;
    }
}

// FUNCTION: CARMA2_HW 0x004a4ac0
// FUNCTION: CARMA2_SW 0x004a0c40
// FUNCTION: CARMA2_CHEAT 0x004a5890
void C2_HOOK_FASTCALL BroadcastStatus(void) {
    tNet_message* message;
    int i;

    switch (gNet_mode) {
    case eNet_mode_client:
        message = NetBuildMessage(eNet_message_chunk_type_status_report, 0);
        message->contents.status_report.status = gNet_players[gThis_net_player_index].player_status;
        NetSendMessageToHost(gCurrent_net_game, message);
        break;
    case eNet_mode_host:
        message = NetBuildMessage(eNet_message_chunk_type_group_status, gNumber_of_net_players);
        message->contents.group_status.player_id_sum = 0;
        message->contents.group_status.number_of_players = gNumber_of_net_players;
        for (i = 0; i < gNumber_of_net_players; i++) {
            message->contents.group_status.player_id_sum += gNet_players[i].ID;
            message->contents.group_status.player_status[i] = gNet_players[i].player_status;
        }
        NetSendMessageToAllPlayers(gCurrent_net_game, message);
        break;
    default:
        break;
    }
    NetSendMessageStacks();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a4da0
// STUB: CARMA2_SW 0x004a0f20
// STUB: CARMA2_CHEAT 0x004a5c10
void C2_HOOK_FASTCALL CheckForDisappearees(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a5240
// FUNCTION: CARMA2_SW 0x004a13c0
// FUNCTION: CARMA2_CHEAT 0x004a6110
void C2_HOOK_FASTCALL CheckForPendingStartRace(void) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, awaiting_confirmation, 0x60);

    if (gSynch_race_start) {
        for (i = 1; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].awaiting_confirmation) {
                return;
            }
        }
        SignalToStartRace();
    }
}

// FUNCTION: CARMA2_HW 0x004a5280
// FUNCTION: CARMA2_SW 0x004a1400
// FUNCTION: CARMA2_CHEAT 0x004a6150
void C2_HOOK_FASTCALL NetService(int pIn_race) {
    tU32 time;

    if (gIn_net_service || gNet_service_disable) {
        return;
    }
    time = PDGetTotalTime();
    gIn_net_service = 1;
    if (gJoin_list_mode) {
        NetFreeExcessMemory();
        DoNextJoinPoll();
        NetReceiveAndProcessMessages();
    } else {
        if (gNet_mode != eNet_mode_none) {
            NetFreeExcessMemory();
            if (!pIn_race) {
                NetReceiveAndProcessMessages();
            }
            if (time - gLast_status_broadcast > 1000) {
                gLast_status_broadcast = PDGetTotalTime();
                BroadcastStatus();
            }
            CheckForDisappearees();
            if (gNet_mode == eNet_mode_host) {
                CheckForPendingStartRace();
                CheckForNeedyEnvironmentRecipients();
            }
        }
    }
    if (gJoin_list_mode || gNet_mode != eNet_mode_none) {
        ResendGuaranteedMessages();
        if (time > gLast_flush_message + 200u || !gProgram_state.racing) {
            NetSendMessageStacks();
        }
    }
    gIn_net_service = 0;
}

// FUNCTION: CARMA2_HW 0x004a5710
// FUNCTION: CARMA2_SW 0x004a1890
// FUNCTION: CARMA2_CHEAT 0x004a6610
void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason) {
    tNet_message* the_message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_race_over, reason, 0x4);

    gSynch_race_start = 0;
    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_race_over, 0);
    the_message->contents.race_over.reason = pReason;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
}

// FUNCTION: CARMA2_HW 0x004a57e0
// FUNCTION: CARMA2_SW 0x004a1960
// FUNCTION: CARMA2_CHEAT 0x004a66e0
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {

    if (gNet_mode != eNet_mode_none && gNet_players[gThis_net_player_index].player_status != pNew_status) {
        gNet_players[gThis_net_player_index].player_status = pNew_status;
        BroadcastStatus();
        if (gProgram_state.current_car.disabled && pNew_status >= ePlayer_status_racing && pNew_status != ePlayer_status_recovering) {
            EnableCar(&gProgram_state.current_car);
        } else if (!gProgram_state.current_car.disabled && pNew_status < ePlayer_status_racing) {
            DisableCar(&gProgram_state.current_car);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004a5b30
// FUNCTION: CARMA2_CHEAT 0x004a6aa0
tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void) {

    return gNet_players[gThis_net_player_index].player_status;
}

// FUNCTION: CARMA2_HW 0x004a5b50
// FUNCTION: CARMA2_SW 0x004a1cd0
// FUNCTION: CARMA2_CHEAT 0x004a6ac0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    int i;
    int err;

    err = 0;
    if (gNumber_of_net_players == 1) {
        pMessage->contents.raw.header.type = eNetMsg_none;
        return err;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            err |= NetGuaranteedSendMessageToPlayer(pDetails, pMessage, gNet_players[i].ID, pNotifyFail);
        }
    }
    return err;
}

// FUNCTION: CARMA2_HW 0x004a5bc0
// FUNCTION: CARMA2_SW 0x004a1d40
// FUNCTION: CARMA2_CHEAT 0x004a6b30
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guarantee message not built proper");
    }
    pMessage->header.field_0x14 = 1;
    ReceivedMessage(pMessage, &gNet_players[gThis_net_player_index], GetRaceTime());
    pMessage->header.field_0x14 = 0xff;
    return NetGuaranteedSendMessageToAllPlayers(pDetails, pMessage, pNotifyFail);
}

// FUNCTION: CARMA2_HW 0x004a5c80
// FUNCTION: CARMA2_SW 0x004a1e00
// FUNCTION: CARMA2_CHEAT 0x004a6bf0
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    NetGuaranteedSendMessageToPlayer(pDetails, pMessage, gNet_players[0].ID, pNotifyFail);
}

// FUNCTION: CARMA2_HW 0x004a5ca0
// FUNCTION: CARMA2_SW 0x004a1e20
void C2_HOOK_FASTCALL AddToGuaranteeList(tNet_message* pMessage, void* pAddress, tPlayer_ID pPlayer, void* pNotifyFail) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gGuarantee_list) != 200);
    C2_HOOK_BUG_ON(sizeof(tGuaranteed_message) != 0x34);
    C2_HOOK_BUG_ON(sizeof(SOCKADDR_IPX) != 14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_raw, data, 2);

    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guaranteed message not built correctly");
    }
    if (gNext_guarantee >= (int)REC2_ASIZE(gGuarantee_list)) {
        for (i = 0; i < 10; i++) {
            NetReallyDisposeMessage(gGuarantee_list[i].message);
        }
        while (gGuarantee_list[i - 1].message == gGuarantee_list[i].message && i < (int)REC2_ASIZE(gGuarantee_list)) {
            i++;
        }
        memmove(gGuarantee_list, &gGuarantee_list[i], (REC2_ASIZE(gGuarantee_list) - i) * sizeof(gGuarantee_list[0]));
        gNext_guarantee -= i;
    }
    if (gNext_guarantee < (int)REC2_ASIZE(gGuarantee_list)) {
        *(tU16*)&pMessage->contents.raw.data[pMessage->contents.raw.header.size] = gGuarantee_number;
        gGuarantee_list[gNext_guarantee].field_0x2c = gGuarantee_number;
        gGuarantee_number++;
        if (gGuarantee_number == 0) {
            gGuarantee_number = 1;
        }
        gGuarantee_list[gNext_guarantee].message = pMessage;
        gGuarantee_list[gNext_guarantee].send_time = PDGetTotalTime();
        gGuarantee_list[gNext_guarantee].next_resend_time = gGuarantee_list[gNext_guarantee].send_time + 100;
        gGuarantee_list[gNext_guarantee].resend_period = 100;
        if (pAddress != NULL) {
            memcpy(gGuarantee_list[gNext_guarantee].pd_address, pAddress, sizeof(SOCKADDR_IPX));
            gGuarantee_list[gNext_guarantee].field_0x30 = 1;
        } else {
            gGuarantee_list[gNext_guarantee].field_0x30 = 0;
        }
        gGuarantee_list[gNext_guarantee].local_id = pPlayer;
        gGuarantee_list[gNext_guarantee].NotifyFail = pNotifyFail;
        gGuarantee_list[gNext_guarantee].recieved = 0;
        gNext_guarantee++;
    }
}

// FUNCTION: CARMA2_HW 0x004a5e20
// FUNCTION: CARMA2_SW 0x004a1fa0
// FUNCTION: CARMA2_CHEAT 0x004a6df0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail) {
    int i;
    int j;
    int size;
    tNet_message_chunk* contents;
    tNet_message_chunk* dest;

    if (pMessage->header.field_0x14 != 0xff) {
        PDFatalError("Guaranteed message not built properly");
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            if (pPlayer == gLocal_net_ID) {
                pMessage->header.field_0x8 = gLocal_net_ID;
                pMessage->header.field_0x10 = PDGetTotalTime();
                pMessage->header.field_0x14 = 1;
                ReceivedMessage(pMessage, &gNet_players[i], GetRaceTime());
                NetDisposeMessage(pDetails, pMessage);
                return 0;
            } else {
                AddToGuaranteeList(pMessage, NULL, pPlayer, pNotifyFail);
                size = 0;
                contents = &pMessage->contents;
                for (j = 0; j < 2; j++) {
                    size += contents->raw.header.size;
                    contents = (tNet_message_chunk*)((tU8*)contents + contents->raw.header.size);
                }
                dest = NetGetToPlayerContentsSize(&gNet_players[i], size);
                gNet_players[i].field_0xcc->header.field_0x14++;
                memcpy(dest, &pMessage->contents, size);
                return 0;
            }
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x004a6020
// FUNCTION: CARMA2_SW 0x004a21a0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress, void* pNotifyFail) {

    if (gNet_mode != eNet_mode_none || gJoin_list_mode) {
        pMessage->header.field_0x8 = gLocal_net_ID;
        pMessage->header.field_0x10 = PDGetTotalTime();
        AddToGuaranteeList(pMessage, pAddress, 0, pNotifyFail);
        GetCheckSum(pMessage);
        return PDNetSendMessageToAddress(pDetails, pMessage, pAddress);
    } else {
        return -3;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a6080
// STUB: CARMA2_SW 0x004a2200
// STUB: CARMA2_CHEAT 0x004a7150
void C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a64c0
// FUNCTION: CARMA2_SW 0x004a2640
// FUNCTION: CARMA2_CHEAT 0x004a7690
void C2_HOOK_FASTCALL NetWaitForGuaranteeReplies(void) {
    tU32 start_time;

    start_time = PDGetTotalTime();
    while (gNext_guarantee != 0 && PDGetTotalTime() - start_time < 5000) {
        NetService(0);
    }
}

// FUNCTION: CARMA2_HW 0x004a6920
// FUNCTION: CARMA2_SW 0x004a2aa0
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromID(tPlayer_ID pPlayer) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a6960
// FUNCTION: CARMA2_SW 0x004a2ae0
// FUNCTION: CARMA2_CHEAT 0x004a7b50
tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer) {
    tNet_game_player_info* player;

    player = NetPlayerFromID(pPlayer);
    if (player != NULL) {
        return player->car;
    } else {
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004a69b0
// FUNCTION: CARMA2_SW 0x004a2b30
// FUNCTION: CARMA2_CHEAT 0x004a7ba0
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].car == pCar) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a69f0
// FUNCTION: CARMA2_SW 0x004a2b70
// FUNCTION: CARMA2_CHEAT 0x004a7be0
void C2_HOOK_FASTCALL GetCheckSum(tNet_message* pMessage) {
    (void)pMessage;
}

// FUNCTION: CARMA2_HW 0x004a6a00
// FUNCTION: CARMA2_SW 0x004a2b80
void C2_HOOK_FASTCALL CheckCheckSum(tNet_message* pMessage) {
    (void)pMessage;
}

void C2_HOOK_FASTCALL StatReceivePacket(void) {
}

void C2_HOOK_FASTCALL StatSendPacket(void) {
}

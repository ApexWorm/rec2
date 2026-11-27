/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "01-network.h"

#include "04-netgame.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "32-spark.h"
#include "41-utility.h"
#include "47-win95net.h"
#include "52-errors.h"
#include "53-controls.h"
#include "68-timer.h"
#include "platform.h"
#include "globvars.h"
#include "globvrpb.h"

#include "rec2_macros.h"

#include "c2_string.h"
#include <stddef.h>

#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <wsipx.h>

#ifdef REC2_V101A
#define NET_MESSAGE_MAGIC 0x763a505a
#else
#define NET_MESSAGE_MAGIC 0x763a5059
#endif

// GLOBAL: CARMA2_HW 0x0068d95c
// GLOBAL: CARMA2_SW 0x006879b4
// GLOBAL: CARMA2_CHEAT 0x006951fc
int gJoin_poll_index;

// GLOBAL: CARMA2_HW 0x0068d978
// GLOBAL: CARMA2_SW 0x006879d0
// GLOBAL: CARMA2_CHEAT 0x00695218
int gBastard_has_answered;

// GLOBAL: CARMA2_HW 0x0068d984
tU32 gAsk_time;

// GLOBAL: CARMA2_HW 0x0068d98c
// GLOBAL: CARMA2_SW 0x006879e4
// GLOBAL: CARMA2_CHEAT 0x0069522c
int gMessage_header_size;

// GLOBAL: CARMA2_HW 0x00690c80
// GLOBAL: CARMA2_CHEAT 0x00698520
tU32 gLast_status_broadcast;

// GLOBAL: CARMA2_HW 0x00690238
// GLOBAL: CARMA2_SW 0x0068a290
// GLOBAL: CARMA2_CHEAT 0x00697ad8
int gTime_for_next_one;

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

// GLOBAL: CARMA2_HW 0x0074a690
// GLOBAL: CARMA2_SW 0x00741150
// GLOBAL: CARMA2_CHEAT 0x00751d30
tU32 gUINT_0074a690;

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

// STUB: CARMA2_HW 0x0049d430
// STUB: CARMA2_SW 0x004995b0
// STUB: CARMA2_CHEAT 0x0049d7e0
void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage) {
    NOT_IMPLEMENTED();
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

// InitialisePlayerStati

void C2_HOOK_FASTCALL LeaveTempGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        NetLeaveGameLowLevel();
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

void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void) {

    if (gNet_mode == eNet_mode_host) {
        PDNetHostFinishGame(gCurrent_net_game);
    } else {
        PDNetLeaveGame();
    }
}

// STUB: CARMA2_HW 0x0049dc90
// STUB: CARMA2_SW 0x00499e10
// STUB: CARMA2_CHEAT 0x0049e1d0
void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
    NOT_IMPLEMENTED();
}

// NetSetPlayerSystemInfo

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

// DisposeCarN

// PlayerHasLeft

// RemoveReferencesToCar

// NetPlayersChanged

// NetHostGame

int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName) {

    return PDNetJoinGame(pGame_details, pName);
}

// NetJoinGame

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
        message->header.field_0x4 = NET_MESSAGE_MAGIC;
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

// SendOutPlayerList

// ReceivedJoin

// KickPlayerOut

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

// ReceivedNewPlayerList

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
                    if (message->header.field_0x4 == NET_MESSAGE_MAGIC) {
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

// STUB: CARMA2_HW 0x004a4ac0
void C2_HOOK_FASTCALL BroadcastStatus(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004a4da0
void C2_HOOK_FASTCALL CheckForDisappearees(void) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x004a5280
// STUB: CARMA2_SW 0x004a1400
// STUB: CARMA2_CHEAT 0x004a6150
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

// STUB: CARMA2_HW 0x004a57e0
// STUB: CARMA2_SW 0x004a1960
// STUB: CARMA2_CHEAT 0x004a66e0
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
    NOT_IMPLEMENTED();
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
// FUNCTION: CARMA2_CHEAT 0x004a6c10
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
// FUNCTION: CARMA2_CHEAT 0x004a70a0
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

// STUB: CARMA2_HW 0x004a6080
void C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
    NOT_IMPLEMENTED();
}

// NetWaitForGuaranteeReplies

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

#ifdef REC2_V101A
// FUNCTION: CARMA2_CHEAT 0x004a7be0
tU32 C2_HOOK_FASTCALL CalculateCheckSum(tNet_message* pMessage) {
    tU32 sum;
    int i;
    tU32* p;
    tU8* pb;

    sum = 0;
    p = (tU32*)pMessage;
    for (i = 0; i < pMessage->header.field_0x16 >> 2; i++) {
        sum += *p;
        p++;
    }
    pb = (tU8*)p;
    for (i *= 4; i < pMessage->header.field_0x16; i++) {
        sum += *pb;
        pb++;
    }
    return sum;
}
#endif

// FUNCTION: CARMA2_HW 0x004a69f0
// FUNCTION: CARMA2_SW 0x004a2b70
void C2_HOOK_FASTCALL GetCheckSum(tNet_message* pMessage) {
#ifdef REC2_V101A
    pMessage->header.checksum = 0;
    pMessage->header.checksum = CalculateCheckSum(pMessage);
#else
    (void)pMessage;
#endif
}

// FUNCTION: CARMA2_HW 0x004a6a00
// FUNCTION: CARMA2_SW 0x004a2b80
void C2_HOOK_FASTCALL CheckCheckSum(tNet_message* pMessage) {
#ifdef REC2_V101A
    tU32 checksum;

    checksum = pMessage->header.checksum;
    pMessage->header.checksum = 0;
    if (checksum != CalculateCheckSum(pMessage)) {
        FatalError(0x6f);
    }
#else
    (void)pMessage;
#endif
}

void C2_HOOK_FASTCALL StatReceivePacket(void) {
}

void C2_HOOK_FASTCALL StatSendPacket(void) {
}

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_01_NETWORK_H
#define GUARD_01_NETWORK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int INT_00659d34;

extern int gIt_or_fox;
extern int INT_00659d34;
extern int gRace_only_flags[52];
extern int rr_min;
extern int rr_mid;
extern int rr_max;
extern int gInitialised_grid;
extern int gReceived_game_scores;
extern tRecent_guarantee gRecent_guarantees[15];
extern int gNext_recent_guarantee;
#ifdef REC2_HW
extern br_actor* gNetscore_box_universe;
extern br_material* gNetscore_box_materials[4];
extern br_actor* gNetscore_box_camera;
extern br_model* gNetScore_box_model;
extern br_actor* gNetscore_box_actors[12];
#endif

extern int C2_HOOK_FASTCALL NetInitialise(void);

extern int C2_HOOK_FASTCALL NetShutdown(void);

extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);

extern void C2_HOOK_FASTCALL DisableNetService(void);

extern void C2_HOOK_FASTCALL ReenableNetService(void);

extern int C2_HOOK_FASTCALL PermitNetServiceReentrancy(void);

extern void C2_HOOK_FASTCALL HaltNetServiceReentrancy(void);

extern void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);

extern void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage);

extern void C2_HOOK_FASTCALL NetSendHeadupToPlayer(char* pMessage, tPlayer_ID pPlayer);

extern void C2_HOOK_FASTCALL InitialisePlayerStati(void);

extern void C2_HOOK_FASTCALL LeaveTempGame(void);

extern void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void);

extern void C2_HOOK_FASTCALL DoNextJoinPoll(void);

extern void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc* pAdd_proc);

extern void C2_HOOK_FASTCALL NetEndJoinList(void);

extern void C2_HOOK_FASTCALL NetDisposePIDGameInfo(tNet_game_details* pDetails);

extern void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails);

extern tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void);

extern void C2_HOOK_FASTCALL NetLeaveGameLowLevel(tNet_game_details* pDetails);

extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

extern void C2_HOOK_FASTCALL NetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address);

extern void C2_HOOK_FASTCALL NetDisposePlayer(tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL FillInThisPlayer(tNet_game_details* pGame, tNet_game_player_info* pPlayer, int pCar_index, int pHost);

// LoadCarN

extern void C2_HOOK_FASTCALL DisposeCarN(int pIndex);

// PlayerHasLeft

// RemoveReferencesToCar

extern void C2_HOOK_FASTCALL NetPlayersChanged(int pNew_count, tNet_game_player_info* pNew_players);

extern tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pGame_type, tNet_game_options* pOptions, int pStart_rank, char* pHost_name, int pCar_index);

extern int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName);

extern int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details* pDetails, char* pPlayer_name, int pCar_index);

extern void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

extern tU32 C2_HOOK_FASTCALL NetExtractGameID(tNet_game_details* pDetails);

extern tPlayer_ID C2_HOOK_FASTCALL NetExtractPlayerID(tNet_game_details* pDetails);

extern int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);

extern int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id);

extern int C2_HOOK_FASTCALL NetSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer);

extern int C2_HOOK_FASTCALL NetSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage);

extern int C2_HOOK_FASTCALL NetSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage);

extern tU32 C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, tU32 pSize_decider);

// NetGetMessageSize

extern tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2);

extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);

extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pPlayer, int pContents_size);

extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tNet_message_type pType, tU32 pSize_decider, tPlayer_ID pPlayer);

extern tNet_message_chunk* C2_HOOK_FASTCALL NetGetToHostContents(tNet_message_type pType, tU32 pSize_decider);

extern tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tU32 pSize_decider);

extern void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage);

extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);

extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);

extern void C2_HOOK_FASTCALL NetFreeExcessMemory(void);

extern void C2_HOOK_FASTCALL DisposeExcessMemory(void);

extern int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage);

extern void C2_HOOK_FASTCALL NetReallyDisposeMessage(tNet_message* pMessage);

extern tNet_message* C2_HOOK_FASTCALL NetGetNextMessage(tNet_game_details* pDetails, void** pSender_address);

extern void C2_HOOK_FASTCALL ReceivedSendMeDetails(tNet_message_chunk* pContents, void* pSender_address);

extern void C2_HOOK_FASTCALL ReceivedDetails(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL SendOutPlayerList(void);

extern void C2_HOOK_FASTCALL ReceivedJoin(tNet_message_chunk* pContents, void* pSender_address, tPlayer_ID pPlayer);

extern void C2_HOOK_FASTCALL KickPlayerOut(tPlayer_ID pID);

extern void C2_HOOK_FASTCALL ReceivedLeave(tNet_message_chunk* pContents, tNet_message* pMessage);

extern void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there);

extern void C2_HOOK_FASTCALL HostHasBittenTheDust(int pMessage_index);

extern void C2_HOOK_FASTCALL ReceivedHosticide(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedNewPlayerList(tNet_message_chunk* pContents, tNet_message* pM);

extern void C2_HOOK_FASTCALL ReceivedRaceOver(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedStatusReport(tNet_message_chunk* pContents, tNet_message* pMessage);

extern void C2_HOOK_FASTCALL ReceivedGroupStatus(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedRepositionCar(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedStartRace(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedGuaranteeReply(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedHeadup(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedHostQuery(tNet_message_chunk* pContents, tNet_message* pMessage);

extern void C2_HOOK_FASTCALL ReceivedHostReply(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedGuaranteedMessage(tNet_message* pMessage, tNet_message_chunk* pContents, void* pSender_address);

extern int C2_HOOK_FASTCALL PlayerIsInList(tPlayer_ID pID);

extern void C2_HOOK_FASTCALL ReceivedTimeSync(tNet_message_chunk* pContents, tNet_message* pMessage, tU32 pReceive_time);

extern void C2_HOOK_FASTCALL ReceivedConfirm(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedDisableCar(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedEnableCar(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedActivateLift(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedScores(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedWasted(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedCarDetailsReq(tNet_message_chunk* pContents, void* pSender_address);

extern void C2_HOOK_FASTCALL ReceivedCarDetails(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedGameScores(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ProcessGuaranteeStuff(tNet_message* pMessage);

extern void C2_HOOK_FASTCALL ReceivedMessage(tNet_message* pMessage, void* pSender_address, tU32 pReceive_time);

extern void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);

extern void C2_HOOK_FASTCALL BroadcastStatus(void);

extern void C2_HOOK_FASTCALL CheckForDisappearees(void);

extern void C2_HOOK_FASTCALL CheckForPendingStartRace(void);

extern void C2_HOOK_FASTCALL NetService(int pIn_race);

extern void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason);

extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

extern tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void);

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

extern void C2_HOOK_FASTCALL AddToGuaranteeList(tNet_message* pMessage, void* pAddress, tPlayer_ID pPlayer, void* pNotifyFail);

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail);

extern int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress, void* pNotifyFail);

extern void C2_HOOK_FASTCALL ResendGuaranteedMessages(void);

extern int C2_HOOK_FASTCALL SampleFailNotifier(tU32 pAge, tNet_message* pMessage);

extern void C2_HOOK_FASTCALL NetWaitForGuaranteeReplies(void);

extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromID(tPlayer_ID pPlayer);

extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);

extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL GetCheckSum(tNet_message* pMessage);

extern void C2_HOOK_FASTCALL CheckCheckSum(tNet_message* pMessage);

extern void C2_HOOK_FASTCALL StatReceivePacket(void);

extern void C2_HOOK_FASTCALL StatReceivePacket(void);

extern void C2_HOOK_FASTCALL StatSendPacket(void);

#endif // GUARD_01_NETWORK_H

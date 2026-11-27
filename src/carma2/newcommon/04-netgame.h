/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_04_NETGAME_H
#define GUARD_04_NETGAME_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gINT_0068d920;
extern tNet_game_player_info* gLast_lepper;
extern int gLast_it_change;

extern void C2_HOOK_FASTCALL SendShapeNumbers(void);

extern void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id);

extern void C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pTime);

extern void C2_HOOK_FASTCALL HostFillInCarContents(tNet_message_chunk* pContents, int pPlayer_index, tU32 pTime);

extern void C2_HOOK_FASTCALL ClientSendCarData(tU32 pNext_frame_time);

extern br_scalar C2_HOOK_FASTCALL DistanceBetweenCars(tCar_spec* pCar_1, tCar_spec* pCar_2);

extern int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id);

extern void C2_HOOK_FASTCALL AddShapeStuffToMechanicsMessage(tNet_message_chunk* pContents, tNet_game_player_info* pPlayer, tNet_game_player_info* pTo_player);

extern int C2_HOOK_FASTCALL TimeToSendData(void);

extern void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time);

extern void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents);

// CopyMechanics

// ReceivedShapeNumbers

extern void C2_HOOK_FASTCALL FillInSimpleCarPos(tNet_message_chunk* pContents, int pPlayer_index);

// ReceivedSimpleCarPos

extern void C2_HOOK_FASTCALL ReceivedMechanics(tNet_message_chunk* pContents);

extern void C2_HOOK_FASTCALL ReceivedCopInfo(void);

// SendAllNonCarPositions

// ReceivedNonCarPosition

// ReceivedNonCar

extern void C2_HOOK_FASTCALL SignalToStartRace(void);

// SignalToRepositionCar

extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);

// ReinitialiseCar

// RepositionPlayer

extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

#ifdef REC2_HW
extern void C2_HOOK_FASTCALL CreateBoxes(void);
#endif

// DrawScoreBoxes

extern void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores);

extern void C2_HOOK_FASTCALL DoNetScores(void);

extern void C2_HOOK_FASTCALL InitNetHeadups(void);

extern void C2_HOOK_FASTCALL DisposeNetHeadups(void);

// EverybodysLost

extern void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);

extern void C2_HOOK_FASTCALL CheckForVampireWinner(void);

extern void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer);

// FarEnoughAway

extern void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer);

// SelectRandomFox

extern void C2_HOOK_FASTCALL CalcPlayerScores(void);

extern void C2_HOOK_FASTCALL SendPlayerScores(void);

extern void C2_HOOK_FASTCALL DoNetGameManagement(void);

extern void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer);

// InitPlayers

extern void C2_HOOK_FASTCALL UseGeneralScore(int pScore);

// NetSendEnvironmentChanges

extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

extern void C2_HOOK_FASTCALL ReceivedGameplay(tNet_message_chunk* pContents, tNet_message* pMessage, tU32 pReceive_time);

extern void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

extern void C2_HOOK_FASTCALL DefaultNetName(void);

extern void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers);

// RemovePlayerFromShapeStatusLists

#endif // GUARD_04_NETGAME_H

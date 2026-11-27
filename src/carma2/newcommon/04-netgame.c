/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "04-netgame.h"

#include "00-car.h"
#include "01-network.h"
#include "03-racestrt.h"
#include "07-structur.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "52-errors.h"
#include "53-controls.h"
#include "54-runtime.h"
#include "58-crush2.h"
#include "60-phil.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "72-interface.h"
#include "c2_stdio.h"
#include "c2_string.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include <stdio.h>
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x0075b8f8
// GLOBAL: CARMA2_SW 0x00751e58
// GLOBAL: CARMA2_cHEAT 0x00762f98
br_pixelmap* gDigits_pix;

// GLOBAL: CARMA2_HW 0x00659c00
// GLOBAL: CARMA2_SW 0x00654598
// GLOBAL: CARMA2_CHEAT 0x00660520
int gGame_scores[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 15, 20 };

// GLOBAL: CARMA2_HW 0x0068d920
// GLOBAL: CARMA2_SW 0x00687978
// GLOBAL: CARMA2_CHEAT 0x006951c0
int gINT_0068d920;

// GLOBAL: CARMA2_HW 0x0068d924
// GLOBAL: CARMA2_SW 0x0068797c
// GLOBAL: CARMA2_CHEAT 0x006951c4
tNet_game_player_info* gLast_lepper;

// GLOBAL: CARMA2_HW 0x0068d928
// GLOBAL: CARMA2_SW 0x00687980
int gLast_it_change;

// GLOBAL: CARMA2_HW 0x0068d92c
// GLOBAL: CARMA2_SW 0x00687984
// GLOBAL: CARMA2_cHEAT 0x006951cc
tU32 gTime_for_punishment;

// SendShapeNumbers

// FUNCTION: CARMA2_HW 0x00497ef0
// FUNCTION: CARMA2_SW 0x00494b20
// FUNCTION: CARMA2_CHEAT 0x004982a0
void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            pShapes->items[i].field_0x4 = 0;
            return;
        }
    }
}

// SendNonCar

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

// FUNCTION: CARMA2_HW 0x00498370
// FUNCTION: CARMA2_SW 0x00494fa0
// FUNCTION: CARMA2_CHEAT 0x00498720
int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            return (tS8)pShapes->items[i].field_0x4;
        }
    }
    return 0;
}

// AddShapeStuffToMechanicsMessage

// FUNCTION: CARMA2_HW 0x004978d0
// FUNCTION: CARMA2_SW 0x00494500
// FUNCTION: CARMA2_CHEAT 0x00497c80
int C2_HOOK_FASTCALL TimeToSendData(void) {
    // GLOBAL: CARMA2_HW 0x00659c30
    // GLOBAL: CARMA2_SW 0x006545c8
    // GLOBAL: CARMA2_CHEAT 0x00660550
    static tU32 last_physics_send_time = (tU32)-1;

    if (gPHIL_last_physics_tick > last_physics_send_time && gPHIL_last_physics_tick < last_physics_send_time + 80) {
        return 0;
    }
    last_physics_send_time = gPHIL_last_physics_tick;
    return 1;
}

// SendCarData

// FUNCTION: CARMA2_HW 0x00498390
// FUNCTION: CARMA2_SW 0x00494fc0
// FUNCTION: CARMA2_CHEAT 0x00498740
void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents) {
    int i;

    if (gNet_players[gThis_net_player_index].ID != pContents->player) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == pContents->player) {
                gNet_players[i].car->time_to_recover = pContents->time;
            }
        }
    }
}

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

// FUNCTION: CARMA2_HW 0x00498bb0
// FUNCTION: CARMA2_SW 0x004957e0
// FUNCTION: CARMA2_CHEAT 0x00498f60
void C2_HOOK_FASTCALL ReceivedCopInfo(void) {
}

// SendAllNonCarPositions

// ReceivedNonCarPosition

// ReceivedNonCar

// FUNCTION: CARMA2_HW 0x00498e70
// FUNCTION: CARMA2_SW 0x00495aa0
// FUNCTION: CARMA2_CHEAT 0x00499220
void C2_HOOK_FASTCALL SignalToStartRace(void) {
    tNet_message* the_message;
    int inactive;
    int i;
    int j;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, car_count, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, racing, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, next_race, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_start_race, car_list, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNet_message_chunk_start_race, car_list, 0xf0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_start_race_car, next_car_index, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_start_race_car, matrix, 0x2);
    C2_HOOK_BUG_ON(sizeof(tNet_start_race_car) != 0x14);

    gCurrent_net_game->no_races_yet = 0;
    gSynch_race_start = 0;
    gINT_0074b780 = 1;
    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_start_race, 0);
    the_message->contents.start_race.racing = (tU8)gProgram_state.racing;
    the_message->contents.start_race.car_count = (tU8)gCurrent_race.number_of_racers;
    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        CompressMatrix34(&the_message->contents.start_race.car_list[i].matrix, &inactive,
            &gCurrent_race.opponent_list[i].car_spec->car_master_actor->t.t.mat);
        for (j = 0; j < gNumber_of_net_players; j++) {
            if (gCurrent_race.opponent_list[i].car_spec == gNet_players[j].car) {
                the_message->contents.start_race.car_list[i].index = (tU8)j;
                if (inactive) {
                    the_message->contents.start_race.car_list[i].index |= 0x80;
                }
                break;
            }
        }
        if (gCurrent_net_game->options.car_choice == eNet_car_all
                || gCurrent_net_game->options.car_choice == eNet_car_novelty) {
            if (gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index < 0) {
                gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index = PickARandomCar();
                gCar_details[gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index].ownership = eCar_owner_someone;
            }
            the_message->contents.start_race.car_list[i].next_car_index = (tU8)gNet_players[the_message->contents.start_race.car_list[i].index & 0x7f].next_car_index;
        }
    }
    if (gPending_race < 0) {
        gPending_race = PickNetRace(gProgram_state.current_race_index,
            gCurrent_net_game->options.race_sequence_type);
    }
    the_message->contents.start_race.next_race = (tU8)gPending_race;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
    if (gProgram_state.racing) {
        SendCurrentPowerups();
    }
}

inline void C2_HOOK_FASTCALL SignalToRepositionCar(int pIndex) {
    tNet_message* the_message;

    gSynch_race_start = 0;
    gINT_0074b780 = 1;
    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_reposition, 0);
    the_message->contents.reposition.racing = gProgram_state.racing;
    gNet_players[pIndex].last_waste_message = 0;
    gNet_players[pIndex].wasteage_attributed = 0;
    the_message->contents.reposition.index = (tU8)pIndex;
    CompressMatrix34(&the_message->contents.reposition.matrix, &the_message->contents.reposition.inactive,
        &gCurrent_race.opponent_list[gNet_players[pIndex].opponent_list_index].car_spec->car_master_actor->t.t.mat);
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
    if (gProgram_state.racing) {
        SendCurrentPowerups();
    }
}

// FUNCTION: CARMA2_HW 0x00499000
// FUNCTION: CARMA2_SW 0x00495c30
// FUNCTION: CARMA2_CHEAT 0x004993b0
void C2_HOOK_FASTCALL SetUpNetCarPositions(void) {
    int i;
    int j;
    int k;
    int grid_index;
    int racer_count;

    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 0x10);

    DisableNetService();
    if (!gInitialised_grid) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            gNet_players[i].grid_position_set = 0;
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
        }
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        gCurrent_race.opponent_list[i].index = -1;
        gCurrent_race.opponent_list[i].ranking = IRandomBetween(0, 99);
        gCurrent_race.opponent_list[i].car_spec = gNet_players[i].car;
        gCurrent_race.opponent_list[i].net_player_index = i;
        gNet_players[i].opponent_list_index = i;
    }
    racer_count = gNumber_of_net_players;
    if (!gInitialised_grid && gCurrent_net_game->options.grid_start) {
        qsort(gCurrent_race.opponent_list, gNumber_of_net_players, sizeof(tOpp_spec), SortGridFunction);
    }
    gCurrent_race.number_of_racers = 0;
    for (i = 0; i < gNumber_of_net_players; i++) {
        gNet_players[gCurrent_race.opponent_list[i].net_player_index].opponent_list_index = i;
    }
    for (i = 0; i < racer_count; i++) {
        if ((gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver == eDriver_oppo && !gInitialised_grid)
            || (gCurrent_race.opponent_list[i].car_spec != NULL
                && gCurrent_race.opponent_list[i].car_spec->driver >= eDriver_net_human
                && !gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set)) {
            grid_index = -1;
            for (j = 0; j < kMax_netplayers && grid_index < 0; j++) {
                grid_index = j;
                for (k = 0; k < racer_count; k++) {
                    if (k != i
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_position_set
                        && gNet_players[gCurrent_race.opponent_list[k].net_player_index].grid_index == j) {
                        grid_index = -1;
                        break;
                    }
                }
            }
            if (grid_index < 0) {
                FatalError(kFatalError_NetworkCodeSelfCheck);
            }
            SetInitialPosition(&gCurrent_race, i, grid_index);
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_index = grid_index;
            if (!gInitialised_grid) {
                gCurrent_race.number_of_racers = i + 1;
            } else {
                InitialiseCar2(gCurrent_race.opponent_list[i].car_spec, 0);
            }
            gNet_players[gCurrent_race.opponent_list[i].net_player_index].grid_position_set = 1;
        }
    }
    gCurrent_race.number_of_racers = racer_count;
    gInitialised_grid = 1;
    ReenableNetService();
}

// FUNCTION: CARMA2_HW 0x00499220
// FUNCTION: CARMA2_SW 0x00495e50
// FUNCTION: CARMA2_CHEAT 0x004995d0
void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar) {

    StopCarSmokingInstantly(pCar);
    LoseAllLocalPowerups(pCar);
    InitialiseCar(pCar);
    TotallyRepairACar(pCar);
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        gTime_for_punishment = PDGetTotalTime() + 2000;
        EmptyInventory();
    }
}

inline void C2_HOOK_FASTCALL RepositionPlayer(int pIndex) {
    tCar_spec* car;

    car = gNet_players[pIndex].car;
    gNet_players[pIndex].wasted = 0;
    gNet_players[pIndex].reposition_time = 0;
    if (gCurrent_net_game->type == eNet_game_type_2
            || gCurrent_net_game->type == eNet_game_type_checkpoint
            || gCurrent_net_game->type == eNet_game_type_4) {
        BrMatrix34Copy(&car->car_master_actor->t.t.mat, &gNet_players[pIndex].initial_position);
    } else {
        SetInitialPosition(&gCurrent_race, gNet_players[pIndex].opponent_list_index, gNet_players[pIndex].grid_index);
    }
    ReinitialiseCar(car);
    SetFlipUpCar(car);
    car->collision_info->field_0x49c = GetRaceTime() + 100;
    SignalToRepositionCar(pIndex);
    if (car != NULL && car->driver == eDriver_local_human) {
        CancelPendingCunningStunt();
        gProgram_state.credits = gCurrent_net_game->options.starting_credits;
    }
}

// FUNCTION: CARMA2_HW 0x00499260
// FUNCTION: CARMA2_SW 0x00495e90
// FUNCTION: CARMA2_CHEAT 0x00499610
void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (!pCar->disabled) {
            pCar->disabled = 1;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] < 500.0f) {
            BrVector3Accumulate(&pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004992e0
// FUNCTION: CARMA2_SW 0x00495f10
// FUNCTION: CARMA2_CHEAT 0x00499690
void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar) {

    if (pCar->driver_name[0] != '\0') {
        if (pCar->disabled) {
            pCar->disabled = 0;
            ForceRebuildActiveCarList();
        }
        if (pCar->car_master_actor->t.t.mat.m[3][0] > 500.0f) {
            BrVector3Sub(&pCar->car_master_actor->t.t.translate.t, &pCar->car_master_actor->t.t.translate.t, &gInitial_position);
            BrVector3Copy((br_vector3*)pCar->old_frame_mat.m[3], &pCar->car_master_actor->t.t.translate.t);
        }
    }
}

// DoNetworkHeadups

// FUNCTION: CARMA2_HW 0x0049a330
// FUNCTION: CARMA2_SW 0x004967b0
// FUNCTION: CARMA2_CHEAT 0x0049a6e0
int C2_HOOK_CDECL SortNetHeadAscending(const void* pFirst_one, const void* pSecond_one) {
    const tNet_head_sort_entry* first;
    const tNet_head_sort_entry* second;

    first = pFirst_one;
    second = pSecond_one;
    if (first->field_0x8 != 0) {
        if (second->field_0x8 == 0) {
            return 0x7fffffff;
        }
        return first->field_0x8 - second->field_0x8;
    }
    if (second->field_0x8 != 0) {
        return -0x7fffffff;
    }
    if (first->score != second->score) {
        return first->score - second->score;
    }
    return gNet_players[first->player_index].last_score_index - gNet_players[second->player_index].last_score_index;
}

// FUNCTION: CARMA2_HW 0x0049a3a0
// FUNCTION: CARMA2_SW 0x00496820
// FUNCTION: CARMA2_CHEAT 0x0049a750
int C2_HOOK_CDECL SortNetHeadDescending(const void* pFirst_one, const void* pSecond_one) {
    const tNet_head_sort_entry* first;
    const tNet_head_sort_entry* second;

    first = pFirst_one;
    second = pSecond_one;
    if (first->field_0x8 != 0) {
        if (second->field_0x8 == 0) {
            return 0x7fffffff;
        }
        return first->field_0x8 - second->field_0x8;
    }
    if (second->field_0x8 != 0) {
        return -0x7fffffff;
    }
    if (first->score != second->score) {
        return second->score - first->score;
    }
    return gNet_players[first->player_index].last_score_index - gNet_players[second->player_index].last_score_index;
}

inline void C2_HOOK_FASTCALL ClipName(char* pName, const tDR_font* pFont, int pMax_width) {

    while (DRTextWidth(pFont, pName) > pMax_width) {
        pName[strlen(pName) - 1] = '\0';
    }
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x0049a410
// FUNCTION: CARMA2_CHEAT 0x0049a7c0
inline void C2_HOOK_FASTCALL CreateBoxes(void) {
    br_camera* camera;
    br_pixelmap* pixelmap;
    br_material* material;
    int i;
    char name[32];

    C2_HOOK_BUG_ON(REC2_ASIZE(gNetscore_box_materials) != 4);
    C2_HOOK_BUG_ON(REC2_ASIZE(gNetscore_box_actors) != 12);

    gNetscore_box_universe = BrActorAllocate(BR_ACTOR_NONE, NULL);
    for (i = 0; i < (int)REC2_ASIZE(gNetscore_box_actors); i++) {
        gNetscore_box_actors[i] = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        if (gNetscore_box_actors[i] == NULL) {
            BrFailure("Out of memory for NetPlay");
        }
        gNetscore_box_actors[i]->render_style = BR_RSTYLE_FACES;
    }
    gNetScore_box_model = BrModelAllocate("NetScore Model", 4, 2);
    gNetScore_box_model->faces[0].vertices[0] = 0;
    gNetScore_box_model->faces[0].vertices[1] = 1;
    gNetScore_box_model->faces[0].vertices[2] = 2;
    gNetScore_box_model->faces[1].vertices[0] = 1;
    gNetScore_box_model->faces[1].vertices[1] = 3;
    gNetScore_box_model->faces[1].vertices[2] = 2;
    gNetScore_box_model->faces[0].material = NULL;
    gNetScore_box_model->faces[1].material = NULL;
    BrVector3Set(&gNetScore_box_model->vertices[0].p, 0.0f, 0.0f, -2.0f);
    BrVector3Set(&gNetScore_box_model->vertices[1].p, 100.0f, 0.0f, -2.0f);
    BrVector3Set(&gNetScore_box_model->vertices[2].p, 0.0f, -38.0f, -2.0f);
    BrVector3Set(&gNetScore_box_model->vertices[3].p, 100.0f, -38.0f, -2.0f);
    BrVector2Set(&gNetScore_box_model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&gNetScore_box_model->vertices[1].map, 1.0f / 1.28f, 0.0f);
    BrVector2Set(&gNetScore_box_model->vertices[2].map, 0.0f, 0.38f / 1.28f);
    BrVector2Set(&gNetScore_box_model->vertices[3].map, 1.0f / 1.28f, 0.38f / 1.28f);
    gNetScore_box_model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrModelAdd(gNetScore_box_model);
    for (i = 0; i < (int)REC2_ASIZE(gNetscore_box_materials); i++) {
        sprintf(name, "NETBOX%i.PIX", i + 1);
        pixelmap = LoadPixelmap(name);
        if (pixelmap == NULL) {
            BrFailure("Cannot find headup graphic %s", name);
        }
        BrMapAdd(pixelmap);
        material = BrMaterialAllocate("NetScore Material");
        material->colour = 0;
        material->colour_map = pixelmap;
        material->flags = BR_MATF_FORCE_FRONT | BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(material);
        gNetscore_box_materials[i] = material;
    }
    gNetscore_box_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gNetscore_box_camera->identifier = "Net HUD Camera";
    camera = gNetscore_box_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->hither_z = 1.0f;
    camera->yon_z = 3.0f;
    camera->width = gScreen->width;
    camera->height = gScreen->height;
    BrActorAdd(gNetscore_box_universe, gNetscore_box_camera);
    gNetscore_box_created = 1;
}
#endif

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x00499640
// FUNCTION: CARMA2_CHEAT 0x004999f0
void C2_HOOK_FASTCALL DrawScoreBoxes(int pX, int pY, int pBox_index, int pMaterial_index) {
    br_actor* actor;

    if (!gNetscore_box_created) {
        CreateBoxes();
    }
    if (gNetscore_box_created) {
        actor = gNetscore_box_actors[pBox_index];
        actor->material = gNetscore_box_materials[pMaterial_index];
        actor->model = gNetScore_box_model;
        BrMatrix34Translate(&actor->t.t.mat, (br_scalar)(pX - 320), (br_scalar)(240 - pY), 0.0f);
        BrActorAdd(gNetscore_box_universe, actor);
        BrZbSceneRender(gNetscore_box_universe, gNetscore_box_camera, gRender_screen, gDepth_buffer);
        BrActorRemove(actor);
    }
}
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00499a00
// FUNCTION: CARMA2_SW 0x004962a0
// FUNCTION: CARMA2_CHEAT 0x00499db0
void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {
    int i;
    int index;
    int ascending_order;
    int my_position;
    int y;
#ifdef REC2_HW
    int box_index;
    int font;
    char rank[2];
#else
    int rank_number;
    char rank[10];
#endif
    char s[256];
    tNet_head_sort_entry entries[12];
    // GLOBAL: CARMA2_HW 0x0068d94c
    // GLOBAL: CARMA2_SW 0x006879a4
    // GLOBAL: CARMA2_CHEAT 0x006951ec
    static tU32 last_flash;
    // GLOBAL: CARMA2_HW 0x0068d950
    // GLOBAL: CARMA2_SW 0x006879a8
    // GLOBAL: CARMA2_CHEAT 0x006951f0
    static int flash_state;

    if (gCurrent_net_game->type == eNet_game_type_checkpoint || gCurrent_net_game->type == eNet_game_type_6) {
        ascending_order = 1;
    } else {
        ascending_order = 0;
    }
    Flash(200, &last_flash, &flash_state);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].player_status >= ePlayer_status_racing) {
            entries[i].player_index = i;
            entries[i].score = gNet_players[i].score2;
            if (abs(gNet_players[i].score2) == 1000000 || (gNet_players[i].score2 < 0 && gCurrent_net_game->type != eNet_game_type_1)) {
                entries[i].field_0x8 = gNet_players[i].last_score_index + 1;
            } else {
                entries[i].field_0x8 = 0;
            }
        } else {
            entries[i].player_index = -1;
            if (ascending_order) {
                entries[i].score = 1000001;
            } else {
                entries[i].score = -1000001;
            }
            entries[i].field_0x8 = 1000;
        }
    }
    for (i = gNumber_of_net_players; i < (int)REC2_ASIZE(entries); i++) {
        entries[i].player_index = -1;
        if (ascending_order) {
            entries[i].score = 1000001;
        } else {
            entries[i].score = -1000001;
        }
        entries[i].field_0x8 = 1000;
    }
    if (ascending_order) {
        qsort(entries, REC2_ASIZE(entries), sizeof(entries[0]), SortNetHeadAscending);
    } else {
        qsort(entries, REC2_ASIZE(entries), sizeof(entries[0]), SortNetHeadDescending);
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (entries[i].player_index == gThis_net_player_index) {
            my_position = i;
            break;
        }
    }
    y = 1;
#ifdef REC2_HW
    box_index = 0;
#else
    rank_number = 1;
#endif
    for (i = 0; i < (int)REC2_ASIZE(entries); i++) {
        index = entries[i].player_index;
        if (index >= 0) {
            gNet_players[index].last_score_index = i;
        }
        if (pOnly_sort_scores || index < 0) {
            continue;
        }
#ifdef REC2_HW
        if (my_position == i) {
            DrawScoreBoxes(540, y, box_index, 1);
            font = 19;
        } else {
            DrawScoreBoxes(540, y, box_index, 0);
            font = 18;
        }
#else
        if (my_position == i) {
            DimRectangle(gRender_screen, 269, y, 319, y + 14, 0);
        } else {
            DimRectangle(gRender_screen, 269, y, 319, y + 14, 0);
        }
#endif
        if (flash_state || gCurrent_net_game->type != eNet_game_type_foxy || index != gIt_or_fox) {
            if (gNet_players[index].field_0x74) {
#ifdef REC2_HW
                ClipName(gNet_players[index].player_name, &gFonts[6], 77);
#else
                ClipName(gNet_players[index].player_name, &gFonts[6], 32);
#endif
                gNet_players[index].field_0x74 = 0;
            }
#ifdef REC2_HW
            PolyFontText(gNet_players[index].player_name, 547, y + 4, font, eJust_left, 0);
#else
            DRPixelmapText(gBack_screen, 275, y + 1, &gFonts[6], gNet_players[index].player_name, 319);
#endif
        }
        if (abs(gNet_players[index].score2) == 1000000) {
            if (flash_state) {
                strcpy(s, GetMiscString(173));
            } else {
                s[0] = '\0';
            }
        } else {
            switch (gCurrent_net_game->type) {
            case eNet_game_type_fight_to_death:
                sprintf(s, "%d%%", gNet_players[index].score2);
                break;
            case eNet_game_type_1:
            case eNet_game_type_2:
                sprintf(s, "%d", gNet_players[index].score2);
                break;
            case eNet_game_type_checkpoint:
                sprintf(s, "%d left", gNet_players[index].score2 >> 16);
                break;
            case eNet_game_type_4:
                if (gNet_players[index].score2 >= 0) {
                    sprintf(s, "%s -%d-", GetMiscString(92), gNet_players[index].score2);
                } else if (flash_state) {
                    sprintf(s, "%s", GetMiscString(93));
                } else {
                    s[0] = '\0';
                }
                break;
            case eNet_game_type_5:
                sprintf(s, "%d-%d",
                    gNet_players[index].score2 / gCurrent_race.check_point_count + 1,
                    gNet_players[index].score2 % gCurrent_race.check_point_count + 1);
                break;
            case eNet_game_type_6:
                if (gNet_players[index].score2 == 0) {
                    sprintf(s, "%s", GetMiscString(192));
                } else if (flash_state) {
                    sprintf(s, "%s", GetMiscString(193));
                } else {
                    s[0] = '\0';
                }
                break;
            case eNet_game_type_foxy:
                if (index == gIt_or_fox && !flash_state) {
                    s[0] = '\0';
                } else {
                    TimerString(gNet_players[index].score2, s, 0, 1, 0);
                }
                break;
            }
        }
#ifdef REC2_HW
        rank[0] = '0' + i;
        rank[1] = '\0';
        PolyFontText(rank, 624, y + 3, 25, eJust_left, 0);
        BrMatrix34Translate(&gCar_icons_model_actor->t.t.mat, 610.0f, (br_scalar)-(y + 20), 0.0f);
        gCar_icons_model_actor->material->colour_map = gTexture_maps[gCar_icons[gNet_players[index].car_index].index];
        BrMaterialUpdate(gCar_icons_model_actor->material, BR_MATU_COLOURMAP);
        gCar_icons_model_actor->model = gCar_icons[gNet_players[index].car_index].model;
        BrActorAdd(gNetscore_box_camera, gCar_icons_model_actor);
        BrZbsSceneRender(gNetscore_box_camera, gNetscore_box_camera, gBack_screen, gDepth_buffer);
        BrActorRemove(gCar_icons_model_actor);
        PolyFontText(s, 547, y + 18, font, eJust_left, 0);
        y += 40;
        box_index++;
#else
        sprintf(rank, "%i", rank_number);
        DRPixelmapText(gBack_screen, 270, y + 1, &gFonts[6], rank, 319);
        DRPixelmapText(gBack_screen, 319 - DRTextWidth(&gFonts[6], s), y + 8, &gFonts[6], s, 319);
        DRPixelmapRectangleMaskedCopy(gBack_screen,
            270,
            y + 8,
            gIcons_low_pix,
            0,
            gNet_players[index].car_index * gCurrent_graf_data->car_icon_height,
            gIcons_low_pix->width,
            gCurrent_graf_data->car_icon_height);
        y += 16;
        rank_number++;
#endif
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004999f0
// FUNCTION: CARMA2_SW 0x00496290
// FUNCTION: CARMA2_CHEAT 0x00499da0
void C2_HOOK_FASTCALL DoNetScores(void) {

    DoNetScores2(0);
}

// FUNCTION: CARMA2_HW 0x0049a710
// FUNCTION: CARMA2_SW 0x00496890
// FUNCTION: CARMA2_CHEAT 0x0049aac0
void C2_HOOK_FASTCALL InitNetHeadups(void) {

    gDigits_pix = LoadPixelmap("HDIGITS.PIX");
    if (gDigits_pix != NULL) {
        BrMapAdd(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049a730
// FUNCTION: CARMA2_SW 0x004968b0
// FUNCTION: CARMA2_CHEAT 0x0049aae0
void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (gDigits_pix != NULL) {
        BrMapRemove(gDigits_pix);
        BrPixelmapFree(gDigits_pix);
    }
}

inline void C2_HOOK_FASTCALL EverybodysLost(void) {
    tNet_message* the_message;
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        gNet_players[i].field_0x7c++;
        the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_race_over, 0);
        the_message->contents.race_over.reason = eRace_over_9;
        NetGuaranteedSendMessageToPlayer(gCurrent_net_game, the_message, gNet_players[i].ID, NULL);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049a760
// STUB: CARMA2_SW 0x004968e0
// STUB: CARMA2_CHEAT 0x0049ab10
void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0049a940
// FUNCTION: CARMA2_SW 0x00496ac0
// FUNCTION: CARMA2_CHEAT 0x0049acf0
void C2_HOOK_FASTCALL CheckForVampireWinner(void) {
    int i;
    int players_left;
    int winner_index;

    players_left = 0;
    winner_index = 0;
    if (gProgram_state.racing && gNet_mode == eNet_mode_host) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (!gNet_players[i].field_0x80) {
                players_left++;
                winner_index = i;
            }
        }
        if (players_left <= 1) {
            DeclareWinner(winner_index);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049b540
// FUNCTION: CARMA2_SW 0x004976c0
// FUNCTION: CARMA2_CHEAT 0x0049b8f0
void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer) {
    int i;
    char s[256];

    C2_HOOK_BUG_ON(REC2_ASIZE(pPlayer->car->power_up_levels) != 3);

    if (!pPlayer->field_0x80) {
        MakeCarIt(pPlayer->car);
        pPlayer->car->invulnerable_no_wastage = 1;
        pPlayer->car->invulnerable_no_crushage = 1;
        pPlayer->car->invulnerable_no_damage = 1;
        pPlayer->field_0x80 = 1;
        for (i = 0; i < (int)REC2_ASIZE(pPlayer->car->power_up_levels); i++) {
            pPlayer->car->power_up_levels[i] = pPlayer->car->power_up_slots[i];
        }
        if (gNet_mode == eNet_mode_host) {
            sprintf(s, "%s", GetMiscString(186));
            NetSendHeadupToPlayer(s, pPlayer->ID);
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (&gNet_players[i] != pPlayer) {
                    sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(eMiscString_is_undead));
                    NetSendHeadupToPlayer(s, gNet_players[i].ID);
                }
            }
        }
        pPlayer->score2 = -(int)(1 + GetTotalTime());
        CheckForVampireWinner();
    }
}

inline int C2_HOOK_FASTCALL FarEnoughAway(tNet_game_player_info* pPlayer_1, tNet_game_player_info* pPlayer_2) {
    br_vector3 difference;

    BrVector3Sub(&difference, &pPlayer_1->car->pos, &pPlayer_2->car->pos);
    return BrVector3LengthSquared(&difference) >= 4.0;
}

// FUNCTION: CARMA2_HW 0x0049a990
// FUNCTION: CARMA2_SW 0x00496b10
// FUNCTION: CARMA2_CHEAT 0x0049ad40
void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer) {

    if ((int)(PDGetTotalTime() - gLast_it_change) > 500) {
        if (gLast_lepper != pPlayer) {
            gLast_it_change = PDGetTotalTime();
            if (gIt_or_fox >= 0) {
                gLast_lepper = &gNet_players[gIt_or_fox];
            }
            PlayerIsIt(pPlayer);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049a9e0
// FUNCTION: CARMA2_SW 0x00496b60
// FUNCTION: CARMA2_CHEAT 0x0049ad90
void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer) {
    int i;
    char s[256];

    if (gIt_or_fox == pPlayer - gNet_players) {
        return;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        StopCarBeingIt(gNet_players[i].car);
    }
    if (gCurrent_net_game->type == eNet_game_type_foxy) {
        pPlayer->car->power_up_levels[1] = 0;
    }
    MakeCarIt(pPlayer->car);
    gIt_or_fox = pPlayer - gNet_players;
    sprintf(s, "%s", GetMiscString(eMiscString_you_re_the_fox));
    NetSendHeadupToPlayer(s, pPlayer->ID);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (&gNet_players[i] != pPlayer) {
            sprintf(s, "%s %s", pPlayer->player_name, GetMiscString(eMiscString_is_the_fox));
            NetSendHeadupToPlayer(s, gNet_players[i].ID);
        }
    }
}

inline void C2_HOOK_FASTCALL SelectRandomFox(int pNot_this_one) {
    int i;
    int new_choice;

    gLast_lepper = NULL;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != pNot_this_one && gNet_players[i].last_score_index == gNumber_of_net_players - 1) {
            PlayerIsIt(&gNet_players[i]);
            return;
        }
    }
    do {
        new_choice = IRandomBetween(0, gNumber_of_net_players - 1);
    } while (new_choice == pNot_this_one && !gNet_players[new_choice].car->knackered);
    PlayerIsIt(&gNet_players[new_choice]);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0049abf0
// FUNCTION: CARMA2_SW 0x00496d70
// FUNCTION: CARMA2_CHEAT 0x0049afa0
void C2_HOOK_FASTCALL CalcPlayerScores(void) {
    int i;
    int j;
    int knock_out_bit;
    int cars_left;
    int car_left;
    int flags;
    int score;
    int highest;
    int next_highest;
    int lowest_score;
    int player_left;
    tCar_spec* car;
    tNet_message* message;
    tS32 time;
    char s[256];
    tNet_game_player_info* lowest_score_player;

    time = GetTotalTime();
    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        highest = 0;
        next_highest = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 > highest) {
                next_highest = highest;
                highest = gNet_players[i].score2;
            } else if (gNet_players[i].score2 > next_highest) {
                next_highest = gNet_players[i].score2;
            }
        }
        gINT_0068d920 = (gCount_smashable_race_targets - (GetRuntimeVariable(1) - highest - next_highest)) / 2 + 1;
        if (gCurrent_net_game->options.starting_target < gINT_0068d920) {
            gINT_0068d920 = gCurrent_net_game->options.starting_target;
        }
        break;
    case eNet_game_type_foxy:
        if (gIt_or_fox < 0) {
            SelectRandomFox(-1);
        }
        if (gLast_lepper != NULL && gIt_or_fox >= 0 && FarEnoughAway(gLast_lepper, &gNet_players[gIt_or_fox])) {
            gLast_lepper = NULL;
        }
        break;
    default:
        break;
    }
    lowest_score = 9999;
    lowest_score_player = NULL;
    for (i = 0; i < gNumber_of_net_players; i++) {
        car = gNet_players[i].car;
        if (gNet_players[i].reposition_time != 0 && (tS32)gNet_players[i].reposition_time <= time
                && (!gRace_finished || gRace_over_reason == eRace_not_over_yet)) {
            RepositionPlayer(i);
        }
        if (gNet_players[i].last_waste_message != 0
                && !gNet_players[i].wasteage_attributed
                && time - gNet_players[i].last_waste_message > 500) {
            sprintf(s, "%s %s", gNet_players[i].player_name, GetMiscString(172));
            gNet_players[i].last_waste_message = 0;
            gNet_players[i].wasteage_attributed = 0;
            if (gCurrent_net_game->type == eNet_game_type_1) {
                gNet_players[i].score2--;
            }
            NetSendHeadupToEverybody(s);
        }
        SetKnackeredFlag(car);
        if (car->knackered && !gNet_players[i].wasted) {
            gNet_players[i].wasted = 1;
            message = NetBuildGuaranteedMessage(eNet_message_chunk_type_wasted, 0);
            message->contents.wasted.victim = gNet_players[i].ID;
            message->contents.wasted.culprit = -1;
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, NULL);
            switch (gCurrent_net_game->type) {
            case eNet_game_type_fight_to_death:
                cars_left = 0;
                for (j = 0; j < gNumber_of_net_players; j++) {
                    if (!gNet_players[j].wasted) {
                        cars_left++;
                        car_left = j;
                    }
                }
                gNet_players[i].score += gGame_scores[REC2_ASIZE(gGame_scores) - 1 - cars_left];
                if (cars_left == 1) {
                    DeclareWinner(car_left);
                } else if (cars_left <= 0) {
                    EverybodysLost();
                }
                break;
            case eNet_game_type_1:
            case eNet_game_type_4:
            case eNet_game_type_5:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_2:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                break;
            case eNet_game_type_checkpoint:
                if (gNet_players[i].score2 >> 16 != gCurrent_race.check_point_count) {
                    knock_out_bit = IRandomBetween(0, gCurrent_race.check_point_count - 1);
                    while (((1 << knock_out_bit) & gNet_players[i].score2) != 0) {
                        knock_out_bit++;
                        if (knock_out_bit >= gCurrent_race.check_point_count) {
                            knock_out_bit = 0;
                        }
                    }
                    gNet_players[i].score2 |= 1 << knock_out_bit;
                }
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                break;
            case eNet_game_type_6:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                if (!gNet_players[i].field_0x80) {
                    PlayerIsInfected(&gNet_players[i]);
                }
                break;
            case eNet_game_type_foxy:
                gNet_players[i].reposition_time = GetTotalTime() + 5000;
                gNet_players[i].score2 /= 2;
                if (gNumber_of_net_players > 1 && i == gIt_or_fox) {
                    SelectRandomFox(i);
                }
                break;
            default:
                break;
            }
        }
        switch (gCurrent_net_game->type) {
        case eNet_game_type_fight_to_death:
            if (car->knackered) {
                if (gCurrent_net_game->type == eNet_game_type_checkpoint || gCurrent_net_game->type == eNet_game_type_6) {
                    gNet_players[i].score2 = 1000000;
                } else {
                    gNet_players[i].score2 = -1000000;
                }
            } else {
                gNet_players[i].score2 = PercentageCarHealth(car);
            }
            break;
        case eNet_game_type_2:
            if (gNet_players[i].score2 >= gINT_0068d920 && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_checkpoint:
            score = 0;
            gNet_players[i].score2 &= 0xffff;
            flags = gNet_players[i].score2;
            for (j = 0; j < gCurrent_race.check_point_count; j++) {
                if (flags & 0x1) {
                    score++;
                }
                flags >>= 1;
            }
            gNet_players[i].score2 |= score << 16;
            if (score == 0 && !gRace_finished) {
                DeclareWinner(i);
            }
            break;
        case eNet_game_type_4:
            if (gNet_players[i].score2 >= 0) {
                if (gNet_players[i].score2 < lowest_score) {
                    lowest_score = gNet_players[i].score2;
                    lowest_score_player = &gNet_players[i];
                } else if (gNet_players[i].score2 == lowest_score) {
                    lowest_score_player = NULL;
                }
            }
            break;
        case eNet_game_type_foxy:
            if (i == gIt_or_fox && !gCountdown && !gRace_finished) {
                gNet_players[i].score2 += gFrame_period;
                if (gNet_players[i].score2 >= gCurrent_net_game->options.starting_target) {
                    DeclareWinner(i);
                }
            }
            break;
        default:
            break;
        }
    }
    if (gCurrent_net_game->type == eNet_game_type_4 && lowest_score_player != NULL && lowest_score_player->score2 >= 0) {
        player_left = -1;
        cars_left = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].score2 >= 0 && &gNet_players[i] != lowest_score_player) {
                cars_left++;
                if (player_left == -1) {
                    player_left = i;
                } else {
                    player_left = -2;
                }
            }
        }
        if (cars_left != 0) {
            lowest_score_player->car->knackered = 1;
            lowest_score_player->wasted = 1;
            lowest_score_player->score += gGame_scores[REC2_ASIZE(gGame_scores) - 1 - cars_left];
            lowest_score_player->score2 = -1;
            if (player_left == -1) {
                EverybodysLost();
            } else if (player_left >= 0) {
                DeclareWinner(player_left);
            } else {
                SendGameplay(lowest_score_player->ID, eNet_gameplay_suddenly_death, 0, 0, 0, 0);
                sprintf(s, "%s %s", lowest_score_player->player_name, GetMiscString(184));
                NetSendHeadupToEverybody(s);
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL SendPlayerScores(void) {
    tNet_message_chunk* the_contents;
    int i;

    the_contents = NetStartBroadcastContents(eNet_message_chunk_type_player_scores, 0);
    if (the_contents == NULL) {
        return;
    }
    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        the_contents->scores.general_score = gINT_0068d920;
        break;
    case eNet_game_type_6:
        the_contents->scores.general_score = 0;
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].field_0x80) {
                the_contents->scores.general_score |= 1 << i;
            }
        }
        break;
    case eNet_game_type_foxy:
        the_contents->scores.general_score = gNet_players[gIt_or_fox].ID;
        break;
    default:
        break;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        the_contents->scores.scores[i] = gNet_players[i].score2;
    }
    NetBroadcastContents(the_contents);
}

// FUNCTION: CARMA2_HW 0x0049ab00
// FUNCTION: CARMA2_SW 0x00496c80
// FUNCTION: CARMA2_CHEAT 0x0049aeb0
void C2_HOOK_FASTCALL DoNetGameManagement(void) {
    // GLOBAL: CARMA2_HW 0x00659c34
    // GLOBAL: CARMA2_SW 0x006545cc
    // GLOBAL: CARMA2_CHEAT 0x00660554
    static tU32 last_playerscores_sent = (tU32)-1;

    if (gNet_mode == eNet_mode_host) {
        CalcPlayerScores();
        if (gPHIL_last_physics_tick <= last_playerscores_sent || gPHIL_last_physics_tick >= last_playerscores_sent + 80) {
            last_playerscores_sent = gPHIL_last_physics_tick;
            SendPlayerScores();
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049b690
// FUNCTION: CARMA2_SW 0x00497810
// FUNCTION: CARMA2_CHEAT 0x0049ba40
void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer) {

    PossibleService();
    switch (gCurrent_net_game->type) {
    case eNet_game_type_fight_to_death:
        pPlayer->score2 = 100;
        break;
    case eNet_game_type_1:
    case eNet_game_type_2:
    case eNet_game_type_4:
    case eNet_game_type_5:
    case eNet_game_type_6:
        pPlayer->score2 = 0;
        break;
    case eNet_game_type_checkpoint:
        pPlayer->score2 = 0xffff;
        break;
    case eNet_game_type_foxy:
        pPlayer->score2 = 0;
        break;
    }
    pPlayer->credits = gCurrent_net_game->options.starting_credits;
    pPlayer->wasted = 0;
    pPlayer->reposition_time = 0;
}

// InitPlayers

// FUNCTION: CARMA2_HW 0x0049b7e0
// FUNCTION: CARMA2_SW 0x00497960
// FUNCTION: CARMA2_CHEAT 0x0049bb90
void C2_HOOK_FASTCALL UseGeneralScore(int pScore) {
    int i;

    switch (gCurrent_net_game->type) {
    case eNet_game_type_2:
        gINT_0068d920 = pScore;
        break;
    case eNet_game_type_6:
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (!gNet_players[i].field_0x80 && (pScore & (1 << i))) {
                PlayerIsInfected(&gNet_players[i]);
            }
        }
        break;
    case eNet_game_type_foxy:
        if (gNet_players[gIt_or_fox].ID != (tPlayer_ID)pScore) {
            for (i = 0; i < gNumber_of_net_players; i++) {
                StopCarBeingIt(gNet_players[i].car);
            }
            for (i = 0; i < gNumber_of_net_players; i++) {
                if (gNet_players[i].ID == (tPlayer_ID)pScore) {
                    MakeCarIt(gNet_players[i].car);
                    gIt_or_fox = i;
                }
            }
        }
        break;
    default:
        break;
    }
}

// NetSendEnvironmentChanges

// STUB: CARMA2_HW 0x0049b9a0
// STUB: CARMA2_SW 0x00497b20
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049bad0
// FUNCTION: CARMA2_SW 0x00497c50
// FUNCTION: CARMA2_CHEAT 0x0049be80
void C2_HOOK_FASTCALL ReceivedGameplay(tNet_message_chunk* pContents, tNet_message* pMessage, tU32 pReceive_time) {
    int must_revert_reentrancy;

    // GLOBAL: CARMA2_HW 0x0068d954
    // GLOBAL: CARMA2_SW 0x006879ac
    // GLOBAL: CARMA2_CHEAT 0x006951f4
    static int pause_semaphore;

    if (pContents->gameplay.mess == eNet_gameplay_host_paused) {
        if (!pause_semaphore) {
            pause_semaphore = 1;
            NetFullScreenMessage(eMiscString_please_wait_host_has_paused, 1);
            must_revert_reentrancy = PermitNetServiceReentrancy();
            do {
                NetService(0);
                if (CheckQuit()) {
                    NetFullScreenMessage(eMiscString_please_wait_host_has_paused, 1);
                }
            } while (gWaiting_for_unpause
                && gProgram_state.prog_status != eProg_idling
                && (!gRace_finished || gRace_over_reason != eRace_over_abandoned));
            if (must_revert_reentrancy) {
                HaltNetServiceReentrancy();
            }
            gWaiting_for_unpause = 1;
            FadePaletteDown();
            ClearEntireScreen();
            FadePaletteUp();
            pause_semaphore = 0;
        }
    } else if (pContents->gameplay.mess == eNet_gameplay_host_unpaused) {
        gWaiting_for_unpause = 0;
    } else if (pContents->gameplay.mess == eNet_gameplay_suicide) {
        KnackerThisCar(NetCarFromPlayerID(pMessage->header.field_0x8));
    } else {
        switch (gCurrent_net_game->type) {
        case eNet_game_type_checkpoint:
        case eNet_game_type_4:
        case eNet_game_type_5:
            switch (pContents->gameplay.mess) {
            case eNet_gameplay_checkpoint:
                Checkpoint(pContents->gameplay.param_1, 1);
                break;
            case eNet_gameplay_wrong_checkpoint:
                WrongCheckpoint(pContents->gameplay.param_1);
                break;
            case eNet_gameplay_suddenly_death:
                DoFancyHeadup(26);
                ChangeAmbientPratcam(36);
                gRace_finished = 1;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049bc20
// FUNCTION: CARMA2_SW 0x00497da0
// FUNCTION: CARMA2_CHEAT 0x0049bfd0
void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_gameplay, 0);
    the_message->contents.gameplay.mess = pMess;
    the_message->contents.gameplay.param_1 = pParam_1;
    the_message->contents.gameplay.param_2 = pParam_2;
    the_message->contents.gameplay.param_3 = pParam_3;
    the_message->contents.gameplay.param_4 = pParam_4;
    NetGuaranteedSendMessageToPlayer(gCurrent_net_game, the_message, pPlayer, NULL);
}

// FUNCTION: CARMA2_HW 0x0049bc70
// FUNCTION: CARMA2_SW 0x00497df0
// FUNCTION: CARMA2_CHEAT 0x0049c020
void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, mess, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_2, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_3, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_gameplay, param_4, 0x14);

    the_message = NetBuildGuaranteedMessage(eNet_message_chunk_type_gameplay, 0);
    the_message->contents.gameplay.mess = pMess;
    the_message->contents.gameplay.param_1 = pParam_1;
    the_message->contents.gameplay.param_2 = pParam_2;
    the_message->contents.gameplay.param_3 = pParam_3;
    the_message->contents.gameplay.param_4 = pParam_4;
    NetGuaranteedSendMessageToAllPlayers(gCurrent_net_game, the_message, NULL);
}

// FUNCTION: CARMA2_HW 0x0049bcb0
// FUNCTION: CARMA2_SW 0x00497e30
// FUNCTION: CARMA2_CHEAT 0x0049c060
void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4) {
    tNet_message* the_message;

    if (gNet_mode == eNet_mode_client) {
        the_message = NetBuildGuaranteedMessage(0x1b, 0);
        the_message->contents.gameplay.mess = pMess;
        the_message->contents.gameplay.param_1 = pParam_1;
        the_message->contents.gameplay.param_2 = pParam_2;
        the_message->contents.gameplay.param_3 = pParam_3;
        the_message->contents.gameplay.param_4 = pParam_4;
        NetGuaranteedSendMessageToHost(gCurrent_net_game, the_message, NULL);
    }
}

// FUNCTION: CARMA2_HW 0x0049bd00
// FUNCTION: CARMA2_SW 0x00497e80
// FUNCTION: CARMA2_CHEAT 0x0049c0b0
void C2_HOOK_FASTCALL InitNetGameplayStuff(void) {

}

// FUNCTION: CARMA2_HW 0x0049bd10
// FUNCTION: CARMA2_SW 0x00497e90
// FUNCTION: CARMA2_CHEAT 0x0049c0c0
void C2_HOOK_FASTCALL DefaultNetName(void) {

    NetObtainSystemUserName(gProgram_state.player_name, 32);
}

// FUNCTION: CARMA2_HW 0x0049bde0
// FUNCTION: CARMA2_SW 0x00497f60
// FUNCTION: CARMA2_CHEAT 0x0049c190
void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers) {
    tCar_crush_network_shapes* shapes;
    int j;

    for (; pCount > 0; pCount--, pPlayers++) {
        if (pPlayers->car != NULL && pPlayers->car->car_crush_spec != NULL) {
            shapes = pPlayers->car->car_crush_spec->network_stuff;
            for (j = 0; j < (int)REC2_ASIZE(shapes->items); j++) {
                if (shapes->items[j].network_id == 0) {
                    shapes->items[j].network_id = pPlayer_id;
                    shapes->items[j].field_0x4 = 1;
                    break;
                }
            }
        }
    }
}

// RemovePlayerFromShapeStatusLists


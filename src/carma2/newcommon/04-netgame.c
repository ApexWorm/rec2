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
#include "27-powerup.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "52-errors.h"
#include "53-controls.h"
#include "58-crush2.h"
#include "59-camera.h"
#include "60-phil.h"
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

// GLOBAL: CARMA2_HW 0x0068d920
// GLOBAL: CARMA2_SW 0x00687978
// GLOBAL: CARMA2_CHEAT 0x006951c0
int gINT_0068d920;

// GLOBAL: CARMA2_HW 0x0068d924
// GLOBAL: CARMA2_SW 0x0068797c
tNet_game_player_info* gLast_lepper;

// GLOBAL: CARMA2_HW 0x0068d928
// GLOBAL: CARMA2_SW 0x00687980
int gLast_it_change;

// GLOBAL: CARMA2_HW 0x0068d92c
// GLOBAL: CARMA2_SW 0x00687984
// GLOBAL: CARMA2_cHEAT 0x006951cc
tU32 gTime_for_punishment;

inline void C2_HOOK_FASTCALL SendShapeNumbers(void) {
    tNet_message_chunk* contents;
    int i;

    contents = NetGetToHostContents(eNet_message_chunk_type_shape_numbers, gNumber_of_net_players * sizeof(contents->shape_numbers.shapes[0]) + 4);
    contents->shape_numbers.player_id = gNet_players[gThis_net_player_index].ID;
    contents->shape_numbers.count = gNumber_of_net_players - 1;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            contents->shape_numbers.shapes[i].player = gNet_players[i].ID;
            contents->shape_numbers.shapes[i].shape_number = gNet_players[i].car->car_crush_spec->field_0x24;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00497ef0
// FUNCTION: CARMA2_SW 0x00494b20
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

// FUNCTION: CARMA2_HW 0x00497f10
// FUNCTION: CARMA2_SW 0x00494b40
// FUNCTION: CARMA2_CHEAT 0x004982c0
void C2_HOOK_FASTCALL SendNonCar(tNon_car_spec* pNon_car, tU32 pTime) {
    int size;
    tNet_message_chunk* contents;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, ID, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, flags, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_non_car, object_hierarchy, 0x12);

    size = GetHierarchyNetworkSize(pNon_car->collision_info);
    contents = NetStartBroadcastContents(eNet_message_chunk_type_non_car, size + offsetof(tNet_message_chunk_non_car, object_hierarchy));
    if (contents != NULL) {
        contents->non_car.ID = (tU16)pNon_car->car_ID;
        contents->non_car.time = pTime;
        contents->non_car.flags = ((pNon_car->flags & 0xff00) != 0) << 3
            | ((pNon_car->flags & 0xff) != 0) << 2
            | pNon_car->collision_info->disable_move_rotate << 1
            | (pNon_car->car_master_actor->identifier[3] == '!');
        if (GetHierarchyNetworkStuff(pNon_car->collision_info, contents->non_car.object_hierarchy, 0x48) != size) {
            FatalError(kFatalError_NetContentsTooBig_S, "in non_car object data");
        }
        NetBroadcastContents(contents);
    }
}

inline void C2_HOOK_FASTCALL HostFillInCarContents(tNet_message_chunk* pContents, int pPlayer_index, tU32 pTime) {
    tCar_spec* car;
    tU8 network_size;
    tU8 flags;
    tU8 size;
    float* wheel_dam_offset;
    int i;

    car = gNet_players[pPlayer_index].car;
    flags = 0;
    network_size = GetHierarchyNetworkSize(car->collision_info);
    size = network_size + offsetof(tNet_message_chunk_mechanics, data);
    if (car->damage_units[eDamage_lf_wheel].damage_level > 30
            || car->damage_units[eDamage_rf_wheel].damage_level > 30
            || car->damage_units[eDamage_lr_wheel].damage_level > 30
            || car->damage_units[eDamage_rr_wheel].damage_level > 30) {
        flags = 0x8;
        size += sizeof(car->wheel_dam_offset);
    }
    pContents->mechanics.header.size = size;
    pContents->mechanics.header.type = eNet_message_chunk_type_mechanics;
    pContents->mechanics.player = gNet_players[pPlayer_index].ID;
    pContents->mechanics.time = pTime;
    pContents->mechanics.curvature = (tS16)(car->curvature / car->maxcurve * 32767.0f);
    pContents->mechanics.keys = car->keys;
    pContents->mechanics.keys.joystick_acc = car->joystick.acc >> 9;
    pContents->mechanics.keys.joystick_dec = car->joystick.dec >> 9;
    pContents->mechanics.revs = (tS16)car->revs;
    for (i = 0; i < (int)REC2_ASIZE(pContents->mechanics.d); i++) {
        pContents->mechanics.d[i] = (tU8)(car->oldd[i] / car->susp_height[i >> 1] * 255.0f);
    }
    for (i = 0; i < (int)REC2_ASIZE(pContents->mechanics.damage); i++) {
        pContents->mechanics.damage[i] = car->damage_units[i].damage_level;
    }
    pContents->mechanics.powerups[0] = car->power_up_levels[0];
    pContents->mechanics.powerups[1] = car->power_up_levels[1];
    pContents->mechanics.powerups[2] = car->power_up_levels[2];
    pContents->mechanics.repair_time = car->repair_time;
    pContents->mechanics.cc_coll_time = car->collision_info->field_0x49c;
    pContents->mechanics.shape_number = car->car_crush_spec->field_0x24;
    pContents->mechanics.flags = flags;
    if (GetHierarchyNetworkStuff(car->collision_info, pContents->mechanics.data, 0x68) != network_size) {
        FatalError(kFatalError_NetContentsTooBig_S, "in car object data");
    }
    if (flags & 0x8) {
        wheel_dam_offset = (float*)&pContents->mechanics.data[network_size];
        for (i = 0; i < (int)REC2_ASIZE(car->wheel_dam_offset); i++) {
            wheel_dam_offset[i] = car->wheel_dam_offset[i];
        }
    }
}

// FUNCTION: CARMA2_HW 0x00497fc0
// FUNCTION: CARMA2_SW 0x00494bf0
// FUNCTION: CARMA2_CHEAT 0x00498370
void C2_HOOK_FASTCALL ClientSendCarData(tU32 pNext_frame_time) {
    tCar_spec* car;
    tNet_message_chunk* pContents;
    tU8 network_size;
    tU8 flags;
    tU8 bend_flags;
    br_angle bend_angle;
    int size;
    int shape_size;
    tU8* data;
    float* wheel_dam_offset;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, player, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, d, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, keys, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, cc_coll_time, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, curvature, 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, revs, 0x1a);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, repair_time, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, damage, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, powerups, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, shape_number, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, flags, 0x32);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_mechanics, data, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, susp_height, 0x1218);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, curvature, 0x1254);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, maxcurve, 0x1258);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, oldd, 0x1264);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, keys, 0x12d0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wheel_dam_offset, 0x132c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, revs, 0x1340);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, time_to_recover, 0x18a0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, repair_time, 0x18a4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_crush_spec, 0x18d4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_crush_spec, field_0x190, 0x190);

    car = &gProgram_state.current_car;
    if (car->disabled) {
        return;
    }
    flags = 0;
    network_size = GetHierarchyNetworkSize(car->collision_info);
    size = network_size + offsetof(tNet_message_chunk_mechanics, data);
    if (car->damage_units[eDamage_lf_wheel].damage_level > 30
            || car->damage_units[eDamage_rf_wheel].damage_level > 30
            || car->damage_units[eDamage_lr_wheel].damage_level > 30
            || car->damage_units[eDamage_rr_wheel].damage_level > 30) {
        flags = 0x8;
        size += sizeof(car->wheel_dam_offset);
    }
    if (car->car_crush_spec->version_le_100) {
        if (car->car_crush_spec->field_0x28) {
            if (car->car_crush_spec->field_0x24 & 0x1) {
                car->car_crush_spec->field_0x24 += 1;
            } else {
                car->car_crush_spec->field_0x24 += 2;
            }
            car->car_crush_spec->field_0x28 = 0;
        }
        if (car->car_crush_spec->field_0x24 != car->car_crush_spec->field_0x26) {
            flags |= 0x1;
            shape_size = GetNetShapeDataLengthFromShape(car);
            size += shape_size;
            if (car->car_crush_spec->field_0x144) {
                flags |= 0x2;
                size += 3;
            }
            if (car->car_crush_spec->field_0x4b8) {
                flags |= 0x4;
                size += sizeof(tCompressed_vector4);
            }
        }
    }
    pContents = NetGetToHostContents(eNet_message_chunk_type_mechanics, size);
    pContents->mechanics.time = pNext_frame_time;
    pContents->mechanics.player = gNet_players[gThis_net_player_index].ID;
    pContents->mechanics.curvature = (tS16)(car->curvature / car->maxcurve * 32767.0f);
    pContents->mechanics.keys = car->keys;
    pContents->mechanics.keys.joystick_acc = car->joystick.acc >> 9;
    pContents->mechanics.keys.joystick_dec = car->joystick.dec >> 9;
    pContents->mechanics.revs = (tS16)car->revs;
    pContents->mechanics.cc_coll_time = car->collision_info->field_0x49c;
    for (i = 0; i < (int)REC2_ASIZE(pContents->mechanics.d); i++) {
        pContents->mechanics.d[i] = (tU8)(car->oldd[i] / car->susp_height[i >> 1] * 255.0f);
    }
    for (i = 0; i < (int)REC2_ASIZE(pContents->mechanics.damage); i++) {
        pContents->mechanics.damage[i] = car->damage_units[i].damage_level;
    }
    pContents->mechanics.powerups[0] = car->power_up_levels[0];
    pContents->mechanics.powerups[1] = car->power_up_levels[1];
    pContents->mechanics.powerups[2] = car->power_up_levels[2];
    pContents->mechanics.repair_time = car->repair_time;
    pContents->mechanics.shape_number = car->car_crush_spec->field_0x24;
    pContents->mechanics.flags = flags;
    data = pContents->mechanics.data;
    if (GetHierarchyNetworkStuff(car->collision_info, data, 0x68) != network_size) {
        FatalError(kFatalError_NetContentsTooBig_S, "in car object data");
    }
    data += network_size;
    if (flags & 0x8) {
        wheel_dam_offset = (float*)data;
        for (i = 0; i < (int)REC2_ASIZE(car->wheel_dam_offset); i++) {
            wheel_dam_offset[i] = car->wheel_dam_offset[i];
        }
        data += sizeof(car->wheel_dam_offset);
    }
    if (flags & 0x1) {
        EncodeNetShapeData(data, car);
        data += shape_size;
    }
    if (flags & 0x2) {
        bend_flags = 0;
        if (car->car_crush_spec->field_0x174 == car->car_crush_spec->bend_z_min) {
            bend_flags = 0x1;
        }
        if (car->car_crush_spec->field_0x190[0] != 0) {
            bend_flags |= 0x2;
            bend_angle = car->car_crush_spec->field_0x190[0];
        } else if (car->car_crush_spec->field_0x190[1] != 0) {
            bend_flags |= 0x4;
            bend_angle = car->car_crush_spec->field_0x190[1];
        } else {
            bend_flags |= 0x8;
            bend_angle = car->car_crush_spec->field_0x190[2];
        }
        *data = bend_flags;
        data++;
        *(br_angle*)data = bend_angle;
        data += sizeof(br_angle);
    }
    if (flags & 0x4) {
        CompressPlane((tCompressed_vector4*)data, &car->car_crush_spec->field_0x548);
    }
    if (car->time_to_recover != 0 && car->time_to_recover - 500 < pNext_frame_time) {
        pContents = NetGetToHostContents(eNet_message_chunk_type_recover, 0);
        pContents->recover.player = gNet_players[gThis_net_player_index].ID;
        pContents->recover.time = car->time_to_recover;
    }
    SendShapeNumbers();
}

inline br_scalar C2_HOOK_FASTCALL DistanceBetweenCars(tCar_spec* pCar_1, tCar_spec* pCar_2) {
    br_vector3 tv;

    BrVector3Sub(&tv, &pCar_1->car_master_actor->t.t.translate.t, &pCar_2->car_master_actor->t.t.translate.t);
    return BrVector3Length(&tv);
}

// FUNCTION: CARMA2_HW 0x00498370
// FUNCTION: CARMA2_SW 0x00494fa0
int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id == pPlayer_id) {
            return (tS8)pShapes->items[i].field_0x4;
        }
    }
    return 0;
}

inline void C2_HOOK_FASTCALL AddShapeStuffToMechanicsMessage(tNet_message_chunk* pContents, tNet_game_player_info* pPlayer, tNet_game_player_info* pTo_player) {
    tCar_spec* car;
    tU8 flags;
    tU8 bend_flags;
    br_angle bend_angle;
    int size;
    int shape_size;
    tU8* data;

    car = pPlayer->car;
    pContents->mechanics.flags &= ~0x7;
    if (!car->car_crush_spec->version_le_100) {
        return;
    }
    if (car->car_crush_spec->field_0x28) {
        if (car->car_crush_spec->field_0x24 & 0x1) {
            car->car_crush_spec->field_0x24 += 2;
        } else {
            car->car_crush_spec->field_0x24 += 1;
        }
        SetAllShapeStatusFlags(car->car_crush_spec->network_stuff);
        ClearShapeStatusFlag(car->car_crush_spec->network_stuff, pPlayer->ID);
        car->car_crush_spec->field_0x28 = 0;
    }
    if (!CarShapeNeedsSendingToPlayer(car->car_crush_spec->network_stuff, pTo_player->ID)) {
        return;
    }
    flags = 0x1;
    shape_size = GetNetShapeDataLengthFromShape(car);
    size = shape_size;
    if (car->car_crush_spec->field_0x144) {
        flags |= 0x2;
        size += 3;
    }
    if (car->car_crush_spec->field_0x4b8) {
        flags |= 0x4;
        size += sizeof(tCompressed_vector4);
    }
    pContents->mechanics.flags |= flags;
    data = (tU8*)pContents + pContents->mechanics.header.size;
    pContents->mechanics.header.size += size;
    if (flags & 0x1) {
        EncodeNetShapeData(data, car);
        data += shape_size;
    }
    if (flags & 0x2) {
        bend_flags = 0;
        if (car->car_crush_spec->field_0x174 == car->car_crush_spec->bend_z_min) {
            bend_flags = 0x1;
        }
        if (car->car_crush_spec->field_0x190[0] != 0) {
            bend_flags |= 0x2;
            bend_angle = car->car_crush_spec->field_0x190[0];
        } else if (car->car_crush_spec->field_0x190[1] != 0) {
            bend_flags |= 0x4;
            bend_angle = car->car_crush_spec->field_0x190[1];
        } else {
            bend_flags |= 0x8;
            bend_angle = car->car_crush_spec->field_0x190[2];
        }
        *data = bend_flags;
        data++;
        *(br_angle*)data = bend_angle;
        data += sizeof(br_angle);
    }
    if (flags & 0x4) {
        CompressPlane((tCompressed_vector4*)data, &car->car_crush_spec->field_0x548);
    }
}

// FUNCTION: CARMA2_HW 0x004978d0
// FUNCTION: CARMA2_SW 0x00494500
int C2_HOOK_FASTCALL TimeToSendData(void) {
    // GLOBAL: CARMA2_HW 0x00659c30
    // GLOBAL: CARMA2_SW 0x006545c8
    static tU32 last_physics_send_time = (tU32)-1;

    if (gPHIL_last_physics_tick > last_physics_send_time && gPHIL_last_physics_tick < last_physics_send_time + 80) {
        return 0;
    }
    last_physics_send_time = gPHIL_last_physics_tick;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00497900
// FUNCTION: CARMA2_SW 0x00494530
// FUNCTION: CARMA2_CHEAT 0x00497cb0
void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time) {
    tNet_message_chunk* contents;
    tCar_spec* car;
    tNon_car_spec* non_car;
    int i;
    int j;
    int index;
    tU8 size;
    union {
        tNet_message_chunk chunk;
        tU8 raw[0x240]; /* FIXME: size taken from the stack frame */
    } message;

    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (gNet_mode == eNet_mode_host) {
        contents = NetStartBroadcastContents(eNet_message_chunk_type_time_sync, 0);
        if (contents != NULL) {
            contents->time_sync.race_start_time = gRace_start;
            NetBroadcastContents(contents);
        }
    } else {
        contents = NetGetToHostContents(eNet_message_chunk_type_time_sync, 0);
        contents->time_sync.race_start_time = gRace_start;
    }
    if (gNet_mode == eNet_mode_host) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            car = gNet_players[i].car;
            if (car->disabled) {
                continue;
            }
            HostFillInCarContents(&message.chunk, i, pNext_frame_time);
            size = message.chunk.mechanics.header.size;
            for (j = 1; j < gNumber_of_net_players; j++) {
                if (i == j && car->collision_info->field_0x49c == car->collision_info->field_0x274) {
                    continue;
                }
                if (gNet_players[j].field_0xd0 != 0) {
                    for (index = 0; index < gNumber_of_net_players - 1; index++) {
                        if (gNet_players[index].ID == gNet_players[j].field_0xd0) {
                            break;
                        }
                    }
                } else {
                    index = j;
                }
                if (i == j || DistanceBetweenCars(gNet_players[i].car, gNet_players[index].car) < 40.0f) {
                    message.chunk.mechanics.header.size = size;
                    AddShapeStuffToMechanicsMessage(&message.chunk, &gNet_players[i], &gNet_players[index]);
                    message.chunk.mechanics.header.size += message.chunk.mechanics.header.size & 0x1;
                    contents = NetGetToPlayerContents(eNet_message_chunk_type_mechanics, message.chunk.mechanics.header.size, gNet_players[j].ID);
                    memcpy(contents, &message.chunk, message.chunk.mechanics.header.size);
                } else {
                    contents = NetGetToPlayerContents(eNet_message_chunk_type_simple_car_pos, 0, gNet_players[j].ID);
                    FillInSimpleCarPos(contents, i);
                }
            }
            if (car->time_to_recover != 0 && car->time_to_recover - 500 < pNext_frame_time) {
                contents = NetStartBroadcastContents(eNet_message_chunk_type_recover, 0);
                if (contents != NULL) {
                    contents->recover.player = gNet_players[i].ID;
                    contents->recover.time = car->time_to_recover;
                    NetBroadcastContents(contents);
                }
            }
        }
        for (i = 0; i < gNum_active_non_cars; i++) {
            non_car = gActive_non_car_list[i];
            if (non_car->flags & 0x100000) {
                if (non_car->driver == eDriver_detached_bit) {
                    SendDBNonCar(non_car, pNext_frame_time, NULL);
                } else if (non_car->driver == eDriver_split_car) {
                    SendSHNonCar(non_car, pNext_frame_time, NULL);
                } else {
                    SendNonCar(non_car, pNext_frame_time);
                }
            }
        }
    } else if (gNet_mode == eNet_mode_client) {
        ClientSendCarData(pNext_frame_time);
    }
}

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

// FUNCTION: CARMA2_HW 0x00498ab0
// FUNCTION: CARMA2_SW 0x004956e0
// FUNCTION: CARMA2_CHEAT 0x00498e60
void C2_HOOK_FASTCALL CopyMechanics(tCar_spec* pCar, tU8* pSrc) {
    int size;
    int i;
    tU8* pData;
    tPhysics_object* object;

    memcpy(&pCar->collision_info->field_0x260, pSrc, 0x34); /* FIXME: 0x34 is the block from field_0x260 up to field_0x294 */
    pData = pSrc + 0x34;
    size = GetNetworkDataSize(pData);
    memcpy(pCar->collision_info->field_0x294, pData, size);
    pData += size;
    if (pCar->collision_info->field_0x292 & 0x8) {
        for (i = 0; i < (int)REC2_ASIZE(pCar->collision_info->field_0x48c); i++) {
            pCar->collision_info->field_0x48c[i] = *(undefined4*)pData;
            pData += sizeof(undefined4);
        }
    } else {
        for (i = 0; i < (int)REC2_ASIZE(pCar->collision_info->field_0x48c); i++) {
            pCar->collision_info->field_0x48c[i] = 0;
        }
    }
    if (pCar->collision_info->field_0x292 & 0x1) {
        size = GetNetShapeDataLengthFromData(pData);
        memcpy(pCar->collision_info->shape_data, pData, size);
        pData += size;
    }
    if (pCar->collision_info->field_0x292 & 0x2) {
        pCar->collision_info->field_0x481 = *pData;
        pData += sizeof(tU8);
        pCar->collision_info->field_0x482 = *(tU16*)pData;
        pData += sizeof(tU16);
    }
    object = pCar->collision_info;
    if (object->field_0x292 & 0x4) {
        object->field_0x484 = *(tCompressed_vector4*)pData;
    }
}

// FUNCTION: CARMA2_HW 0x004983f0
// FUNCTION: CARMA2_SW 0x00495020
// FUNCTION: CARMA2_CHEAT 0x004987a0
void C2_HOOK_FASTCALL ReceivedShapeNumbers(tNet_message_chunk* pContents) {
    int i;
    tCar_spec* car;

    for (i = 0; i < pContents->shape_numbers.count; i++) {
        car = NetCarFromPlayerID(pContents->shape_numbers.shapes[i].player);
        if (car != NULL && pContents->shape_numbers.shapes[i].shape_number == car->car_crush_spec->field_0x24) {
            ClearShapeStatusFlag(car->car_crush_spec->network_stuff, pContents->shape_numbers.player_id);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00498460
// FUNCTION: CARMA2_SW 0x00495090
// FUNCTION: CARMA2_CHEAT 0x00498810
void C2_HOOK_FASTCALL FillInSimpleCarPos(tNet_message_chunk* pContents, int pPlayer_index) {
    tCar_spec* car;

    car = gNet_players[pPlayer_index].car;
    CompressVector3(&pContents->simple_car_pos.pos, &car->car_master_actor->t.t.translate.t, -500.0f, 500.0f);
    pContents->simple_car_pos.yaw = BrRadianToAngle(atan2(car->car_master_actor->t.t.mat.m[2][0], car->car_master_actor->t.t.mat.m[2][2]));
    pContents->simple_car_pos.player = gNet_players[pPlayer_index].ID;
    pContents->simple_car_pos.repair_time = car->repair_time;
}

// FUNCTION: CARMA2_HW 0x004984c0
// FUNCTION: CARMA2_SW 0x004950f0
// FUNCTION: CARMA2_CHEAT 0x00498870
void C2_HOOK_FASTCALL ReceivedSimpleCarPos(tNet_message_chunk* pContents) {
    int i;
    tCar_spec* car;
    br_matrix34* mat;
    br_scalar total_deflection;

    car = NULL;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pContents->simple_car_pos.player) {
            car = gNet_players[i].car;
            break;
        }
    }
    if (car != NULL) {
        car->active = 0;
        mat = &car->car_master_actor->t.t.mat;
        BrMatrix34RotateY(mat, pContents->simple_car_pos.yaw);
        ExpandVector3((br_vector3*)mat->m[3], &pContents->simple_car_pos.pos, -500.0f, 500.0f);
        BrMatrix34Copy(&car->collision_info->transform_matrix, mat);
        BrVector3Copy(&car->pos, (br_vector3*)car->collision_info->transform_matrix.m[3]);
        BrVector3Copy(&car->collision_info->pos, (br_vector3*)car->collision_info->transform_matrix.m[3]);
        if (car != NULL && car->driver >= eDriver_net_human && pContents->simple_car_pos.repair_time > car->repair_time) {
            if (pContents->simple_car_pos.repair_time - car->repair_time >= 100000) {
                TotallyRepairACar(car);
                car->repair_time = car->collision_info->field_0x27c;
            } else {
                RepairCar2(car, pContents->simple_car_pos.repair_time - car->repair_time, &total_deflection);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004985f0
// FUNCTION: CARMA2_SW 0x00495220
// FUNCTION: CARMA2_CHEAT 0x004989a0
void C2_HOOK_FASTCALL ReceivedMechanics(tNet_message_chunk* pContents) {
    int i;
    int j;
    tCar_spec* car;
    br_scalar total_deflection;

    car = NULL;
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pContents->mechanics.player) {
            car = gNet_players[i].car;
            break;
        }
    }
    if (car == NULL || car->collision_info->message_time > pContents->mechanics.time) {
        return;
    }
    if (car->disabled) {
        EnableCar(car);
        WriteHierarchyNetworkStuff(pContents->mechanics.data, car->collision_info, 2);
        car->collision_info->box_face_ref = gFace_num__car - 2;
        car->collision_info->message_time = pContents->mechanics.time;
        car->collision_info->field_0x261 = 0xff;
        for (j = 0; j < (int)REC2_ASIZE(car->collision_info->field_0x26c); j++) {
            car->collision_info->field_0x26c[j] = pContents->mechanics.d[j];
        }
        for (j = 0; j < (int)REC2_ASIZE(car->collision_info->field_0x280); j++) {
            car->collision_info->field_0x280[j] = pContents->mechanics.damage[j];
        }
        DisableCar(car);
        return;
    }
    if (gNet_mode == eNet_mode_host) {
        if (i == gThis_net_player_index) {
            return;
        }
        if (car->collision_info->field_0x49c > pContents->mechanics.cc_coll_time) {
            if (pContents->mechanics.cc_coll_time == 0) {
                car->collision_info->field_0x274 = 0;
                car->collision_info->field_0x261 = 0;
            }
            if (pContents->mechanics.shape_number == car->car_crush_spec->field_0x24) {
                ClearShapeStatusFlag(car->car_crush_spec->network_stuff, pContents->mechanics.player);
            }
        } else {
            CopyMechanics(car, (tU8*)pContents);
            if (pContents->mechanics.shape_number != car->car_crush_spec->field_0x24) {
                SetAllShapeStatusFlags(car->car_crush_spec->network_stuff);
                ClearShapeStatusFlag(car->car_crush_spec->network_stuff, pContents->mechanics.player);
                car->car_crush_spec->field_0x26 = pContents->mechanics.shape_number;
            }
        }
        car->power_up_levels[0] = pContents->mechanics.powerups[0];
        car->power_up_levels[1] = pContents->mechanics.powerups[1];
        car->power_up_levels[2] = pContents->mechanics.powerups[2];
        car->keys = pContents->mechanics.keys;
        if (pContents->mechanics.keys.joystick_acc >= 0) {
            car->joystick.acc = pContents->mechanics.keys.joystick_acc << 9;
        } else {
            car->joystick.acc = -1;
        }
        if (pContents->mechanics.keys.joystick_dec >= 0) {
            car->joystick.dec = pContents->mechanics.keys.joystick_dec << 9;
        } else {
            car->joystick.dec = -1;
        }
    } else if (gNet_mode == eNet_mode_client) {
        if (gCar_to_view_id == gNet_players[i].ID) {
            ReallyViewOpponent();
        }
        if (i == gThis_net_player_index) {
            car->car_crush_spec->field_0x26 = pContents->mechanics.shape_number;
            if (car->collision_info->field_0x49c < pContents->mechanics.cc_coll_time) {
                CopyMechanics(car, (tU8*)pContents);
            }
            return;
        }
        CopyMechanics(car, (tU8*)pContents);
        car->power_up_levels[0] = pContents->mechanics.powerups[0];
        car->power_up_levels[1] = pContents->mechanics.powerups[1];
        car->power_up_levels[2] = pContents->mechanics.powerups[2];
        car->keys = car->collision_info->field_0x270;
        if (car->collision_info->field_0x270.joystick_acc >= 0) {
            car->joystick.acc = car->collision_info->field_0x270.joystick_acc << 9;
        } else {
            car->joystick.acc = -1;
        }
        if (car->collision_info->field_0x270.joystick_dec >= 0) {
            car->joystick.dec = car->collision_info->field_0x270.joystick_dec << 9;
        } else {
            car->joystick.dec = -1;
        }
        if (car->active) {
            return;
        }
        WriteHierarchyNetworkStuff(pContents->mechanics.data, car->collision_info, 2);
        BrVector3Copy(&car->pos, (br_vector3*)car->collision_info->transform_matrix.m[3]);
        BrVector3Copy(&car->collision_info->pos, (br_vector3*)car->collision_info->transform_matrix.m[3]);
        if (car != NULL && car->driver >= eDriver_net_human && car->collision_info->field_0x27c > car->repair_time) {
            if (car->collision_info->field_0x27c - car->repair_time >= 100000) {
                TotallyRepairACar(car);
                car->repair_time = car->collision_info->field_0x27c;
            } else {
                RepairCar2(car, car->collision_info->field_0x27c - car->repair_time, &total_deflection);
            }
            for (j = 0; j < (int)REC2_ASIZE(car->damage_units); j++) {
                car->damage_units[j].damage_level = car->collision_info->field_0x280[j];
            }
            SetSmokeLastDamageLevel(car);
            StopCarSmoking(car);
        } else {
            for (j = 0; j < (int)REC2_ASIZE(car->damage_units); j++) {
                car->damage_units[j].damage_level = car->collision_info->field_0x280[j];
            }
            SortOutSmoke(car);
        }
        car->collision_info->box_face_ref = gFace_num__car - 2;
        if ((car->collision_info->field_0x292 & 0x1) && car->collision_info->field_0x290 != car->car_crush_spec->field_0x24) {
            if (car->collision_info->field_0x292 & 0x2) {
                CopyBendInMessage(car);
            } else if (car->car_crush_spec->field_0x144) {
                CompletelyUnBendCar(car);
            }
            if (car->collision_info->field_0x292 & 0x4) {
                CopySplitInMessage(car);
            } else if (car->car_crush_spec->field_0x4b8) {
                WeldCar(car);
            }
            DecodeNetShapeData(car, car->collision_info->shape_data);
        }
    }
}

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

// SignalToRepositionCar

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

// RepositionPlayer

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

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x0049a410
void C2_HOOK_FASTCALL CreateBoxes(void) {
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

// DrawScoreBoxes

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00499a00
// STUB: CARMA2_SW 0x004962a0
// STUB: CARMA2_CHEAT 0x00499db0
void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores) {
    NOT_IMPLEMENTED();
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

// EverybodysLost

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

// FarEnoughAway

// FUNCTION: CARMA2_HW 0x0049a990
// FUNCTION: CARMA2_SW 0x00496b10
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

// SelectRandomFox

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049abf0
// STUB: CARMA2_SW 0x00496d70
// STUB: CARMA2_CHEAT 0x0049afa0
void C2_HOOK_FASTCALL CalcPlayerScores(void) {
    NOT_IMPLEMENTED();
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


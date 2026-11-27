/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "30-opponent.h"

#include <c2_hooks.h>
#include "00-car.h"
#include "08-loading1.h"
#include "13-crush1.h"
#include "23-piping.h"
#include "26-oppoproc.h"
#include "36-trig.h"
#include "39-oil.h"
#include "41-utility.h"
#include "53-controls.h"
#include "56-aiworld.h"
#include "60-phil.h"
#include "globvars.h"
#include "globvrme.h"
#include "globvrpb.h"
#include "c2_stdio.h"
#include "platform.h"
#include "rec2_macros.h"
#include <math.h>
#include <stdarg.h>
#include <string.h>

#define CAR_SPEC_IS_ROZZER(CAR_SPEC) (VEHICLE_TYPE_FROM_ID((tU16)(CAR_SPEC)->car_ID) == eVehicle_rozzer)
#define CAR_SPEC_IS_OPPONENT(CAR_SPEC) (VEHICLE_TYPE_FROM_ID((tU16)(CAR_SPEC)->car_ID) == eVehicle_opponent)
#define GET_CAR_SPEED_FACTOR(CAR) (CAR_SPEC_IS_ROZZER(CAR) ? gCop_speed_factor : gOpponent_speed_factor)

// GLOBAL: CARMA2_HW 0x0065a3bc
// GLOBAL: CARMA2_SW 0x00654cd4
const char* gCop_name = "Faceless Cop";

// GLOBAL: CARMA2_HW 0x0065a3c0
// GLOBAL: CARMA2_SW 0x00654cd8
const char* gDrone_name = "Innocent Civilian";

// GLOBAL: CARMA2_HW 0x0065a3c4
// GLOBAL: CARMA2_SW 0x00654cdc
// GLOBAL: CARMA2_CHEAT 0x00660d50
int gBIG_APC_index = -1;

// GLOBAL: CARMA2_HW 0x0065a3c8
// GLOBAL: CARMA2_SW 0x00654ce0
// GLOBAL: CARMA2_CHEAT 0x00660d54
int gChallenger_index__opponent = -1;

// GLOBAL: CARMA2_HW 0x0065a3cc
// GLOBAL: CARMA2_SW 0x00654ce4
// GLOBAL: CARMA2_CHEAT 0x00660d58
float gOpponent_nastyness_frigger = 1.0f;

// GLOBAL: CARMA2_HW 0x0074a684
// GLOBAL: CARMA2_SW 0x00741144
// GLOBAL: CARMA2_CHEAT 0x00751d24
int gMinTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x0074a688
// GLOBAL: CARMA2_SW 0x00741148
// GLOBAL: CARMA2_CHEAT 0x00751d28
int gMaxTimeOpponentRepair;

// GLOBAL: CARMA2_HW 0x00691754
// GLOBAL: CARMA2_SW 0x0068b7ac
// GLOBAL: CARMA2_CHEAT 0x00699104
tU32 gNext_grudge_reduction;

// GLOBAL: CARMA2_HW 0x00691724
// GLOBAL: CARMA2_SW 0x0068b77c
// GLOBAL: CARMA2_CHEAT 0x006990d0
int gGrudge_reduction_per_period;

// GLOBAL: CARMA2_HW 0x00691760
// GLOBAL: CARMA2_SW 0x0068b7b8
// GLOBAL: CARMA2_CHEAT 0x00699110
br_scalar gMinimum_yness_before_knackerisation;

// GLOBAL: CARMA2_HW 0x00691728
// GLOBAL: CARMA2_SW 0x0068b780
// GLOBAL: CARMA2_CHEAT 0x006990d4
tU32 gAcme_frame_count;

// GLOBAL: CARMA2_HW 0x0069173c
// GLOBAL: CARMA2_SW 0x0068b794
// GLOBAL: CARMA2_CHEAT 0x006990e8
int gActive_car_list_rebuild_required;

// GLOBAL: CARMA2_HW 0x00691744
// GLOBAL: CARMA2_SW 0x0068b79c
// GLOBAL: CARMA2_CHEAT 0x006990f0
int gNumber_of_cops_before_faffage;

// GLOBAL: CARMA2_HW 0x00691758
// GLOBAL: CARMA2_SW 0x0068b7b0
int gNum_of_opponents_completing_race;

// GLOBAL: CARMA2_HW 0x00691740
// GLOBAL: CARMA2_SW 0x0068b798
int gNum_of_opponents_getting_near;

// GLOBAL: CARMA2_HW 0x00691718
// GLOBAL: CARMA2_SW 0x0068b770
int gNum_of_opponents_pursuing;

// GLOBAL: CARMA2_HW 0x0069175c
// GLOBAL: CARMA2_SW 0x0068b7b4
// GLOBAL: CARMA2_CHEAT 0x0069910c
tU8* gBit_per_node;

// GLOBAL: CARMA2_HW 0x00691764
// GLOBAL: CARMA2_SW 0x0068b7bc
float gIn_view_distance;

// GLOBAL: CARMA2_HW 0x0069176c
// GLOBAL: CARMA2_SW 0x0068b7c4
// GLOBAL: CARMA2_CHEAT 0x0069911c
int gRoute_search_calls;

// GLOBAL: CARMA2_HW 0x0069174c
// GLOBAL: CARMA2_SW 0x0068b7a4
// GLOBAL: CARMA2_CHEAT 0x006990f8
int gRoute_search_last_target_count;

// GLOBAL: CARMA2_HW 0x0069172c
// GLOBAL: CARMA2_SW 0x0068b784
// GLOBAL: CARMA2_CHEAT 0x006990d8
int gRoute_search_target_count;

// GLOBAL: CARMA2_HW 0x0069171c
// GLOBAL: CARMA2_SW 0x0068b774
// GLOBAL: CARMA2_CHEAT 0x00698fc4
float gShortest_route_length;

// GLOBAL: CARMA2_HW 0x00691720
// GLOBAL: CARMA2_SW 0x0068b778
// GLOBAL: CARMA2_CHEAT 0x00698fc8
int gStart_jumped;

// GLOBAL: CARMA2_HW 0x00691750
// GLOBAL: CARMA2_SW 0x0068b7a8
// GLOBAL: CARMA2_CHEAT 0x006990fc
int gAcknowledged_start;

// GLOBAL: CARMA2_HW 0x0074c7c0
// GLOBAL: CARMA2_SW 0x00742e80
// GLOBAL: CARMA2_CHEAT 0x00753e60
tCar_spec* gActive_car_list[139];

// GLOBAL: CARMA2_HW 0x0074c9ec
// GLOBAL: CARMA2_SW 0x007430ac
// GLOBAL: CARMA2_CHEAT 0x0075408c
int gNum_active_cars;

// GLOBAL: CARMA2_HW 0x00691770
// GLOBAL: CARMA2_SW 0x0068b7c8
// GLOBAL: CARMA2_CHEAT 0x00699150
int gRoute_search_attempts;

#ifdef REC2_CHEAT
// GLOBAL: CARMA2_CHEAT 0x00698fd0
char gOppo_path_filename[256];
#endif

// GLOBAL: CARMA2_HW 0x00691778
// GLOBAL: CARMA2_SW 0x0068b7d0
// GLOBAL: CARMA2_CHEAT 0x00699158
int gRoute_search_max_calls;

// GLOBAL: CARMA2_HW 0x00691774
// GLOBAL: CARMA2_SW 0x0068b7cc
// GLOBAL: CARMA2_CHEAT 0x00699154
int gRoute_search_total_calls;

// GLOBAL: CARMA2_HW 0x00691734
// GLOBAL: CARMA2_SW 0x0068b78c
float gDefinite_no_cop_pursuit_speed;

// GLOBAL: CARMA2_HW 0x00691738
// GLOBAL: CARMA2_SW 0x0068b790
// GLOBAL: CARMA2_CHEAT 0x006990e4
int gFirst_frame;

// GLOBAL: CARMA2_HW 0x00691748
// GLOBAL: CARMA2_SW 0x0068b7a0
float gCop_pursuit_speed_percentage_multiplier;

// GLOBAL: CARMA2_HW 0x00691768
// GLOBAL: CARMA2_SW 0x0068b7c0
float gHead_on_cos_value;

#ifdef REC2_CHEAT
// GLOBAL: CARMA2_CHEAT 0x00698fb8
tS16 gMobile_section;

// GLOBAL: CARMA2_CHEAT 0x00699130
int gAlready_elasticating;
#endif

#ifdef REC2_CHEAT
// GLOBAL: CARMA2_CHEAT 0x00699100
int gProcessing_opponents;

// GLOBAL: CARMA2_CHEAT 0x00699148
tU32 gNext_elastication;

// GLOBAL: CARMA2_CHEAT 0x0069914c
tU32 gNext_write_during_elastication;
#endif

// FUNCTION: CARMA2_HW 0x004a8160
// FUNCTION: CARMA2_SW 0x004a42d0
// FUNCTION: CARMA2_CHEAT 0x004a9470
void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...) {

}

static void C2_HOOK_FASTCALL ProcessCompleteRace(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
static void C2_HOOK_FASTCALL ProcessRunAway(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
static void C2_HOOK_FASTCALL ProcessFrozen(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessReturnToStart(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);
void C2_HOOK_FASTCALL ProcessLevitate(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand);

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004aa900
// FUNCTION: CARMA2_SW 0x004a6a70
// FUNCTION: CARMA2_CHEAT 0x004abdf0
void C2_HOOK_FASTCALL ProcessCurrentObjective(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pOpponent_spec->current_objective) {
    case eOOT_complete_race:
        ProcessCompleteRace(pOpponent_spec, pCommand);
        break;
    case eOOT_pursue_and_twat:
        ProcessPursueAndTwat(pOpponent_spec, pCommand);
        break;
    case eOOT_run_away:
        ProcessRunAway(pOpponent_spec, pCommand);
        break;
    case eOOT_get_near_player:
        ProcessGetNearPlayer(pOpponent_spec, pCommand);
        break;
    case eOOT_levitate:
        ProcessLevitate(pOpponent_spec, pCommand);
        break;
    case eOOT_knackered_and_freewheeling:
        memset(&pOpponent_spec->car_spec->keys, 0, sizeof(pOpponent_spec->car_spec->keys));
        pOpponent_spec->car_spec->acc_force = 0.0f;
        pOpponent_spec->car_spec->brake_force = 0.0f;
        pOpponent_spec->car_spec->curvature = 0.0f;
        break;
    case eOOT_frozen:
        ProcessFrozen(pOpponent_spec, pCommand);
        break;
    case eOOT_wait_for_some_hapless_sod:
        ProcessWaitForSomeHaplessSod(pOpponent_spec, pCommand);
        break;
    case eOOT_return_to_start:
        ProcessReturnToStart(pOpponent_spec, pCommand);
        break;
    default:
        break;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline tS16 C2_HOOK_FASTCALL ReallocExtraPathNodes(int pHow_many_then) {
    tPath_node* new_nodes;
    tS16 first_new_node;

    first_new_node = -1;
    if (pHow_many_then != 0) {
        first_new_node = gProgram_state.AI_vehicles.number_of_path_nodes;
        new_nodes = BrMemAllocate(sizeof(tPath_node) * (pHow_many_then + gProgram_state.AI_vehicles.number_of_path_nodes), kMem_oppo_new_nodes);
        memcpy(new_nodes, gProgram_state.AI_vehicles.path_nodes, sizeof(tPath_node) * gProgram_state.AI_vehicles.number_of_path_nodes);
        if (gProgram_state.AI_vehicles.path_nodes != NULL) {
            BrMemFree(gProgram_state.AI_vehicles.path_nodes);
        }
        gProgram_state.AI_vehicles.path_nodes = new_nodes;
        gProgram_state.AI_vehicles.number_of_path_nodes += pHow_many_then;
    }
    DoNotDprintf("ReallocExtraPathNodes(): Allocated %d bytes for %d path nodes",
        sizeof(tPath_node) * (pHow_many_then + gProgram_state.AI_vehicles.number_of_path_nodes),
        pHow_many_then);
    return first_new_node;
}

inline tS16 C2_HOOK_FASTCALL ReallocExtraPathSections(int pHow_many_then) {
    tPath_section* new_sections;
    tS16 first_new_section;

    first_new_section = -1;
    if (pHow_many_then != 0) {
        first_new_section = gProgram_state.AI_vehicles.number_of_path_sections;
        new_sections = BrMemAllocate(sizeof(tPath_section) * (pHow_many_then + gProgram_state.AI_vehicles.number_of_path_sections), kMem_oppo_new_sections);
        memcpy(new_sections, gProgram_state.AI_vehicles.path_sections, sizeof(tPath_section) * gProgram_state.AI_vehicles.number_of_path_sections);
        if (gProgram_state.AI_vehicles.path_sections != NULL) {
            BrMemFree(gProgram_state.AI_vehicles.path_sections);
        }
        gProgram_state.AI_vehicles.path_sections = new_sections;
        gProgram_state.AI_vehicles.number_of_path_sections += pHow_many_then;
    }
    DoNotDprintf("ReallocExtraPathSections(): Allocated %d bytes for %d path sections",
        sizeof(tPath_section) * (pHow_many_then + gProgram_state.AI_vehicles.number_of_path_sections),
        pHow_many_then);
    return first_new_section;
}

// FUNCTION: CARMA2_HW 0x004aa400
// FUNCTION: CARMA2_SW 0x004a6570
// FUNCTION: CARMA2_CHEAT 0x004ab8f0
inline int C2_HOOK_FASTCALL PointVisibleFromHere(br_vector3* pFrom, br_vector3* pTo) {
    br_vector3 from;
    br_vector3 dir;
    br_vector3 norm;
    br_scalar t;
    br_material* material;

    BrVector3Sub(&dir, pTo, pFrom);
    BrVector3Copy(&from, pFrom);
    from.v[1] += 0.15f;
    dir.v[1] += 0.15f;
    FindFace(&from, &dir, &norm, &t, &material);
    if (t > 1.0f) {
        return 1;
    } else {
        return 0;
    }
}

inline void C2_HOOK_FASTCALL WeightedFindNearestNodeAndSection(tCar_spec* pCar, const br_vector3* pActor_coords, int* pNearest_node_section_no, int* pNearest_node, br_scalar* pT, br_scalar pWeight) {
    int section_no;
    int no_sections;
    int nearest_section;
    br_scalar nearest_section_distance_squared;
    br_scalar closest_distance_squared;
    br_scalar the_distance_squared;
    br_scalar t;
    br_scalar length_squared_a;
    br_vector3 a;
    br_vector3 p;
    br_vector3 wank;
    br_vector3* start;
    br_vector3* finish;

    *pNearest_node_section_no = -1;
    *pNearest_node = -1;
    nearest_section_distance_squared = BR_SCALAR_MAX;
    closest_distance_squared = BR_SCALAR_MAX;
    if (pCar != NULL) {
        no_sections = pCar->my_trail.number_of_nodes - 1;
    } else {
        no_sections = gProgram_state.AI_vehicles.number_of_path_sections;
    }
    for (section_no = 0; section_no < no_sections; section_no++) {
        if (pCar != NULL) {
            start = &pCar->my_trail.trail_nodes[section_no];
            finish = &pCar->my_trail.trail_nodes[section_no + 1];
        } else {
            start = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]].pos;
            finish = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]].pos;
        }
#ifdef REC2_CHEAT
        if (gAlready_elasticating && section_no == gMobile_section) {
            continue;
        }
#endif
        BrVector3Sub(&a, finish, start);
        a.v[1] *= pWeight;
        BrVector3Sub(&p, pActor_coords, start);
        p.v[1] *= pWeight;
        BrVector3Sub(&wank, &p, &a);
        the_distance_squared = BrVector3LengthSquared(&wank);
        if (the_distance_squared < closest_distance_squared) {
            closest_distance_squared = the_distance_squared;
            if (pCar != NULL) {
                *pNearest_node = section_no;
            } else {
                *pNearest_node = (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1];
            }
            nearest_section = section_no;
        }
        the_distance_squared = BrVector3LengthSquared(&p);
        if (the_distance_squared < closest_distance_squared) {
            closest_distance_squared = the_distance_squared;
            if (pCar != NULL) {
                *pNearest_node = section_no + 1;
            } else {
                *pNearest_node = (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0];
            }
            nearest_section = section_no;
        }
        if ((length_squared_a = BrVector3LengthSquared(&a)) >= 0.0001f) {
            t = BrVector3Dot(&p, &a) / length_squared_a;
            if (t >= 0.0f && t <= 1.0f) {
                BrVector3Scale(&wank, &a, t);
                BrVector3Sub(&wank, &p, &wank);
                the_distance_squared = BrVector3LengthSquared(&wank);
                if (the_distance_squared < nearest_section_distance_squared) {
                    *pNearest_node_section_no = section_no;
                    *pT = t;
                    nearest_section_distance_squared = the_distance_squared;
                }
            }
        }
    }
    if (closest_distance_squared < nearest_section_distance_squared) {
        *pNearest_node_section_no = nearest_section;
    } else {
        *pNearest_node = -1;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004a82d0
// FUNCTION: CARMA2_SW 0x004a4440
// FUNCTION: CARMA2_CHEAT 0x004a95e0
tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {
    int nearest_node_section_no;
    int nearest_node;
    br_scalar t;
    br_vector3 delta;
    br_vector3 tv;

    WeightedFindNearestNodeAndSection(pCar, pActor_coords, &nearest_node_section_no, &nearest_node, &t, 2.0f);
    if (pCar != NULL) {
        BrVector3Sub(&delta, &pCar->my_trail.trail_nodes[nearest_node_section_no + 1], &pCar->my_trail.trail_nodes[nearest_node_section_no]);
    } else {
        BrVector3Sub(&delta,
            &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_node_section_no].node_indices[1]].pos,
            &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_node_section_no].node_indices[0]].pos);
    }
    if (nearest_node >= 0) {
        BrVector3Copy(pIntersect, pCar != NULL ? &pCar->my_trail.trail_nodes[nearest_node] : &gProgram_state.AI_vehicles.path_nodes[nearest_node].pos);
    } else {
        BrVector3Scale(pIntersect, &delta, t);
        BrVector3Accumulate(pIntersect, &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[nearest_node_section_no].node_indices[0]].pos);
    }
    BrVector3NormaliseQuick(pPath_direction, &delta);
    BrVector3Sub(&tv, pActor_coords, pIntersect);
    *pDistance = BrVector3Length(&tv);
    if (pCar != NULL) {
        nearest_node_section_no += 15000;
    }
    return nearest_node_section_no;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

tS16 C2_HOOK_FASTCALL FindNearestPathSection(const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance) {

    return FindNearestGeneralSection(NULL, pActor_coords, pPath_direction, pIntersect, pDistance);
}

inline void C2_HOOK_FASTCALL DeadStopCar(tCar_spec* pCar_spec) {

    pCar_spec->acc_force = 0.0f;
    pCar_spec->brake_force = 0.0f;
    pCar_spec->curvature = 0.0f;
    pCar_spec->gear = 0;
    pCar_spec->revs = 0.0f;
    BrVector3Set(&pCar_spec->collision_info->omega, 0.0f, 0.0f, 0.0f);
    BrVector3Set(&pCar_spec->collision_info->v, 0.0f, 0.0f, 0.0f);
}

// FUNCTION: CARMA2_HW 0x004a7960
// FUNCTION: CARMA2_SW 0x004a3ad0
// FUNCTION: CARMA2_CHEAT 0x004a8c70
void C2_HOOK_FASTCALL TurnOpponentPhysicsOn(tOpponent_spec* pOpponent_spec) {
    pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
    PipeSingleOppoRenderage(pOpponent_spec, 1);
    if (!pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 1;
        gActive_car_list_rebuild_required = 1;
    }
}

// FUNCTION: CARMA2_HW 0x004a79a0
// FUNCTION: CARMA2_SW 0x004a3b10
// FUNCTION: CARMA2_CHEAT 0x004a8cb0
inline void C2_HOOK_FASTCALL TurnOpponentPhysicsOff(tOpponent_spec* pOpponent_spec) {

    DeadStopCar(pOpponent_spec->car_spec);
    if (pOpponent_spec->physics_me) {
        pOpponent_spec->physics_me = 0;
        pOpponent_spec->car_spec->car_master_actor->render_style = BR_RSTYLE_NONE;
        PipeSingleOppoRenderage(pOpponent_spec, 0);
        gActive_car_list_rebuild_required = 1;
    }
}

// STUB: CARMA2_HW 0x004a7a20
// STUB: CARMA2_SW 0x004a3b90
// STUB: CARMA2_CHEAT 0x004a8d30
void C2_HOOK_FASTCALL ApplyOppoRenderage2(tOpponent_spec* pOpponent_spec, int pRender) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004aad70
// FUNCTION: CARMA2_SW 0x004a6ee0
// FUNCTION: CARMA2_CHEAT 0x004ac260
void C2_HOOK_CDECL NewObjective(tOpponent_spec* pOpponent_spec, tOpponent_objective_type pObjective_type, ...) {
    va_list marker;

    if (pOpponent_spec->current_objective != eOOT_none) {
        ProcessCurrentObjective(pOpponent_spec, ePOC_die);
    }
    pOpponent_spec->current_objective = pObjective_type;
    pOpponent_spec->time_this_objective_started = gTime_stamp_for_this_munging;
    pOpponent_spec->time_for_this_objective_to_finish = gTime_stamp_for_this_munging + IRandomBetween(20, 60) * 1000;
    if (pObjective_type == eOOT_pursue_and_twat) {
        pOpponent_spec->time_for_this_objective_to_finish += 90000;
    }
    va_start(marker, pObjective_type);
    switch (pObjective_type) {
    case eOOT_complete_race:
        gNum_of_opponents_completing_race++;
        break;
    case eOOT_get_near_player:
        gNum_of_opponents_getting_near++;
        break;
    case eOOT_pursue_and_twat:
        pOpponent_spec->pursue_car_data.pursuee = va_arg(marker, tCar_spec*);
        gNum_of_opponents_pursuing++;
        break;
    default:
        break;
    }
    DoNotDprintf("%s: NewObjective() - type %d", pOpponent_spec->car_spec->driver_name, pObjective_type);
    ProcessCurrentObjective(pOpponent_spec, ePOC_start);
    va_end(marker);
}

// FUNCTION: CARMA2_HW 0x004aae20
// FUNCTION: CARMA2_SW 0x004a6f90
void C2_HOOK_FASTCALL CalcRaceRoute(tOpponent_spec* pOpponent_spec) {
    tS16 section_no;
    tS16 section_no_index;
    tS16 node_no;
    tS16 race_section_count;
    tS16 normal_section_ok_direction_count;
    tS16 normal_section_wrong_direction_count;
    tS16 temp_section_array[REC2_ASIZE(gProgram_state.AI_vehicles.path_nodes[0].sections)];
    br_scalar distance;
    br_vector3 direction_v;
    br_vector3 intersect;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);
    C2_HOOK_BUG_ON(REC2_ASIZE(temp_section_array) != 8);

    if (pOpponent_spec->nnext_sections >= (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
        DoNotDprintf("%s: CalcRaceRoute() - Pissing off 'cos projected route full up", pOpponent_spec->car_spec->driver_name);
        return;
    }
    if (pOpponent_spec->nnext_sections == 0) {
        DoNotDprintf("%s: CalcRaceRoute() - Projected route empty; starting from nearest section", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->complete_race_data.finished_calcing_race_route = 0;
        pOpponent_spec->complete_race_data.found_race_section = 0;
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (section_no < 0) {
            return;
        }
        AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
        if (gProgram_state.AI_vehicles.path_sections[section_no].type == ePST_race_path) {
            pOpponent_spec->complete_race_data.found_race_section = 1;
        }
    }
    while (pOpponent_spec->nnext_sections < (int)REC2_ASIZE(pOpponent_spec->next_sections) && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
        node_no = gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no].node_indices[pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction];
        race_section_count = 0;
        normal_section_ok_direction_count = 0;
        normal_section_wrong_direction_count = 0;
        for (section_no_index = 0; section_no_index < gProgram_state.AI_vehicles.path_nodes[node_no].number_of_sections; section_no_index++) {
            section_no = gProgram_state.AI_vehicles.path_nodes[node_no].sections[section_no_index];
            if (section_no != pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no) {
                if (gProgram_state.AI_vehicles.path_sections[section_no].type == ePST_race_path && node_no == (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]) {
                    pOpponent_spec->complete_race_data.found_race_section = 1;
                    temp_section_array[race_section_count] = section_no;
                    race_section_count++;
                } else if (race_section_count == 0 && node_no == (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]) {
                    temp_section_array[normal_section_ok_direction_count] = section_no;
                    normal_section_ok_direction_count++;
                } else if (race_section_count == 0 && normal_section_ok_direction_count == 0 && (!gProgram_state.AI_vehicles.path_sections[section_no].one_way || node_no != (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1])) {
                    temp_section_array[normal_section_wrong_direction_count] = section_no;
                    normal_section_wrong_direction_count++;
                }
            }
        }
        if (race_section_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, race_section_count - 1)], 1);
        } else if (normal_section_ok_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_ok_direction_count - 1)], 1);
        } else if (normal_section_wrong_direction_count != 0) {
            AddToOpponentsProjectedRoute(pOpponent_spec, temp_section_array[IRandomBetween(0, normal_section_wrong_direction_count - 1)], 0);
        } else if (pOpponent_spec->complete_race_data.found_race_section) {
            pOpponent_spec->complete_race_data.finished_calcing_race_route = 1;
        } else {
            AddToOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no, pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction == 0);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a93c0
// STUB: CARMA2_SW 0x004a5530
// STUB: CARMA2_CHEAT 0x004aa6e0
void C2_HOOK_FASTCALL TopUpRandomRoute(tOpponent_spec* pOpponent_spec, int pSections_to_add) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a9550
// FUNCTION: CARMA2_SW 0x004a56c0
// FUNCTION: CARMA2_CHEAT 0x004aa870
void C2_HOOK_FASTCALL CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(tOpponent_spec* pOpponent_spec) {

    BrVector3Sub(&pOpponent_spec->player_to_oppo_v, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
    pOpponent_spec->player_to_oppo_d = BrVector3Length(&pOpponent_spec->player_to_oppo_v);
    if (pOpponent_spec->player_to_oppo_d < gIn_view_distance) {
        pOpponent_spec->last_in_view = gTime_stamp_for_this_munging;
    }
}

// FUNCTION: CARMA2_HW 0x004ac180
// FUNCTION: CARMA2_SW 0x004a82f0
// FUNCTION: CARMA2_CHEAT 0x004ad670
int C2_HOOK_FASTCALL SearchForSection(tRoute_section* pRoute, tRoute_section* pBest_route, int* pBest_count, tS16 pTarget, int pDepth, br_scalar pLength, tOpponent_spec* pOpponent_spec) {
    char prefix[32];
    const tPath_node* path_node;
    tS16 node;
    tS16 next_node;
    tS16 section;
    tS16 i;
    int direction;
    br_scalar length;

    C2_HOOK_BUG_ON(sizeof(tRoute_section) != 4);

    DoNotDprintf("%sSearchForSection() - START: depth %d", prefix, pDepth);
    gRoute_search_calls++;
    if (pDepth == 1) {
        DoNotDprintf("%sSearchForSection() - First time called", prefix);
        memset(gBit_per_node, 0, (gProgram_state.AI_vehicles.number_of_path_nodes + 7) / 8);
        gShortest_route_length = BR_SCALAR_MAX;
        gRoute_search_target_count = 0;
        *pBest_count = 0;
    }
    if (pDepth >= 10) {
        DoNotDprintf("%sSearchForSection() - *** Reached max depth ***", prefix);
        return 0;
    }
    node = (tS16)gProgram_state.AI_vehicles.path_sections[pRoute[pDepth - 1].section_no].node_indices[pRoute[pDepth - 1].direction];
    path_node = &gProgram_state.AI_vehicles.path_nodes[node];
    gBit_per_node[node / 8] |= 1 << (node % 8);
    DoNotDprintf("%sSearchForSection() - node #%d has %d sections", prefix, node, path_node->number_of_sections);
    for (i = 0; i < path_node->number_of_sections; i++) {
        section = (tS16)path_node->sections[i];
        direction = node != (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[1];
        DoNotDprintf("%sSearchForSection() - node's section #%d is #%d", prefix, i, section);
        next_node = (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[direction];
        if (gBit_per_node[next_node / 8] & (1 << (next_node % 8))) {
            DoNotDprintf("%sSearchForSection() - *** node #%d already visited ***", prefix, next_node);
            continue;
        }
        if (gProgram_state.AI_vehicles.path_sections[section].one_way && node == (tS16)gProgram_state.AI_vehicles.path_sections[section].node_indices[1]) {
            DoNotDprintf("%sSearchForSection() - *** no entry ***", prefix);
            continue;
        }
        if (!pOpponent_spec->cheating && gProgram_state.AI_vehicles.path_sections[section].type == ePST_cheat_only) {
            continue;
        }
        DoNotDprintf("%sSearchForSection() - adding section #%d to list", prefix, section);
        pRoute[pDepth].section_no = section;
        pRoute[pDepth].direction = direction;
        length = gProgram_state.AI_vehicles.path_sections[section].length + pLength;
        if (section == pTarget && length < gShortest_route_length) {
            gShortest_route_length = length;
            *pBest_count = pDepth + 1;
            memcpy(pBest_route, pRoute, (pDepth + 1) * sizeof(tRoute_section));
            gRoute_search_last_target_count = gRoute_search_target_count + 1;
            gRoute_search_target_count++;
            if (gRoute_search_target_count >= 2) {
                DoNotDprintf("%sSearchForSection() - Found final target", prefix);
                return 1;
            }
            DoNotDprintf("%sSearchForSection() - Found a target", prefix);
            DoNotDprintf("%sSearchForSection() - Returning to depth %d", prefix, pDepth - 1);
            gBit_per_node[node / 8] &= ~(1 << (node % 8));
            return 0;
        }
        if (pDepth < 9 && SearchForSection(pRoute, pBest_route, pBest_count, pTarget, pDepth + 1, length, pOpponent_spec)) {
            return 1;
        }
    }
    gBit_per_node[node / 8] &= ~(1 << (node % 8));
    DoNotDprintf("%sSearchForSection() - Returning to depth %d", prefix, pDepth - 1);
    return 0;
}

// UpdatePlayersSection

// AllowForDecel

// FUNCTION: CARMA2_HW 0x004ab100
// FUNCTION: CARMA2_SW 0x004a7270
// FUNCTION: CARMA2_CHEAT 0x004ac5f0
int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);

    if (pOpponent_spec->nnext_sections > pPlaces) {
        for (i = 0; i < (int)REC2_ASIZE(pOpponent_spec->next_sections) - pPlaces; i++) {
            pOpponent_spec->next_sections[i].section_no = pOpponent_spec->next_sections[pPlaces + i].section_no;
            pOpponent_spec->next_sections[i].direction = pOpponent_spec->next_sections[pPlaces + i].direction;
        }
        pOpponent_spec->nnext_sections -= pPlaces;
        return 1;
    } else {
        return 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004ac9e0
// STUB: CARMA2_SW 0x004a8b50
// STUB: CARMA2_CHEAT 0x004aded0
void C2_HOOK_FASTCALL NewCalcGetNearPlayerRoute(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, int pForce) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004abf60
// FUNCTION: CARMA2_SW 0x004a80d0
// FUNCTION: CARMA2_CHEAT 0x004ad450
void C2_HOOK_FASTCALL CalcReturnToStartPointRoute(tOpponent_spec* pOpponent_spec) {
    br_vector3 direction;
    br_vector3 intersect;
    br_scalar distance;
    tRoute_section route[10];
    tRoute_section best[10];
    tS16 section;
    int best_count;
    int count;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);
    C2_HOOK_BUG_ON(sizeof(tRoute_section) != 4);

    ClearOpponentsProjectedRoute(pOpponent_spec);
    section = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction, &intersect, &distance);
    BrVector3Normalise(&direction, &direction);
    if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction) > 0.f) {
        AddToOpponentsProjectedRoute(pOpponent_spec, section, 1);
    } else {
        AddToOpponentsProjectedRoute(pOpponent_spec, section, 0);
    }
    route[0].section_no = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].section_no;
    route[0].direction = pOpponent_spec->next_sections[pOpponent_spec->nnext_sections - 1].direction;
    gRoute_search_calls = 0;
    gRoute_search_attempts++;
    SearchForSection(route, best, &best_count, pOpponent_spec->return_to_start_data.section_no, 1, 0.f, pOpponent_spec);
    gRoute_search_total_calls += gRoute_search_calls;
    if (gRoute_search_calls > gRoute_search_max_calls) {
        gRoute_search_max_calls = gRoute_search_calls;
    }
    DoNotDprintf(">>>SearchForSection() - max %d, avg %.1f", gRoute_search_max_calls, (double)gRoute_search_total_calls / (double)gRoute_search_attempts);
    if (best_count > 1) {
        count = MIN(best_count - 1, (int)REC2_ASIZE(pOpponent_spec->next_sections) - pOpponent_spec->nnext_sections);
        memcpy(&pOpponent_spec->next_sections[pOpponent_spec->nnext_sections], &best[1], count * sizeof(tRoute_section));
        pOpponent_spec->nnext_sections += count;
        TopUpRandomRoute(pOpponent_spec, 2);
    } else {
        DoNotDprintf("%s: CalcReturnToStartPointRoute() - SearchForSection() produced bugger all", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->nnext_sections <= (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
            TopUpRandomRoute(pOpponent_spec, (int)REC2_ASIZE(pOpponent_spec->next_sections) + 2 - pOpponent_spec->nnext_sections);
        }
    }
}

void C2_HOOK_FASTCALL ClearOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->nnext_sections = 0;
}

// FUNCTION: CARMA2_HW 0x004ab150
// FUNCTION: CARMA2_SW 0x004a72c0
// FUNCTION: CARMA2_CHEAT 0x004ac640
int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection) {

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);

    if (pOpponent_spec->nnext_sections < (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].section_no = pSection_no;
        pOpponent_spec->next_sections[pOpponent_spec->nnext_sections].direction = pDirection;
        pOpponent_spec->nnext_sections++;
        return 1;
    } else {
        return 0;
    }
}

void C2_HOOK_FASTCALL StunTheBugger(tOpponent_spec* pOpponent, int pMilliseconds) {

    pOpponent->car_spec->acc_force = 0.0f;
    pOpponent->car_spec->brake_force = 0.0f;
    pOpponent->car_spec->curvature = 0.0f;
    pOpponent->stun_time_ends = MAX(pOpponent->stun_time_ends, gTime_stamp_for_this_munging + pMilliseconds);
}

void C2_HOOK_FASTCALL UnStunTheBugger(tOpponent_spec* pOpponent) {

    pOpponent->stun_time_ends = 0;
}

static inline void C2_HOOK_FASTCALL ProcessCompleteRace(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    int res;

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf("%s: ProcessCompleteRace() - new objective started", pOpponent_spec->car_spec->driver_name);
        ClearOpponentsProjectedRoute(pOpponent_spec);
        CalcRaceRoute(pOpponent_spec);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        break;
    case ePOC_run:
        if (pOpponent_spec->follow_path_data.section_no > 20000) {
            ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path_data.section_no - 20000);
            pOpponent_spec->follow_path_data.section_no = 20000;
        }
        if (pOpponent_spec->nnext_sections <= (int)REC2_ASIZE(pOpponent_spec->next_sections) - 2 && !pOpponent_spec->complete_race_data.finished_calcing_race_route) {
            CalcRaceRoute(pOpponent_spec);
        }
        if (pOpponent_spec->nnext_sections == 0 || (res = ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0)) == eFPR_end_of_path) {
            DoNotDprintf("%s: Giving up following race path because ran out of race path", pOpponent_spec->car_spec->driver_name);
            NewObjective(pOpponent_spec, eOOT_get_near_player);
        }
        if (res != eFPR_OK) {
            if (res == eFPR_given_up) {
                ClearOpponentsProjectedRoute(pOpponent_spec);
                CalcRaceRoute(pOpponent_spec);
                ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
            } else {
                ObjectiveComplete(pOpponent_spec);
            }
        }
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004aa490
// FUNCTION: CARMA2_SW 0x004a6600
// FUNCTION: CARMA2_CHEAT 0x004ab980
void C2_HOOK_FASTCALL StartRecordingTrail(tCar_spec* pPursuee) {

    if (pPursuee->no_of_processes_recording_my_trail == 0) {
        DoNotDprintf("StartRecordingTrail - starting from scratch");
        pPursuee->no_of_processes_recording_my_trail = 1;
        pPursuee->my_trail.nodes_shifted_this_frame = 0;
        pPursuee->my_trail.has_deviated_recently = 0;
        pPursuee->my_trail.number_of_nodes = 2;
        pPursuee->my_trail.time_of_next_recording = gTime_stamp_for_this_munging + 500;
        BrVector3Copy(&pPursuee->my_trail.base_heading, &pPursuee->direction);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[0], &pPursuee->car_master_actor->t.t.translate.t);
        BrVector3Copy(&pPursuee->my_trail.trail_nodes[1], &pPursuee->car_master_actor->t.t.translate.t);
        pPursuee->my_trail.trail_nodes[0].v[2] += 0.2f;
    } else {
        DoNotDprintf("StartRecordingTrail - another pursuer attaching");
        pPursuee->no_of_processes_recording_my_trail++;
    }
}

inline void C2_HOOK_FASTCALL RecordNextTrailNode(tCar_spec* pPursuee) {
    tPursuee_trail* trail;
    br_vector3 start1;
    br_vector3 finish1;
    br_vector3 start2;
    br_vector3 finish2;
    br_vector3 offset_v;
    br_vector3 car_to_last_point_v;
    br_scalar length;
    int visible;

    trail = &pPursuee->my_trail;
    visible = 1;
    trail->nodes_shifted_this_frame = 0;
    if (trail->time_of_next_recording >= gTime_stamp_for_this_munging) {
        return;
    }
    trail->time_of_next_recording = gTime_stamp_for_this_munging + 500;
    if (BrVector3Dot(&pPursuee->direction, &trail->base_heading) < FastScalarCos(30)) {
        trail->has_deviated_recently = 1;
    }
    BrVector3Sub(&car_to_last_point_v, &trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t);
    length = BrVector3Length(&car_to_last_point_v);
    if (length < 0.3f) {
        return;
    }
    CalcNegativeXVector(&offset_v, &trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t, 0.5f);
    BrVector3Add(&start1, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
    BrVector3Add(&finish1, &pPursuee->car_master_actor->t.t.translate.t, &offset_v);
    BrVector3Sub(&start2, &trail->trail_nodes[trail->number_of_nodes - 2], &offset_v);
    BrVector3Sub(&finish2, &pPursuee->car_master_actor->t.t.translate.t, &offset_v);
    if (!trail->has_deviated_recently) {
        visible = PointVisibleFromHere(&start1, &finish1);
        if (visible) {
            visible = PointVisibleFromHere(&start2, &finish2);
            if (visible) {
                visible = PointVisibleFromHere(&trail->trail_nodes[trail->number_of_nodes - 2], &pPursuee->car_master_actor->t.t.translate.t);
            }
        }
    }
    if (trail->has_deviated_recently || !visible) {
        if ((visible && length > 2.0f) || (!visible && length > 1.5f)) {
            if (trail->number_of_nodes < (int)REC2_ASIZE(trail->trail_nodes)) {
                trail->number_of_nodes++;
            } else {
                memmove(&trail->trail_nodes[0], &trail->trail_nodes[1], sizeof(trail->trail_nodes) - sizeof(trail->trail_nodes[0]));
                trail->nodes_shifted_this_frame = 1;
            }
            trail->has_deviated_recently = 0;
            BrVector3Copy(&trail->base_heading, &pPursuee->direction);
        }
    }
    BrVector3Copy(&trail->trail_nodes[trail->number_of_nodes - 1], &pPursuee->car_master_actor->t.t.translate.t);
}

inline tS16 C2_HOOK_FASTCALL FindNearestTrailSection(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, br_vector3* pSection_v, br_vector3* pIntersect, br_scalar* pDistance) {

    return FindNearestGeneralSection(pPursuee, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, pSection_v, pIntersect, pDistance);
}

// CalcNextTrailSection

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004ab190
// FUNCTION: CARMA2_SW 0x004a7300
// FUNCTION: CARMA2_CHEAT 0x004ac680
void C2_HOOK_FASTCALL ProcessPursueAndTwat(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    tPursue_car_data* data;
    br_vector3 wank;
    br_vector3 section_v;
    br_vector3 intersect;
    br_scalar d;
    br_scalar s;
    br_scalar t;
    br_scalar distance;
    tFollow_path_result res;
    char str[256];
    tS16 section_no;

    data = &pOpponent_spec->pursue_car_data;
    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf("%s: ProcessPursueAndTwat() - new objective started", pOpponent_spec->car_spec->driver_name);
        data->direct_line_nodes[0].number_of_sections = 1;
        data->direct_line_nodes[0].sections[0] = 10000;
        data->direct_line_nodes[1].number_of_sections = 1;
        data->direct_line_nodes[1].sections[0] = 10000;
        data->direct_line_section.node_indices[0] = 10000;
        data->direct_line_section.node_indices[1] = 10001;
        data->direct_line_section.min_speed[0] = 0;
        data->direct_line_section.min_speed[1] = 0;
        data->direct_line_section.max_speed[0] = 255;
        data->direct_line_section.max_speed[1] = 255;
        data->direct_line_section.width = 0.5f;
        data->direct_line_section.type = ePST_normal;
        data->start_backup_time = 0;
        data->time_of_next_visibility_check = 0;
        data->time_pursuee_last_visible = 0;
        data->time_last_twatted_em = 0;
        data->time_last_away_from_pursuee = gTime_stamp_for_this_munging;
        data->state = ePCS_what_now;
        return;
    case ePOC_run:
        if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) && pOpponent_spec->distance_from_home > 75.0f) {
            DoNotDprintf("%s: Completing pursuit objective because I'm out of my precinct", pOpponent_spec->car_spec->driver_name);
            NewObjective(pOpponent_spec, eOOT_return_to_start);
            return;
        }
        data->direct_line_section.length = MAX(pOpponent_spec->player_to_oppo_d, 3.0f);
        if (pOpponent_spec->player_to_oppo_d > 3.0f) {
            data->time_last_away_from_pursuee = gTime_stamp_for_this_munging;
        }
        if (pOpponent_spec->field_0xb4 != 3 && gOpponents[pOpponent_spec->index].psyche.grudge_against_player < 15) {
            DoNotDprintf("%s: Completing pursuit objective because I'm happy now", pOpponent_spec->car_spec->driver_name);
            ObjectiveComplete(pOpponent_spec);
            return;
        }
        if (data->state != ePCS_backing_up) {
            if (data->time_last_twatted_em + 1000 < gTime_stamp_for_this_munging
                    && data->time_last_twatted_em + 3000 > gTime_stamp_for_this_munging
                    && BrVector3Length(&data->pursuee->collision_info->v) < 0.3f / WORLD_SCALE) {
                DoNotDprintf("%s: Backing up because we're 'stationary' after colliding with pursuee", pOpponent_spec->car_spec->driver_name);
                data->start_backup_time = gTime_stamp_for_this_munging;
                data->state = ePCS_backing_up;
            } else if (data->time_last_away_from_pursuee + 7000 < gTime_stamp_for_this_munging
                    && data->time_last_twatted_em + 7000 < gTime_stamp_for_this_munging
                    && data->start_backup_time + 10000 < gTime_stamp_for_this_munging) {
                DoNotDprintf("%s: Backing up because we're too close to pursuee without having twatted him", pOpponent_spec->car_spec->driver_name);
                data->start_backup_time = gTime_stamp_for_this_munging;
                data->state = ePCS_backing_up;
            } else {
                if (pOpponent_spec->cheating) {
                    if (pOpponent_spec->player_to_oppo_d < 30.0f
                            && PointVisibleFromHere(&data->pursuee->car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
                        data->time_pursuee_last_visible = gTime_stamp_for_this_munging;
                    } else {
                        data->time_pursuee_last_visible = 0;
                    }
                } else if (pOpponent_spec->player_in_view_now
                        || (data->time_of_next_visibility_check < gTime_stamp_for_this_munging
                            && pOpponent_spec->player_to_oppo_d < 35.0f
                            && PointVisibleFromHere(&data->pursuee->car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t))) {
                    data->time_pursuee_last_visible = gTime_stamp_for_this_munging;
                    data->time_of_next_visibility_check = gTime_stamp_for_this_munging + 900;
                }
                if (data->time_pursuee_last_visible + 3000 > gTime_stamp_for_this_munging) {
                    if (data->state != ePCS_following_line_of_sight) {
                        DoNotDprintf("%s: Commencing ePCS_following_line_of_sight state", pOpponent_spec->car_spec->driver_name);
                        data->state = ePCS_following_line_of_sight;
                        sprintf(str, "%s: I've spotted you!", pOpponent_spec->car_spec->driver_name);
                        ProcessFollowPath(pOpponent_spec, ePOC_start, 1, 1, 0);
                    }
                } else {
                    if (data->pursuee->my_trail.number_of_nodes < 2) {
                        DoNotDprintf("%s: Giving up pursuit - not visible & no trail yet", pOpponent_spec->car_spec->driver_name);
                        NewObjective(pOpponent_spec, eOOT_get_near_player);
                        return;
                    }
                    if (data->state != ePCS_following_trail) {
                        section_no = FindNearestTrailSection(pOpponent_spec, data->pursuee, &section_v, &intersect, &distance);
                        data->state = ePCS_following_trail;
                        if (distance > 20.0f || section_no == -1) {
                            DoNotDprintf("%s: Giving up pursuit - not visible & trail ain't close enough (%f)", pOpponent_spec->car_spec->driver_name, distance);
                            NewObjective(pOpponent_spec, eOOT_get_near_player);
                            return;
                        }
                        DoNotDprintf("%s: Commencing ePCS_following_trail state", pOpponent_spec->car_spec->driver_name);
                        pOpponent_spec->follow_path_data.section_no = section_no;
                        ProcessFollowPath(pOpponent_spec, ePOC_start, 1, 0, 0);
                    }
                }
            }
        }
        switch (data->state) {
        case ePCS_backing_up:
            if (data->start_backup_time + 2200 < gTime_stamp_for_this_munging) {
                pOpponent_spec->car_spec->acc_force = 0.0f;
                pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
                if (data->start_backup_time + 3000 < gTime_stamp_for_this_munging) {
                    pOpponent_spec->car_spec->brake_force = 0.0f;
                    data->state = ePCS_what_now;
                    DoNotDprintf("%s: Finished backing up.", pOpponent_spec->car_spec->driver_name);
                }
            } else {
                pOpponent_spec->car_spec->curvature = 0.0f;
                pOpponent_spec->car_spec->brake_force = 0.0f;
                pOpponent_spec->car_spec->acc_force = pOpponent_spec->car_spec->collision_info->M * -8.0f;
            }
            break;
        case ePCS_following_line_of_sight:
            BrVector3Copy(&data->direct_line_nodes[0].pos, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            BrVector3Sub(&wank, &data->pursuee->car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            d = BrVector3Length(&wank);
            BrVector3Sub(&wank, &data->pursuee->collision_info->v, &pOpponent_spec->car_spec->collision_info->v);
            s = BrVector3Length(&wank);
            if (s < 1.0f) {
                t = 0.0f;
            } else {
                t = d / s * 0.5f;
            }
            BrVector3Scale(&data->direct_line_nodes[1].pos, &data->pursuee->collision_info->v, t);
            BrVector3Accumulate(&data->direct_line_nodes[1].pos, &data->pursuee->car_master_actor->t.t.translate.t);
            if (d < 2.0f) {
                ProcessFollowPath(pOpponent_spec, ePOC_run, 1, 1, 1);
            } else {
                ProcessFollowPath(pOpponent_spec, ePOC_run, 1, 1, 0);
            }
            break;
        case ePCS_following_trail:
            if (data->pursuee->my_trail.nodes_shifted_this_frame) {
                if (pOpponent_spec->follow_path_data.section_no > 15000) {
                    pOpponent_spec->follow_path_data.section_no--;
                } else {
                    data->state = ePCS_following_trail;
                    section_no = FindNearestTrailSection(pOpponent_spec, data->pursuee, &section_v, &intersect, &distance);
                    DoNotDprintf("%s: Trail got away; found new trail section %d", pOpponent_spec->car_spec->driver_name, section_no);
                    if (section_no == -1 || distance > 20.0f || !PointVisibleFromHere(&intersect, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
                        DoNotDprintf("%s: ...which unfortunately is too far away (%fBRU) or not visible - end of pursuit", pOpponent_spec->car_spec->driver_name, distance);
                        NewObjective(pOpponent_spec, eOOT_get_near_player);
                        return;
                    }
                    pOpponent_spec->follow_path_data.section_no = section_no;
                    ProcessFollowPath(pOpponent_spec, ePOC_start, 1, 0, 0);
                }
                DoNotDprintf("%s: Following re-jobbied section %d/%d", pOpponent_spec->car_spec->driver_name, pOpponent_spec->follow_path_data.section_no, data->pursuee->my_trail.number_of_nodes - 1);
            }
            sprintf(str, "%s: Trail section %d/%d", pOpponent_spec->car_spec->driver_name, pOpponent_spec->follow_path_data.section_no, data->pursuee->my_trail.number_of_nodes - 1);
            res = ProcessFollowPath(pOpponent_spec, ePOC_run, 1, 0, 0);
            if (res == eFPR_given_up) {
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                return;
            }
            if (res == eFPR_end_of_path) {
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                return;
            }
            break;
        case ePCS_what_now:
            PDEnterDebugger("ERROR: what_now state called in ProcessPursueAndTwat()");
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

static inline void C2_HOOK_FASTCALL ProcessRunAway(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    tS16 section_no;
    br_scalar distance;
    br_vector3 direction_v;
    br_vector3 intersect;
    char str[256];

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf("%s: ProcessRunAway() - new objective started", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->run_away_data.time_to_stop = gTime_stamp_for_this_munging + 1000 * IRandomBetween(30, 90);
        ClearOpponentsProjectedRoute(pOpponent_spec);
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) >= 0.0f) {
            AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
        } else {
            AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 0);
        }
        TopUpRandomRoute(pOpponent_spec, -1);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        sprintf(str, "%s: Shit! I'm out of here...", pOpponent_spec->car_spec->driver_name);
        break;
    case ePOC_run:
        if (gTime_stamp_for_this_munging > pOpponent_spec->run_away_data.time_to_stop) {
            ObjectiveComplete(pOpponent_spec);
        } else {
            if (pOpponent_spec->follow_path_data.section_no > 20000) {
                ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path_data.section_no - 20000);
                pOpponent_spec->follow_path_data.section_no = 20000;
            }
            if (pOpponent_spec->nnext_sections < (int)REC2_ASIZE(pOpponent_spec->next_sections)) {
                TopUpRandomRoute(pOpponent_spec, (int)REC2_ASIZE(pOpponent_spec->next_sections) - pOpponent_spec->nnext_sections);
            }
            if (ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0) == eFPR_given_up) {
                ClearOpponentsProjectedRoute(pOpponent_spec);
                section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
                if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) >= 0.0f) {
                    AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 1);
                } else {
                    AddToOpponentsProjectedRoute(pOpponent_spec, section_no, 0);
                }
                TopUpRandomRoute(pOpponent_spec, -1);
                ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
            }
        }
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004abc00
// FUNCTION: CARMA2_SW 0x004a7d70
// FUNCTION: CARMA2_CHEAT 0x004ad0f0
void C2_HOOK_FASTCALL ProcessWaitForSomeHaplessSod(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        return;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = 15.f * pOpponent_spec->car_spec->collision_info->M;
        return;
    case ePOC_die:
        return;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004abc40
// STUB: CARMA2_SW 0x004a7db0
// STUB: CARMA2_CHEAT 0x004ad130
void C2_HOOK_FASTCALL ProcessReturnToStart(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004ac540
// STUB: CARMA2_SW 0x004a86b0
// STUB: CARMA2_CHEAT 0x004ada30
void C2_HOOK_FASTCALL ProcessLevitate(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ac890
// FUNCTION: CARMA2_SW 0x004a8a00
// FUNCTION: CARMA2_CHEAT 0x004add80
void C2_HOOK_FASTCALL ProcessGetNearPlayer(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {
    int res;

    C2_HOOK_BUG_ON(REC2_ASIZE(pOpponent_spec->next_sections) != 12);

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf("%s: ProcessGetNearPlayer() - new objective started", pOpponent_spec->car_spec->driver_name);
        NewCalcGetNearPlayerRoute(pOpponent_spec, &gProgram_state.current_car, 1);
        ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        break;
    case ePOC_run:
        if (((tU16)pOpponent_spec->car_spec->car_ID >> 8) == eVehicle_rozzer && pOpponent_spec->distance_from_home > 75.0f) {
            DoNotDprintf("%s: Completing get_near objective because I'm out of my precinct", pOpponent_spec->car_spec->driver_name);
            NewObjective(pOpponent_spec, eOOT_return_to_start);
            return;
        }
        if (pOpponent_spec->follow_path_data.section_no > 20000) {
            ShiftOpponentsProjectedRoute(pOpponent_spec, pOpponent_spec->follow_path_data.section_no - 20000);
            pOpponent_spec->follow_path_data.section_no = 20000;
        }
        if (pOpponent_spec->nnext_sections <= (int)REC2_ASIZE(pOpponent_spec->next_sections) - 2) {
            NewCalcGetNearPlayerRoute(pOpponent_spec, &gProgram_state.current_car, 0);
        }
        res = ProcessFollowPath(pOpponent_spec, ePOC_run, 0, 0, 0);
        if (res == eFPR_given_up || res == eFPR_end_of_path) {
            if (res == eFPR_given_up) {
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            } else {
                DoNotDprintf("%s: Restarting get_near_player route because ran out of path!", pOpponent_spec->car_spec->driver_name);
            }
            NewCalcGetNearPlayerRoute(pOpponent_spec, &gProgram_state.current_car, 1);
            ProcessFollowPath(pOpponent_spec, ePOC_start, 0, 0, 0);
        }
        break;
    default:
        break;
    }
}

static inline void C2_HOOK_FASTCALL ProcessFrozen(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand) {

    switch (pCommand) {
    case ePOC_start:
        DoNotDprintf("%d ProcessFrozen() - new task started", pOpponent_spec->index);
        DoNotDprintf("%s: Rematerialising from ePOC_start in ProcessFrozen()...", pOpponent_spec->car_spec->driver_name);
        RematerialiseOpponentOnNearestSection(pOpponent_spec, 0.0f);
        pOpponent_spec->car_spec->acc_force = 0.0f;
        pOpponent_spec->car_spec->brake_force = 15.0f * pOpponent_spec->car_spec->collision_info->M;
        break;
    case ePOC_run:
        pOpponent_spec->car_spec->brake_force = 15.0f * pOpponent_spec->car_spec->collision_info->M;
        break;
    case ePOC_die:
        pOpponent_spec->car_spec->brake_force = 0.0f;
        break;
    }
}

inline int C2_HOOK_FASTCALL HeadOnWithPlayerPossible(tOpponent_spec* pOpponent_spec) {
    br_vector3 oppo_to_player_norm;

    BrVector3Sub(&oppo_to_player_norm,
        &gProgram_state.current_car.car_master_actor->t.t.translate.t,
        &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    BrVector3Normalise(&oppo_to_player_norm, &oppo_to_player_norm);
    if (BrVector3Dot(&pOpponent_spec->car_spec->direction, &oppo_to_player_norm) > gHead_on_cos_value
            && BrVector3Dot(&gProgram_state.current_car.direction, &oppo_to_player_norm) < -gHead_on_cos_value) {
        DoNotDprintf("HOORAY! Head-on imminent");
        return 1;
    }
    return 0;
}

inline int C2_HOOK_FASTCALL AlreadyPursuingCar(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee) {

    return pOpponent_spec->current_objective == eOOT_pursue_and_twat && pOpponent_spec->pursue_car_data.pursuee == pPursuee;
}

inline int C2_HOOK_FASTCALL LastTwatteeAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_we_hit != NULL && pOpponent_spec->car_spec->last_person_we_hit->driver == eDriver_local_human;
}

inline int C2_HOOK_FASTCALL LastTwatterAPlayer(tOpponent_spec* pOpponent_spec) {

    return pOpponent_spec->car_spec->last_person_to_hit_us != NULL && pOpponent_spec->car_spec->last_person_to_hit_us->driver == eDriver_local_human;
}

// FUNCTION: CARMA2_HW 0x004ace20
// FUNCTION: CARMA2_SW 0x004a8f90
void C2_HOOK_FASTCALL ObjectiveComplete(tOpponent_spec* pOpponent_spec) {
    DoNotDprintf("%s: Objective Completed", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->new_objective_required = 1;
    switch (pOpponent_spec->current_objective) {
    case eOOT_complete_race:
        gNum_of_opponents_completing_race--;
        break;
    case eOOT_get_near_player:
        gNum_of_opponents_getting_near--;
        break;
    case eOOT_pursue_and_twat:
        gNum_of_opponents_pursuing--;
        break;
    default:
        break;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004a9180
// FUNCTION: CARMA2_SW 0x004a52f0
// FUNCTION: CARMA2_CHEAT 0x004aa4a0
void C2_HOOK_FASTCALL TeleportOpponentToNearestSafeLocation(tOpponent_spec* pOpponent_spec) {
    tS16 section_no;
    br_vector3 direction_v;
    br_vector3 intersect;
    br_scalar distance;
    int found_safe_place;
    int section_no_index;
    tU8 reversed;

    found_safe_place = 0;
    section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
    if (section_no >= 0) {
        pOpponent_spec->last_in_view = 0;
        TurnOpponentPhysicsOff(pOpponent_spec);
        reversed = BrVector3Dot(&pOpponent_spec->car_spec->direction, &direction_v) < 0.f;
        ClearOpponentsProjectedRoute(pOpponent_spec);
        AddToOpponentsProjectedRoute(pOpponent_spec, section_no, reversed);
        TopUpRandomRoute(pOpponent_spec, -1);
        section_no_index = 1;
        while (!found_safe_place) {
            BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t,
                &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[pOpponent_spec->next_sections[section_no_index].section_no].node_indices[pOpponent_spec->next_sections[section_no_index].direction]].pos);
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(pOpponent_spec);
            if (pOpponent_spec->player_to_oppo_d > gIn_view_distance) {
                found_safe_place = 1;
            }
            section_no_index++;
            if (section_no_index >= pOpponent_spec->nnext_sections) {
                ShiftOpponentsProjectedRoute(pOpponent_spec, section_no_index - 1);
                TopUpRandomRoute(pOpponent_spec, -1);
                section_no_index = 0;
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004ace70
// FUNCTION: CARMA2_SW 0x004a8fe0
// FUNCTION: CARMA2_CHEAT 0x004ae360
void C2_HOOK_FASTCALL ChooseNewObjective(tOpponent_spec* pOpponent_spec, int pMust_choose_one) {
    char str[256];

    if (pOpponent_spec->current_objective == eOOT_knackered_and_freewheeling || pOpponent_spec->knackeredness_detected) {
        return;
    }
    if (gTime_stamp_for_this_munging > pOpponent_spec->next_out_of_world_check) {
        pOpponent_spec->next_out_of_world_check = gTime_stamp_for_this_munging + 500;
        if (HasCarFallenOffWorld(pOpponent_spec->car_spec)) {
            TeleportOpponentToNearestSafeLocation(pOpponent_spec);
            NewObjective(pOpponent_spec, eOOT_get_near_player);
            return;
        }
    }
    if (pOpponent_spec->car_spec->knackered && !pOpponent_spec->knackeredness_detected) {
        pOpponent_spec->knackeredness_detected = 1;
        DoNotDprintf("%s: Knackered - dealing with appropriately", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->car_spec->has_been_stolen) {
            NewObjective(pOpponent_spec, eOOT_levitate);
        } else {
            NewObjective(pOpponent_spec, eOOT_knackered_and_freewheeling);
        }
        return;
    }

    if (pOpponent_spec->current_objective == eOOT_frozen) {
        if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == 0.f) {
            return;
        }
        DoNotDprintf("%s: Time to unfreeze", pOpponent_spec->car_spec->driver_name);
        if (pOpponent_spec->pursuing_player_before_freeze) {
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
        } else {
            NewObjective(pOpponent_spec, eOOT_get_near_player);
        }
        return;
    }
    if (GET_CAR_SPEED_FACTOR(pOpponent_spec->car_spec) == 0.f) {
        DoNotDprintf("%s: Decided to freeze", pOpponent_spec->car_spec->driver_name);
        pOpponent_spec->pursuing_player_before_freeze = pOpponent_spec->current_objective == eOOT_pursue_and_twat && pOpponent_spec->pursue_car_data.pursuee == &gProgram_state.current_car;
        NewObjective(pOpponent_spec, eOOT_frozen);
        return;
    }
    if (!gFirst_frame) {
        int general_grudge_increase;

        general_grudge_increase = (int)(pOpponent_spec->nastiness * 40.0f + 10.0f);
        if (pOpponent_spec->car_spec->scary_bang && pOpponent_spec->player_to_oppo_d < 10.f) {
            int pursuit_percentage;

            if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
                pursuit_percentage = 40;
            } else {
                pursuit_percentage = 0;
            }
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                if (PercentageChance(20)) {
                    DoNotDprintf("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            } else if (gOpponents[pOpponent_spec->index].strength_rating < 5) {
                if (PercentageChance((int)((pursuit_percentage + 60) - 50.f * pOpponent_spec->nastiness))) {
                    DoNotDprintf("%s: Decided to run away", pOpponent_spec->car_spec->driver_name);
                    NewObjective(pOpponent_spec, eOOT_run_away);
                    return;
                }
            }
        }

        if (pOpponent_spec->current_objective != eOOT_run_away || gTime_stamp_for_this_munging >= pOpponent_spec->time_this_objective_started + 20000) {
            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) && pOpponent_spec->murder_reported && pOpponent_spec->player_to_oppo_d < 20.f && !AlreadyPursuingCar(pOpponent_spec, &gProgram_state.current_car)) {
                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Furderous melon!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf("%s: Decided to pursue after MURDER", pOpponent_spec->car_spec->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            }
            if (pOpponent_spec->car_spec->big_bang
                    && LastTwatterAPlayer(pOpponent_spec)
                    && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_to_hit_us)) {

                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Christ! What was that?", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf("%s: Decided to pursue after big bang; last person to twat us was %s",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, pOpponent_spec->car_spec->last_person_to_hit_us);
                return;
            }
            if (LastTwatteeAPlayer(pOpponent_spec) && !AlreadyPursuingCar(pOpponent_spec, pOpponent_spec->car_spec->last_person_we_hit)) {

                gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                sprintf(str, "%s: Ha! Bet you weren't expecting that!", pOpponent_spec->car_spec->driver_name);
                DoNotDprintf("%s: Decided to pursue %s after accidentally hitting them",
                    pOpponent_spec->car_spec->driver_name,
                    pOpponent_spec->car_spec->last_person_we_hit->driver_name);
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                return;
            }
            if (!AlreadyPursuingCar(pOpponent_spec, &gProgram_state.current_car)) {
                if (pOpponent_spec->car_spec->grudge_raised_recently
                        && (!CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec) || pOpponent_spec->player_to_oppo_d <= 20.f)
                        && LastTwatterAPlayer(pOpponent_spec)
                        && gOpponents[pOpponent_spec->index].psyche.grudge_against_player > 20) {
                    gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                    sprintf(str, "%s: Right! That's enough, %s!", pOpponent_spec->car_spec->driver_name, gProgram_state.current_car.driver_name);
                    DoNotDprintf("%s: Decided to pursue after grudginess raised; last person to twat us was %s",
                        pOpponent_spec->car_spec->driver_name, pOpponent_spec->car_spec->last_person_to_hit_us->driver_name);
                    NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                    return;
                }
                if (pOpponent_spec->player_in_view_now && !pOpponent_spec->acknowledged_piv) {
                    int pursuit_percentage;
                    int do_it;

                    pOpponent_spec->acknowledged_piv = 1;
                    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                        pursuit_percentage = (int)(gCop_pursuit_speed_percentage_multiplier * (BrVector3Length(&gProgram_state.current_car.collision_info->v) * WORLD_SCALE_D - gDefinite_no_cop_pursuit_speed));
                    } else if (gNum_of_opponents_pursuing < gProgram_state.skill_level + 3) {
                        pursuit_percentage = (int)((float)(gOpponents[pOpponent_spec->index].psyche.grudge_against_player - 20) + 30.f * pOpponent_spec->nastiness);
                    } else {
                        pursuit_percentage = 0;
                    }

                    pursuit_percentage += 50 * HeadOnWithPlayerPossible(pOpponent_spec);
                    do_it = PercentageChance(pursuit_percentage);
                    DoNotDprintf("%s: Spotted player; chance of pursuing %d%%: %s", pOpponent_spec->car_spec->driver_name, pursuit_percentage, do_it ? "YES, Decided to pursue" : "NO, Decided NOT to pursue");
                    if (do_it) {
                        gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + general_grudge_increase);
                        sprintf(str, "%s: I've decided to kill you for the fun of it", pOpponent_spec->car_spec->driver_name);
                        NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                        return;
                    }
                }
            }
        }
    }
    if (pMust_choose_one) {
        DoNotDprintf("%s: Choosing new objective because we have to...", pOpponent_spec->car_spec->driver_name, pOpponent_spec->car_spec->last_person_to_hit_us);
        if (pOpponent_spec->has_moved_at_some_point) {
            int pursuit_percentage;

            if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
                NewObjective(pOpponent_spec, eOOT_return_to_start);
                return;
            }
            switch (pOpponent_spec->field_0xb4) {
            case 1:
                NewObjective(pOpponent_spec, eOOT_complete_race);
                break;
            case 2:
                NewObjective(pOpponent_spec, eOOT_get_near_player);
                break;
            case 3:
                NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
                break;
            default:
                pursuit_percentage = (int)pOpponent_spec->player_to_oppo_d - 15;
                if (PercentageChance(pursuit_percentage)) {
                    DoNotDprintf("%s: Choosing to get_near because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_get_near_player);
                } else {
                    DoNotDprintf("%s: Choosing to complete_race because chance dictated it (%d%%)", pOpponent_spec->car_spec->driver_name, pursuit_percentage);
                    NewObjective(pOpponent_spec, eOOT_complete_race);
                }
                return;
            }
        } else if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
            NewObjective(pOpponent_spec, eOOT_wait_for_some_hapless_sod);
            return;
        } else if (pOpponent_spec->pursue_from_start) {

            gOpponents[pOpponent_spec->index].psyche.grudge_against_player = (tU8)MIN(100, MAX(gOpponents[pOpponent_spec->index].psyche.grudge_against_player, 20) + 20 + 40 * pOpponent_spec->nastiness);
            NewObjective(pOpponent_spec, eOOT_pursue_and_twat, &gProgram_state.current_car);
            return;
        } else {
            NewObjective(pOpponent_spec, eOOT_complete_race);
        }
    }
}

inline void C2_HOOK_FASTCALL DisplayOpponentRecoveringHeadup(tOpponent_spec* pOpponent_spec) {
}

inline void C2_HOOK_FASTCALL StartToCheat(tOpponent_spec* pOpponent_spec) {
    tS16 section_no;
    int i;
    br_vector3 direction_v;
    br_vector3 intersect;
    br_scalar distance;

    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        DoNotDprintf("%s: StartToCheat() - Turning physics OFF", pOpponent_spec->car_spec->driver_name);
        TurnOpponentPhysicsOff(pOpponent_spec);
        RebuildActiveCarList();
        return;
    }
    DoNotDprintf("%s: StartToCheat() - Starting to cheat", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->cheating = 1;
    if (pOpponent_spec->current_objective != eOOT_levitate
        && pOpponent_spec->current_objective != eOOT_knackered_and_freewheeling
        && pOpponent_spec->current_objective != eOOT_none
        && !pOpponent_spec->car_spec->knackered
        && ((pOpponent_spec->car_spec->collision_info->last_special_volume != NULL && pOpponent_spec->car_spec->collision_info->last_special_volume->gravity_multiplier < 1.0f)
            || !pOpponent_spec->follow_path_data.has_moved_during_this_task
            || (pOpponent_spec->follow_path_data.struggle_time != 0 && !TimeToStopStruggling(pOpponent_spec)))) {
        DisplayOpponentRecoveringHeadup(pOpponent_spec);
    }
    if (pOpponent_spec->current_objective == eOOT_complete_race
        || pOpponent_spec->current_objective == eOOT_pursue_and_twat
        || pOpponent_spec->current_objective == eOOT_run_away
        || pOpponent_spec->current_objective == eOOT_get_near_player
        || pOpponent_spec->current_objective == eOOT_return_to_start) {
        if (pOpponent_spec->follow_path_data.section_no >= 15000 && pOpponent_spec->follow_path_data.section_no < 20000) {
            return;
        }
        i = pOpponent_spec->follow_path_data.section_no - 20000;
        if (i < 0 || i >= pOpponent_spec->nnext_sections - 1) {
            return;
        }
        section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
        if (section_no == GetOpponentsRealSection(pOpponent_spec, pOpponent_spec->follow_path_data.section_no)) {
            return;
        }
        for (; i < pOpponent_spec->nnext_sections; i++) {
            if (pOpponent_spec->next_sections[i].section_no == section_no) {
                break;
            }
        }
        if (i < pOpponent_spec->nnext_sections) {
            ShiftOpponentsProjectedRoute(pOpponent_spec, i);
            pOpponent_spec->follow_path_data.section_no = 20000;
        } else {
            pOpponent_spec->follow_path_data.section_no = 20000;
            pOpponent_spec->nnext_sections = 1;
            pOpponent_spec->next_sections[0].section_no = section_no;
            if ((intersect.v[2] - pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[3][2]) * pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[2][2]
                    + (intersect.v[0] - pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[3][0]) * pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[2][0]
                    + (intersect.v[1] - pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[3][1]) * pOpponent_spec->car_spec->car_master_actor->t.t.mat.m[2][1]
                >= 0.0f) {
                pOpponent_spec->next_sections[0].direction = 1;
            } else {
                pOpponent_spec->next_sections[0].direction = 0;
            }
        }
    }
}

inline void C2_HOOK_FASTCALL OiStopCheating(tOpponent_spec* pOpponent_spec) {

    DoNotDprintf("%s: OiStopCheating() - End of cheating sesh", pOpponent_spec->car_spec->driver_name);
    pOpponent_spec->cheating = 0;
    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        DoNotDprintf("%s: OiStopCheating() - Turning physics ON", pOpponent_spec->car_spec->driver_name);
        TurnOpponentPhysicsOn(pOpponent_spec);
        RebuildActiveCarList();
    }
}

// FUNCTION: CARMA2_HW 0x004aa570
// FUNCTION: CARMA2_SW 0x004a66e0
// FUNCTION: CARMA2_CHEAT 0x004aba60
void C2_HOOK_FASTCALL ProcessThisOpponent(tOpponent_spec* pOpponent_spec) {

    if ((gMap_view != 2 || !gShow_opponents) && gTime_stamp_for_this_munging > pOpponent_spec->last_in_view + 3000) {
        if (!pOpponent_spec->cheating) {
            StartToCheat(pOpponent_spec);
        }
    } else {
        if (pOpponent_spec->cheating) {
            OiStopCheating(pOpponent_spec);
        }
    }
    PossiblyRepairOpponent(pOpponent_spec);
    ChooseNewObjective(pOpponent_spec, pOpponent_spec->new_objective_required || gTime_stamp_for_this_munging > pOpponent_spec->time_for_this_objective_to_finish);
    pOpponent_spec->new_objective_required = 0;
    if (gCountdown || gRace_finished) {
        pOpponent_spec->car_spec->brake_force = 10.0f * pOpponent_spec->car_spec->collision_info->M;
    }
    if (!pOpponent_spec->finished_for_this_race && !gStop_opponents_moving && !gRace_finished && pOpponent_spec->stun_time_ends < gTime_stamp_for_this_munging) {
        ProcessCurrentObjective(pOpponent_spec, ePOC_run);
    }
    if (pOpponent_spec->cheating) {
        BrVector3Copy(&pOpponent_spec->car_spec->pos, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    }
}


// IsNetCarActive

// NoteCarsCurrentlyUsed

// FUNCTION: CARMA2_HW 0x004a7ea0
// FUNCTION: CARMA2_SW 0x004a4010
void C2_HOOK_FASTCALL AddIfNotInList(tPhysics_object* pObject, tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] == pObject) {
            pList[i] = NULL;
            return;
        }
    }
    PHILAddActiveObject(pObject, NULL, NULL, NULL);
}

// FUNCTION: CARMA2_HW 0x004a7ef0
// FUNCTION: CARMA2_SW 0x004a4060
void C2_HOOK_FASTCALL RemoveAnythingStillInList(tPhysics_object** pList, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (pList[i] != NULL) {
            PHILRemoveObject(pList[i]);
            pList[i]->field183_0x1d8 = 0;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a7a80
// STUB: CARMA2_SW 0x004a3bf0
// STUB: CARMA2_CHEAT 0x004a8d90
void C2_HOOK_FASTCALL RebuildActiveCarList(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004a7a60
// FUNCTION: CARMA2_SW 0x004a3bd0
// FUNCTION: CARMA2_CHEAT 0x004a8d70
void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void) {
    gActive_car_list_rebuild_required = 1;
    if (gProgram_state.racing) {
        RebuildActiveCarList();
    }
}

int C2_HOOK_FASTCALL OpponentRepairNecessary(tOpponent_spec* pOpponent_spec) {
    int i;
    tDamage_unit* damage = pOpponent_spec->car_spec->damage_units;

    for (i = 0, damage = pOpponent_spec->car_spec->damage_units; i < (int)REC2_ASIZE(pOpponent_spec->car_spec->damage_units); i++, damage++) {
        if (i != eDamage_driver && damage != NULL) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ad910
// FUNCTION: CARMA2_SW 0x004a9a80
// FUNCTION: CARMA2_CHEAT 0x004aee20
void C2_HOOK_FASTCALL PossiblyRepairOpponent(tOpponent_spec* pOpponent_spec) {
    char message[256];
    char headup[256];

    if (((tU16)pOpponent_spec->car_spec->car_ID >> 8) != eVehicle_rozzer
            && gTime_stamp_for_this_munging > pOpponent_spec->next_repair_check
            && pOpponent_spec->cheating
            && pOpponent_spec->current_objective != eOOT_levitate
            && pOpponent_spec->current_objective != eOOT_knackered_and_freewheeling
            && !pOpponent_spec->car_spec->knackered) {
        pOpponent_spec->next_repair_check = IRandomBetween(0, 2000) + 3000 + gTime_stamp_for_this_munging;
        if (gTime_stamp_for_this_munging > pOpponent_spec->car_spec->last_time_we_touched_a_player + pOpponent_spec->repair_interval) {
            if (OpponentRepairNecessary(pOpponent_spec)) {
                DoNotDprintf("*** Opponent %d, REPAIRING, timestamp %d", pOpponent_spec->car_spec->driver_name, gTime_stamp_for_this_munging);
                TotallyRepairACar(pOpponent_spec->car_spec);
                sprintf(message, "%s %s", pOpponent_spec->car_spec->driver_name, GetMiscString(273));
                strcpy(headup, message);
                pOpponent_spec->repair_interval = IRandomBetween(gMinTimeOpponentRepair, gMaxTimeOpponentRepair) * 60000;
                pOpponent_spec->last_repair_time = gTime_stamp_for_this_munging;
                DoNotDprintf("*** Opponent %s, repair_interval is %d", pOpponent_spec->car_spec->driver_name, pOpponent_spec->repair_interval);
            }
        }
    }
}

// DisplayOpponentRecoveringHeadup (moved above ProcessThisOpponent for inlining purposes)

// StartToCheat (moved above ProcessThisOpponent for inlining purposes)

// OiStopCheating (moved above ProcessThisOpponent for inlining purposes)

// TeleportCopToStart

// FUNCTION: CARMA2_HW 0x004ada90
// FUNCTION: CARMA2_SW 0x004a9c00
// FUNCTION: CARMA2_CHEAT 0x004aefa0
void C2_HOOK_FASTCALL CalcDistanceFromHome(tOpponent_spec* pOpponent_spec) {
    br_vector3 wank;

    BrVector3Sub(&wank, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &pOpponent_spec->start_pos);
    pOpponent_spec->distance_from_home = BrVector3Length(&wank);
}

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// FUNCTION: CARMA2_HW 0x004a8170
// FUNCTION: CARMA2_SW 0x004a42e0
// FUNCTION: CARMA2_CHEAT 0x004a9480
int C2_HOOK_FASTCALL RematerialiseOpponentOnNearestSection(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {
    br_vector3 intersect;
    br_vector3 direction_v;
    br_vector3 car_to_end;
    br_vector3* finish;
    br_scalar distance;
    br_scalar distance_to_end;
    tS16 section_no;

    if (pOpponent_spec->physics_me) {
        DoNotDprintf("%s: Actually, we're already materialised", pOpponent_spec->car_spec->driver_name);
        return 1;
    }
    section_no = FindNearestPathSection(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &direction_v, &intersect, &distance);
    finish = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]].pos;
    BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &intersect);
    PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &direction_v);
    BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    if (RematerialiseOpponent(pOpponent_spec, pSpeed)) {
        pOpponent_spec->car_spec->brake_force = 0.0f;
        pOpponent_spec->car_spec->acc_force = 0.0f;
        distance_to_end = BrVector3Length(&car_to_end);

        if (distance_to_end < 5.0f) {
            pOpponent_spec->car_spec->brake_force = pOpponent_spec->car_spec->collision_info->M * 15.0f;
        } else {
            pOpponent_spec->car_spec->acc_force = pOpponent_spec->car_spec->collision_info->M / 2.0f;
        }
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a87a0
// STUB: CARMA2_SW 0x004a4910
// STUB: CARMA2_CHEAT 0x004a9ac0
int C2_HOOK_FASTCALL RematerialiseOpponent(tOpponent_spec* pOpponent_spec, br_scalar pSpeed) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004adae0
// FUNCTION: CARMA2_SW 0x004a9c50
// FUNCTION: CARMA2_CHEAT 0x004aeff0
void C2_HOOK_FASTCALL CalcPlayerConspicuousness(tOpponent_spec* pOpponent_spec) {
    br_vector3 pos_in_cop_space;
    br_matrix34 inverse_transform;

    if (gTime_stamp_for_this_munging <= pOpponent_spec->next_player_visibility_check) {
        return;
    }
    pOpponent_spec->player_in_view_now = 0;
    if (CAR_SPEC_IS_ROZZER(pOpponent_spec->car_spec)) {
        pOpponent_spec->next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 100;
        if (pOpponent_spec->player_to_oppo_d < 20.0f) {
            BrMatrix34LPInverse(&inverse_transform, &pOpponent_spec->car_spec->car_master_actor->t.t.mat);
            BrMatrix34ApplyP(&pos_in_cop_space, &gProgram_state.current_car.car_master_actor->t.t.translate.t, &inverse_transform);
            if (pos_in_cop_space.v[2] < 0.0f && PointVisibleFromHere(&gProgram_state.current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
                pOpponent_spec->player_in_view_now = 1;
                pOpponent_spec->acknowledged_piv = 0;
            }
        }
    } else {
        pOpponent_spec->next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 4000;
        DoNotDprintf("%s: Time now: %9.2f; next vis check at %9.2f", pOpponent_spec->car_spec->driver_name, gTime_stamp_for_this_munging / 1000.0, pOpponent_spec->next_player_visibility_check / 1000.0);
        if (pOpponent_spec->player_to_oppo_d < 50.0f && PointVisibleFromHere(&gProgram_state.current_car.car_master_actor->t.t.translate.t, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t)) {
            pOpponent_spec->player_in_view_now = 1;
            pOpponent_spec->acknowledged_piv = 0;
        }
    }
}

// CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck

// NumberOfOpponentsStillRunning

inline void C2_HOOK_FASTCALL ResetPredominantJobbies(void) {
    int i;
    int j;
    int start;
    int count;
    int number_of_racers;
    int number_of_pursuers;

    count = 0;
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        if (!gProgram_state.AI_vehicles.opponents[i].car_spec->knackered) {
            count++;
        }
    }
    if (count != 0) {
        if (gCurrent_race.race_spec->opponent_nastiness_level >= 10.0f) {
            for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
                gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 3;
            }
        } else {
            for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
                gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 0;
            }
            number_of_pursuers = (int)(count * 0.6);
            number_of_racers = (int)(count * 0.2);
            for (i = 0; i < number_of_pursuers; i++) {
                start = IRandomBetween(0, count - i - 1);
                j = start;
                do {
                    if (gProgram_state.AI_vehicles.opponents[j].field_0xb4 == 0) {
                        gProgram_state.AI_vehicles.opponents[j].field_0xb4 = 2;
                        break;
                    }
                    j++;
                    if (j >= count) {
                        j = 0;
                    }
                } while (j != start);
            }
            for (i = 0; i < number_of_racers; i++) {
                start = IRandomBetween(0, count - number_of_pursuers - i - 1);
                j = start;
                do {
                    if (gProgram_state.AI_vehicles.opponents[j].field_0xb4 == 0) {
                        gProgram_state.AI_vehicles.opponents[j].field_0xb4 = 1;
                        break;
                    }
                    j++;
                    if (j >= count) {
                        j = 0;
                    }
                } while (j != start);
            }
        }
    }
}

#ifdef REC2_CHEAT
#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_CHEAT 0x004b0d30
void C2_HOOK_FASTCALL DeleteSection(tS16 pSection_to_delete) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_CHEAT 0x004b0ea0
void C2_HOOK_FASTCALL DeleteNode(tS16 pNode_to_delete, int pAnd_sections) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_CHEAT 0x004b10c0
void C2_HOOK_FASTCALL RebuildOppoPathModel(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL DeleteOrphanNodes(void) {
    tS16 node_no;

    for (node_no = 0; node_no < gProgram_state.AI_vehicles.number_of_path_nodes; node_no++) {
        while (node_no < gProgram_state.AI_vehicles.number_of_path_nodes && gProgram_state.AI_vehicles.path_nodes[node_no].number_of_sections == 0) {
            DeleteNode(node_no, 1);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_CHEAT 0x004b1ec0
int C2_HOOK_FASTCALL ConsistencyCheck(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_CHEAT 0x004b2310
void C2_HOOK_FASTCALL WriteOutOppoPaths(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL TrackElasticateyPath(void) {

    if (gAlready_elasticating && gNext_elastication < gTime_stamp_for_this_munging) {
        gNext_elastication = gTime_stamp_for_this_munging + 2000;
        BrVector3Copy(&gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[gMobile_section].node_indices[1]].pos, &gPlayer_car_master_actor->t.t.translate.t);
        RebuildOppoPathModel();
        if (gNext_write_during_elastication < gTime_stamp_for_this_munging) {
            gNext_write_during_elastication = gTime_stamp_for_this_munging + 10000;
            WriteOutOppoPaths();
        }
    }
}
#endif

// FUNCTION: CARMA2_HW 0x004a9600
// FUNCTION: CARMA2_SW 0x004a5770
// FUNCTION: CARMA2_CHEAT 0x004aa920
void C2_HOOK_FASTCALL LoadCopCars(tRace_info* pRace_info) {
    (void)pRace_info;

    gProgram_state.AI_vehicles.number_of_cops = 0;
}

// FUNCTION: CARMA2_HW 0x004a9610
// FUNCTION: CARMA2_SW 0x004a5780
// FUNCTION: CARMA2_CHEAT 0x004aa930
void C2_HOOK_FASTCALL LoadInOppoPaths(FILE* pF) {
    char s[256];
    char* res;
    int section_no;
    int node_no;
    int i;
    br_scalar x;
    br_scalar y;
    br_scalar z;
    br_scalar scalars[8];
    br_scalar distance;
    tPath_node* node_ptr;
    br_vector3 section_v;
    br_vector3 positive_y_vector;
    br_vector3 intersect;
    br_vector3 cop_to_section;
#ifdef REC2_CHEAT
    int data_errors;
    int sections_to_delete;
    tS16 delete_these[1024];

    data_errors = 0;
    sections_to_delete = 0;
#endif
    DoNotDprintf("Start of LoadInOppoPaths()...");
    gProgram_state.AI_vehicles.number_of_path_nodes = 0;
    gProgram_state.AI_vehicles.number_of_path_sections = 0;
    gProgram_state.AI_vehicles.path_nodes = NULL;
    gProgram_state.AI_vehicles.path_sections = NULL;
    gBit_per_node = NULL;
    gBIG_APC_index = -1;
    do {
        res = GetALineAndDontArgue(pF, s);
    } while (res != NULL && strcmp("START OF OPPONENT PATHS", s) != 0);
    if (res != NULL) {
        ReallocExtraPathNodes(GetAnInt(pF));
        if (gProgram_state.AI_vehicles.number_of_path_nodes != 0) {
            for (node_no = 0; node_no < gProgram_state.AI_vehicles.number_of_path_nodes; node_no++) {
                GetThreeFloats(pF, &x, &y, &z);
                gProgram_state.AI_vehicles.path_nodes[node_no].pos.v[0] = x;
                gProgram_state.AI_vehicles.path_nodes[node_no].pos.v[1] = y;
                gProgram_state.AI_vehicles.path_nodes[node_no].pos.v[2] = z;
                gProgram_state.AI_vehicles.path_nodes[node_no].number_of_sections = 0;
            }
        }
        ReallocExtraPathSections(GetAnInt(pF));
        if (gProgram_state.AI_vehicles.number_of_path_sections != 0) {
            for (section_no = 0; section_no < gProgram_state.AI_vehicles.number_of_path_sections; section_no++) {
                PossibleService();
                GetNScalars(pF, 8, scalars);
                gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0] = (tU16)scalars[0];
                gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1] = (tU16)scalars[1];
                gProgram_state.AI_vehicles.path_sections[section_no].min_speed[0] = (tU8)scalars[2];
                gProgram_state.AI_vehicles.path_sections[section_no].max_speed[0] = (tU8)scalars[3];
                gProgram_state.AI_vehicles.path_sections[section_no].min_speed[1] = (tU8)scalars[4];
                gProgram_state.AI_vehicles.path_sections[section_no].max_speed[1] = (tU8)scalars[5];
                gProgram_state.AI_vehicles.path_sections[section_no].width = scalars[6];
                BrVector3Sub(&section_v,
                    &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]].pos,
                    &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]].pos);
                gProgram_state.AI_vehicles.path_sections[section_no].length = BrVector3Length(&section_v);
                if (scalars[7] >= 1000.0f) {
                    gProgram_state.AI_vehicles.path_sections[section_no].type = (tU8)(scalars[7] - 1000.0f);
                    gProgram_state.AI_vehicles.path_sections[section_no].one_way = 1;
                } else {
                    gProgram_state.AI_vehicles.path_sections[section_no].type = (tU8)scalars[7];
                    gProgram_state.AI_vehicles.path_sections[section_no].one_way = 0;
                }
                for (node_no = 0; node_no < 2; node_no++) {
                    node_ptr = &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[node_no]];
                    if (node_ptr->number_of_sections < 8) {
                        node_ptr->sections[node_ptr->number_of_sections] = section_no;
                        node_ptr->number_of_sections++;
                    } else {
                        DoNotDprintf("ERROR: Too many sections (including section #%d) attached to node #%d",
                            section_no,
                            (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[node_no]);
#ifdef REC2_CHEAT
                        data_errors = 1;
#endif
                    }
                }
#ifdef REC2_CHEAT
                BrVector3Sub(&section_v,
                    &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]].pos,
                    &gProgram_state.AI_vehicles.path_nodes[(tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0]].pos);
                if (BrVector3LengthSquared(&section_v) == 0.0f) {
                    DoNotDprintf("ERROR: Opponent path section #%d has zero length (nodes #%d and #%d are in same position). Section and one node will be deleted.",
                        section_no,
                        (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[0],
                        (tS16)gProgram_state.AI_vehicles.path_sections[section_no].node_indices[1]);
                    delete_these[sections_to_delete] = section_no;
                    sections_to_delete++;
                }
#endif
            }
        }
#ifdef REC2_CHEAT
        if (data_errors) {
            PDFatalError("Opponent path data inconsistencies. Unable to rectumify them.");
        }
        if (sections_to_delete != 0) {
            for (i = 0; i < sections_to_delete; i++) {
                DoNotDprintf("Deleting section #%d (was #%d)", delete_these[i], i + delete_these[i]);
                DeleteSection(delete_these[i]);
                DeleteOrphanNodes();
                for (node_no = i; node_no < sections_to_delete; node_no++) {
                    delete_these[i]--;
                }
            }
            WriteOutOppoPaths();
            sprintf(s, "Errors in opponent path data. All have been corrected and written out to '%s'. Refer to diagnostic file for more details.", gOppo_path_filename);
            PDFatalError(s);
        }
#endif
        if (gAusterity_mode || gNet_mode != eNet_mode_none) {
            gProgram_state.AI_vehicles.number_of_cops = GetAnInt(pF);
            for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
                GetALineAndDontArgue(pF, s);
            }
            gProgram_state.AI_vehicles.number_of_cops = 0;
        } else {
            gProgram_state.AI_vehicles.number_of_cops = GetAnInt(pF);
            if (gProgram_state.AI_vehicles.number_of_cops != 0) {
                for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
                    PossibleService();
                    GetNScalars(pF, 6, scalars);
                    BrVector3Set(&gProgram_state.AI_vehicles.cop_start_points[i], scalars[0], scalars[1], scalars[2]);
                    if (scalars[3] == 9.0f && scalars[4] == 9.0f && scalars[5] == 9.0f) {
                        gBIG_APC_index = i;
                    }
                    FindNearestGeneralSection(NULL, &gProgram_state.AI_vehicles.cop_start_points[i], &section_v, &intersect, &distance);
                    BrVector3Set(&positive_y_vector, 0.0f, 1.0f, 0.0f);
                    BrVector3Cross(&gProgram_state.AI_vehicles.cop_start_vectors[i], &positive_y_vector, &section_v);
                    BrVector3Sub(&cop_to_section, &intersect, &gProgram_state.AI_vehicles.cop_start_points[i]);
                    if (BrVector3Dot(&gProgram_state.AI_vehicles.cop_start_vectors[i], &cop_to_section) < 0.0f) {
                        BrVector3Negate(&gProgram_state.AI_vehicles.cop_start_vectors[i], &gProgram_state.AI_vehicles.cop_start_vectors[i]);
                    }
                }
            }
        }
        do {
            GetALineAndDontArgue(pF, s);
        } while (strcmp("END OF OPPONENT PATHS", s) != 0);
        if (gProgram_state.AI_vehicles.number_of_path_sections != 0) {
            gBit_per_node = BrMemAllocate((gProgram_state.AI_vehicles.number_of_path_nodes + 7) / 8, kMem_oppo_bit_per_node);
        } else {
            gBit_per_node = NULL;
        }
        DoNotDprintf("End of LoadInOppoPaths(), totals:");
        DoNotDprintf("Nodes: %d", gProgram_state.AI_vehicles.number_of_path_nodes);
        DoNotDprintf("Sections: %d", gProgram_state.AI_vehicles.number_of_path_sections);
#ifdef REC2_CHEAT
        ConsistencyCheck();
#endif
    }
}

// FUNCTION: CARMA2_HW 0x004a9c50
// FUNCTION: CARMA2_SW 0x004a5dc0
// FUNCTION: CARMA2_CHEAT 0x004ab130
void C2_HOOK_FASTCALL DisposeOpponentPaths(void) {

    if (gProgram_state.AI_vehicles.path_nodes != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_nodes);
    }
    if (gProgram_state.AI_vehicles.path_sections != NULL) {
        BrMemFree(gProgram_state.AI_vehicles.path_sections);
    }
    if (gBit_per_node != NULL) {
        BrMemFree(gBit_per_node);
    }

    gProgram_state.AI_vehicles.number_of_path_nodes = 0;
    gProgram_state.AI_vehicles.number_of_path_sections = 0;
    gProgram_state.AI_vehicles.path_nodes = NULL;
    gProgram_state.AI_vehicles.path_sections = NULL;
    gBit_per_node = NULL;
}

// FUNCTION: CARMA2_HW 0x004a9e10
// FUNCTION: CARMA2_SW 0x004a5f80
// FUNCTION: CARMA2_CHEAT 0x004ab2f0
void C2_HOOK_FASTCALL MungeOpponents(tU32 pFrame_period) {
    /* FIXME: MungeAIWorld passes pFrame_period in ecx; the Mac version has no parameter */
    int i;
    int un_stun_flag;

    un_stun_flag = 0;
    if (gProgram_state.AI_vehicles.number_of_opponents == 0 && gNumber_of_cops_before_faffage == 0) {
        return;
    }
    if (gProgram_state.AI_vehicles.number_of_path_nodes == 0 || gProgram_state.AI_vehicles.number_of_path_sections == 0) {
        return;
    }
    gAcme_frame_count++;
    if (!gAcknowledged_start && !gCountdown) {
        gAcknowledged_start = 1;
        if (!gStart_jumped) {
            un_stun_flag = 1;
        }
    }
    if (gProgram_state.current_car.no_of_processes_recording_my_trail != 0) {
        RecordNextTrailNode(&gProgram_state.current_car);
    } else {
        StartRecordingTrail(&gProgram_state.current_car);
    }
#ifdef REC2_CHEAT
    TrackElasticateyPath();
    if (!gProcessing_opponents) {
        return;
    }
#endif
    gNum_of_opponents_pursuing = 0;
    gNum_of_opponents_getting_near = 0;
    gNum_of_opponents_completing_race = 0;
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        if (!gProgram_state.AI_vehicles.opponents[i].finished_for_this_race) {
            if (gProgram_state.AI_vehicles.opponents[i].current_objective == eOOT_pursue_and_twat) {
                gNum_of_opponents_pursuing++;
            } else if (gProgram_state.AI_vehicles.opponents[i].current_objective == eOOT_get_near_player) {
                gNum_of_opponents_getting_near++;
            } else if (gProgram_state.AI_vehicles.opponents[i].current_objective == eOOT_complete_race) {
                gNum_of_opponents_completing_race++;
            }
        }
    }
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        if (!gProgram_state.AI_vehicles.opponents[i].finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger(&gProgram_state.AI_vehicles.opponents[i]);
            }
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(&gProgram_state.AI_vehicles.opponents[i]);
            CalcPlayerConspicuousness(&gProgram_state.AI_vehicles.opponents[i]);
            ProcessThisOpponent(&gProgram_state.AI_vehicles.opponents[i]);
            ClearTwattageOccurrenceVariables(&gProgram_state.AI_vehicles.opponents[i]);
        }
    }
    for (i = 0; i < gNumber_of_cops_before_faffage; i++) {
        if (!gProgram_state.AI_vehicles.cops[i].finished_for_this_race) {
            if (un_stun_flag) {
                UnStunTheBugger(&gProgram_state.AI_vehicles.cops[i]);
            }
            CalcDistanceFromHome(&gProgram_state.AI_vehicles.cops[i]);
            CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck(&gProgram_state.AI_vehicles.cops[i]);
            CalcPlayerConspicuousness(&gProgram_state.AI_vehicles.cops[i]);
            ProcessThisOpponent(&gProgram_state.AI_vehicles.cops[i]);
            ClearTwattageOccurrenceVariables(&gProgram_state.AI_vehicles.cops[i]);
            gProgram_state.AI_vehicles.cops[i].murder_reported = 0;
        }
    }
    if (gNext_grudge_reduction < gTime_stamp_for_this_munging) {
        gNext_grudge_reduction = gTime_stamp_for_this_munging + 5000;
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
            if (!gProgram_state.AI_vehicles.opponents[i].finished_for_this_race) {
                gOpponents[gProgram_state.AI_vehicles.opponents[i].index].psyche.grudge_against_player -= MIN(gOpponents[gProgram_state.AI_vehicles.opponents[i].index].psyche.grudge_against_player, gGrudge_reduction_per_period);
            }
        }
    }
    RebuildActiveCarList();
    gFirst_frame = 0;
}

// FUNCTION: CARMA2_HW 0x004add50
// FUNCTION: CARMA2_SW 0x004a9ec0
// FUNCTION: CARMA2_CHEAT 0x004af260
void C2_HOOK_FASTCALL InitOpponents(tRace_info* pRace_info) {
    int i;
    int opponent_number;
    int rank_dependent_difficulty;
    int skill_dependent_difficulty;
    br_bounds bounds;
    tCar_spec* car_spec;
    tOpponent_spec* opponent_spec;

    gNext_grudge_reduction = gTime_stamp_for_this_munging + 10000;
    gGrudge_reduction_per_period = 3 - gProgram_state.skill_level;
    gFirst_frame = 1;
    gAcknowledged_start = 0;
    gStart_jumped = 0;
    gNum_viewable_cars = 0;
    gViewable_car_list[gNum_viewable_cars] = &gProgram_state.current_car;
    gNum_viewable_cars++;
    BrActorToBounds(&bounds, gProgram_state.track_spec.the_actor);
    gMinimum_yness_before_knackerisation = bounds.min.v[1] - 2.0f;
    gHead_on_cos_value = (float)cos(0.5235668420791626);
    gAcme_frame_count = 0;
    gProgram_state.current_car.no_of_processes_recording_my_trail = 0;
    gProgram_state.current_car.field_0x1540 = 0;
    gProgram_state.current_car.field_0x155c = -1;
    rank_dependent_difficulty = (int)((101.0 - (1.0 - (double)gCurrent_race.index / gNumber_of_races) * 99.0) * -0.1);
    skill_dependent_difficulty = gProgram_state.skill_level * 5;
    gBig_bang = (int)(70.0f - (2 * skill_dependent_difficulty - 3 * rank_dependent_difficulty) * gOpponent_nastyness_frigger);
    gIn_view_distance = MAX(45.0f, gCamera_yon - 10.0f);
    if (gNet_mode != eNet_mode_none) {
        gProgram_state.AI_vehicles.number_of_opponents = 0;
    } else {
        gProgram_state.AI_vehicles.number_of_opponents = pRace_info->number_of_racers - 1;
    }
    for (i = 0, opponent_number = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++, opponent_number++) {
        PossibleService();
        if (pRace_info->opponent_list[opponent_number].index < 0) {
            opponent_number++;
        }
        gProgram_state.AI_vehicles.opponents[i].car_spec = pRace_info->opponent_list[opponent_number].car_spec;
        gProgram_state.AI_vehicles.opponents[i].car_spec->car_ID = i + 0x200;
        DoNotDprintf("Car '%s', car_ID %x",
            gProgram_state.AI_vehicles.opponents[i].car_spec->driver_name,
            (tU16)gProgram_state.AI_vehicles.opponents[i].car_spec->car_ID);
        gProgram_state.AI_vehicles.opponents[i].index = pRace_info->opponent_list[opponent_number].index;
        gProgram_state.AI_vehicles.opponents[i].time_last_processed = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].next_repair_check = 0;
        gProgram_state.AI_vehicles.opponents[i].repair_interval = IRandomBetween(gMinTimeOpponentRepair, gMaxTimeOpponentRepair) * 60000;
        gProgram_state.AI_vehicles.opponents[i].last_repair_time = gTime_stamp_for_this_munging;
        DoNotDprintf("*** Opponent %s, repair_interval is %d",
            gProgram_state.AI_vehicles.opponents[i].car_spec->driver_name,
            gProgram_state.AI_vehicles.opponents[i].repair_interval);
        gProgram_state.AI_vehicles.opponents[i].time_this_objective_started = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].last_moved_ok = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].last_in_view = 0;
        gProgram_state.AI_vehicles.opponents[i].stun_time_ends = 0;
        gProgram_state.AI_vehicles.opponents[i].next_player_visibility_check = gTime_stamp_for_this_munging + IRandomBetween(0, 900) + 2000;
        gProgram_state.AI_vehicles.opponents[i].next_out_of_world_check = gTime_stamp_for_this_munging + 500;
        gProgram_state.AI_vehicles.opponents[i].finished_for_this_race = 0;
        gProgram_state.AI_vehicles.opponents[i].physics_me = 1;
        gProgram_state.AI_vehicles.opponents[i].pursue_from_start = 0;
        gProgram_state.AI_vehicles.opponents[i].cheating = 0;
        gProgram_state.AI_vehicles.opponents[i].knackeredness_detected = 0;
        gProgram_state.AI_vehicles.opponents[i].players_section_when_last_calced_full_path = -1;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_person_to_hit_us = NULL;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_person_we_hit = NULL;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_collision_time = 0;
        gProgram_state.AI_vehicles.opponents[i].car_spec->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
        gProgram_state.AI_vehicles.opponents[i].car_spec->grudge_raised_recently = 1;
        gProgram_state.AI_vehicles.opponents[i].car_spec->no_of_processes_recording_my_trail = 0;
        gProgram_state.AI_vehicles.opponents[i].nnext_sections = 0;
        gProgram_state.AI_vehicles.opponents[i].new_objective_required = 1;
        gProgram_state.AI_vehicles.opponents[i].current_objective = eOOT_none;
        gProgram_state.AI_vehicles.opponents[i].has_moved_at_some_point = 0;
        gProgram_state.AI_vehicles.opponents[i].player_in_view_now = 0;
        gProgram_state.AI_vehicles.opponents[i].acknowledged_piv = 0;
        gProgram_state.AI_vehicles.opponents[i].field_0xb4 = 0;
        gProgram_state.AI_vehicles.opponents[i].nastiness = (float)(((gOpponents[gProgram_state.AI_vehicles.opponents[i].index].strength_rating - 1) * 0.25
                                                                        + gProgram_state.skill_level * 0.5
                                                                        + gCurrent_race.race_spec->opponent_nastiness_level)
            / 3.0 * gOpponent_nastyness_frigger);
        BrVector3Set(&gProgram_state.AI_vehicles.opponents[i].pos_last_frame, 0.0f, 0.0f, 0.0f);
        gOpponents[gProgram_state.AI_vehicles.opponents[i].index].psyche.grudge_against_player = 10;
        gViewable_car_list[gNum_viewable_cars] = gProgram_state.AI_vehicles.opponents[i].car_spec;
        gNum_viewable_cars++;
        StunTheBugger(&gProgram_state.AI_vehicles.opponents[i], 0xfffffff);
    }
    if (gChallenger_index__opponent >= 0) {
        car_spec = GetCarSpecFromGlobalOppoIndex(gChallenger_index__opponent);
        opponent_spec = GetOpponentSpecFromCarSpec(car_spec);
        if (opponent_spec != NULL) {
            opponent_spec->pursue_from_start = 1;
        } else {
            DoNotDprintf("ERROR - can't record dare - no opponent_spec for car_spec");
        }
        gChallenger_index__opponent = -1;
    }
    ResetPredominantJobbies();
    gActive_car_list_rebuild_required = 1;
    RebuildActiveCarList();
}

// FUNCTION: CARMA2_HW 0x004ae5d0
// FUNCTION: CARMA2_SW 0x004aa740
// FUNCTION: CARMA2_CHEAT 0x004afae0
void C2_HOOK_FASTCALL DisposeOpponents(void) {
    int i;

    if (gProgram_state.AI_vehicles.number_of_cops != 0) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            DisposeCar(gProgram_state.AI_vehicles.cops[i].car_spec, (i == gBIG_APC_index) ? 99 : 98);
            BrMemFree(gProgram_state.AI_vehicles.cops[i].car_spec);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ae620
// FUNCTION: CARMA2_SW 0x004aa790
// FUNCTION: CARMA2_CHEAT 0x004afb30
void C2_HOOK_FASTCALL WakeUpOpponentsToTheFactThatTheStartHasBeenJumped(int pWhat_the_countdown_was) {
    int i;
    tOpponent_spec* opponent;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        opponent = &gProgram_state.AI_vehicles.opponents[i];
        UnStunTheBugger(opponent);
        StunTheBugger(opponent, MIN(IRandomBetween(1000, 2500), 1000 * pWhat_the_countdown_was));
    }
    for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
        opponent = &gProgram_state.AI_vehicles.cops[i];
        UnStunTheBugger(opponent);
        StunTheBugger(opponent, MIN(IRandomBetween(1000, 2500), 1000 * pWhat_the_countdown_was));
    }
    gAcknowledged_start = 1;
    gStart_jumped = 1;
}

// FUNCTION: CARMA2_HW 0x004ae750
// FUNCTION: CARMA2_SW 0x004aa8c0
void C2_HOOK_FASTCALL ReportMurderToPoliceDepartment(tCar_spec* pCar_spec) {
    int i;

    if (pCar_spec == &gProgram_state.current_car) {
        for (i = 0; i < gProgram_state.AI_vehicles.number_of_cops; i++) {
            gProgram_state.AI_vehicles.cops[i].murder_reported = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ae790
// FUNCTION: CARMA2_SW 0x004aa900
// FUNCTION: CARMA2_CHEAT 0x004afca0
int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory) {
    switch (pCategory) {
    case eVehicle_self:
        return 1;
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.number_of_opponents;
    case eVehicle_net_player:
        if (gNet_mode != eNet_mode_none) {
            return gNumber_of_net_players - 1;
        } else {
            return 0;
        }
    case eVehicle_rozzer:
        return gNumber_of_cops_before_faffage;
    case eVehicle_drone:
        return 0;
    case eVehicle_not_really:
        return gNum_active_non_cars;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ae7e0
// FUNCTION: CARMA2_SW 0x004aa950
// FUNCTION: CARMA2_CHEAT 0x004afcf0
tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex) {
    int net_player_index;

    switch (pCategory) {
    case eVehicle_self:
        return &gProgram_state.current_car;
    case eVehicle_net_player:
        net_player_index = gThis_net_player_index;
        if (pIndex < net_player_index) {
            return gNet_players[pIndex].car;
        } else {
            return (gNet_players + 1)[pIndex].car;
        }
    case eVehicle_opponent:
        return gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
    case eVehicle_rozzer:
        return gProgram_state.AI_vehicles.cops[pIndex].car_spec;
    case eVehicle_drone:
        PDEnterDebugger("OPPONENT.C: GetCarSpec() can't return drone car_specs");
        return NULL;
    case eVehicle_not_really:
        return (tCar_spec*)gActive_non_car_list[pIndex];
    default:
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004ae880
// FUNCTION: CARMA2_SW 0x004aa9f0
// FUNCTION: CARMA2_CHEAT 0x004afd90
const char* C2_HOOK_FASTCALL GetDriverName(tVehicle_type pCategory, int pIndex) {
    tCar_spec* car;
    char* name;

    switch (pCategory) {
    case eVehicle_self:
        return gProgram_state.player_name;
    case eVehicle_opponent:
        car = gProgram_state.AI_vehicles.opponents[pIndex].car_spec;
        if (car != NULL) {
            name = car->driver_name;
            if (name != NULL) {
                return name;
            }
        }
        return "???";
    case eVehicle_rozzer:
        return gCop_name;
    case eVehicle_drone:
        return gDrone_name;
    case eVehicle_not_really:
        return NULL;
    default:
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004ae8e0
// FUNCTION: CARMA2_SW 0x004aaa50
// FUNCTION: CARMA2_CHEAT 0x004afdf0
tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec) {
    int i;
    tU16 car_ID;
    int car_category;

    car_ID = pCar_spec->car_ID;
    car_category = car_ID & ~0xff;
    if (car_category == eVehicle_opponent << 8) {
        for (i = 0; i < GetCarCount(eVehicle_opponent); i++) {
            if (gProgram_state.AI_vehicles.opponents[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.opponents[i];
            }
        }
    } else if (car_category == eVehicle_rozzer << 8) {
        for (i = 0; i < GetCarCount(eVehicle_rozzer); i++) {
            if (gProgram_state.AI_vehicles.cops[i].car_spec == pCar_spec) {
                return &gProgram_state.AI_vehicles.cops[i];
            }
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004ae970
// FUNCTION: CARMA2_SW 0x004aaae0
tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex) {
    int i;

    for (i = 0; i < gProgram_state.AI_vehicles.number_of_opponents; i++) {
        if (gProgram_state.AI_vehicles.opponents[i].index == pIndex) {
            return gProgram_state.AI_vehicles.opponents[i].car_spec;
        }
    }
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004ae9e0
// FUNCTION: CARMA2_SW 0x004aab50
// FUNCTION: CARMA2_CHEAT 0x004afef0
int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no) {

    if (pSection_no >= 20000) {
        return pOpponent_spec->next_sections[pSection_no - 20000].section_no;
    } else if (pSection_no >= 15000) {
        return pSection_no - 15000;
    } else {
        return pSection_no;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004aea00
// FUNCTION: CARMA2_SW 0x004aab70
int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec) {
    if (pOpponent_spec->current_objective == eOOT_pursue_and_twat) {
        if (pOpponent_spec->pursue_car_data.state == ePCS_following_trail) {
            return pOpponent_spec->follow_path_data.section_no;
        } else if (pOpponent_spec->pursue_car_data.state == ePCS_following_line_of_sight) {
            return 10000;
        }
    }
    return 20000;
}

// FUNCTION: CARMA2_HW 0x004aea30
// FUNCTION: CARMA2_SW 0x004aaba0
int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section) {
    if (pCurrent_section >= 20000) {
        if (pOpponent_spec->nnext_sections > pCurrent_section - 19999
                && (pOpponent_spec->cheating || gProgram_state.AI_vehicles.path_sections[pCurrent_section - 19999].type != ePST_cheat_only)) {
            return pCurrent_section + 1;
        }
        return -1;
    }
    if (pCurrent_section >= 15000) {
        int trail_section = pCurrent_section - 15000;

        if (trail_section >= pOpponent_spec->pursue_car_data.pursuee->my_trail.number_of_nodes - 2) {
            trail_section = -1;
        } else {
            trail_section += 15001;
        }
        return (tS16)trail_section;
    }
    return -1;
}

// GetOpponentsSectionStartNodePoint

// GetOpponentsSectionFinishNodePoint

// FUNCTION: CARMA2_HW 0x004aece0
// FUNCTION: CARMA2_SW 0x004aae50
// FUNCTION: CARMA2_CHEAT 0x004b01f0
float C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection) {
    if (pSection >= 20000) {
        int sections = pOpponent_spec->nnext_sections;
        int s = pSection - 20000;
        if (sections > s) {
            tS16 idx = pOpponent_spec->next_sections[pSection - 20000].section_no;
            return gProgram_state.AI_vehicles.path_sections[idx].width;
        }
    }
    if (pSection >= 15000) {
        return 0.5f;
    }
    if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.width;
    }
    return gProgram_state.AI_vehicles.path_sections[(tS32)pSection].width;
}

// FUNCTION: CARMA2_HW 0x004aed50
// FUNCTION: CARMA2_SW 0x004aaec0
int C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].min_speed[(route->direction ^ pTowards_finish) == 0];
    } else if (pSection >= 15000) {
        return 0;
    } else if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.min_speed[pTowards_finish];
    }
    DoNotDprintf("WARNING - GetOpponentsSectionMinSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("WARNING - GetOpponentsSectionMinSpeed()");
    return 0;
}

// FUNCTION: CARMA2_HW 0x004aedf0
// FUNCTION: CARMA2_SW 0x004aaf60
int C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish) {
    if (pSection >= 20000 && pOpponent_spec->nnext_sections > pSection - 20000) {
        const tRoute_section* route = &pOpponent_spec->next_sections[pSection - 20000];
        return gProgram_state.AI_vehicles.path_sections[route->section_no].max_speed[(route->direction ^ pTowards_finish) == 0];
    } else if (pSection >= 15000) {
        return 255;
    } else if (pSection == 10000) {
        return pOpponent_spec->pursue_car_data.direct_line_section.max_speed[pTowards_finish];
    }
    DoNotDprintf("WARNING - GetOpponentsSectionMaxSpeed() - section not found in next_section array for opponent %s", pOpponent_spec->car_spec->driver_name);
    PDEnterDebugger("WARNING - GetOpponentsSectionMaxSpeed()");
    return 255;
}

// FUNCTION: CARMA2_HW 0x004aee90
// FUNCTION: CARMA2_SW 0x004ab000
// FUNCTION: CARMA2_CHEAT 0x004b03a0
void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index) {

    gOpponents[pOpponent_index].psyche.grudge_against_player = 0;
}

// FUNCTION: CARMA2_CHEAT 0x004b03c0
inline void C2_HOOK_FASTCALL ClearTwattageOccurrenceVariables(tOpponent_spec* pOpponent_spec) {

    pOpponent_spec->car_spec->big_bang = 0;
    pOpponent_spec->car_spec->scary_bang = 0;
    pOpponent_spec->car_spec->grudge_raised_recently = 0;
    pOpponent_spec->car_spec->last_person_to_hit_us = NULL;
    pOpponent_spec->car_spec->last_person_we_hit = NULL;
}

// FUNCTION: CARMA2_HW 0x004aeeb0
// FUNCTION: CARMA2_SW 0x004ab020
void C2_HOOK_FASTCALL TwoCarsHitEachOther(tCar_spec* pA_car, tCar_spec* pAnother_car) {

    if (pA_car != NULL && pA_car->driver == eDriver_local_human) {
        pAnother_car->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
    }
    if (pAnother_car != NULL && pAnother_car->driver == eDriver_local_human) {
        pA_car->last_time_we_touched_a_player = gTime_stamp_for_this_munging;
    }
}

// FUNCTION: CARMA2_HW 0x004aeef0
// FUNCTION: CARMA2_SW 0x004ab060
// FUNCTION: CARMA2_CHEAT 0x004b0450
void C2_HOOK_FASTCALL RecordOpponentTwattageOccurrence(tCar_spec* pTwatter, tCar_spec* pTwattee) {
    int bangness;
    int twatter_index;
    int twattee_index;
    int grudginess_caused_by_damage;
    int new_grudge_value;
    float damage;
    tOpponent_spec* twattee_opponent_spec;
    tOpponent_spec* twatter_opponent_spec;

    if ((pTwatter == NULL || pTwatter->driver != eDriver_oppo) && (pTwattee == NULL || pTwattee->driver != eDriver_oppo)) {
        return;
    }
    damage = pTwattee->damage_magnitude_accumulator;
    bangness = (int)MIN(100.0, sqrt(damage * 300000.0));
    grudginess_caused_by_damage = bangness / 10 + 50 * CAR_SPEC_IS_ROZZER(pTwattee);
    DoNotDprintf("Frame %0.6d: %s hit %s, damage %f, bangness %d, gBig_bang %d, grudginess %d",
        gAcme_frame_count,
        pTwatter->driver_name,
        pTwattee->driver_name,
        damage,
        bangness,
        gBig_bang,
        grudginess_caused_by_damage);
    if (bangness < 5) {
        return;
    }
    pTwatter->last_collision_time = gTime_stamp_for_this_munging;
    pTwatter->last_person_we_hit = pTwattee;
    pTwattee->last_collision_time = gTime_stamp_for_this_munging;
    pTwattee->last_person_to_hit_us = pTwatter;
    pTwattee->grudge_raised_recently = 1;
    if (bangness >= gBig_bang || CAR_SPEC_IS_ROZZER(pTwattee)) {
        pTwattee->big_bang = 1;
    }
    if (bangness >= 80) {
        pTwattee->scary_bang = 1;
    }
    if (pTwatter != NULL && pTwatter->driver == eDriver_local_human) {
        twattee_opponent_spec = GetOpponentSpecFromCarSpec(pTwattee);
        if (pTwattee->scary_bang && gOpponents[twattee_opponent_spec->index].strength_rating < 5) {
            StunTheBugger(twattee_opponent_spec, 30 * MIN(100, bangness) + 1000);
        }
        twattee_index = twattee_opponent_spec->index;
        new_grudge_value = grudginess_caused_by_damage + gOpponents[twattee_index].psyche.grudge_against_player;
        if (new_grudge_value > 100) {
            new_grudge_value = 100;
        }
        gOpponents[twattee_index].psyche.grudge_against_player = new_grudge_value;
    } else if (pTwattee != NULL && pTwattee->driver == eDriver_local_human) {
        twatter_opponent_spec = GetOpponentSpecFromCarSpec(pTwatter);
        if (twatter_opponent_spec->current_objective == eOOT_pursue_and_twat && twatter_opponent_spec->pursue_car_data.pursuee == pTwattee) {
            twatter_opponent_spec->pursue_car_data.time_last_twatted_em = gTime_stamp_for_this_munging;
        }
        twatter_index = twatter_opponent_spec->index;
        new_grudge_value = gOpponents[twatter_index].psyche.grudge_against_player - (twatter_opponent_spec->current_objective == eOOT_pursue_and_twat ? 0 : 2 * grudginess_caused_by_damage);
        if (new_grudge_value < 0) {
            new_grudge_value = 0;
        }
        gOpponents[twatter_index].psyche.grudge_against_player = new_grudge_value;
    } else {
        twatter_opponent_spec = GetOpponentSpecFromCarSpec(pTwatter);
        twattee_opponent_spec = GetOpponentSpecFromCarSpec(pTwattee);
        if (pTwattee->scary_bang && gOpponents[twattee_opponent_spec->index].strength_rating < 5) {
            StunTheBugger(twattee_opponent_spec, 30 * bangness + 1000);
        }
        twatter_index = twatter_opponent_spec->index;
        twattee_index = twattee_opponent_spec->index;
        if (twatter_opponent_spec->current_objective == eOOT_pursue_and_twat && twatter_opponent_spec->pursue_car_data.pursuee == pTwattee) {
            twatter_opponent_spec->pursue_car_data.time_last_twatted_em = gTime_stamp_for_this_munging;
        }
        if (CAR_SPEC_IS_OPPONENT(pTwatter) && CAR_SPEC_IS_ROZZER(pTwattee)) {
            new_grudge_value = grudginess_caused_by_damage + gOpponents[twattee_index].psyche.grudge_against_player;
            if (new_grudge_value > 100) {
                new_grudge_value = 100;
            }
            gOpponents[twattee_index].psyche.grudge_against_player = new_grudge_value;
        }
    }
}

// GetOpponentMood

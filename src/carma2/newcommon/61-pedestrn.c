/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "61-pedestrn.h"

#include "07-structur.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "23-piping.h"
#include "27-powerup.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "41-utility.h"
#include "42-input.h"
#include "43-pratcam.h"
#include "49-physics.h"
#include "51-smash.h"
#include "52-errors.h"
#include "54-runtime.h"
#include "61-pedestrn-phys.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "brender/brender.h"
#include "globvrkm.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_stdlib.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006a0420
// GLOBAL: CARMA2_SW 0x0069a478
// GLOBAL: CARMA2_CHEAT 0x006a7f98
int gPed_count_limbed_actors;

// GLOBAL: CARMA2_HW 0x006a0424
// GLOBAL: CARMA2_SW 0x0069a47c
// GLOBAL: CARMA2_CHEAT 0x006a7f9c
br_actor* gLimbs_actor;

// GLOBAL: CARMA2_HW 0x00694468
// GLOBAL: CARMA2_SW 0x0068e4c0
// GLOBAL: CARMA2_CHEAT 0x0069bfe0
tSmashable_initial_position_spec gGiblet_initial_position;

// GLOBAL: CARMA2_HW 0x0069411c
// GLOBAL: CARMA2_SW 0x0068e174
// GLOBAL: CARMA2_CHEAT 0x0069bc94
float gGiblet_scrape_start;

// GLOBAL: CARMA2_HW 0x00694238
// GLOBAL: CARMA2_SW 0x0068e290
// GLOBAL: CARMA2_CHEAT 0x0069bdb0
tSlick_spec gPed_slick;

// GLOBAL: CARMA2_HW 0x0069445c
// GLOBAL: CARMA2_SW 0x0068e4b4
// GLOBAL: CARMA2_CHEAT 0x0069bfd4
int gPed_spurting_period;

// GLOBAL: CARMA2_HW 0x00694460
// GLOBAL: CARMA2_SW 0x0068e4b8
// GLOBAL: CARMA2_CHEAT 0x0069bfd8
br_material* gPed_smear_material;

// GLOBAL: CARMA2_HW 0x00694478
// GLOBAL: CARMA2_SW 0x0068e4d0
// GLOBAL: CARMA2_CHEAT 0x0069bff0
tExplosion_animation gPed_explosion_small_blood_cloud;

// GLOBAL: CARMA2_HW 0x00694480
// GLOBAL: CARMA2_SW 0x0068e4d8
// GLOBAL: CARMA2_CHEAT 0x0069bff8
tExplosion_animation gPed_medium_blood_cloud;

// GLOBAL: CARMA2_HW 0x00694488
// GLOBAL: CARMA2_SW 0x0068e4e0
// GLOBAL: CARMA2_CHEAT 0x0069c000
tExplosion_animation gPed_large_blood_cloud;

// GLOBAL: CARMA2_HW 0x0069bc38
// GLOBAL: CARMA2_SW 0x00695c90
// GLOBAL: CARMA2_CHEAT 0x006a37b0
tMovement_spec gGiblet_initial_speed;

// GLOBAL: CARMA2_HW 0x006a0408
// GLOBAL: CARMA2_SW 0x0069a460
// GLOBAL: CARMA2_CHEAT 0x006a7f80
tShrapnel_time_info gGiblet_min_max_time;

// GLOBAL: CARMA2_HW 0x0069bc18
// GLOBAL: CARMA2_SW 0x00695c70
// GLOBAL: CARMA2_CHEAT 0x006a3790
int gGiblet_size_count;

// GLOBAL: CARMA2_HW 0x0065d7f0
// GLOBAL: CARMA2_SW 0x00658128
// GLOBAL: CARMA2_CHEAT 0x00664ea8
int gPed_move_fsm[118][7] = {
    {   0,  -2,   5,   6, 111,  31, 112 },
    {   0,   0,   5,   6, 111,  31, 112 },
    {   0,   1,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,   2,   2,   6, 111,  31, 112 },
    {   0,   2,   2,   6, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,  10,   5,   6, 111,  31, 112 },
    {   0,   2,   5,   6, 111,  31, 112 },
    {   0,  10,   5,   6, 111,  31, 112 },
    {   0,   1,   5,   6, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -2,  50,  51, 111,  31, 112 },
    {   1,  30,  31,  31, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -2,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  52,  50,  51, 111,  31, 112 },
    {   1,  52,  50,  51, 111,  31, 112 },
    {   1,  40,  50,  51, 111,  31, 112 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  72,  71,  71,  71,  71,  71 },
    {   1,  -2,  72,  72,  72,  72,  72 },
    {   1,  -1,  -1,  -1,  -1,  -1,  -1 },
    {   1,  -2,  74,  74,  74,  74,  74 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -2,  82,  83,  80,  80,  80 },
    {   0,  -2,  84,  85,  81,  81,  81 },
    {   0,  80,  82,  83,  80,  80,  80 },
    {   0,  80,  82,  83,  80,  80,  80 },
    {   0,  81,  84,  85,  81,  81,  81 },
    {   0,  81,  84,  85,  81,  81,  81 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  91,  90,  90,  90,  90,  90 },
    {   0,  -2,  91,  91,  91,  91,  91 },
    {   0,  91,  92,  92,  92,  92,  92 },
    {   0,  94,  93,  93,  93,  93,  93 },
    {   0,  -2,  94,  94,  94,  94,  94 },
    {   0,  -2,  95,  95,  95,  95,  95 },
    {   0,  91,  96,  96,  96,  96,  96 },
    {   0,  72,  97,  97,  97,  97,  97 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   0,  -3,  -3,  -3,  -3,  -3,  -3 },
    {   1,  72, 110, 110, 110, 110, 110 },
    {   1,  -2, 111, 111, 111,  31, 112 },
    {   1,  -2, 112, 112, 111,  31, 112 },
    {   0,  91, 113, 113, 113, 113, 113 },
    {   2,   2,   2,   2,   2,   2,   3 },
    {   4,   2,   2,   2,  10,  11,  13 },
    { -41, -41, -41,   0,   2,   2,   2 },
    {   2,   2,   3,   4,   4,  10,  11 },
};

// GLOBAL: CARMA2_HW 0x0067697c
// GLOBAL: CARMA2_SW 0x006710ec
// GLOBAL: CARMA2_CHEAT 0x0067e22c
tPedForms_vtable* gPed_vtable = NULL;

// GLOBAL: CARMA2_HW 0x00677234
// GLOBAL: CARMA2_SW 0x006719a4
// GLOBAL: CARMA2_CHEAT 0x0067eae4
int gPed_retain_root_mode = 0;

// GLOBAL: CARMA2_HW 0x00676980
// GLOBAL: CARMA2_SW 0x006710f0
// GLOBAL: CARMA2_CHEAT 0x0067e230
tPed_move* gPed_moves[250] = { 0 };

// GLOBAL: CARMA2_HW 0x00676d68
// GLOBAL: CARMA2_SW 0x006714d8
// GLOBAL: CARMA2_CHEAT 0x0067e618
br_material* gPedestrian_character_cloned_materials[50] = { 0 };

// GLOBAL: CARMA2_HW 0x0067772c
// GLOBAL: CARMA2_SW 0x00671e9c
// GLOBAL: CARMA2_CHEAT 0x0067efdc
int gCount_pedestrian_personality_cloned_materials;

// GLOBAL: CARMA2_HW 0x00677730
// GLOBAL: CARMA2_SW 0x00671ea0
// GLOBAL: CARMA2_CHEAT 0x0067efe0
tPed_morph gPed_morphs[20];

// GLOBAL: CARMA2_HW 0x00677238
// GLOBAL: CARMA2_SW 0x006719a8
// GLOBAL: CARMA2_CHEAT 0x0067eae8
tPed_remap* gPed_remaps[10] = { 0 };

// GLOBAL: CARMA2_HW 0x00677260
// GLOBAL: CARMA2_SW 0x006719d0
// GLOBAL: CARMA2_CHEAT 0x0067eb10
tPed_personality* gPed_personalities[50] = { 0 };

// GLOBAL: CARMA2_HW 0x00677328
// GLOBAL: CARMA2_SW 0x00671a98
// GLOBAL: CARMA2_CHEAT 0x0067ebd8
float gPed_sin_table[256] = { 0 };

// GLOBAL: CARMA2_HW 0x007447e0
// GLOBAL: CARMA2_SW 0x0073b2a0
// GLOBAL: CARMA2_CHEAT 0x0074be80
float gPedestrian_speed_factor;

// GLOBAL: CARMA2_HW 0x007447ec
// GLOBAL: CARMA2_SW 0x0073b2ac
// GLOBAL: CARMA2_CHEAT 0x0074be8c
int gCount_active_peds;

// GLOBAL: CARMA2_HW 0x007447f0
// GLOBAL: CARMA2_SW 0x0073b2b0
// GLOBAL: CARMA2_CHEAT 0x0074be90
int gSelected_ped;

// GLOBAL: CARMA2_HW 0x007447ac
// GLOBAL: CARMA2_SW 0x0073b26c
// GLOBAL: CARMA2_CHEAT 0x0074be4c
int gPeds_suicidal;

// GLOBAL: CARMA2_HW 0x007447b0
// GLOBAL: CARMA2_SW 0x0073b270
// GLOBAL: CARMA2_CHEAT 0x0074be50
int gExploding_pedestrians;

// GLOBAL: CARMA2_HW 0x007447c0
// GLOBAL: CARMA2_SW 0x0073b280
// GLOBAL: CARMA2_CHEAT 0x0074be60
br_vector3 gPed_scale;

// GLOBAL: CARMA2_HW 0x0069bc24
// GLOBAL: CARMA2_SW 0x00695c7c
// GLOBAL: CARMA2_CHEAT 0x006a379c
float gAverage_ped_scale;

// GLOBAL: CARMA2_HW 0x006a03f8
// GLOBAL: CARMA2_SW 0x0069a450
// GLOBAL: CARMA2_CHEAT 0x006a7f70
tPed_face_cache_0x34* gPed_face_caches[4];

// GLOBAL: CARMA2_HW 0x006a0410
// GLOBAL: CARMA2_SW 0x0069a468
// GLOBAL: CARMA2_CHEAT 0x006a7f88
int gPipe_halted_ped_status;

// GLOBAL: CARMA2_HW 0x006a041c
// GLOBAL: CARMA2_SW 0x0069a474
// GLOBAL: CARMA2_CHEAT 0x006a7f94
int gPed_valium_left;

// GLOBAL: CARMA2_HW 0x006944c0
// GLOBAL: CARMA2_SW 0x0068e518
// GLOBAL: CARMA2_CHEAT 0x0069c038
tPed_cache_006944c0 gPed_cache_006944c0[100] = { 0 };

// GLOBAL: CARMA2_HW 0x0069b9f0
// GLOBAL: CARMA2_SW 0x00695a48
// GLOBAL: CARMA2_CHEAT 0x006a3568
tNapalm_bolt gNapalm_bolts[5] = { 0 };

// GLOBAL: CARMA2_HW 0x0069bc0c
// GLOBAL: CARMA2_SW 0x00695c64
// GLOBAL: CARMA2_CHEAT 0x006a3784
int gPed_severing_chance = 0;

// GLOBAL: CARMA2_HW 0x0069bc10
// GLOBAL: CARMA2_SW 0x00695c68
// GLOBAL: CARMA2_CHEAT 0x006a3788
br_material* gPed_slick_material = NULL;

// GLOBAL: CARMA2_HW 0x0069fd88
// GLOBAL: CARMA2_SW 0x00699de0
// GLOBAL: CARMA2_CHEAT 0x006a7900
br_material* gNapalm_bolt_materials[7] = { 0 };

// GLOBAL: CARMA2_HW 0x0069fda4
int gINT_0069fda4 = 0;

// GLOBAL: CARMA2_HW 0x00694308
// GLOBAL: CARMA2_SW 0x0068e360
// GLOBAL: CARMA2_CHEAT 0x0069be80
br_model* gNapalm_bolt_models[7];

// GLOBAL: CARMA2_HW 0x0065e598
// GLOBAL: CARMA2_SW 0x00658ed0
// GLOBAL: CARMA2_CHEAT 0x00665c50
const char* gNapalm_bolt_texture_names[7] = {
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
    "Ex00006",
};

// GLOBAL: CARMA2_HW 0x0069412c
// GLOBAL: CARMA2_SW 0x0068e184
// GLOBAL: CARMA2_CHEAT 0x0069bca4
int gCount_changed_points;

// GLOBAL: CARMA2_HW 0x0069bc68
// GLOBAL: CARMA2_SW 0x00695cc0
// GLOBAL: CARMA2_CHEAT 0x006a37e0
br_vector3 gChanged_points[10];

// GLOBAL: CARMA2_HW 0x00694454
// GLOBAL: CARMA2_SW 0x0068e4ac
// GLOBAL: CARMA2_CHEAT 0x0069bfcc
int gPed_dismember_spurting_member_count;

// GLOBAL: CARMA2_HW 0x00694490
// GLOBAL: CARMA2_SW 0x0068e4e8
// GLOBAL: CARMA2_CHEAT 0x0069c008
int gPed_nearness;

// GLOBAL: CARMA2_HW 0x00694498
// GLOBAL: CARMA2_SW 0x0068e4f0
// GLOBAL: CARMA2_CHEAT 0x0069c010
tTWTVFS gTwtPeds;

// GLOBAL: CARMA2_HW 0x006944a0
// GLOBAL: CARMA2_SW 0x0068e4f8
// GLOBAL: CARMA2_CHEAT 0x0069c018
tMovement_spec gPed_spurt_initial_speed;

// GLOBAL: CARMA2_HW 0x0069fda8
// GLOBAL: CARMA2_SW 0x00699e00
// GLOBAL: CARMA2_CHEAT 0x006a7920
tSmashable_initial_position_spec gPed_spurt_initial_position;

// GLOBAL: CARMA2_HW 0x0069bc30
// GLOBAL: CARMA2_SW 0x00695c88
// GLOBAL: CARMA2_CHEAT 0x006a37a8
int gPed_recent_points;

// GLOBAL: CARMA2_HW 0x0069bce4
// GLOBAL: CARMA2_SW 0x00695d3c
// GLOBAL: CARMA2_CHEAT 0x006a385c
int gPed_score_multiplier;

// GLOBAL: CARMA2_HW 0x0069bcf8
// GLOBAL: CARMA2_SW 0x00695d50
int gPed_points[3];

// GLOBAL: CARMA2_HW 0x006a0418
// GLOBAL: CARMA2_SW 0x0069a470
// GLOBAL: CARMA2_CHEAT 0x006a7f90
tU32 gDAT_006a0418;

// GLOBAL: CARMA2_HW 0x007447a4
// GLOBAL: CARMA2_SW 0x0073b264
// GLOBAL: CARMA2_CHEAT 0x0074be44
float gMutant_speed;

// GLOBAL: CARMA2_HW 0x007447a8
// GLOBAL: CARMA2_SW 0x0073b268
// GLOBAL: CARMA2_CHEAT 0x0074be48
int gDancing_peds;

// GLOBAL: CARMA2_HW 0x007447b8
// GLOBAL: CARMA2_SW 0x0073b278
// GLOBAL: CARMA2_CHEAT 0x0074be58
int gDrunk_pedestrians;

// GLOBAL: CARMA2_HW 0x0069bce0
// GLOBAL: CARMA2_SW 0x00695d38
// GLOBAL: CARMA2_CHEAT 0x006a3858
int gPeds_already_munged;

// GLOBAL: CARMA2_HW 0x0065d7b8
// GLOBAL: CARMA2_SW 0x006580f0
// GLOBAL: CARMA2_CHEAT 0x00664e70
int gPed_cache_sizes_1[4] = { 40, 25, 15, 5 };

// GLOBAL: CARMA2_HW 0x00694120
// GLOBAL: CARMA2_SW 0x0068e178
// GLOBAL: CARMA2_CHEAT 0x0069bc98
tPedestrian* gPTR_00694120;

// GLOBAL: CARMA2_HW 0x0069bc58
// GLOBAL: CARMA2_SW 0x00695cb0
// GLOBAL: CARMA2_CHEAT 0x006a37d0
tPed_face_cache_0x34* gPed_face_cache_heads[4];

// GLOBAL: CARMA2_HW 0x00694134
// GLOBAL: CARMA2_SW 0x0068e18c
// GLOBAL: CARMA2_CHEAT 0x0069bcac
int gPed_groups_count;

// GLOBAL: CARMA2_HW 0x00694138
// GLOBAL: CARMA2_SW 0x0068e190
// GLOBAL: CARMA2_CHEAT 0x0069bcb0
tRace_ped_spec* gRace_pedestrian_specs;

// GLOBAL: CARMA2_HW 0x0069bc14
// GLOBAL: CARMA2_SW 0x00695c6c
// GLOBAL: CARMA2_CHEAT 0x006a378c
tPed_group_spec* gPed_groups;

// GLOBAL: CARMA2_HW 0x00694124
// GLOBAL: CARMA2_SW 0x0068e17c
// GLOBAL: CARMA2_CHEAT 0x0069bc9c
int gPed_cache_notify_flags;

// GLOBAL: CARMA2_HW 0x0069bce8
// GLOBAL: CARMA2_SW 0x00695d40
// GLOBAL: CARMA2_CHEAT 0x006a3860
int gCount_race_pedestrian_specs;

// GLOBAL: CARMA2_HW 0x0069bcec
// GLOBAL: CARMA2_SW 0x00695d44
// GLOBAL: CARMA2_CHEAT 0x006a3864
int gPed_movements_count;

// GLOBAL: CARMA2_HW 0x0069bcf0
// GLOBAL: CARMA2_SW 0x00695d48
// GLOBAL: CARMA2_CHEAT 0x006a3868
tPed_movement_spec* gPed_movements;


inline void C2_HOOK_FASTCALL ClearOutMorphs(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_morphs); i++) {
        gPed_morphs[i].field_0x4 = 0.f;
    }
}

// InitBoner

// DRVector3SafeCross

// DRVector3SafeCrossXZ

// FUNCTION: CARMA2_HW 0x00404290
// FUNCTION: CARMA2_SW 0x00404290
// FUNCTION: CARMA2_CHEAT 0x00404290
void C2_HOOK_FASTCALL ScaleModelXYZ(br_model* pModel, br_scalar pScale_x, br_scalar pScale_y, br_scalar pScale_z) {
    int i;
    br_vertex* v;

    v = pModel->vertices;
    for (i = 0; i < pModel->nvertices; i++, v++) {
        v->p.v[0] *= pScale_x;
        v->p.v[1] *= pScale_y;
        v->p.v[2] *= pScale_z;
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x004042f0
// FUNCTION: CARMA2_SW 0x004042f0
// FUNCTION: CARMA2_CHEAT 0x004042f0
void C2_HOOK_FASTCALL ScaleModel(br_model* pModel, float pScale) {

    ScaleModelXYZ(pModel, pScale, pScale, pScale);
}

inline void C2_HOOK_FASTCALL DisposeMove(tPed_move* pMove) {
    int i;

    for (i = 0; i < pMove->count_frames; i++) {
        if (pMove->frames[i].axis != NULL) {
            BrMemFree(pMove->frames[i].axis);
        }
    }
    if (pMove->frames != NULL) {
        BrMemFree(pMove->frames);
    }
    if (pMove != NULL) {
        BrMemFree(pMove);
    }
}

inline void C2_HOOK_FASTCALL DisposeAllLoadedMoves(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_moves); i++) {
        if (gPed_moves[i] != NULL) {
            DisposeMove(gPed_moves[i]);
            gPed_moves[i] = NULL;
        }
    }
}

// ReadRemap

// FUNCTION: CARMA2_HW 0x004046d0
// FUNCTION: CARMA2_SW 0x004046d0
void C2_HOOK_FASTCALL DisposeRemap(tPed_remap* pRemap) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_remap, bones, 0x2c);

    if (pRemap != NULL) {
        if (pRemap->bones != NULL) {
            BrMemFree(pRemap->bones);
        }
        BrMemFree(pRemap);
    }
}

// FUNCTION: CARMA2_HW 0x00404700
// FUNCTION: CARMA2_SW 0x00404700
void C2_HOOK_FASTCALL DisposeAllLoadedRemaps(void) {
    int i;

    for (i = 0; i < 10; i++) {
        if (gPed_remaps[i] != NULL) {
            DisposeRemap(gPed_remaps[i]);
            gPed_remaps[i] = NULL;
        }
    }
}

// FindOrOpenRemap

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00404f60
// STUB: CARMA2_SW 0x00404f60
tPed_form* C2_HOOK_FASTCALL SetUpCharacterForm(const char* pName) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x004067e0
// FUNCTION: CARMA2_SW 0x004067e0
// FUNCTION: CARMA2_CHEAT 0x004067e0
void C2_HOOK_FASTCALL DisposeCharacterForm(tPed_form* pForm) {
    int i;
    int j;
    tPhysics_object* object;

    if (gPed_vtable->dispose_form != NULL) {
        gPed_vtable->dispose_form(pForm);
    }
    for (i = 0; i < pForm->max_rendering_at_once; i++) {
        for (j = 0; j < pForm->count_bones; j++) {
            if (IsActorRenderworthy(pForm->actor_sets[i].actors[j])) {
                StopActorBeingRenderable(pForm->actor_sets[i].actors[j]);
            }
            BrActorFree(pForm->actor_sets[i].actors[j]);
        }
        if (pForm->actor_sets[i].actors != NULL) {
            BrMemFree(pForm->actor_sets[i].actors);
        }
    }
    for (i = 0; i < pForm->count_bones; i++) {
        if (pForm->boned_physicing[0].collision_infos[i] != NULL) {
            if (pForm->bones[i].hinge != NULL) {
                FreePhysicsJoint(pForm->bones[i].hinge);
            }
            for (j = 0; j < pForm->max_boned_physicing_at_once; j++) {
                if (pForm->boned_physicing[j].collision_infos[i]->shape != NULL) {
                    FreeShape(pForm->boned_physicing[j].collision_infos[i]->shape);
                }
                if (pForm->boned_physicing[j].collision_infos[i] != NULL) {
                    object = pForm->boned_physicing[j].collision_infos[i];
                    if (object != NULL) {
                        BrMemFree(object);
                    }
                }
            }
        }
    }
    for (i = 0; i < pForm->max_boned_physicing_at_once; i++) {
        if (pForm->boned_physicing[i].collision_infos != NULL) {
            BrMemFree(pForm->boned_physicing[i].collision_infos);
        }
    }
    for (i = 0; i < pForm->max_simple_physicing_at_once; i++) {
        FreeShape(pForm->simple_physicing[i].collision_info->shape);
        if (pForm->simple_physicing[i].collision_info != NULL) {
            BrMemFree(pForm->simple_physicing[i].collision_info);
        }
    }
    for (i = 0; i < pForm->max_stored_dismembered_characters; i++) {
        if (pForm->stored_dismembered_characters[i] != NULL) {
            BrMemFree(pForm->stored_dismembered_characters[i]);
        }
    }
    if (pForm->stored_dismembered_characters != NULL) {
        BrMemFree(pForm->stored_dismembered_characters);
    }
    if (pForm->moves != NULL) {
        BrMemFree(pForm->moves);
    }
    if (pForm->actor_sets != NULL) {
        BrMemFree(pForm->actor_sets);
    }
    if (pForm->simple_physicing != NULL) {
        BrMemFree(pForm->simple_physicing);
    }
    if (pForm->boned_physicing != NULL) {
        BrMemFree(pForm->boned_physicing);
    }
    if (pForm->bones != NULL) {
        BrMemFree(pForm->bones);
    }
    if (pForm != NULL) {
        BrMemFree(pForm);
    }
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

inline void C2_HOOK_FASTCALL DisposeAllLoadedForms(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_forms); i++) {
        if (gPed_forms[i] != NULL) {
            DisposeCharacterForm(gPed_forms[i]);
            gPed_forms[i] = NULL;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00406a30
// FUNCTION: CARMA2_SW 0x00406a30
// FUNCTION: CARMA2_CHEAT 0x00406a30
void C2_HOOK_FASTCALL DisposeCharacterInstance(tPed_character_instance* pCharacter) {

    if (pCharacter != NULL) {
        BrMemFree(pCharacter);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00406a40
// FUNCTION: CARMA2_SW 0x00406a40
tPed_form* C2_HOOK_FASTCALL FindOrOpenForm(const char* pName) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_forms) != 20);
    for (i = 0; i < (int)REC2_ASIZE(gPed_forms); i++) {
        if (gPed_forms[i] != NULL) {
            if (strcmp(pName, gPed_forms[i]->name) == 0) {
                return gPed_forms[i];
            }
        }
    }
    return SetUpCharacterForm(pName);
}

// MakeRotationMatrix

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00406ab0
// STUB: CARMA2_SW 0x00406ab0
// STUB: CARMA2_CHEAT 0x00406ab0
tPed_personality* C2_HOOK_FASTCALL ReadPersonality(const char* pName) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004077f0
// STUB: CARMA2_SW 0x004077f0
// STUB: CARMA2_CHEAT 0x004077f0
void C2_HOOK_FASTCALL DisposePersonality(tPed_personality* pPersonality) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL DisposeAllLoadedPersonalities(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPed_personalities); i++) {
        if (gPed_personalities[i] != NULL) {
            DisposePersonality(gPed_personalities[i]);
            gPed_personalities[i] = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00407930
// FUNCTION: CARMA2_SW 0x00407920
// FUNCTION: CARMA2_CHEAT 0x00407920
tPed_personality* C2_HOOK_FASTCALL FindOrOpenPersonality(const char* pName) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_personalities) != 50);
    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_sin_table) != 256);

    for (i = 0; i < (int)REC2_ASIZE(gPed_personalities); i++) {
        if (gPed_personalities[i] == NULL) {
            continue;
        }
        if (strcmp(pName, gPed_personalities[i]->name) == 0) {
            return gPed_personalities[i];
        }
    }
    return ReadPersonality(pName);
}

// FUNCTION: CARMA2_HW 0x004079a0
// FUNCTION: CARMA2_SW 0x00407990
// FUNCTION: CARMA2_CHEAT 0x00407990
tPed_character_instance* C2_HOOK_FASTCALL BuildCharacterInstance(const char* pGroup_name, br_matrix34* pMat34) {
    tPed_character_instance* instance;

    C2_HOOK_BUG_ON(sizeof(tPed_character_instance) != 0xec);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x6, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x7, 0x7);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x9, 0x9);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0xa, 0xa);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x28, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x2c, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x5c, 0x5c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x8c, 0x8c);

    instance = BrMemAllocate(sizeof(tPed_character_instance), kBoner_mem_type_char_instance);
    if (instance == NULL) {
        FatalError(kFatalError_BonerError_UnableToAllocateMemory, 0);
    }
    instance->personality = FindOrOpenPersonality(pGroup_name);
    instance->field_0x4 = -1;
    instance->field_0x7 = -1;
    instance->field_0x6 = -1;
    instance->field_0x28 = 0;
    instance->field_0x2c = *pMat34;
    instance->field_0x8 = -1;
    instance->field_0x9 = 0;
    instance->field_0xa = 0;
    instance->field_0x14 = 0;
    instance->field_0xc = 0;
    BrMatrix34Identity(&instance->field_0x5c);
    BrMatrix34Identity(&instance->field_0x8c);
    return instance;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00407aa0
// FUNCTION: CARMA2_SW 0x00407a90
// FUNCTION: CARMA2_CHEAT 0x00407a90
inline br_matrix34* C2_HOOK_FASTCALL GetCharacterMatrixPtr(tPed_character_instance* pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return pCharacter->field_0xe8;
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0]->t.t.mat;
}

#ifdef REC2_MATCHING
// #pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00407ad0
// FUNCTION: CARMA2_SW 0x00407ac0
// FUNCTION: CARMA2_CHEAT 0x00407ac0
inline br_matrix34* C2_HOOK_FASTCALL GetBoneMatrixPtr(tPed_character_instance* pCharacter, int pBone_index) {
    if (pCharacter->field_0x4 < 0) {
        return &pCharacter->field_0x2c;
    }
    if (pCharacter->field_0xe8 != NULL) {
        return &pCharacter->field_0xe8[pBone_index];
    }
    return &pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->t.t.mat;
}

// FUNCTION: CARMA2_HW 0x00407b10
// FUNCTION: CARMA2_SW 0x00407b00
// FUNCTION: CARMA2_CHEAT 0x00407b00
br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(const tPed_character_instance* pCharacter) {

    if (pCharacter->field_0x4 < 0) {
        return NULL;
    }
    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[0];
}

#ifdef REC2_MATCHING
// #pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00407b30
// STUB: CARMA2_SW 0x00407b20
// STUB: CARMA2_CHEAT 0x00407b20
void C2_HOOK_FASTCALL SetCharacterBonePositions(tPed_character_instance* pPed, int pArg2, int pArg3) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

#if defined(REC2_MATCHING)
// #pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x00408200
// FUNCTION: CARMA2_SW 0x00408200
// FUNCTION: CARMA2_CHEAT 0x00408200
int C2_HOOK_FASTCALL MakeCharacterRenderable2(tPed_character_instance* pCharacter, int pIndex) {
    tPed_personality* personality;
    tPed_form* form;
    tPed_form_actor_set* actor_set;
    // br_matrix34* matrix;
    int i;

    if (pCharacter->field_0x4 >= 0) {
        return 2;
    }
    personality = pCharacter->personality;
    form = personality->form;
    actor_set = &form->actor_sets[pIndex];
    if (!gAction_replay_mode) {
        actor_set->field_0x0 = 1;
    }
    pCharacter->field_0x4 = pIndex;
    BrMatrix34Copy(GetCharacterMatrixPtr(pCharacter), &pCharacter->field_0x2c);
    if (pCharacter->field_0xc != 0) {
        if (pCharacter->field_0x28 != NULL) {
            for (i = 1; i < form->count_bones; i++) {
                if (pCharacter->field_0xc & gPow2_array[i]) {
                    BrMatrix34Copy(GetBoneMatrixPtr(pCharacter, i), &pCharacter->field_0x28[i]);
                }
            }
            pCharacter->field_0x28->m[0][0] = 0.0f;
        } else {
            pCharacter->field_0x10 = pCharacter->field_0xc;
        }
    }
    for (i = 0; i < form->count_bones; i++) {
        actor_set->actors[i]->type_data = pCharacter;
        actor_set->actors[i]->render_style = BR_RSTYLE_DEFAULT;
        actor_set->actors[i]->model = personality->bones[i].models->models[pCharacter->field_0xa];
        if (actor_set->actors[i]->model != NULL) {
            actor_set->actors[i]->type = BR_ACTOR_MODEL;
        } else {
            actor_set->actors[i]->type = BR_ACTOR_NONE;
        }
        if (!((pCharacter->field_0xc & gPow2_array[i]) && (pCharacter->field_0x10 & gPow2_array[i]))) {
            MakeActorRenderable(actor_set->actors[i], i, pCharacter);
        }
    }
    SetCharacterBonePositions(pCharacter, 0, 0);
    return 0;
}
#if defined(REC2_MATCHING)
// #pragma inline_depth()
#endif


#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004083b0
// FUNCTION: CARMA2_SW 0x004083b0
// FUNCTION: CARMA2_CHEAT 0x004083b0
int C2_HOOK_FASTCALL MakeCharacterRenderable(tPed_character_instance* pCharacter) {
    int i;
    tPed_form_actor_set* actor_set;

    if (pCharacter->field_0x4 >= 0) {
        return 2;
    }
    for (i = 0, actor_set = pCharacter->personality->form->actor_sets; i < pCharacter->personality->form->max_rendering_at_once; i++, actor_set++) {
        if (!actor_set->field_0x0) {
            break;
        }
    }
    if (i == pCharacter->personality->form->max_rendering_at_once) {
        return 1;
    }
    return MakeCharacterRenderable2(pCharacter, i);
}

// CharacterNoLongerRenderable

// FUNCTION: CARMA2_HW 0x004087b0
// FUNCTION: CARMA2_SW 0x004087b0
// FUNCTION: CARMA2_CHEAT 0x004087b0
void C2_HOOK_FASTCALL CalcCollisionObjectThings(tPhysics_object* pObject, br_bounds3* pFrame, int pNull, br_vector3* pBoneVec, br_vector3* pBonePtr) {
    br_scalar maxcomp;
    br_vector3 P;
    br_vector3 Q;
    br_vector3 dmax;
    br_vector3 d;
    br_vector3 tv;
    int index;
    int r1;
    int r2;
    int i;

    if (pFrame != NULL) {
        BrVector3Sub(&d, &pFrame->max, &pFrame->min);
        BrVector3Copy(&dmax, &d);
    } else {
#ifdef REC2_CHEAT
        if (pObject->shape->common.type != kCollisionShapeType_Wireframe || pBoneVec == NULL) {
            PDFatalError("Can't make shape for object with no model unless its a wireframe");
        }
#endif
    }
    switch (pObject->shape->common.type) {
    case kCollisionShapeType_Box:
        pObject->shape->common.bb = *pFrame;
        break;
    case kCollisionShapeType_Wireframe:
        if (pBoneVec != NULL && pBonePtr != NULL) {
            BrVector3Copy(&pObject->shape->wireframe.wireframe.points[0], pBoneVec);
            BrVector3Copy(&pObject->shape->wireframe.wireframe.points[1], pBonePtr);
        } else {
            maxcomp = -1.0f;
            index = -1;
            for (i = 0; i < 3; i++) {
                if (maxcomp < dmax.v[i]) {
                    maxcomp = dmax.v[i];
                    index = i;
                }
            }
            r1 = (index + 1) % 3;
            r2 = (index + 2) % 3;
            P.v[index] = pFrame->min.v[index];
            Q.v[index] = pFrame->max.v[index];
            P.v[r1] = (pFrame->min.v[r1] + pFrame->max.v[r1]) / 2.0f;
            Q.v[r1] = P.v[r1];
            P.v[r2] = (pFrame->min.v[r2] + pFrame->max.v[r2]) / 2.0f;
            Q.v[r2] = P.v[r2];
            if (pBoneVec == NULL && pBonePtr == NULL) {
                BrVector3Copy(&pObject->shape->wireframe.wireframe.points[0], &P);
                BrVector3Copy(&pObject->shape->wireframe.wireframe.points[1], &Q);
            } else {
                if (pBoneVec != NULL) {
                    BrVector3Copy(&tv, pBoneVec);
                } else {
                    BrVector3Copy(&tv, pBonePtr);
                }
                if (Vector3DistanceSquared(&tv, &P) > Vector3DistanceSquared(&tv, &Q)) {
                    BrVector3Copy(&pObject->shape->wireframe.wireframe.points[0], &P);
                    BrVector3Copy(&pObject->shape->wireframe.wireframe.points[1], &tv);
                } else {
                    BrVector3Copy(&pObject->shape->wireframe.wireframe.points[0], &Q);
                    BrVector3Copy(&pObject->shape->wireframe.wireframe.points[1], &tv);
                }
            }
        }
        break;
    case kCollisionShapeType_Sphere:
        pObject->shape->sphere.sphere.radius = (d.v[0] + d.v[1] + d.v[2]) / 6.0f;
        pObject->shape->sphere.sphere.radius_squared = REC2_SQR(pObject->shape->sphere.sphere.radius);
        pObject->shape->sphere.sphere.center.v[0] = pFrame->min.v[0] + d.v[0] / 2.0f;
        pObject->shape->sphere.sphere.center.v[1] = pFrame->min.v[1] + d.v[1] / 2.0f;
        pObject->shape->sphere.sphere.center.v[2] = pFrame->min.v[2] + d.v[2] / 2.0f;
        break;
    default:
        break;
    }
    FillInShape(pObject->shape);
    UpdateCollisionObject(pObject);
    if (pFrame == NULL) {
        pFrame = &pObject->shape->common.bb;
        BrVector3Sub(&d, &pFrame->max, &pFrame->min);
    }
    pObject->I.v[2] = pObject->M * (REC2_SQR(d.v[1]) + REC2_SQR(d.v[0])) / 40.0f;
    pObject->I.v[1] = pObject->M * (REC2_SQR(d.v[2]) + REC2_SQR(d.v[0])) / 40.0f;
    pObject->I.v[0] = pObject->M * (REC2_SQR(d.v[2]) + REC2_SQR(d.v[1])) / 40.0f;
    if (pNull != 0) {
        pObject->cmpos.v[0] = pFrame->min.v[0] + d.v[0] / 2.0f;
        pObject->cmpos.v[1] = pFrame->min.v[1] + d.v[1] / 2.0f;
        pObject->cmpos.v[2] = pFrame->min.v[2] + d.v[2] / 2.0f;
    }
}

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00408c30
// STUB: CARMA2_SW 0x00408c30
int C2_HOOK_FASTCALL MakeCharacterCollideworthy2(tPed_character_instance* pCharacter, int pArg2, int pArg3, int pArg4) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00409030
// FUNCTION: CARMA2_SW 0x00409030
// FUNCTION: CARMA2_CHEAT 0x00409060
int C2_HOOK_FASTCALL MakeCharacterCollideworthy(tPed_character_instance* pCharacter, int pArg2, int pArg3) {
    return MakeCharacterCollideworthy2(pCharacter, pArg2, pArg3, -1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00409040
// FUNCTION: CARMA2_SW 0x00409040
// FUNCTION: CARMA2_CHEAT 0x00409070
tPhysics_object* C2_HOOK_FASTCALL GetRootObject(tPed_character_instance* pPed) {
    tPed_form* form;
    tPed_form_boned_phys* boned;
    int i;

    form = pPed->personality->form;
    boned = &form->boned_physicing[(tS8)pPed->field_0x5];
    for (i = 0; i < form->count_bones; i++) {
        if (boned->collision_infos[i]->shape != NULL) {
            return boned->collision_infos[i];
        }
    }
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// CharacterNoLongerCollideworthy

// FUNCTION: CARMA2_HW 0x00409300
// FUNCTION: CARMA2_SW 0x00409300
void C2_HOOK_FASTCALL SetObjectV(tPhysics_object* pObject, const br_vector3* pV) {
    tPhysics_object* child;

    BrVector3Copy(&pObject->v, pV);
    for (child = pObject->child; child != NULL; child = child->next) {
        SetObjectV(child, pV);
    }
}

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

// MakeCharacterPhysicworthy

// FUNCTION: CARMA2_HW 0x00409340
// FUNCTION: CARMA2_SW 0x00409340
// FUNCTION: CARMA2_CHEAT 0x00409390
void C2_HOOK_FASTCALL RecalculateOrientationOfRoot(tPed_character_instance* pCharacter) {
    tPed_form* form;
    br_matrix34* character_matrix;
    br_matrix34 mat;

    form = pCharacter->personality->form;
    character_matrix = GetCharacterMatrixPtr(pCharacter);
    BrMatrix34LPInverse(&mat, &form->moves[pCharacter->field_0x7].move->frames[pCharacter->field_0x1c].mat);
    BrMatrix34Mul(&pCharacter->field_0x8c, &mat, character_matrix);
    BrVector3Set((br_vector3*)pCharacter->field_0x8c.m[3], 0.0f, 0.0f, 0.0f);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00409400
// STUB: CARMA2_SW 0x00409400
int C2_HOOK_FASTCALL BonerActiveHalted2(tPed_character_instance* pCharacter, undefined4 pArg2) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004093e0
// FUNCTION: CARMA2_SW 0x004093e0
int C2_HOOK_FASTCALL CharacterNoLongerPhysicworthy(tPed_character_instance* pCharacter) {

    if ((pCharacter->field_0x14 & 4) == 0) {
        return 0x1f;
    }
    BonerActiveHalted2(pCharacter, 1);
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00409570
// STUB: CARMA2_SW 0x00409570
// STUB: CARMA2_CHEAT 0x004095c0
void C2_HOOK_CDECL SetCharacterPhysicsLevel(tPed_character_instance* pCharacter, tU32 pLevel, ...) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004096f0
// FUNCTION: CARMA2_SW 0x004096f0
// FUNCTION: CARMA2_CHEAT 0x00409760
void C2_HOOK_FASTCALL StopCharacterMorphing(tPed_character_instance* pCharacter) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0x1c, 0x1c);

    if (pCharacter->field_0xbc != NULL) {
        pCharacter->field_0xbc->field_0x4 = 0.f;
        if (pCharacter->field_0xbc->field_0x2c) {
            RecalculateOrientationOfRoot(pCharacter);
        }
        pCharacter->field_0xbc = NULL;
        SetCharacterBonePositions(pCharacter, 0, 0);
    }
}

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040a050
// STUB: CARMA2_SW 0x0040a050
// STUB: CARMA2_CHEAT 0x0040a0c0
int C2_HOOK_FASTCALL SetCharacterMove(tPed_character_instance* pPed, int pMove_action, float pSpeed, int pArg4, int pArg5, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DRMatrix34RotateCos

// FUNCTION: CARMA2_HW 0x0040b030
// FUNCTION: CARMA2_SW 0x0040b020
// FUNCTION: CARMA2_CHEAT 0x0040b0a0
void C2_HOOK_FASTCALL DropPointOntoPlane(const br_vector3* pPoint, const br_vector3* pPlane, br_vector3* pDest) {
    float dot;

    dot = BrVector3Dot(pPlane, pPoint);
    BrVector3Scale(pDest, pPlane, -dot);
    BrVector3Accumulate(pDest, pPoint);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040a730
// STUB: CARMA2_SW 0x0040a730
// STUB: CARMA2_CHEAT 0x0040a7a0
void C2_HOOK_FASTCALL SetCharacterDirection(tPed_character_instance* pPed, const br_vector3* pDir, const br_vector3* pUp) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040b0a0
// STUB: CARMA2_SW 0x0040b090
// STUB: CARMA2_CHEAT 0x0040b110
void C2_HOOK_FASTCALL SetCharacterPosition(tPed_character_instance* pCharacter, br_vector3* pPos, int pArg3) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL DisposeClonedMaterials(void) {
    int i;

    for (i = 0; i < gCount_pedestrian_personality_cloned_materials; i++) {
        BrMaterialRemove(gPedestrian_character_cloned_materials[i]);
        BrMaterialFree(gPedestrian_character_cloned_materials[i]);
    }
    gCount_pedestrian_personality_cloned_materials = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040b2e0
// FUNCTION: CARMA2_SW 0x0040b2d0
// FUNCTION: CARMA2_CHEAT 0x0040b350
void C2_HOOK_FASTCALL DisposeAllBonerData(void) {

    DisposeAllLoadedRemaps();
    DisposeAllLoadedPersonalities();
    DisposeAllLoadedMoves();
    DisposeAllLoadedForms();
    ClearOutMorphs();
    DisposeClonedMaterials();
    gPed_overall_movement_disabled = 0;
}

// FUNCTION: CARMA2_HW 0x0040b420
// FUNCTION: CARMA2_SW 0x0040b410
// FUNCTION: CARMA2_CHEAT 0x0040b490
int C2_HOOK_FASTCALL SetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index, int pModel_index, int pModel_set) {

    C2_HOOK_BUG_ON(REC2_ASIZE(pCharacter->personality->bones[0].models) != 4);

    if (pModel_index >= (int)REC2_ASIZE(pCharacter->personality->bones[0].models)
            || pBone_index >= pCharacter->personality->form->count_bones
            || pBone_index < 0
            || pCharacter->field_0x4 < 0) {
        return 0;
    }
    if (pModel_index < 0) {
        br_actor* actor;
        br_model* old_model;
        int i;

        actor = pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index];
        old_model = actor->model;
        for (i = 0; i < (int)REC2_ASIZE(pCharacter->personality->bones[pBone_index].models); i++) {
            if (pCharacter->personality->bones[pBone_index].models[i].models[pCharacter->field_0xa] == old_model) {
                break;
            }
        }
        if (i == REC2_ASIZE(pCharacter->personality->bones[pBone_index].models)) {
            i = 0;
        }
        actor->model = pCharacter->personality->bones[pBone_index].models[i].models[pModel_set];
        pCharacter->field_0xa = (tS8)pModel_set;
    } else {
        br_model* model;

        pCharacter->field_0xa = (tS8)pModel_set;
        model = pCharacter->personality->bones[pBone_index].models[pModel_index].models[pCharacter->field_0xa];
        if (model == NULL) {
            return 0;
        }
        pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->model = model;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0040b500
// FUNCTION: CARMA2_SW 0x0040b4f0
void C2_HOOK_FASTCALL SetCharacterBoneModelP(tPed_character_instance* pCharacter, int pBone_index, br_model* pModel, int pArg4) {
    if (pCharacter->field_0x4 >= 0) {
        pCharacter->field_0xa = (tS8)pArg4;
        pCharacter->personality->form->actor_sets[(tS8)pCharacter->field_0x4].actors[pBone_index]->model = pModel;
    }
}

// FUNCTION: CARMA2_HW 0x0040b530
// FUNCTION: CARMA2_SW 0x0040b520
// FUNCTION: CARMA2_CHEAT 0x0040b5a0
void C2_HOOK_FASTCALL SetCharacterAllBonesModel(tPed_character_instance* pCharacter, int pModel_index, int pModel_set) {
    int i;

    for (i = 0; i < pCharacter->personality->form->count_bones; i++) {
        SetCharacterBoneModel(pCharacter, i, pModel_index, pModel_set);
    }
}

// FUNCTION: CARMA2_HW 0x0040b570
// FUNCTION: CARMA2_SW 0x0040b560
// FUNCTION: CARMA2_CHEAT 0x0040b5e0
br_model* C2_HOOK_FASTCALL GetCharacterBoneModel(tPed_character_instance* pCharacter, int pBone_index) {

    return pCharacter->personality->form->actor_sets[pCharacter->field_0x4].actors[pBone_index]->model;
}

// FUNCTION: CARMA2_HW 0x0040b590
// FUNCTION: CARMA2_SW 0x0040b580
// FUNCTION: CARMA2_CHEAT 0x0040b600
int C2_HOOK_FASTCALL GetCharacterModelSet(tPed_character_instance* pCharacter) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPed_character_instance, field_0xa, 0xa);

    return pCharacter->field_0xa;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// BonerPassiveCollision

// FUNCTION: CARMA2_HW 0x0040b760
// FUNCTION: CARMA2_SW 0x0040b750
int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter) {

    return BonerActiveHalted2(pCharacter, 0);
}

// BonerPedMovedByPhysics

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040b860
// STUB: CARMA2_SW 0x0040b850
// STUB: CARMA2_CHEAT 0x0040b8e0
int C2_HOOK_FASTCALL DismemberCharacter(tPed_character_instance* pCharacter, int pBone_index, br_vector3* pVelocity, br_vector3* pAngular_velocity) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040bae0
// FUNCTION: CARMA2_SW 0x0040bad0
// FUNCTION: CARMA2_CHEAT 0x0040bb80
float C2_HOOK_FASTCALL GetClearanceFromCharacterInstance(tPed_character_instance* pCharacter, int pMoveNum) {

    if (pMoveNum < 0 || pMoveNum > pCharacter->personality->form->count_moves) {
        BrFailure("MoveNum out of range in GetClearance()");
    }
    return pCharacter->personality->moves[pMoveNum].grounding_offset;
}

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040c420
// STUB: CARMA2_SW 0x0040c410
// STUB: CARMA2_CHEAT 0x0040c4c0
void C2_HOOK_FASTCALL ScaleBone(int pBone_index, br_vector3* pScale, int pArg3) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040c490
// FUNCTION: CARMA2_SW 0x0040c480
// FUNCTION: CARMA2_CHEAT 0x0040c530
void C2_HOOK_FASTCALL MungeAllCharacterMaterials(material_cbfn* pCallback) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_personalities) != 50);
    C2_HOOK_BUG_ON(REC2_ASIZE(gPedestrian_character_cloned_materials) != 50);
    C2_HOOK_BUG_ON(REC2_ASIZE(gPed_sin_table) != 256);

    for (i = 0; i < (int)REC2_ASIZE(gPed_personalities); i++) {
        if (gPed_personalities[i] != NULL) {
            pCallback(gPed_personalities[i]->bones[0].models[0].models[0]->faces[0].material);
        }
    }
    for (i = 0; i < gCount_pedestrian_personality_cloned_materials; i++) {
        pCallback(gPedestrian_character_cloned_materials[i]);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040c4e0
// FUNCTION: CARMA2_SW 0x0040c4d0
// FUNCTION: CARMA2_CHEAT 0x0040c580
void C2_HOOK_FASTCALL MungeAllCharacterObjects(void (C2_HOOK_FASTCALL *pCallback)(tPhysics_object*)) {
    tPed_form** form;
    int i;
    int j;
    int k;

    for (i = 0, form = gPed_forms; i < (int)REC2_ASIZE(gPed_forms); i++, form++) {

        if (*form != NULL) {
            for (j = 0; j < (*form)->max_boned_physicing_at_once; j++) {
                for (k = 0; k < (*form)->count_bones; k++) {
                    pCallback((*form)->boned_physicing[j].collision_infos[k]);
                }
            }
            for (j = 0; j < (*form)->max_simple_physicing_at_once; j++) {
                pCallback((*form)->simple_physicing[j].collision_info);
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040c560
// FUNCTION: CARMA2_SW 0x0040c550
// FUNCTION: CARMA2_CHEAT 0x0040c600
void C2_HOOK_FASTCALL MungeAllCharacterModels(void (C2_HOOK_FASTCALL *pCallback)(br_model*)) {
    tPed_personality** personality;
    int i;
    int j;
    int k;
    int l;

    for (i = 0, personality = gPed_personalities; i < (int)REC2_ASIZE(gPed_personalities); i++, personality++) {
        if (*personality != NULL) {
            for (j = 0; j < (*personality)->form->count_bones; j++) {
                for (k = 0; k < REC2_ASIZE((*personality)->bones[j].models[l].models); k++) { // 2x
                    for (l = 0; l < (int)REC2_ASIZE((*personality)->bones[j].models); l++) { // 4x
                        br_model* model = (*personality)->bones[j].models[l].models[k];

                        if (model != NULL) {
                            pCallback(model);
                        }
                    }
                }
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040c600
// FUNCTION: CARMA2_SW 0x0040c5f0
// FUNCTION: CARMA2_CHEAT 0x0040c6a0
void C2_HOOK_FASTCALL DisableOverallMovement(void) {
    gPed_overall_movement_disabled = 1;
}

// FUNCTION: CARMA2_HW 0x0040c610
// FUNCTION: CARMA2_SW 0x0040c600
// FUNCTION: CARMA2_CHEAT 0x0040c6b0
void C2_HOOK_FASTCALL EnableOverallMovement(void) {
    gPed_overall_movement_disabled = 0;
}

// FUNCTION: CARMA2_HW 0x0040c620
// FUNCTION: CARMA2_SW 0x0040c610
// FUNCTION: CARMA2_CHEAT 0x0040c6c0
void C2_HOOK_FASTCALL EnableRetainRootMode(void) {
    gPed_retain_root_mode = 1;
}

// FUNCTION: CARMA2_HW 0x0040c630
// FUNCTION: CARMA2_SW 0x0040c620
// FUNCTION: CARMA2_CHEAT 0x0040c6d0
void C2_HOOK_FASTCALL DisableRetainRootMode(void) {
    gPed_retain_root_mode = 0;
}

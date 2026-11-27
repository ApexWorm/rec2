/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "globvars.h"


// GLOBAL: CARMA2_HW 0x00676918
// GLOBAL: CARMA2_SW 0x00671088
// GLOBAL: CARMA2_CHEAT 0x0067e1c8
int gDisable_piped_sounds = 0;

// GLOBAL: CARMA2_HW 0x0068b868
// GLOBAL: CARMA2_SW 0x00685dc8
// GLOBAL: CARMA2_CHEAT 0x00693110
int gApplicationDataTwtMounted;

// GLOBAL: CARMA2_HW 0x0075ba60
// GLOBAL: CARMA2_SW 0x00751fc0
// GLOBAL: CARMA2_CHEAT 0x00763100
tPath_name gApplication_path;

// GLOBAL: CARMA2_HW 0x0074d0a0
// GLOBAL: CARMA2_CHEAT 0x00754740
// GLOBAL: CARMA2_SW 0x00743600
char gRace_path[256];

// GLOBAL: CARMA2_HW 0x00761a5c
// GLOBAL: CARMA2_SW 0x00757fbc
// GLOBAL: CARMA2_CHEAT 0x007690fc
const char* gDir_separator;


// GLOBAL: CARMA2_HW 0x0068b87c
// GLOBAL: CARMA2_SW 0x00685ddc
// GLOBAL: CARMA2_CHEAT 0x00693124
int gAusterity_mode;

// GLOBAL: CARMA2_HW 0x0068b878
// GLOBAL: CARMA2_SW 0x00685dd8
int gDefault_engine_noise_index;

// GLOBAL: CARMA2_HW 0x0068b884
// GLOBAL: CARMA2_SW 0x00685de4
// GLOBAL: CARMA2_CHEAT 0x0069312c
int gAustere_override;


// GLOBAL: CARMA2_HW 0x0068b8a4
// GLOBAL: CARMA2_SW 0x00685e04
// GLOBAL: CARMA2_CHEAT 0x0069314c
br_pixelmap* gDepth_buffer;

// GLOBAL: CARMA2_HW 0x0074d3e0
// GLOBAL: CARMA2_SW 0x00743940
// GLOBAL: CARMA2_CHEAT 0x00754a80
br_pixelmap* gScreen;

// GLOBAL: CARMA2_HW 0x0068b888
// GLOBAL: CARMA2_SW 0x00685de8
// GLOBAL: CARMA2_CHEAT 0x00693130
int gSound_override;


// GLOBAL: CARMA2_HW 0x0074d360
// GLOBAL: CARMA2_SW 0x007438c0
// GLOBAL: CARMA2_CHEAT 0x00754a00
br_pixelmap* gBack_screen;

// GLOBAL: CARMA2_HW 0x006ad47c
// GLOBAL: CARMA2_CHEAT 0x006b4b08
br_pixelmap* gDouble_back_screen;

// GLOBAL: CARMA2_HW 0x006ad480
// GLOBAL: CARMA2_CHEAT 0x006b4b0c
int gHas_double_back_screen;


// GLOBAL: CARMA2_HW 0x00761f4c
// GLOBAL: CARMA2_SW 0x007584ac
// GLOBAL: CARMA2_CHEAT 0x007695ec
br_scalar gCamera_yon;


// GLOBAL: CARMA2_HW 0x0075bb60
// GLOBAL: CARMA2_SW 0x007520c0
// GLOBAL: CARMA2_CHEAT 0x00763200
br_actor* gTrack_actor;


// GLOBAL: CARMA2_HW 0x0074d3e4
// GLOBAL: CARMA2_SW 0x00743944
// GLOBAL: CARMA2_CHEAT 0x00754a84
tU32 gFrame_period;


// GLOBAL: CARMA2_HW 0x0075b9c0
// GLOBAL: CARMA2_SW 0x00751f20
// GLOBAL: CARMA2_CHEAT 0x00763060
tBrender_storage gMisc_storage_space;


// GLOBAL: CARMA2_HW 0x0075ba54
// GLOBAL: CARMA2_SW 0x00751fb4
// GLOBAL: CARMA2_CHEAT 0x007630f4
br_font* gBig_font;

// GLOBAL: CARMA2_HW 0x0074d3c4
// GLOBAL: CARMA2_SW 0x00743924
// GLOBAL: CARMA2_CHEAT 0x00754a64
br_font* gFont_7;

// GLOBAL: CARMA2_HW 0x0074cf9c
// GLOBAL: CARMA2_SW 0x007434f0
// GLOBAL: CARMA2_CHEAT 0x0075463c
br_font* gHeadup_font;


// GLOBAL: CARMA2_HW 0x0075bb80
// GLOBAL: CARMA2_SW 0x007520e0
// GLOBAL: CARMA2_CHEAT 0x00763220
tProgram_state gProgram_state;

// GLOBAL: CARMA2_HW 0x0068b880
// GLOBAL: CARMA2_SW 0x00685de0
// GLOBAL: CARMA2_CHEAT 0x00693128
int gSausage_override;

// GLOBAL: CARMA2_HW 0x0074d660
// GLOBAL: CARMA2_SW 0x00743bc0
// GLOBAL: CARMA2_CHEAT 0x00754d00
tRace_list_spec gRace_list[100];

// GLOBAL: CARMA2_HW 0x0074d4a0
// GLOBAL: CARMA2_SW 0x00743a00
// GLOBAL: CARMA2_CHEAT 0x00754b40
int gNumber_of_races;


// GLOBAL: CARMA2_HW 0x0075b960
// GLOBAL: CARMA2_SW 0x00751ec0
// GLOBAL: CARMA2_CHEAT 0x00763000
tBrender_storage gOur_car_storage_space;

// GLOBAL: CARMA2_HW 0x00761ca0
// GLOBAL: CARMA2_SW 0x00758200
// GLOBAL: CARMA2_CHEAT 0x00769340
tBrender_storage gTheir_cars_storage_space;

// GLOBAL: CARMA2_HW 0x00762340
// GLOBAL: CARMA2_SW 0x007588a0
// GLOBAL: CARMA2_CHEAT 0x007699e0
tBrender_storage gDroneStorage;

// GLOBAL: CARMA2_HW 0x0074d5a0
// GLOBAL: CARMA2_SW 0x00743b00
// GLOBAL: CARMA2_CHEAT 0x00754c40
tBrender_storage gPedStorage;

// GLOBAL: CARMA2_HW 0x0074d400
// GLOBAL: CARMA2_SW 0x00743960
// GLOBAL: CARMA2_CHEAT 0x00754aa0
tBrender_storage gTrack_storage_space;

// GLOBAL: CARMA2_HW 0x0068b89c
// GLOBAL: CARMA2_SW 0x00685dfc
// GLOBAL: CARMA2_CHEAT 0x00693144
int gCD_is_in_drive;

// GLOBAL: CARMA2_HW 0x0076211c
// GLOBAL: CARMA2_SW 0x0075867c
// GLOBAL: CARMA2_CHEAT 0x007697bc
int gDev_initial_race;

// GLOBAL: CARMA2_HW 0x0074d460
// GLOBAL: CARMA2_SW 0x007439c0
// GLOBAL: CARMA2_CHEAT 0x00754b00
br_matrix34 gCamera_to_world = { 0 };

// GLOBAL: CARMA2_HW 0x0074d490
// GLOBAL: CARMA2_SW 0x007439f0
// GLOBAL: CARMA2_CHEAT 0x00754b30
int gCurrent_APO_potential_levels[3] = { 0 };

// GLOBAL: CARMA2_HW 0x0074d560
// GLOBAL: CARMA2_SW 0x00743ac0
// GLOBAL: CARMA2_CHEAT 0x00754c00
br_matrix34 gRearview_camera_to_world;


// GLOBAL: CARMA2_HW 0x00705b80
// GLOBAL: CARMA2_SW 0x006fc640
// GLOBAL: CARMA2_CHEAT 0x0070d220
undefined4 gUnknown_00705b80[20];

// GLOBAL: CARMA2_HW 0x00705be0
// GLOBAL: CARMA2_SW 0x006fc6a0
// GLOBAL: CARMA2_CHEAT 0x0070d280
int gCredit_multiplier;


// GLOBAL: CARMA2_HW 0x0074d44c
// GLOBAL: CARMA2_SW 0x007439ac
// GLOBAL: CARMA2_CHEAT 0x00754aec
br_actor* gUniverse_actor;

// GLOBAL: CARMA2_HW 0x007634b8
// GLOBAL: CARMA2_SW 0x00759a18
// GLOBAL: CARMA2_CHEAT 0x0076ab58
br_actor* gNon_track_actor;

// GLOBAL: CARMA2_HW 0x00761ce4
// GLOBAL: CARMA2_SW 0x00758244
// GLOBAL: CARMA2_CHEAT 0x00769384
br_actor* gDont_render_actor;


// GLOBAL: CARMA2_HW 0x00655dc4
// GLOBAL: CARMA2_SW 0x00650cac
// GLOBAL: CARMA2_CHEAT 0x0065c644
int gAmbient_sound = 1;

// GLOBAL: CARMA2_HW 0x0068b864
// GLOBAL: CARMA2_SW 0x00685dc4
// GLOBAL: CARMA2_CHEAT 0x0069310c
int gAuto_load;

// GLOBAL: CARMA2_HW 0x006aa5c4
// GLOBAL: CARMA2_SW 0x006a4014
// GLOBAL: CARMA2_CHEAT 0x006b213c
int gRussels_fannies;

// GLOBAL: CARMA2_HW 0x007051e0
// GLOBAL: CARMA2_SW 0x006fbca0
// GLOBAL: CARMA2_CHEAT 0x0070c880
int gAPO_pre_race[3];

// GLOBAL: CARMA2_HW 0x007051f8
// GLOBAL: CARMA2_SW 0x006fbcb8
// GLOBAL: CARMA2_CHEAT 0x0070c898
int gMoney_pre_race;

// GLOBAL: CARMA2_HW 0x00705220
// GLOBAL: CARMA2_SW 0x006fbce0
// GLOBAL: CARMA2_CHEAT 0x0070c8c0
char gQuick_time_quality[32];

// GLOBAL: CARMA2_HW 0x00705200
// GLOBAL: CARMA2_SW 0x006fbcc0
// GLOBAL: CARMA2_CHEAT 0x0070c8a0
char gQuick_time_compressor[32];

// GLOBAL: CARMA2_HW 0x0065ef30
// GLOBAL: CARMA2_SW 0x00659890
// GLOBAL: CARMA2_CHEAT 0x00666708
int gQuick_time_banner_number = 1;


// GLOBAL: CARMA2_HW 0x007638a0
// GLOBAL: CARMA2_SW 0x00759e00
// GLOBAL: CARMA2_CHEAT 0x0076af40
int gNet_last_game_type;

// GLOBAL: CARMA2_HW 0x0075b9a4
// GLOBAL: CARMA2_SW 0x00751f04
// GLOBAL: CARMA2_CHEAT 0x00763044
int gMap_view;

// GLOBAL: CARMA2_HW 0x0074cadc
// GLOBAL: CARMA2_CHEAT 0x0075417c
int gLock_often;

// GLOBAL: CARMA2_HW 0x0074ca0c
// GLOBAL: CARMA2_CHEAT 0x007540ac
int gShadow_workaround;

// GLOBAL: CARMA2_HW 0x0074cf38
// GLOBAL: CARMA2_CHEAT 0x007545d8
int gEnable_texture_interpolation;

// GLOBAL: CARMA2_HW 0x0074cf20
// GLOBAL: CARMA2_CHEAT 0x007545c0
int gEnable_texture_antialiasing;

// GLOBAL: CARMA2_HW 0x0074ca20
// GLOBAL: CARMA2_CHEAT 0x007540c0
int gTexture_power_of_2;

// GLOBAL: CARMA2_HW 0x0074cab8
// GLOBAL: CARMA2_CHEAT 0x00754158
int gAdapt_sky_model_for_cockpit;

// GLOBAL: CARMA2_HW 0x0074ca10
// GLOBAL: CARMA2_CHEAT 0x007540b0
int gNo_fog;

// GLOBAL: CARMA2_HW 0x0074c9f4
// GLOBAL: CARMA2_CHEAT 0x00754094
const char* gRenderer_fixup_basename;

// GLOBAL: CARMA2_HW 0x0074ca6c
// GLOBAL: CARMA2_CHEAT 0x0075410c
const char* gRenderer_fixup_extension;

// GLOBAL: CARMA2_HW 0x0074cf8c
// GLOBAL: CARMA2_CHEAT 0x0075462c
int gNo_render_indent;

// GLOBAL: CARMA2_HW 0x0074cf68
// GLOBAL: CARMA2_CHEAT 0x00754608
int gNo_2d_effects;

// GLOBAL: CARMA2_HW 0x0074ca54
// GLOBAL: CARMA2_CHEAT 0x007540f4
int gEnable_perspective_maps;

// GLOBAL: CARMA2_HW 0x0074cf60
// GLOBAL: CARMA2_CHEAT 0x00754600
int gDevious_2d;

// GLOBAL: CARMA2_HW 0x0068c71c
// GLOBAL: CARMA2_SW 0x0068676c
// GLOBAL: CARMA2_CHEAT 0x00693fbc
int gNumber_of_actors;

// GLOBAL: CARMA2_HW 0x0074ac00
// GLOBAL: CARMA2_SW 0x7412c0
// GLOBAL: CARMA2_CHEAT 0x007522a0
br_actor* gActor_array[100];

// GLOBAL: CARMA2_HW 0x0068c720
// GLOBAL: CARMA2_SW 0x00686770
// GLOBAL: CARMA2_CHEAT 0x00693fc0
int gNumber_of_lights;

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x006572cc
// GLOBAL: CARMA2_CHEAT 0x0065db5c
tLighting_data gLighting_data = {
    { 255, 255, 255 },
    0.2f, 0.8f,
    0.2f, 0.8f,
    0.2f, 0.8f,
};
#endif

#ifdef REC2_SW
// GLOBAL: CARMA2_SW 0x00651c84
tLighting_data_ambient gLighting_data_ambient = {
    { 255, 255, 255 },
    1.0f,
    1.0f,
    1.0f,
};

// GLOBAL: CARMA2_SW 0x0068672c
tLighting_data_diffuse gLighting_data_diffuse;

#endif

// GLOBAL: CARMA2_HW 0x0074b3e0
// GLOBAL: CARMA2_SW 0x00741aa0
// GLOBAL: CARMA2_CHEAT 0x00752a80
br_actor* gLight_array[50];


// GLOBAL: CARMA2_HW 0x0074ca00
// GLOBAL: CARMA2_SW 0x007430b4
// GLOBAL: CARMA2_CHEAT 0x007540a0
br_actor* gHUD_root;

// GLOBAL: CARMA2_HW 0x0074cf74
// GLOBAL: CARMA2_SW 0x007434e8
// GLOBAL: CARMA2_CHEAT 0x00754614
br_actor* gHUD_camera;

// GLOBAL: CARMA2_HW 0x0074ca34
// GLOBAL: CARMA2_CHEAT 0x007540d4
br_actor* gLine_actor;

// GLOBAL: CARMA2_HW 0x0074cac8
// GLOBAL: CARMA2_CHEAT 0x00754168
br_model* gLine_model;

// GLOBAL: CARMA2_HW 0x0074ca4c
// GLOBAL: CARMA2_CHEAT 0x007540ec
br_material* gLine_material;

// GLOBAL: CARMA2_HW 0x0074cf30
br_model* gBlend_model;

// GLOBAL: CARMA2_HW 0x0074cf94
br_model* gBlend_model2;

// GLOBAL: CARMA2_HW 0x0074caac
br_actor* gBlend_actor;

// GLOBAL: CARMA2_HW 0x006a87f0
tBRender_smoke gBR_smoke_structs[35];


// GLOBAL: CARMA2_HW 0x00655dc0
// GLOBAL: CARMA2_SW 0x00650ca8
// GLOBAL: CARMA2_CHEAT 0x0065c640
int gNo_current_game = 1;

// GLOBAL: CARMA2_HW 0x0074b7d8
// GLOBAL: CARMA2_SW 0x00741e98
// GLOBAL: CARMA2_CHEAT 0x00752e78
int gWaiting_for_unpause;

// GLOBAL: CARMA2_HW 0x0075b8f0
// GLOBAL: CARMA2_SW 0x00751e50
// GLOBAL: CARMA2_CHEAT 0x00762f90
tU32 gWait_for_it;

// GLOBAL: CARMA2_HW 0x00655dd4
// GLOBAL: CARMA2_SW 0x00650cbc
// GLOBAL: CARMA2_CHEAT 0x0065c654
int gGame_to_load = -1;

// GLOBAL: CARMA2_HW 0x007623a0
// GLOBAL: CARMA2_SW 0x00758900
// GLOBAL: CARMA2_CHEAT 0x00769a40
tRace_info gCurrent_race;

// GLOBAL: CARMA2_HW 0x0068be3c
// GLOBAL: CARMA2_SW 0x00685e80
// GLOBAL: CARMA2_CHEAT 0x006936dc
int gGame_initialized;

// GLOBAL: CARMA2_HW 0x0074d49c
// GLOBAL: CARMA2_SW 0x007439fc
// GLOBAL: CARMA2_CHEAT 0x00754b3c
int gNo_races_yet;

// GLOBAL: CARMA2_HW 0x0074b750
// GLOBAL: CARMA2_SW 0x00741e10
// GLOBAL: CARMA2_CHEAT 0x00752df0
tNet_mode gNet_mode_of_last_game;

// GLOBAL: CARMA2_HW 0x0074d5e4
// GLOBAL: CARMA2_SW 0x00743b44
// GLOBAL: CARMA2_CHEAT 0x00754c84
tRace_group_spec* gCurrent_race_group;

// GLOBAL: CARMA2_HW 0x00761a60
// GLOBAL: CARMA2_SW 0x00757fc0
// GLOBAL: CARMA2_CHEAT 0x00769100
int gIs_boundary_race;

// GLOBAL: CARMA2_HW 0x00761a80
// GLOBAL: CARMA2_SW 0x00757fe0
// GLOBAL: CARMA2_CHEAT 0x00769120
char gCurrent_load_name[256];

// GLOBAL: CARMA2_HW 0x0074d1ac
// GLOBAL: CARMA2_SW 0x0074370c
// GLOBAL: CARMA2_CHEAT 0x0075484c
int gNumber_of_racers;

// GLOBAL: CARMA2_HW 0x007634bc
// GLOBAL: CARMA2_SW 0x00759a1c
// GLOBAL: CARMA2_CHEAT 0x0076ab5c
tOpponent* gOpponents;

// GLOBAL: CARMA2_HW 0x00761f50
// GLOBAL: CARMA2_SW 0x007584b0
// GLOBAL: CARMA2_CHEAT 0x007695f0
int gCurrent_APO_levels[3];

// GLOBAL: CARMA2_HW 0x0074b784
// GLOBAL: CARMA2_SW 0x00741e44
// GLOBAL: CARMA2_CHEAT 0x00752e24
tNet_game_details* gCurrent_net_game;


// GLOBAL: CARMA2_HW 0x00660ce4
// GLOBAL: CARMA2_SW 0x0065b658
// GLOBAL: CARMA2_CHEAT 0x0066852c
int gPratcam_on = 1;

// GLOBAL: CARMA2_HW 0x006aaa30
// GLOBAL: CARMA2_CHEAT 0x006b25a8
int gCockpit_on;


// GLOBAL: CARMA2_HW 0x006aaf44
int gTree_surgery_pass1_count;

// GLOBAL: CARMA2_HW 0x006aaf10
int gTree_surgery_pass2_count;

// GLOBAL: CARMA2_HW 0x006aaf18
// GLOBAL: CARMA2_SW 0x006a4968
tTree_surgery_pass1 gTree_surgery_pass1[5];

// GLOBAL: CARMA2_HW 0x006aaa60
tTree_surgery_pass2 gTree_surgery_pass2[25];


// GLOBAL: CARMA2_HW 0x0068b858
// GLOBAL: CARMA2_SW 0x00685db8
// GLOBAL: CARMA2_CHEAT 0x00693100
int gTyping;

// GLOBAL: CARMA2_HW 0x0074b588
// GLOBAL: CARMA2_SW 0x00741c48
int gCount_opponents;

// GLOBAL: CARMA2_HW 0x0076350c
// GLOBAL: CARMA2_CHEAT 0x0076abac
br_actor* gPlayer_car_master_actor;

// GLOBAL: CARMA2_HW 0x00655de0
// GLOBAL: CARMA2_SW 0x00650cc8
// GLOBAL: CARMA2_CHEAT 0x0065c660
br_vector3 gInitial_position = {{1000.f, 1000.f, 1000.f}};

// GLOBAL: CARMA2_HW 0x0068b85c
// GLOBAL: CARMA2_SW 0x00685dbc
// GLOBAL: CARMA2_CHEAT 0x00693104
int gArrow_mode;


// GLOBAL: CARMA2_HW 0x0074d040
// GLOBAL: CARMA2_SW 0x007435a0
// GLOBAL: CARMA2_CHEAT 0x007546e0
int gRace_head_ups[22];

//int gInfo_on;

// GLOBAL: CARMA2_HW 0x0068b86c
// GLOBAL: CARMA2_SW 0x00685dcc
// GLOBAL: CARMA2_CHEAT 0x00693114
int gHost_abandon_game;

// GLOBAL: CARMA2_HW 0x0068b870
// GLOBAL: CARMA2_SW 0x00685dd0
// GLOBAL: CARMA2_CHEAT 0x00693118
int gRecover_car;

// GLOBAL: CARMA2_HW 0x0068b874
int gDisallow_abandon_race;
//int gPipe_reg_snap_before_new_game;
//int gPipe_reg_snap_before_our_car;
//int gPipe_reg_snap_after_our_car;

// GLOBAL: CARMA2_HW 0x00676914
// GLOBAL: CARMA2_SW 0x00671084
// GLOBAL: CARMA2_CHEAT 0x0067e1c4
int gAction_replay_mode = 0;

// GLOBAL: CARMA2_HW 0x00676928
// GLOBAL: CARMA2_SW 0x00671098
// GLOBAL: CARMA2_CHEAT 0x0067e1d8
tPed_form* gPed_forms[20] = { 0 };

// GLOBAL: CARMA2_HW 0x00676978
// GLOBAL: CARMA2_SW 0x006710e8
// GLOBAL: CARMA2_CHEAT 0x0067e228
int gPed_overall_movement_disabled = 0;

//int gDefault_engine_noise_index;
//int gAusterity_mode;
//int gAustere_override;

//int gKey_map_index;

// GLOBAL: CARMA2_HW 0x0068b890
// GLOBAL: CARMA2_SW 0x00685df0
// GLOBAL: CARMA2_CHEAT 0x00693138
int gInterface_within_race_mode;
//int gCut_scene_override;
//int gGame_to_load = -1;
//int gCD_is_in_drive;

// GLOBAL: CARMA2_HW 0x0068b8a8
// GLOBAL: CARMA2_SW 0x00685e08
// GLOBAL: CARMA2_CHEAT 0x00693150
br_pixelmap* gRearview_screen;
//br_scalar gCut_delay_1;
//br_scalar gCut_delay_2;
//br_scalar gCut_delay_3;
//br_scalar gCut_delay_4;

// GLOBAL: CARMA2_HW 0x00655ddc
// GLOBAL: CARMA2_SW 0x00650cc4
// GLOBAL: CARMA2_CHEAT 0x0065c65c
br_scalar gYon_multiplier = 1.f;
//tSave_game* gPre_race_saved_game;
//VFILE* gDiagnostic_file;
//br_vector3 gDisabled_vector;

// GLOBAL: CARMA2_HW 0x0068b8e0
// GLOBAL: CARMA2_SW 0x00685e40
// GLOBAL: CARMA2_CHEAT 0x0065c680
tU32 gI_am_cheating;

// GLOBAL: CARMA2_HW 0x0068b8e4
// GLOBAL: CARMA2_SW 0x00685e44
// GLOBAL: CARMA2_CHEAT 0x00693188
int gINT_0068b8e4;
//char gNet_player_name[32];
//br_matrix34 gRearview_camera_to_world;
//tSpecial_volume gDefault_default_water_spec_vol;
//br_matrix34 gCamera_to_world;
//tRace_list_spec gRace_list[100];

// GLOBAL: CARMA2_HW 0x00762120
// GLOBAL: CARMA2_SW 0x00758680
// GLOBAL: CARMA2_CHEAT 0x007697c0
int gWasted_explosion_chance;

// GLOBAL: CARMA2_HW 0x007620f8
// GLOBAL: CARMA2_SW 0x00758658
// GLOBAL: CARMA2_CHEAT 0x00769798
tExplosion_animation gExplosion_pix_animation_groups;

// GLOBAL: CARMA2_HW 0x00762124
// GLOBAL: CARMA2_SW 0x00758684
// GLOBAL: CARMA2_CHEAT 0x007697c4
float gPinball_factor;

// GLOBAL: CARMA2_HW 0x007620f0
// GLOBAL: CARMA2_SW 0x00758650
// GLOBAL: CARMA2_CHEAT 0x00769790
float gOpponent_speed_factor;
//float gCar_crush_min_fold;
//float gNet_recovery_cost[7];
//float gCar_crush_softness;

// GLOBAL: CARMA2_HW 0x007634b0
// GLOBAL: CARMA2_CHEAT 0x0076ab50
br_vector3* gOur_pos;

// GLOBAL: CARMA2_HW 0x007634b4
// GLOBAL: CARMA2_SW 0x00759a14
int gTrack_version;
//float gCar_crush_max_fold;

// GLOBAL: CARMA2_HW 0x00761f00
// GLOBAL: CARMA2_SW 0x00758460
// GLOBAL: CARMA2_CHEAT 0x007695a0
tBrender_storage gNet_cars_storage_space;
//float gNet_softness[7];
//tS32 gJoystick_deadzone;
//float gCar_crush_limit_deviant;
//float gNet_offensive[7];
//float gCar_crush_split_chance;

// GLOBAL: CARMA2_HW 0x0074d1a8
// GLOBAL: CARMA2_SW 0x00743708
// GLOBAL: CARMA2_CHEAT 0x00754848
float gGravity_multiplier;
//float gCar_crush_wibble;

// GLOBAL: CARMA2_HW 0x00761d00
// GLOBAL: CARMA2_SW 0x00758260
// GLOBAL: CARMA2_CHEAT 0x007693a0
tFloat_bunch_info gRepair_cost;

// GLOBAL: CARMA2_HW 0x007634c0
// GLOBAL: CARMA2_SW 0x00759a20
// GLOBAL: CARMA2_CHEAT 0x0076ab60
tFloat_bunch_info gRecovery_cost;

// GLOBAL: CARMA2_HW 0x0074d634
// GLOBAL: CARMA2_SW 0x00743b94
// GLOBAL: CARMA2_CHEAT 0x00754cd4
float gCop_speed_factor;

// GLOBAL: CARMA2_HW 0x0074d3bc
// GLOBAL: CARMA2_SW 0x0074391c
// GLOBAL: CARMA2_CHEAT 0x00754a5c
float gDefault_gravity;

// GLOBAL: CARMA2_HW 0x0074d350
// GLOBAL: CARMA2_SW 0x007438b0
// GLOBAL: CARMA2_CHEAT 0x007549f0
tU32 gKeys_pressed;
//tSpecial_volume* gDefault_default_water_spec_vol;
//float gNet_repair_cost[7];

// GLOBAL: CARMA2_HW 0x0079efb4
// GLOBAL: CARMA2_SW 0x00795554
// GLOBAL: CARMA2_CHEAT 0x007a6654
tU32 gLast_replay_frame_time;
//tU32 gTag_start_time;
//tOpponent* gOpponents;
//tU32 gLast_credit_tick;
//tBrender_storage gTheir_cars_storage_space;

// GLOBAL: CARMA2_HW 0x0074d62c
// GLOBAL: CARMA2_SW 0x00743b8c
// GLOBAL: CARMA2_CHEAT 0x00754ccc
tU32 gRace_finished;
//tU32 gFrame_start_time;
//tProgram_state gProgram_state;
//tU32 gCredit_period_single[3];

// GLOBAL: CARMA2_HW 0x00763460
// GLOBAL: CARMA2_SW 0x007599c0
// GLOBAL: CARMA2_CHEAT 0x0076ab00
tU32 gRace_start;
//tBrender_storage gTrack_storage_space;
//tU32 gCredit_period;
//tU32 gCredit_period_network[7];
//tBrender_storage gOur_car_storage_space;
//tU32 gFox_start_time;
//tBrender_storage gPedestrians_storage_space;
//tRace_info gCurrent_race;

// GLOBAL: CARMA2_HW 0x0068be28
// GLOBAL: CARMA2_SW 0x00685e6c
// GLOBAL: CARMA2_CHEAT 0x006936c8
br_actor* gSelf;

// GLOBAL: CARMA2_HW 0x00762128
// GLOBAL: CARMA2_SW 0x00758688
// GLOBAL: CARMA2_CHEAT 0x007697c8
br_pixelmap* gRender_screen;

// GLOBAL: CARMA2_HW 0x00762130
// GLOBAL: CARMA2_SW 0x00758690
int gInitial_powerup_slots[3];

// GLOBAL: CARMA2_HW 0x0075b940
// GLOBAL: CARMA2_SW 0x00751ea0
// GLOBAL: CARMA2_CHEAT 0x00762fe0
br_actor* gRearview_camera;

// GLOBAL: CARMA2_HW 0x0074d3e8
// GLOBAL: CARMA2_SW 0x00743948
// GLOBAL: CARMA2_CHEAT 0x00754a88
br_scalar gCamera_hither;
//br_scalar gCamera_angle;
//br_pixelmap* gDigits_pix;
//br_pixelmap* gRender_shade_table;

// GLOBAL: CARMA2_HW 0x0075b93c
// GLOBAL: CARMA2_SW 0x00751e9c
// GLOBAL: CARMA2_CHEAT 0x00762fdc
br_pixelmap* gRearview_depth_buffer;
//br_pixelmap* gWhole_screen;

// GLOBAL: CARMA2_HW 0x0068b8ac
// GLOBAL: CARMA2_SW 0x00685e0c
// GLOBAL: CARMA2_CHEAT 0x00693154
br_pixelmap* gIcons_pix;

#ifdef REC2_SW
// GLOBAL: CARMA2_SW 0x00685e10
br_pixelmap* gIcons_low_pix;
#endif

// GLOBAL: CARMA2_HW 0x0074d35c
// GLOBAL: CARMA2_SW 0x007438bc
// GLOBAL: CARMA2_CHEAT 0x007549fc
br_actor* gCamera;

// GLOBAL: CARMA2_HW 0x0074d644
// GLOBAL: CARMA2_SW 0x00743ba4
// GLOBAL: CARMA2_CHEAT 0x00754ce4
br_actor* gOther_selfs[4];

#ifdef REC2_CHEAT
// GLOBAL: CARMA2_CHEAT 0x00754a60
br_actor* gArrow_actor;
#endif

//br_pixelmap* gIcons_pix_low_res;
//int gInitial_net_credits[5];
//float gCar_cred_value[3];
//int gNo_races_yet;

// GLOBAL: CARMA2_HW 0x0074d3d0
// GLOBAL: CARMA2_SW 0x00743930
// GLOBAL: CARMA2_CHEAT 0x00754a70
int gJump_start_fine[3];
//int gReal_graf_data_index;
//float gCar_time_value[3];
//int gTyping_slot;
//int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x007634f0
// GLOBAL: CARMA2_SW 0x00759a50
// GLOBAL: CARMA2_CHEAT 0x0076ab90
tU32 gKnobbled_frame_period;
//int gWait_for_it;
//int gCheck_point_cash[3];
//int gPoints_per_second[3];

// GLOBAL: CARMA2_HW 0x00761edc
// GLOBAL: CARMA2_SW 0x0075843c
// GLOBAL: CARMA2_CHEAT 0x0076957c
tU32 gMap_time;
//int gPed_time_value[3];

// GLOBAL: CARMA2_HW 0x0074d1a0
// GLOBAL: CARMA2_SW 0x00743700
// GLOBAL: CARMA2_CHEAT 0x00754840
int gAbandon_game;

// GLOBAL: CARMA2_HW 0x0075b944
// GLOBAL: CARMA2_SW 0x00751ea4
// GLOBAL: CARMA2_CHEAT 0x00762fe4
int gLap;

// GLOBAL: CARMA2_HW 0x00763510
// GLOBAL: CARMA2_SW 0x00759a70
int gShow_opponents;

// GLOBAL: CARMA2_HW 0x0075ba50
// GLOBAL: CARMA2_SW 0x00751fb0
// GLOBAL: CARMA2_CHEAT 0x007630f0
int gTotal_laps;

// GLOBAL: CARMA2_HW 0x0075b8f4
// GLOBAL: CARMA2_SW 0x00751e54
// GLOBAL: CARMA2_CHEAT 0x00762f94
int gFree_repairs;

// GLOBAL: CARMA2_HW 0x0075bb64
// GLOBAL: CARMA2_SW 0x007520c4
// GLOBAL: CARMA2_CHEAT 0x00763204
int gFreeze_timer;
//int gRoll_over_creds[3];

// GLOBAL: CARMA2_HW 0x0074d09c
// GLOBAL: CARMA2_SW 0x007435fc
// GLOBAL: CARMA2_CHEAT 0x0075473c
int gShow_peds_on_map;

// GLOBAL: CARMA2_HW 0x0074d444
// GLOBAL: CARMA2_SW 0x007439a4
// GLOBAL: CARMA2_CHEAT 0x00754ae4
int gOn_drugs;
//int gWasted_time[3];

// GLOBAL: CARMA2_HW 0x00761eec
// GLOBAL: CARMA2_SW 0x0075844c
// GLOBAL: CARMA2_CHEAT 0x0076958c
int gCheckpoint;
//int gRoll_over_time[3];
//int gWasted_creds[3];

// GLOBAL: CARMA2_HW 0x00761ee8
// GLOBAL: CARMA2_SW 0x00758448
// GLOBAL: CARMA2_CHEAT 0x00769588
tU32 gTimer;

// GLOBAL: CARMA2_HW 0x00761ce8
// GLOBAL: CARMA2_SW 0x00758248
// GLOBAL: CARMA2_CHEAT 0x00769388
int gCheckpoint_count;

// GLOBAL: CARMA2_HW 0x00761f48
// GLOBAL: CARMA2_SW 0x007584a8
// GLOBAL: CARMA2_CHEAT 0x007695e8
int gCountdown;

// GLOBAL: CARMA2_HW 0x00761ee0
int gFrame_rate;
//int gNumber_of_racers;

// GLOBAL: CARMA2_HW 0x00762328
// GLOBAL: CARMA2_SW 0x00758888
// GLOBAL: CARMA2_CHEAT 0x007699c8
int gSound_enabled;
//int gGraf_spec_index;

// GLOBAL: CARMA2_HW 0x00761ee4
// GLOBAL: CARMA2_SW 0x00758444
// GLOBAL: CARMA2_CHEAT 0x00769584
int gSound_available;
//int gNumber_of_races;
//char gDir_separator[3] = "";


// GLOBAL: CARMA2_HW 0x0074ca18
// GLOBAL: CARMA2_SW 0x007430b8
// GLOBAL: CARMA2_CHEAT 0x007540b8
tPixelFlags gPixelFlags;

// GLOBAL: CARMA2_HW 0x0074d358
// GLOBAL: CARMA2_SW 0x007438b8
// GLOBAL: CARMA2_CHEAT 0x007549f8
int gNo_credits_APO_restore;

// GLOBAL: CARMA2_HW 0x0074d3dc
// GLOBAL: CARMA2_SW 0x0074393c
// GLOBAL: CARMA2_CHEAT 0x00754a7c
int gAFE;

// GLOBAL: CARMA2_HW 0x0068b894
// GLOBAL: CARMA2_SW 0x00685df4
// GLOBAL: CARMA2_CHEAT 0x0069313c
int gNoCutscenes;

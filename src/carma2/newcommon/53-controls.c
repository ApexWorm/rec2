/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "53-controls.h"

#include "00-car.h"
#include "01-network.h"
#include "04-netgame.h"
#include "06-apo.h"
#include "07-structur.h"
#include "11-world1.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "23-piping.h"
#include "24-loadsave.h"
#include "25-grafdata.h"
#include "27-powerup.h"
#include "29-raycast.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "42-input.h"
#include "43-pratcam.h"
#include "49-physics.h"
#include "52-errors.h"
#include "58-crush2.h"
#include "59-camera.h"
#include "62-graphics3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "joystick.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <s3/internal/audio.h>
#include <stdio.h>

// GLOBAL: CARMA2_HW 0x00659b28
// GLOBAL: CARMA2_SW 0x00654510
int gCheckpoint_finder_enabled = 1;

// GLOBAL: CARMA2_HW 0x00659b2c
// GLOBAL: CARMA2_SW 0x00654514
// GLOBAL: CARMA2_CHEAT 0x00660444
float gMap_render_x = 80.f;

// GLOBAL: CARMA2_HW 0x00659b30
// GLOBAL: CARMA2_SW 0x00654518
// GLOBAL: CARMA2_CHEAT 0x00660448
float gMap_render_y = 6.f;

// GLOBAL: CARMA2_HW 0x00659b34
// GLOBAL: CARMA2_SW 0x0065451c
// GLOBAL: CARMA2_CHEAT 0x0066044c
float gMap_render_width = 128.f;

// GLOBAL: CARMA2_HW 0x00659b38
// GLOBAL: CARMA2_SW 0x00654520
// GLOBAL: CARMA2_CHEAT 0x00660450
float gMap_render_height = 80.f;

// GLOBAL: CARMA2_HW 0x0068d8d4
// GLOBAL: CARMA2_SW 0x0068792c
// GLOBAL: CARMA2_CHEAT 0x00695174
int gMap_trans;

// GLOBAL: CARMA2_HW 0x0074abac
// GLOBAL: CARMA2_SW 0x00741258
// GLOBAL: CARMA2_CHEAT 0x0075224c
int gHeadup_map_x;

// GLOBAL: CARMA2_HW 0x0074abb0
// GLOBAL: CARMA2_SW 0x0074125c
// GLOBAL: CARMA2_CHEAT 0x00752250
int gHeadup_map_y;

// GLOBAL: CARMA2_HW 0x0074aba8
// GLOBAL: CARMA2_SW 0x00741254
// GLOBAL: CARMA2_CHEAT 0x00752248
int gHeadup_map_w;

// GLOBAL: CARMA2_HW 0x0074abb4
// GLOBAL: CARMA2_SW 0x00741260
// GLOBAL: CARMA2_CHEAT 0x00752254
int gHeadup_map_h;

// GLOBAL: CARMA2_HW 0x0067c400
// GLOBAL: CARMA2_SW 0x00676b78
// GLOBAL: CARMA2_CHEAT 0x00683cb0
char gString[84];

// GLOBAL: CARMA2_HW 0x0067c498
// GLOBAL: CARMA2_SW 0x00676c10
// GLOBAL: CARMA2_CHEAT 0x00683d40
int gInvulnerability;

// GLOBAL: CARMA2_HW 0x0067c3b8
// GLOBAL: CARMA2_SW 0x00676b30
// GLOBAL: CARMA2_CHEAT 0x00683c68
int gToo_late;

// GLOBAL: CARMA2_HW 0x0067c3bc
// GLOBAL: CARMA2_SW 0x00676b34
// GLOBAL: CARMA2_CHEAT 0x00683c6c
int gRecover_timer;

// GLOBAL: CARMA2_HW 0x0067c3c8
// GLOBAL: CARMA2_SW 0x00676b40
// GLOBAL: CARMA2_CHEAT 0x00683c78
char* gAbuse_text[10] = { 0 };

// GLOBAL: CARMA2_HW 0x0067c3f0
// GLOBAL: CARMA2_SW 0x00676b68
int gINT_0067c3f0 = 0;

// GLOBAL: CARMA2_HW 0x0067c3f8
// GLOBAL: CARMA2_SW 0x00676b70
// GLOBAL: CARMA2_CHEAT 0x00683ca8
int gRecovery_voucher_count;

// GLOBAL: CARMA2_HW 0x0067c45c
// GLOBAL: CARMA2_SW 0x00676bd4
// GLOBAL: CARMA2_CHEAT 0x00683d0c
int gRepair_last_time;

// GLOBAL: CARMA2_HW 0x0067c460
// GLOBAL: CARMA2_SW 0x00676bd8
// GLOBAL: CARMA2_CHEAT 0x00683d10
int gHad_auto_recover;

// GLOBAL: CARMA2_HW 0x0067c464
// GLOBAL: CARMA2_SW 0x00676bdc
// GLOBAL: CARMA2_CHEAT 0x00683d14
tU32 gLast_repair_time;

// GLOBAL: CARMA2_HW 0x0067c46c
// GLOBAL: CARMA2_SW 0x00676be4
// GLOBAL: CARMA2_CHEAT 0x00597888
int gAllow_car_flying;

// GLOBAL: CARMA2_HW 0x0067c470
// GLOBAL: CARMA2_SW 0x00676be8
// GLOBAL: CARMA2_CHEAT 0x00683d18
int gINT_0067c470;

// GLOBAL: CARMA2_HW 0x0067c474
// GLOBAL: CARMA2_SW 0x00676bec
// GLOBAL: CARMA2_CHEAT 0x00683d1c
int gEntering_message;

// GLOBAL: CARMA2_HW 0x0067c47c
// GLOBAL: CARMA2_SW 0x00676bf4
// GLOBAL: CARMA2_CHEAT 0x00683d24
tU32 gToo_poor_for_recovery_timeout;

// GLOBAL: CARMA2_HW 0x0079ec54
// GLOBAL: CARMA2_SW 0x007951b4
// GLOBAL: CARMA2_CHEAT 0x007a62f4
int gAuto_repair;

// GLOBAL: CARMA2_HW 0x0079efa4
// GLOBAL: CARMA2_SW 0x00795544
// GLOBAL: CARMA2_CHEAT 0x007a6644
tCamera_key_flags gCamera_key_flags;

// GLOBAL: CARMA2_HW 0x006b781c
// GLOBAL: CARMA2_SW 0x006ae4fc
// GLOBAL: CARMA2_CHEAT 0x006beebc
tRace_over_reason gRace_over_reason;

// GLOBAL: CARMA2_HW 0x0067c478
// GLOBAL: CARMA2_SW 0x00676bf0
// GLOBAL: CARMA2_CHEAT 0x00683d20
tU32 gPalette_fade_time;

// GLOBAL: CARMA2_HW 0x005900a0
// GLOBAL: CARMA2_SW 0x0058b098
tToggle_element gToggle_array[44] = {
    { 35, -2, 1, 1, 0, ToggleMiniMap },
    { 74, -2, 1, 1, 0, ToggleFlaps },
    { 57, -2, 1, 1, 0, ToggleMap },
    { 57, 8, 1, 1, 0, (void (C2_HOOK_FASTCALL*)(void))ToggleHeadupMap },
    { 57, 7, 1, 1, 0, CycleMapOptions },
    { 57, 9, 1, 1, 0, ToggleMapTransparency },
    { 60, -2, 1, 1, 0, SetRecovery },
    { 4, 7, 1, 0, 0, (void (C2_HOOK_FASTCALL*)(void))AbortRace },
    { 37, -2, 0, 1, 0, ToggleSoundEnable },
    { 5, 8, 0, 1, 0, PrintScreen },
    { 76, -2, 1, 0, 0, ToggleHeadupLevel },
    { 18, -2, 0, 0, 0, F4Key },
    { 19, -2, 1, 0, 0, F5Key },
    { 20, -2, 1, 0, 0, F6Key },
    { 21, -2, 1, 0, 0, F7Key },
    { 22, -2, 1, 0, 0, F8Key },
    { 23, -2, 1, 0, 0, F10Key },
    { 24, -2, 1, 0, 0, F11Key },
    { 25, -2, 1, 0, 0, F12Key },
    { 15, -2, 1, 0, 0, NumberKey0 },
    { 38, -2, 1, 0, 0, NumberKey1 },
    { 39, -2, 1, 0, 0, NumberKey2 },
    { 40, -2, 1, 0, 0, NumberKey3 },
    { 41, -2, 1, 0, 0, NumberKey4 },
    { 42, -2, 1, 0, 0, NumberKey5 },
    { 43, -2, 1, 0, 0, NumberKey6 },
    { 44, -2, 1, 0, 0, NumberKey7 },
    { 16, -2, 1, 0, 0, NumberKey8 },
    { 17, -2, 1, 0, 0, NumberKey9 },
    { 11, 7, 1, 1, 0, ToggleFlying },
    { 65, -2, 1, 0, 0, ScreenSmaller },
    { 66, -2, 1, 0, 0, ScreenLarger },
    { 67, -2, 1, 0, 0, BuyArmour },
    { 68, -2, 1, 0, 0, BuyPower },
    { 69, -2, 1, 0, 0, BuyOffense },
    { 70, -2, 1, 0, 0, ViewNetPlayer },
    { 71, -2, 1, 0, 0, UserSendMessage },
    { 72, -2, 1, 0, 0, ToggleTargetLock },
    { 73, -2, 1, 0, 0, CycleTargetLock },
    { 62, -2, 1, 0, 0, ToggleInventory },
    { 63, -2, 1, 0, 0, LeftInventory },
    { 64, -2, 1, 0, 0, RightInventory },
    { 75, -2, 1, 0, 0, ChangeCameraTypeInGame },
    { 46, -2, 1, 0, 0, ToggleCockpit },
};

// GLOBAL: CARMA2_HW 0x005904c0
// GLOBAL: CARMA2_SW 0x0058b4b8
// GLOBAL: CARMA2_CHEAT 0x00595768
char* gEdit_mode_names[12] = {
    "Options",
    "Cheat",
    "Accessories",
    "Powerups",
    "Special Volumes",
    "Pedestrians",
    "Drones",
    "Opponents",
    "Prat-cam",
    "Depth effects",
    "Damage",
    "Crush Test",
};

// GLOBAL: CARMA2_HW 0x005904f0
// GLOBAL: CARMA2_SW 0x0058b4e8
// GLOBAL: CARMA2_CHEAT 0x00595798
tEdit_func* gEdit_funcs[eEdit_mode_count][18][8] = { 0 };

// GLOBAL: CARMA2_HW 0x00590f60
// GLOBAL: CARMA2_SW 0x0058bf58
// GLOBAL: CARMA2_CHEAT 0x0059788c
int gCancel_recover = 1;

// GLOBAL: CARMA2_HW 0x0067c468
// GLOBAL: CARMA2_SW 0x00676be0
// GLOBAL: CARMA2_CHEAT 0x00595760
tEdit_mode gWhich_edit_mode = eEdit_mode_default;

void C2_HOOK_FASTCALL CompleteRace(void) {
    gRace_over_reason = eRace_over_0;
    gAbandon_game = 2;
}

// FUNCTION: CARMA2_HW 0x00441490
// FUNCTION: CARMA2_SW 0x00441570
// FUNCTION: CARMA2_CHEAT 0x00441740
void C2_HOOK_FASTCALL AbortRace(tFrontend_spec* pFrontend) {

    if (!gRace_finished) {
#ifdef REC2_CHEAT
        if (PDKeyDown(0)) {
            gRace_over_reason = 0;
            gAbandon_game = 2;
            return;
        }
#endif
        gAbandon_game = 1;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004414b0
// STUB: CARMA2_SW 0x00441590
// STUB: CARMA2_CHEAT 0x00441780
void C2_HOOK_FASTCALL F4Key(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00441580
// FUNCTION: CARMA2_SW 0x00441660
// FUNCTION: CARMA2_CHEAT 0x00441850
void C2_HOOK_FASTCALL SetFlag(tU32 pFlag) {

    if (gI_am_cheating == pFlag) {
        gI_am_cheating = 0;
        NewTextHeadupSlot(4, 0, 3000, -4, "Cheating Off");
    } else {
        gI_am_cheating = pFlag;
    }
    F4Key();
}

// FUNCTION: CARMA2_HW 0x004415c0
// FUNCTION: CARMA2_SW 0x004416a0
// FUNCTION: CARMA2_CHEAT 0x00441890
void C2_HOOK_FASTCALL FinishRace(int pArg) {
    (void)pArg;
    CompleteRace();
}

// FUNCTION: CARMA2_HW 0x004415e0
// FUNCTION: CARMA2_SW 0x004416c0
// FUNCTION: CARMA2_CHEAT 0x004418b0
void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {

#ifdef REC2_CHEAT
    if (gWhich_edit_mode == eEdit_mode_spec_vol) {
        HideSpecialVolumes();
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004415f0
// FUNCTION: CARMA2_SW 0x004416d0
// FUNCTION: CARMA2_CHEAT 0x004418c0
void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {

#ifdef REC2_CHEAT
    if (gWhich_edit_mode == eEdit_mode_spec_vol) {
        ShowSpecialVolumes();
    }
#endif
}

void C2_HOOK_FASTCALL DoEditModeKey(int pIndex) {
    int modifiers;

#ifndef REC2_CHEAT
    if (!(gI_am_cheating == 0xa11ee75d || (gI_am_cheating == 0x564e78b9 && gNet_mode == eNet_mode_none))) {
        gWhich_edit_mode = eEdit_mode_options;
        return;
    }
#endif

    modifiers = 0;
    modifiers |= PDKeyDown(0) ? 0x4 : 0;
    modifiers |= PDKeyDown(1) ? 0x2 : 0;
    modifiers |= PDKeyDown(2) ? 0x1 : 0;
    if (gEdit_funcs[gWhich_edit_mode][pIndex][modifiers] != NULL) {
        (*gEdit_funcs[gWhich_edit_mode][pIndex][modifiers])();
    }
}

// FUNCTION: CARMA2_HW 0x00441600
// FUNCTION: CARMA2_SW 0x004416e0
// FUNCTION: CARMA2_CHEAT 0x004418d0
void C2_HOOK_FASTCALL F5Key(void) {

    DoEditModeKey(0);
}

// FUNCTION: CARMA2_HW 0x00441680
// FUNCTION: CARMA2_SW 0x00441760
// FUNCTION: CARMA2_CHEAT 0x00441930
void C2_HOOK_FASTCALL F6Key(void) {

    DoEditModeKey(1);
}

// FUNCTION: CARMA2_HW 0x00441700
// FUNCTION: CARMA2_SW 0x004417e0
// FUNCTION: CARMA2_CHEAT 0x00441990
void C2_HOOK_FASTCALL F7Key(void) {

    DoEditModeKey(2);
}

// FUNCTION: CARMA2_HW 0x00441780
// FUNCTION: CARMA2_SW 0x00441860
// FUNCTION: CARMA2_CHEAT 0x004419f0
void C2_HOOK_FASTCALL F8Key(void) {

    DoEditModeKey(3);
}

// FUNCTION: CARMA2_HW 0x00441800
// FUNCTION: CARMA2_SW 0x004418e0
// FUNCTION: CARMA2_CHEAT 0x00441a50
void C2_HOOK_FASTCALL F10Key(void) {

    DoEditModeKey(4);
}

// FUNCTION: CARMA2_HW 0x00441880
// FUNCTION: CARMA2_SW 0x00441960
// FUNCTION: CARMA2_CHEAT 0x00441ab0
void C2_HOOK_FASTCALL F11Key(void) {

    DoEditModeKey(5);
}

// FUNCTION: CARMA2_HW 0x00441900
// FUNCTION: CARMA2_SW 0x004419e0
// FUNCTION: CARMA2_CHEAT 0x00441b10
void C2_HOOK_FASTCALL F12Key(void) {

#ifdef REC2_CHEAT
    DoEditModeKey(6);
#endif
}

// FUNCTION: CARMA2_HW 0x00441910
// FUNCTION: CARMA2_SW 0x004419f0
// FUNCTION: CARMA2_CHEAT 0x00441b70
void C2_HOOK_FASTCALL NumberKey0(void) {

    DoEditModeKey(7);
}

// FUNCTION: CARMA2_HW 0x00441990
// FUNCTION: CARMA2_SW 0x00441a70
// FUNCTION: CARMA2_CHEAT 0x00441bd0
void C2_HOOK_FASTCALL NumberKey1(void) {

    DoEditModeKey(8);
}

// FUNCTION: CARMA2_HW 0x00441a10
// FUNCTION: CARMA2_SW 0x00441af0
// FUNCTION: CARMA2_CHEAT 0x00441c30
void C2_HOOK_FASTCALL NumberKey2(void) {

    DoEditModeKey(9);
}

// FUNCTION: CARMA2_HW 0x00441a90
// FUNCTION: CARMA2_SW 0x00441b70
// FUNCTION: CARMA2_CHEAT 0x00441c90
void C2_HOOK_FASTCALL NumberKey3(void) {

    DoEditModeKey(10);
}

// FUNCTION: CARMA2_HW 0x00441b10
// FUNCTION: CARMA2_SW 0x00441bf0
// FUNCTION: CARMA2_CHEAT 0x00441cf0
void C2_HOOK_FASTCALL NumberKey4(void) {

    DoEditModeKey(11);
}

// FUNCTION: CARMA2_HW 0x00441b90
// FUNCTION: CARMA2_SW 0x00441c70
// FUNCTION: CARMA2_CHEAT 0x00441d50
void C2_HOOK_FASTCALL NumberKey5(void) {

    DoEditModeKey(12);
}

// FUNCTION: CARMA2_HW 0x00441c10
// FUNCTION: CARMA2_SW 0x00441cf0
// FUNCTION: CARMA2_CHEAT 0x00441db0
void C2_HOOK_FASTCALL NumberKey6(void) {

    DoEditModeKey(13);
}

// FUNCTION: CARMA2_HW 0x00441c90
// FUNCTION: CARMA2_SW 0x00441d70
// FUNCTION: CARMA2_CHEAT 0x00441e10
void C2_HOOK_FASTCALL NumberKey7(void) {

    DoEditModeKey(14);
}

// FUNCTION: CARMA2_HW 0x00441d10
// FUNCTION: CARMA2_SW 0x00441df0
// FUNCTION: CARMA2_CHEAT 0x00441e70
void C2_HOOK_FASTCALL NumberKey8(void) {

    DoEditModeKey(15);
}

// FUNCTION: CARMA2_HW 0x00441d90
// FUNCTION: CARMA2_SW 0x00441e70
// FUNCTION: CARMA2_CHEAT 0x00441ed0
void C2_HOOK_FASTCALL NumberKey9(void) {

    DoEditModeKey(16);
}

// FUNCTION: CARMA2_HW 0x00441e10
// FUNCTION: CARMA2_SW 0x00441ef0
// FUNCTION: CARMA2_CHEAT 0x00441f30
void C2_HOOK_FASTCALL LookLeft(void) {

    PratcamEvent(27);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_left) {
        LookForward();
    } else if (gProgram_state.which_view == eView_right) {
        LookForward();
        gProgram_state.pending_view = eView_left;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_left;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441ec0
// FUNCTION: CARMA2_SW 0x00441fa0
// FUNCTION: CARMA2_CHEAT 0x00441fe0
void C2_HOOK_FASTCALL LookForward(void) {

    if (gProgram_state.which_view == eView_right) {
        PratcamEvent(27);
    } else if (gProgram_state.which_view == eView_left) {
        PratcamEvent(28);
    }
    if (gProgram_state.which_view != eView_forward) {
        gProgram_state.old_view = gProgram_state.which_view;
        ClearWobbles();
        gProgram_state.new_view = eView_forward;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441f20
// FUNCTION: CARMA2_SW 0x00442000
// FUNCTION: CARMA2_CHEAT 0x00442040
void C2_HOOK_FASTCALL LookRight(void) {

    PratcamEvent(28);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_right) {
        LookForward();
    } else if (gProgram_state.which_view == eView_left) {
        LookForward();
        gProgram_state.pending_view = eView_right;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_right;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441fd0
// FUNCTION: CARMA2_SW 0x004420b0
// FUNCTION: CARMA2_CHEAT 0x004420f0
void C2_HOOK_FASTCALL DamageTest(void) {

}

// FUNCTION: CARMA2_HW 0x00441fe0
// FUNCTION: CARMA2_SW 0x004420c0
// FUNCTION: CARMA2_CHEAT 0x00442100
void C2_HOOK_FASTCALL TDamageEngine(void) {
    DamageEngine(21);
}

// FUNCTION: CARMA2_HW 0x00441ff0
// FUNCTION: CARMA2_SW 0x004420d0
// FUNCTION: CARMA2_CHEAT 0x00442110
void C2_HOOK_FASTCALL TDamageDriver(void) {
    if (gProgram_state.current_car.damage_units[eDamage_driver].damage_level < 80) {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 80 - gProgram_state.current_car.damage_units[eDamage_driver].damage_level);
    } else {
        DamageUnit(&gProgram_state.current_car, eDamage_driver, 2);
    }
}

// FUNCTION: CARMA2_HW 0x00442030
// FUNCTION: CARMA2_SW 0x00442110
// FUNCTION: CARMA2_CHEAT 0x00442150
void C2_HOOK_FASTCALL TDamageTrans(void) {
    DamageTrans(21);
}

// FUNCTION: CARMA2_HW 0x00442040
// FUNCTION: CARMA2_SW 0x00442120
// FUNCTION: CARMA2_CHEAT 0x00442160
void C2_HOOK_FASTCALL TDamageSteering(void) {

    DamageSteering(21);
}

// FUNCTION: CARMA2_HW 0x00442050
// FUNCTION: CARMA2_SW 0x00442130
// FUNCTION: CARMA2_CHEAT 0x00442170
void C2_HOOK_FASTCALL TDamageLFWheel(void) {

    DamageLFWheel(21);
}

// FUNCTION: CARMA2_HW 0x00442060
// FUNCTION: CARMA2_SW 0x00442140
// FUNCTION: CARMA2_CHEAT 0x00442180
void C2_HOOK_FASTCALL TDamageLFBrake(void) {

    DamageLFBrake(21);
}

// FUNCTION: CARMA2_HW 0x00442070
// FUNCTION: CARMA2_SW 0x00442150
// FUNCTION: CARMA2_CHEAT 0x00442190
void C2_HOOK_FASTCALL TDamageLRBrake(void) {

    DamageLRBrake(21);
}

// FUNCTION: CARMA2_HW 0x00442080
// FUNCTION: CARMA2_SW 0x00442160
// FUNCTION: CARMA2_CHEAT 0x004421a0
void C2_HOOK_FASTCALL TDamageLRWheel(void) {

    DamageLRWheel(21);
}

// FUNCTION: CARMA2_HW 0x00442090
// FUNCTION: CARMA2_SW 0x00442170
// FUNCTION: CARMA2_CHEAT 0x004421b0
void C2_HOOK_FASTCALL TDamageRFWheel(void) {

    DamageRFWheel(21);
}

// FUNCTION: CARMA2_HW 0x004420a0
// FUNCTION: CARMA2_SW 0x00442180
// FUNCTION: CARMA2_CHEAT 0x004421c0
void C2_HOOK_FASTCALL TDamageRFBrake(void) {

    DamageRFBrake(21);
}

// FUNCTION: CARMA2_HW 0x004420b0
// FUNCTION: CARMA2_SW 0x00442190
// FUNCTION: CARMA2_CHEAT 0x004421d0
void C2_HOOK_FASTCALL TDamageRRBrake(void) {

    DamageRRBrake(21);
}

// FUNCTION: CARMA2_HW 0x004420c0
// FUNCTION: CARMA2_SW 0x004421a0
// FUNCTION: CARMA2_CHEAT 0x004421e0
void C2_HOOK_FASTCALL TDamageRRWheel(void) {

    DamageRRWheel(21);
}

// FUNCTION: CARMA2_HW 0x004420e0
// FUNCTION: CARMA2_SW 0x004421c0
// FUNCTION: CARMA2_CHEAT 0x00442200
void C2_HOOK_FASTCALL ToggleMiniMap(void) {

    if (gMap_view != 2) {
        gMini_map_visible = !gMini_map_visible;
        if (!gMini_map_visible) {
            NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_off));
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(eMiscString_minimap_on));
        }
        MapChanged();
    }
}

inline void C2_HOOK_FASTCALL CheckToggles(int pRacing) {
    int i;
    int new_state;

    CheckKeysForMouldiness();
    for (i = 0; i < REC2_ASIZE(gToggle_array); i++) {
        if (gToggle_array[i].in_game_only && !pRacing) {
            continue;
        }
        if ((gTyping || gEntering_message) && gToggle_array[i].key2 == -2) {
            continue;
        }
        new_state = (gToggle_array[i].key1 == -2 || KeyIsDownNoMouldiness(gToggle_array[i].key1))
            && ((gToggle_array[i].key2 == -2 && gToggle_array[i].exact_modifiers) ? (!PDKeyDown(0) && !PDKeyDown(1) && !PDKeyDown(2) && !PDKeyDown(3)) : KeyIsDown(gToggle_array[i].key2));
        if (gToggle_array[i].on_last_time != new_state) {
            gToggle_array[i].on_last_time = new_state;
            if (new_state) {
                gToggle_array[i].action_proc();
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00442140
// FUNCTION: CARMA2_SW 0x00442220
// FUNCTION: CARMA2_CHEAT 0x00442260
int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around) {
    br_vector3 car_pos;
    br_vector3 offset_c;
    br_vector3 offset_w;
    int result;
    int i;

    if (gCar_flying && pCar != NULL && pCar->driver == eDriver_local_human) {
        return 0;
    }
    if (gAction_replay_mode) {
        return 0;
    }
    if (pCar->number_of_wheels_on_ground != 0) {
        for (i = pCar->collision_info->box_face_start; i < pCar->collision_info->box_face_end; i++) {
            if (gFace_list__car[i].material != NULL
                    && gFace_list__car[i].material->identifier != NULL
                    && gFace_list__car[i].material->identifier[0] == '^') {
                return 1;
            }
        }
        return 0;
    }
    BrVector3Copy(&car_pos, &pCar->car_master_actor->t.t.translate.t);
    car_pos.v[1] += 0.5f;
    if (FindYVerticallyBelow2(&car_pos) < -100.f) {
        BrVector3Set(&offset_c, 0.f, 1.f, 0.f);
        BrMatrix34ApplyV(&offset_w, &offset_c, &pCar->car_master_actor->t.t.mat);
        BrVector3Add(&car_pos, &pCar->car_master_actor->t.t.translate.t, &offset_w);
        if (FindYVerticallyBelow2(&car_pos) < -100.f) {
            if (pCheck_around) {
                pCar->car_master_actor->t.t.translate.t.v[0] += 0.05f;
                result = CarWorldOffFallenCheckThingy(pCar, 0);
                pCar->car_master_actor->t.t.translate.t.v[0] -= 0.05f;
                if (!result) {
                    return 0;
                }
                pCar->car_master_actor->t.t.translate.t.v[2] += 0.05f;
                result = CarWorldOffFallenCheckThingy(pCar, 0);
                pCar->car_master_actor->t.t.translate.t.v[2] -= 0.05f;
                return result;
            } else {
                return 1;
            }
        }
    } else if (gMaterial_below->identifier != NULL && gMaterial_below->identifier[0] == '^') {
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00442130
// FUNCTION: CARMA2_SW 0x00442210
// FUNCTION: CARMA2_CHEAT 0x00442250
int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar) {

    return CarWorldOffFallenCheckThingy(pCar, 1);
}

inline void C2_HOOK_FASTCALL CheckForBeingOutOfThisWorld(void) {
    // GLOBAL: CARMA2_HW 0x0067c3f4
    // GLOBAL: CARMA2_SW 0x00676b6c
    // GLOBAL: CARMA2_CHEAT 0x00683ca4
    static tU32 the_time;
    // GLOBAL: CARMA2_HW 0x0067c480
    // GLOBAL: CARMA2_SW 0x00676bf8
    // GLOBAL: CARMA2_CHEAT 0x00683d28
    static tU32 sLast_check;
    int time_step;

    time_step = PDGetTotalTime() - the_time;
    the_time += time_step;
    if (gProgram_state.current_car.disabled) {
        gRecover_timer = 0;
        return;
    }
    if (gRecover_timer != 0) {
        if ((gProgram_state.current_car.frame_collision_flag || gProgram_state.current_car.number_of_wheels_on_ground) && !IsCarInTheSea()) {
            gRecover_timer = 0;
        } else {
            gRecover_timer -= gFrame_period;
            if (gRecover_timer <= 0 || IsCarInTheSea() == 2) {
                gRecover_timer = 0;
                SetQuickRecovery();
                gHad_auto_recover = 1;
            }
            return;
        }
    }
    if (the_time - sLast_check > 200) {
        sLast_check = the_time;
        if (HasCarFallenOffWorld(&gProgram_state.current_car)) {
            gRecover_timer = 3000;
        }
    }
    if (IsCarInTheSea()) {
        if (gRecover_timer == 0) {
            gRecover_timer = 3000;
        }
    }
}

inline void C2_HOOK_FASTCALL CheckHornLocal(tCar_spec* pCar) {

    if (pCar->keys.horn && pCar->horn_sound_tag == 0) {
        pCar->horn_sound_tag = DRS3StartSound(gEffects_outlet, 5209);
    } else if (!pCar->keys.horn && pCar->horn_sound_tag != 0) {
        while (S3SoundStillPlaying(pCar->horn_sound_tag)) {
            DRS3StopSound(pCar->horn_sound_tag);
            DRS3StopOutletSound(gEffects_outlet);
        }
        if (!S3SoundStillPlaying(pCar->horn_sound_tag)) {
            pCar->horn_sound_tag = 0;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00443ba0
// FUNCTION: CARMA2_SW 0x00443c80
// FUNCTION: CARMA2_CHEAT 0x00443cc0
void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar) {

    if (pCar->keys.horn && pCar->horn_sound_tag == 0) {
        pCar->horn_sound_tag = DRS3StartSound3D(gEffects_outlet, 5209,
            &pCar->car_master_actor->t.t.translate.t, &pCar->vel,
            0, 255, ((pCar->car_ID & 0x7) + 12) * 0x1000, 0x10000);
    } else if (!pCar->keys.horn && pCar->horn_sound_tag != 0) {
        while (S3SoundStillPlaying(pCar->horn_sound_tag)) {
            DRS3StopSound(pCar->horn_sound_tag);
            DRS3StopOutletSound(gEffects_outlet);
        }
        if (!S3SoundStillPlaying(pCar->horn_sound_tag)) {
            pCar->horn_sound_tag = 0;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL CheckHorns(void) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        CheckHornLocal(&gProgram_state.current_car);
    } else {
        for (i = 0; i < gNumber_of_net_players; i++) {
            CheckHorn3D(gNet_players[i].car);
        }
    }
}

inline void C2_HOOK_FASTCALL SetQuickRecovery(void) {

    if (gNet_mode == eNet_mode_none) {
        RecoverCar();
    } else if (gProgram_state.current_car.time_to_recover == 0 && !CheckRecoverCost()) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_cant_afford_to_recover));
        DoFancyHeadup(27);
        KnackerThisCar(&gProgram_state.current_car);
        SendGameplayToHost(3, 0, 0, 0, 0);
    } else if (gProgram_state.current_car.time_to_recover == 0 || GetRaceTime() + 500 < gProgram_state.current_car.time_to_recover) {
        gProgram_state.current_car.time_to_recover = GetRaceTime() + 500;
        gCancel_recover = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00442300
// FUNCTION: CARMA2_SW 0x004423e0
// FUNCTION: CARMA2_CHEAT 0x00442420
void C2_HOOK_FASTCALL SetRecovery(void) {

    if (!gINT_0068b8e4
        && !gINT_0068b8e8
        && !gRace_finished
        && !gProgram_state.current_car.knackered
        && !gWait_for_it
        && !gHad_auto_recover
        && !gPalette_fade_time) {
        if (gNet_mode != eNet_mode_none) {
            if (gProgram_state.current_car.time_to_recover) {
                if (gCancel_recover) {
                    if (gProgram_state.current_car.time_to_recover > GetRaceTime() + 600) {
                        gProgram_state.current_car.time_to_recover = 0;
                        if (!gINT_0067c470) {
                            EarnCredits((int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
                        }
                        NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_recovery_cancelled), 0);
                    } else {
                        NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_too_late_to_cancel), 1);
                        gToo_late = 1;
                    }
                }
            } else {
                gCancel_recover = 1;
                if (CheckRecoverCost()) {
                    if (gCurrent_net_game->type == eNet_game_type_foxy) {
                        if (gThis_net_player_index == gIt_or_fox) {
                            gProgram_state.current_car.time_to_recover = GetRaceTime() + 5000;
                        } else {
                            gProgram_state.current_car.time_to_recover = GetRaceTime() + 1000;
                        }
                    } else if (gCurrent_net_game->type == eNet_game_type_6 && gNet_players[gThis_net_player_index].field_0x80 != 0) {
                        gProgram_state.current_car.time_to_recover = GetRaceTime() + 500;
                    } else {
                        gProgram_state.current_car.time_to_recover = GetRaceTime() + 3000;
                    }
                    gToo_late = 0;
                }
            }
        } else {
            gRecover_car = 1;
            gRecover_timer = 0;
        }
    }
}

inline void C2_HOOK_FASTCALL RecoverCar(void) {

    if (gNet_mode == eNet_mode_none || gPalette_fade_time == 0) {
        gRecover_car = 1;
    }
    gProgram_state.current_car.time_to_recover = 0;
}

inline void C2_HOOK_FASTCALL CheckMapRenderMove(void) {
    int amount;
    float old_x;
    float old_y;

    old_y = gMap_render_y;
    old_x = gMap_render_x;
    if (gMap_view == 2) {
        amount = (int)(gFrame_period * 0.1);
        KeyIsDown(8);
        if (KeyIsDown(31)) {
            gMap_render_y -= (float)amount;
        } else if (KeyIsDown(32)) {
            gMap_render_y += (float)amount;
        }
        if (KeyIsDown(33)) {
            gMap_render_x -= (float)amount;
        } else if (KeyIsDown(34)) {
            gMap_render_x += (float)amount;
        }
        if (old_x != gMap_render_x || old_y != gMap_render_y) {
            SetIntegerMapRenders();
            if (gMap_render_x_i < gCurrent_graf_data->map_render_x_marg) {
                gMap_render_x = (float)((gCurrent_graf_data->map_render_x_marg + 3) & ~0x3);
            }
            if (gMap_render_y_i < gCurrent_graf_data->map_render_y_marg) {
                gMap_render_y = (float)((gCurrent_graf_data->map_render_y_marg + 1) & ~0x1);
            }
            if (gBack_screen->width - gMap_render_width_i - gCurrent_graf_data->map_render_x_marg < gMap_render_x_i) {
                gMap_render_x = (float)((gBack_screen->width - gMap_render_width_i - gCurrent_graf_data->map_render_x_marg) & ~0x3);
            }
            if (gBack_screen->height - gMap_render_height_i - gCurrent_graf_data->map_render_y_marg < gMap_render_y_i) {
                gMap_render_y = (float)((gBack_screen->height - gMap_render_height_i - gCurrent_graf_data->map_render_y_marg) & ~0x1);
            }
            SetIntegerMapRenders();
            AdjustRenderScreenSize();
        }
    }
}

// ExplodeCar

// CheckRecoveryOfCars

inline int C2_HOOK_FASTCALL TradeInAPO(int pCost) {
    int type;

    while (pCost > 0
        && (gProgram_state.current_car.power_up_levels[0] != 0
            || gProgram_state.current_car.power_up_levels[1] != 0
            || gProgram_state.current_car.power_up_levels[2] != 0)) {
        type = IRandomBetween(0, (int)REC2_ASIZE(gProgram_state.current_car.power_up_levels) - 1);
        if (gProgram_state.current_car.power_up_levels[type] != 0) {
            gProgram_state.current_car.power_up_levels[type]--;
            pCost -= gNet_mode != eNet_mode_none ? gTrade_in_value_APO.initial_network[gCurrent_net_game->type] : gTrade_in_value_APO.initial[gProgram_state.skill_level];
            gProgram_state.credits += gNet_mode != eNet_mode_none ? gTrade_in_value_APO.initial_network[gCurrent_net_game->type] : gTrade_in_value_APO.initial[gProgram_state.skill_level];
        }
    }
    if (pCost < 0) {
        pCost = 0;
    }
    return pCost;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00442f90
// FUNCTION: CARMA2_SW 0x00443070
// FUNCTION: CARMA2_CHEAT 0x004430b0
void C2_HOOK_FASTCALL CheckOtherRacingKeys(void) {
    int i;
    int new_level;
    int old_level;
    char s[256];
    tU32 cost;
    br_scalar ts;
    tCar_spec* car;
    br_scalar bodywork_repair_amount;
    // GLOBAL: CARMA2_HW 0x0067c484
    // GLOBAL: CARMA2_SW 0x00676bfc
    // GLOBAL: CARMA2_CHEAT 0x00683d2c
    static tU32 total_repair_cost;
    // GLOBAL: CARMA2_HW 0x0067c488
    // GLOBAL: CARMA2_SW 0x00676c00
    // GLOBAL: CARMA2_CHEAT 0x00683d30
    static tS3_sound_tag sound_tag;
    // GLOBAL: CARMA2_HW 0x0067c3fc
    // GLOBAL: CARMA2_SW 0x00676b74
    // GLOBAL: CARMA2_CHEAT 0x00683cac
    static br_scalar amount;
    // GLOBAL: CARMA2_HW 0x0067c48c
    // GLOBAL: CARMA2_SW 0x00676c04
    // GLOBAL: CARMA2_CHEAT 0x00683d34
    static int NeedToExpandBoundingBox;
    // GLOBAL: CARMA2_HW 0x0067c3c0
    // GLOBAL: CARMA2_SW 0x00676b38
    // GLOBAL: CARMA2_CHEAT 0x00683c70
    static int total_difference;
    // GLOBAL: CARMA2_HW 0x0067c490
    // GLOBAL: CARMA2_SW 0x00676c08
    // GLOBAL: CARMA2_CHEAT 0x00683d38
    static int stopped_repairing;

    car = GetCarSpec(eVehicle_self, 0);
    CheckMapRenderMove();
    CheckHorns();
    CheckForBeingOutOfThisWorld();
    if (gPalette_fade_time == 0) {
        if (gNet_mode != eNet_mode_none && NetGetPlayerStatus() == ePlayer_status_recovering) {
            NetPlayerStatusChanged(ePlayer_status_racing);
        }
    } else {
        SortOutRecover(car);
    }
    if ((gAuto_repair || KeyIsDown(45))
        && !gRace_finished
        && !gProgram_state.current_car.knackered
        && !gWait_for_it
        && !gEntering_message) {
        if (gProgram_state.current_car.car_crush_spec != NULL && gProgram_state.current_car.car_crush_spec->field_0x4b8 != 0) {
            FlipUpCar(&gProgram_state.current_car);
        }
        if (!gAuto_repair && gRepair_last_time == 0 && GetTotalTime() - gLast_repair_time < 1200) {
            gAuto_repair = 1;
        }
        gLast_repair_time = GetTotalTime();
        gRepair_last_time = 1;
        if (!NeedToExpandBoundingBox) {
            bodywork_repair_amount = RepairCar(gProgram_state.current_car.car_ID, gFrame_period, &amount);
            if (bodywork_repair_amount > 0.0f) {
                NeedToExpandBoundingBox = 1;
            } else {
                NeedToExpandBoundingBox = 0;
            }
            cost = 0;
            for (i = 0; i < REC2_ASIZE(gProgram_state.current_car.damage_units); i++) {
                old_level = gProgram_state.current_car.damage_units[i].damage_level;
                if (amount == 0.0f) {
                    new_level = 0;
                } else {
                    new_level = (int)(old_level - floor(old_level * (bodywork_repair_amount / amount)));
                }
                if (new_level < 0) {
                    gProgram_state.current_car.damage_units[i].damage_level = 0;
                } else if (new_level < 100) {
                    gProgram_state.current_car.damage_units[i].damage_level = new_level;
                } else {
                    gProgram_state.current_car.damage_units[i].damage_level = 99;
                }
                gProgram_state.current_car.damage_units[i].smoke_last_level = gProgram_state.current_car.damage_units[i].damage_level;
                if (gNet_mode == eNet_mode_none) {
                    ts = gRepair_cost.initial[gProgram_state.skill_level];
                } else {
                    ts = gRepair_cost.initial_network[gCurrent_net_game->type];
                }
                cost = (tU32)((old_level - gProgram_state.current_car.damage_units[i].damage_level) * ts + cost);
                total_difference += old_level - new_level;
            }
            total_difference %= 100;
            cost = (cost / 10) * 10;
            if (((total_repair_cost == 0 && cost != 0) || bodywork_repair_amount != 0.0f) && sound_tag == 0) {
                sound_tag = DRS3StartSound(gCar_outlet, eSoundId_DamageRepair);
            }
            StopCarSmoking(&gProgram_state.current_car);
            if (cost == 0 && bodywork_repair_amount == 0.0f) {
                gAuto_repair = 0;
            }
            if (!gFree_repairs) {
                if (cost > (tU32)gProgram_state.credits) {
                    TradeInAPO(cost);
                }
                SpendCredits(cost);
            }
            total_repair_cost += cost;
            if (total_repair_cost != 0) {
                if (gFree_repairs) {
                    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_repairing_for_free));
                } else {
                    sprintf(s, "%s %d", GetMiscString(eMiscString_repair_cost_colon), total_repair_cost);
                    NewTextHeadupSlot(4, 0, 1000, -4, s);
                }
            }
        }
    } else {
        (void)stopped_repairing;
        gRepair_last_time = 0;
        stopped_repairing = 0;
        total_repair_cost = 0;
        total_difference = 0;
        if (sound_tag != 0) {
            while (S3SoundStillPlaying(sound_tag)) {
                DRS3StopSound(sound_tag);
            }
            sound_tag = 0;
        }
    }
    if (NeedToExpandBoundingBox) {
        NeedToExpandBoundingBox = gProgram_state.current_car.car_crush_spec->expand_bounding_box;
    }
    if (gToo_poor_for_recovery_timeout != 0) {
        if (PDGetTotalTime() < gToo_poor_for_recovery_timeout) {
            if (PDKeyDown(51)) {
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_cant_afford_to_recover));
                DoFancyHeadup(27);
                KnackerThisCar(&gProgram_state.current_car);
                SendGameplayToHost(3, 0, 0, 0, 0);
            }
        } else {
            gToo_poor_for_recovery_timeout = 0;
        }
    }
    if (!gRecover_car || gProgram_state.current_car.knackered) {
        gHad_auto_recover = 0;
    } else {
        SetFlipUpCar(car);
        if (gNet_mode != eNet_mode_none) {
            NewTextHeadupSlot(4, 0, 1500, -4, " ");
        }
        if (gRecovery_voucher_count != 0
            || (gNet_mode != eNet_mode_none && NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80 != 0)) {
            if (gRecovery_voucher_count != 0) {
                gRecovery_voucher_count -= 1;
            }
            sprintf(s, "%s", GetMiscString(eMiscString_recovering_for_free));
        } else {
            if (gNet_mode == eNet_mode_none) {
                if (!gProgram_state.current_car.knackered
                    && gProgram_state.credits < (gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
                    TradeInAPO((int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
                }
                SpendCredits((int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
            }
            sprintf(s, "%s %d", GetMiscString(eMiscString_recovery_cost_colon), (int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
        }
        NewTextHeadupSlot(4, 0, 1500, -4, s);
        CancelPendingCunningStunt();
        PipeSingleSpecial(ePipe_special_action_recovery);
    }
    gRecover_car = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00442750
// FUNCTION: CARMA2_SW 0x00442830
// FUNCTION: CARMA2_CHEAT 0x00442870
int C2_HOOK_FASTCALL CheckRecoverCost(void) {

    if (gNet_mode != eNet_mode_none) {
        if (gProgram_state.current_car.knackered
            || gRecovery_voucher_count != 0
            || (NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80 != 0)) {
            gINT_0067c470 = 1;
        } else {
            gINT_0067c470 = 0;
            if (gProgram_state.credits < (gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
                TradeInAPO((int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
            }
            if (gProgram_state.credits < (gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level])) {
                gToo_poor_for_recovery_timeout = PDGetTotalTime() + 2000;
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_press_return_to_commit_suicide));
                return 0;
            }
            SpendCredits((int)(gNet_mode != eNet_mode_none ? gRecovery_cost.initial_network[gCurrent_net_game->type] : gRecovery_cost.initial[gProgram_state.skill_level]));
        }
    }
    return 1;
}

inline void C2_HOOK_FASTCALL SortOutRecover(tCar_spec* pCar) {
    int the_time;
    int val;
    // GLOBAL: CARMA2_HW 0x0067c494
    // GLOBAL: CARMA2_SW 0x00676c0c
    // GLOBAL: CARMA2_CHEAT 0x00683d3c
    static int old_time;

    the_time = GetRaceTime() - gPalette_fade_time;
    if (the_time < 0) {
        gPalette_fade_time = 0;
        old_time = 0;
    }
    if (the_time >= 500) {
        if (old_time < 500) {
            FlipUpCar(pCar);
            PipeSingleSpecial(ePipe_special_action_recovery);
        }
        val = ((the_time - 1000) * 256) / 500;
        if (val >= 256) {
            val = 256;
            gPalette_fade_time = 0;
            old_time = 0;
            SetHierarchyDoingNothingFlag(pCar->collision_info, 0);
        } else if (!pCar->collision_info->disable_move_rotate) {
            SetHierarchyDoingNothingFlag(pCar->collision_info, 1);
        }
    } else {
        val = 256 - (the_time * 256) / 500;
    }
    if (val <= 0) {
        val = 0;
    }
    SetFadedPalette(val);
    old_time = the_time;
}

// FUNCTION: CARMA2_HW 0x00442970
// FUNCTION: CARMA2_SW 0x00442a50
// FUNCTION: CARMA2_CHEAT 0x00442a90
void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar) {
    if (gNet_mode == eNet_mode_none || pCar == NULL || pCar->driver != eDriver_local_human) {
        FlipUpCar(pCar);
    } else {
        DisableCar(pCar);
        gPalette_fade_time = GetRaceTime();
        NetPlayerStatusChanged(ePlayer_status_recovering);
    }
}


// FUNCTION: CARMA2_HW 0x00442e10
// FUNCTION: CARMA2_SW 0x00442ef0
// FUNCTION: CARMA2_CHEAT 0x00442f30
void C2_HOOK_FASTCALL AlignChildren(tPhysics_object* pCollision_info) {
    tPhysics_object* child;

    for (child = pCollision_info->child; child != NULL; child = child->next) {
        int i;
        int j;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                child->transform_matrix.m[i][j] = pCollision_info->transform_matrix.m[i][j];
            }
        }
        BrVector3SetFloat(&child->v, 0.0f, 0.0f, 0.0f);
        BrVector3SetFloat(&child->omega, 0.0f, 0.0f, 0.0f);
        AlignChildren(child);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004429b0
// STUB: CARMA2_SW 0x00442a90
void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00442e80
// FUNCTION: CARMA2_SW 0x00442f60
// FUNCTION: CARMA2_CHEAT 0x00442fa0
void C2_HOOK_FASTCALL GetPowerup(int pNum) {

    GotPowerup(&gProgram_state.current_car, pNum);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00442e90
// FUNCTION: CARMA2_SW 0x00442f70
// FUNCTION: CARMA2_CHEAT 0x00442fb0
void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) {
    tU32 start_menu_time;

    start_menu_time = PDGetTotalTime();
    CheckQuit();
    AddLostTime(PDGetTotalTime() - start_menu_time);
    CheckToggles(pRacing);
    if (pRacing && !gAction_replay_mode) {
        CheckOtherRacingKeys();
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00443c50
// FUNCTION: CARMA2_SW 0x00443d30
// FUNCTION: CARMA2_CHEAT 0x00443d70
void C2_HOOK_FASTCALL CheckForCheatingGits(void) {
    tU32* keys;

    keys = KevKeyService();
    if (keys[0] == 0x616fb8e4 && keys[1] == 0x7c6100a8) {
        ToggleSoundEnable();
        gProgram_state.game_completed = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_FlaskGone);
    }
#ifdef REC2_CHEAT
    if (PDKeyDown3(21) && PDKeyDown3(0)) {
        gProgram_state.game_completed = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_FlaskGone);
    }
#endif
}

// CheckKevKeys

inline void C2_HOOK_FASTCALL BrakeInstantly(void) {
    int i;
    br_scalar speed_squared;

    gProgram_state.current_car.revs = 0.f;
    speed_squared = BrVector3Dot(&gProgram_state.current_car.collision_info->v, &gProgram_state.current_car.collision_info->v);
    if (gProgram_state.current_car.number_of_wheels_on_ground != 0 && speed_squared > .0001f) {
        PratcamEvent(41);
        for (i = 0; i < 5; i++) {
            DRS3StartSound(gCar_outlet, eSoundId_TyreScreeching + i);
        }
    }
    if (speed_squared > .1f) {
        PDPlayFFBEffect("Instant_Handbrake");
    }
    BrVector3Set(&gProgram_state.current_car.collision_info->v, 0.f, 0.f, 0.f);
}

// FUNCTION: CARMA2_HW 0x00443e80
// FUNCTION: CARMA2_SW 0x00443f60
// FUNCTION: CARMA2_CHEAT 0x00443fd0
void C2_HOOK_FASTCALL PollCarControls(tU32 pTime_difference) {
    tCar_controls keys = { 0 };
    tJoystick joystick;
    tCar_spec* c;

    c = &gProgram_state.current_car;
    joystick.left = -1;
    joystick.right = -1;
    joystick.acc = -1;
    joystick.dec = -1;
    CheckKeysForMouldiness();
    if (gEntering_message) {
        c->keys = keys;
        c->joystick = joystick;
    } else {

        if (gKey_mapping[47] < 143 && gKey_mapping[48] < 143) {
            if (KeyIsDownNoMouldiness(47)) {
                keys.left = 1;
            }
            if (KeyIsDownNoMouldiness(48)) {
                keys.right = 1;
            }
        }
        if (!(keys.left || keys.right)) {
            if (PDIsJoystickDPadEnabled()) {
                if (gJoy_array[0] > 10) {
                    keys.left = 1;
                }
                if (gJoy_array[1] > 10) {
                    keys.right = 1;
                }
            } else {
                joystick.left = gJoy_array[0];
                joystick.right = gJoy_array[1];
                if (joystick.left < 0 && joystick.right < 0) {
                    joystick.left = 0;
                }
            }
        }
        if (KeyIsDownNoMouldiness(13)) {
            keys.holdw = 1;
        }
        if (KeyIsDownNoMouldiness(54) || gRace_finished) {
            if (gInstant_handbrake && !gRace_finished) {
                BrakeInstantly();
            } else {
                keys.brake = 1;
            }
        }
        if (KeyIsDownNoMouldiness(49) && !gRace_finished && !c->knackered && !gWait_for_it) {
            keys.acc = 1;
        }
        if (!keys.acc) {
            if (PDIsJoystickDPadEnabled()) {
                if (gJoy_array[2] > 10) {
                    keys.acc = 1;
                }
            } else {
                if (HasCurrentJoystick()) {
                    joystick.acc = gJoy_array[2];
                }
            }
        }
        if (KeyIsDownNoMouldiness(50) && !gRace_finished && !c->knackered && !gWait_for_it) {
            keys.dec = 1;
        }
        if (!keys.dec) {
            if (PDIsJoystickDPadEnabled()) {
                if (gJoy_array[3] > 10) {
                    keys.dec = 1;
                }
            } else {
                if (HasCurrentJoystick()) {
                    joystick.dec = gJoy_array[3];
                }
            }
        }

        if (gCredit_multiplier) {
            int temp;

            temp = joystick.dec;
            joystick.dec = joystick.acc;
            joystick.acc = temp;

            temp = joystick.left;
            joystick.left = joystick.right;
            joystick.right = temp;

            temp = keys.dec;
            keys.dec = keys.acc;
            keys.acc = temp;

            temp = keys.left;
            keys.left = keys.right;
            keys.right = temp;
        }
        if (KeyIsDownNoMouldiness(56) && c->gear >= 0) {
            keys.change_down = 1;
            c->just_changed_gear = 1;
            if (keys.acc || joystick.acc > 32000) {
                c->traction_control = 0;
            } else if (c->gear > 1) {
                c->gear -= !c->keys.change_down;
            }
            if (gCountdown != 0 && !c->keys.change_down) {
                JumpTheStart();
            }
        }
        if (gCar_flying) {
            if (KeyIsDownNoMouldiness(14)) {
                keys.up = 1;
            }
            if (KeyIsDownNoMouldiness(12)) {
                keys.down = 1;
            }
        }
        if (KeyIsDownNoMouldiness(59)) {
            if (!gEntering_message) {
                keys.horn = 1;
            }
        }
        c->keys = keys;
        c->joystick = joystick;
    }
}

// FUNCTION: CARMA2_HW 0x00444270
// FUNCTION: CARMA2_SW 0x00444350
// FUNCTION: CARMA2_CHEAT 0x004443c0
void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period) {
    int left;
    int right;

    left = PDKeyDown(0);
    right = PDKeyDown(1);
    if (left || right) {
        CheckMapMoveKeys(left, right);
        gCamera_key_flags.field_0x0_bit1 = 0;
        gCamera_key_flags.field_0x0_bit2 = 0;
        gCamera_key_flags.field_0x0_bit3 = 0;
        gCamera_key_flags.field_0x0_bit4 = 0;
    } else {
        gCamera_key_flags.field_0x0_bit1 = KeyIsDownNoMouldiness(31);
        gCamera_key_flags.field_0x0_bit2 = KeyIsDownNoMouldiness(32);
        gCamera_key_flags.field_0x0_bit3 = KeyIsDownNoMouldiness(33);
        gCamera_key_flags.field_0x0_bit4 = KeyIsDownNoMouldiness(34);
        gCamera_key_flags.field_0x0_bit0 = PDKeyDown(2);
    }
}

// FUNCTION: CARMA2_HW 0x00444350
// FUNCTION: CARMA2_SW 0x00444430
// FUNCTION: CARMA2_CHEAT 0x004444a0
void C2_HOOK_FASTCALL SetFlag2(int pArg) {
    (void)pArg;
    gAllow_car_flying = 1;
    ToggleFlying();
    gAllow_car_flying = gCar_flying;
}

// FUNCTION: CARMA2_HW 0x004443c0
// FUNCTION: CARMA2_SW 0x004444a0
// FUNCTION: CARMA2_CHEAT 0x004444f0
void C2_HOOK_FASTCALL ToggleFlying(void) {
#ifdef REC2_CHEAT
    if (gAllow_car_flying) {
#else
    if (gAllow_car_flying && gNet_mode == eNet_mode_none) {
#endif
        gCar_flying = !gCar_flying;
        if (gCar_flying) {
            NewTextHeadupSlot(4, 0, 500, -4, "We have lift off!!");
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, "Back down to Earth");
        }
    } else {
        gCar_flying = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00444420
// FUNCTION: CARMA2_SW 0x00444500
// FUNCTION: CARMA2_CHEAT 0x00444540
void C2_HOOK_FASTCALL CycleInvulnerability(void) {
    char message[256];

    gInvulnerability = (gInvulnerability + 1) % 8;
    gCar_to_view->invulnerable_no_damage = gInvulnerability & 0x1;
    gCar_to_view->invulnerable_no_crushage = gInvulnerability & 0x2;
    gCar_to_view->invulnerable_no_wastage = gInvulnerability & 0x4;
    message[0] = '\0';
    if (gInvulnerability == 0) {
        strcpy(message, "FULLY VULNERABLE");
    }
    if (gCar_to_view->invulnerable_no_damage) {
        strcat(message, " NO DAMAGE");
    }
    if (gCar_to_view->invulnerable_no_crushage != 0) {
        strcat(message, " NO CRUSHAGE");
    }
    if (gCar_to_view->invulnerable_no_wastage != 0) {
        strcat(message, " NO WASTAGE");
    }
    NewTextHeadupSlot(4, 0, 1000, -4, message);
}

// FUNCTION: CARMA2_HW 0x00444590
// FUNCTION: CARMA2_SW 0x00444670
// FUNCTION: CARMA2_CHEAT 0x004446b0
void C2_HOOK_FASTCALL ToggleTimerFreeze(void) {

    gFreeze_timer = !gFreeze_timer;
    if (gFreeze_timer) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_timer_frozen));
    } else {
        NewTextHeadupSlot(4, 0, 1000, -4, "Timer thawed out");
    }
}

// FUNCTION: CARMA2_HW 0x004445e0
// FUNCTION: CARMA2_SW 0x004446c0
// FUNCTION: CARMA2_CHEAT 0x00444700
void C2_HOOK_FASTCALL EarnDosh(void) {

    EarnCredits(5000);
}

// FUNCTION: CARMA2_HW 0x004445f0
// FUNCTION: CARMA2_SW 0x004446d0
// FUNCTION: CARMA2_CHEAT 0x00444710
void C2_HOOK_FASTCALL LoseDosh(void) {
    EarnCredits(-5000);
}

// FUNCTION: CARMA2_HW 0x00444600
// FUNCTION: CARMA2_SW 0x004446e0
// FUNCTION: CARMA2_CHEAT 0x00444720
void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg) {

}

inline void C2_HOOK_FASTCALL HackyMapFixForMac3dfx(void) {

    if (gCurrent_race.map_image != NULL) {
        PixelmapSwapByteOrder(gCurrent_race.map_image);
    }
}

// FUNCTION: CARMA2_HW 0x00444610
// FUNCTION: CARMA2_SW 0x004446f0
// FUNCTION: CARMA2_CHEAT 0x00444730
void C2_HOOK_FASTCALL ToggleMap(void) {
    // GLOBAL: CARMA2_HW 0x0067c458
    // GLOBAL: CARMA2_SW 0x00676bd0
    // GLOBAL: CARMA2_CHEAT 0x00683d08
    static int old_indent;

    if (gMap_view == 2) {
        gMap_view = 1;
        gRender_indent = old_indent;
        HackyMapFixForMac3dfx();
    } else if (!gAction_replay_mode) {
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
        } else {
            old_indent = gRender_indent;
            gRender_indent = 0;
            gMap_view = 2;
            gMap_time = PDGetTotalTime();
            HackyMapFixForMac3dfx();
        }
    }
    AdjustRenderScreenSize();
}

// FUNCTION: CARMA2_HW 0x004446f0
// FUNCTION: CARMA2_SW 0x004447d0
// FUNCTION: CARMA2_CHEAT 0x00444880
int C2_HOOK_FASTCALL GetRecoverVoucherCount(void) {

    return gRecovery_voucher_count;
}

// FUNCTION: CARMA2_HW 0x00444700
// FUNCTION: CARMA2_SW 0x004447e0
// FUNCTION: CARMA2_CHEAT 0x00444890
void C2_HOOK_FASTCALL AddVouchers(int pCount) {

    gRecovery_voucher_count += pCount;
}

// FUNCTION: CARMA2_HW 0x00444710
// FUNCTION: CARMA2_SW 0x004447f0
// FUNCTION: CARMA2_CHEAT 0x004448a0
void C2_HOOK_FASTCALL ResetRecoveryVouchers(void) {

    gRecovery_voucher_count = 0;
}

// FUNCTION: CARMA2_HW 0x00444720
// FUNCTION: CARMA2_SW 0x00444800
// FUNCTION: CARMA2_CHEAT 0x004448b0
void C2_HOOK_FASTCALL CycleYonFactor(void) {
    float factor;

    factor = 0.5f * GetYonFactor();
    if (factor < 0.1f) {
        factor = 1.0f;
    }
    SetYonFactor(factor);
    if (factor > 0.75f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_quickly));
    } else if (factor > 0.375f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_quickly));
    } else if (factor > 0.187f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_late));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_late));
    }
}

// FUNCTION: CARMA2_HW 0x004447c0
// FUNCTION: CARMA2_SW 0x004448a0
// FUNCTION: CARMA2_CHEAT 0x00444950
void C2_HOOK_FASTCALL CycleSoundDetailLevel(void) {
    int new_level;

    new_level = (gSound_detail_level + 1) % 3;
    ReallySetSoundDetailLevel(new_level);
    SetSoundDetailLevel(new_level);
    switch (gSound_detail_level) {
    case 0:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_fewest_sounds));
        break;
    case 1:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_partial_sound));
        break;
    case 2:
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_all_sounds));
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00444820
// FUNCTION: CARMA2_SW 0x00444900
// FUNCTION: CARMA2_CHEAT 0x004449b0
void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void) {
    const char* src;
    char* dst;

    gCar_simplification_level++;
    gCar_simplification_level = gCar_simplification_level % 5u;
    src = GetMiscString(eMiscString_car_simplification_level_D);
    dst = BrMemAllocate(strlen(src) + 21, kMem_simp_level);
    sprintf(dst, src, gCar_simplification_level);
    NewTextHeadupSlot(4, 0, 2000, -4, dst);
    BrMemFree(dst);
}

// FUNCTION: CARMA2_HW 0x004448a0
// FUNCTION: CARMA2_SW 0x00444980
// FUNCTION: CARMA2_CHEAT 0x00444a30
void C2_HOOK_FASTCALL ToggleAccessoryRendering(void) {
    int on;

    if (gNet_mode != eNet_mode_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_net_games_always_accessorized));
    } else {
        on = !GetAccessoryRendering();
        SetAccessoryRendering(on);
        if (on) {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_on));
        } else {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_off));
        }
    }
}

// FUNCTION: CARMA2_HW 0x004448f0
// FUNCTION: CARMA2_SW 0x004449d0
// FUNCTION: CARMA2_CHEAT 0x00444a80
void C2_HOOK_FASTCALL UserSendMessage(void) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_details, options, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_options, enable_text_messages, 0x4);

    if (gNet_mode != eNet_mode_none && gCurrent_net_game->options.enable_text_messages) {
        gEntering_message = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00444910
// FUNCTION: CARMA2_SW 0x004449f0
// FUNCTION: CARMA2_CHEAT 0x00444aa0
void C2_HOOK_FASTCALL EnterUserMessage(void) {
    // GLOBAL: CARMA2_HW 0x00590f64
    // GLOBAL: CARMA2_SW 0x0058bf5c
    // GLOBAL: CARMA2_CHEAT 0x00597890
    static int last_key = -1;
    // GLOBAL: CARMA2_HW 0x0067c49c
    // GLOBAL: CARMA2_SW 0x00676c14
    // GLOBAL: CARMA2_CHEAT 0x00683d44
    static int about_to_die;
    // GLOBAL: CARMA2_HW 0x0067c454
    // GLOBAL: CARMA2_SW 0x00676bcc
    // GLOBAL: CARMA2_CHEAT 0x00683d04
    static tU32 next_time;
    char* the_message;
    char* p;
    int len;
    int the_key;
    int abuse_num;

    C2_HOOK_BUG_ON(REC2_ASIZE(gString) != 84);

    the_message = &gString[20];
    if (!gEntering_message || gNet_mode == eNet_mode_none || !gCurrent_net_game->options.enable_text_messages) {
        return;
    }
    the_key = PDAnyKeyDown();
    if (gEntering_message == 1) {
        if (the_key == -1) {
            gEntering_message = 2;
            PDClearKeyboardBuffer();
        } else {
            return;
        }
    }
    if (about_to_die) {
        if (the_key == -1) {
            about_to_die = 0;
            gEntering_message = 0;
        }
        return;
    }
    if (the_key == last_key) {
        if (next_time < PDGetTotalTime()) {
            next_time += 100;
        } else {
            the_key = -1000;
        }
    } else {
        last_key = the_key;
        next_time = PDGetTotalTime() + 500;
    }
    switch (the_key) {
    case -1:
    case 0:
        break;
    case 51:
    case 52:
        len = strlen(gNet_players[gThis_net_player_index].player_name);
        if (len > 18) {
            break;
        }
        p = the_message - len - 2;
        strcpy(p, gNet_players[gThis_net_player_index].player_name);
        p[len + 0] = ':';
        p[len + 1] = ' ';
        gString[REC2_ASIZE(gString) - 1] = '\0';
        NetSendHeadupToAllPlayers(p);
        the_message[0] = '\0';
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_message_sent));
        about_to_die = 1;
        break;
    case 2:
    case 3:
    case 53:
    case 63:
        about_to_die = 1;
        break;
    case 50:
    case 65:
    case 70:
        len = strlen(the_message);
        if (len > 0) {
            the_message[len - 1] = '\0';
        }
        break;
    default:
        if (gKey_mapping[71] == the_key) {
            about_to_die = 1;
            break;
        }
        if (the_key >= 75 && the_key <= 105) {
            if (the_key >= 81 && the_key <= 90) {
                if (the_key == 81) {
                    abuse_num = 9;
                } else {
                    abuse_num = the_key - 82;
                }
                if (gAbuse_text[abuse_num] != NULL) {
                    strcpy(the_message, gAbuse_text[abuse_num]);
                }
                break;
            } else {
                gEntering_message = 0;
                break;
            }
        }
        for (;;) {
            int raw_ch = PDGetKeyboardCharacter();
            int ch;

            if (raw_ch == '\0') {
                break;
            }
            ch = (tU8)PDConvertToASCIILessThan128((char)raw_ch);
            len = strlen(the_message);
            if (len < (int)REC2_ASIZE(gString) - 20 - 1) {
                the_message[len] = (char)ch;
                if ((tU8)the_message[len] < gFonts[4].offset || (tU8)the_message[len] >= gFonts[4].offset + gFonts[4].num_entries) {
                    the_message[len] = '\0';
                }
                the_message[len + 1] = '\0';
                dr_dprintf("NETMESSAGE: '%s'", the_message);
            }
        }
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00444c00
// FUNCTION: CARMA2_SW 0x00444ce0
// FUNCTION: CARMA2_CHEAT 0x00444d90
void C2_HOOK_FASTCALL DisplayUserMessage(void) {
    int len;

    if (!gEntering_message || gNet_mode == eNet_mode_none) {
        return;
    }
    len = strlen(&gString[20]);
    if (len < 63 && (PDGetTotalTime() & 0x200)) {
        gString[20 + len] = '_';
        gString[21 + len] = '\0';
    }
    DimRectangle(gBack_screen,
        15 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y - gFonts[6].height,
        85 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * gFonts[6].height,
        1);
    DRPixelmapText(gBack_screen,
        20 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y,
        &gFonts[6],
        GetMiscString(eMiscString_enter_message),
        100);
    OoerrIveGotTextInMeBoxMissus(6,
        &gString[20],
        gBack_screen,
        20 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 2 * gFonts[6].height,
        80 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * gFonts[6].height,
        0);
    gString[20 + len] = '\0';
}

// FUNCTION: CARMA2_HW 0x00444d70
// FUNCTION: CARMA2_SW 0x00444e50
// FUNCTION: CARMA2_CHEAT 0x00444f00
void C2_HOOK_FASTCALL InitAbuseomatic(void) {
    char path[256];
    char s[256];
    FILE* f;
    int i;
    int len;

    gString[20] = '\0';
    PDBuildAppPath(path);
    strcat(path, "ABUSE.TXT");
    for (i = 0; i < REC2_ASIZE(gAbuse_text); i++) {
        gAbuse_text[i] = NULL;
    }
    f = PFfopen(path, "rt");
    if (f == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (PFfgets(s, REC2_ASIZE(s) - 1, f) == NULL) {
            break;
        }
        // len = strlen(s);
        if (strlen(s) > 63) {
            s[63] = '\0';
        }
        len = strlen(s);
        while (len != 0 && s[len - 1] < ' ') {
            s[--len] = '\0';
        }
        gAbuse_text[i] = BrMemAllocate(strlen(s) + 1, kMem_abuse_text);
        strcpy(gAbuse_text[i], s);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00444ea0
// FUNCTION: CARMA2_SW 0x00444f80
// FUNCTION: CARMA2_CHEAT 0x00445030
void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (gAbuse_text[i] != NULL) {
            BrMemFree(gAbuse_text[i]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00444ed0
// FUNCTION: CARMA2_SW 0x00444fb0
// FUNCTION: CARMA2_CHEAT 0x004453c0
void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void) {

    if (!gAction_replay_mode) {
        ChangeCameraType();
        NewTextHeadupSlot2(4, 0, 1000, -4, GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode), 0);
    }
}

void C2_HOOK_FASTCALL SteelBollock(void) {

    SphericizeCar(gCar_to_view);
    NewTextHeadupSlot(4, 0, 2000, -4, "STEEL GONAD O' DEATH");
}

// FUNCTION: CARMA2_HW 0x00444f10
// FUNCTION: CARMA2_SW 0x00444ff0
// FUNCTION: CARMA2_CHEAT 0x00445430
void C2_HOOK_FASTCALL RichardsSphere(int pNum) {

    TotallyRepairACar(gCar_to_view);
    SteelBollock();
}

// FUNCTION: CARMA2_HW 0x00444f40
// FUNCTION: CARMA2_SW 0x00445020
// FUNCTION: CARMA2_CHEAT 0x00445460
void C2_HOOK_FASTCALL ToggleHeadupLevel(void) {

    if (PDKeyDown(0)) {
        gHeadup_detail_level--;
        if (gHeadup_detail_level < 0) {
            gHeadup_detail_level = 5;
        }
    } else {
        gHeadup_detail_level++;
        if (gHeadup_detail_level > 5) {
            gHeadup_detail_level = 0;
        }
    }
    NewTextHeadupSlot2(4, 0, 250, -4, GetMiscString(eMiscString_no_HUD + gHeadup_detail_level % 3), 0);
}


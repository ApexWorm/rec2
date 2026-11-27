/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "22-replay.h"

#include "04-netgame.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "23-piping.h"
#include "25-grafdata.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "40-main.h"
#include "41-utility.h"
#include "42-input.h"
#include "53-controls.h"
#include "59-camera.h"
#include "61-pedestrn.h"
#include "64-movie.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include "rec2_macros.h"

#include <math.h>
#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x006a2360
// GLOBAL: CARMA2_SW 0x0069c3b8
// GLOBAL: CARMA2_CHEAT 0x006a9ed8
tU32 gDAT_006a2360;

// GLOBAL: CARMA2_HW 0x006a2364
// GLOBAL: CARMA2_SW 0x0069c3bc
// GLOBAL: CARMA2_CHEAT 0x006a9edc
tU32 gDAT_006a2364;

// GLOBAL: CARMA2_HW 0x006a2368
// GLOBAL: CARMA2_SW 0x0069c3c0
// GLOBAL: CARMA2_CHEAT 0x006a9ee0
int gINT_006a2368;

// GLOBAL: CARMA2_HW 0x006a2374
// GLOBAL: CARMA2_SW 0x0069c3cc
// GLOBAL: CARMA2_CHEAT 0x006a9eec
int gKey_down;

// GLOBAL: CARMA2_HW 0x006a2378
// GLOBAL: CARMA2_SW 0x0069c3d0
// GLOBAL: CARMA2_CHEAT 0x006a9ef0
float gPending_replay_rate;

// GLOBAL: CARMA2_HW 0x006a237c
// GLOBAL: CARMA2_SW 0x0069c3d4
// GLOBAL: CARMA2_CHEAT 0x006a9ef4
tU32 gUINT_006a237c;

// GLOBAL: CARMA2_HW 0x006a2380
// GLOBAL: CARMA2_SW 0x0069c3d8
// GLOBAL: CARMA2_CHEAT 0x006a9ef8
tU32 gLast_replay_zappy_screen;

// GLOBAL: CARMA2_HW 0x006a2388
// GLOBAL: CARMA2_SW 0x0069c3e0
// GLOBAL: CARMA2_CHEAT 0x006a9f00
br_pixelmap* gReplay_pixies[11] = { NULL };

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x006a23b4
// GLOBAL: CARMA2_SW 0x0069c40c
int gINT_006a23b4 = 0;
#endif

// GLOBAL: CARMA2_HW 0x006a23c4
// GLOBAL: CARMA2_SW 0x0069c41c
// GLOBAL: CARMA2_CHEAT 0x006a9f3c
int gSingle_frame_mode;

// GLOBAL: CARMA2_HW 0x006a23c8
// GLOBAL: CARMA2_SW 0x0069c420
// GLOBAL: CARMA2_CHEAT 0x006a9f40
tU32 gCam_change_time;

// GLOBAL: CARMA2_HW 0x006a23d4
// GLOBAL: CARMA2_SW 0x0069c42c
// GLOBAL: CARMA2_CHEAT 0x006a9f4c
tU32 gLast_synch_time;

// GLOBAL: CARMA2_HW 0x006768fc
// GLOBAL: CARMA2_SW 0x0067106c
// GLOBAL: CARMA2_CHEAT 0x0067e1ac
int gPaused;

// GLOBAL: CARMA2_HW 0x0058f2d8
// GLOBAL: CARMA2_SW 0x0058a2d8
// GLOBAL: CARMA2_CHEAT 0x005942d8
const int gPow2_array[32] = {
    0x1, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800,
    0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000,
    0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
};

// GLOBAL: CARMA2_HW 0x0058f600
// GLOBAL: CARMA2_SW 0x0058a600
// GLOBAL: CARMA2_CHEAT 0x00594690
tU8 gCamera_type_allowed_replay[9] = {
#ifdef REC2_V101A
    1, 1, 1, 1, 1, 0, 0, 1, 1,
#else
    1, 1, 1, 1, 1, 1, 1, 1, 1,
#endif
};

// GLOBAL: CARMA2_HW 0x0058f610
// GLOBAL: CARMA2_SW 0x0058a610
// GLOBAL: CARMA2_CHEAT 0x005946a0
tU8 gCamera_type_allowed_gameplay[9] = {
#ifdef REC2_101A
    1, 0, 0, 0, 1, 1, 1, 1, 1,
#else
    1, 0, 0, 0, 1, 0, 0, 1, 1,
#endif
};

// GLOBAL: CARMA2_HW 0x0065fbe0
// GLOBAL: CARMA2_SW 0x0065a520
// GLOBAL: CARMA2_CHEAT 0x00667428
int gProgress_line_left[2] = { 240, 443 };

// GLOBAL: CARMA2_HW 0x0065fbe8
// GLOBAL: CARMA2_SW 0x0065a528
// GLOBAL: CARMA2_CHEAT 0x00667430
int gProgress_line_right[2] = { 306, 612 };

// GLOBAL: CARMA2_HW 0x0065fbf0
// GLOBAL: CARMA2_SW 0x0065a530
// GLOBAL: CARMA2_CHEAT 0x00667438
int gProgress_line_top[2] = { 184, 446 };

// GLOBAL: CARMA2_HW 0x0058f620
// GLOBAL: CARMA2_SW 0x0058a620
// GLOBAL: CARMA2_CHEAT 0x005946b0
int gAR_camera_type = 2;

// GLOBAL: CARMA2_HW 0x0058f6a4
// GLOBAL: CARMA2_SW 0x0058a6a4
int gCar_car_collisions = 1;

// GLOBAL: CARMA2_HW 0x0058f6b0
// GLOBAL: CARMA2_SW 0x0058a6b0
// GLOBAL: CARMA2_CHEAT 0x00594740
const float gCar_simplification_factor[2][5] = {
    { 50.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.5f, 0.0f }
};

// GLOBAL: CARMA2_HW 0x0058f7f0
// GLOBAL: CARMA2_SW 0x0058a7f0
// GLOBAL: CARMA2_CHEAT 0x00594880
tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12] = {
    { 0, 1 }, { 2, 3 }, { 4, 5 }, { 6, 7 },
    { 0, 2 }, { 1, 3 }, { 4, 6 }, { 5, 7 },
    { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 },
};

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e6280
// FUNCTION: CARMA2_SW 0x004e22a0
// FUNCTION: CARMA2_CHEAT 0x004ebf90
void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard) {
    tU32 the_time;
    int x;
    int i;
    int cam_text_shown;
    tU16 played_col1;
    tU16 played_col2;
    tU16 to_play_col1;
    tU16 to_play_col2;

    played_col1 = 4;
    played_col2 = 2;
    to_play_col1 = 82;
    to_play_col2 = 81;
#ifdef REC2_HW
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        played_col1 = PaletteEntry16Bit(gRender_palette, 4);
        played_col2 = PaletteEntry16Bit(gRender_palette, 2);
        to_play_col1 = PaletteEntry16Bit(gRender_palette, 82);
        to_play_col2 = PaletteEntry16Bit(gRender_palette, 81);
    }
#endif
    the_time = PDGetTotalTime();
    if (PDKeyDown(0)) {
        return;
    }
#ifdef REC2_HW
    PossibleLock(1);
#endif
    DRMaskedStamp(gCurrent_graf_data->field_0x300, gCurrent_graf_data->field_0x304, gReplay_pixies[1]);
    if (MovieRecording()) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x358,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x35c,
            gReplay_pixies[10]);
    }
    if (pSpecial_zappy_bastard < 0) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x310,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x314,
            gReplay_pixies[2]);
    } else if (pSpecial_zappy_bastard > 0) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x340,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x344,
            gReplay_pixies[8]);
    } else if (ARGetReplayRate() < -1.0f) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x318,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x31c,
            gReplay_pixies[3]);
    } else if (ARGetReplayRate() > 1.0f) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x338,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x33c,
            gReplay_pixies[7]);
    } else if (ARGetReplayRate() == 1.0f) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x330,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x334,
            gReplay_pixies[6]);
    } else if (ARGetReplayRate() == -1.0f) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x320,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x324,
            gReplay_pixies[4]);
    } else {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x328,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x32c,
            gReplay_pixies[5]);
    }
    x = (int)((float)(int)(gLast_replay_frame_time - gDAT_006a2360)
            / (float)(int)(gDAT_006a2364 - gDAT_006a2360)
            * (gProgress_line_right[gReal_graf_data_index] - gProgress_line_left[gReal_graf_data_index]))
        + gProgress_line_left[gReal_graf_data_index];
    if (x > gProgress_line_left[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen,
            gProgress_line_left[gReal_graf_data_index], gProgress_line_top[gReal_graf_data_index],
            x - 1, gProgress_line_top[gReal_graf_data_index],
            played_col2);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen,
                gProgress_line_left[gReal_graf_data_index], gProgress_line_top[gReal_graf_data_index] + i,
                x - 1, gProgress_line_top[gReal_graf_data_index] + i,
                played_col1);
        }
        BrPixelmapLine(gBack_screen,
            gProgress_line_left[gReal_graf_data_index], gProgress_line_top[gReal_graf_data_index] + 6,
            x - 1, gProgress_line_top[gReal_graf_data_index] + 6,
            played_col2);
    }
    if (x < gProgress_line_right[gReal_graf_data_index]) {
        BrPixelmapLine(gBack_screen,
            x, gProgress_line_top[gReal_graf_data_index],
            gProgress_line_right[gReal_graf_data_index] - 1, gProgress_line_top[gReal_graf_data_index],
            to_play_col2);
        for (i = 1; i < 6; i++) {
            BrPixelmapLine(gBack_screen,
                x, gProgress_line_top[gReal_graf_data_index] + i,
                gProgress_line_right[gReal_graf_data_index] - 1, gProgress_line_top[gReal_graf_data_index] + i,
                to_play_col1);
        }
        BrPixelmapLine(gBack_screen,
            x, gProgress_line_top[gReal_graf_data_index] + 6,
            gProgress_line_right[gReal_graf_data_index] - 1, gProgress_line_top[gReal_graf_data_index] + 6,
            to_play_col2);
    }
    BrPixelmapLine(gBack_screen,
        gProgress_line_left[gReal_graf_data_index] - 1, gProgress_line_top[gReal_graf_data_index],
        gProgress_line_left[gReal_graf_data_index] - 1, gProgress_line_top[gReal_graf_data_index] + 6,
        played_col2);
    BrPixelmapLine(gBack_screen,
        gProgress_line_right[gReal_graf_data_index], gProgress_line_top[gReal_graf_data_index],
        gProgress_line_right[gReal_graf_data_index], gProgress_line_top[gReal_graf_data_index] + 6,
        to_play_col2);
    if (gINT_006a2368) {
        DRMaskedStamp(gCurrent_graf_data->field_0x300 + gCurrent_graf_data->field_0x348,
            gCurrent_graf_data->field_0x304 + gCurrent_graf_data->field_0x34c,
            gReplay_pixies[9]);
    }
#ifdef REC2_HW
    PossibleUnlock(1);
#endif
    cam_text_shown = 0;
    if (the_time - gCam_change_time < 2000) {
        cam_text_shown = 1;
        DRPixelmapText(gBack_screen,
            gCurrent_graf_data->field_0x350, gCurrent_graf_data->field_0x354,
            &gFonts[1],
            GetMiscString(eMiscString_standard_cam + gAction_replay_camera_mode),
            gCurrent_graf_data->field_0x350 * 2);
    }
    if (MovieRecording()) {
        DisplayMovieStatusPanel(cam_text_shown);
    } else if ((the_time / 400) & 0x1) {
#ifdef REC2_HW
        PossibleLock(1);
#endif
        DRMaskedStamp(gCurrent_graf_data->field_0x2f8, gCurrent_graf_data->field_0x2fc, gReplay_pixies[0]);
#ifdef REC2_HW
        PossibleUnlock(1);
#endif
    }
#ifdef REC2_HW
    if (!gRecording_mouse_disabled && !MovieRecording()) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
#else
    if (!MovieRecording()) {
        DoMouseCursor();
    }
#endif
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004e6260
// FUNCTION: CARMA2_SW 0x004e2280
// FUNCTION: CARMA2_CHEAT 0x004ebf70
void C2_HOOK_FASTCALL DoActionReplayPostSwap(void) {
    RemoveTransientBitmaps(1);
}

// FUNCTION: CARMA2_HW 0x004e6900
// FUNCTION: CARMA2_SW 0x004e27f0
// FUNCTION: CARMA2_CHEAT 0x004ec5f0
void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pSpecial_zappy_bastard) {
    tU32 the_time;

    the_time = PDGetTotalTime();
    if (abs(pSpecial_zappy_bastard) > 10000 && the_time - gLast_replay_zappy_screen > 50) {
        ActualActionReplayHeadups(pSpecial_zappy_bastard);
        gLast_replay_zappy_screen = the_time;
        PDScreenBufferSwap(0);
        RemoveTransientBitmaps(1);
    }
}

// FUNCTION: CARMA2_HW 0x004e6270
// FUNCTION: CARMA2_SW 0x004e2290
// FUNCTION: CARMA2_CHEAT 0x004ebf80
void C2_HOOK_FASTCALL DoActionReplayHeadups(void) {
    ActualActionReplayHeadups(0);
}

// PreProcess

// PostProcess

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

// CheckReplayTurnOn

// InitializeActionReplayPix

// FUNCTION: CARMA2_HW 0x004e70d0
// FUNCTION: CARMA2_SW 0x004e2f10
void C2_HOOK_FASTCALL FreeActionReplayPix(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gReplay_pixies) != 11);

    for (i = 0; i < (int)REC2_ASIZE(gReplay_pixies); i++) {
        if (gReplay_pixies[i] != NULL) {
            BrMapRemove(gReplay_pixies[i]);
            BrPixelmapFree(gReplay_pixies[i]);
            gReplay_pixies[i] = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e7110
// FUNCTION: CARMA2_SW 0x004e2f50
// FUNCTION: CARMA2_CHEAT 0x004ecdb0
void C2_HOOK_FASTCALL SynchronizeActionReplay(void) {

    while (ARGetReplayRate() != 0.f && PDGetTotalTime() - gLast_synch_time < gFrame_period / fabs(ARGetReplayRate())) {
        ServiceGameInRace();
    }
    gLast_synch_time = PDGetTotalTime();
    if (gSingle_frame_mode) {
        ARSetReplayRate(0.f);
        gSingle_frame_mode = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e71c0
// FUNCTION: CARMA2_SW 0x004e3000
// FUNCTION: CARMA2_CHEAT 0x004ece60
void C2_HOOK_FASTCALL TurnOnActionReplay(void) {

    SavePedStuff();
    if (gMap_view == 2) {
        ToggleMap();
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_paused, 0, 0, 0, 0);
    }
    gUINT_006a237c = PDGetTotalTime();
    gDAT_006a2360 = ARGetStartTime();
    LoadInterfaceStuff(1);
    StartMouseCursor();
    gKey_down = 52;
    gPending_replay_rate = 0.0f;
    gCam_change_time = PDGetTotalTime();
    gDAT_006a2364 = GetTotalTime();
    if (!gRace_finished) {
        SaveCameraPosition(0);
    }
    gCamera_type = gAction_replay_camera_mode;
    SetCameraType(gAR_camera_type);
    InitReplaySplashes();
}

// FUNCTION: CARMA2_HW 0x004e7270
// FUNCTION: CARMA2_SW 0x004e30b0
// FUNCTION: CARMA2_CHEAT 0x004ecf10
void C2_HOOK_FASTCALL TurnOffActionReplay(void) {
    UnlockInterfaceStuff();
    AssertThisTimeAsCurrentTime();
    if (gRace_finished == 0) {
        RestoreCameraPosition(0);
    }
    if (gNet_mode == eNet_mode_host) {
        SendGameplayToAllPlayers(eNet_gameplay_host_unpaused, 0, 0, 0, 0);
    }
    gAR_camera_type = gAction_replay_camera_mode;
    SetCameraType(gCamera_type);
    ComparePedStuff();
    EndMouseCursor();
}

// FUNCTION: CARMA2_HW 0x004e72d0
// FUNCTION: CARMA2_SW 0x004e3110
// FUNCTION: CARMA2_CHEAT 0x004ecf70
void C2_HOOK_FASTCALL AfterActionReplay(void) {
    ForceRebuildActiveCarList();
}

// ToggleReplay

// FUNCTION: CARMA2_HW 0x004e7350
// FUNCTION: CARMA2_SW 0x004e3170
// FUNCTION: CARMA2_CHEAT 0x004ecff0
inline void C2_HOOK_FASTCALL DisplayMovieStatusPanel(int pCam_text_shown) {
    int x;
    int y;

    if (gShow_status_message) {
        if (gNext_replay_record_state == 4) {
            x = gCurrent_graf_data->field_0x350;
            y = gCurrent_graf_data->field_0x354;
            if (!pCam_text_shown) {
                DRPixelmapText(gBack_screen, x, y, &gFonts[1], GetMiscString(gStatus_message_1), gBack_screen->width);
            }
        } else {
            x = gCurrent_graf_data->width / 2;
            y = gCurrent_graf_data->height / 2;
            DRPixelmapCentredText(gBack_screen, x, y - 64, &gFonts[4], GetMiscString(gStatus_message_1));
            if (gStatus_message_2 != 0) {
                DRPixelmapCentredText(gBack_screen, x, y - 32, &gFonts[6], GetMiscString(gStatus_message_2));
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004022e0
// FUNCTION: CARMA2_SW 0x004022e0
// FUNCTION: CARMA2_CHEAT 0x004022e0
void C2_HOOK_FASTCALL ApplyTime(tU32 pTime) {
    gLast_replay_frame_time = pTime;
}

// FUNCTION: CARMA2_HW 0x004022f0
// FUNCTION: CARMA2_SW 0x004022f0
// FUNCTION: CARMA2_CHEAT 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gPaused = pPause;
}

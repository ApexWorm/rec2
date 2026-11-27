/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "22-replay.h"

#include "00-car.h"
#include "04-netgame.h"
#include "08-loading1.h"
#include "09-funks.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "21-mainloop.h"
#include "23-piping.h"
#include "25-grafdata.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "35-intrface.h"
#include "39-oil.h"
#include "40-main.h"
#include "41-utility.h"
#include "42-input.h"
#include "53-controls.h"
#include "59-camera.h"
#include "61-pedestrn-phys.h"
#include "61-pedestrn.h"
#include "63-loading3.h"
#include "64-movie.h"
#include "69-sound.h"
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

// GLOBAL: CARMA2_HW 0x0065fbf8
// GLOBAL: CARMA2_SW 0x0065a538
// GLOBAL: CARMA2_CHEAT 0x00667440
int gReplay_button_keys[9] = { 88, 85, 59, 106, 58, 87, 90, 76, 75 };

// GLOBAL: CARMA2_HW 0x0065fc20
// GLOBAL: CARMA2_SW 0x0065a560
// GLOBAL: CARMA2_CHEAT 0x00667468
tReplay_button_rect gReplay_button_rects[2][9] = {
    {
        { 268, 156, 278, 174 },
        { 279, 150, 289, 160 },
        { 279, 170, 289, 180 },
        { 281, 160, 298, 170 },
        { 290, 170, 300, 180 },
        { 290, 150, 300, 160 },
        { 301, 156, 311, 174 },
        { 239, 168, 251, 180 },
        { 254, 168, 266, 180 },
    },
    {
        { 519, 395, 543, 419 },
        { 542, 372, 566, 396 },
        { 545, 418, 569, 442 },
        { 555, 395, 579, 419 },
        { 566, 418, 590, 442 },
        { 569, 372, 593, 396 },
        { 595, 395, 619, 419 },
        { 447, 415, 471, 439 },
        { 481, 417, 505, 441 },
    },
};

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

// GLOBAL: CARMA2_HW 0x0065fbb0
// GLOBAL: CARMA2_SW 0x0065a4f0
// GLOBAL: CARMA2_CHEAT 0x006673f8
const char* gReplay_pixie_names[11] = {
    "REPLAY.PIX",
    "RBUTTONS.PIX",
    "REWSTART.PIX",
    "REW.PIX",
    "REVPLAY.PIX",
    "PAUSE.PIX",
    "PLAY.PIX",
    "FFWD.PIX",
    "FWDEND.PIX",
    "CAMERA.PIX",
    "RECORD.PIX",
};

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

// GLOBAL: CARMA2_HW 0x006a23cc
// GLOBAL: CARMA2_SW 0x0069c424
// GLOBAL: CARMA2_CHEAT 0x006a9f44
int gINT_006a23cc;

// GLOBAL: CARMA2_HW 0x006a23d0
// GLOBAL: CARMA2_SW 0x0069c428
// GLOBAL: CARMA2_CHEAT 0x006a9f48
tU32 gUINT_006a23d0;

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

// FUNCTION: CARMA2_HW 0x004e6950
// FUNCTION: CARMA2_SW 0x004e2840
// FUNCTION: CARMA2_CHEAT 0x004ec640
void C2_HOOK_FASTCALL PreProcess(int pFrame_period) {

    UpdateFramePeriod(&gCamera_period);
    if (ARGetReplayRate() < 0.0f) {
        MungePedestrians(pFrame_period);
    }
}

// FUNCTION: CARMA2_HW 0x004e6980
// FUNCTION: CARMA2_SW 0x004e2870
// FUNCTION: CARMA2_CHEAT 0x004ec670
void C2_HOOK_FASTCALL PostProcess(int pFrame_period) {

    ProcessOilSpills(pFrame_period);
    if (ARGetReplayRate() >= 0.0f) {
        MungePedestrians(pFrame_period);
    }
    MungePowerupStuff(pFrame_period);
}

// FUNCTION: CARMA2_HW 0x004e68e0
// FUNCTION: CARMA2_SW 0x004e27d0
// FUNCTION: CARMA2_CHEAT 0x004ec5d0
void C2_HOOK_FASTCALL DoActionReplay(tU32 pFrame_period) {

    gLast_replay_zappy_screen = 0;
    ARDoActionReplay(pFrame_period, DoZappyActionReplayHeadups, PreProcess, PostProcess);
}

inline void C2_HOOK_FASTCALL MoveToEndOfReplay(void) {

    gLast_replay_zappy_screen = 0;
    if (gNoTransients) {
        gRecording_mouse_disabled = 1;
    }
    RenderAFrame(1);
    RenderAFrame(1);
    ARMoveToEndOfReplay(DoZappyActionReplayHeadups, PreProcess, PostProcess);
    if (gNoTransients && !MovieRecording()) {
        gRecording_mouse_disabled = 0;
    }
}

inline void C2_HOOK_FASTCALL MoveToStartOfReplay(void) {

    gLast_replay_zappy_screen = 0;
    if (gNoTransients) {
        gRecording_mouse_disabled = 1;
    }
    RenderAFrame(1);
    RenderAFrame(1);
    ARMoveToStartOfReplay(DoZappyActionReplayHeadups, PreProcess, PostProcess);
    if (gNoTransients) {
        gRecording_mouse_disabled = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004e69b0
// FUNCTION: CARMA2_SW 0x004e28a0
// FUNCTION: CARMA2_CHEAT 0x004ec6a0
void C2_HOOK_FASTCALL PollActionReplayControls(tU32* pFrame_period, tU32* pAverage_frame_period) {
    tU32 the_time;
    float old_replay_rate;
    int old_key_down;
    int x_coord;
    int y_coord;
    int i;

    EdgeTriggerModeOff();
    the_time = PDGetTotalTime();
    old_replay_rate = ARGetReplayRate();
    old_key_down = gKey_down;
    gKey_down = PDAnyKeyDown();
    if ((old_key_down == -1 || old_key_down == 4) && KeyIsDown(58)) {
        DRS3StopAllOutletSoundsExceptCDA();
        ToggleReplay(pFrame_period, pAverage_frame_period);
        return;
    }
    if (gKey_down != -1) {
        gMouse_in_use = 0;
    } else {
        if ((old_key_down == -1 || old_key_down == 4 || old_key_down == 85 || old_key_down == 87 || old_key_down == 76) && EitherMouseButtonDown()) {
            GetMousePosition(&x_coord, &y_coord);
            for (i = 0; i < (int)REC2_ASIZE(gReplay_button_keys); i++) {
                if (x_coord >= gReplay_button_rects[gReal_graf_data_index][i].left
                    && y_coord >= gReplay_button_rects[gReal_graf_data_index][i].top
                    && x_coord < gReplay_button_rects[gReal_graf_data_index][i].right
                    && y_coord < gReplay_button_rects[gReal_graf_data_index][i].bottom) {
                    gKey_down = gReplay_button_keys[i];
                    break;
                }
            }
            if (i == (int)REC2_ASIZE(gReplay_button_keys) && !gINT_006a23cc) {
                MouseClickInActionReplay(x_coord, y_coord, 0);
            }
            gINT_006a23cc = 1;
        } else {
            gINT_006a23cc = 0;
        }
    }
    if (gKey_down == 75 && (old_key_down == -1 || old_key_down == 4)) {
        MovieRecordButtonPressed();
    } else if (gKey_down == 76) {
        gINT_006a2368 = 1;
        if (old_key_down != 76) {
            ChangeCameraType();
            gCam_change_time = PDGetTotalTime();
        }
    } else {
        gINT_006a2368 = 0;
    }
    if ((gKey_down == 86 || gKey_down == 106) && (old_key_down == -1 || old_key_down == 4)) {
        ARSetPause(!ARReplayPauseState());
    } else if ((gKey_down == 81 || gKey_down == 50) && (old_key_down == -1 || old_key_down == 4)) {
        ARSetReplayDirection((float)-ARGetReplayDirection());
        if (ARReplayPauseState()) {
            ARSetPause(0);
        }
    }
    if (gKey_down == 82 && (old_key_down == -1 || old_key_down == 4)) {
        ARSetReplayRate(-1.0f);
        ARSetReplayDirection(-1.0f);
        gSingle_frame_mode = 1;
    } else if (gKey_down == 84 && (old_key_down == -1 || old_key_down == 4)) {
        ARSetReplayRate(1.0f);
        ARSetReplayDirection(1.0f);
        gSingle_frame_mode = 1;
    } else if (gKey_down == 85 || gKey_down == 68) {
        if (ARGetReplayRate() > -1.2) {
            ARSetReplayRate(-1.2f);
        }
        if (gUINT_006a23d0 != 0) {
            ARSetReplayRate(ARGetReplayRate() - (the_time - gUINT_006a23d0) * 0.002);
        }
        if (ARGetReplayRate() < -8.0f) {
            ARSetReplayRate(-8.0f);
        }
    } else if (gKey_down == 87 || gKey_down == 69) {
        if (ARGetReplayRate() < 1.2) {
            ARSetReplayRate(1.2f);
        }
        if (gUINT_006a23d0 != 0) {
            ARSetReplayRate(ARGetReplayRate() - (the_time - gUINT_006a23d0) * -0.002);
        }
        if (ARGetReplayRate() > 8.0f) {
            ARSetReplayRate(8.0f);
        }
    } else if (gKey_down == 59) {
        ARSetReplayRate(-1.0f);
        ARSetReplayDirection(-1.0f);
        ARSetPause(0);
    } else if (gKey_down == 58) {
        ARSetReplayRate(1.0f);
        ARSetReplayDirection(1.0f);
        ARSetPause(0);
    } else if (ARReplayPauseState()) {
        ARSetReplayRate(0.0f);
    } else {
        ARSetReplayRate((float)ARGetReplayDirection());
    }
    if ((gKey_down == 88 || gKey_down == 66) && (old_key_down == -1 || old_key_down == 4)) {
        MoveToStartOfReplay();
        ARSetReplayRate(1.0f);
        MungeCarGraphics(gFrame_period);
        GrooveThoseDelics();
        ARSetReplayRate(0.0f);
        ARSetReplayDirection(1.0f);
        ARSetPause(1);
    } else if ((gKey_down == 90 || gKey_down == 67) && (old_key_down == -1 || old_key_down == 4)) {
        MoveToEndOfReplay();
        ARSetReplayRate(-1.0f);
        MungeCarGraphics(gFrame_period);
        GrooveThoseDelics();
        ARSetReplayRate(0.0f);
        ARSetReplayDirection(-1.0f);
        ARSetPause(1);
    }
    if (gPending_replay_rate != 0.0f) {
        ARSetReplayRate(gPending_replay_rate);
        gPending_replay_rate = 0.0f;
    }
    if (ARGetReplayRate() * old_replay_rate < 0.0f) {
        gPending_replay_rate = ARGetReplayRate();
        ARSetReplayRate(0.0f);
    } else {
        gPending_replay_rate = 0.0f;
    }
    if (old_replay_rate != 0.0f) {
        gFrame_period = (tU32)(ARGetReplayRate() / old_replay_rate * gFrame_period);
    }
    if (fabs(ARGetReplayRate()) < 1.2) {
        gUINT_006a23d0 = 0;
    } else {
        gUINT_006a23d0 = the_time;
    }
    if (old_replay_rate <= 0.0f && ARGetReplayRate() > 0.0f) {
        ARGoForwards();
    } else if (old_replay_rate >= 0.0f && ARGetReplayRate() < 0.0f) {
        ARGoBackwards();
    }
}

// FUNCTION: CARMA2_HW 0x004e72e0
// FUNCTION: CARMA2_CHEAT 0x004ecf80
void C2_HOOK_FASTCALL ToggleReplay(tU32* pFrame_period, tU32* pFrame_period_2) {

    if (gAction_replay_mode) {
        RenderAFrame(1);
        RenderAFrame(1);
    }
    if (!ARToggleReplay(TurnOnActionReplay, TurnOffActionReplay, AfterActionReplay, DoZappyActionReplayHeadups, PreProcess, PostProcess, pFrame_period, pFrame_period_2)) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_action_replay_unavailable));
    }
}

// FUNCTION: CARMA2_HW 0x004e6ff0
// FUNCTION: CARMA2_CHEAT 0x004ecce0
void C2_HOOK_FASTCALL CheckReplayTurnOn(void) {

    if (!gAction_replay_mode) {
        if (!KeyIsDown(58) || gEntering_message) {
            gKey_down = -1;
        } else if (gKey_down == -1) {
            ToggleReplay(NULL, NULL);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e7090
// FUNCTION: CARMA2_SW 0x004e2ed0
// FUNCTION: CARMA2_CHEAT 0x004ecd80
void C2_HOOK_FASTCALL InitializeActionReplayPix(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gReplay_pixies); i++) {
        gReplay_pixies[i] = LoadPixelmap(gReplay_pixie_names[i]);
        BRPM_convert(gReplay_pixies[i], gBack_screen->type);
#ifndef REC2_CHEAT
        BrMapAdd(gReplay_pixies[i]);
#endif
    }
}

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004022f0
// FUNCTION: CARMA2_SW 0x004022f0
// FUNCTION: CARMA2_CHEAT 0x004022f0
void C2_HOOK_FASTCALL ARSetPause(int pPause) {
    gPaused = pPause;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

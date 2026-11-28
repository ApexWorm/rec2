/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "12-menuoptions.h"

#include "01-network.h"
#include "05-drmem.h"
#include "08-loading1.h"
#include "10-loading2.h"

#include "16-graphics1.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "19-font.h"
#include "25-grafdata.h"
#include "32-spark.h"
#include "33-depth.h"
#include "35-intrface.h"
#include "37-brucetrk.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "57-drone.h"
#include "61-pedestrn-phys.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "72-interface.h"
#include "globvars.h"
#include "platform.h"

#include "c2_string.h"

#include "rec2_macros.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x0065bd30
// GLOBAL: CARMA2_SW 0x00656648
// GLOBAL: CARMA2_CHEAT 0x00663328
int gKey_defns[18] = { 48, 49, 46, 47, 53, 44, 59, 57, 55, 45, 50, 51, 52, 56, 62, 63, 64, 66 };

// GLOBAL: CARMA2_HW 0x0069178c
// GLOBAL: CARMA2_SW 0x0068b7e4
// GLOBAL: CARMA2_CHEAT 0x00699174
int gKey_count;

// GLOBAL: CARMA2_HW 0x00691798
// GLOBAL: CARMA2_SW 0x0068b7f0
// GLOBAL: CARMA2_CHEAT 0x00699180
int gOrig_key_mapping[77];

// GLOBAL: CARMA2_HW 0x006918d0
// GLOBAL: CARMA2_SW 0x0068b928
// GLOBAL: CARMA2_CHEAT 0x006992b8
char* gKey_names__options[153];

// GLOBAL: CARMA2_HW 0x00691b34
// GLOBAL: CARMA2_SW 0x0068bb8c
// GLOBAL: CARMA2_CHEAT 0x0069951c
int gPending_entry;

// GLOBAL: CARMA2_HW 0x00691b38
// GLOBAL: CARMA2_SW 0x0068bb90
// GLOBAL: CARMA2_CHEAT 0x00699520
int gCurrent_key;

// GLOBAL: CARMA2_HW 0x0065bd78
// GLOBAL: CARMA2_SW 0x00656690
// GLOBAL: CARMA2_CHEAT 0x00663370
tRadio_bastards gRadio_bastards__options[13] = {
    { 4, 36, 0, { 132, 175, 221, 253, 0 } },
    { 3, 45, 0, { 132, 157, 217, 0, 0 } },
    { 3, 54, 0, { 132, 183, 236, 0, 0 } },
    { 3, 67, 0, { 132, 194, 234, 0, 0 } },
    { 3, 76, 0, { 132, 194, 234, 0, 0 } },
    { 4, 89, 0, { 132, 176, 204, 246, 0 } },
    { 4, 98, 0, { 132, 176, 204, 246, 0 } },
    { 2, 111, 0, { 132, 158, 0, 0, 0 } },
    { 2, 120, 0, { 132, 158, 0, 0, 0 } },
    { 2, 129, 0, { 132, 158, 0, 0, 0 } },
    { 2, 138, 0, { 132, 158, 0, 0, 0 } },
    { 3, 150, 0, { 132, 164, 207, 0, 0 } },
    { 4, 153, 0, { 177, 199, 220, 242, 0 } },
};

// GLOBAL: CARMA2_HW 0x00691788
// GLOBAL: CARMA2_SW 0x0068b7e0
// GLOBAL: CARMA2_CHEAT 0x00699170
int gLast_graph_sel__options;

// GLOBAL: CARMA2_HW 0x006918cc
// GLOBAL: CARMA2_SW 0x0068b924
// GLOBAL: CARMA2_CHEAT 0x006992b4
tInterface_spec* gThe_interface_spec__options;

// GLOBAL: CARMA2_HW 0x00691790
// GLOBAL: CARMA2_SW 0x0068b7e8
// GLOBAL: CARMA2_CHEAT 0x00699178
br_pixelmap* gDials_pix;

inline void C2_HOOK_FASTCALL DrawDial(int pWhich_one, int pWhich_stage) {

    RemoveTransientBitmaps(1);
    DRPixelmapRectangleMaskedCopy(gBack_screen,
        gCurrent_graf_data->dial__x[pWhich_one],
        gCurrent_graf_data->dial__y[pWhich_one],
        gDials_pix,
        0,
        pWhich_stage * 64,
        gDials_pix->width,
        64);
    ProcessFlicQueue(gFrame_period);
    DoMouseCursor();
    PDScreenBufferSwap(0);
}

// FUNCTION: CARMA2_HW 0x004b1ca0
// FUNCTION: CARMA2_SW 0x004ade10
// FUNCTION: CARMA2_CHEAT 0x004b7570
void C2_HOOK_FASTCALL SoundOptionsStart(void) {

    DrawDial(0, 0);
    DrawDial(1, 0);
    MoveDialFromTo(0, 0, 4 * gProgram_state.music_volume);
    MoveDialFromTo(1, 0, 4 * gProgram_state.effects_volume);
}

// FUNCTION: CARMA2_HW 0x004b1d80
// FUNCTION: CARMA2_SW 0x004adef0
// FUNCTION: CARMA2_CHEAT 0x004b7650
void C2_HOOK_FASTCALL MoveDialFromTo(int pWhich_one, int pOld_stage, int pNew_stage) {
    tS32 time_diff;
    tU32 start_time;

    DrawDial(pWhich_one, pOld_stage);
    start_time = PDGetTotalTime();
    while ((time_diff = PDGetTotalTime() - start_time) < 100) {
        DrawDial(pWhich_one, pOld_stage + (pNew_stage - pOld_stage) * time_diff / 100);
    }
    DrawDial(pWhich_one, pNew_stage);

    start_time = PDGetTotalTime();
    DrawDial(pWhich_one, pNew_stage < 24 ? pNew_stage + 1 : 22);
    while (PDGetTotalTime() - start_time < 20) {
    }

    start_time = PDGetTotalTime();
    DrawDial(pWhich_one, pNew_stage != 0 ? pNew_stage - 1 : pNew_stage + 2);
    while (PDGetTotalTime() - start_time < 20) {
    }

    start_time = PDGetTotalTime();
    DrawDial(pWhich_one, pNew_stage < 24 ? pNew_stage + 1 : 22);
    while (PDGetTotalTime() - start_time < 20) {
    }

    start_time = PDGetTotalTime();
    DrawDial(pWhich_one, pNew_stage);
    while (PDGetTotalTime() - start_time < 20) {
    }

    start_time = PDGetTotalTime();
    DrawDial(pWhich_one, pNew_stage != 0 ? pNew_stage - 1 : pNew_stage + 2);
    while (PDGetTotalTime() - start_time < 20) {
    }
    DrawDial(pWhich_one, pNew_stage);
}

// FUNCTION: CARMA2_HW 0x004b21a0
// FUNCTION: CARMA2_SW 0x004ae310
// FUNCTION: CARMA2_CHEAT 0x004b7a70
int C2_HOOK_FASTCALL SoundOptionsDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out) {

    MoveDialFromTo(0, 4 * gProgram_state.music_volume, 0);
    MoveDialFromTo(1, 4 * gProgram_state.effects_volume, 0);
    return pCurrent_choice;
}

// FUNCTION: CARMA2_HW 0x004b21e0
// FUNCTION: CARMA2_SW 0x004ae350
// FUNCTION: CARMA2_CHEAT 0x004b7ab0
int C2_HOOK_FASTCALL SoundOptionsLeft(int* pCurrent_choice, int* pCurrent_mode) {
    int old_value;
    int* the_value;

    if (*pCurrent_choice != 2) {
        if (*pCurrent_choice == 0) {
            the_value = &gProgram_state.music_volume;
        } else {
            the_value = &gProgram_state.effects_volume;
        }
        old_value = *the_value;
        *the_value -= 1;
        if (*the_value < 0) {
            *the_value = 0;
        }
        SetSoundVolumes(0);
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        MoveDialFromTo(*pCurrent_choice, 4 * old_value, 4 * *the_value);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b2240
// FUNCTION: CARMA2_SW 0x004ae3b0
// FUNCTION: CARMA2_CHEAT 0x004b7b10
int C2_HOOK_FASTCALL SoundOptionsRight(int* pCurrent_choice, int* pCurrent_mode) {
    int old_value;
    int* the_value;

    if (*pCurrent_choice != 2) {
        if (*pCurrent_choice == 0) {
            the_value = &gProgram_state.music_volume;
        } else {
            the_value = &gProgram_state.effects_volume;
        }
        old_value = *the_value;
        *the_value += 1;
        if (*the_value > 6) {
            *the_value = 6;
        }
        SetSoundVolumes(0);
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        MoveDialFromTo(*pCurrent_choice, 4 * old_value, 4 * *the_value);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b22a0
// FUNCTION: CARMA2_SW 0x004ae410
// FUNCTION: CARMA2_CHEAT 0x004b7b70
int C2_HOOK_FASTCALL SoundClick(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {
    float x_delta;
    float y_delta;
    float angle;
    int old_value;
    int* the_value;

#define ANGLE_RANGE_START (20 * REC2_PI / 360)
#define ANGLE_RANGE_END (340 * REC2_PI / 360)

    x_delta = (float)(pX_offset - gCurrent_graf_data->dial__x_centre);
    y_delta = (float)(gCurrent_graf_data->dial__y_centre - pY_offset);
    if (y_delta > 0.0f) {
        if (x_delta != 0.0f) {
            angle = (float)atan(y_delta / x_delta);
        } else {
            angle = (float)(REC2_PI / 2);
        }
        if (angle < 0.0f) {
            angle = (float)(angle + REC2_PI);
        }
        if (angle > ANGLE_RANGE_START && angle < ANGLE_RANGE_END) {
            if (*pCurrent_choice == 0) {
                the_value = &gProgram_state.music_volume;
            } else {
                the_value = &gProgram_state.effects_volume;
            }
            old_value = *the_value;
            *the_value = (int)((ANGLE_RANGE_END - angle + 0.23300145514124296) / 0.4660029102824859);
            if (*the_value > 6) {
                *the_value = 6;
            } else if (*the_value < 0) {
                *the_value = 0;
            }
            if (*the_value != old_value) {
                SetSoundVolumes(0);
                if (*the_value > old_value) {
                    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
                } else {
                    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
                }
                MoveDialFromTo(*pCurrent_choice, 4 * old_value, 4 * *the_value);
            }
        }
    }
    return 0;

#undef ANGLE_RANGE_START
#undef ANGLE_RANGE_END
}

inline void C2_HOOK_FASTCALL PlayRadioOn2(int pIndex, int pValue) {

    RunFlicAt(288,
        gRadio_bastards__options[pIndex].left[pValue],
        gRadio_bastards__options[pIndex].top);
}

inline void C2_HOOK_FASTCALL PlayRadioOff2(int pIndex, int pValue) {

    RunFlicAt(287,
        gRadio_bastards__options[pIndex].left[pValue],
        gRadio_bastards__options[pIndex].top);
}

inline void C2_HOOK_FASTCALL PlayRadioOn__options(int pIndex, int pValue) {

    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    PlayRadioOn2(pIndex, pValue);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
}

inline void C2_HOOK_FASTCALL PlayRadioOff__options(int pIndex, int pValue) {

    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    PlayRadioOff2(pIndex, pValue);
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
}

// FUNCTION: CARMA2_HW 0x004b23d0
// FUNCTION: CARMA2_SW 0x004ae540
// FUNCTION: CARMA2_CHEAT 0x004b7ca0
void C2_HOOK_FASTCALL DrawInitialRadios(void) {
    int i;

    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    for (i = 0; i < (int)REC2_ASIZE(gRadio_bastards__options) - 1; i++) {
        PlayRadioOn2(i, gRadio_bastards__options[i].current_value);
    }
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
}

inline void C2_HOOK_FASTCALL RadioChanged(int pIndex, int pNew_value) {

    PlayRadioOff__options(pIndex, gRadio_bastards__options[pIndex].current_value);
    PlayRadioOn__options(pIndex, pNew_value);
    gRadio_bastards__options[pIndex].current_value = pNew_value;
}

// FUNCTION: CARMA2_HW 0x004b2430
// FUNCTION: CARMA2_SW 0x004ae5a0
// FUNCTION: CARMA2_CHEAT 0x004b7d00
int C2_HOOK_FASTCALL GraphOptLeft(int* pCurrent_choice, int* pCurrent_mode) {
    int new_value;

    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    new_value = gRadio_bastards__options[*pCurrent_choice - 2].current_value - 1;
    if (new_value < 0) {
        new_value = gRadio_bastards__options[*pCurrent_choice - 2].count - 1;
    }
    RadioChanged(*pCurrent_choice - 2, new_value);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b24f0
// FUNCTION: CARMA2_SW 0x004ae660
// FUNCTION: CARMA2_CHEAT 0x004b7dc0
int C2_HOOK_FASTCALL GraphOptRight(int* pCurrent_choice, int* pCurrent_mode) {
    int new_value;

    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    new_value = gRadio_bastards__options[*pCurrent_choice - 2].current_value + 1;
    if (new_value == gRadio_bastards__options[*pCurrent_choice - 2].count) {
        new_value = 0;
    }
    RadioChanged(*pCurrent_choice - 2, new_value);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b25c0
// FUNCTION: CARMA2_SW 0x004ae730
// FUNCTION: CARMA2_CHEAT 0x004b7e90
int C2_HOOK_FASTCALL GraphOptUp(int* pCurrent_choice, int* pCurrent_mode) {

    if (*pCurrent_mode == 0) {
        *pCurrent_mode = 1;
        *pCurrent_choice = 13;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        return 1;
    } else if (*pCurrent_choice == 1) {
        *pCurrent_mode = 0;
        *pCurrent_choice = 0;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004b2600
// FUNCTION: CARMA2_SW 0x004ae770
// FUNCTION: CARMA2_CHEAT 0x004b7ed0
int C2_HOOK_FASTCALL GraphOptDown(int* pCurrent_choice, int* pCurrent_mode) {

    if (*pCurrent_mode == 0) {
        *pCurrent_mode = 1;
        *pCurrent_choice = 2;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        return 1;
    } else if (*pCurrent_choice == 14) {
        *pCurrent_mode = 0;
        *pCurrent_choice = 0;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004b2640
// FUNCTION: CARMA2_SW 0x004ae7b0
// FUNCTION: CARMA2_CHEAT 0x004b7f10
int C2_HOOK_FASTCALL RadioClick(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {
    int i;

    for (i = gRadio_bastards__options[*pCurrent_choice - 2].count - 1; i >= 0; i--) {
        if (gThe_interface_spec__options->mouse_areas[2].left[gReal_graf_data_index] + pX_offset + 3 >= gRadio_bastards__options[*pCurrent_choice - 2].left[i]) {
            DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
            RadioChanged(*pCurrent_choice - 2, i);
            return 0;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b2740
// FUNCTION: CARMA2_SW 0x004ae8b0
// FUNCTION: CARMA2_CHEAT 0x004b8010
int C2_HOOK_FASTCALL GraphOptGoAhead(int* pCurrent_choice, int* pCurrent_mode) {

    GraphOptRight(pCurrent_choice, pCurrent_mode);
    return 0;
}

inline void C2_HOOK_FASTCALL PlotAGraphBox__options(int pIndex, int pColour_value) {

    if (pIndex < 0) {
        return;
    }
    DrawRRectangle(gBack_screen,
        gThe_interface_spec__options->mouse_areas[2].left[gReal_graf_data_index] - 6,
        gRadio_bastards__options[pIndex].top - 3,
        gThe_interface_spec__options->mouse_areas[2].right[gReal_graf_data_index] + 3,
        gRadio_bastards__options[pIndex].top + gFonts[12].height + 2,
        pColour_value);
}

inline void C2_HOOK_FASTCALL DrawAGraphBox__options(int pIndex) {

    PlotAGraphBox__options(pIndex, 45);
}

inline void C2_HOOK_FASTCALL EraseAGraphBox__options(int pIndex) {

    PlotAGraphBox__options(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x004b2800
// FUNCTION: CARMA2_SW 0x004ae970
// FUNCTION: CARMA2_CHEAT 0x004b80d0
void C2_HOOK_FASTCALL DrawGraphBox(int pCurrent_choice, int pCurrent_mode) {

    if (gLast_graph_sel__options != pCurrent_choice) {
        EraseAGraphBox__options(gLast_graph_sel__options - 2);
        DrawAGraphBox__options(pCurrent_choice - 2);
        gLast_graph_sel__options = pCurrent_choice;
    }
}

inline void C2_HOOK_FASTCALL SaveOrigKeyMapping(void) {

    memcpy(gOrig_key_mapping, gKey_mapping, sizeof(gKey_mapping));
}

inline void C2_HOOK_FASTCALL SaveKeyMapping__options(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "KEYMAP_X.TXT");
    the_path[strlen(the_path) - 5] = '0' + gKey_map_index;
    PDFileUnlock(the_path);
    f = DRfopen(the_path, "wb");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }
    for (i = 0; i < (int)REC2_ASIZE(gKey_mapping); i++) {
        fprintf(f, "%d", gKey_mapping[i]);
        fputc('\r', f);
        fputc('\n', f);
    }
    PFfclose(f);
}

inline void C2_HOOK_FASTCALL ChangeKeyMapIndex(int pNew_one) {

    SaveKeyMapping__options();
    gKey_map_index = pNew_one;
    LoadKeyMapping();
    SaveOrigKeyMapping();
}

inline void C2_HOOK_FASTCALL SetKeysToDefault(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "DKEYMAPX.TXT");
    the_path[strlen(the_path) - 5] = '0' + gKey_map_index;
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }
    for (i = 0; i < (int)REC2_ASIZE(gKey_mapping); i++) {
        fscanf(f, "%d", &gKey_mapping[i]);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x004b28c0
// FUNCTION: CARMA2_SW 0x004aea30
// FUNCTION: CARMA2_CHEAT 0x004b8190
void C2_HOOK_FASTCALL DrawKeyAssignments(int pCurrent_choice, int pCurrent_mode) {
    int i;
    int y;
    int x_coord;
    int y_coord;
    int name_x;
    int key_x;
    int new_key;
    int end_box;
    tDR_font* font_n;
    tDR_font* font_k;
    // GLOBAL: CARMA2_HW 0x00691b3c
    // GLOBAL: CARMA2_SW 0x0068bb94
    // GLOBAL: CARMA2_CHEAT 0x00699524
    static int on_radios_last_time;

    if (gMouse_in_use && pCurrent_choice == 4) {
        GetMousePosition(&x_coord, &y_coord);
        if (y_coord >= gCurrent_graf_data->key_assign_key_map_y
            && y_coord <= gCurrent_graf_data->key_assign_key_map_y + gFonts[12].height + 5
            && x_coord > gCurrent_graf_data->key_assign_col_1
            && x_coord < gCurrent_graf_data->key_assign_col_2 + gCurrent_graf_data->key_assign_col_2 - gCurrent_graf_data->key_assign_col_1 - 7) {
            gCurrent_key = -1;
        } else {
            if (x_coord > gCurrent_graf_data->key_assign_col_2) {
                new_key = (gKey_count + 1) / 2;
            } else {
                new_key = 0;
            }
            if (y_coord >= gCurrent_graf_data->key_assign_y - 2
                && y_coord < gCurrent_graf_data->key_assign_y + gCurrent_graf_data->key_assign_y_pitch * (gKey_count + 1) / 2) {
                new_key += (y_coord - gCurrent_graf_data->key_assign_y - 2) / gCurrent_graf_data->key_assign_y_pitch;
                if (new_key >= 0 && new_key < gKey_count) {
                    gCurrent_key = new_key;
                }
            }
        }
    }
    BrPixelmapRectangleFill(gBack_screen,
        gCurrent_graf_data->key_assign_col_1 - 3,
        gCurrent_graf_data->key_assign_y - 3,
        2 * (gCurrent_graf_data->key_assign_col_2 - gCurrent_graf_data->key_assign_col_1) - 3,
        gCurrent_graf_data->key_assign_y_pitch * gKey_count / 2 + 4,
        0);
    GetKeyCoords(-1, &y, &name_x, &key_x, &end_box);
    DrawRRectangle(gBack_screen, name_x - 3, y - 3, end_box, gFonts[12].height + y + 2, 0);
    for (i = 0; i < gKey_count; i++) {
        GetKeyCoords(i, &y, &name_x, &key_x, &end_box);
        if (gCurrent_key == i && pCurrent_mode != 0) {
            font_n = &gFonts[10];
            font_k = &gFonts[12];
        } else {
            font_n = &gFonts[9];
            font_k = &gFonts[11];
        }
        DRPixelmapText(gBack_screen, name_x, y, font_n, GetMiscString(eMiscString_key_assignment_names + i), 640);
        DRPixelmapText(gBack_screen, key_x, y, font_k, gKey_names__options[gKey_mapping[gKey_defns[i]] + 2], 640);
        if (gCurrent_key == i && pCurrent_mode != 0) {
            DrawRRectangle(gBack_screen, name_x - 3, y - 3, end_box, font_k->height + y + 2, 45);
        }
    }
    if (gCurrent_key < 0 && pCurrent_mode != 0) {
        GetKeyCoords(-1, &y, &name_x, &key_x, &end_box);
        DrawRRectangle(gBack_screen, name_x - 3, y - 3, end_box, font_k->height + y + 2, 45);
    }
    if (on_radios_last_time && (pCurrent_mode == 0 || gCurrent_key >= 0)) {
        DontLetFlicFuckWithPalettes();
        TurnFlicTransparencyOn();
        RunFlicAt(193, gCurrent_graf_data->key_assign_flic_x, gCurrent_graf_data->key_assign_flic_y);
        TurnFlicTransparencyOff();
        LetFlicFuckWithPalettes();
    } else if (!on_radios_last_time && pCurrent_mode != 0 && gCurrent_key < 0) {
        DontLetFlicFuckWithPalettes();
        TurnFlicTransparencyOn();
        RunFlicAt(194, gCurrent_graf_data->key_assign_flic_x, gCurrent_graf_data->key_assign_flic_y);
        TurnFlicTransparencyOff();
        LetFlicFuckWithPalettes();
    }
    if (pCurrent_mode != 0 && gCurrent_key < 0) {
        on_radios_last_time = 1;
    } else {
        on_radios_last_time = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004b2c30
// FUNCTION: CARMA2_SW 0x004aeda0
// FUNCTION: CARMA2_CHEAT 0x004b8500
void C2_HOOK_FASTCALL GetKeyCoords(int pIndex, int* pY, int* pName_x, int* pKey_x, int* pEnd_box) {
    int col;

    if (pIndex < 0) {
        *pName_x = gCurrent_graf_data->key_assign_col_1;
        *pKey_x = 0;
        *pEnd_box = gCurrent_graf_data->key_assign_col_2 + gCurrent_graf_data->key_assign_col_2 - gCurrent_graf_data->key_assign_col_1 - 7;
        *pY = gCurrent_graf_data->key_assign_key_map_y;
    } else {
        if (pIndex >= (gKey_count + 1) / 2) {
            col = 1;
        } else {
            col = 0;
        }
        *pY = (pIndex % ((gKey_count + 1) / 2)) * gCurrent_graf_data->key_assign_y_pitch + gCurrent_graf_data->key_assign_y;
        if (col != 0) {
            *pName_x = gCurrent_graf_data->key_assign_col_2;
            *pKey_x = gCurrent_graf_data->key_assign_col_2_a;
            *pEnd_box = gCurrent_graf_data->key_assign_col_2 + gCurrent_graf_data->key_assign_col_2 - gCurrent_graf_data->key_assign_col_1 - 7;
        } else {
            *pName_x = gCurrent_graf_data->key_assign_col_1;
            *pKey_x = gCurrent_graf_data->key_assign_col_1_a;
            *pEnd_box = gCurrent_graf_data->key_assign_col_2 - 7;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004b2d50
// FUNCTION: CARMA2_SW 0x004aeec0
// FUNCTION: CARMA2_CHEAT 0x004b8620
int C2_HOOK_FASTCALL KeyAssignLeft(int* pCurrent_choice, int* pCurrent_mode) {
    int new_index;

    if (gCurrent_key < 0) {
        if (gKey_map_index == 0) {
            new_index = 3;
        } else {
            new_index = gKey_map_index - 1;
        }
        ChangeKeyMapIndex(new_index);
        RadioChanged(12, new_index);
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        return 1;
    } else {
        if (gCurrent_key >= (gKey_count + 1) / 2) {
            gCurrent_key -= (gKey_count + 1) / 2;
        } else {
            gCurrent_key += (gKey_count + 1) / 2;
            if (gCurrent_key >= gKey_count) {
                gCurrent_key -= (gKey_count + 1) / 2;
            }
        }
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x004b2f10
// FUNCTION: CARMA2_SW 0x004af080
// FUNCTION: CARMA2_CHEAT 0x004b87e0
int C2_HOOK_FASTCALL KeyAssignRight(int* pCurrent_choice, int* pCurrent_mode) {
    int new_index;

    if (gCurrent_key < 0) {
        if (gKey_map_index >= 3) {
            new_index = 0;
        } else {
            new_index = gKey_map_index + 1;
        }
        ChangeKeyMapIndex(new_index);
        RadioChanged(12, new_index);
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        return 1;
    }
    if (gCurrent_key >= (gKey_count + 1) / 2) {
        gCurrent_key -= (gKey_count + 1) / 2;
    } else {
        gCurrent_key += (gKey_count + 1) / 2;
        if (gCurrent_key >= gKey_count) {
            gCurrent_key -= (gKey_count + 1) / 2;
        }
    }
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b30d0
// FUNCTION: CARMA2_SW 0x004af240
// FUNCTION: CARMA2_CHEAT 0x004b89a0
int C2_HOOK_FASTCALL KeyAssignUp(int* pCurrent_choice, int* pCurrent_mode) {

    if (*pCurrent_mode == 0) {
        gCurrent_key = -1;
        *pCurrent_choice = 4;
        *pCurrent_mode = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    } else if (gCurrent_key < 0) {
        gCurrent_key = (gKey_count + 1) / 2 - 1;
        *pCurrent_choice = 4;
        *pCurrent_mode = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    } else if (gCurrent_key > (gKey_count + 1) / 2) {
        gCurrent_key -= 1;
    } else if (gCurrent_key == (gKey_count + 1) / 2) {
        *pCurrent_choice = 0;
        *pCurrent_mode = 0;
    } else if (gCurrent_key != 0) {
        gCurrent_key -= 1;
    } else {
        *pCurrent_choice = 0;
        *pCurrent_mode = 0;
    }
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b3170
// FUNCTION: CARMA2_SW 0x004af2e0
// FUNCTION: CARMA2_CHEAT 0x004b8a40
int C2_HOOK_FASTCALL KeyAssignDown(int* pCurrent_choice, int* pCurrent_mode) {

    if (*pCurrent_mode == 0) {
        if (*pCurrent_choice < 2) {
            gCurrent_key = 0;
        } else {
            gCurrent_key = (gKey_count + 1) / 2;
        }
        *pCurrent_choice = 4;
        *pCurrent_mode = 1;
    } else if (gCurrent_key < 0) {
        *pCurrent_choice = 0;
        *pCurrent_mode = 0;
    } else {
        if (gCurrent_key >= (gKey_count + 1) / 2) {
            if (gCurrent_key < gKey_count - 1) {
                gCurrent_key += 1;
            } else {
                gCurrent_key = -1;
            }
        } else if (gCurrent_key < ((gKey_count + 1) / 2 - 1)) {
            gCurrent_key += 1;
        } else {
            gCurrent_key = -1;
        }
    }
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
    return 1;
}

// FUNCTION: CARMA2_HW 0x004b3220
// FUNCTION: CARMA2_SW 0x004af390
// FUNCTION: CARMA2_CHEAT 0x004b8af0
int C2_HOOK_FASTCALL KeyAssignGoAhead(int* pCurrent_choice, int* pCurrent_mode) {
    int key;
    int i;
    int j;
    int y;
    int new_index;
    int disallowed;
    int name_x;
    int key_x;
    int end_box;
    tDR_font* font;

    RemoveTransientBitmaps(1);
    if (*pCurrent_mode != 0) {
        if (gCurrent_key < 0) {
            if (gKey_map_index >= 3) {
                new_index = 0;
            } else {
                new_index = gKey_map_index + 1;
            }
            ChangeKeyMapIndex(new_index);
            RadioChanged(12, new_index);
        } else {
            PDScreenBufferSwap(0);
            CyclePollKeys();
            PollKeys();
            WaitForNoKeys();
            GetKeyCoords(gCurrent_key, &y, &name_x, &key_x, &end_box);
            font = &gFonts[12];
            gPending_entry = -1;
            while (1) {
                key = PDAnyKeyDown();
                if (key != -1 && key != 63) {
                    for (i = 28; i <= 76; i++) {
                        if (gKey_mapping[i] == key && gKey_defns[gCurrent_key] != i) {
                            for (j = 0; j < REC2_ASIZE(gKey_defns); j++) {
                                if (gKey_defns[j] == i) {
                                    gKey_mapping[i] = -2;
                                    gPending_entry = j;
                                    break;
                                }
                            }
                            if (gPending_entry < 0) {
                                DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
                                key = -1;
                                break;
                            }
                        }
                    }
                }
                CyclePollKeys();
                PollKeys();
                if (((int)PDGetTotalTime() / 100) & 1) {
                    DRPixelmapText(gBack_screen, key_x, y, font, gKey_names__options[gKey_mapping[gKey_defns[gCurrent_key]] + 2], 640);
                } else {
                    BrPixelmapRectangleFill(gBack_screen, key_x, y, end_box - key_x, font->height, 0);
                }
                PDScreenBufferSwap(0);
                if (key != -1 || EitherMouseButtonDown()) {
                    break;
                }
            }
            DRS3StartSound(gEffects_outlet, eSoundId_Done);
            WaitForNoKeys();
            if (key != 63 && key != -1) {
                gKey_mapping[gKey_defns[gCurrent_key]] = key;
            }
            if (gPending_entry >= 0) {
                *pCurrent_choice = gPending_entry + 4;
                gCurrent_key = gPending_entry;
            }
        }
        return 0;
    } else {
        key = -1;
        if (*pCurrent_choice == 0) {
            for (i = 0; i < REC2_ASIZE(gKey_defns); i++) {
                if (gKey_mapping[gKey_defns[i]] == -2) {
                    key = i;
                    break;
                }
            }
        }
        if (key < 0) {
            DontLetFlicFuckWithPalettes();
            TurnFlicTransparencyOn();
            RunFlicAt(
                gThe_interface_spec__options->pushed_flics[*pCurrent_choice].flic_index,
                gThe_interface_spec__options->pushed_flics[*pCurrent_choice].x[gReal_graf_data_index],
                gThe_interface_spec__options->pushed_flics[*pCurrent_choice].y[gReal_graf_data_index]);
            TurnFlicTransparencyOff();
            LetFlicFuckWithPalettes();
        }
        switch (*pCurrent_choice) {
        case 0:
            if (key < 0) {
                disallowed = 1;
            } else {
                *pCurrent_choice = key + 4;
                *pCurrent_mode = 1;
                gCurrent_key = key;
                gMouse_in_use = 0;
                DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
                disallowed = 0;
            }
            break;
        case 1:
            *pCurrent_choice = -1;
            disallowed = 1;
            break;
        case 2:
            SetKeysToDefault();
            disallowed = 0;
            break;
        case 3:
            disallowed = 1;
            break;
        default:
            disallowed = 0;
            break;
        }
        return disallowed;
    }
}

// FUNCTION: CARMA2_HW 0x004b36f0
// FUNCTION: CARMA2_SW 0x004af860
// FUNCTION: CARMA2_CHEAT 0x004b8fc0
int C2_HOOK_FASTCALL MouseyClickBastard(int* pCurrent_choice, int* pCurrent_mode, int pX_offset, int pY_offset) {
    int i;
    int x_coord;
    int y_coord;

    if (gCurrent_key < 0) {
        GetMousePosition(&x_coord, &y_coord);
        for (i = gRadio_bastards__options[12].count - 1; i >= 0; i--) {
            if (x_coord + 3 >= gRadio_bastards__options[12].left[i]) {
                DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
                ChangeKeyMapIndex(i);
                RadioChanged(12, i);
                break;
            }
        }
    } else {
        KeyAssignGoAhead(pCurrent_choice, pCurrent_mode);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b38b0
// FUNCTION: CARMA2_SW 0x004afa20
// FUNCTION: CARMA2_CHEAT 0x004b9180
void C2_HOOK_FASTCALL DrawInitialKMRadios(void) {

    RemoveTransientBitmaps(1);
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    PlayRadioOn2(12, gKey_map_index);
    gRadio_bastards__options[12].current_value = gKey_map_index;
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
}

// FUNCTION: CARMA2_HW 0x004b3900
// FUNCTION: CARMA2_SW 0x004afa70
// FUNCTION: CARMA2_CHEAT 0x004b91d0
void C2_HOOK_FASTCALL LoadSoundOptionsData(void) {

    gDials_pix = LoadPixelmap("DIALSTCK.PIX");
    if (gDials_pix == NULL) {
        FatalError(kFatalError_CannotLoadDialsImage);
    }
}

// FUNCTION: CARMA2_HW 0x004b3920
// FUNCTION: CARMA2_SW 0x004afa90
// FUNCTION: CARMA2_CHEAT 0x004b91f0
void C2_HOOK_FASTCALL FreeSoundOptionsData(void) {

    BrPixelmapFree(gDials_pix);
}

// FUNCTION: CARMA2_HW 0x004b3930
// FUNCTION: CARMA2_SW 0x004afaa0
// FUNCTION: CARMA2_CHEAT 0x004b9200
void C2_HOOK_FASTCALL DrawDisabledOptions(void) {
    br_pixelmap* image;

    PrintMemoryDump(0, "INSIDE OPTIONS");

    image = LoadPixelmap("NOSNDOPT.PIX");
    DisableChoice(0);
    if (image != NULL) {
        DRPixelmapRectangleMaskedCopy(gBack_screen,
            gCurrent_graf_data->sound_opt_disable_x,
            gCurrent_graf_data->sound_opt_disable_y,
            image, 0, 0, image->width, image->height);
        BrPixelmapFree(image);
    }

    if (gProgram_state.track_spec.the_actor != NULL) {
        image = LoadPixelmap("NODETOPT.PIX");
        DisableChoice(1);
        if (image != NULL) {
            DRPixelmapRectangleMaskedCopy(gBack_screen,
                gCurrent_graf_data->graph_opt_disable_x,
                gCurrent_graf_data->graph_opt_disable_y,
                image, 0, 0, image->width, image->height);
            BrPixelmapFree(image);
        }
    }
}

// GLOBAL: CARMA2_HW 0x0065bf18
// GLOBAL: CARMA2_SW 0x00656830
// GLOBAL: CARMA2_CHEAT 0x00663510
tFlicette gSound_options_flicker_on[3] = {
    { 156, { 26, 52 }, { 21, 50 } },
    { 156, { 155, 310 }, { 88, 211 } },
    { 43, { 38, 76 }, { 153, 367 } },
};

// GLOBAL: CARMA2_HW 0x0065bf58
// GLOBAL: CARMA2_SW 0x00656870
// GLOBAL: CARMA2_CHEAT 0x00663550
tFlicette gSound_options_flicker_off[3] = {
    { 155, { 26, 52 }, { 21, 50 } },
    { 155, { 155, 310 }, { 88, 211 } },
    { 42, { 38, 76 }, { 153, 367 } },
};

// GLOBAL: CARMA2_HW 0x0065bf98
// GLOBAL: CARMA2_SW 0x006568b0
// GLOBAL: CARMA2_CHEAT 0x00663590
tFlicette gSound_options_push[3] = {
    { 156, { 26, 52 }, { 21, 50 } },
    { 156, { 155, 310 }, { 88, 211 } },
    { 43, { 38, 76 }, { 153, 367 } },
};

// GLOBAL: CARMA2_HW 0x0065bfd8
// GLOBAL: CARMA2_SW 0x006568f0
// GLOBAL: CARMA2_CHEAT 0x006635d0
tMouse_area gSound_options_mouse_areas[3] = {
    { { 26, 52 }, { 21, 50 }, { 144, 288 }, { 97, 233 }, 0, 0, 0, SoundClick },
    { { 155, 310 }, { 88, 211 }, { 273, 546 }, { 164, 394 }, 1, 0, 0, SoundClick },
    { { 38, 76 }, { 153, 367 }, { 101, 202 }, { 173, 415 }, 2, 0, 0, NULL },
};

// GLOBAL: CARMA2_HW 0x0065c068
// GLOBAL: CARMA2_SW 0x00656980
// GLOBAL: CARMA2_CHEAT 0x00663660
tInterface_spec gSound_options_interface_spec = {
    0,
    150,
    0,
    0,
    0,
    0,
    1,
    { -1, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { SoundOptionsLeft, NULL },
    { -1, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { SoundOptionsRight, NULL },
    { -1, 0 },
    { -1, 0 },
    { 0, 0 },
    { 2, 0 },
    { NULL, NULL },
    { -1, 0 },
    { 1, 0 },
    { 0, 0 },
    { 2, 0 },
    { NULL, NULL },
    { 1, 1 },
    { NULL, NULL },
    { 1, 1 },
    { NULL, NULL },
    NULL,
    NULL,
    0,
    NULL,
    SoundOptionsStart,
    SoundOptionsDone,
    0,
    { 0, 0 },
    NULL,
    2,
    1,
    REC2_ASIZE(gSound_options_flicker_on),
    gSound_options_flicker_on,
    gSound_options_flicker_off,
    gSound_options_push,
    REC2_ASIZE(gSound_options_mouse_areas),
    gSound_options_mouse_areas,
    0,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065c198
// GLOBAL: CARMA2_SW 0x00656ab0
// GLOBAL: CARMA2_CHEAT 0x00663790
tFlicette gGraphics_options_flicker_on[14] = {
    { 43, { 45, 90 }, { 166, 398 } },
    { 43, { 220, 440 }, { 166, 398 } },
    { 238, { 49, 98 }, { 36, 86 } },
    { 239, { 49, 98 }, { 45, 108 } },
    { 240, { 49, 98 }, { 54, 130 } },
    { 241, { 49, 98 }, { 67, 161 } },
    { 242, { 49, 98 }, { 76, 182 } },
    { 243, { 49, 98 }, { 89, 214 } },
    { 244, { 49, 98 }, { 98, 235 } },
    { 245, { 49, 98 }, { 111, 266 } },
    { 246, { 49, 98 }, { 120, 288 } },
    { 247, { 49, 98 }, { 129, 310 } },
    { 248, { 49, 98 }, { 138, 331 } },
    { 249, { 49, 98 }, { 150, 360 } },
};

// GLOBAL: CARMA2_HW 0x0065c2b0
// GLOBAL: CARMA2_SW 0x00656bc8
// GLOBAL: CARMA2_CHEAT 0x006638a8
tFlicette gGraphics_options_flicker_off[14] = {
    { 42, { 45, 90 }, { 166, 398 } },
    { 42, { 220, 440 }, { 166, 398 } },
    { 265, { 49, 98 }, { 36, 86 } },
    { 266, { 49, 98 }, { 45, 108 } },
    { 267, { 49, 98 }, { 54, 130 } },
    { 268, { 49, 98 }, { 67, 161 } },
    { 269, { 49, 98 }, { 76, 182 } },
    { 270, { 49, 98 }, { 89, 214 } },
    { 271, { 49, 98 }, { 98, 235 } },
    { 272, { 49, 98 }, { 111, 266 } },
    { 273, { 49, 98 }, { 120, 288 } },
    { 274, { 49, 98 }, { 129, 310 } },
    { 275, { 49, 98 }, { 138, 331 } },
    { 276, { 49, 98 }, { 150, 360 } },
};

// GLOBAL: CARMA2_HW 0x0065c3c8
// GLOBAL: CARMA2_SW 0x00656ce0
// GLOBAL: CARMA2_CHEAT 0x006639c0
tFlicette gGraphics_options_push[14] = {
    { 154, { 45, 90 }, { 166, 398 } },
    { 45, { 220, 440 }, { 166, 398 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
    { 45, { 210, 440 }, { 170, 408 } },
};

// GLOBAL: CARMA2_HW 0x0065c4e0
// GLOBAL: CARMA2_SW 0x00656df8
// GLOBAL: CARMA2_CHEAT 0x00663ad8
tMouse_area gGraphics_options_mouse_areas[14] = {
    { { 45, 90 }, { 165, 396 }, { 107, 214 }, { 185, 444 }, 0, 0, 0, NULL },
    { { 220, 440 }, { 165, 396 }, { 276, 552 }, { 185, 444 }, 1, 0, 0, NULL },
    { { 49, 98 }, { 35, 84 }, { 284, 568 }, { 43, 103 }, 2, 1, 0, RadioClick },
    { { 49, 98 }, { 44, 106 }, { 284, 568 }, { 52, 125 }, 3, 1, 0, RadioClick },
    { { 49, 98 }, { 53, 127 }, { 284, 568 }, { 61, 146 }, 4, 1, 0, RadioClick },
    { { 49, 98 }, { 66, 158 }, { 284, 568 }, { 74, 178 }, 5, 1, 0, RadioClick },
    { { 49, 98 }, { 75, 180 }, { 284, 568 }, { 83, 199 }, 6, 1, 0, RadioClick },
    { { 49, 98 }, { 88, 192 }, { 284, 568 }, { 96, 230 }, 7, 1, 0, RadioClick },
    { { 49, 98 }, { 97, 233 }, { 284, 568 }, { 105, 252 }, 8, 1, 0, RadioClick },
    { { 49, 98 }, { 110, 264 }, { 284, 568 }, { 118, 283 }, 9, 1, 0, RadioClick },
    { { 49, 98 }, { 119, 286 }, { 284, 568 }, { 127, 305 }, 10, 1, 0, RadioClick },
    { { 49, 98 }, { 128, 307 }, { 284, 322 }, { 136, 326 }, 11, 1, 0, RadioClick },
    { { 49, 98 }, { 137, 329 }, { 284, 322 }, { 145, 348 }, 12, 1, 0, RadioClick },
    { { 49, 98 }, { 149, 358 }, { 284, 322 }, { 157, 377 }, 13, 1, 0, RadioClick },
};

// GLOBAL: CARMA2_HW 0x0065c780
// GLOBAL: CARMA2_SW 0x00657098
// GLOBAL: CARMA2_CHEAT 0x00663d78
tInterface_spec gGraphics_options_interface_spec = {
    0,
    160,
    0,
    0,
    0,
    0,
    1,
    { -1, 0 },
    { -1, 0 },
    { 0, 2 },
    { 1, 13 },
    { NULL, GraphOptLeft },
    { -1, 0 },
    { 1, 0 },
    { 0, 2 },
    { 1, 13 },
    { NULL, GraphOptRight },
    { -1, -1 },
    { 0, -1 },
    { 0, 1 },
    { 0, 13 },
    { GraphOptUp, GraphOptUp },
    { -1, -1 },
    { 1, 1 },
    { 0, 2 },
    { 0, 14 },
    { GraphOptDown, GraphOptDown },
    { 1, 1 },
    { NULL, GraphOptGoAhead },
    { 1, 1 },
    { NULL, NULL },
    NULL,
    DrawGraphBox,
    0,
    NULL,
    DrawInitialRadios,
    NULL,
    0,
    { 0, 0 },
    NULL,
    1,
    1,
    REC2_ASIZE(gGraphics_options_flicker_on),
    gGraphics_options_flicker_on,
    gGraphics_options_flicker_off,
    gGraphics_options_push,
    REC2_ASIZE(gGraphics_options_mouse_areas),
    gGraphics_options_mouse_areas,
    0,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065c8b0
// GLOBAL: CARMA2_SW 0x006571c8
// GLOBAL: CARMA2_CHEAT 0x00663ea8
tFlicette gControl_options_flicker_on[4] = {
    { 177, { 51, 102 }, { 166, 398 } },
    { 177, { 112, 224 }, { 166, 398 } },
    { 177, { 173, 346 }, { 166, 398 } },
    { 177, { 234, 468 }, { 166, 398 } },
};

// GLOBAL: CARMA2_HW 0x0065c900
// GLOBAL: CARMA2_SW 0x00657218
// GLOBAL: CARMA2_CHEAT 0x00663ef8
tFlicette gControl_options_flicker_off[4] = {
    { 176, { 51, 102 }, { 166, 398 } },
    { 176, { 112, 224 }, { 166, 398 } },
    { 176, { 173, 346 }, { 166, 398 } },
    { 176, { 234, 468 }, { 166, 398 } },
};

// GLOBAL: CARMA2_HW 0x0065c950
// GLOBAL: CARMA2_SW 0x00657268
// GLOBAL: CARMA2_CHEAT 0x00663f48
tFlicette gControl_options_push[4] = {
    { 172, { 51, 102 }, { 166, 398 } },
    { 175, { 112, 224 }, { 166, 398 } },
    { 174, { 173, 346 }, { 166, 398 } },
    { 173, { 234, 468 }, { 166, 398 } },
};

// GLOBAL: CARMA2_HW 0x0065c9a0
// GLOBAL: CARMA2_SW 0x006572b8
// GLOBAL: CARMA2_CHEAT 0x00663f98
tMouse_area gControl_options_mouse_areas[5] = {
    { { 51, 102 }, { 166, 398 }, { 102, 204 }, { 187, 449 }, 0, 0, 0, NULL },
    { { 112, 224 }, { 166, 398 }, { 164, 328 }, { 187, 449 }, 1, 0, 0, NULL },
    { { 173, 346 }, { 166, 398 }, { 225, 450 }, { 187, 449 }, 2, 0, 0, NULL },
    { { 234, 468 }, { 166, 398 }, { 286, 572 }, { 187, 449 }, 3, 0, 0, NULL },
    { { 45, 90 }, { 33, 79 }, { 285, 570 }, { 159, 382 }, 4, 1, 0, MouseyClickBastard },
};

// GLOBAL: CARMA2_HW 0x0065ca90
// GLOBAL: CARMA2_SW 0x006573a8
// GLOBAL: CARMA2_CHEAT 0x00664088
tInterface_spec gControl_options_interface_spec = {
    0,
    170,
    179,
    0,
    0,
    0,
    1,
    { -1, -1 },
    { -1, 0 },
    { 0, 4 },
    { 3, 4 },
    { NULL, KeyAssignLeft },
    { -1, -1 },
    { 1, 0 },
    { 0, 4 },
    { 3, 4 },
    { NULL, KeyAssignRight },
    { -1, -1 },
    { 0, 0 },
    { 0, 4 },
    { 3, 4 },
    { KeyAssignUp, KeyAssignUp },
    { -1, -1 },
    { 0, 0 },
    { 0, 4 },
    { 3, 4 },
    { KeyAssignDown, KeyAssignDown },
    { 1, 1 },
    { KeyAssignGoAhead, KeyAssignGoAhead },
    { 1, 1 },
    { NULL, NULL },
    NULL,
    DrawKeyAssignments,
    0,
    NULL,
    DrawInitialKMRadios,
    NULL,
    0,
    { 0, 0 },
    NULL,
    3,
    1,
    REC2_ASIZE(gControl_options_flicker_on),
    gControl_options_flicker_on,
    gControl_options_flicker_off,
    gControl_options_push,
    REC2_ASIZE(gControl_options_mouse_areas),
    gControl_options_mouse_areas,
    0,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0065cbc0
// GLOBAL: CARMA2_SW 0x006574d8
// GLOBAL: CARMA2_CHEAT 0x006641b8
tFlicette gOptions_flicker_on[4] = {
    { 43, { 57, 114 }, { 41, 98 } },
    { 43, { 57, 114 }, { 78, 187 } },
    { 43, { 57, 114 }, { 114, 274 } },
    { 43, { 57, 114 }, { 154, 370 } },
};

// GLOBAL: CARMA2_HW 0x0065cc10
// GLOBAL: CARMA2_SW 0x00657528
// GLOBAL: CARMA2_CHEAT 0x00664208
tFlicette gOptions_flicker_off[4] = {
    { 42, { 57, 114 }, { 41, 98 } },
    { 42, { 57, 114 }, { 78, 187 } },
    { 42, { 57, 114 }, { 114, 274 } },
    { 42, { 57, 114 }, { 154, 370 } },
};

// GLOBAL: CARMA2_HW 0x0065cc60
// GLOBAL: CARMA2_SW 0x00657578
// GLOBAL: CARMA2_CHEAT 0x00664258
tFlicette gOptions_push[4] = {
    { 144, { 57, 114 }, { 41, 98 } },
    { 146, { 57, 114 }, { 78, 187 } },
    { 145, { 57, 114 }, { 114, 274 } },
    { 45, { 57, 114 }, { 154, 370 } },
};

// GLOBAL: CARMA2_HW 0x0065ccb0
// GLOBAL: CARMA2_SW 0x006575c8
// GLOBAL: CARMA2_CHEAT 0x006642a8
tMouse_area gOptions_mouse_areas[4] = {
    { { 57, 114 }, { 41, 98 }, { 123, 246 }, { 62, 149 }, 0, 0, 0, NULL },
    { { 57, 114 }, { 78, 187 }, { 123, 246 }, { 99, 238 }, 1, 0, 0, NULL },
    { { 57, 114 }, { 114, 274 }, { 123, 246 }, { 135, 324 }, 2, 0, 0, NULL },
    { { 57, 114 }, { 154, 370 }, { 123, 246 }, { 175, 420 }, 3, 0, 0, NULL },
};

// GLOBAL: CARMA2_HW 0x0065cd70
// GLOBAL: CARMA2_SW 0x00657688
// GLOBAL: CARMA2_CHEAT 0x00664368
tInterface_spec gOptions_interface_spec = {
    0,
    140,
    0,
    141,
    141,
    141,
    1,
    { -1, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { NULL, NULL },
    { -1, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { NULL, NULL },
    { -1, 0 },
    { -1, 0 },
    { 0, 0 },
    { 3, 0 },
    { NULL, NULL },
    { -1, 0 },
    { 1, 0 },
    { 0, 0 },
    { 3, 0 },
    { NULL, NULL },
    { 1, 1 },
    { NULL, NULL },
    { 1, 1 },
    { NULL, NULL },
    NULL,
    NULL,
    0,
    NULL,
    DrawDisabledOptions,
    NULL,
    0,
    { 0, 0 },
    NULL,
    3,
    1,
    REC2_ASIZE(gOptions_flicker_on),
    gOptions_flicker_on,
    gOptions_flicker_off,
    gOptions_push,
    REC2_ASIZE(gOptions_mouse_areas),
    gOptions_mouse_areas,
    0,
    NULL,
};

inline void C2_HOOK_FASTCALL DoSoundOptions(void) {

    DoInterfaceScreen(&gSound_options_interface_spec, 0, 0);
    if (gProgram_state.racing) {
        FadePaletteDown();
    } else {
        RunFlic(151);
    }
}

inline void C2_HOOK_FASTCALL GetGraphicsOptions(void) {
    int value;
    br_scalar br_value;

    value = GetCarSimplificationLevel();
    if (value < 2) {
        gRadio_bastards__options[0].current_value = value;
    } else {
        gRadio_bastards__options[0].current_value = value - 1;
    }

    switch (GetCarTexturingLevel()) {
    case eCTL_none:
        gRadio_bastards__options[1].current_value = 2;
        break;
    case eCTL_transparent:
        gRadio_bastards__options[1].current_value = 1;
        break;
    case eCTL_full:
        gRadio_bastards__options[1].current_value = 0;
        break;
    default:
        break;
    }

    switch (GetShadowLevel()) {
    case eShadow_none:
        gRadio_bastards__options[2].current_value = 2;
        break;
    case eShadow_us_only:
        gRadio_bastards__options[2].current_value = 1;
        break;
    default:
        gRadio_bastards__options[2].current_value = 0;
        break;
    }

    switch (GetWallTexturingLevel()) {
    case eWTL_none:
        gRadio_bastards__options[3].current_value = 2;
        break;
    case eWTL_linear:
        gRadio_bastards__options[3].current_value = 1;
        break;
    case eWTL_full:
        gRadio_bastards__options[3].current_value = 0;
        break;
    default:
        break;
    }

    switch (GetRoadTexturingLevel()) {
    case eRTL_none:
        gRadio_bastards__options[4].current_value = 2;
        break;
    case eRTL_full:
        gRadio_bastards__options[4].current_value = 0;
        break;
    default:
        break;
    }

    br_value = GetYon();
    gRadio_bastards__options[5].current_value = (int)((35.0f - br_value) / 5.0f);
    if (gRadio_bastards__options[5].current_value > 3) {
        gRadio_bastards__options[5].current_value = 3;
    }

    br_value = GetYonFactor();
    if (br_value >= 1.0f) {
        gRadio_bastards__options[6].current_value = 0;
    } else if (br_value >= 0.5f) {
        gRadio_bastards__options[6].current_value = 1;
    } else if (br_value >= 0.25f) {
        gRadio_bastards__options[6].current_value = 2;
    } else {
        gRadio_bastards__options[6].current_value = 3;
    }

    if (GetSkyTextureOn()) {
        gRadio_bastards__options[7].current_value = 0;
    } else {
        gRadio_bastards__options[7].current_value = 1;
    }

    if (GetDepthCueingOn()) {
        gRadio_bastards__options[8].current_value = 0;
    } else {
        gRadio_bastards__options[8].current_value = 1;
    }

    if (GetAccessoryRendering()) {
        gRadio_bastards__options[9].current_value = 0;
    } else {
        gRadio_bastards__options[9].current_value = 1;
    }

    if (GetSmokeOn()) {
        gRadio_bastards__options[10].current_value = 0;
    } else {
        gRadio_bastards__options[10].current_value = 1;
    }

    value = GetSoundDetailLevel();
    gRadio_bastards__options[11].current_value = 2 - value;
}

inline void C2_HOOK_FASTCALL DoGraphicsOptions(void) {

    gThe_interface_spec__options = &gGraphics_options_interface_spec;
    gLast_graph_sel__options = -1;
    LoadFont(12);
    GetGraphicsOptions();
    if (DoInterfaceScreen(&gGraphics_options_interface_spec, 0, 0) == 0) {
        SetGraphicsOptions();
    }
    if (gProgram_state.racing) {
        FadePaletteDown();
    } else {
        RunFlic(161);
    }
    DisposeFont(12);
}

inline void C2_HOOK_FASTCALL CalibrateJoysticks(void) {

    NetFullScreenMessage(eMiscString_use_system_control_panel_for_joysticks, 0);
}

inline void C2_HOOK_FASTCALL LoadKeyNames__options(void) {
    int i;
    FILE* f;
    tPath_name the_path;
    char s[256];

    PathCat(the_path, gApplication_path, "KEYNAMES.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantOpenKeyNamesFile);
    }
    for (i = 0; i < (int)REC2_ASIZE(gKey_names__options); i++) {
        PFfgets(s, sizeof(s), f);
        StripControls(s);
        gKey_names__options[i] = BrMemAllocate(strlen(s) + 1, kMem_misc);
        strcpy(gKey_names__options[i], s);
    }
    PFfclose(f);
}

inline void C2_HOOK_FASTCALL DoControlOptions(void) {
    int result;
    int second_time_around;
    int orig_key_map_index;

    orig_key_map_index = gKey_map_index;
    second_time_around = 0;
    gThe_interface_spec__options = &gControl_options_interface_spec;
    SaveOrigKeyMapping();
    LoadKeyNames__options();
    LoadFont(9);
    LoadFont(11);
    LoadFont(10);
    LoadFont(12);
    gPending_entry = -1;
    gKey_count = 18;
    gCurrent_key = 8;
    while ((result = DoInterfaceScreen(&gControl_options_interface_spec, second_time_around, 0)) < 0) {
        FadePaletteDown();
        CalibrateJoysticks();
        FadePaletteDown();
        second_time_around = 1;
    }
    if (result == 0) {
        SaveKeyMapping__options();
    } else {
        memcpy(gKey_mapping, gOrig_key_mapping, sizeof(gKey_mapping));
        gKey_map_index = orig_key_map_index;
    }
    DisposeFont(9);
    DisposeFont(11);
    DisposeFont(10);
    DisposeFont(12);
    DisposeKeyNames__options();
    if (gProgram_state.racing) {
        FadePaletteDown();
    } else {
        RunFlic(171);
    }
}

// FUNCTION: CARMA2_HW 0x004b39f0
// FUNCTION: CARMA2_SW 0x004afb60
// FUNCTION: CARMA2_CHEAT 0x004b92c0
void C2_HOOK_FASTCALL DoOptions(void) {
    int result;

    PrintMemoryDump(0, "BEFORE ENTERING OPTIONS");
    PreloadBunchOfFlics(1);
    result = DoInterfaceScreen(&gOptions_interface_spec, 0, gProgram_state.track_spec.the_actor != NULL ? 2 : 1);
    switch (result) {
    case 0:
        DoSoundOptions();
        break;
    case 1:
        DoGraphicsOptions();
        break;
    case 2:
        DoControlOptions();
        break;
    }
    UnlockBunchOfFlics(1);
    PrintMemoryDump(0, "AFTER ENTERING OPTIONS");
}

// FUNCTION: CARMA2_HW 0x004b3ec0
// FUNCTION: CARMA2_SW 0x004b0030
// FUNCTION: CARMA2_CHEAT 0x004b9790
void C2_HOOK_FASTCALL SetGraphicsOptions(void) {

    if (gRadio_bastards__options[0].current_value >= 2) {
        SetCarSimplificationLevel(gRadio_bastards__options[0].current_value + 1);
    } else {
        SetCarSimplificationLevel(gRadio_bastards__options[0].current_value);
    }

    switch (gRadio_bastards__options[1].current_value) {
    case 2:
        SetCarTexturingLevel(eCTL_none);
        break;
    case 1:
        SetCarTexturingLevel(eCTL_transparent);
        break;
    case 0:
        SetCarTexturingLevel(eCTL_full);
        break;
    }

    switch (gRadio_bastards__options[2].current_value) {
    case 0:
        SetShadowLevel(eShadow_everyone);
        break;
    case 1:
        SetShadowLevel(eShadow_us_only);
        break;
    case 2:
        SetShadowLevel(eShadow_none);
        break;
    }

    switch (gRadio_bastards__options[3].current_value) {
    case 0:
        SetWallTexturingLevel(eWTL_full);
        break;
    case 1:
        SetWallTexturingLevel(eWTL_linear);
        break;
    case 2:
        SetWallTexturingLevel(eWTL_none);
        break;
    }

    switch (gRadio_bastards__options[4].current_value) {
    case 0:
        SetRoadTexturingLevel(eRTL_full);
        break;
    case 2:
        SetRoadTexturingLevel(eRTL_none);
        break;
    }

    SetYon(35.0f - gRadio_bastards__options[5].current_value * 5.0f);

    switch (gRadio_bastards__options[6].current_value) {
    case 0:
        SetYonFactor(1.0f);
        break;
    case 1:
        SetYonFactor(0.5f);
        break;
    case 2:
        SetYonFactor(0.25f);
        break;
    case 3:
        SetYonFactor(0.125f);
        break;
    }

    SetSkyTextureOn(1 - gRadio_bastards__options[7].current_value);
    SetDepthCueingOn(1 - gRadio_bastards__options[8].current_value);
    SetAccessoryRendering(1 - gRadio_bastards__options[9].current_value);
    SetSmokeOn(1 - gRadio_bastards__options[10].current_value);
    SetSoundDetailLevel(2 - gRadio_bastards__options[11].current_value);
    SaveOptions();
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004b4070
// FUNCTION: CARMA2_SW 0x004b01e0
// FUNCTION: CARMA2_CHEAT 0x004b9940
void C2_HOOK_FASTCALL DisposeKeyNames__options(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gKey_names__options); i++) {
        BrMemFree(gKey_names__options[i]);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SaveKeyMapping (see 72-interface)

// FUNCTION: CARMA2_HW 0x004b4090
// FUNCTION: CARMA2_SW 0x004b0200
// FUNCTION: CARMA2_CHEAT 0x004b9960
void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend) {
    float yon_factor;
    float yon;

    switch (GetCarSimplificationLevel()) {
    case 0:
        SelectThisItemIn(pFrontend, 1, 3);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 1, 4);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 1, 5);
        break;
    case 3:
        SelectThisItemIn(pFrontend, 1, 6);
        break;
    }

    yon = GetYon();
    switch ((int)((35.f - yon) / 5.f)) {
    case 0:
        SelectThisItemIn(pFrontend, 2, 8);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 2, 9);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 2, 10);
        break;
    default:
        SelectThisItemIn(pFrontend, 2, 11);
        break;
    }

    yon_factor = GetYonFactor();
    if (yon_factor >= 1.f) {
        SelectThisItemIn(pFrontend, 3, 13);
    } else if (yon_factor >= 0.5f) {
        SelectThisItemIn(pFrontend, 3, 14);
    } else if (yon_factor >= 0.25f) {
        SelectThisItemIn(pFrontend, 3, 15);
    } else {
        SelectThisItemIn(pFrontend, 3, 16);
    }

    switch (GetShadowLevel()) {
    default:
        SelectThisItemIn(pFrontend, 4, 18);
        break;
    case eShadow_none:
        SelectThisItemIn(pFrontend, 4, 20);
        break;
    case eShadow_us_only:
        SelectThisItemIn(pFrontend, 4, 19);
        break;
    }

    switch (GetSoundDetailLevel()) {
    case 0:
        SelectThisItemIn(pFrontend, 5, 24);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 5, 23);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 5, 22);
        break;
    }

    switch (GetHowMuchBloodAndSnotToSmearAbout()) {
    case 0:
        SelectThisItemIn(pFrontend, 6, 26);
        break;
    case 1:
        SelectThisItemIn(pFrontend, 6, 27);
        break;
    case 2:
        SelectThisItemIn(pFrontend, 6, 28);
        break;
    }

    if (GetSkyTextureOn()) {
        SelectThisItemIn(pFrontend, 7, 30);
    } else {
        SelectThisItemIn(pFrontend, 7, 31);
    }

    if (GetDepthCueingOn()) {
        SelectThisItemIn(pFrontend, 8, 33);
    } else {
        SelectThisItemIn(pFrontend, 8, 34);
    }

    if (OtherPerambulatoryInstasificationIsEnablificated()) {
        SelectThisItemIn(pFrontend, 9, 36);
    } else {
        SelectThisItemIn(pFrontend, 9, 37);
    }

    if (AreWeGoingToOutrageAnimalLoversAroundTheGlobe()) {
        SelectThisItemIn(pFrontend, 10, 39);
    } else {
        SelectThisItemIn(pFrontend, 10, 40);
    }

    if (IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense()) {
        SelectThisItemIn(pFrontend, 11, 42);
    } else {
        SelectThisItemIn(pFrontend, 11, 43);
    }

    if (IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()) {
        SelectThisItemIn(pFrontend, 12, 45);
    } else {
        SelectThisItemIn(pFrontend, 12, 46);
    }

    if (gAuto_load) {
        SelectThisItemIn(pFrontend, 13, 48);
    } else {
        SelectThisItemIn(pFrontend, 13, 49);
    }
    if (gAmbient_sound) {
        SelectThisItemIn(pFrontend, 14, 51);
    } else {
        SelectThisItemIn(pFrontend, 14, 52);
    }
}

// FUNCTION: CARMA2_HW 0x004b42c0
// FUNCTION: CARMA2_SW 0x004b0430
// FUNCTION: CARMA2_CHEAT 0x004b9b90
void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend) {

    switch (WhichItemIsSelectedIn(pFrontend, 1)) {
    case 3:
        SetCarSimplificationLevel(0);
        break;
    case 4:
        SetCarSimplificationLevel(1);
        break;
    case 5:
        SetCarSimplificationLevel(2);
        break;
    case 6:
        SetCarSimplificationLevel(3);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 2)) {
    case 8:
        SetYon(35.f);
        break;
    case 9:
        SetYon(30.f);
        break;
    case 10:
        SetYon(25.f);
        break;
    case 11:
        SetYon(20.f);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 3)) {
    case 13:
        SetYonFactor(1.0f);
        break;
    case 14:
        SetYonFactor(0.5f);
        break;
    case 15:
        SetYonFactor(0.25f);
        break;
    case 16:
        SetYonFactor(0.125f);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 4)) {
    case 18:
        SetShadowLevel(eShadow_everyone);
        break;
    case 19:
        SetShadowLevel(eShadow_us_only);
        break;
    case 20:
        SetShadowLevel(eShadow_none);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 5)) {
    case 22:
        SetSoundDetailLevel(2);
        break;
    case 23:
        SetSoundDetailLevel(1);
        break;
    case 24:
        SetSoundDetailLevel(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 6)) {
    case 26:
        SetGoreLevel(0);
        break;
    case 27:
        SetGoreLevel(1);
        break;
    case 28:
        SetGoreLevel(2);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 7)) {
    case 30:
        SetSkyTextureOn(1);
        break;
    case 31:
        SetSkyTextureOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 8)) {
    case 33:
        SetDepthCueingOn(1);
        break;
    case 34:
        SetDepthCueingOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 9)) {
    case 36:
        SetTrafficOn(1);
        break;
    case 37:
        SetTrafficOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 10)) {
    case 39:
        SetAnimalsOn(1);
        break;
    case 40:
        SetAnimalsOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 11)) {
    case 42:
        SetFlameThrowerOn(1);
        break;
    case 43:
        SetFlameThrowerOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 12)) {
    case 45:
        SetExplosivesOn(1);
        break;
    case 46:
        SetExplosivesOn(0);
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 13)) {
    case 48:
        gAuto_load = 1;
        break;
    case 49:
        gAuto_load = 0;
        break;
    }

    switch (WhichItemIsSelectedIn(pFrontend, 14)) {
    case 51:
        gAmbient_sound = 1;
        break;
    case 52:
        gAmbient_sound = 0;
        break;
    }

    SaveOptions();
}


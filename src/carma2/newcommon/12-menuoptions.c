/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "12-menuoptions.h"

#include "10-loading2.h"

#include "16-graphics1.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "25-grafdata.h"
#include "33-depth.h"
#include "35-intrface.h"
#include "37-brucetrk.h"
#include "38-flicplay.h"
#include "52-errors.h"
#include "57-drone.h"
#include "61-pedestrn-phys.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"

#include "rec2_macros.h"

#include <math.h>

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

// SaveOrigKeyMapping

// SaveKeyMapping (see 72-interface)

// ChangeKeyMapIndex

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


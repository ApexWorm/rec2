/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "45-cutscene.h"

#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "42-input.h"
#include "41-utility.h"
#include "69-sound.h"
#include "globvars.h"

// FUNCTION: CARMA2_HW 0x0043e750
// FUNCTION: CARMA2_SW 0x0043e860
// FUNCTION: CARMA2_CHEAT 0x0043ea00
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
// FUNCTION: CARMA2_SW 0x0043e870
// FUNCTION: CARMA2_CHEAT 0x0043ea10
void C2_HOOK_STDCALL DoStainlessLogo(void) {
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0043e770
// STUB: CARMA2_SW 0x0043e880
// STUB: CARMA2_CHEAT 0x0043ea20
void C2_HOOK_FASTCALL PlaySmackerFile(char* pSmack_name) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0043f330
// FUNCTION: CARMA2_SW 0x0043f410
// FUNCTION: CARMA2_CHEAT 0x0043f5e0
void C2_HOOK_FASTCALL DoOpeningAnimation(void) {
    PlaySmackerFile("INTERPLAYGO.SMK");
    PlaySmackerFile("MIX_INTR.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f350
// FUNCTION: CARMA2_SW 0x0043f430
// FUNCTION: CARMA2_CHEAT 0x0043f600
void C2_HOOK_FASTCALL DoNewGameAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f360
// FUNCTION: CARMA2_SW 0x0043f440
// FUNCTION: CARMA2_CHEAT 0x0043f610
void C2_HOOK_FASTCALL DoGoToTrackAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f370
// FUNCTION: CARMA2_SW 0x0043f450
// FUNCTION: CARMA2_CHEAT 0x0043f620
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {
    FadePaletteDown();
}

// FUNCTION: CARMA2_HW 0x0043f380
// FUNCTION: CARMA2_SW 0x0043f460
// FUNCTION: CARMA2_CHEAT 0x0043f630
void C2_HOOK_FASTCALL DoGameOverAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f390
// FUNCTION: CARMA2_SW 0x0043f470
// FUNCTION: CARMA2_CHEAT 0x0043f640
void C2_HOOK_FASTCALL DoGameCompletedAnimation(void) {

    PlaySmackerFile("TOPRANK.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f5c0
// FUNCTION: CARMA2_SW 0x0043f6a0
// FUNCTION: CARMA2_CHEAT 0x0043f870
void C2_HOOK_FASTCALL StartLoadingScreen(void) {

    SwitchToLoresMode();
    StartMusicTrack(9998);
    PossibleService();
    if (gProgram_state.sausage_eater_mode) {
        SplashScreenWith("LOADSCRN.PIX");
    } else {
        SplashScreenWith("LOADSCRN.PIX");
    }
}


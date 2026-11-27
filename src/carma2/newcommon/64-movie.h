/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_64_MOVIE_H
#define GUARD_64_MOVIE_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include <ImageCompression.h>

extern int gNext_replay_record_state;
extern int gShow_status_message;

extern int gShow_status_message;
extern int gStatus_message_1;
extern int gStatus_message_2;

extern char gQuick_time_temp_path[256];
extern char gQuick_time_movie_path_stub[256];
extern char gQuick_time_banner_texture_name[256];

extern void C2_HOOK_FASTCALL Create555BELookupTable(void);

extern void C2_HOOK_FASTCALL Convert565BrPixelmapTo555BigEndianPixmap(PixMapHandle pDest, br_pixelmap* pSrc);

// SwapPixelmapBytePairs

extern int C2_HOOK_FASTCALL WriteBannerFrame(void);

extern tU16* C2_HOOK_FASTCALL Allocate555BackScreen(void);

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

// InitMovie

extern OSErr C2_HOOK_FASTCALL CreateMovie(void);

// MovieStartNewMovie

extern int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void);

extern void C2_HOOK_FASTCALL FreeOffQTshite(void);

extern void C2_HOOK_CDECL InitQuickTimeStuff(void);

extern void C2_HOOK_FASTCALL SetQuickTimeDefaults(void);

// MovieRecordButtonPressed

extern void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void);

extern OSErr C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen);

// MovieRecordFrame

extern int C2_HOOK_FASTCALL MovieRecording(void);

extern void C2_HOOK_FASTCALL KillStatusMessage(void);

extern void C2_HOOK_FASTCALL ShowStatusMessage(int pMessage1, int pMessage2);

extern int C2_HOOK_FASTCALL GetMovieWidth(void);

extern int C2_HOOK_FASTCALL GetMovieHeight(void);

extern void C2_HOOK_FASTCALL TidyMovieARDisplay(void);

// TrapS3SoundOutput

// FreeS3SoundOutput

#endif // GUARD_64_MOVIE_H

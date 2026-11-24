/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "64-movie.h"

#include "16-graphics1.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "globvars.h"
#include "platform.h"

#include <FileTypesAndCreators.h>
#include <GXMath.h>
#include <ImageCompression.h>
#include <MacMemory.h>
#include <Movies.h>
#include <QTML.h>
#include <Script.h>
#include <TextUtils.h>

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x006a0c24
// GLOBAL: CARMA2_SW 0x0069ac7c
// GLOBAL: CARMA2_CHEAT 0x006a879c
int gQuickTime_initialized;

// GLOBAL: CARMA2_HW 0x006a0c50
// GLOBAL: CARMA2_SW 0x0069aca8
// GLOBAL: CARMA2_CHEAT 0x006a87c8
int gNext_replay_record_state;

// GLOBAL: CARMA2_HW 0x006a23b8
// GLOBAL: CARMA2_SW 0x0069c410
// GLOBAL: CARMA2_CHEAT 0x006a9f30
int gShow_status_message;

// GLOBAL: CARMA2_HW 0x006a23bc
// GLOBAL: CARMA2_SW 0x0069c414
// GLOBAL: CARMA2_CHEAT 0x006a9f34
int gStatus_message_1;

// GLOBAL: CARMA2_HW 0x006a23c0
// GLOBAL: CARMA2_SW 0x0069c418
// GLOBAL: CARMA2_CHEAT 0x006a9f38
int gStatus_message_2;

// GLOBAL: CARMA2_HW 0x00705440
// GLOBAL: CARMA2_SW 0x006fbf00
// GLOBAL: CARMA2_CHEAT 0x0070cae0
char gQuick_time_temp_path[256];

// GLOBAL: CARMA2_HW 0x00705240
// GLOBAL: CARMA2_SW 0x006fbd00
// GLOBAL: CARMA2_CHEAT 0x0070c8e0
char gQuick_time_movie_path_stub[256];

// GLOBAL: CARMA2_HW 0x00705340
// GLOBAL: CARMA2_SW 0x006fbe00
// GLOBAL: CARMA2_CHEAT 0x0070c9e0
char gQuick_time_banner_texture_name[256];

// GLOBAL: CARMA2_HW 0x0065ee50
// GLOBAL: CARMA2_CHEAT 0x00666628
char gDefault_quick_time_quality[32] = "normal";

// GLOBAL: CARMA2_HW 0x0065ed30
// GLOBAL: CARMA2_SW 0x00659690
// GLOBAL: CARMA2_CHEAT 0x00666508
char gDefault_quick_time_compressor[32] = "animation";

// GLOBAL: CARMA2_HW 0x006a0af0
// GLOBAL: CARMA2_SW 0x0069ab48
// GLOBAL: CARMA2_CHEAT 0x006a8668
short gQuicktime_movie_file;

// GLOBAL: CARMA2_HW 0x006a0af4
// GLOBAL: CARMA2_SW 0x0069ab4c
// GLOBAL: CARMA2_CHEAT 0x006a866c
ImageSequence gQuicktime_image_sequence;

// GLOBAL: CARMA2_HW 0x006a0afc
// GLOBAL: CARMA2_SW 0x0069ab54
// GLOBAL: CARMA2_CHEAT 0x006a8674
Media gQuicktime_media;

// GLOBAL: CARMA2_HW 0x006a0c10
// GLOBAL: CARMA2_SW 0x0069ac68
// GLOBAL: CARMA2_CHEAT 0x006a8788
Track gQuicktime_track;

// GLOBAL: CARMA2_HW 0x006a0c2c
// GLOBAL: CARMA2_SW 0x0069ac84
// GLOBAL: CARMA2_CHEAT 0x006a87a4
int gQuicktime_movie_counter;

// GLOBAL: CARMA2_HW 0x006a0c30
// GLOBAL: CARMA2_SW 0x0069ac88
// GLOBAL: CARMA2_CHEAT 0x006a87a8
Movie gQuicktime_movie;

// GLOBAL: CARMA2_HW 0x006a0c34
// GLOBAL: CARMA2_SW 0x0069ac8c
// GLOBAL: CARMA2_CHEAT 0x006a87ac
Handle gQuicktime_frame_pixmap_handle;

// GLOBAL: CARMA2_HW 0x006a0c3c
// GLOBAL: CARMA2_SW 0x0069ac94
// GLOBAL: CARMA2_CHEAT 0x006a87b4
Handle gQuicktime_compressed_frame_handle;

// GLOBAL: CARMA2_HW 0x006a0c40
// GLOBAL: CARMA2_SW 0x0069ac98
// GLOBAL: CARMA2_CHEAT 0x006a87b8
Handle gQuicktime_image_description_handle;

// Create555BELookupTable

// Convert565BrPixelmapTo555BigEndianPixmap

// SwapPixelmapBytePairs

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004e1250
// STUB: CARMA2_SW 0x004dd570
// STUB: CARMA2_CHEAT 0x004e6e10
void C2_HOOK_FASTCALL WriteBannerFrame(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// Allocate555BackScreen

// GetCompressorQualityFromOptions

// GetCompressorTypeFromOptions

// InitMovie

// CreateMovie

// MovieStartNewMovie

inline void C2_HOOK_FASTCALL FreeOffQTshite(void) {
    char path[260];

    if (gQuicktime_movie_file != 0) {
        CloseMovieFile(gQuicktime_movie_file);
    }
    DisposeMovie(gQuicktime_movie);
    PDFileDelete(gQuick_time_temp_path, 1);
    sprintf(path, "%s.#Res", gQuick_time_temp_path);
    PDFileDelete(path, 1);
    if (gQuicktime_frame_pixmap_handle != NULL) {
        DisposeHandle(gQuicktime_frame_pixmap_handle);
        gQuicktime_frame_pixmap_handle = NULL;
    }
    if (gQuicktime_image_description_handle != NULL) {
        DisposeHandle(gQuicktime_image_description_handle);
        gQuicktime_image_description_handle = NULL;
    }
    if (gQuicktime_compressed_frame_handle != NULL) {
        DisposeHandle(gQuicktime_compressed_frame_handle);
        gQuicktime_compressed_frame_handle = NULL;
    }
}

inline int C2_HOOK_FASTCALL MovieStopRecordingAndSave(void) {
    FSSpec fs_spec;
    TimeValue duration;
    char path[260];

    if (gNext_replay_record_state == 4) {
        gNext_replay_record_state = 5;
        ShowStatusMessage(eMiscString_post_processing, eMiscString_please_wait);
        TidyMovieARDisplay();
        WriteBannerFrame();
        gNext_replay_record_state = 0;
        CDSequenceEnd(gQuicktime_image_sequence);
        EndMediaEdits(gQuicktime_media);
        duration = GetMediaDuration(gQuicktime_media);
        if (duration != 0) {
            InsertMediaIntoTrack(gQuicktime_track, 0, 0, duration, IntToFixed(1));
        }
        if (gQuicktime_movie_file != 0) {
            CloseMovieFile(gQuicktime_movie_file);
        }
        sprintf(path, "%s%3.3d.MOV", gQuick_time_movie_path_stub, gQuicktime_movie_counter);
        gQuicktime_movie_counter++;
        fs_spec.vRefNum = 0;
        fs_spec.parID = 0;
        strcpy(fs_spec.name, path);
        c2pstr(fs_spec.name);
        FlattenMovieData(gQuicktime_movie, flattenAddMovieToDataFork, &fs_spec, sigMoviePlayer, smSystemScript, createMovieFileDeleteCurFile);
        FreeOffQTshite();
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004e1700
// FUNCTION: CARMA2_SW 0x004dda00
// FUNCTION: CARMA2_CHEAT 0x004e72a0
void C2_HOOK_CDECL InitQuickTimeStuff(void) {
    OSErr err;

    gQuickTime_initialized = 0;
    err = InitializeQTML(0);
    if (err == noErr) {
        err = EnterMovies();
        if (err == noErr) {
            gQuickTime_initialized = 1;
        }
    }
    gQuick_time_temp_path[0] = '\0';
    gQuick_time_movie_path_stub[0] = '\0';
    gQuick_time_banner_texture_name[0] = '\0';
}

// FUNCTION: CARMA2_HW 0x004e1740
// FUNCTION: CARMA2_SW 0x004dda40
// FUNCTION: CARMA2_CHEAT 0x004e72e0
void C2_HOOK_FASTCALL SetQuickTimeDefaults(void) {

    if (gQuick_time_quality[0] == '\0') {
        strcpy(gQuick_time_quality, gDefault_quick_time_quality);
    }
    if (gQuick_time_compressor[0] == '\0') {
        strcpy(gQuick_time_compressor, gDefault_quick_time_compressor);
    }
    if (gQuick_time_temp_path[0] == '\0') {
        PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
    }
    if (gQuick_time_movie_path_stub[0] == '\0') {
        PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
    }
    if (gQuick_time_banner_texture_name[0] == '\0') {
        strcpy(gQuick_time_banner_texture_name, "ARBANNER.PIX");
    }
}

// MovieRecordButtonPressed

// FUNCTION: CARMA2_HW 0x004e1a20
// FUNCTION: CARMA2_SW 0x004ddd20
// FUNCTION: CARMA2_CHEAT 0x004e75b0
void C2_HOOK_FASTCALL MovieStopRecordingIfNecessary(void) {

    MovieStopRecordingAndSave();
    KillStatusMessage();
}

// FUNCTION: CARMA2_HW 0x004e1c30
// FUNCTION: CARMA2_SW 0x004ddf30
void C2_HOOK_FASTCALL FUN_004e1c30(void) {
    /* Unused */
}

// FUNCTION: CARMA2_HW 0x004e1c40
// FUNCTION: CARMA2_SW 0x004ddf40
void C2_HOOK_FASTCALL FUN_004e1c40(void) {
    /* Unused */
}

// FUNCTION: CARMA2_HW 0x004e1c50
// FUNCTION: CARMA2_SW 0x004ddf50
void C2_HOOK_FASTCALL FUN_004e1c50(void) {
    /* Unused */
}

// FUNCTION: CARMA2_HW 0x004e1c60
// FUNCTION: CARMA2_SW 0x004ddf60
void C2_HOOK_FASTCALL FUN_004e1c60(undefined4 pArg1, undefined4 pArg2, undefined4 pArg3) {
    /* Unused */
}

// GenerateOneMovieFrame

// MovieRecordFrame

// FUNCTION: CARMA2_HW 0x004e2100
// FUNCTION: CARMA2_SW 0x004de480
// FUNCTION: CARMA2_CHEAT 0x004e7c90
int C2_HOOK_FASTCALL MovieRecording(void) {
    return gNext_replay_record_state;
}

// FUNCTION: CARMA2_HW 0x004e2110
// FUNCTION: CARMA2_SW 0x004de490
// FUNCTION: CARMA2_CHEAT 0x004e7ca0
void C2_HOOK_FASTCALL KillStatusMessage(void) {

    gShow_status_message = 0;
}

// FUNCTION: CARMA2_HW 0x004e2120
// FUNCTION: CARMA2_SW 0x004de4a0
// FUNCTION: CARMA2_CHEAT 0x004e7cb0
void C2_HOOK_FASTCALL ShowStatusMessage(int pMessage1, int pMessage2) {

    gShow_status_message = 1;
    gStatus_message_1 = pMessage1;
    gStatus_message_2 = pMessage2;
}

// FUNCTION: CARMA2_HW 0x004e2140
// FUNCTION: CARMA2_SW 0x004de4c0
// FUNCTION: CARMA2_CHEAT 0x004e7cd0
int C2_HOOK_FASTCALL GetMovieWidth(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->width / 2;
    } else {
        return gCurrent_graf_data->width;
    }
}

// FUNCTION: CARMA2_HW 0x004e2160
// FUNCTION: CARMA2_SW 0x004de4e0
// FUNCTION: CARMA2_CHEAT 0x004e7cf0
int C2_HOOK_FASTCALL GetMovieHeight(void) {

    if (gNbPixelBits == 16) {
        return gCurrent_graf_data->height / 2;
    } else {
        return gCurrent_graf_data->height;
    }
}

// FUNCTION: CARMA2_HW 0x004e2190
// FUNCTION: CARMA2_SW 0x004de510
// FUNCTION: CARMA2_CHEAT 0x004e7d20
void C2_HOOK_FASTCALL TidyMovieARDisplay(void) {
    int old_state;

    old_state = gNext_replay_record_state;
    gNext_replay_record_state = 20;
    RenderAFrame(1);
    RenderAFrame(1);
    gNext_replay_record_state = old_state;
}

// TrapS3SoundOutput

// FreeS3SoundOutput


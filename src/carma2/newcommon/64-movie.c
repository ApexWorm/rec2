/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "64-movie.h"

#include "16-graphics1.h"
#include "18-graphics2.h"
#include "25-grafdata.h"
#include "41-utility.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include <FileTypesAndCreators.h>
#include <FixMath.h>
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
// GLOBAL: CARMA2_SW 0x006597b0
// GLOBAL: CARMA2_CHEAT 0x00666628
tQuick_time_option gDefault_quick_time_quality[6] = {
    { "normal", codecNormalQuality },
    { "min", codecMinQuality },
    { "low", codecLowQuality },
    { "high", codecHighQuality },
    { "max", codecMaxQuality },
    { "lossless", codecLosslessQuality },
};

// GLOBAL: CARMA2_HW 0x0065ed30
// GLOBAL: CARMA2_SW 0x00659690
// GLOBAL: CARMA2_CHEAT 0x00666508
tQuick_time_option gDefault_quick_time_compressor[8] = {
    { "animation", FOUR_CHAR_CODE('r', 'l', 'e', ' ') },
    { "video", FOUR_CHAR_CODE('r', 'p', 'z', 'a') },
    { "jpeg", FOUR_CHAR_CODE('j', 'p', 'e', 'g') },
    { "raw", FOUR_CHAR_CODE('r', 'a', 'w', ' ') },
    { "flic", FOUR_CHAR_CODE('f', 'l', 'i', 'c') },
    { "gif", FOUR_CHAR_CODE('g', 'i', 'f', ' ') },
    { "cinepak", FOUR_CHAR_CODE('c', 'v', 'i', 'd') },
    { "compactvideo", FOUR_CHAR_CODE('c', 'd', 'v', 'c') },
};

// GLOBAL: CARMA2_HW 0x0065ef28
// GLOBAL: CARMA2_SW 0x00659888
// GLOBAL: CARMA2_CHEAT 0x00666700
CodecQ gQuicktime_quality = (CodecQ)-1;

// GLOBAL: CARMA2_HW 0x006a0af0
// GLOBAL: CARMA2_SW 0x0069ab48
// GLOBAL: CARMA2_CHEAT 0x006a8668
short gQuicktime_movie_file;

// GLOBAL: CARMA2_HW 0x006a0af4
// GLOBAL: CARMA2_SW 0x0069ab4c
// GLOBAL: CARMA2_CHEAT 0x006a866c
ImageSequence gQuicktime_image_sequence;

// GLOBAL: CARMA2_HW 0x006a0af8
// GLOBAL: CARMA2_SW 0x0069ab50
// GLOBAL: CARMA2_CHEAT 0x006a8670
long gQuicktime_max_compressed_size;

// GLOBAL: CARMA2_HW 0x006a0afc
// GLOBAL: CARMA2_SW 0x0069ab54
// GLOBAL: CARMA2_CHEAT 0x006a8674
Media gQuicktime_media;

// GLOBAL: CARMA2_HW 0x006a0b08
// GLOBAL: CARMA2_SW 0x0069ab60
// GLOBAL: CARMA2_CHEAT 0x006a8680
FSSpec gQuicktime_fs_spec;

// GLOBAL: CARMA2_HW 0x006a0c10
// GLOBAL: CARMA2_SW 0x0069ac68
// GLOBAL: CARMA2_CHEAT 0x006a8788
Track gQuicktime_track;

// GLOBAL: CARMA2_HW 0x006a0c18
// GLOBAL: CARMA2_SW 0x0069ac70
// GLOBAL: CARMA2_CHEAT 0x006a8790
Rect gQuicktime_rect;

// GLOBAL: CARMA2_HW 0x006a0c28
// GLOBAL: CARMA2_SW 0x0069ac80
// GLOBAL: CARMA2_CHEAT 0x006a87a0
int gQuicktime_paused;

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
PixMapHandle gQuicktime_frame_pixmap_handle;

// GLOBAL: CARMA2_HW 0x006a0c38
// GLOBAL: CARMA2_SW 0x0069ac90
// GLOBAL: CARMA2_CHEAT 0x006a87b0
Ptr gQuicktime_compressed_frame;

// GLOBAL: CARMA2_HW 0x006a0c3c
// GLOBAL: CARMA2_SW 0x0069ac94
// GLOBAL: CARMA2_CHEAT 0x006a87b4
Handle gQuicktime_compressed_frame_handle;

// GLOBAL: CARMA2_HW 0x006a0c40
// GLOBAL: CARMA2_SW 0x0069ac98
// GLOBAL: CARMA2_CHEAT 0x006a87b8
ImageDescriptionHandle gQuicktime_image_description_handle;

// GLOBAL: CARMA2_HW 0x006a0c44
// GLOBAL: CARMA2_SW 0x0069ac9c
// GLOBAL: CARMA2_CHEAT 0x006a87bc
tU16* gQuicktime_555_back_screen;

// GLOBAL: CARMA2_HW 0x006a0c48
// GLOBAL: CARMA2_SW 0x0069aca0
// GLOBAL: CARMA2_CHEAT 0x006a87c0
tU16* gQuicktime_555_lookup;

// GLOBAL: CARMA2_HW 0x006a0c4c
// GLOBAL: CARMA2_SW 0x0069aca4
// GLOBAL: CARMA2_CHEAT 0x006a87c4
CodecType gQuicktime_codec_type;

// GLOBAL: CARMA2_HW 0x0065ef2c
// GLOBAL: CARMA2_SW 0x0065988c
// GLOBAL: CARMA2_CHEAT 0x00666704
int gQuicktime_key_frame = 1;

// GLOBAL: CARMA2_HW 0x0065ef34
// GLOBAL: CARMA2_SW 0x00659894
// GLOBAL: CARMA2_CHEAT 0x0066670c
short gQuicktime_depth = 16;

// GLOBAL: CARMA2_HW 0x006a0c54
// GLOBAL: CARMA2_SW 0x0069acac
// GLOBAL: CARMA2_CHEAT 0x006a87cc
tU32 gQuicktime_frame_time;

inline void C2_HOOK_FASTCALL Create555BELookupTable(void) {
    int i;
    tU16 value;
    br_uint_32 colour;

    gQuicktime_555_lookup = BrMemAllocate(0x10000 * sizeof(tU16), kMem_misc);
    if (gNbPixelBits == 16) {
        for (i = 0; i < 0x10000; i++) {
            value = ((i >> 1) & 0x7fe0) | (i & 0x1f);
            gQuicktime_555_lookup[i] = (tU16)((value >> 8) | (value << 8));
        }
    } else {
        for (i = 0; i < 256; i++) {
            colour = ((br_uint_32*)gRender_palette->pixels)[i];
            value = (((((colour >> 19) & 0x1f) << 5) | ((colour >> 11) & 0x1f)) << 5) | ((colour >> 3) & 0x1f);
            gQuicktime_555_lookup[i] = (tU16)((value >> 8) | (value << 8));
        }
    }
}

// FUNCTION: CARMA2_HW 0x004e1400
// FUNCTION: CARMA2_SW 0x004dd720
// FUNCTION: CARMA2_CHEAT 0x004e6fa0
void C2_HOOK_FASTCALL Convert565BrPixelmapTo555BigEndianPixmap(PixMapHandle pDest, br_pixelmap* pSrc) {
    int width;
    int height;
    int step;
    int first_line;
    int x;
    int y;

    if (gQuicktime_555_lookup == NULL) {
        Create555BELookupTable();
    }
#ifdef REC2_HW
    PossibleLock(1);
#endif
    width = GetMovieWidth();
    height = GetMovieHeight();
    step = 2;
    if (gNbPixelBits == 16) {
        if (pSrc->height == 200) {
            height = 200;
            first_line = 20;
            step = 1;
        } else {
            first_line = 0;
        }
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                gQuicktime_555_back_screen[(first_line + y) * width + x] = gQuicktime_555_lookup[((tU16*)pSrc->pixels)[step * y * pSrc->row_bytes / 2 + x * step]];
            }
        }
    } else {
        for (y = 0; y < 200; y++) {
            for (x = 0; x < width; x++) {
                gQuicktime_555_back_screen[y * width + x] = gQuicktime_555_lookup[((tU8*)pSrc->pixels)[y * pSrc->row_bytes + x]];
            }
        }
    }
#ifdef REC2_HW
    PossibleUnlock(1);
#endif
}

// SwapPixelmapBytePairs

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004e1250
// FUNCTION: CARMA2_SW 0x004dd570
// FUNCTION: CARMA2_CHEAT 0x004e6e10
int C2_HOOK_FASTCALL WriteBannerFrame(void) {
    br_pixelmap* banner;
    int i;
    long data_size;
    unsigned char similarity;
    OSErr err;

    if (gQuick_time_banner_number != 0) {
        banner = LoadPixelmap(gQuick_time_banner_texture_name);
        if (banner != NULL) {
            if (banner->width != 320 || banner->height != 200) {
                BrFailure("Banner picture must be 320x200");
            }
            (*gQuicktime_frame_pixmap_handle)->baseAddr = (Ptr)gQuicktime_555_back_screen;
            (*gQuicktime_frame_pixmap_handle)->rowBytes = GetMovieWidth() * 2;
            memset(gQuicktime_555_back_screen, 0, GetMovieWidth() * GetMovieHeight() * 2);
            dr_dprintf("Converting banner. w=%d, h=%d, type=%d", banner->width, banner->height, banner->type);
            Convert565BrPixelmapTo555BigEndianPixmap(gQuicktime_frame_pixmap_handle, banner);
            for (i = 0; i < 18; i++) {
                err = CompressSequenceFrame(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle, &gQuicktime_rect,
                    codecFlagUpdatePreviousComp | codecFlagForceKeyFrame, *gQuicktime_compressed_frame_handle, &data_size, &similarity, NULL);
                if (err != noErr) {
                    return err;
                }
                (*gQuicktime_image_description_handle)->dataSize = data_size;
                err = AddMediaSample(gQuicktime_media, gQuicktime_compressed_frame_handle, 0, (*gQuicktime_image_description_handle)->dataSize, 55,
                    (SampleDescriptionHandle)gQuicktime_image_description_handle, 1, 0, NULL);
                if (err != noErr) {
                    return err;
                }
            }
        }
    }
    gQuicktime_key_frame = 1;
    return noErr;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline tU16* C2_HOOK_FASTCALL Allocate555BackScreen(void) {

    if (gQuicktime_555_back_screen == NULL) {
        gQuicktime_555_back_screen = BrMemAllocate(GetMovieWidth() * GetMovieHeight() * sizeof(tU16), kMem_misc);
    }
    return gQuicktime_555_back_screen;
}

inline int C2_HOOK_FASTCALL GetCompressorQualityFromOptions(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gDefault_quick_time_quality); i++) {
        if (strcmp(gDefault_quick_time_quality[i].name, gQuick_time_quality) == 0) {
            gQuicktime_quality = gDefault_quick_time_quality[i].value;
            return 1;
        }
    }
    return 0;
}

inline int C2_HOOK_FASTCALL GetCompressorTypeFromOptions(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gDefault_quick_time_compressor); i++) {
        if (strcmp(gDefault_quick_time_compressor[i].name, gQuick_time_compressor) == 0) {
            gQuicktime_codec_type = gDefault_quick_time_compressor[i].value;
            return 1;
        }
    }
    return 0;
}

// InitMovie

// FUNCTION: CARMA2_HW 0x004e0fe0
// FUNCTION: CARMA2_SW 0x004dd300
// FUNCTION: CARMA2_CHEAT 0x004e6ba0
OSErr C2_HOOK_FASTCALL CreateMovie(void) {
    OSErr err;

    gQuicktime_track = NewMovieTrack(gQuicktime_movie, FixRatio(GetMovieWidth() * 2, 1), FixRatio(GetMovieHeight() * 2, 1), kFullVolume);
    err = GetMoviesError();
    if (err != noErr) {
        return err;
    }
    gQuicktime_media = NewTrackMedia(gQuicktime_track, VideoMediaType, 1000, NULL, 0);
    err = GetMoviesError();
    if (err != noErr) {
        return err;
    }
    err = BeginMediaEdits(gQuicktime_media);
    if (err != noErr) {
        return err;
    }
    gQuicktime_frame_pixmap_handle = (PixMapHandle)NewHandle(sizeof(PixMap));
    err = MemError();
    if (err != noErr) {
        return err;
    }
    memset(*gQuicktime_frame_pixmap_handle, 0, sizeof(PixMap));
    (*gQuicktime_frame_pixmap_handle)->pixelType = 16;
    (*gQuicktime_frame_pixmap_handle)->pixelSize = 16;
    (*gQuicktime_frame_pixmap_handle)->cmpCount = 3;
    (*gQuicktime_frame_pixmap_handle)->cmpSize = 5;
    err = GetMaxCompressionSize(gQuicktime_frame_pixmap_handle, &gQuicktime_rect, 0, codecNormalQuality, gQuicktime_codec_type, bestFidelityCodec, &gQuicktime_max_compressed_size);
    if (err != noErr) {
        return err;
    }
    gQuicktime_compressed_frame_handle = NewHandle(gQuicktime_max_compressed_size);
    err = MemError();
    if (err != noErr) {
        return err;
    }
    MoveHHi(gQuicktime_compressed_frame_handle);
    HLock(gQuicktime_compressed_frame_handle);
    gQuicktime_compressed_frame = *gQuicktime_compressed_frame_handle;
    gQuicktime_image_description_handle = (ImageDescriptionHandle)NewHandle(4);
    err = MemError();
    if (err != noErr) {
        return err;
    }
    if (Allocate555BackScreen() == NULL) {
        return 1;
    }
    err = CompressSequenceBegin(&gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle, NULL, &gQuicktime_rect, &gQuicktime_rect,
        gQuicktime_depth, gQuicktime_codec_type, bestFidelityCodec, codecNormalQuality, codecNormalQuality, 15, NULL,
        codecFlagUpdatePreviousComp | codecFlagForceKeyFrame, gQuicktime_image_description_handle);
    if (err != noErr) {
        return err;
    }
    return WriteBannerFrame();
}

inline OSErr C2_HOOK_FASTCALL MovieStartNewMovie(void) {

    gNext_replay_record_state = 1;
    ShowStatusMessage(278, eMiscString_please_wait);
    gQuicktime_movie_file = 0;
    gQuicktime_paused = 0;
    if (gQuicktime_quality == (CodecQ)-1 && !GetCompressorQualityFromOptions()) {
        gQuicktime_quality = codecLowQuality;
    }
    if (gQuicktime_codec_type == 0 && !GetCompressorTypeFromOptions()) {
        gQuicktime_codec_type = FOUR_CHAR_CODE('r', 'l', 'e', ' ');
    }
    gQuicktime_fs_spec.vRefNum = 0;
    gQuicktime_fs_spec.parID = 0;
    strcpy(gQuicktime_fs_spec.name, gQuick_time_temp_path);
    c2pstr(gQuicktime_fs_spec.name);
    return noErr;
}

// FUNCTION: CARMA2_HW 0x004e1640
// FUNCTION: CARMA2_SW 0x004dd940
// FUNCTION: CARMA2_CHEAT 0x004e71e0
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
        DisposeHandle((Handle)gQuicktime_frame_pixmap_handle);
        gQuicktime_frame_pixmap_handle = NULL;
    }
    if (gQuicktime_image_description_handle != NULL) {
        DisposeHandle((Handle)gQuicktime_image_description_handle);
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
        strcpy(gQuick_time_quality, gDefault_quick_time_quality[0].name);
    }
    if (gQuick_time_compressor[0] == '\0') {
        strcpy(gQuick_time_compressor, gDefault_quick_time_compressor[0].name);
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

// FUNCTION: CARMA2_HW 0x004e1810
// FUNCTION: CARMA2_SW 0x004ddb10
// FUNCTION: CARMA2_CHEAT 0x004e73b0
void C2_HOOK_FASTCALL MovieRecordButtonPressed(void) {

    if (gQuickTime_initialized) {
        if (gNext_replay_record_state == 4) {
            if (gQuicktime_paused == 0) {
                gQuicktime_paused = 1;
            } else {
                gQuicktime_paused = 0;
            }
            if (gQuicktime_paused) {
                DRS3StartSound(gEffects_outlet, 9003);
                DRS3StartSound(gEffects_outlet, 5500);
                ShowStatusMessage(281, 0);
            } else {
                DRS3StartSound(gEffects_outlet, 9011);
                ShowStatusMessage(eMiscString_recording_movie, 0);
            }
        } else if (gNext_replay_record_state == 0) {
            if (MovieStartNewMovie() != noErr) {
                gNext_replay_record_state = 10;
                ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
            }
        }
    }
}

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

// FUNCTION: CARMA2_HW 0x004e1c70
// FUNCTION: CARMA2_SW 0x004ddf70
// FUNCTION: CARMA2_CHEAT 0x004e7800
OSErr C2_HOOK_FASTCALL GenerateOneMovieFrame(br_pixelmap* pScreen) {
    CodecFlags flags;
    long data_size;
    unsigned char similarity;
    OSErr err;

    if (gQuicktime_key_frame) {
        flags = codecFlagUpdatePreviousComp | codecFlagForceKeyFrame;
        gQuicktime_key_frame = 0;
    } else {
        flags = codecFlagUpdatePreviousComp;
    }
#ifdef REC2_HW
    PossibleLock(1);
#endif
    (*gQuicktime_frame_pixmap_handle)->baseAddr = (Ptr)gQuicktime_555_back_screen;
    (*gQuicktime_frame_pixmap_handle)->rowBytes = GetMovieWidth() * 2;
    Convert565BrPixelmapTo555BigEndianPixmap(gQuicktime_frame_pixmap_handle, pScreen);
    err = CompressSequenceFrame(gQuicktime_image_sequence, gQuicktime_frame_pixmap_handle, &gQuicktime_rect,
        flags, *gQuicktime_compressed_frame_handle, &data_size, &similarity, NULL);
    if (err != noErr) {
#ifdef REC2_HW
        PossibleUnlock(1);
#endif
        return err;
    }
    (*gQuicktime_image_description_handle)->dataSize = data_size;
    err = AddMediaSample(gQuicktime_media, gQuicktime_compressed_frame_handle, 0, (*gQuicktime_image_description_handle)->dataSize, 55,
        (SampleDescriptionHandle)gQuicktime_image_description_handle, 1, 0, NULL);
    if (err != noErr) {
#ifdef REC2_HW
        PossibleUnlock(1);
#endif
        return err;
    }
#ifdef REC2_HW
    PossibleUnlock(1);
#endif
    return noErr;
}

// FUNCTION: CARMA2_HW 0x004e1da0
// FUNCTION: CARMA2_SW 0x004de050
// FUNCTION: CARMA2_CHEAT 0x004e7930
int C2_HOOK_FASTCALL MovieRecordFrame(br_pixelmap* pScreen, int pFrame_period) {
    tU32 time;

    switch (gNext_replay_record_state) {
    case 1:
        gNext_replay_record_state = 2;
        break;
    case 2:
        WaitFor(2000);
        gQuicktime_rect.top = 0;
        gQuicktime_rect.left = 0;
        gQuicktime_rect.right = GetMovieWidth();
        gQuicktime_rect.bottom = GetMovieHeight();
        if (CreateMovieFile(&gQuicktime_fs_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie) != noErr) {
            PathCat(gQuick_time_temp_path, gApplication_path, "QTTMP");
            PathCat(gQuick_time_movie_path_stub, gApplication_path, "MOVIE");
            if (CreateMovieFile(&gQuicktime_fs_spec, sigMoviePlayer, smCurrentScript, createMovieFileDeleteCurFile, &gQuicktime_movie_file, &gQuicktime_movie) != noErr) {
                gNext_replay_record_state = 10;
                ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
                break;
            }
        }
        gNext_replay_record_state = 3;
        ShowStatusMessage(eMiscString_initialising_movie, eMiscString_please_wait);
        break;
    case 3:
        if (CreateMovie() != noErr) {
            gNext_replay_record_state = 10;
            ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
        } else {
            gNext_replay_record_state = 4;
            ShowStatusMessage(eMiscString_recording_movie, 0);
        }
        break;
    case 4:
        if (!gQuicktime_paused) {
            time = pFrame_period + gQuicktime_frame_time;
            while (time >= 55) {
                if (GenerateOneMovieFrame(pScreen) != noErr) {
                    gNext_replay_record_state = 10;
                    ShowStatusMessage(eMiscString_movie_error, eMiscString_aborting);
                    return 0;
                }
                time -= 55;
            }
            gQuicktime_frame_time = time;
        }
        break;
    case 10:
        WaitFor(2000);
        gNext_replay_record_state = 0;
        FreeOffQTshite();
        break;
    case 5:
        gRecording_mouse_disabled = 0;
        break;
    }
    return 0;
}

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


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef __MOVIES__
#define __MOVIES__

#include "ConditionalMacros.h"
#include "Files.h"
#include "MacTypes.h"

#include "ImageCompression.h"

typedef struct MediaType**              Media;
typedef struct MovieType**              Movie;
typedef struct TrackType**              Track;
typedef struct SampleDescription**      SampleDescriptionHandle;

enum {
    VideoMediaType                = FOUR_CHAR_CODE('v', 'i', 'd', 'e'),
};

enum {
    kFullVolume                   = 0x0100,
};

enum {
    flattenAddMovieToDataFork = 1L << 0,
};
enum {
    createMovieFileDeleteCurFile  = 1L << 31,
};

extern OSErr C2_HOOK_CDECL EnterMovies(void);

extern OSErr C2_HOOK_CDECL GetMoviesError(void);

extern Track C2_HOOK_CDECL NewMovieTrack(Movie theMovie, Fixed width, Fixed height, short trackVolume);

extern Media C2_HOOK_CDECL NewTrackMedia(Track theTrack, OSType mediaType, TimeScale timeScale, Handle dataRef, OSType dataRefType);

extern OSErr C2_HOOK_CDECL BeginMediaEdits(Media theMedia);

extern OSErr C2_HOOK_CDECL AddMediaSample(
    Media                   theMedia,
    Handle                  dataIn,
    long                    inOffset,
    unsigned long           size,
    TimeValue               durationPerSample,
    SampleDescriptionHandle sampleDescriptionH,
    long                    numberOfSamples,
    short                   sampleFlags,
    TimeValue*              sampleTime);

extern OSErr C2_HOOK_CDECL CloseMovieFile(short resRefNum);

extern void C2_HOOK_CDECL DisposeMovie(Movie theMovie);

extern OSErr C2_HOOK_CDECL EndMediaEdits(Media theMedia);

extern TimeValue C2_HOOK_CDECL GetMediaDuration(Media theMedia);

extern OSErr C2_HOOK_CDECL InsertMediaIntoTrack(
    Track       theTrack,
    TimeValue   trackStart,
    TimeValue   mediaTime,
    TimeValue   mediaDuration,
    Fixed       mediaRate);


extern Movie C2_HOOK_CDECL FlattenMovieData(
    Movie           theMovie,
    long            movieFlattenFlags,
    const FSSpec *  theFile,
    OSType          creator,
    ScriptCode      scriptTag,
    long            createMovieFileFlags);

extern OSErr C2_HOOK_CDECL CreateMovieFile(
    const FSSpec *  fileSpec,
    OSType          creator,
    ScriptCode      scriptTag,
    long            createMovieFileFlags,
    short *         resRefNum,
    Movie *         newmovie);

#endif /* __MOVIES__ */

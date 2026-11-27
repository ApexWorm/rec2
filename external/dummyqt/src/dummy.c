/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "FixMath.h"
#include "ImageCompression.h"
#include "MacMemory.h"
#include "Movies.h"
#include "QTML.h"

#include <stdlib.h>

// STUB: CARMA2_HW 0x00575950
// STUB: CARMA2_SW 0x00570ff0
// STUB: CARMA2_CHEAT 0x0057b540
OSErr C2_HOOK_CDECL InitializeQTML(long flag) {
    (void)flag;
    return (OSErr)0xf7d3;
}

// STUB: CARMA2_SW 0x005709e0
// STUB: CARMA2_HW 0x00575340
// STUB: CARMA2_CHEAT 0x0057af30
StringPtr C2_HOOK_CDECL c2pstr(char *aStr) {
    (void)aStr;
    return NULL;
}

// STUB: CARMA2_HW 0x00570ac0
// STUB: CARMA2_SW 0x0056c160
// STUB: CARMA2_CHEAT 0x005766b0
void C2_HOOK_CDECL DisposeHandle(Handle h) {
    (void)h;
}

// STUB: CARMA2_HW 0x0056a8e0
// STUB: CARMA2_SW 0x00565f80
// STUB: CARMA2_CHEAT 0x005704d0
OSErr C2_HOOK_CDECL EnterMovies(void) {
    return 0x1;
}

// STUB: CARMA2_HW 0x0056baa0
// STUB: CARMA2_SW 0x00567140
// STUB: CARMA2_CHEAT 0x00571690
OSErr C2_HOOK_CDECL CloseMovieFile(short resRefNum) {
    (void)resRefNum;
    return 0x1;
}

// STUB: CARMA2_HW 0x0056abe0
// STUB: CARMA2_SW 0x00566280
// STUB: CARMA2_CHEAT 0x005707d0
void C2_HOOK_CDECL DisposeMovie(Movie theMovie) {
    (void)theMovie;
}

// STUB: CARMA2_HW 0x0056b360
// STUB: CARMA2_SW 0x00566a00
// STUB: CARMA2_CHEAT 0x00570f50
OSErr C2_HOOK_CDECL EndMediaEdits(Media theMedia) {
    (void)theMedia;
    return 1;
}

// STUB: CARMA2_HW 0x0056b240
// STUB: CARMA2_SW 0x005668e0
// STUB: CARMA2_CHEAT 0x00570e30
TimeValue C2_HOOK_CDECL GetMediaDuration(Media theMedia) {
    (void)theMedia;
    return 0;
}

// STUB: CARMA2_HW 0x0056dac0
// STUB: CARMA2_SW 0x00569160
// STUB: CARMA2_CHEAT 0x005736b0
OSErr C2_HOOK_CDECL CDSequenceEnd(ImageSequence seqID) {
    (void)seqID;
    return 1;
}

// STUB: CARMA2_HW 0x0056c600
// STUB: CARMA2_SW 0x00567ca0
// STUB: CARMA2_CHEAT 0x005721f0
OSErr C2_HOOK_CDECL InsertMediaIntoTrack(Track theTrack, TimeValue trackStart, TimeValue mediaTime, TimeValue mediaDuration, Fixed mediaRate) {
    (void)theTrack;
    (void)trackStart;
    (void)mediaTime;
    (void)mediaDuration;
    (void)mediaRate;
    return 1;
}

// STUB: CARMA2_HW 0x0056c800
// STUB: CARMA2_SW 0x00567ea0
// STUB: CARMA2_CHEAT 0x005723f0
Movie C2_HOOK_CDECL FlattenMovieData(Movie theMovie, long movieFlattenFlags, const FSSpec *theFile, OSType creator, ScriptCode scriptTag, long createMovieFileFlags) {
    (void)theMovie;
    (void)movieFlattenFlags;
    (void)theFile;
    (void)creator;
    (void)scriptTag;
    (void)createMovieFileFlags;
    return NULL;
}

// STUB: CARMA2_HW 0x0056c720
// STUB: CARMA2_SW 0x00567dc0
// STUB: CARMA2_CHEAT 0x00572310
OSErr C2_HOOK_CDECL CreateMovieFile(const FSSpec* fileSpec, OSType creator, ScriptCode scriptTag, long createMovieFileFlags, short* resRefNum, Movie* newmovie) {
    return 1;
}

// STUB: CARMA2_HW 0x0056fde0
// STUB: CARMA2_SW 0x0056b480
// STUB: CARMA2_CHEAT 0x005759d0
Fixed C2_HOOK_CDECL FixRatio(short numer, short denom) {
    (void)numer;
    (void)denom;
    return 0;
}

// STUB: CARMA2_HW 0x0056c640
// STUB: CARMA2_SW 0x00567ce0
// STUB: CARMA2_CHEAT 0x00572230
Track C2_HOOK_CDECL NewMovieTrack(Movie theMovie, Fixed width, Fixed height, short trackVolume) {
    (void)theMovie;
    (void)width;
    (void)height;
    (void)trackVolume;
    return NULL;
}

// STUB: CARMA2_HW 0x0056a920
// STUB: CARMA2_SW 0x00565fc0
// STUB: CARMA2_CHEAT 0x00570510
OSErr C2_HOOK_CDECL GetMoviesError(void) {
    return 1;
}

// STUB: CARMA2_HW 0x0056c6e0
// STUB: CARMA2_SW 0x00567d80
// STUB: CARMA2_CHEAT 0x005722d0
Media C2_HOOK_CDECL NewTrackMedia(Track theTrack, OSType mediaType, TimeScale timeScale, Handle dataRef, OSType dataRefType) {
    (void)theTrack;
    (void)mediaType;
    (void)timeScale;
    (void)dataRef;
    (void)dataRefType;
    return NULL;
}

// STUB: CARMA2_HW 0x0056b340
// STUB: CARMA2_SW 0x005669e0
// STUB: CARMA2_CHEAT 0x00570f30
OSErr C2_HOOK_CDECL BeginMediaEdits(Media theMedia) {
    (void)theMedia;
    return 1;
}

// STUB: CARMA2_HW 0x00570600
// STUB: CARMA2_SW 0x0056bca0
// STUB: CARMA2_CHEAT 0x005761f0
Handle C2_HOOK_CDECL NewHandle(Size byteCount) {
    (void)byteCount;
    return NULL;
}

// STUB: CARMA2_HW 0x005705c0
// STUB: CARMA2_SW 0x0056bc60
// STUB: CARMA2_CHEAT 0x005761b0
OSErr C2_HOOK_CDECL MemError(void) {
    return 1;
}

// STUB: CARMA2_HW 0x0056d7e0
// STUB: CARMA2_SW 0x00568e80
// STUB: CARMA2_CHEAT 0x005733d0
OSErr C2_HOOK_CDECL GetMaxCompressionSize(PixMapHandle src, const Rect* srcRect, short colorDepth, CodecQ quality, CodecType cType, CompressorComponent codec, long* size) {
    (void)src;
    (void)srcRect;
    (void)colorDepth;
    (void)quality;
    (void)cType;
    (void)codec;
    (void)size;
    return 1;
}

// STUB: CARMA2_HW 0x00570a40
// STUB: CARMA2_SW 0x0056c0e0
// STUB: CARMA2_CHEAT 0x00576630
void C2_HOOK_CDECL MoveHHi(Handle h) {
    (void)h;
}

// STUB: CARMA2_HW 0x005707e0
// STUB: CARMA2_SW 0x0056be80
// STUB: CARMA2_CHEAT 0x005763d0
void C2_HOOK_CDECL HLock(Handle h) {
    (void)h;
}

// STUB: CARMA2_HW 0x0056d8a0
// STUB: CARMA2_SW 0x00568f40
// STUB: CARMA2_CHEAT 0x00573490
OSErr C2_HOOK_CDECL CompressSequenceBegin(ImageSequence* seqID, PixMapHandle src, PixMapHandle prev, const Rect* srcRect, const Rect* prevRect, short colorDepth, CodecType cType, CompressorComponent codec, CodecQ spatialQuality, CodecQ temporalQuality, long keyFrameRate, CTabHandle ctable, CodecFlags flags, ImageDescriptionHandle desc) {
    (void)seqID;
    (void)src;
    (void)prev;
    (void)srcRect;
    (void)prevRect;
    (void)colorDepth;
    (void)cType;
    (void)codec;
    (void)spatialQuality;
    (void)temporalQuality;
    (void)keyFrameRate;
    (void)ctable;
    (void)flags;
    (void)desc;
    return 1;
}

// STUB: CARMA2_HW 0x0056d8c0
// STUB: CARMA2_SW 0x00568f60
// STUB: CARMA2_CHEAT 0x005734b0
OSErr C2_HOOK_CDECL CompressSequenceFrame(ImageSequence seqID, PixMapHandle src, const Rect* srcRect, CodecFlags flags, Ptr data, long* dataSize, unsigned char* similarity, ICMCompletionProcRecordPtr asyncCompletionProc) {
    (void)seqID;
    (void)src;
    (void)srcRect;
    (void)flags;
    (void)data;
    (void)dataSize;
    (void)similarity;
    (void)asyncCompletionProc;
    return 1;
}

// STUB: CARMA2_HW 0x0056b440
// STUB: CARMA2_SW 0x00566ae0
// STUB: CARMA2_CHEAT 0x00571030
OSErr C2_HOOK_CDECL AddMediaSample(Media theMedia, Handle dataIn, long inOffset, unsigned long size, TimeValue durationPerSample, SampleDescriptionHandle sampleDescriptionH, long numberOfSamples, short sampleFlags, TimeValue* sampleTime) {
    (void)theMedia;
    (void)dataIn;
    (void)inOffset;
    (void)size;
    (void)durationPerSample;
    (void)sampleDescriptionH;
    (void)numberOfSamples;
    (void)sampleFlags;
    (void)sampleTime;
    return 1;
}

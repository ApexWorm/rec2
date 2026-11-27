/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef __IMAGECOOMPRESSION__
#define __IMAGECOOMPRESSION__

#include "MacTypes.h"

typedef long                            ImageSequence;
typedef OSType                          CodecType;
typedef unsigned short                  CodecFlags;
typedef unsigned long                   CodecQ;
typedef long                            CompressorComponent;
typedef struct ColorTable**             CTabHandle;
typedef void*                           ICMCompletionProcRecordPtr;

enum {
    codecFlagUpdatePreviousComp         = 1L << 6,
    codecFlagForceKeyFrame              = 1L << 7,
};

enum {
    codecMinQuality                     = 0x00000000,
    codecLowQuality                     = 0x00000100,
    codecNormalQuality                  = 0x00000200,
    codecHighQuality                    = 0x00000300,
    codecMaxQuality                     = 0x000003ff,
    codecLosslessQuality                = 0x00000400,
};

enum {
    bestFidelityCodec                   = -2,
};

#pragma pack(push, 2)
typedef struct PixMap {
    Ptr                 baseAddr;
    short               rowBytes;
    Rect                bounds;
    short               pmVersion;
    short               packType;
    long                packSize;
    Fixed               hRes;
    Fixed               vRes;
    short               pixelType;
    short               pixelSize;
    short               cmpCount;
    short               cmpSize;
    long                planeBytes;
    CTabHandle          pmTable;
    long                pmReserved;
} PixMap;
typedef PixMap*                         PixMapPtr;
typedef PixMapPtr*                      PixMapHandle;

typedef struct ImageDescription {
    long                idSize;
    CodecType           cType;
    long                resvd1;
    short               resvd2;
    short               dataRefIndex;
    short               version;
    short               revisionLevel;
    long                vendor;
    CodecQ              temporalQuality;
    CodecQ              spatialQuality;
    short               width;
    short               height;
    Fixed               hRes;
    Fixed               vRes;
    long                dataSize;
    short               frameCount;
    Str31               name;
    short               depth;
    short               clutID;
} ImageDescription;
#pragma pack(pop)
typedef ImageDescription*               ImageDescriptionPtr;
typedef ImageDescriptionPtr*            ImageDescriptionHandle;

extern OSErr C2_HOOK_CDECL CDSequenceEnd(ImageSequence seqID);

extern OSErr C2_HOOK_CDECL GetMaxCompressionSize(
    PixMapHandle        src,
    const Rect*         srcRect,
    short               colorDepth,
    CodecQ              quality,
    CodecType           cType,
    CompressorComponent codec,
    long*               size);

extern OSErr C2_HOOK_CDECL CompressSequenceBegin(
    ImageSequence*          seqID,
    PixMapHandle            src,
    PixMapHandle            prev,
    const Rect*             srcRect,
    const Rect*             prevRect,
    short                   colorDepth,
    CodecType               cType,
    CompressorComponent     codec,
    CodecQ                  spatialQuality,
    CodecQ                  temporalQuality,
    long                    keyFrameRate,
    CTabHandle              ctable,
    CodecFlags              flags,
    ImageDescriptionHandle  desc);

extern OSErr C2_HOOK_CDECL CompressSequenceFrame(
    ImageSequence               seqID,
    PixMapHandle                src,
    const Rect*                 srcRect,
    CodecFlags                  flags,
    Ptr                         data,
    long*                       dataSize,
    unsigned char*              similarity,
    ICMCompletionProcRecordPtr  asyncCompletionProc);

#endif /* __IMAGECOOMPRESSION__ */

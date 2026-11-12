#include "18b-graphics2.h"

#include "16-graphics1.h"
#include "18-graphics2.h"
#include "25-grafdata.h"
#include "33-depth.h"
#include "41-utility.h"
#include "63-loading3.h"
#include "globvars.h"
#include "platform.h"

// FUNCTION: CARMA2_HW 0x0047b450
void C2_HOOK_FASTCALL DRDrawLine(br_pixelmap* pDestn, int pX1, int pY1, int pX2, int pY2, int pColour) {

    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
    BrPixelmapLine(pDestn, pX1, pY1, pX2, pY2, pColour);
}

// FUNCTION: CARMA2_HW 0x0047b880
// FUNCTION: CARMA2_SW 0x0047aba0
void C2_HOOK_FASTCALL ClearEntireScreen(void) {

    if (gScreen != NULL) {
        BrPixelmapFill(gScreen, gGraf_specs[gGraf_spec_index].black_value);
    }
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
    BrPixelmapFill(gDepth_buffer, 0xffffffff);
    PDScreenBufferSwap(0);
#ifdef REC2_HW
    BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
#endif
}

// DistanceFromPlane

// FUNCTION: CARMA2_HW 0x0047b960
// FUNCTION: CARMA2_SW 0x0047ac50
void C2_HOOK_FASTCALL KillSplashScreen(void) {

    if (gCurrent_splash != NULL) {
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0047b990
// FUNCTION: CARMA2_SW 0x0047ac80
void C2_HOOK_FASTCALL SplashScreenWith(const char* pPixmap_name) {
    br_pixelmap* map;
    tPixelFlags flags;

    map = BrMapFind(pPixmap_name);
    if (gCurrent_splash != NULL && map == gCurrent_splash) {
        return;
    }
    if (gCurrent_splash != NULL) {
        KillSplashScreen();
    }
    flags = gPixelFlags;
#ifdef REC2_SW
    gPixelFlags |= 0x8;
#endif
    gCurrent_splash = map;
    if (map == NULL) {
        gCurrent_splash = LoadPixelmap(pPixmap_name);
        BRPM_convert(gCurrent_splash, gBack_screen->type);
        if (gCurrent_splash != NULL) {
            BrMapAdd(gCurrent_splash);
        }
    }
    gPixelFlags = flags;
#ifdef REC2_SW
    if (gCurrent_splash == NULL || gCurrent_splash->map == NULL) {
        EnsureRenderPalette();
    } else {
        if (gScreen != NULL) {
            BrPixelmapFill(gScreen, gGraf_specs[gGraf_spec_index].black_value);
        }
        BrPixelmapFill(gBack_screen, gGraf_specs[gGraf_spec_index].black_value);
        BrPixelmapFill(gDepth_buffer, 0xffffffff);
        PDScreenBufferSwap(0);
        FadePaletteUp();
        PDSetPalette(gCurrent_splash->map);
    }
#endif
    if (gCurrent_splash != NULL) {
        PixelmapSwapByteOrder(gCurrent_splash);
        DrPixelmapRectangleCopyPossibleLock(gBack_screen, 0, 0, gCurrent_splash, 0, 0,
            (br_int_16)gCurrent_splash->width, (br_int_16)gCurrent_splash->height);
        PDScreenBufferSwap(0);
        BrMapRemove(gCurrent_splash);
        BrPixelmapFree(gCurrent_splash);
        gCurrent_splash = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x0047ba80
// FUNCTION: CARMA2_SW 0x0047ae00
void C2_HOOK_FASTCALL DRPixelmapRectangleMaskedCopy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, const br_pixelmap* pSource, br_int_16 pSource_x, br_int_16 pSource_y, br_int_16 pWidth, br_int_16 pHeight) {
#ifdef REC2_SW
    int y_count;
    int x_count;
    int dest_row_wrap;
    int source_row_wrap;
    int x_delta;
    tU8 the_byte;
    tU8* source_ptr;
    tU8* dest_ptr;

    source_ptr = (tU8*)pSource->pixels + pSource->row_bytes * pSource_y + pSource_x;
    dest_ptr = (tU8*)pDest->pixels + pDest->row_bytes * pDest_y + pDest_x;
    source_row_wrap = pSource->row_bytes - pWidth;
    dest_row_wrap = pDest->row_bytes - pWidth;

    if (pDest_y < 0) {
        pHeight += pDest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr -= pDest_y * pSource->row_bytes;
        dest_ptr -= pDest_y * pDest->row_bytes;
        pDest_y = 0;
    }
    if (pDest_y >= pDest->height) {
        return;
    }
    if (pDest_y + pHeight > pDest->height) {
        pHeight = pDest->height - pDest_y;
    }
    if (pDest_x < 0) {
        pWidth += pDest_x;
        if (pWidth <= 0) {
            return;
        }
        source_ptr -= pDest_x;
        dest_ptr -= pDest_x;
        source_row_wrap -= pDest_x;
        dest_row_wrap -= pDest_x;
        pDest_x = 0;
    }
    if (pDest_x >= pDest->width) {
        return;
    }
    if (pDest_x + pWidth > pDest->width) {
        x_delta = pDest_x + pWidth - pDest->width;
        pWidth -= x_delta;
        source_row_wrap += x_delta;
        dest_row_wrap += x_delta;
    }
    for (y_count = 0; y_count < pHeight; y_count++) {
        for (x_count = 0; x_count < pWidth; x_count++) {
            the_byte = *source_ptr;
            if (the_byte != 0) {
                *dest_ptr = the_byte;
            }
            source_ptr++;
            dest_ptr++;
        }
        source_ptr += source_row_wrap;
        dest_ptr += dest_row_wrap;
    }
#else
    int y;
    int x;
    int dest_row_wrap;
    int source_row_wrap;
    tU16 the_byte;
    tU16* source_ptr;
    tU16* dest_ptr;
    int dest_x;
    int dest_y;

    source_ptr = (tU16*)pSource->pixels + pSource->row_bytes * pSource_y / sizeof(tU16) + pSource_x;
    dest_x = pDest_x;
    dest_y = pDest_y;
    dest_ptr = (tU16*)pDest->pixels + dest_y * pDest->row_bytes / sizeof(tU16) + pDest->base_x / sizeof(tU16) + dest_x;
    source_row_wrap = pSource->row_bytes / sizeof(tU16) - pWidth;
    dest_row_wrap = pDest->row_bytes / sizeof(tU16) - pWidth;

    if (dest_y < 0) {
        pHeight += dest_y;
        if (pHeight <= 0) {
            return;
        }
        source_ptr -= dest_y * pSource->row_bytes / sizeof(tU16);
        dest_ptr -= dest_y * pDest->row_bytes / sizeof(tU16);
        dest_y = 0;
    }
    if (dest_y < pDest->height) {
        if (pDest->height < dest_y + pHeight) {
            pHeight = pDest->height - dest_y;
        }
        if (dest_x < 0) {
            pWidth += dest_x;
            if (pWidth <= 0) {
                return;
            }
            dest_x = 0;
            source_ptr -= dest_x;
            dest_ptr -= dest_x;
            source_row_wrap -= dest_x;
            dest_row_wrap -= dest_x;
        }
        if (dest_x < pDest->width) {
            if (dest_x + pWidth > pDest->width) {
                pWidth -= pWidth + dest_x - pDest->width;
                source_row_wrap += pWidth + dest_x - pDest->width;
                dest_row_wrap += pWidth + dest_x - pDest->width;
            }
            for (y = 0; y < pHeight; y++) {
                for (x = 0; x < pWidth; x++) {
                    the_byte = *source_ptr;
                    if (the_byte != 0) {
                        *dest_ptr = the_byte;
                    }
                    source_ptr++;
                    dest_ptr++;
                }
                source_ptr += source_row_wrap;
                dest_ptr += dest_row_wrap;
            }
        }
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0047bc40
// FUNCTION: CARMA2_SW 0x0047af60
void C2_HOOK_FASTCALL DRMaskedStamp(br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource) {

    DRPixelmapRectangleMaskedCopy(gBack_screen, pDest_x, pDest_y, pSource, 0, 0, pSource->width, pSource->height);
}

// FUNCTION: CARMA2_HW 0x0047c650
// FUNCTION: CARMA2_SW 0x0047b8d0
int C2_HOOK_FASTCALL SwitchToHiresMode(void) {
#ifdef REC2_SW
    if (gGraf_spec_index == 0) {
        int mode = 1;
        gCurrent_graf_data = &gGraf_data[mode];
        gGraf_spec_index = gReal_graf_data_index = gGraf_data_index = mode;
        PDSwitchToHiresMode();
        gRender_screen->pixels = gBack_screen->pixels;
        gRender_screen->row_bytes = gBack_screen->row_bytes;
        gRender_screen->width = gBack_screen->width;
        gRender_screen->height = gBack_screen->height;
        SetupDepthBuffer();
        DRSetPalette(gRender_palette);
        return 1;
    } else {
        DRSetPalette(gRender_palette);
        return 0;
    }
#endif
    return 0;
}

// FUNCTION: CARMA2_HW 0x0047c660
// FUNCTION: CARMA2_SW 0x0047b980
int C2_HOOK_FASTCALL SwitchToLoresMode(void) {
#ifdef REC2_SW
    if (gGraf_spec_index == 1) {
        gCurrent_graf_data = &gGraf_data[0];
        gGraf_data_index = 0;
        gReal_graf_data_index = 0;
        gGraf_spec_index = 0;
        PDSwitchToLoresMode();
        gRender_screen->pixels = gBack_screen->pixels;
        gRender_screen->row_bytes = gBack_screen->row_bytes;
        gRender_screen->width = gBack_screen->width;
        gRender_screen->height = gBack_screen->height;
        SetupDepthBuffer();
        DRSetPalette(gRender_palette);
        return 1;
    } else {
        DRSetPalette(gRender_palette);
        return 0;
    }
#endif
    return 0;
}

#ifdef REC2_SW
// FUNCTION: CARMA2_SW 0x0047ba20
void DRPixelmapDoubledCopy(br_pixelmap* pDestn, br_pixelmap* pSource, int pSource_width, int pSource_height, int pX_offset, int pY_offset) {
    tU16* sptr;
    tU16 pixels;
    tU8* dptr;
    tU8* dptr2;
    tU8 pixel_1;
    tU8 pixel_2;
    int i;
    int j;
    int dst_row_skip;
    int src_row_skip;
    int width_over_2;

    dst_row_skip = 2 * pDestn->row_bytes - 2 * pSource_width;
    src_row_skip = (pSource->row_bytes - pSource_width) / 2;
    sptr = (tU16*)((tU8*)pSource->pixels + 2 * (pSource->row_bytes * pSource_height / 2) - 2 * src_row_skip);
    dptr = (tU8*)pDestn->pixels + (2 * pSource_height + pY_offset) * pDestn->row_bytes - pDestn->row_bytes + 2 * pSource_width;
    dptr2 = dptr - pDestn->row_bytes;
    width_over_2 = pSource_width / 2;
    for (i = 0; i < pSource_height; i++) {
        for (j = 0; j < width_over_2; j++) {
            pixels = *--sptr;
            pixel_1 = pixels >> 8;
            pixel_2 = pixels >> 0;
            *--dptr = pixel_1;
            *--dptr2 = pixel_1;
            *--dptr = pixel_1;
            *--dptr2 = pixel_1;
            *--dptr = pixel_2;
            *--dptr2 = pixel_2;
            *--dptr = pixel_2;
            *--dptr2 = pixel_2;
        }
        dptr -= dst_row_skip;
        dptr2 -= dst_row_skip;
        sptr -= src_row_skip;
    }
}
#endif

// PoshDrawLine

// FUNCTION: CARMA2_HW 0x0047c9c0
// FUNCTION: CARMA2_SW 0x0047bca0
void C2_HOOK_FASTCALL DrawRectangle(br_pixelmap* pMap, int pLeft, int pTop, int pRight, int pBottom, br_uint_32 pColour) {

#ifdef REC2_HW
    if (gBack_screen->type != BR_PMT_INDEX_8) {
        pColour = PaletteEntry16Bit(gRender_palette, pColour);
    }
#endif
    BrPixelmapLine(pMap, pLeft, pTop, pRight, pTop, pColour);
    BrPixelmapLine(pMap, pLeft, pBottom, pRight, pBottom, pColour);
    BrPixelmapLine(pMap, pLeft, pTop, pLeft, pBottom, pColour);
    BrPixelmapLine(pMap, pRight, pTop, pRight, pBottom, pColour);
}

// FUNCTION: CARMA2_HW 0x0047ca50
// FUNCTION: CARMA2_SW 0x0047bd10
void C2_HOOK_FASTCALL DrawRRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, br_uint_32 pColour) {

    BrPixelmapLine(pPixelmap, pLeft + 1, pTop, pRight - 1, pTop, pColour);
    BrPixelmapLine(pPixelmap, pLeft + 1, pBottom, pRight - 1, pBottom, pColour);
    BrPixelmapLine(pPixelmap, pLeft, pTop + 1, pLeft, pBottom - 1, pColour);
    BrPixelmapLine(pPixelmap, pRight, pTop + 1, pRight, pBottom - 1, pColour);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0047cad0
void C2_HOOK_FASTCALL DeviouslyDimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0047cbd0
// FUNCTION: CARMA2_SW 0x0047bd90
void C2_HOOK_CDECL DimRectangle(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {
    tU8* ptr;
    tU8* depth_table_ptr;
    tU8* right_ptr;
    int y;
    int line_skip;
    int width;

#ifdef REC2_HW
    if (gDevious_2d) {
        DeviouslyDimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
        return;
    }
#endif
    ptr = (tU8*)pPixelmap->pixels + pLeft + pPixelmap->row_bytes * pTop;
    line_skip = pPixelmap->row_bytes - pRight + pLeft;
    depth_table_ptr = (tU8*)gDepth_shade_table->pixels + gDepth_shade_table->row_bytes * gDim_amount;
    width = pRight - pLeft;
    if (pKnock_out_corners) {
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
        ptr += line_skip + 1;
        for (y = pTop + 1; y < pBottom - 1; y++, ptr += line_skip) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
        }
        ptr++;
        for (right_ptr = ptr + width - 2; ptr < right_ptr; ptr++) {
            *ptr = depth_table_ptr[*ptr];
        }
    } else {
        for (y = pTop; y < pBottom; y++) {
            for (right_ptr = ptr + width; ptr < right_ptr; ptr++) {
                *ptr = depth_table_ptr[*ptr];
            }
            ptr += line_skip;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047cce0
// FUNCTION: CARMA2_SW 0x0047be70
void C2_HOOK_FASTCALL DimRectangleClipped(br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pKnock_out_corners) {

    if (pLeft < 0) {
        pLeft = 0;
    }
    if (pTop < 0) {
        pTop = 0;
    }
    if (pRight > pPixelmap->width) {
        pRight = pPixelmap->width;
    }
    if (pBottom > pPixelmap->height) {
        pBottom = pPixelmap->height;
    }
    if (pLeft < pRight && pTop < pBottom) {
        DimRectangle(pPixelmap, pLeft, pTop, pRight, pBottom, pKnock_out_corners);
    }
}

// DR8BitPixelmapRotatedAndFeatheredCopy

// DR16BitPixelmapRotatedAndFeatheredCopy

// DRPixelmapRotatedAndFeatheredCopy

// FUNCTION: CARMA2_HW 0x0047d260
void C2_HOOK_FASTCALL DRPixelmapBlendedLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, br_pixelmap* pPalette) {

    DRDrawLine(pMap, pX1, pY1, pX2, pY2, pColour);
}

// DR8BitFancyDrawLine

// STUB: CARMA2_HW 0x0047d2b0
void C2_HOOK_FASTCALL FancyDrawLine(br_pixelmap* pMap, int pX1, int pY1, int pX2, int pY2, br_uint_32 pColour, tU32 pTime_period) {
    NOT_IMPLEMENTED();
}

// DRPixelmapCopyMapBlack8Bit

// DRPixelmapCopyMapBlack

// FUNCTION: CARMA2_HW 0x0047d6a0
void C2_HOOK_FASTCALL DisableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightDisable(gLight_array[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0047d6d0
void C2_HOOK_FASTCALL EnableLights(void) {
    int i;

    for (i = 0; i < gNumber_of_lights; i++) {
        BrLightEnable(gLight_array[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0047d7b0
// FUNCTION: CARMA2_SW 0x0047d970
void C2_HOOK_FASTCALL DRPixelmapBlend75Copy(br_pixelmap* pDest, br_int_16 pDest_x, br_int_16 pDest_y, br_pixelmap* pSource) {
    int x;
    int y;
    tU8* dest_ptr;

    for (y = 0; y < pSource->height; y++) {
        dest_ptr = (tU8*)pDest->pixels + (pDest_y + y) * pDest->row_bytes + pDest_x;
        for (x = 0; x < pSource->width; x++) {
            *dest_ptr = ((tU8*)gBlend75_table->pixels)[*dest_ptr + 256 * ((tU8*)pSource->pixels)[y * pSource->row_bytes + x]];
            dest_ptr++;
        }
    }
}

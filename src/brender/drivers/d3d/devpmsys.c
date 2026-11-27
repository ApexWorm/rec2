/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "devpmsys.h"

#include "devpixmp.h"
#include "object.h"
#include "core/fw/object.h"
#include "core/fw/resource.h"
#include "core/host/prot.h"
#include "core/pixelmap/pmgen.h"
#include "core/pixelmap/pmmem.h"
#include "core/pixelmap/pmnull.h"

// GLOBAL: D3D 0x10015590
static pm_type_info pmTypeInfoD3D[] = {
    { 1,    1,      32, BR_PMCHAN_INDEX,                        },
    { 2,    1,      16, BR_PMCHAN_INDEX,                        },
    { 4,    1,      8,  BR_PMCHAN_INDEX,                        },
    { 8,    1,      4,  BR_PMCHAN_INDEX,                        },

    { 16,   2,      2,  BR_PMCHAN_RGB,                          },
    { 16,   2,      2,  BR_PMCHAN_RGB,                          },
    { 24,   3,      4,  BR_PMCHAN_RGB,                          },
    { 32,   4,      1,  BR_PMCHAN_RGB,                          },
    { 32,   4,      1,  BR_PMCHAN_RGB | BR_PMCHAN_ALPHA,        },

    { 16,   1,      2,  BR_PMCHAN_YUV,                          },
    { 32,   1,      1,  BR_PMCHAN_YUV,                          },

    { 16,   2,      4,  BR_PMCHAN_DEPTH,                        },
    { 32,   4,      4,  BR_PMCHAN_DEPTH,                        },
    { 8,    1,      4,  BR_PMCHAN_ALPHA,                        },
    { 16,   2,      2,  BR_PMCHAN_INDEX | BR_PMCHAN_ALPHA       },
};

static br_device_pixelmap_dispatch devicePixelmapDispatchSysmem;

// FUNCTION: D3D 0x10001ec0
br_error C2_HOOK_CDECL AllocateD3DSysMemPixelmap(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, int w, int h) {
    br_uint_16 qual;
    br_device_d3d* dev;
    br_device_pixelmap_d3d* devpm;
    pm_type_info* tip = pmTypeInfoD3D + self->pm_type;

    /* Allocate new pixelmap */
    dev = (br_device_d3d*)self->dispatch->_device((br_object*)self);
    devpm = BrResAllocate(dev->res, sizeof(*devpm), BR_MEMORY_OBJECT);

    /* Fill members, mostly by copy */
    *devpm = *self;
    devpm->dispatch = &devicePixelmapDispatchSysmem;
    devpm->pm_width = w;
    devpm->pm_height = h;
    devpm->pm_row_bytes = tip->bits * tip->align * ((w + tip->align - 1) / tip->align) / 8;
    HostSelectorDS(&qual);
    devpm->pm_pixels_qualifier = qual;
    devpm->buffer_type = BT_SYSMEM;

    /* Allocate pixels */
    devpm->pm_pixels = BrResAllocate(devpm, devpm->pm_row_bytes * devpm->pm_height, BR_MEMORY_PIXELS);

    *newpm = devpm;

    return 0;
}

// FUNCTION: D3D 0x10001f80
static void C2_HOOK_CDECL _M_br_device_pixelmap_d3d_sysmem_free(br_device_pixelmap_d3d* self) {

    /*
     * Free pixelmap structure
     */
    BrResFreeNoCallback(self);
}

/*
 * Default dispatch table for device pixelmap
 */
// GLOBAL: D3D 0x10015608
static br_device_pixelmap_dispatch devicePixelmapDispatchSysmem = {
    NULL,
    NULL,
    NULL,
    NULL,
    (void*)_M_br_device_pixelmap_d3d_sysmem_free,
    (void*)_M_br_object_d3d_identifier,
    (void*)_M_br_device_pixelmap_d3d_type,
    (void*)_M_br_device_pixelmap_d3d_isType,
    (void*)_M_br_object_d3d_device,
    (void*)_M_br_device_pixelmap_d3d_space,

    (void*)_M_br_device_pixelmap_d3d_queryTemplate,
    (void*)_M_br_object_query,
    (void*)_M_br_object_queryBuffer,
    _M_br_object_queryMany,
    _M_br_object_queryManySize,
    _M_br_object_queryAll,
    _M_br_object_queryAllSize,

    _M_br_device_pixelmap_mem_validSource,
    _M_br_device_pixelmap_null_resize,
    _M_br_device_pixelmap_mem_match,
    _M_br_device_pixelmap_mem_allocateSub,

    _M_br_device_pixelmap_gen_copyTo,
    _M_br_device_pixelmap_gen_copyTo,
    _M_br_device_pixelmap_gen_copyFrom,
    _M_br_device_pixelmap_mem_fill,
    _M_br_device_pixelmap_mem_doubleBuffer,

    _M_br_device_pixelmap_gen_copyDirty,
    _M_br_device_pixelmap_gen_copyToDirty,
    _M_br_device_pixelmap_gen_copyFromDirty,
    _M_br_device_pixelmap_gen_fillDirty,
    _M_br_device_pixelmap_gen_doubleBufferDirty,

    _M_br_device_pixelmap_gen_rectangle,
    _M_br_device_pixelmap_gen_rectangle2,
    _M_br_device_pixelmap_mem_rectangleCopyTo,
    _M_br_device_pixelmap_mem_rectangleCopyTo,
    _M_br_device_pixelmap_mem_rectangleCopyFrom,
    (void*)_M_br_device_pixelmap_d3d_rectangleStretchCopy,
    _M_br_device_pixelmap_mem_rectangleStretchCopyTo,
    _M_br_device_pixelmap_mem_rectangleStretchCopyFrom,
    _M_br_device_pixelmap_mem_rectangleFill,
    _M_br_device_pixelmap_mem_pixelSet,
    _M_br_device_pixelmap_gen_line,
    _M_br_device_pixelmap_gen_copyBits,

    _M_br_device_pixelmap_gen_text,
    _M_br_device_pixelmap_gen_textBounds,

    _M_br_device_pixelmap_mem_rowSize,
    _M_br_device_pixelmap_mem_rowQuery,
    _M_br_device_pixelmap_mem_rowSet,

    _M_br_device_pixelmap_mem_pixelQuery,
    _M_br_device_pixelmap_mem_pixelAddressQuery,

    _M_br_device_pixelmap_mem_pixelAddressSet,
    _M_br_device_pixelmap_mem_originSet,

    _M_br_device_pixelmap_mem_flush,
    _M_br_device_pixelmap_mem_synchronise,
    _M_br_device_pixelmap_mem_directLock,
    _M_br_device_pixelmap_mem_directUnlock,
};

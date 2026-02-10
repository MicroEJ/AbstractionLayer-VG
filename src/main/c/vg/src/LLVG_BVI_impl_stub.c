/*
 * C
 *
 * Copyright 2023-2026 MicroEJ Corp. All rights reserved.
 * MicroEJ Corp. PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/**
 * @file
 * @brief MicroVG library low level API: stubbed implementation of LLVG_BVI_impl.h.
 *
 * @author MicroEJ Developer Team
 * @version 8.0.1
 */

#include "vg_configuration.h"

#if defined VG_FEATURE_BUFFERED_VECTOR_IMAGE && (VG_FEATURE_BUFFERED_VECTOR_IMAGE != 1)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_BVI_impl.h>

// -----------------------------------------------------------------------------
// LLVG_BVI_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void LLVG_BVI_IMPL_map_context(MICROUI_Image *ui, MICROVG_Image *vg) {
	(void)ui;
	(void)vg;
	// nothing to do
}

// See the header file for the function documentation
void LLVG_BVI_IMPL_clear(MICROUI_GraphicsContext *gc) {
	(void)gc;
	// nothing to do
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#endif // VG_FEATURE_BUFFERED_VECTOR_IMAGE

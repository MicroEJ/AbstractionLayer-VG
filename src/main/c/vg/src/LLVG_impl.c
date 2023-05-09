/*
 * C
 *
 * Copyright 2022-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Initializes the MicroVG implementation.
 * @author MicroEJ Developer Team
 * @version 3.0.0
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <LLVG_impl.h>
#include <LLUI_DISPLAY.h>

#include "microvg_configuration.h"
#include "microvg_helper.h"
#include "microvg_font_freetype.h"
#include "microvg_path.h"

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

#if (defined(LLVG_MAJOR_VERSION) && (LLVG_MAJOR_VERSION != 1)) || (defined(LLVG_MINOR_VERSION) && (LLVG_MINOR_VERSION < 3))
#error "This CCO is only compatible with VG Pack [1.3.0,2.0.0["
#endif

// -----------------------------------------------------------------------------
// LLVG_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void LLVG_IMPL_initialize(void) {
	MICROVG_HELPER_initialize();

#if defined VG_FEATURE_FONT && \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

	MICROVG_FONT_FREETYPE_initialize();
#endif

	MICROVG_PATH_initialize();
}


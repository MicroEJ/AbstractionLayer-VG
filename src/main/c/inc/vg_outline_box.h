/*
 * C
 *
 * Copyright 2026 MicroEJ Corp. All rights reserved.
 * MicroEJ Corp. PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * Build: 7E4D1F7C
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: box of the curve an outline draws.
 * @author MicroEJ Developer Team
 * @version 8.0.3
 */

#if !defined VG_OUTLINE_BOX_H
#define VG_OUTLINE_BOX_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_configuration.h"

#if defined VG_FEATURE_FONT &&                                                              \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

#include <stdbool.h>
#include <freetype/ftoutln.h>

// -----------------------------------------------------------------------------
// Typedef
// -----------------------------------------------------------------------------

/**
 * @brief The box of the curve an outline draws, control points excluded.
 *
 * Coordinates are in the units of the outline they were computed from. Every caller loads its
 * glyphs with FT_LOAD_NO_SCALE, so they are font units.
 */
typedef struct {
	float x_min; /**< The smallest x coordinate of the box. */
	float x_max; /**< The largest x coordinate of the box. */
	float y_min; /**< The smallest y coordinate of the box. */
	float y_max; /**< The largest y coordinate of the box. */
	bool empty; /**< true when the outline draws nothing: the four coordinates are then 0 */
} VG_OUTLINE_BOX;

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/**
 * @brief Computes the box of the curve the given outline draws, where the box a font declares
 * also bounds the control points, which the curve never touches.
 *
 * The walk relies on FT_Outline_Decompose, which validates nothing: it reports only the
 * malformations it happens to trip over, and an outline whose contour ends past its point array
 * is read past that array and yields an arbitrary box. Passing a well-formed outline is the
 * caller's responsibility; every caller passes an outline FreeType itself produced.
 *
 * @param[in] outline the well-formed outline to measure
 * @param[out] box the box, zeroed and flagged empty when the outline draws nothing, and when
 * FreeType reports a malformation while walking it
 */
void VG_OUTLINE_BOX_compute(const FT_Outline *outline, VG_OUTLINE_BOX *box);

// cppcheck-suppress [misra-c2012-3.2]
#endif \
    // defined VG_FEATURE_FONT && \
    // (defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
    // (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined VG_OUTLINE_BOX_H

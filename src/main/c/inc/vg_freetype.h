/*
 * C
 *
 * Copyright 2020-2026 MicroEJ Corp. All rights reserved.
 * MicroEJ Corp. PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * Build: 7E4D1F7C
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: implementation over FreeType.
 * @author MicroEJ Developer Team
 * @version 8.0.3
 */

#if !defined VG_FREETYPE_H
#define VG_FREETYPE_H

#if defined __cplusplus
extern "C" {
#endif

#include "vg_configuration.h"

#if defined VG_FEATURE_FONT &&                                                              \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_path.h"

// --------------------------------------------------------------------------------
// Typedef
// --------------------------------------------------------------------------------

/**
 * @brief Function to draw a character element: a glyph. A glyph is a vectorial path.
 *
 * This function is called by VG_FREETYPE_draw_string(). Two implementations
 * are required: one that draws the path with a color and one that draws the path with
 * a gradient.
 *
 * The list of the drawing parameters is reduced to the elements that the Freetype engine
 * changes for each glyph: the glyph's path, the transformation to apply on the glyph,
 * the glyph color (useful for colored glyphs like emoji), the glyph's outine.
 *
 * All others elements required by the drawer (if any) have to be listed in the dedicated
 * structure user_data.
 *
 * @return: the MicroVG error code
 */
typedef jint (* VG_FREETYPE_draw_glyph_t) (
	/*
	 * @brief The path of the glyph.
	 */
	VG_PATH_HEADER_t *path,

	/*
	 * @brief The deformation to apply on the path.
	 */
	jfloat *matrix,

	/*
	 * @brief The specific glyph color (example: emoji) or the current destination
	 * color. Useless if the string is drawn with a gradient.
	 */
	uint32_t color,

	/*
	 * @brief The glyph outline: true: EVEN_ODD or false: NON_ZERO.
	 */
	bool fill_rule_even_odd,

	/*
	 * @brief The custom drawer data (may be null). For instance this structure can
	 * hold the gradient if the GPU's drawing function requires it. In case of
	 * the gradient is set before calling VG_FREETYPE_draw_string(), the
	 * gradient doesn't need to be stored in this structure.
	 */
	void *user_data);

// --------------------------------------------------------------------------------
// API
// --------------------------------------------------------------------------------

/**
 * @brief Initializes the lowlevel font library.
 */
void VG_FREETYPE_initialize(void);

/**
 * @brief Folds the ink of a string and returns its two extreme edges.
 *
 * Every glyph is placed at its own pen position, and a glyph that draws nothing contributes no
 * edge at either end. The pen carries the letter spacing, so a string that draws no glyph has no
 * span whatever the spacing is.
 *
 * Returns the two extreme edges of the ink; the caller decides what to do with them.
 *
 * A caller that passes NULL for the right edge asks for the left one alone. The fold then stops as
 * soon as no remaining glyph can reach further left, so it reads a few glyphs instead of the whole
 * string. It returns the same left edge as a full fold as long as no glyph is placed more than one
 * EM to the left of its own pen position, which is the margin the stop keeps; a letter spacing
 * below zero and an advance seen to be negative each disable the stop instead of narrowing that
 * margin.
 *
 * The function leaves no iteration open, so a caller that iterates afterwards must call
 * VG_HELPER_layout_configure() again.
 *
 * @param[in] text the array of characters to fold.
 * @param[in] length the length of the array.
 * @param[in] face_handle the font reference handle.
 * @param[in] letter_spacing_font_units the extra letter spacing, in font units.
 * @param[out] left the left edge of the ink, in font units; untouched when no glyph draws.
 * @param[out] right the right edge of the ink, in font units; untouched when no glyph draws. NULL
 *    asks for the left edge alone, which lets the fold stop before the end of the string.
 *
 * @return true when at least one glyph draws, false when none does.
 */
bool VG_FREETYPE_string_span(const jchar *text, jint length, jint face_handle, jfloat letter_spacing_font_units,
                             jfloat *left, jfloat *right);

/**
 * @brief Measures the width of a text for the specified font and size.
 *
 * The width is the union of the ink of every glyph, each at its own pen position: the leftmost
 * pixel any glyph draws to the rightmost, whichever glyphs those are. A character that draws
 * nothing contributes nothing, at either end.
 *
 * @param[in] text the array of characters to draw.
 * @param[in] length the length of the array
 * @param[in] face_handle the font reference handle
 * @param[in] size the height of the font in pixels.
 * @param[in] letter_spacing the extra letter spacing to use
 *
 * @return the width of the specified string, in pixels, never negative.
 */
jfloat VG_FREETYPE_string_width(jchar *text, jint length, jint face_handle, jfloat size, jfloat letter_spacing);

/**
 * @brief Computes the scale that converts font units to pixels at the given size.
 *
 * LLVG_FONT_IMPL_load_font() refuses a face whose EM size is 0, so the division is always defined.
 *
 * @param[in] face_handle the font reference handle.
 * @param[in] size the height of the font in pixels.
 *
 * @return the size of one font unit, in pixels.
 */
jfloat VG_FREETYPE_get_scale(jint face_handle, jfloat size);

/**
 * @brief Draws a string using the Freetype engine along a line or a circle, with a
 * color or a linear gradient. The implementation does not draw, it calls the
 * drawer function for each glyph: VG_FREETYPE_draw_glyph_t.
 *
 * @param[in] drawer the function to draw a glyph.
 * @param[in] text the array of characters to draw.
 * @param[in] length the length of the array
 * @param[in] face_handle the font reference handle.
 * @param[in] size the height of the font in pixels.
 * @param[in] matrix the transformation to apply.
 * @param[in] color the 32-bit color to apply (useless when drawing with a gradient).
 * @param[in] letter_spacing the extra letter spacing to use.
 * @param[in] radius the radius of the circle (0 to draw along a line).
 * @param[in] direction the direction of the text along the circle (0 to draw along a line).
 * @param[in] user_data the data used by the drawer; may be null.
 *
 * @return LLVG_SUCCESS if something has been drawn, an different value otherwise
 */
jint VG_FREETYPE_draw_string(VG_FREETYPE_draw_glyph_t drawer, const jchar *text, jint length, jint face_handle,
                             jfloat size, const jfloat *matrix, uint32_t color, jfloat letter_spacing, jfloat radius,
                             jint direction, void *user_data);

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

/*
 * @brief Converts an ARGB8888 color to a format compatible with the GPU. The default
 * implementation does nothing and just returns the original color.
 */
jint VG_FREETYPE_IMPL_convert_color(jint color);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------
#endif \
    // defined VG_FEATURE_FONT && \
    // (defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
    // (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

#ifdef __cplusplus
}
#endif

#endif // !defined VG_FREETYPE_H

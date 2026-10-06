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
 * @brief MicroEJ MicroVG library low level API: implementation over Freetype.
 * @author MicroEJ Developer Team
 * @version 8.0.3
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_configuration.h"

#if defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

#include <math.h>
#include <inttypes.h>

#include <freetype/internal/ftobjs.h>
#if defined VG_FEATURE_FREETYPE_COLORED_EMOJI && (VG_FEATURE_FREETYPE_COLORED_EMOJI == 1)
#include <freetype/ftcolor.h>
#endif // VG_FEATURE_FREETYPE_COLORED_EMOJI
#include "ftvector/ftvector.h"

#include <LLVG_FONT_impl.h>
#include <LLVG_PATH_impl.h>
#include <LLVG_GRADIENT_impl.h>
#include <LLVG_MATRIX_impl.h>
#include <sni.h>

#include "vg_freetype.h"
#include "vg_helper.h"
#include "bsp_util.h"

VG_LOG_DECLARE_MODULE()

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

#if defined VG_FEATURE_FREETYPE_COLORED_EMOJI && (VG_FEATURE_FREETYPE_COLORED_EMOJI == 1)
#define FT_COLOR_TO_INT(x) (*((int *)&(x)))
#endif // VG_FEATURE_FREETYPE_COLORED_EMOJI

#define DIRECTION_CLOCK_WISE 0

// -----------------------------------------------------------------------------
// Extern Variables
// -----------------------------------------------------------------------------

extern FT_Library library;
extern FT_Renderer renderer;

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

/*
 * @brief Sets renderer parameters.
 */
static void __set_renderer(FTVECTOR_draw_glyph_data_t *data) {
	FT_Parameter params[1];

	params[0].tag = FT_PARAM_TAG_DRAWER;
	// cppcheck-suppress [misra-c2012-11.1] pointer conversion to store the drawer
	params[0].data = (void *)data;

	FT_Set_Renderer(library, renderer, 1, &params[0]);
}

/*
 * @brief Updates the angle to use for the next glyph when drawn on an arc.
 * When drawing on an arc, the glyph position is defined by its angle. We thus
 * convert the advance (distance to the next glyph) to an angle.
 */
static float __get_angle(float advance, float radius) {
	float angle = advance / radius;
	angle *= 180.0f;
	angle /= M_PI;
	return angle;
}

#if defined VG_FEATURE_FREETYPE_COLORED_EMOJI && (VG_FEATURE_FREETYPE_COLORED_EMOJI == 1)

/**
 * @brief Renders the glyph the layout loaded. When the glyph has color layers, loads and renders
 * each of them instead, in the color the palette gives it.
 *
 * A glyph without layers costs a single FT_Get_Color_Glyph_Layer() call: on a target that reads the
 * font from slow external memory, each call reads the COLR table.
 *
 * @param[in] face: the face of the font.
 * @param[in] glyph_index: the index of the glyph, already loaded by the layout.
 * @param[in] palette: the palette of the face, NULL when the face has none.
 * @param[in,out] drawer_data: the renderer parameters; its color is restored before returning.
 *
 * @return FT_ERR( Ok ) on a success, a different value otherwise.
 */
static FT_Error __render_glyph(FT_Face face, FT_UInt glyph_index, FT_Color *palette,
                               FTVECTOR_draw_glyph_data_t *drawer_data) {
	FT_Error error;

	FT_UInt layer_glyph_index = 0;
	FT_UInt layer_color_index = 0;
	FT_LayerIterator iterator;
	iterator.p = NULL;

	if ((NULL == palette) ||
	    (0U == FT_Get_Color_Glyph_Layer(face, glyph_index, &layer_glyph_index, &layer_color_index, &iterator))) {
		// convert to an anti-aliased bitmap
		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	} else {
		uint32_t default_color = drawer_data->color;
		bool has_next_layer;

		do {
			// Update renderer color with layer_color
			if (layer_color_index != 0xFFFF) {
				drawer_data->color = VG_FREETYPE_IMPL_convert_color(FT_COLOR_TO_INT(palette[layer_color_index]));
			}

			error = FT_Load_Glyph(face, layer_glyph_index, FT_LOAD_NO_SCALE);
			if (FT_ERR(Ok) == error) {
				// convert to an anti-aliased bitmap
				error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			} else {
				VG_LOG_ERROR("Error while loading glyphid %d: 0x%x, refer to fterrdef.h", layer_glyph_index,
				             error);
			}

			has_next_layer = (FT_ERR(Ok) == error) &&
			                 (0U != FT_Get_Color_Glyph_Layer(face, glyph_index, &layer_glyph_index,
			                                                 &layer_color_index, &iterator));
		} while (has_next_layer);

		// Revert renderer color to original color.
		drawer_data->color = default_color;
	}

	return error;
}

#endif // VG_FEATURE_FREETYPE_COLORED_EMOJI

// -----------------------------------------------------------------------------
// vg_freetype.h painter functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint VG_FREETYPE_draw_string(VG_FREETYPE_draw_glyph_t drawer, const jchar *text, jint length, jint faceHandle,
                             jfloat size, const jfloat *matrix, uint32_t color, jfloat letterSpacing, jfloat radius,
                             jint direction, void *user_data) {
	jint result = LLVG_SUCCESS;

	if (0 < length) {
		FT_Face face = (FT_Face)faceHandle;
#if defined VG_FEATURE_FREETYPE_COLORED_EMOJI && (VG_FEATURE_FREETYPE_COLORED_EMOJI == 1)
		FT_Color *palette;

		// Select palette
		if (0 != FT_Palette_Select(face, 0, &palette)) {
			palette = NULL;
		}
#endif // VG_FEATURE_FREETYPE_COLORED_EMOJI

		float scale = VG_FREETYPE_get_scale(faceHandle, size);
		float letterSpacingFontUnits = letterSpacing / scale;
		float radiusScaled = radius / scale;
		short baselineposition = face->ascender;

		// transformation to apply on all glyphs
		float scaled_matrix[LLVG_MATRIX_SIZE];
		LLVG_MATRIX_IMPL_copy(scaled_matrix, matrix);
		LLVG_MATRIX_IMPL_scale(scaled_matrix, scale, scale);

		float working_matrix[LLVG_MATRIX_SIZE];

		FTVECTOR_draw_glyph_data_t drawer_data;
		drawer_data.drawer = drawer;
		drawer_data.matrix = working_matrix;
		drawer_data.color = color;
		drawer_data.user_data = user_data;
		drawer_data.destination_error = LLVG_SUCCESS;

		// give drawing parameters to freetype
		__set_renderer(&drawer_data);

		int glyph_index;  // current glyph index
		int glyph_offset_y;
		int glyph_advance_x;
		int glyph_advance_y;
		int glyph_offset_x;
		float advance_x = 0.f;
		int advance_y = 0;

		// The string starts on the leftmost ink of the whole string, not on the first glyph's:
		// the two differ as soon as a later glyph reaches further left, and the measure is the
		// width of that same box.
		float span_left = 0.f;
		if (VG_FREETYPE_string_span(text, length, faceHandle, letterSpacingFontUnits, &span_left, NULL)) {
			advance_x = -span_left;
		}

		VG_HELPER_layout_configure(faceHandle, text, length);

		while ((LLVG_SUCCESS == result) && (VG_HELPER_layout_load_glyph((uint32_t *)&glyph_index, &glyph_advance_x,
		                                                                &glyph_advance_y, &glyph_offset_x,
		                                                                &glyph_offset_y))) {
			// At that point the current glyph has been loaded by Freetype

			int charWidth = glyph_advance_x;

			// reset drawer's matrix
			LLVG_MATRIX_IMPL_copy(working_matrix, scaled_matrix);

			if (0.f == radius) {
				LLVG_MATRIX_IMPL_translate(working_matrix, advance_x + (float)glyph_offset_x,
				                           (float)(baselineposition + advance_y + glyph_offset_y));
			} else {
				float sign = (DIRECTION_CLOCK_WISE != direction) ? -1.f : 1.f;

				// Space characters joining bboxes at baseline
				float angleDegrees = 90 + __get_angle(advance_x + (float)glyph_offset_x,
				                                      radiusScaled) + __get_angle((float)(charWidth / 2), radiusScaled);

				// Rotate to angle
				LLVG_MATRIX_IMPL_rotate(working_matrix, sign * angleDegrees);

				// Translate left to center of bbox
				// Translate baseline over circle
				LLVG_MATRIX_IMPL_translate(working_matrix, -charWidth / 2, -sign * radiusScaled);
			}

			// Draw the glyph
#if defined VG_FEATURE_FREETYPE_COLORED_EMOJI && (VG_FEATURE_FREETYPE_COLORED_EMOJI == 1)
			FT_Error error = __render_glyph(face, glyph_index, palette, &drawer_data);
#else
			// convert to an anti-aliased bitmap
			FT_Error error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
#endif // VG_FEATURE_FREETYPE_COLORED_EMOJI
			if (FT_ERR(Ok) != error) {
				if (LLVG_SUCCESS != drawer_data.destination_error) {
					// the drawing destination refused: FreeType only relayed its error
					result = drawer_data.destination_error;
					VG_LOG_ERROR("String cut at glyphid %d: the drawing destination refused (error %" PRId32 ")",
					             glyph_index, result);
				} else {
					VG_LOG_ERROR("Error while rendering glyphid %d: 0x%x, refer to fterrdef.h", glyph_index,
					             error);
					result = (FT_ERR(Out_Of_Memory) == error) ? LLVG_OUT_OF_MEMORY : LLVG_DATA_INVALID;
				}
				continue;
			}

			// Compute advance to next glyph
			advance_x += (float)charWidth;
			advance_x += letterSpacingFontUnits;
			advance_y += glyph_advance_y;
		}
	}

	return result;
}

// See the header file for the function documentation
BSP_DECLARE_WEAK_FCNT jint VG_FREETYPE_IMPL_convert_color(jint color) {
	return color;
}

#endif // defined VG_FEATURE_FONT && (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

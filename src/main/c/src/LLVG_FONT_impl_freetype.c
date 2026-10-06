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
 * @brief MicroEJ MicroVG library low level API: implementation over FreeType
 * @author MicroEJ Developer Team
 * @version 8.0.3
 */

#include <assert.h>
#include "vg_configuration.h"

#if defined VG_FEATURE_FONT &&                                                              \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <math.h>
#include <string.h>
#include <aftypes.h>

#include <LLVG_impl.h>
#include <LLVG_FONT_impl.h>

#if defined VG_FEATURE_FONT_EXTERNAL && (VG_FEATURE_FONT_EXTERNAL == 1)
#include <LLEXT_RES_impl.h>
#include <freetype/internal/ftmemory.h>
#endif

#include "vg_freetype.h"
#include "vg_helper.h"
#include "vg_outline_box.h"
#include "vg_trace.h"
#include "ui_util.h"

VG_LOG_DECLARE_MODULE()

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/*
 * @brief Macro to add a FONT event and its type.
 */
#define LOG_MICROVG_FONT_START(fn) LOG_MICROVG_START(LOG_MICROVG_FONT_ID, CONCAT_DEFINES(LOG_MICROVG_FONT_, fn))
#define LOG_MICROVG_FONT_END(fn) LOG_MICROVG_END(LOG_MICROVG_FONT_ID, CONCAT_DEFINES(LOG_MICROVG_FONT_, fn))

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/*
 * @brief Structure to load a resource by calling SNIX_get_resource()
 */
typedef struct {
	void *data;
	uint32_t size;
} SNIX_resource;

// -----------------------------------------------------------------------------
// Extern functions
// -----------------------------------------------------------------------------

/*
 * @brief SNIX_get_resource() available since MicroEJ Architecture version 7.13
 */
extern int32_t SNIX_get_resource(jchar *path, SNIX_resource *resource);

// -----------------------------------------------------------------------------
// Internal function definitions
// -----------------------------------------------------------------------------

/*
 * @brief Disposes Freetype font when the associated Java object VectorFont is
 * garbaged collected.
 */
static void __dispose_registered_font(void *faceHandle);

/*
 * @brief Disposes Freetype font.
 */
static void __dispose_font(void *faceHandle);

/*
 * @brief Opens a font that has been loaded into memory.
 *
 * @see FT_New_Memory_Face in freetype.h
 *
 * @param[in] face: a handle to a new face object.
 * @param[in] data: a pointer to the beginning of the font data.
 * @param[in] length: the size of the memory chunk used by the font data.
 *
 * @return FreeType error code.
 */
static FT_Error __load_memory_font(FT_Face *face, void *data, int length);

/*
 * @brief Opens a font that has been compiled with the application.
 *
 * @see FT_New_Memory_Face in freetype.h
 *
 * @param[in] face: a handle to a new face object.
 * @param[in] font_name: a path to the font file.
 *
 * @return FreeType error code.
 */
static FT_Error __load_internal_font(FT_Face *face, jchar *font_name);

/*
 * @brief Gets the description of the given native resource: a vector font.
 * @see SNI_getDescriptionFunction
 */
static void __register_font_description(void *resource, char *buffer, uint32_t bufferLength);

#if defined VG_FEATURE_FONT_EXTERNAL && (VG_FEATURE_FONT_EXTERNAL == 1)

/*
 * @brief Opens a font that has not been compiled with the application.
 *
 * @see FT_Open_Face in freetype.h
 *
 * @param[in] face: a handle to a new face object.
 * @param[in] font_name: a path to the font file.
 *
 * @return FreeType error code.
 */
static FT_Error __load_external_font(FT_Face *face, jchar *font_name);

/*
 * @brief Reads a chunk from an external resource.
 *
 * @see FT_Stream_IoFunc in ftsystem.h
 *
 * @param[in] stream: a handle to the source stream.
 * @param[in] offset: the offset of read in stream.
 * @param[in] buffer: the address of the read buffer.
 * @param[in] count: the number of bytes to read from the stream.
 *
 * @return the number of bytes effectively read by the stream.
 */
static unsigned long __read_external_resource(FT_Stream stream, unsigned long offset, unsigned char *buffer,
                                              unsigned long count);

/*
 * @brief Closes a given input stream.
 *
 * @see FT_Stream_IoFunc in ftsystem.h
 *
 * @param[in] stream: a handle to the target stream.
 */
static void __close_external_resource(FT_Stream stream);

#endif // VG_FEATURE_FONT_EXTERNAL

// -----------------------------------------------------------------------------
// Global Variables
// -----------------------------------------------------------------------------

FT_Library library;
FT_Renderer renderer;

#if defined VG_FEATURE_FONT && defined VG_FEATURE_FONT_FREETYPE_VECTOR && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR)
const char *renderer_name = "microvg";
#endif
#if defined VG_FEATURE_FONT && defined VG_FEATURE_FONT_FREETYPE_BITMAP && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)
const char *renderer_name = "smooth";
#endif

// -----------------------------------------------------------------------------
// vg_freetype.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void VG_FREETYPE_initialize(void) {
	FT_Error error = FT_Init_FreeType(&library);
	if (FT_ERR(Ok) == error) {
		renderer = FT_RENDERER(FT_Get_Module(library, renderer_name));
		if (0 != renderer) {
			VG_LOG_INFO("Freetype renderer: %s", FT_MODULE_CLASS(renderer)->module_name);
		} else {
			VG_LOG_ERROR("No renderer found with name %s", renderer_name);
		}
	} else {
		VG_LOG_ERROR("Internal freetype error initializing library, ID = %d", error);
	}
}

// See the header file for the function documentation
bool VG_FREETYPE_string_span(const jchar *text, jint length, jint face_handle, jfloat letter_spacing_font_units,
                             jfloat *left, jfloat *right) {
	FT_Face face = (FT_Face)face_handle;
	bool has_drawn_glyph = false;
	float pen = 0.f;
	float span_left = 0.f;
	float span_right = 0.f;

	// Layout variables
	FT_UInt glyph_index = 0;

	int advance_x = 0;
	int advance_y = 0;
	int offset_x = 0;
	int offset_y = 0;

	// A caller that asks for the left edge alone lets the fold stop once the pen has moved past the
	// leftmost ink the face declares: while the pen grows, no later glyph can reach further left.
	// The margin of one EM covers what places a glyph left of its own pen, the kerning and the
	// positioning offsets, and a face declaring a box tighter than the ink it draws. So the stop
	// needs the pen to grow, which a letter spacing below zero and a negative advance both break.
	bool stop_on_left = (NULL == right) && (letter_spacing_font_units >= 0.f);
	const float unreachable_left = MIN((float)face->bbox.xMin, 0.f) - (float)face->units_per_EM;

	// Box the current glyph draws, control points excluded.
	VG_OUTLINE_BOX box = { .x_min = 0.f, .x_max = 0.f, .y_min = 0.f, .y_max = 0.f, .empty = true };

	VG_HELPER_layout_configure(face_handle, text, length);

	while (VG_HELPER_layout_load_glyph(&glyph_index, &advance_x, &advance_y, &offset_x, &offset_y)) {
		// At that point the current glyph has been loaded by Freetype
		VG_OUTLINE_BOX_compute(&face->glyph->outline, &box);

		// A space carries a real glyph index and an empty box, so the box is what says whether the
		// glyph draws. An empty one contributes no edge, at either end.
		if (!box.empty) {
			float glyph_left = pen + (float)offset_x + box.x_min;
			float glyph_right = pen + (float)offset_x + box.x_max;

			if (!has_drawn_glyph) {
				span_left = glyph_left;
				span_right = glyph_right;
				has_drawn_glyph = true;
			} else {
				span_left = MIN(glyph_left, span_left);
				span_right = MAX(glyph_right, span_right);
			}
		}

		float step = (float)advance_x + letter_spacing_font_units;
		pen += step;

		// A step that moves the pen backwards is evidence that this face does it, so the fold stops
		// trusting the bound and reads the string to its end.
		if (step < 0.f) {
			stop_on_left = false;
		}

		if (stop_on_left && has_drawn_glyph && ((pen + unreachable_left) >= span_left)) {
			break;
		}
	}

	// The iteration may have stopped before its end, which leaves the layouter holding it.
	VG_HELPER_layout_stop();

	if (has_drawn_glyph) {
		*left = span_left;
		if (NULL != right) {
			*right = span_right;
		}
	}

	return has_drawn_glyph;
}

// See the header file for the function documentation
jfloat VG_FREETYPE_get_scale(jint face_handle, jfloat size) {
	FT_Face face = (FT_Face)face_handle;
	return size / (float)face->units_per_EM;
}

// See the header file for the function documentation
jfloat VG_FREETYPE_string_width(jchar *text, jint length, jint face_handle, jfloat size, jfloat letter_spacing) {
	float scaled_width = 0.f;
	FT_Face face = (FT_Face)face_handle;

	if ((size > 0) && (length > 0)) {
		float scale = VG_FREETYPE_get_scale(face_handle, size);

		float left = 0.f;
		float right = 0.f;

		if (VG_FREETYPE_string_span(text, length, face_handle, letter_spacing / scale, &left, &right)) {
			assert(right >= left);
			scaled_width = scale * (right - left);
		}
	}

	return scaled_width;
}

// -----------------------------------------------------------------------------
// LLVG_FONT_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint LLVG_FONT_IMPL_load_font(jchar *font_name, jboolean complex_layout) {
	LOG_MICROVG_FONT_START(load);

	FT_Face face = 0;
	jint ret = LLVG_FONT_UNLOADED;

	// Check that complex layouter is available for complex text layout
	if (complex_layout && !LLVG_FONT_IMPL_has_complex_layouter()) {
		ret = LLVG_FONT_UNLOADED;
	} else {
		// try to load an internal resource
		FT_Error error = __load_internal_font(&face, font_name);

#if defined VG_FEATURE_FONT_EXTERNAL && (VG_FEATURE_FONT_EXTERNAL == 1)
		if (FT_ERR(Cannot_Open_Resource) == error) {
			// try to load an external resource
			error = __load_external_font(&face, font_name);
		}
#endif // VG_FEATURE_FONT_EXTERNAL

		if ((FT_ERR(Ok) == error) && (!FT_IS_SCALABLE(face) || (0U == face->units_per_EM))) {
			// Every text measure and the drawing divide by the EM size, so a face that declares none
			// is refused here, once, instead of being guarded in each of them.
			VG_LOG_ERROR("Font not scalable: %s", (const char *)font_name);
			__dispose_font((void *)face);
		} else if (FT_ERR(Ok) == error) {
			// Use Unicode
			FT_Select_Charmap(face, ft_encoding_unicode);
			VG_LOG_INFO("Freetype font loaded: %s", (const char *)font_name);

			int32_t registered = SNI_registerResource((void *)face, (SNI_closeFunction) & __dispose_registered_font,
			                                          &__register_font_description);
			assert(SNI_OK == registered);

#if defined VG_FEATURE_FONT_COMPLEX_LAYOUT && (VG_FEATURE_FONT_COMPLEX_LAYOUT == 1)
			if (JTRUE == complex_layout) {
				face->face_flags |= FT_FACE_FLAG_COMPLEX_LAYOUT;
			}
#endif // VG_FEATURE_FONT_COMPLEX_LAYOUT

			ret = (jint)face;
		} else if (FT_ERR(Cannot_Open_Resource) != error) {
			VG_LOG_ERROR("An error occurred during font loading: 0x%x, refer to fterrdef.h", error);
		} else {
			VG_LOG_ERROR("Resource not found: %s", (const char *)font_name);
		}
	}

	LOG_MICROVG_FONT_END(load);
	return ret;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_string_width(jchar *text, jint faceHandle, jfloat size, jfloat letterSpacing) {
	LOG_MICROVG_FONT_START(stringWidth);
	jfloat ret;

	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jfloat)LLVG_RESOURCE_CLOSED;
	} else {
		int length = (int)SNI_getArrayLength(text);
		ret = VG_FREETYPE_string_width(text, length, faceHandle, size, letterSpacing);
	}

	LOG_MICROVG_FONT_END(stringWidth);
	return ret;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_string_height(jchar *text, jint faceHandle, jfloat size) {
	LOG_MICROVG_FONT_START(stringHeight);
	jfloat ret;

	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jfloat)LLVG_RESOURCE_CLOSED;
	} else {
		FT_Face face = (FT_Face)faceHandle;
		float scaled_height = 0.f;

		// This fold is the vertical twin of VG_FREETYPE_string_span(), and it stays separate: it
		// folds boxes where they are loaded rather than at a pen position, so one shared core
		// would have to carry a horizontal axis this function never advances.
		if (size > 0) {
			float scale = VG_FREETYPE_get_scale(faceHandle, size);

			float drawn_y_top = 0.f;
			float drawn_y_bottom = 0.f;
			// Whether a glyph with a non-empty box has been folded in yet: a space carries a
			// real glyph index, so the glyph index cannot be used to seed the first drawn glyph,
			// and neither end can start pinned at 0 without biasing a string that draws entirely
			// on one side of the baseline.
			bool has_drawn_glyph = false;

			// Layout variables
			FT_UInt glyph_index = 0;

			int advance_x = 0;
			int advance_y = 0;
			int offset_x = 0;
			int offset_y = 0;

			// Box the current glyph draws, control points excluded.
			VG_OUTLINE_BOX box = { .x_min = 0.f, .x_max = 0.f, .y_min = 0.f, .y_max = 0.f, .empty = true };

			int length = (int)SNI_getArrayLength(text);
			VG_HELPER_layout_configure(faceHandle, text, length);

			while (VG_HELPER_layout_load_glyph(&glyph_index, &advance_x, &advance_y, &offset_x, &offset_y)) {
				// At that point the current glyph has been loaded by Freetype
				VG_OUTLINE_BOX_compute(&face->glyph->outline, &box);

				// A space has no outline: without this guard its zeroed, empty box would enter
				// the fold below and drag the bottom of the string to 0.
				if (!box.empty) {
					if (!has_drawn_glyph) {
						drawn_y_bottom = box.y_min;
						drawn_y_top = box.y_max;
						has_drawn_glyph = true;
					} else {
						drawn_y_bottom = MIN(box.y_min, drawn_y_bottom);
						drawn_y_top = MAX(box.y_max, drawn_y_top);
					}
				}
			}

			assert(drawn_y_top >= drawn_y_bottom);
			scaled_height = scale * (drawn_y_top - drawn_y_bottom);
		}

		ret = scaled_height;
	}

	LOG_MICROVG_FONT_END(stringHeight);
	return ret;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_get_baseline_position(jint faceHandle, jfloat size) {
	LOG_MICROVG_FONT_START(baseline);
	jfloat ret;

	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jfloat)LLVG_RESOURCE_CLOSED;
	} else {
		FT_Face face = (FT_Face)faceHandle;
		float advance_y = 0.f;

		if (size > 0) {
			float scale = VG_FREETYPE_get_scale(faceHandle, size);
			advance_y = (face->ascender * scale);
		}
		ret = advance_y;
	}

	LOG_MICROVG_FONT_END(baseline);
	return ret;
}

// See the header file for the function documentation
jfloat LLVG_FONT_IMPL_get_height(jint faceHandle, jfloat size) {
	LOG_MICROVG_FONT_START(height);
	jfloat ret;

	if (LLVG_FONT_UNLOADED == faceHandle) {
		ret = (jfloat)LLVG_RESOURCE_CLOSED;
	} else {
		FT_Face face = (FT_Face)faceHandle;
		float scaled_height = 0.f;

		if (size > 0) {
			scaled_height = face->height * VG_FREETYPE_get_scale(faceHandle, size);
		}

		ret = scaled_height;
	}

	LOG_MICROVG_FONT_END(height);
	return ret;
}

// See the header file for the function documentation
void LLVG_FONT_IMPL_dispose(jint faceHandle) {
	// unregister the resource since the VEE does not need to call it anymore
	int32_t unregistered = SNI_unregisterResource((void *)faceHandle, (SNI_closeFunction) & __dispose_registered_font);
	assert(SNI_OK == unregistered);
	__dispose_font((void *)faceHandle);
}

// See the header file for the function documentation
bool LLVG_FONT_IMPL_has_complex_layouter(void) {
#if defined VG_FEATURE_FONT_COMPLEX_LAYOUT && (VG_FEATURE_FONT_COMPLEX_LAYOUT == 1)
	return true;
#else
	return false;
#endif
}

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

static void __dispose_font(void *faceHandle) {
	FT_Face face = (FT_Face)faceHandle;

#if defined VG_FEATURE_FONT_EXTERNAL && (VG_FEATURE_FONT_EXTERNAL == 1)
	// FT_Done_Face() frees the stream of a memory font and only closes the stream this CCO allocated
	// for an external font, so the stream is read here, before it can be freed. Freetype doesn't
	// recommend to read the flag FT_FACE_FLAG_EXTERNAL_STREAM.
	FT_Stream stream = face->stream;
	bool is_own_stream = (&__close_external_resource == stream->close);
#endif // VG_FEATURE_FONT_EXTERNAL

	// The layouter's state holds this face and reads through it for its whole life, so it goes first.
	VG_HELPER_layout_dispose((int)faceHandle);

	FT_Done_Face(face);

#if defined VG_FEATURE_FONT_EXTERNAL && (VG_FEATURE_FONT_EXTERNAL == 1)
	if (is_own_stream) {
		ft_mem_free(library->memory, stream);
	}
#endif // VG_FEATURE_FONT_EXTERNAL
}

/*
 * Only called when killing a KF feature (automatic SNI resource management): have
 * to wait the end of GPU before closing the image.
 */
static void __dispose_registered_font(void *faceHandle) {
	// ensure GPU is not working in our font...
	LLUI_DISPLAY_waitAsynchronousDrawingEnd();
	// ... and close it
	__dispose_font(faceHandle);
}

static FT_Error __load_memory_font(FT_Face *face, void *data, int length) {
	return FT_New_Memory_Face(library, data, length, 0, face);
}

static FT_Error __load_internal_font(FT_Face *face, jchar *font_name) {
	SNIX_resource font_resource;
	FT_Error error;

	if (0 == SNIX_get_resource(font_name, &font_resource)) {
		error = __load_memory_font(face, font_resource.data, font_resource.size);
	} else {
		error = FT_ERR(Cannot_Open_Resource);
	}
	return error;
}

#if defined VG_FEATURE_FONT_EXTERNAL && (VG_FEATURE_FONT_EXTERNAL == 1)

static FT_Error __load_external_font(FT_Face *face, jchar *font_name) {
	FT_Error error;

	// try to load an external resource (respect LLEXT_RES_open path naming convention)
	char const *ext_path = &(((char const *)(font_name))[1]) /* first '/' */;
	RES_ID resource_id = LLEXT_RES_open(ext_path);

	if (0 <= resource_id) {
		int32_t resource_size = LLEXT_RES_available(resource_id);

		// check if the external resource is byte addressable
		int32_t base_address = LLEXT_RES_getBaseAddress(resource_id);

		if (-1 != base_address) {
			// load the font as "memory" font
			// cppcheck-suppress [misra-c2012-11.6] base_address is an address
			error = __load_memory_font(face, (void *)base_address, resource_size);

			// we can close the resource because we know its address
			LLEXT_RES_close(resource_id);
		} else {
			// load the font as external font

			FT_Open_Args args;
			args.flags = FT_OPEN_STREAM;
			args.driver = NULL;
			args.stream = (FT_StreamRec *)ft_mem_alloc(library->memory, sizeof(FT_StreamRec), &error);

			if (FT_ERR(Ok) == error) {
				args.stream->base = NULL;
				args.stream->size = resource_size;
				args.stream->pos = 0;
				args.stream->descriptor.value = resource_id;
				// args.stream->pathname = NULL; // not used
				args.stream->read = &__read_external_resource;
				args.stream->close = &__close_external_resource;

				error = FT_Open_Face(library, &args, 0, face);
			} else {
				error = FT_ERR(Out_Of_Memory);
			}
		}
	} else {
		error = FT_ERR(Cannot_Open_Resource);
	}

	return error;
}

static unsigned long __read_external_resource(FT_Stream stream, unsigned long offset, unsigned char *buffer,
                                              unsigned long count) {
	int32_t size = count;
	RES_ID resource_id = (RES_ID)(stream->descriptor.value);
	LLEXT_RES_seek(resource_id, offset);
	LLEXT_RES_read(resource_id, buffer, &size);
	return size;
}

static void __close_external_resource(FT_Stream stream) {
	RES_ID resource_id = (RES_ID)(stream->descriptor.value);
	LLEXT_RES_close(resource_id);
}

#endif // VG_FEATURE_FONT_EXTERNAL

static void __register_font_description(void *resource, char *buffer, uint32_t bufferLength) {
	(void)resource;
	REGISTERDESC("VectorFont", buffer, bufferLength);
}

// -----------------------------------------------------------------------------
// TestResource functions
// -----------------------------------------------------------------------------

jlong Java_com_microej_microvg_test_TestResource_getSNICloseFunctionForFont(void) {
	return (jlong) & __dispose_registered_font;
}

// cppcheck-suppress [misra-c2012-3.2]
#endif \
    // defined VG_FEATURE_FONT && \
    // (defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
    // (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

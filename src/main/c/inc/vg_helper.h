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
 * @brief MicroEJ MicroVG library low level API: helper to implement library natives
 * methods.
 * @author MicroEJ Developer Team
 * @version 8.0.3
 */

#if !defined VG_HELPER_H
#define VG_HELPER_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <sni.h>

#include "vg_configuration.h"

// -----------------------------------------------------------------------------
// Macros and Defines
// -----------------------------------------------------------------------------

/**
 * @brief Logs an error. The format has no trailing line return.
 *
 * The errors are always compiled. The printer is VG_LOG_ERROR_PRINT() (see vg_configuration.h).
 */
#define VG_LOG_ERROR(fmt, ...) VG_LOG_ERROR_PRINT(fmt, ## __VA_ARGS__)

/**
 * @brief Logs an informative message. The format has no trailing line return.
 *
 * Compiled only when VG_LOG_INFO_ENABLED is 1. The printer is VG_LOG_INFO_PRINT()
 * (see vg_configuration.h).
 */
#if defined VG_LOG_INFO_ENABLED && (VG_LOG_INFO_ENABLED == 1)
#define VG_LOG_INFO(fmt, ...) VG_LOG_INFO_PRINT(fmt, ## __VA_ARGS__)
#else
#define VG_LOG_INFO(fmt, ...)
#endif

/**
 * @brief Deprecated, use VG_LOG_ERROR(). Kept for the Abstraction Layers that build on this one.
 */
#define MEJ_LOG_ERROR_MICROVG(fmt, ...) VG_LOG_ERROR(fmt, ## __VA_ARGS__)

/**
 * @brief Deprecated, use VG_LOG_INFO(). Kept for the Abstraction Layers that build on this one.
 */
#define MEJ_LOG_INFO_MICROVG(fmt, ...) VG_LOG_INFO(fmt, ## __VA_ARGS__)

/**
 * @brief Set this define to monitor freetype heap evolution.
 *        It needs VG_LOG_INFO_ENABLED to print the heap logs.
 */
//#define MICROVG_MONITOR_HEAP

/**
 * @brief NULL Gradient value
 */
#define VG_HELPER_NULL_GRADIENT 0

/**
 * @brief Freetype supplementary flag for complex layout
 *        Uses a free bit in freetype face flags to convey the complex layout mode
 *        information with the freetype face.
 *        freetype.h must be checked on freetype update to ensure that this bit is
 *        still free.
 */
#define FT_FACE_FLAG_COMPLEX_LAYOUT  (((uint32_t)1) << 31)

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#define RAD_TO_DEG(r) ((r) * (180.0f / M_PI))
#define DEG_TO_RAD(d) (((d) * M_PI) / 180.0f)

#define JFLOAT_TO_UINT32_t(f) (*(uint32_t *)&(f))
#define UINT32_t_TO_JFLOAT(i) (*(float *)&(i))

/**
 * @brief Registers the description of a SNI close function.
 */
#define REGISTERDESC(desc, buf, buf_len) if ((buf_len) >= sizeof(desc)) { (void)memcpy((buf), (desc), sizeof(desc)); }

// --------------------------------------------------------------------------------
// UI Pack > 14.5.1 function
// --------------------------------------------------------------------------------

/**
 * @brief Waits until the end of current asynchronous drawing.
 *
 * To avoid potential side effects from the release of objects (images, fonts) retained by
 * a feature during the killing of that feature, ensure that no third-party components
 * (e.g., GPU) are using these objects at the time of the kill.
 *
 * UI packs with versions higher than 14.5.1 provide the blocking API `LLUI_DISPLAY_waitAsynchronousDrawingEnd()`.
 * This API is stubbed on the VG Pack for backward compatibility issues between VG Pack
 * 1.8.0 and UI Packs [14.4.0, 14.5.1]. However, it is highly recommended to use a UI Pack
 * version greater than 14.5.1.
 */
void LLUI_DISPLAY_waitAsynchronousDrawingEnd(void);

// -----------------------------------------------------------------------------
// API
// -----------------------------------------------------------------------------

/**
 * @brief Gets the UTF character from a text buffer at the given offset and updates
 * the offset to point to the next character.
 *
 * Some characters have some special values; they are made up of two Unicode characters
 * in two specific ranges such that the first Unicode character is in one range (for
 * example 0xD800-0xD8FF) and the second Unicode character is in the second range (for
 * example 0xDC00-0xDCFF). This is called a surrogate pair.
 *
 * If a surrogate pair is incomplete (missing second character), this function returns
 * "0" (error) and does not update the offset.
 *
 * @param[in] text: text buffer encoded in UTF16 where to read UTF character.
 * @param[in] length: lenght of the text buffer.
 * @param[in,out] offset: offset in the text buffer where to read UTF character. Updated
 *    to the next character position.
 *
 * @return The decoded UTF character.
 */
int VG_HELPER_get_utf(const unsigned short *text, int length, int *offset);

/**
 * @brief Configures the font layouter with a font and a text
 *
 * Releases what an iteration left open, as VG_HELPER_layout_stop() does, so a caller that stops
 * before the end of its own iteration costs the next caller nothing.
 *
 * @param[in] faceHandle: handle on font face.
 * @param[in] text: text buffer encoded in UTF16 where to read UTF character.
 * @param[in] length: text buffer length.
 *
 */
void VG_HELPER_layout_configure(int faceHandle, const unsigned short *text, int length);

/**
 * @brief Releases the font layouter's state built from a font face, when that face is about to be
 * freed. Does nothing when the layouter holds no state for this face.
 *
 * Must be called before the face itself is freed: the state holds the face and reads through it for
 * its whole life.
 *
 * @param[in] faceHandle: handle on the font face being freed.
 *
 */
void VG_HELPER_layout_dispose(int faceHandle);

/**
 * @brief Loads the next layouted glyph and gets index and positions.
 *
 * @param[out] glyph_idx: next glyph index.
 * @param[out] x_advance: the horizontal advance to add to the cursor position after drawing the glyph.
 * @param[out] y_advance: the vertical advance to add to thecursor after drawing the glyph.
 * @param[out] x_offset: the hozizontal offset of the glyph, does not affect the cursor position.
 * @param[out] y_offset: the vertical offset of the glyph, does not affect the cursor position.
 *
 * @return true if a glyph is available otherwise false.
 */
bool VG_HELPER_layout_load_glyph(uint32_t *glyph_idx, int *x_advance, int *y_advance, int *x_offset, int *y_offset);

/**
 * @brief Releases what the layouter still holds for the current iteration. Does nothing when there
 * is nothing to release, so a caller that may or may not have stopped early calls it either way,
 * and calling it twice is safe.
 *
 * It does not reset the reading position, so the next iteration starts with
 * VG_HELPER_layout_configure() as it always did.
 *
 */
void VG_HELPER_layout_stop(void);

/**
 * @brief Checks if the matrix is null. In that case, returns an identity matrix.
 * This allows to prevent to make some checks on the matrix in the algorithms.
 *
 * The identity matrix can be used several times by the same algorithm. The caller
 * must not modify it (read-only matrix).
 *
 * @param[in] matrix the matrix to check
 *
 * @return the matrix or an identity matrix
 */
const jfloat * VG_HELPER_check_matrix(const jfloat *matrix);

/**
 * @brief Applies the global opacity on given color.
 *
 * @param[in] color: the 32-bit color.
 * @param[in] alpha: the opacity.
 *
 * @return the new color.
 */
uint32_t VG_HELPER_apply_alpha(uint32_t color, uint32_t alpha);

/**
 * @brief Converts a MicroVG matrix in another MicroVG matrix applying a translation.
 *
 * @param[out] dest the MicroVG matrix to set
 * @param[in] x the X translation to apply
 * @param[in] y the Y translation to apply
 * @param[in] matrix the MicroVG transformation to apply (can be null)
 */
void VG_HELPER_prepare_matrix(jfloat *dest, jfloat x, jfloat y, const jfloat *matrix);

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined VG_HELPER_H

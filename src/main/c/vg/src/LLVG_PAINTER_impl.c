/*
 * C
 *
 * Copyright 2023-2026 MicroEJ Corp. All rights reserved.
 * MicroEJ Corp. PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/**
 * @file
 * @brief This file implements all MicroVG drawing native functions.
 * @see LLVG_PAINTER_impl.h file comment
 * @author MicroEJ Developer Team
 * @version 8.0.1
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// implements LLVG_PAINTER_impl functions
#include <LLVG_PAINTER_impl.h>

// use graphical engine functions to synchronize drawings
#include <LLUI_DISPLAY.h>

#include <LLVG_FONT_impl.h>
#include <LLVG_MATRIX_impl.h>

// calls vg_drawing functions
#include "vg_drawing.h"
#include "vg_helper.h"
#include "vg_trace.h"

// -----------------------------------------------------------------------------
// LLVG_PAINTER_impl.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawPath(MICROUI_GraphicsContext *gc, jbyte *pathData, jint x, jint y, jfloat *matrix,
                                jint fillRule, jint blend, jint color) {
	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLVG_PAINTER_IMPL_drawPath)) {
		VG_TRACE_DRAW_START(drawPathColor, gc, x, y);
		jfloat translated_matrix[LLVG_MATRIX_SIZE];
		VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
		DRAWING_Status status = VG_DRAWING_drawPath(gc, pathData, translated_matrix, fillRule, blend, color);
		LLUI_DISPLAY_setDrawingStatus(status);
		VG_TRACE_DRAW_END(drawPathColor, status);
	}
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawGradient(MICROUI_GraphicsContext *gc, jbyte *pathData, jint x, jint y, jfloat *matrix,
                                    jint fillRule, jint alpha, jint blend, jint *gradientData, jfloat *gradientMatrix) {
	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLVG_PAINTER_IMPL_drawGradient)) {
		VG_TRACE_DRAW_START(drawPathGradient, gc, x, y);
		jfloat translated_matrix[LLVG_MATRIX_SIZE];
		VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
		DRAWING_Status status = VG_DRAWING_drawGradient(gc, pathData, translated_matrix, fillRule, alpha, blend,
		                                                gradientData, gradientMatrix);
		LLUI_DISPLAY_setDrawingStatus(status);
		VG_TRACE_DRAW_END(drawPathGradient, status);
	}
	return LLVG_SUCCESS;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawString(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size, jfloat x,
                                  jfloat y, jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing) {
	jint ret;

	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawString))) {
		DRAWING_Status status;
		int length = (int)SNI_getArrayLength(text);
		VG_TRACE_DRAW_START(drawStringColor, gc, length, (jint)x, (jint)y);

		if (LLVG_FONT_UNLOADED != faceHandle) {
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			status = VG_DRAWING_drawString(gc, text, length, faceHandle, size, translated_matrix, alpha, blend,
			                               letterSpacing);
			ret = (jint)LLVG_SUCCESS;
		} else {
			status = DRAWING_DONE;
			ret = (jint)LLVG_RESOURCE_CLOSED;
		}

		LLUI_DISPLAY_setDrawingStatus(status);
		VG_TRACE_DRAW_END(drawStringColor, status);
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawStringGradient(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size,
                                          jfloat x, jfloat y, jfloat *matrix, jint alpha, jint blend,
                                          jfloat letterSpacing, jint *gradientData, jfloat *gradientMatrix) {
	jint ret;

	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawStringGradient))) {
		DRAWING_Status status;
		int length = (int)SNI_getArrayLength(text);
		VG_TRACE_DRAW_START(drawStringGradient, gc, length, (jint)x, (jint)y);

		if (LLVG_FONT_UNLOADED != faceHandle) {
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			status = VG_DRAWING_drawStringGradient(gc, text, length, faceHandle, size,
			                                       translated_matrix, alpha, blend, letterSpacing, gradientData,
			                                       gradientMatrix);
			ret = (jint)LLVG_SUCCESS;
		} else {
			status = DRAWING_DONE;
			ret = (jint)LLVG_RESOURCE_CLOSED;
		}

		LLUI_DISPLAY_setDrawingStatus(status);
		VG_TRACE_DRAW_END(drawStringGradient, status);
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawStringOnCircle(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle, jfloat size,
                                          jint x, jint y, jfloat *matrix, jint alpha, jint blend, jfloat letterSpacing,
                                          jfloat radius, jint direction) {
	jint ret;

	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawStringOnCircle))) {
		DRAWING_Status status;
		int length = (int)SNI_getArrayLength(text);
		VG_TRACE_DRAW_START(drawStringOnCircleColor, gc, length, x, y, (uint32_t)radius, direction);

		if (LLVG_FONT_UNLOADED != faceHandle) {
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			status = VG_DRAWING_drawStringOnCircle(gc, text, length, faceHandle, size,
			                                       translated_matrix, alpha, blend, letterSpacing, radius, direction);
			ret = (jint)LLVG_SUCCESS;
		} else {
			status = DRAWING_DONE;
			ret = (jint)LLVG_RESOURCE_CLOSED;
		}

		LLUI_DISPLAY_setDrawingStatus(status);
		VG_TRACE_DRAW_END(drawStringOnCircleColor, status);
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawStringOnCircleGradient(MICROUI_GraphicsContext *gc, jchar *text, jint faceHandle,
                                                  jfloat size, jint x, jint y, jfloat *matrix, jint alpha, jint blend,
                                                  jfloat letterSpacing, jfloat radius, jint direction,
                                                  jint *gradientData, jfloat *gradientMatrix) {
	jint ret;

	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & (LLVG_PAINTER_IMPL_drawStringOnCircleGradient))) {
		DRAWING_Status status;
		int length = (int)SNI_getArrayLength(text);
		VG_TRACE_DRAW_START(drawStringOnCircleGradient, gc, length, x, y, (uint32_t)radius, direction);

		if (LLVG_FONT_UNLOADED != faceHandle) {
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			status = VG_DRAWING_drawStringOnCircleGradient(gc, text, length, faceHandle, size,
			                                               translated_matrix, alpha, blend, letterSpacing, radius,
			                                               direction, gradientData, gradientMatrix);
			ret = (jint)LLVG_SUCCESS;
		} else {
			status = DRAWING_DONE;
			ret = (jint)LLVG_RESOURCE_CLOSED;
		}

		LLUI_DISPLAY_setDrawingStatus(status);
		VG_TRACE_DRAW_END(drawStringOnCircleGradient, status);
	}

	return ret;
}

// See the header file for the function documentation
jint LLVG_PAINTER_IMPL_drawImage(MICROUI_GraphicsContext *gc, MICROVG_Image *image, jint x, jint y, jfloat *matrix,
                                 jint alpha, jlong elapsed, const float color_matrix[]) {
	jint error = LLVG_SUCCESS;

	if (LLUI_DISPLAY_requestDrawing(gc, (SNI_callback) & LLVG_PAINTER_IMPL_drawImage)) {
		DRAWING_Status status;
		VG_TRACE_DRAW_START(drawImage, gc, VG_TRACE_IMAGE(image), x, y);

		if (!VG_DRAWING_image_is_closed(image) && (alpha > (jint)0)) {
			jfloat translated_matrix[LLVG_MATRIX_SIZE];
			VG_HELPER_prepare_matrix(translated_matrix, x, y, matrix);
			status = VG_DRAWING_drawImage(gc, image, translated_matrix, alpha, elapsed, color_matrix, &error);

			/* FIXME LLVG_OUT_OF_MEMORY errors are not returned as they are reported with error flags.
			 * See M0092MEJAUI-2908.
			 */
			if (LLVG_OUT_OF_MEMORY == error) {
				error = LLVG_SUCCESS;
			}
		} else {
			status = DRAWING_DONE;
		}
		LLUI_DISPLAY_setDrawingStatus(status);
		VG_TRACE_DRAW_END(drawImage, status);
	}
	return error;
}

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

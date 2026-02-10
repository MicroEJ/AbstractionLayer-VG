/*
 * C
 *
 * Copyright 2020-2026 MicroEJ Corp. All rights reserved.
 * MicroEJ Corp. PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/**
 * @file
 * @brief MicroEJ MicroVG library low level API: enable some features according to
 * the hardware capacities.
 * @author MicroEJ Developer Team
 */

#ifndef VG_CONFIGURATION_H
#define VG_CONFIGURATION_H

#if defined __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// Include VEE Port User configuration file
// (already included in ui_configuration.h of UI Pack 14.5.0 and higher)
#if defined __has_include
	#if __has_include("veeport_configuration.h")
		#include "veeport_configuration.h"
	#endif // __has_include("veeport_configuration.h")
#else
// Ensure 'veeport_configuration.h' exists in your project for custom configurations.
	#include "veeport_configuration.h"
#endif // defined __has_include

#include "ui_configuration.h"

// -----------------------------------------------------------------------------
// MicroVG's Path Options
// -----------------------------------------------------------------------------

/*
 * @brief Value of "VG_FEATURE_PATH" to use only one array to store the path's
 * commands and the commands' parameters.
 */
#define VG_FEATURE_PATH_SINGLE_ARRAY (1)

/*
 * @brief Value of "VG_FEATURE_PATH" to use two arrays to store the path's data:
 * one for the commands and one for the commands' parameters.
 */
#define VG_FEATURE_PATH_DUAL_ARRAY (2)

// -----------------------------------------------------------------------------
// MicroVG's LinearGradient Options
// -----------------------------------------------------------------------------

/*
 * @brief Value of "VG_FEATURE_GRADIENT" to use a full implementation of the
 * MicroVG's LinearGradient.
 *
 * This implementation holds an array of all colors and positions set by the
 * application.
 */
#define VG_FEATURE_GRADIENT_FULL (1)

/*
 * @brief Value of "VG_FEATURE_GRADIENT" to use a reduced implementation of the
 * MicroVG's LinearGradient.
 *
 * This implementation keeps only the first color set by the application when
 * inializing a gradient.
 */
#define VG_FEATURE_GRADIENT_FIRST_COLOR (2)

// -----------------------------------------------------------------------------
// MicroVG's VectorFont Options
// -----------------------------------------------------------------------------

/*
 * @brief Value of "VG_FEATURE_FONT" to use FreeTYPE as implementation of the
 * MicroVG's VectorFont.
 *
 * This implementation configures FreeTYPE to manipulate the font's glyphs as a
 * succession of vector paths. It requires a "vector" renderer.
 */
#define VG_FEATURE_FONT_FREETYPE_VECTOR (1)

/*
 * @brief Value of "VG_FEATURE_FONT" to use FreeTYPE as implementation of the
 * MicroVG' VectorFont.
 *
 * This implementation configures FreeTYPE to manipulate the font's glyphs as
 * bitmaps. No renderer is required
 */
#define VG_FEATURE_FONT_FREETYPE_BITMAP (2)

// -----------------------------------------------------------------------------
// MicroVG's Features Configuration
// -----------------------------------------------------------------------------

/*
 * @brief Set this define to embed the implementation of the MicroVG's
 * Path (dynamic path creation and path rendering).
 *
 * This implementation holds the path's commands and paramters defined by the
 * application. The define value specifies how this data is stored (one or two arrays).
 *
 * When not set, a stub implementation is used. No error is thrown at runtime when
 * the application uses a path: the dynamic path are not created and the path
 * rendering is not performed.
 */
#ifndef VG_FEATURE_PATH
#define VG_FEATURE_PATH VG_FEATURE_PATH_SINGLE_ARRAY
#endif

/*
 * @brief Set this define to specify the implementation of the MicroVG's
 * LinearGradient (dynamic gradient creation and drawings with gradient).
 *
 * @see VG_FEATURE_GRADIENT_FULL
 * @see VG_FEATURE_GRADIENT_FIRST_COLOR
 *
 * When not set, a stub implementation is used. No error is thrown at runtime when
 * the application uses a gradient: the dynamic gradient are not created and the
 * gradient rendering is not performed.
 */
#ifndef VG_FEATURE_GRADIENT
#define VG_FEATURE_GRADIENT VG_FEATURE_GRADIENT_FULL
#endif

/*
 * @brief Set this define to specify the implementation of the MicroVG'
 * VectorFont (dynamic font loading and text rendering).
 *
 * @see VG_FEATURE_FONT_FREETYPE_VECTOR
 * @see VG_FEATURE_FONT_FREETYPE_BITMAP
 *
 * When not set, a stub implementation is used. The application cannot load a font
 * (and by consequence cannot draw a text).
 */
#ifndef VG_FEATURE_FONT
#define VG_FEATURE_FONT VG_FEATURE_FONT_FREETYPE_VECTOR
#endif

/**
 * @brief Set this define to 1 to enable the support of TTF font files (disabled by default).
 */
#ifndef VG_FEATURE_FREETYPE_TTF
#define VG_FEATURE_FREETYPE_TTF (0)
#endif

/**
 * @brief Set this define to 1 to enable the support of OTF font files (disabled by default).
 */
#ifndef VG_FEATURE_FREETYPE_OTF
#define VG_FEATURE_FREETYPE_OTF (0)
#endif

/*
 * @brief Uncomment this define to enable the support of colored emoji.
 */
#ifndef VG_FEATURE_FREETYPE_COLORED_EMOJI
#define VG_FEATURE_FREETYPE_COLORED_EMOJI (0)
#endif

/*
 * @brief Set this define to 1 to enable the support of complex layout (disabled by default).
 *
 * When set, the complex layout feature is disabled by default (the Freetype layout
 * manager is used). See functions MICROVG_HELPER_set_complex_layout() and
 * MICROVG_HELPER_has_complex_layouter().
 *
 * Note: the complex layout feature is managed by the Harfbuzz engine.
 *       Harfbuzz is used beside Freetype, thus the FT_CONFIG_OPTION_USE_HARFBUZZ define
 *       is not needed. This implementation has been chosen to ease the replacement of
 *       harfbuzz by an other complex layouter.
 */
#ifndef VG_FEATURE_FONT_COMPLEX_LAYOUT
#define VG_FEATURE_FONT_COMPLEX_LAYOUT (0)
#endif

/*
 * @brief Set this define to 1 to enable the support of external font files (disabled by default).
 *
 * When a font file is not available in the application classpath, the implementation tries to
 * load it from an external resource system.
 *
 * The VEE Port must embed the module "External Resource" and the BSP must implement
 * "LLEXT_RES_impl.h" header file.
 *
 * When not set, only the resources compiled with the application are used.
 */
#ifndef VG_FEATURE_FONT_EXTERNAL
#define VG_FEATURE_FONT_EXTERNAL (0)
#endif

/*
 * @brief Configure this define to set the freetype heap size.
 *
 * The freetype heap size depends on the font used by the application
 * @see MICROVG_MONITOR_HEAP in vg_helper.h to monitor the heap usage evolution.
 */
#ifndef VG_FEATURE_FREETYPE_HEAP_SIZE
#define VG_FEATURE_FREETYPE_HEAP_SIZE (80 * 1024)
#endif

/*
 * @brief Configure this define to set the complex layouter heap size.
 *
 * @see VG_FEATURE_FONT_COMPLEX_LAYOUT
 *
 * The complex layouter heap size depends on the font used by the application
 * @see MICROVG_MONITOR_HEAP in vg_helper.h to monitor the heap usage evolution.
 */
#if VG_FEATURE_FONT_COMPLEX_LAYOUT == 1
#define VG_FEATURE_FONT_COMPLEX_LAYOUT_HEAP_SIZE (80 * 1024)
#endif

#if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

/*
 * @brief Set this define to 1 to enable the support of MicroVG BufferedVectorImage
 * (disabled by default).
 *
 * This feature requires the available number of supported GraphicsContext formats
 * is higher than 1.
 */
#ifndef VG_FEATURE_BUFFERED_VECTOR_IMAGE
#define VG_FEATURE_BUFFERED_VECTOR_IMAGE (0)
#endif

/*
 * @brief The drawing functions to target the BufferedVectorImage have by default the
 * identifier 1.
 */
#ifndef UI_DRAWING_IDENTIFIER_BVI_FORMAT
#define UI_DRAWING_IDENTIFIER_BVI_FORMAT 1
#endif

#elif defined(VG_FEATURE_BUFFERED_VECTOR_IMAGE) && (VG_FEATURE_BUFFERED_VECTOR_IMAGE == 1)
#error "The BufferedVectorImage feature requires the support of several Graphics Context formats".
#endif // if defined(UI_GC_SUPPORTED_FORMATS) && (UI_GC_SUPPORTED_FORMATS > 1)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined VG_CONFIGURATION_H

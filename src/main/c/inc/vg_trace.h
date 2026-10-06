/*
 * C
 *
 * Copyright 2023-2026 MicroEJ Corp. All rights reserved.
 * MicroEJ Corp. PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#if !defined VG_TRACE_H
#define VG_TRACE_H

#if defined __cplusplus
extern "C" {
#endif

/**
 * @brief Provides elements that allow to trace some external events in the MicroVG trace
 * group.
 *
 * The MicroVG trace group is identified by the global LLVG_TRACE_group.
 *
 * Notes:
 * - The first event index is the event 10 (events [0,9] are reserved for MicroVG).
 * - The number of events is 30 (fixed by MicroVG): [10,39].
 *
 * Example:
 *
 * 		#include "vg_trace.h"
 * 		LLTRACE_record_event_u32(LLVG_TRACE_group, MY_EVENT_OFFSET, my_event_data);
 *
 * @author MicroEJ Developer Team
 * @version 8.0.3
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// include "ui_log.h" for bakcward compatibility with UI Pack [14.4.0-14.5.1]
#include "ui_log.h"

// -----------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------

/*
 * Events identifiers
 */
#define LOG_MICROVG_FONT_ID 1

/**
 * @brief Types of Font events
 */
#define LOG_MICROVG_FONT_load 0
#define LOG_MICROVG_FONT_baseline 1
#define LOG_MICROVG_FONT_height 2
#define LOG_MICROVG_FONT_stringWidth 3
#define LOG_MICROVG_FONT_stringHeight 4

/**
 *  @brief Identifies the traces used by LLVG_PAINTER_impl.c
 */
#define VG_TRACE_drawPathColor 10
#define VG_TRACE_drawPathGradient 11
#define VG_TRACE_drawStringColor 12
#define VG_TRACE_drawStringGradient 13
#define VG_TRACE_drawStringOnCircleColor 14
#define VG_TRACE_drawStringOnCircleGradient 15
#define VG_TRACE_drawImage 16

/**
 * @brief Useful macros to concatenate easily some strings and defines.
 */
#define CONCAT_STRINGS(p, s) p ## s
#define CONCAT_DEFINES(p, s) CONCAT_STRINGS(p, s)

/**
 * @brief Macro UI_TRACE_IMAGE is only available in UI Pack >= 14.5.2
 */
#ifndef UI_TRACE_IMAGE
#define UI_TRACE_IMAGE UI_LOG_BUFFER
#endif

/**
 * @brief Macro to add an event and its type.
 */
#define LOG_MICROVG_START(event, type) LLTRACE_record_event_u32(LLVG_TRACE_group, event, type);

/**
 * @brief Macro to notify the end of an event and its type.
 */
#define LOG_MICROVG_END(event, type) LLTRACE_record_event_end_u32(LLVG_TRACE_group, event, type);

/**
 * @brief Macros to call the trace functions.
 * Macro UI_TRACE_COUNT_ARGS is only available in UI Pack >= 14.5.2
 */
#ifndef UI_TRACE_COUNT_ARGS
#define UI_TRACE_COUNT_ARGS UI_LOG_COUNT_ARGS
#endif
#define VG_TRACE_FUNCTION(...) CONCAT(LLTRACE_record_event_u32x, UI_TRACE_COUNT_ARGS(__VA_ARGS__))
#define VG_TRACE_OFFSET(fn) CONCAT(VG_TRACE_, fn)
#define VG_TRACE_PARAMS(fn, ...) LLVG_TRACE_group, VG_TRACE_OFFSET(fn), __VA_ARGS__

/**
 * @brief Add the image's address as a trace
 */
#define VG_TRACE_IMAGE(img) ((uint32_t)((img)->data))

/**
 * @brief Starts a VG trace (at least one parameter is required)
 */
#define VG_TRACE_START(fn, ...) VG_TRACE_FUNCTION(__VA_ARGS__)(VG_TRACE_PARAMS(fn, __VA_ARGS__))

/**
 * @brief Ends a VG trace (one parameter is required)
 */
#define VG_TRACE_END(fn, v) LLTRACE_record_event_end_u32(VG_TRACE_PARAMS(fn, v))

/**
 * @brief Starts a VG trace that denotes a drawing (destination "gc" is required)
 */
#define VG_TRACE_DRAW_START(fn, gc, ...) VG_TRACE_START(fn, UI_TRACE_IMAGE(&gc->image), __VA_ARGS__)

/**
 * @brief Ends a VG trace that denotes a drawing
 */
#define VG_TRACE_DRAW_END VG_TRACE_END

// -----------------------------------------------------------------------------
// Fields
// -----------------------------------------------------------------------------

/**
 * @brief Identifies the MicroVG group to trace an event.
 */
extern int32_t LLVG_TRACE_group;

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // !defined VG_TRACE_H

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

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "vg_configuration.h"

#if defined VG_FEATURE_FONT &&                                                              \
	(defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
	(VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

#include <float.h>
#include <math.h>

#include "vg_outline_box.h"
#include "ui_util.h"

// -----------------------------------------------------------------------------
// Internal functions
// -----------------------------------------------------------------------------

static void __add_point(VG_OUTLINE_BOX *box, float x, float y) {
	box->x_min = MIN(box->x_min, x);
	box->x_max = MAX(box->x_max, x);
	box->y_min = MIN(box->y_min, y);
	box->y_max = MAX(box->y_max, y);
	box->empty = false;
}

static void __add_axis_value(VG_OUTLINE_BOX *box, float v, bool is_x) {
	if (is_x) {
		box->x_min = MIN(box->x_min, v);
		box->x_max = MAX(box->x_max, v);
	} else {
		box->y_min = MIN(box->y_min, v);
		box->y_max = MAX(box->y_max, v);
	}
}

static float __quad_at(float p0, float p1, float p2, float t) {
	float u = 1.f - t;
	return (u * u * p0) + (2.f * u * t * p1) + (t * t * p2);
}

static float __cubic_at(float p0, float p1, float p2, float p3, float t) {
	float u = 1.f - t;
	return (u * u * u * p0) + (3.f * u * u * t * p1) + (3.f * u * t * t * p2) + (t * t * t * p3);
}

static void __quad_axis(VG_OUTLINE_BOX *box, float p0, float p1, float p2, float lo, float hi,
                        bool is_x) {
	if ((p1 >= lo) && (p1 <= hi)) {
		return; // a curve stays within the hull of its points, so it cannot leave the box
	}
	/*
	 * d is an integer combination of outline coordinates, so it is either exactly 0 or at least 1
	 * in magnitude: comparing it to 0.f is exact, never a near-miss needing a tolerance.
	 */
	float d = (p0 - (2.f * p1)) + p2;
	if (0.f == d) {
		return;
	}
	float t = (p0 - p1) / d;
	if ((t > 0.f) && (t < 1.f)) {
		__add_axis_value(box, __quad_at(p0, p1, p2, t), is_x);
	}
}

static void __cubic_axis(VG_OUTLINE_BOX *box, float p0, float p1, float p2, float p3, float lo,
                         float hi, bool is_x) {
	if ((p1 >= lo) && (p1 <= hi) && (p2 >= lo) && (p2 <= hi)) {
		return; // a curve stays within the hull of its points, so it cannot leave the box
	}
	float a = ((-p0 + (3.f * p1)) - (3.f * p2)) + p3;
	float b = ((2.f * p0) - (4.f * p1)) + (2.f * p2);
	float c = -p0 + p1;
	float roots[2];
	int nb = 0;
	if (0.f == a) {
		if (0.f != b) {
			roots[nb] = -c / b;
			nb++;
		}
	} else {
		float disc = (b * b) - (4.f * a * c);
		if (disc >= 0.f) {
			float sq = sqrtf(disc);
			roots[nb] = (-b + sq) / (2.f * a);
			nb++;
			roots[nb] = (-b - sq) / (2.f * a);
			nb++;
		}
	}
	for (int i = 0; i < nb; i++) {
		float t = roots[i];
		if ((t > 0.f) && (t < 1.f)) {
			__add_axis_value(box, __cubic_at(p0, p1, p2, p3, t), is_x);
		}
	}
}

// -----------------------------------------------------------------------------
// FT_Outline_Decompose callbacks
// -----------------------------------------------------------------------------

struct outline_walk {
	VG_OUTLINE_BOX *box;
	FT_Vector current;
};

static int __move_to(const FT_Vector *to, void *user) {
	struct outline_walk *w = (struct outline_walk *)user;
	__add_point(w->box, (float)to->x, (float)to->y);
	w->current = *to;
	return 0;
}

static int __line_to(const FT_Vector *to, void *user) {
	struct outline_walk *w = (struct outline_walk *)user;
	__add_point(w->box, (float)to->x, (float)to->y);
	w->current = *to;
	return 0;
}

static int __conic_to(const FT_Vector *control, const FT_Vector *to, void *user) {
	struct outline_walk *w = (struct outline_walk *)user;
	__add_point(w->box, (float)to->x, (float)to->y);
	__quad_axis(w->box, (float)w->current.x, (float)control->x, (float)to->x, w->box->x_min,
	            w->box->x_max, true);
	__quad_axis(w->box, (float)w->current.y, (float)control->y, (float)to->y, w->box->y_min,
	            w->box->y_max, false);
	w->current = *to;
	return 0;
}

static int __cubic_to(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to,
                      void *user) {
	struct outline_walk *w = (struct outline_walk *)user;
	__add_point(w->box, (float)to->x, (float)to->y);
	__cubic_axis(w->box, (float)w->current.x, (float)control1->x, (float)control2->x, (float)to->x,
	             w->box->x_min, w->box->x_max, true);
	__cubic_axis(w->box, (float)w->current.y, (float)control1->y, (float)control2->y, (float)to->y,
	             w->box->y_min, w->box->y_max, false);
	w->current = *to;
	return 0;
}

// -----------------------------------------------------------------------------
// vg_outline_box.h functions
// -----------------------------------------------------------------------------

// See the header file for the function documentation
void VG_OUTLINE_BOX_compute(const FT_Outline *outline, VG_OUTLINE_BOX *box) {
	box->x_min = FLT_MAX;
	box->x_max = -FLT_MAX;
	box->y_min = FLT_MAX;
	box->y_max = -FLT_MAX;
	box->empty = true;

	static const FT_Outline_Funcs funcs = {
		.move_to = __move_to,
		.line_to = __line_to,
		.conic_to = __conic_to,
		.cubic_to = __cubic_to,
		.shift = 0,
		.delta = 0,
	};

	struct outline_walk walk;
	walk.box = box;
	walk.current.x = 0;
	walk.current.y = 0;
	int error = FT_Outline_Decompose((FT_Outline *)outline, &funcs, &walk);
	if ((0 != error) || box->empty) {
		// a malformed outline may have pushed points in before FreeType detected it: undo that
		// partial walk, so the caller sees "nothing drawn" and not a truncated box
		box->x_min = 0.f;
		box->x_max = 0.f;
		box->y_min = 0.f;
		box->y_max = 0.f;
		box->empty = true;
	}
}

// cppcheck-suppress [misra-c2012-3.2]
#endif \
    // defined VG_FEATURE_FONT && \
    // (defined VG_FEATURE_FONT_FREETYPE_VECTOR || defined VG_FEATURE_FONT_FREETYPE_BITMAP) && \
    // (VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_VECTOR || VG_FEATURE_FONT == VG_FEATURE_FONT_FREETYPE_BITMAP)

// -----------------------------------------------------------------------------
// EOF
// -----------------------------------------------------------------------------

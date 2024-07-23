# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [6.0.1] - 2024-07-23

- Compatible with MicroEJ VG Pack 1.6.x.

### Fixed

- Fix the signature of `LLVG_BVI_IMPL_map_context`.

## [6.0.0] - 2024-07-19

- Compatible with MicroEJ VG Pack 1.6.x.

### Added

- Add the API `VG_HELPER_prepare_matrix` that allows to translate the application matrix with the drawing destination point.
- Add the `drawString` painter over Freetype with vector backend.
    - Add `VG_FREETYPE_initialize`.
    - Add `VG_FREETYPE_draw_string`.
    - Add `VG_FREETYPE_IMPL_convert_color`.
- Add `LLVG_GRADIENT_impl_stub.c`: generic implementation of gradient "single color".
- Add the implementation of `LLVG_MATRIX_IMPL_transformPoint()`.
- Add the API `VG_DRAWING_initialize`, called during the initialization step.
- Add the API `VG_DRAWING_image_is_closed`.
- Add dual-array mode for paths, storing commands and parameters separately. Define the constant `VG_FEATURE_PATH` to `VG_FEATURE_PATH_DUAL_ARRAY` to use it.
    - Add `VG_PATH_get_path_param_begin`.
    - Add `VG_PATH_get_path_param_end`.
    - Add `VG_PATH_get_path_command_begin`.
    - Add `VG_PATH_get_path_command_end`.
- Add macros for mapping the following buffered vector image functions:
    - `UI_DRAWING_BVI_drawImage`.
    - `UI_DRAWING_BVI_drawFlippedImage`.
    - `UI_DRAWING_BVI_drawRotatedImageNearestNeighbor`.
    - `UI_DRAWING_BVI_drawRotatedImageBilinear`.
    - `UI_DRAWING_BVI_drawScaledImageNearestNeighbor`.
    - `UI_DRAWING_BVI_drawScaledImageBilinear`.

### Changed

- Rename files:
    - `microvg_configuration.h` becomes `vg_configuration.h`.
    - `LLVG_BVI_stub.c` becomes `LLVG_BVI_impl_stub.c`.
    - `LLVG_FONT_freetype.c` becomes `LLVG_FONT_impl_freetype.c`.
    - `LLVG_FONT_stub.c` becomes `LLVG_FONT_impl_stub.c`.
    - `LLVG_PATH_impl.c` becomes `LLVG_PATH_impl_single.c` and `LLVG_PATH_impl_dual.c`.
    - `LLVG_PATH_stub.c` becomes `LLVG_PATH_impl_stub.c`.
    - `microvg_helper.h` becomes `vg_helper.h`.
    - `microvg_trace.h` becomes `vg_trace.h`.
    - `microvg_helper.c` becomes `vg_helper.c`.
    - `microvg_path.h` becomes `vg_path.h`.
    - `microvg_font_freetype.h` becomes `vg_freetype.h`.
- Rename functions:
    - `MICROVG_PATH_get_path_encoder_format` becomes `VG_PATH_get_path_encoder_format`.
    - `MICROVG_PATH_convert_path_command` becomes `VG_PATH_convert_path_command`.
    - `MICROVG_PATH_initialize` becomes `VG_PATH_initialize`.
    - `MICROVG_PATH_get_path_header_size` becomes `VG_PATH_get_path_header_size`.
    - `MICROVG_PATH_get_path_command_size` becomes `VG_PATH_get_path_command_size`.
    - `MICROVG_PATH_get_command_parameter_number` becomes `VG_PATH_get_command_parameter_number`.
    - `MICROVG_PATH_append_path_command0` becomes `VG_PATH_append_path_command0`.
    - `MICROVG_PATH_append_path_command1` becomes `VG_PATH_append_path_command1`.
    - `MICROVG_PATH_append_path_command2` becomes `VG_PATH_append_path_command2`.
    - `MICROVG_PATH_append_path_command3` becomes `VG_PATH_append_path_command3`.
    - `MICROVG_HELPER_initialize` becomes `VG_HELPER_initialize`.
    - `MICROVG_HELPER_get_utf` becomes `VG_HELPER_get_utf`.
    - `MICROVG_HELPER_layout_configure` becomes `VG_HELPER_layout_configure`.
    - `MICROVG_HELPER_layout_load_glyph` becomes `VG_HELPER_layout_load_glyph`.
    - `MICROVG_HELPER_check_matrix` becomes `VG_HELPER_check_matrix`.
    - `MICROVG_HELPER_apply_alpha` becomes `VG_HELPER_apply_alpha`.
- Change the signature of all drawing functions: remove the parameters `x` and `y` (not backward compatible).
- Change the behavior of `VG_HELPER_initialize()`: it initializes the CCO according to the configuration of `vg_configuration.h`.
- Change the version of `MICROVG_CONFIGURATION_VERSION` (3).
- Decrease the Freetype's heap default size (CCO Freetype 3.0.0 requires less memory).

### Fixed

- Fix the list of MicroUI drawing functions in a buffered vector image.

### Removed

- Remove the file `LLVG_impl.c`: the function `LLVG_IMPL_initialize()` must be explicitly implemented by each backend.
- Remove the generic implementation of linear gradients (`microvg_gradient.h` and `LLVG_GRADIENT_impl.c`): the implementation is GPU-specific.
    - Remove `MICROVG_GRADIENT_get_gradient_scale_size`.
    - Remove `MICROVG_GRADIENT_get_gradient_header_size`.
- Remove the generic implementation of `Java_ej_microvg_VectorGraphicsNatives_getImage`: the implementation is GPU-specific.

## [5.0.0] - 2024-02-15

- Compatible with MicroEJ VG Pack 1.5.x.

### Added

- Add the API that allows to fix the release of the BVI resources.
- Add the API that allows to translate the application matrix with the drawing destination point.
- Add the `drawString` painter over Freetype with vector backend.

### Changed

- Make the CCO compatible with VG pack 1.5.0 and higher.
- Make the CCO compatible with the UI Pack 14.0.0 (CCO MicroUI 4.0.0).
- Change the signature of all drawing functions: remove the parameters `x` and `y` (not backward compatible).

### Fixed

- Fix traces when debugging the SNI resources with external resource support.
- Remove an unused include.
- Do not define Freetype variables if `VG_FEATURE_FONT` is not defined.
- Do not call `MICROVG_PATH_initialize` if `VG_FEATURE_PATH` is not defined.

## [4.0.0] - 2023-11-13

- Compatible with MicroEJ VG Pack 1.4.2.

### Added

- Add some traces when debugging the SNI resources (external VectorFont).

### Changed

- Make the CCO compatible with VG pack 1.4.2 and higher.

### Fixed

- Fix dynamic paths larger than 64 KB (not backward compatible).
- Fix some comments.

## [3.0.1] - 2023-07-21

- Compatible with MicroEJ VG Pack 1.4.x.

### Changed

- Make the CCO compatible with VG pack 1.4.0 and higher.

### Fixed

- Fix the SystemView log identifiers.
- Fix the documentation of `MICROVG_HELPER_get_utf()`.
- Fix FreeType fonts closing twice.

## [3.0.0] - 2023-05-09

- Compatible with MicroEJ VG Pack 1.3.x.

### Added

- Add the compatibility with multiple Graphics Context output formats (UI Pack 13.5.0).
- Add stub implementations for all MicroVG library algorithms.
- Add `LLVG_PAINTER_impl.c` to implement all MicroVG drawings and dispatch them to `vg_drawing.h` (like MicroUI and `LLUI_PAINTER_impl.c` / `ui_drawing.h`).
- Add the MicroVG BufferedVectorImage definition (the functions to implement to draw into it).

### Changed

- Make the CCO compatible with VG pack 1.3.0 and higher.
- CCO MicroVG now depends on CCO MicroUI (to manage the support of multiple Graphics Context output formats).

### Fixed

- Remove an extraneous file.
- Fix issue when measuring string width in complex layout mode.

### Removed

- Remove the useless implementation of `LLVG_PATH_IMPL_mergePaths` (useless since VG Pack 1.2).
- Remove partial Freetype implementation that manipulates the font's glyphs as bitmaps (not compatible anymore with VG pack 1.3.0).

## [2.1.0] - 2022-12-30

- Compatible with MicroEJ VG Pack 1.2.x.

### Added

- Add `LLVG_MATRIX_IMPL_multiply(c,a,b)` (C = AxB): faster than `setConcat` when destination and source target the same matrix.
- Add an entry point to initialize the path engine on startup.

### Changed

- Make the CCO compatible with VG pack 1.2.0 and higher.
- Prevent a copy in a temp matrix when calling `postXXX` functions.

### Fixed

- Fix `A.setConcat(B,A)`

## [2.0.0] - 2022-09-02

- Compatible with MicroEJ VG Pack 1.1.x.

### Added

- Add an option to load a font from the external resources.
- Configure freetype from microvg_configuration.h header file.
- Add microvg_configuration.h versionning.
- Add an option to select the text layouter between Freetype and Harfbuzz.
- Add a function to apply an opacity on a color.

### Changed

- Make the CCO compatible with VG pack 1.1.0 and higher.
- Manage the closed fonts.
- Use mej_log.h global log library for console logs.


## [1.0.0] - 2022-05-13

### Added

- CCO creation.
- Initial revision based on `com.microej.clibrary.llimpl#vector-font#1.0.0`.
- Add UTF16 characters parsing in `LLVG_FONT_IMPL_string_width()`.

---
_Copyright 2021-2024 MicroEJ Corp. All rights reserved._\
_Use of this source code is governed by a BSD-style license that can be found with this software._

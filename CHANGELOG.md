# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [5.0.0] - 2024-02-15

- Compatible with MicroEJ VG Pack 1.5.x.

### Added

  - Add the API that allows to fix the release of the BVI resources.

### Changed

  - Make the CCO compatible with VG pack 1.5.0 and higher.
  - Make the CCO compatible with the UI Pack 14.0.0 (CCO MicroUI 4.0.0).

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
_Copyright 2021-2024 MicroEJ Corp. All rights reserved._  
_Use of this source code is governed by a BSD-style license that can be found with this software._  

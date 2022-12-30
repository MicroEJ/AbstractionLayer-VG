# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [2.1.0] - 2022-12-30

### Added

  - Add `LLVG_MATRIX_IMPL_multiply(c,a,b)` (C = AxB): faster than `setConcat` when destination and source target the same matrix.  
  - Add an entry point to initialize the path engine on startup.

### Changed

  - Prevent a copy in a temp matrix when calling `postXXX` functions. 

### Fixed

  - Fix `A.setConcat(B,A)` 

## [2.0.0] - 2022-09-02

### Added

  - Add an option to load a font from the external resources.
  - Configure freetype from microvg_configuration.h header file.
  - Add microvg_configuration.h versionning.
  - Add an option to select the text layouter between Freetype and Harfbuzz.
  - Add a function to apply an opacity on a color.

### Changed

  - Manage the closed fonts.
  - Use mej_log.h global log library for console logs.


## [1.0.0] - 2022-05-13

### Added

  - CCO creation.
  - Initial revision based on `com.microej.clibrary.llimpl#vector-font#1.0.0`.
  - Add UTF16 characters parsing in `LLVG_FONT_IMPL_string_width()`.
 
---
_Copyright 2021-2022 MicroEJ Corp. All rights reserved._  
_Use of this source code is governed by a BSD-style license that can be found with this software._  

![ARCH](https://shields.microej.com/endpoint?url=https://repository.microej.com/packages/badges/arch_7.16.json)

# Overview

This Abstraction Layer implements the MicroVG Low Level API, described in [LLVG: Vector Graphics](https://docs.microej.com/en/latest/VEEPortingGuide/appendix/llapi.html#llvg-vectorgraphics): paths, gradients, matrices, vector fonts and images.
It does not target a GPU: a GPU Abstraction Layer provides the drawings by overriding its `weak` functions, and a drawing that no GPU Abstraction Layer provides is logged, not rendered.
See the [MicroVG](https://docs.microej.com/en/latest/VEEPortingGuide/vgCco.html#section-vg-c-module-microvg) section of the MicroEJ documentation.

This Abstraction Layer is tied to the MicroEJ VG Pack: the versions it is compatible with are listed in the [compatibility](https://docs.microej.com/en/latest/VEEPortingGuide/vgReleaseNotes.html#c-modules-compatibility-version) tables of the documentation.
The badge above gives the oldest MicroEJ Architecture it supports, not the latest one: the [MicroEJ Architecture Compatibility Version](https://docs.microej.com/en/latest/VEEPortingGuide/uiReleaseNotes.html#microej-architecture-compatibility-version) tables list the Architectures of each UI Pack version.

# Usage

1. Add the sources of this Abstraction Layer to the BSP of the VEE Port, in either of these two ways:

   - From the MicroEJ repository: install the `.cco` archive of the version the VEE Port needs, as the [installation guide](https://docs.microej.com/en/latest/VEEPortingGuide/appendix/cmodules.html) describes.
     The headers land in `bsp/vee/port/vg/inc` and the sources in `bsp/vee/port/vg/src`.
   - From GitHub: add the [GitHub repository](https://github.com/MicroEJ/AbstractionLayer-VG) to the BSP of the VEE Port as a git submodule, at the tag of the version the VEE Port needs.
     The VEE Port chooses the folder of the submodule, and either of these two layouts works.
     A flat folder sits beside the other components of the BSP:

     ```sh
     git submodule add https://github.com/MicroEJ/AbstractionLayer-VG.git bsp/vee/port/vg
     git -C bsp/vee/port/vg checkout <version>
     ```

     A folder grouped by domain sits under the folder of the other components of the same domain:

     ```sh
     git submodule add https://github.com/MicroEJ/AbstractionLayer-VG.git bsp/vee/port/vg/VG
     git -C bsp/vee/port/vg/VG checkout <version>
     ```

     The headers then sit in `<folder>/src/main/c/inc` and the sources in `<folder>/src/main/c/src`, where `<folder>` is the folder of the submodule.
     A copy of these sources in the VEE Port repository works as well.

   Then add the folder of the headers to the include paths of the BSP project, and the files of the folder of the sources to its sources.

2. The configuration file [vg_configuration.h](src/main/c/inc/vg_configuration.h) stores the default values of the Abstraction Layer options.
   To change an option, define it in the file `veeport_configuration.h` of the VEE Port, which `vg_configuration.h` includes first.

3. When the VEE Port moves to another version of this Abstraction Layer, follow the [MicroVG migration guide](https://docs.microej.com/en/latest/VEEPortingGuide/vgMigrationGuide.html).

# Requirements

None.

# Validation

This Abstraction Layer is tested on the following boards:

- MIMXRT595-EVK.
- STM32U5G9J-DK2.
- Linux.

# MISRA Compliance

This Abstraction Layer is MISRA-compliant (MISRA C:2012) with some noted exceptions.
It has been verified with Cppcheck v2.13.
Here is the list of deviations from the MISRA standard:

| Deviation  | Category  | Justification                                                                                                                                   |
|:----------:|:---------:|:----------------------------------------------------------------------------------------------------------------------------------------------- |
| Rule 2.2   | Required  | Indexes of 0 are used for a better understanding.                                                                                               |
| Rule 2.5   | Advisory  | A macro can be defined at API level and not used by the application.                                                                            |
| Rule 3.2   | Required  | The `//` comments after an `#endif` restate the condition of its `#if`, which spans several lines joined by a backslash.                        |
| Rule 5.9   | Advisory  | The same static function name can be used in several C files.                                                                                   |
| Rule 8.2   | Required  | Functions with no parameters are declared with `(void)`.                                                                                        |
| Rule 8.4   | Required  | The compatible declarations are the UI Pack's and VG Pack's functions.                                                                          |
| Rule 8.6   | Required  | Wrong detection: the alternative implementations of a Low Level API define the same functions under mutually exclusive preprocessor conditions. |
| Rule 8.7   | Advisory  | The LLAPI functions require external linkage, but cppcheck cannot detect their invocation by the Pack.                                          |
| Rule 11.1  | Required  | The pointer conversion stores the drawer.                                                                                                       |
| Rule 11.3  | Required  | Cast to a path parameter structure is valid.                                                                                                    |
| Rule 11.6  | Required  | The integer cast to `(void *)` holds an address.                                                                                                |
| Rule 13.3  | Advisory  | There is no side effect.                                                                                                                        |
| Rule 17.3  | Mandatory | The implicitly declared functions are the UI Pack's and VG Pack's functions.                                                                    |
| Rule 18.4  | Advisory  | Pointer arithmetic can be used in a configurable C library.                                                                                     |
| Rule 18.8  | Required  | The size of the arrays is a constant defined by a macro.                                                                                        |
| Rule 20.10 | Advisory  | The `#` and `##` operators are used by MicroEJ Architectures.                                                                                   |
| Rule 21.6  | Required  | Required to use `printf`.                                                                                                                       |

# Dependencies

- The MicroUI Abstraction Layer.
- The FreeType Abstraction Layer, for the vector fonts.
- The HarfBuzz Abstraction Layer, for the complex text layout.
- A GPU Abstraction Layer to render the drawings: MicroVG over NemaVG, or MicroVG over VGLite.

# Changelog

The changes of this Abstraction Layer are listed in the [public changelog of the MicroEJ VG Pack](https://docs.microej.com/en/latest/VEEPortingGuide/vgChangeLog.html), section `C Module MicroVG`.

# Source

- `vg_configuration.h`: the options of the Abstraction Layer.
- `vg_drawing.h`, `vg_drawing.c`: the drawing functions called by the MicroVG natives, and their redirection to the drawer of the destination.
- `vg_drawing_stub.h`, `vg_drawing_stub.c`: the stubbed drawing functions, which report the drawing as not implemented.
- `ui_drawing_bvi.h`, `vg_drawing_bvi.h`: the drawings over a BufferedVectorImage.
- `vg_path.h`, `LLVG_PATH_impl_single.c`, `LLVG_PATH_impl_dual.c`, `LLVG_PATH_impl_stub.c`: the path natives, and their stub.
- `LLVG_MATRIX_impl.c`: the matrix natives.
- `LLVG_GRADIENT_impl_stub.c`, `LLVG_BVI_impl_stub.c`: the stubbed gradient and BufferedVectorImage natives.
- `vg_freetype.h`, `LLVG_FONT_impl_freetype.c`, `vg_freetype_path.c`, `LLVG_FONT_impl_stub.c`: the vector fonts, over FreeType.
- `ui_font_drawing_vg.c`: the MicroUI font drawing functions of `ui_font_drawing.h` for the vector fonts, the custom font format of MicroVG.
- `vg_outline_box.h`, `vg_outline_box.c`: the box of a glyph outline.
- `LLVG_PAINTER_impl.c`: the MicroVG drawing natives.
- `vg_helper.h`, `vg_helper.c`: the helpers shared by the other files.
- `vg_trace.h`: the trace identifiers of the Abstraction Layer.

# Restrictions

None.

---
_Copyright 2021-2026 MicroEJ Corp. All rights reserved._\
_MicroEJ Corp. PROPRIETARY/CONFIDENTIAL. Use is subject to license terms._

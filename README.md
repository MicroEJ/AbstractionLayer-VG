
# Overview

MicroEJ C Component: `com.microej.clibrary.llimpl#microvg`.

This component implements some Low-Level API (LLAPI) of MicroVG-1.x. 
Refer to the VEE Porting Guide > Vector Graphics > C Module to have more information on this module.

# MicroEJ VG Pack

This compoment is MicroEJ VG Pack dependent: it is compatible with a one or several MicroEJ VG Pack versions. The following table illustrates the compatibility:

| Component | MicroEJ VG Pack | Comment                      |
| ----------| ----------------| -----------------------------|
| 5.0.0     | 1.5.0           | UI Pack 14.0.0 compatibility |
| 4.0.0     | 1.4.2           | path larger than 64KB        |
| 3.0.1     | [1.4.0 - 1.4.1[ | get path bounds              |
| 3.0.0     | 1.3.0           | buffered vector image        |
| 2.1.0     | [1.2.0 - 1.2.1[ | image RAW format             |
| 2.0.0     | [1.1.0 - 1.1.1[ |                              |
| 1.0.0     | n/a             | deprecated                   |

# Usage

1. Add the following line to your `module.ivy`:

    <dependency org="com.microej.clibrary.llimpl" name="microvg" rev="6.0.1"/>
    
2. Follow the **Usage** chapters of each component part (see VEE Porting Guide > Vector Graphics > C Module)

# Migration Guide

The migration guides are described in the VEE Porting Guide > Vector Graphical > Migration Guide. 

# Requirements

N/A.

# Dependencies

_All dependencies are retrieved transitively by MicroEJ Module Manager_.

# Source

N/A.

# Restrictions

None.

---
_Copyright 2021-2024 MicroEJ Corp. All rights reserved._  
_Use of this source code is governed by a BSD-style license that can be found with this software._  

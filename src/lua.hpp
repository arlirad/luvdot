/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (c) 2025 Arlirad
 * Licensed under the GNU General Public License v3.0
 * See the LICENSE file in the top-level directory for details.
 */

#pragma once

extern "C" {
#include <lauxlib.h>
}

#include <opencv4/opencv2/core.hpp>

struct image_udata {
    cv::Mat* mat;
};

struct color {
    int r, g, b, a;

    static int Color(lua_State* L);
    static int __index(lua_State* L);
    static int __newindex(lua_State* L);
};

extern constinit luaL_Reg il_lib[];
extern constinit luaL_Reg image_lib[];
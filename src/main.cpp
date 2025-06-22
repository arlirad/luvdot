/*
 * Copyright (c) 2025 Arlirad
 * Licensed under the GNU General Public License v3.0
 * See the LICENSE file in the top-level directory for details.
 */

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

#include "conversion.hpp"
#include "lua.hpp"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <cstdlib>
#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <vector>

void create_argv_table(lua_State* L, int argc, char* argv[]) {
    lua_newtable(L);

    for (int i = 2; i < argc; i++) {
        lua_pushnumber(L, i - 1);
        lua_pushstring(L, argv[i]);
        lua_settable(L, -3);
    }

    lua_setfield(L, -2, "args");
    lua_setglobal(L, "il");
}

void create_il_table(lua_State* L, int argc, char* argv[]) {
    lua_newtable(L);
    luaL_setfuncs(L, il_lib, 0);
    create_argv_table(L, argc, argv);
}

void create_image_table(lua_State* L) {
    luaL_newmetatable(L, "image");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_setfuncs(L, image_lib, 0);
}

void create_color_class(lua_State* L) {
    const struct luaL_Reg color_funcs[] = {{NULL, NULL}};

    luaL_newmetatable(L, "color");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, color::__index);
    lua_settable(L, -3);
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, color::__newindex);
    lua_settable(L, -3);
    luaL_setfuncs(L, color_funcs, 0);

    lua_pushcfunction(L, color::Color);
    lua_setglobal(L, "Color");
}

int main(int argc, char* argv[]) {
    if (argc < 2)
        return EXIT_FAILURE;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    create_image_table(L);
    create_il_table(L, argc, argv);
    create_color_class(L);

    if (luaL_loadfile(L, argv[1]) == LUA_OK) {
        if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
            lua_pop(L, lua_gettop(L));
        }
        else {
            std::cerr << std::format("{}: {}", argv[0], lua_tostring(L, -1)) << std::endl;
            return EXIT_FAILURE;
        }
    }
    else {
        std::cerr << std::format("{}: {}", argv[0], lua_tostring(L, -1)) << std::endl;
        return EXIT_FAILURE;
    }

    lua_close(L);
    return EXIT_SUCCESS;
}

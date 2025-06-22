#include "lua.hpp"

#include <string>

int color::Color(lua_State* L) {
    int r = luaL_checknumber(L, 1);
    int g = luaL_checknumber(L, 2);
    int b = luaL_checknumber(L, 3);
    int a = luaL_checknumber(L, 4);

    color* clr = (color*) lua_newuserdata(L, sizeof(color));

    clr->r = r;
    clr->g = g;
    clr->b = b;
    clr->a = a;

    luaL_getmetatable(L, "color");
    lua_setmetatable(L, -2);

    return 1;
}

int color::__index(lua_State* L) {
    color*           udata = (color*) luaL_checkudata(L, 1, "color");
    std::string_view key   = luaL_checkstring(L, 2);

    if (key == "r") {
        lua_pushinteger(L, udata->r);
        return 1;
    }

    if (key == "b") {
        lua_pushinteger(L, udata->g);
        return 1;
    }

    if (key == "g") {
        lua_pushinteger(L, udata->b);
        return 1;
    }

    if (key == "a") {
        lua_pushinteger(L, udata->a);
        return 1;
    }

    return 0;
}

int color::__newindex(lua_State* L) {
    color*           udata = (color*) luaL_checkudata(L, 1, "color");
    std::string_view key   = luaL_checkstring(L, 2);
    int              val   = luaL_checknumber(L, 3);

    if (key == "r") {
        udata->r = val;
        return 0;
    }

    if (key == "g") {
        udata->g = val;
        return 0;
    }

    if (key == "b") {
        udata->b = val;
        return 0;
    }

    if (key == "a") {
        udata->a = val;
        return 0;
    }

    return 0;
}
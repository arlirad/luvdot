#include "conversion.hpp"
#include "lua.hpp"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <format>
#include <iostream>

namespace il {
    int render(lua_State* L) {
        const char* path_in  = luaL_checkstring(L, 1);
        const char* path_out = luaL_checkstring(L, 2);

        std::cout << std::format("Rendering {} to {}", path_in, path_out) << std::endl;

        std::string cmdline = "inkscape \"" + std::string(path_in) +
                              "\" --export-type=png --export-filename=\"" + std::string(path_out) +
                              "\"";

        int result = std::system(cmdline.c_str());
        if (result != 0)
            return luaL_error(L, "Failed to invoke inkscape: %s", std::strerror(result));

        return 0;
    }

    int open(lua_State* L) {
        const char*  path     = luaL_checkstring(L, 1);
        image_udata* userdata = (image_udata*) lua_newuserdata(L, sizeof(image_udata));

        userdata->mat  = new cv::Mat();
        *userdata->mat = imread(path, cv::IMREAD_UNCHANGED);

        if (userdata->mat->channels() == 3)
            cv::cvtColor(*userdata->mat, *userdata->mat, cv::COLOR_BGR2BGRA);

        luaL_getmetatable(L, "image");
        lua_setmetatable(L, -2);

        std::cout << std::format("Opened {}: {}x{}", path, userdata->mat->rows, userdata->mat->cols)
                  << std::endl;

        return 1;
    }

    int create(lua_State* L) {
        int          width    = luaL_checkinteger(L, 1);
        int          height   = luaL_checkinteger(L, 2);
        image_udata* userdata = (image_udata*) lua_newuserdata(L, sizeof(image_udata));

        userdata->mat = new cv::Mat(width, height, CV_8UC4);

        luaL_getmetatable(L, "image");
        lua_setmetatable(L, -2);

        return 1;
    }

    int empty(lua_State* L) {
        image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

        image_udata* userdata = (image_udata*) lua_newuserdata(L, sizeof(image_udata));

        userdata->mat  = new cv::Mat();
        *userdata->mat = cv::Mat::zeros(udata->mat->size(), udata->mat->type());

        luaL_getmetatable(L, "image");
        lua_setmetatable(L, -2);

        return 1;
    }

    int hex2color(lua_State* L) {
        std::string_view hex = luaL_checkstring(L, 1);
        color*           clr = (color*) lua_newuserdata(L, sizeof(color));

        if (hex[0] == '#')
            hex = hex.begin() + 1;

        if (hex.size() != 3 && hex.size() != 6)
            lua_error(L);

        char buffer[8] = {};

        if (hex.size() == 3) {
            for (int i = 0; i < 6; i++)
                buffer[i] = hex[i / 2];
        }
        else
            std::copy_n(hex.begin(), hex.size(), buffer);

        clr->r = hex2byte(buffer + 0);
        clr->g = hex2byte(buffer + 2);
        clr->b = hex2byte(buffer + 4);
        clr->a = 255;

        luaL_getmetatable(L, "color");
        lua_setmetatable(L, -2);

        return 1;
    }
}

constinit luaL_Reg il_lib[] = {{"open", il::open},           {"create", il::create},
                               {"render", il::render},       {"empty", il::empty},
                               {"hex2color", il::hex2color}, {nullptr, nullptr}};
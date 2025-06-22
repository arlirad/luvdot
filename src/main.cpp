extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

struct image_udata {
    cv::Mat* mat;
};

struct color {
    int r, g, b, a;
};

int char_to_channel(char ch) {
    return ch == 'r' ? 2 : ch == 'g' ? 1 : ch == 'b' ? 0 : ch == 'a' ? 3 : -1;
}

uint8_t hex2byte(const char* byte) {
    char buffer[4] = {};

    buffer[0] = byte[0];
    buffer[1] = byte[1];

    return std::stoul(buffer, nullptr, 16);
}

cv::Mat* image(void* udata) {
    return (cv::Mat*) *((void**) udata);
}

int img_save(lua_State* L) {
    image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");
    const char*  path  = luaL_checkstring(L, 2);

    imwrite(path, *udata->mat);

    std::cout << "saved " << path << std::endl;
    return 0;
}

int img_multiply(lua_State* L) {
    image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

    auto r = luaL_checknumber(L, 2);
    auto g = luaL_checknumber(L, 3);
    auto b = luaL_checknumber(L, 4);
    auto a = luaL_checknumber(L, 5);

    cv::multiply(*udata->mat, cv::Scalar(b, g, r, a), *udata->mat);
    return 0;
}

int img_multiplyImg(lua_State* L) {
    image_udata* udataA = (image_udata*) luaL_checkudata(L, 1, "image");
    image_udata* udataB = (image_udata*) luaL_checkudata(L, 2, "image");

    auto a = *udataA->mat;
    auto b = *udataB->mat;

    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.cols; j++) {
            // auto fac = b.at<cv::Vec4b>(i, j)[3] / 255.0;

            a.at<cv::Vec4b>(i, j)[0] *= b.at<cv::Vec4b>(i, j)[0] / 255.00;
            a.at<cv::Vec4b>(i, j)[1] *= b.at<cv::Vec4b>(i, j)[1] / 255.00;
            a.at<cv::Vec4b>(i, j)[2] *= b.at<cv::Vec4b>(i, j)[2] / 255.00;
            a.at<cv::Vec4b>(i, j)[3] *= b.at<cv::Vec4b>(i, j)[3] / 255.00;
        }
    }

    return 0;
}

int img_rectangle(lua_State* L) {
    image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int w = luaL_checkinteger(L, 4);
    int h = luaL_checkinteger(L, 5);

    color* clr = (color*) luaL_checkudata(L, 6, "color");

    int width = luaL_checkinteger(L, 7);

    std::cout << "rect " << x << "x" << y << ", " << w << "x" << h << " rgb(" << clr->r << ", "
              << clr->g << ", " << clr->b << ")"
              << " " << width << std::endl;

    cv::rectangle(*udata->mat, cv::Rect(x, y, w, h), cv::Scalar(clr->b, clr->g, clr->r, clr->a),
                  width);
    return 0;
}

int img_line(lua_State* L) {
    image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

    int xa = luaL_checkinteger(L, 2);
    int ya = luaL_checkinteger(L, 3);
    int xb = luaL_checkinteger(L, 4);
    int yb = luaL_checkinteger(L, 5);

    color* clr = (color*) luaL_checkudata(L, 6, "color");

    int width = luaL_checkinteger(L, 7);

    cv::line(*udata->mat, cv::Point(xa, ya), cv::Point(xb, yb),
             cv::Scalar(clr->r, clr->g, clr->b, clr->a), width);
    return 0;
}

int img_circle(lua_State* L) {
    image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

    int x      = luaL_checkinteger(L, 2);
    int y      = luaL_checkinteger(L, 3);
    int radius = luaL_checkinteger(L, 4);

    color* clr = (color*) luaL_checkudata(L, 5, "color");

    int width = luaL_checkinteger(L, 6);

    cv::circle(*udata->mat, cv::Point(x, y), radius, cv::Scalar(clr->r, clr->g, clr->b, clr->a),
               width);
    return 0;
}

int img_ontop(lua_State* L) {
    image_udata* udata_top  = (image_udata*) luaL_checkudata(L, 1, "image");
    image_udata* udata_base = (image_udata*) luaL_checkudata(L, 2, "image");

    int x_offset = luaL_optinteger(L, 3, 0);
    int y_offset = luaL_optinteger(L, 4, 0);

    auto top  = *udata_top->mat;
    auto base = *udata_base->mat;

    for (int i = 0; i < top.rows; i++) {
        for (int j = 0; j < top.cols; j++) {
            int bi = i + y_offset;
            int bj = j + x_offset;
            int ti = i;
            int tj = j;

            if (top.at<cv::Vec4b>(ti, tj)[3] > base.at<cv::Vec4b>(bi, bj)[3])
                base.at<cv::Vec4b>(bi, bj)[3] = top.at<cv::Vec4b>(ti, tj)[3];

            auto fac = top.at<cv::Vec4b>(ti, tj)[3] / 255.0;

            base.at<cv::Vec4b>(bi, bj)[0] =
                base.at<cv::Vec4b>(bi, bj)[0] * (1 - fac) + top.at<cv::Vec4b>(ti, tj)[0] * fac;
            base.at<cv::Vec4b>(bi, bj)[1] =
                base.at<cv::Vec4b>(bi, bj)[1] * (1 - fac) + top.at<cv::Vec4b>(ti, tj)[1] * fac;
            base.at<cv::Vec4b>(bi, bj)[2] =
                base.at<cv::Vec4b>(bi, bj)[2] * (1 - fac) + top.at<cv::Vec4b>(ti, tj)[2] * fac;
        }
    }

    return 0;
}

int img_swap(lua_State* L) {
    image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");
    const char*  a     = luaL_checkstring(L, 2);
    const char*  b     = luaL_checkstring(L, 3);

    int a_ch = char_to_channel(a[0]);
    int b_ch = char_to_channel(b[0]);

    std::cout << a_ch << " to " << b_ch << std::endl;

    auto mat = *udata->mat;

    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            auto a_v = mat.at<cv::Vec4b>(i, j)[a_ch];
            auto b_v = mat.at<cv::Vec4b>(i, j)[b_ch];

            mat.at<cv::Vec4b>(i, j)[b_ch] = a_v;
            mat.at<cv::Vec4b>(i, j)[a_ch] = b_v;
        }
    }

    return 0;
}

int img_set(lua_State* L) {
    image_udata* udata  = (image_udata*) luaL_checkudata(L, 1, "image");
    const char*  ch_str = luaL_checkstring(L, 2);
    int          val    = luaL_checkinteger(L, 3);

    int  ch  = char_to_channel(ch_str[0]);
    auto mat = *udata->mat;

    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++)
            mat.at<cv::Vec4b>(i, j)[ch] = val;
    }

    return 0;
}

int il_render(lua_State* L) {
    const char* path_in  = luaL_checkstring(L, 1);
    const char* path_out = luaL_checkstring(L, 2);

    std::string cmdline = "inkscape \"" + std::string(path_in) +
                          "\" --export-type=png --export-filename=\"" + std::string(path_out) +
                          "\"";

    int result = std::system(cmdline.c_str());
    if (result != 0)
        return luaL_error(L, "Failed to invoke inkscape: %s", std::strerror(result));

    return 0;
}

int il_open(lua_State* L) {
    const char*  path     = luaL_checkstring(L, 1);
    image_udata* userdata = (image_udata*) lua_newuserdata(L, sizeof(image_udata));

    userdata->mat  = new cv::Mat();
    *userdata->mat = imread(path, cv::IMREAD_UNCHANGED);

    if (userdata->mat->channels() == 3)
        cv::cvtColor(*userdata->mat, *userdata->mat, cv::COLOR_BGR2BGRA);

    luaL_getmetatable(L, "image");
    lua_setmetatable(L, -2);

    std::cout << "opened " << path << ": " << userdata->mat->rows << "x" << userdata->mat->cols
              << std::endl;

    return 1;
}

int il_create(lua_State* L) {
    int          width    = luaL_checkinteger(L, 1);
    int          height   = luaL_checkinteger(L, 2);
    image_udata* userdata = (image_udata*) lua_newuserdata(L, sizeof(image_udata));

    userdata->mat = new cv::Mat(width, height, CV_8UC4);

    luaL_getmetatable(L, "image");
    lua_setmetatable(L, -2);

    return 1;
}

int il_empty(lua_State* L) {
    image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

    image_udata* userdata = (image_udata*) lua_newuserdata(L, sizeof(image_udata));

    userdata->mat  = new cv::Mat();
    *userdata->mat = cv::Mat::zeros(udata->mat->size(), udata->mat->type());

    luaL_getmetatable(L, "image");
    lua_setmetatable(L, -2);

    return 1;
}

int il_hex2color(lua_State* L) {
    const char* hex = luaL_checkstring(L, 1);
    color*      clr = (color*) lua_newuserdata(L, sizeof(color));

    if (hex[0] == '#')
        hex++;

    if (strlen(hex) != 3 && strlen(hex) != 6)
        lua_error(L);

    char buffer[8] = {};

    if (strlen(hex) == 3) {
        for (int i = 0; i < 6; i++)
            buffer[i] = hex[i / 2];
    }
    else {
        strncpy(buffer, hex, sizeof(buffer));
    }

    clr->r = hex2byte(buffer + 0);
    clr->g = hex2byte(buffer + 2);
    clr->b = hex2byte(buffer + 4);
    clr->a = 255;

    luaL_getmetatable(L, "color");
    lua_setmetatable(L, -2);

    return 1;
}

int color_index(lua_State* L) {
    color*      udata = (color*) luaL_checkudata(L, 1, "color");
    const char* key   = luaL_checkstring(L, 2);

    if (strcmp(key, "r") == 0) {
        lua_pushinteger(L, udata->r);
        return 1;
    }

    if (strcmp(key, "g") == 0) {
        lua_pushinteger(L, udata->g);
        return 1;
    }

    if (strcmp(key, "b") == 0) {
        lua_pushinteger(L, udata->b);
        return 1;
    }

    if (strcmp(key, "a") == 0) {
        lua_pushinteger(L, udata->a);
        return 1;
    }

    return 0;
}

int color_newindex(lua_State* L) {
    color*      udata = (color*) luaL_checkudata(L, 1, "color");
    const char* key   = luaL_checkstring(L, 2);
    int         val   = luaL_checknumber(L, 3);

    if (strcmp(key, "r") == 0) {
        udata->r = val;
        return 0;
    }

    if (strcmp(key, "g") == 0) {
        udata->g = val;
        return 0;
    }

    if (strcmp(key, "b") == 0) {
        udata->b = val;
        return 0;
    }

    if (strcmp(key, "a") == 0) {
        udata->a = val;
        return 0;
    }

    return 0;
}

int color_ctor(lua_State* L) {
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

int main(int argc, char* argv[]) {
    if (argc < 2)
        return 1;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    const struct luaL_Reg imageFuncs[] = {{"save", img_save},
                                          {"multiply", img_multiply},
                                          {"multiplyImg", img_multiplyImg},
                                          {"rectangle", img_rectangle},
                                          {"line", img_line},
                                          {"circle", img_circle},
                                          {"ontop", img_ontop},
                                          {"swap", img_swap},
                                          {"set", img_set},
                                          {NULL, NULL}};

    luaL_newmetatable(L, "image");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_setfuncs(L, imageFuncs, 0);

    const struct luaL_Reg colorFuncs[] = {{NULL, NULL}};

    luaL_newmetatable(L, "color");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, color_index);
    lua_settable(L, -3);
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, color_newindex);
    lua_settable(L, -3);
    luaL_setfuncs(L, colorFuncs, 0);

    const struct luaL_Reg imageLib[] = {{"open", il_open},           {"create", il_create},
                                        {"render", il_render},       {"empty", il_empty},
                                        {"hex2color", il_hex2color}, {NULL, NULL}};

    lua_newtable(L);
    luaL_setfuncs(L, imageLib, 0);

    lua_newtable(L);

    for (int i = 2; i < argc; i++) {
        lua_pushnumber(L, i - 1);
        lua_pushstring(L, argv[i]);
        lua_settable(L, -3);
    }

    lua_setfield(L, -2, "args");
    lua_setglobal(L, "il");

    lua_pushcfunction(L, color_ctor);
    lua_setglobal(L, "Color");

    if (luaL_loadfile(L, argv[1]) == LUA_OK) {
        if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
            lua_pop(L, lua_gettop(L));
        }
        else {
            std::cerr << lua_tostring(L, -1) << std::endl;
            return 1;
        }
    }
    else {
        std::cerr << lua_tostring(L, -1) << std::endl;
        return 1;
    }

    lua_close(L);
    return 0;
}

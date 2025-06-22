/*
 * Copyright (c) 2025 Arlirad
 * Licensed under the GNU General Public License v3.0
 * See the LICENSE file in the top-level directory for details.
 */

#include "conversion.hpp"
#include "lua.hpp"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include <format>
#include <iostream>

namespace image {

    int save(lua_State* L) {
        image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");
        const char*  path  = luaL_checkstring(L, 2);

        imwrite(path, *udata->mat);

        std::cout << std::format("Saved {}", path) << std::endl;
        return 0;
    }

    int multiply(lua_State* L) {
        image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

        auto r = luaL_checknumber(L, 2);
        auto g = luaL_checknumber(L, 3);
        auto b = luaL_checknumber(L, 4);
        auto a = luaL_checknumber(L, 5);

        cv::multiply(*udata->mat, cv::Scalar(b, g, r, a), *udata->mat);
        return 0;
    }

    int multiplyImg(lua_State* L) {
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

    int rectangle(lua_State* L) {
        image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

        int    x     = luaL_checkinteger(L, 2);
        int    y     = luaL_checkinteger(L, 3);
        int    w     = luaL_checkinteger(L, 4);
        int    h     = luaL_checkinteger(L, 5);
        color* clr   = (color*) luaL_checkudata(L, 6, "color");
        int    width = luaL_checkinteger(L, 7);

        std::cout << std::format("Rect {}x{}, {}x{}, rgb({}, {}, {}), {}", x, y, w, h, clr->r,
                                 clr->g, clr->b, width);

        cv::rectangle(*udata->mat, cv::Rect(x, y, w, h), cv::Scalar(clr->b, clr->g, clr->r, clr->a),
                      width);

        return 0;
    }

    int line(lua_State* L) {
        image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

        int    xa    = luaL_checkinteger(L, 2);
        int    ya    = luaL_checkinteger(L, 3);
        int    xb    = luaL_checkinteger(L, 4);
        int    yb    = luaL_checkinteger(L, 5);
        color* clr   = (color*) luaL_checkudata(L, 6, "color");
        int    width = luaL_checkinteger(L, 7);

        cv::line(*udata->mat, cv::Point(xa, ya), cv::Point(xb, yb),
                 cv::Scalar(clr->r, clr->g, clr->b, clr->a), width);

        return 0;
    }

    int circle(lua_State* L) {
        image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");

        int    x      = luaL_checkinteger(L, 2);
        int    y      = luaL_checkinteger(L, 3);
        int    radius = luaL_checkinteger(L, 4);
        color* clr    = (color*) luaL_checkudata(L, 5, "color");
        int    width  = luaL_checkinteger(L, 6);

        cv::circle(*udata->mat, cv::Point(x, y), radius, cv::Scalar(clr->r, clr->g, clr->b, clr->a),
                   width);

        return 0;
    }

    int ontop(lua_State* L) {
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

    int swap(lua_State* L) {
        image_udata* udata = (image_udata*) luaL_checkudata(L, 1, "image");
        const char*  a     = luaL_checkstring(L, 2);
        const char*  b     = luaL_checkstring(L, 3);

        int a_ch = char_to_channel(a[0]);
        int b_ch = char_to_channel(b[0]);

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

    int set(lua_State* L) {
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
}

constinit luaL_Reg image_lib[] = {{"save", image::save},
                                  {"multiply", image::multiply},
                                  {"multiplyImg", image::multiplyImg},
                                  {"rectangle", image::rectangle},
                                  {"line", image::line},
                                  {"circle", image::circle},
                                  {"ontop", image::ontop},
                                  {"swap", image::swap},
                                  {"set", image::set},
                                  {nullptr, nullptr}};

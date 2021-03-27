#include "src/example.h"
#include <microgl/canvas.h>
#include <microgl/Q.h>
#include <microgl/samplers/flat_color.h>
#include <microgl/pixel_coders/RGB888_PACKED_32.h>
#include <microgl/tesselation/stroke_tessellation.h>
#include <microgl/static_array.h>
#include <vector>

#define W 640*1
#define H 640*1

template<typename item>
using stat_array = static_array<item, 800>;

template<typename item>
//using container = dynamic_array<item>;
//using container = std::vector<item>;
using container = stat_array<item>;

template <typename number>
container<vec2<number>> path_diagonal() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 100};
    vec2<number> p1 = {200, 200};
    vec2<number> p2 = {300, 300};

    return il{p0, p1, p2};
}

template <typename number>
container<vec2<number>> path_horizontal() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 100};
    vec2<number> p1 = {300, 100};
    vec2<number> p2 = {500, 100};
    return il{p0, p1};
}

template <typename number>
container<vec2<number>> path_resh() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 100};
    vec2<number> p1 = {300, 100};
    vec2<number> p2 = {300, 300};
    return il{p0, p1, p2};
}

template <typename number>
container<vec2<number>> path_2() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 100};
    vec2<number> p1 = {300, 400};
    vec2<number> p2 = {400, 100};
    vec2<number> p3 = {400, 300};

//    return {p0,p1,p2, p3, {50,50}};
    return il{p0,p1,p2, p3};//, {250,320}};
    return il{p0, p1, p2, p3};
}

template <typename number>
container<vec2<number>> path_3() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 100};
    vec2<number> p1 = {300, 100};
    vec2<number> p2 = {300, 300};
    vec2<number> p3 = {400, 300};

    return il{p0, p1,p2};//, p3};
}


template <typename number>
container<vec2<number>> path_line() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 100};
    vec2<number> p1 = {300, 100};

    return il{p0, p1};
}

template <typename number>
container<vec2<number>> path_tri() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 100};
    vec2<number> p1 = {200, 100};
    vec2<number> p2 = {200, 200};
//    vec2<T> p3 = {220, 100};
    vec2<number> p3 = {300, 200};

    return il{p0, p1, p2, p3};
//    return {p0, p1};
}

template <typename number>
container<vec2<number>> path_rect() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 200};
    vec2<number> p1 = {420, 100};
    vec2<number> p2 = {200, 100};
    vec2<number> p3 = {50, 50};

    return il{p0, p1, p2, p3};
}

template <typename number>
container<vec2<number>> path_rect2() {
    using il = std::initializer_list<vec2<number>>;
    vec2<number> p0 = {100, 200};
    vec2<number> p1 = {100, 100};
    vec2<number> p2 = {200, 100};
    vec2<number> p3 = {200, 200};
    vec2<number> p4 = {100, 200};

    return il{p0, p1, p2, p3, p4,p4};
}

int main() {
    using number = float;
//    using number = double;
//    using number = Q<12>;
//    using number = Q<4>;

    using Canvas24= canvas<bitmap<coder::RGB888_PACKED_32>>;
    sampling::flat_color<> color_red{{255,0,0,255}};
    Canvas24 canvas(W, H);
    float t = 0;
    auto render_path = [&](const container<vec2<number>> & path, number stroke_width, bool close_path) {
        using index = unsigned int;
//        using stroke_tess = microgl::tessellation::stroke_tessellation<number, dynamic_array>;
//        using stroke_tess = microgl::tessellation::stroke_tessellation<number, std::vector>;
        using stroke_tess = microgl::tessellation::stroke_tessellation<number, stat_array>;

        //auto type = triangles::indices::TRIANGLES_STRIP;
        auto type = triangles::indices::TRIANGLES_STRIP_WITH_BOUNDARY;
        t+=0.0128;
        container<index> indices;
        container<vec2<number>> vertices;
        container<boundary_info> boundary_buffer;

        stroke_tess::compute_with_dashes(
                stroke_width,
                close_path,
                tessellation::stroke_cap::butt,
//            tessellation::stroke_cap::square,
//            tessellation::stroke_cap::round,
                tessellation::stroke_line_join::round,
//            tessellation::stroke_line_join::bevel,
//            tessellation::stroke_line_join::miter_clip,
//            tessellation::stroke_line_join::miter,
//            tessellation::stroke_line_join::none,
                8,
                std::initializer_list<int>{50, 20}, t,
                path.data(),
                path.size(),
                vertices,
                indices,
                type,
//            nullptr);
                &boundary_buffer);

        // draw triangles batch
        canvas.clear({255,255,255,255});
        canvas.drawTriangles<blendmode::Normal, porterduff::FastSourceOverOnOpaque, true>(
                color_red,
                matrix_3x3<number>::identity(),
                vertices.data(),
                (vec2<number> *)nullptr,
                indices.data(),
                boundary_buffer.data(),
                indices.size(),
                type,
                127
        );

//        return;
        canvas.drawTrianglesWireframe(
                {0,0,0,255},
                matrix_3x3<number>::identity(),
                vertices.data(),
                indices.data(),
                indices.size(),
                type,
                255
        );

    };

    auto render = [&]() {
        static auto path = path_rect<number>();
//        static auto path = path_rect2<number>();
//        static auto path = path_tri<number>();
//        static auto path = path_2<number>();
//        static auto path = path_line<number>();
//        static auto path = path_3<number>();

        render_path(path, 20.0f, false);
//        render_path(path, 40.0f, false);
//        render_path(path, 40.0f, true);
//        render_path(path, 50.0f, false);
//        render_path(path, 20.0f, false);
//        render_path(path, 15.0f, false);
//        render_path(path, number(10.0f), true);
//        render_path(path, number(15.0f), false);
//        render_path(path, number(15.0f), false);
    };

    example_run(&canvas, render);
}
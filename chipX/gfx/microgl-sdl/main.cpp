//
// Created by Tomer Shalev on 2019-06-15.
// this is a sandbox for playing with microgl lib
//

#include <stdio.h>
#include <iostream>
#include <chrono>
#include "src/Resources.h"
#include <SDL2/SDL.h>
#include <microgl/FrameBuffer.h>
#include <microgl/Canvas.h>
#include <microgl/Types.h>
#include <microgl/PixelFormat.h>
#include <microgl/PixelCoder.h>
#include <microgl/Bitmap.h>
#include <microgl/BentleyOttmannLineIntersection.h>

void test_curve_split();
void test_curve_adaptive_subdivide();

#define TEST_ITERATIONS 1
#define W 640*1
#define H 480*1

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;

typedef Bitmap<vec3<uint8_t>, RGB888_ARRAY> Bitmap24bitU8;
typedef Bitmap<uint32_t, RGB888_PACKED_32> Bitmap24bit_Packed32;

typedef Canvas<uint32_t, RGB888_PACKED_32> Canvas24Bit_Packed32;
typedef Canvas<vec3<uint8_t >, RGB888_ARRAY> Canvas24BitU8;

Canvas24Bit_Packed32 * canvas;

Bitmap24bitU8 * bmp_1;
Bitmap24bit_Packed32 * bmp_2;
Bitmap24bitU8 * bmp_uv_U8;
Bitmap24bit_Packed32 * bmp_uv;

Resources resources{};
Resources::image_info_t img_1;

color_f_t RED{1.0,0.0,0.0, 1.0};
color_f_t YELLOW{1.0,1.0,0.0, 1.0};
color_f_t WHITE{1.0,1.0,1.0, 1.0};
color_f_t GREEN{0.0,1.0,0.0, 1.0};
color_f_t BLUE{0.0,0.0,1.0, 1.0};
color_f_t BLACK{0.0,0.0,0.0, 1.0};

void loop();
void init_sdl(int width, int height);

using namespace tessellation;

//segment_t s1 {{10,10}, {400, 400}};
//segment_t s2 {{600,40}, {150, 350}};
//segment_t s3 {{300,40}, {40, 350}};

segment_t s1 {{20,200}, {400, 200}};
segment_t s2 {{50,20}, {50, 400}};
segment_t s3 {{150,20}, {150, 400}};

vec2_32i s1_[2] = {{10, 10}, {200, 200}};
vec2_32i s2_[2] = {{40, 10}, {150, 200}};

//std::vector<segment_t> segments {s1, s2};
std::vector<segment_t> segments {s1, s2, s3};

BentleyOttmann bentleyOttmann(segments.data(), segments.size());

int t = 0;
inline void render() {

    canvas->setAntialiasing(false);

    for (int ix = 0; ix < 1; ++ix) {
        t += 1;
        canvas->clear(WHITE);

        s2.p0.x = 200;

        bentleyOttmann.line_intersection();

        auto & I = bentleyOttmann.getIntersections();
        for (auto & inter : I) {
            canvas->drawCircle<blendmode::Normal, porterduff::SourceOverOnOpaque, true>(RED, inter.x, inter.y, 5);

        }

        for (auto & segment : segments) {
            canvas->drawLine(BLACK,
                    segment.p0.x, segment.p0.y,
                    segment.p1.x, segment.p1.y,
                    0
                    );
        }



    }

}


int main() {


    init_sdl(W, H);
    loop();
}

void init_sdl(int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL2 Pixel Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, width, height);

    canvas = new Canvas24Bit_Packed32(width, height, new RGB888_PACKED_32());

    img_1 = resources.loadImageFromCompressedPath("charsprites.png");
    auto img_2 = resources.loadImageFromCompressedPath("uv_256.png");

    bmp_1 = new Bitmap<vec3<uint8_t>, RGB888_ARRAY>(img_1.data, img_1.width, img_1.height, new RGB888_ARRAY());
    bmp_2 = bmp_1->convertToBitmap<uint32_t , RGB888_PACKED_32>();

    bmp_uv_U8 = new Bitmap<vec3<uint8_t>, RGB888_ARRAY>(img_2.data, img_2.width, img_2.height, new RGB888_ARRAY());
    bmp_uv = bmp_uv_U8->convertToBitmap<uint32_t , RGB888_PACKED_32>();

    resources.init();
}

int render_test(int N) {
    auto ms = std::chrono::milliseconds(1);
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        render();
    }

    auto end = std::chrono::high_resolution_clock::now();

    return (end-start)/(ms*N);
}

void loop() {
    bool quit = false;
    SDL_Event event;

    // 100 Quads
    int ms = render_test(TEST_ITERATIONS);
    cout << ms << endl;

    while (!quit)
    {
        SDL_PollEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYUP:
                if( event.key.keysym.sym == SDLK_ESCAPE )
                    quit = true;
                break;
        }
//
        render();

        SDL_UpdateTexture(texture, nullptr, canvas->pixels(), canvas->width() * canvas->sizeofPixel());
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }


//    SDL_Delay(2000);

    SDL_DestroyWindow(window);
    SDL_Quit();
}

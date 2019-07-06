#pragma once

#include "Types.h"
#include "FrameBuffer.h"
#include "PixelFormat.h"
#include "Functions.h"
#include "PorterDuff.h"
#include "BlendMode.h"
#include "PixelCoder.h"
#include "Bitmap.h"
#include "BlendMode.h"
#include "PorterDuff.h"

template<typename P, typename CODER>
class Canvas {
public:

    Canvas(Bitmap<P, CODER> * $bmp);
    Canvas(int width, int height, PixelCoder<P, CODER> * $coder);
    int width();
    int height();
    PixelFormat pixelFormat();
    unsigned int sizeofPixel();

    P* pixels();
    P &getPixel(int x, int y);
    P &getPixel(int index);
    void getPixelColor(int index, color_t & output);
    void getPixelColor(int x, int y, color_t & output);
    void getPixelColor(int x, int y, color_f_t & output);
    void getPixelColor(int index, color_f_t & output);

    PixelCoder<P, CODER> * coder();
    Bitmap<P, CODER> * bitmapCanvas();

    bool hasAlphaChannel();
    bool hasAntialiasing();
    void setAntialiasing(bool value);

    void clear(const color_f_t &color);

    // float blenders
    template<typename BlendMode, typename PorterDuff>
    void blendColor(const color_f_t &val, int x, int y, float opacity=1.0f);
    template<typename BlendMode, typename PorterDuff>
    void blendColor(const color_f_t &val, int index, float opacity=1.0f);

    // integer blenders
    template<typename BlendMode, typename PorterDuff>
    void blendColor(const color_t &val, int x, int y, uint8_t opacity=255);
    template<typename BlendMode, typename PorterDuff>
    void blendColor(const color_t &val, int index, uint8_t opacity=255);

    void drawPixel(const P &val, int x, int y);
    void drawPixel(const P &val, int index);

    void drawLine(const color_f_t & color, int x0, int y0, int x1, int y1);

    void drawGradient(const color_f_t &startColor, const color_f_t &endColor,
                      int left, int top, int w, int h);
    void drawCircle(const color_f_t & color, int centerX, int centerY, int radius);

    void drawTriangle(const color_f_t & color, const int x0, const int y0,
                                               const int x1, const int y1,
                                               const int x2, const int y2);
    template <typename P2, typename CODER2>
    void drawTriangle(Bitmap<P2, CODER2> &bmp,
                       int v0_x, int v0_y, float u0, float v0,
                       int v1_x, int v1_y, float u1, float v1,
                       int v2_x, int v2_y, float u2, float v2);

    template<typename BlendMode=blendmode::Normal, typename PorterDuff=porterduff::SourceOverOnOpaque>
    void drawQuad(const color_f_t &color,
                  const int left, const int top,
                  const int right, const int bottom,
                  const uint8_t opacity = 255);

    template <typename P2, typename CODER2>
    void drawQuad(Bitmap<P2, CODER2> &bmp, const int left, const int top, const int right, const int bottom);

private:
    int _width = 0, _height = 0;
    Bitmap<P, CODER> * _bitmap_canvas = nullptr;
    bool _flag_hasAlphaChannel = false;
    bool _flag_antiAlias = true;
};

#include "../src/Canvas.tpp"



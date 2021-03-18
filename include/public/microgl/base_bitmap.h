#pragma once

#include <microgl/buffer.h>
#include <microgl/crpt.h>
#include <microgl/color.h>
#include <microgl/pixel_coder.h>
#include <microgl/micro_gl_traits.h>

/**
 * a base bitmap type, use it with crpt design pattern for extension.
 * Bitmap is a general type that :
 * 1. references an array of pixels
 * 2. has a pixel coder
 * 3. has interface to read/write pixels
 *
 * @tparam impl implementation type of derived class
 * @tparam pixel_coder_ the pixel coder type of the bitmap
 * @tparam buffer_element_type the type of the elements stored in the pixel array
 */
template <typename impl, typename pixel_coder_,
          typename buffer_element_type=typename pixel_coder_::pixel>
class base_bitmap : public crpt<impl> {
public:
    using pixel_coder=pixel_coder_;
    using pixel=typename pixel_coder::pixel;
    using rgba=typename pixel_coder::rgba;

protected:
    int _width = 0, _height = 0;
    pixel_coder _coder;
    buffer<buffer_element_type> _buffer;
    // todo:: add a sub window feature, only involves translating x and y coords or a fixed index ?

public:
    static constexpr bool hasNativeAlphaChannel() { return pixel_coder::rgba::a != 0; }
    static constexpr bool nativeAlphaChannelBits() { return hasNativeAlphaChannel() ? pixel_coder::rgba::a : 0; }
    static constexpr int maxNativeAlphaChannelValue() { return (1u<<nativeAlphaChannelBits())-1; }

    base_bitmap(int w, int h) : base_bitmap(new buffer_element_type[w * h], w * h, w, h, true) {}
    base_bitmap(void *$pixels, int w, int h) : base_bitmap($pixels, w*h, w, h, false) {}
    base_bitmap(void *$pixels, int size, int w, int h, bool owner=false) :
            _width{w}, _height{h}, _coder{},
            _buffer(reinterpret_cast<buffer_element_type *>($pixels), size, owner) {
    }
    base_bitmap(const base_bitmap & bmp) : _buffer{bmp._buffer}, _width{bmp.width()}, _height{bmp.height()} {
    }
    base_bitmap(base_bitmap && bmp)  noexcept : _buffer{microgl::traits::move(bmp._buffer)}, _width{bmp.width()}, _height{bmp.height()} {
    }
    base_bitmap & operator=(const base_bitmap & bmp) {
        _width=bmp.width(); _height=bmp.height();
        _buffer = bmp._buffer;
        return *this;
    }
    base_bitmap & operator=(base_bitmap && bmp)  noexcept {
        _width=bmp.width(); _height=bmp.height();
        _buffer = microgl::traits::move(bmp._buffer);
        return *this;
    }
    virtual ~base_bitmap() = default;

    // does the underlying pixel buffer own the data ?
    bool isOwner() { return _buffer.owner; }
    int width() const { return _width; }
    int height() const { return _height; }
    int size() const { return _buffer.size();}
    pixel * data() { return _buffer._data; }

    int locate(int x, int y) const { return y*this->_width + x; }
    pixel pixelAt(int x, int y) const { return this->pixelAt(y*this->_width + x); }
    pixel pixelAt(int index) const { return this->derived().pixelAt(index); }
    void writeAt(int x, int y, const pixel &value) { this->writeAt(y*this->_width + x, value); }
    void writeAt(int index, const pixel &value) { this->derived().writeAt(index, value); }
    void fill(const pixel &value) { this->derived().fill(value); }

    const pixel_coder &coder() const { return _coder; }

    void decode(int x, int y, microgl::color::color_t &output) const{
        _coder.decode(pixelAt(x, y), output);
    }

    void decode(int index, microgl::color::color_t &output) const{
        _coder.decode(pixelAt(index), output);
    }

    template <typename number>
    void decode(int x, int y, microgl::color::intensity<number> &output) const {
        microgl::coder::decode<number, pixel_coder>(pixelAt(x, y), output, coder());
    }

    template <typename number>
    void decode(int index, microgl::color::intensity<number> &output) const {
        microgl::coder::decode<number, pixel_coder>(pixelAt(index), output, coder());
    }

    void writeColor(int index, const microgl::color::color_t &color) {
        pixel output;
        _coder.encode(color, output);
        writeAt(index, output);
    }

    void writeColor(int x, int y, const microgl::color::color_t &color) {
        writeColor(y*_width + x, color);
    }

    template <typename number>
    void writeColor(int index, const microgl::color::intensity<number> &color) {
        pixel output;
        microgl::coder::encode<number, pixel_coder>(color, output, coder());
        writeAt(index, output);
    }

    template <typename number>
    void writeColor(int x, int y, const microgl::color::intensity<number> &color) {
        writeColor<number>(y*_width + x, color);
    }

};
#pragma once

#include <microgl/rgba_t.h>
#include <microgl/vec2.h>

namespace microgl {
    namespace sampling {

//        float sdSegment( in vec2 p, in vec2 a, in vec2 b )
//        {
//            vec2 pa = p-a, ba = b-a;
//            float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
//            return length( pa - ba*h );
//        }

        template <typename number, typename rgba_=rgba_t<8,8,8,8>,
                  bool useBigIntegers=true>
        struct lines_sampler {
            using rgba = rgba_;
            using rint_big=int64_t;
            using rint= typename microgl::traits::conditional<useBigIntegers,
                                    int64_t, int32_t>::type;
            using vertex = vec2<number>;
            using ivertex = vec2<rint>;
            static constexpr precision_t p_bits= 15;
            static constexpr precision_t p_bits_double= p_bits<<1;
            static constexpr rint ONE= rint(1)<<p_bits;
            static constexpr rint HALF= ONE>>1;

            struct segment_t {
                ivertex p;
                rint reciprocal_a_dot_b=0; // 1/dot(p1, p2)
            };
            segment_t * _seg=nullptr;

        public:
            lines_sampler() = default;
            ~lines_sampler() {
                delete [] _seg;
            }

            static inline
            rint convert(rint from_value, int from_precision, int to_precision) {
                const int pp= int(from_precision) - int(to_precision);
                if(pp==0) return from_value;
                else if(pp > 0) return from_value>>pp;
                else return from_value<<(-pp);
            }

            unsigned _size=0;
            color_t color1= {255,0,0, 255};
            rint epsilon;
            void updatePoints(vertex * vertices, unsigned size, number $epsilon) {
                delete [] _seg;
                _size=size;
                _seg = new segment_t[size];
                epsilon = microgl::math::to_fixed(($epsilon/number(2))*($epsilon/number(2)), p_bits);
                for (int ix = 0; ix < size; ++ix) {
                    _seg[ix].p.x = microgl::math::to_fixed(vertices[ix].x, p_bits);
                    _seg[ix].p.y = microgl::math::to_fixed(vertices[ix].y, p_bits);
                    if(ix>=1) {
                        const auto ab = _seg[ix-0].p - _seg[ix-1].p;
                        const rint dot = ((ab.x*ab.x)>>p_bits) +
                                         ((ab.y*ab.y)>>p_bits);
                        _seg[ix].reciprocal_a_dot_b = (ONE<<p_bits) / dot;
                    }
                }
            }

            int clamp(const int &v, const int &e0, const int &e1) const
            {
                return v<e0 ? e0 : (v<e1 ? v : e1);
            }

            inline rint sdSegment_squared(const ivertex &p, const ivertex &a,
                                  const ivertex &b, const rint reciprocal_a_dot_b) const
            {
                auto pa = p-a, ba = b-a;
                rint pa_dot_ba = ((pa.x*ba.x)>>p_bits) + ((pa.y*ba.y)>>p_bits);
                const rint div = (pa_dot_ba*reciprocal_a_dot_b)>>(p_bits);
                auto h = div < 0 ? 0 : (div < ONE ? div : ONE);
                auto vv= pa - ivertex((ba.x*h)>>p_bits, (ba.y*h)>>p_bits);
                auto length_squared= ((vv.x*vv.x)>>p_bits) + ((vv.y*vv.y)>>p_bits);
//                return microgl::math::sqrt_64(length_squared<<p_bits);
                return (length_squared);
//                return length( pa - ba*h );
            }

            inline rint linearstep() {

            }

            inline void sample(const int u, const int v,
                               const unsigned bits,
                               color_t &output) const {
                const auto u_tag= convert(u, bits, p_bits);
                const auto v_tag= convert(v, bits, p_bits);
                rint dis=ONE;
                ivertex p{u_tag, v_tag};
                for (int ix = 0; ix < _size-1; ++ix) {
                    rint dis_res = sdSegment_squared(p, _seg[ix].p, _seg[ix+1].p,
                                   _seg[ix+1].reciprocal_a_dot_b);
                    dis = dis < dis_res ? dis : dis_res;
                }
                constexpr rint aa_bits = p_bits - 8 < 0 ? 0 : p_bits - 8;
                constexpr rint aa_band = 1u << aa_bits;

                dis -= epsilon;

                output={255,0,0,0};

                if((dis)<=0) {
                    output={0,0,0, 255};
                } else if (dis < aa_band) {
                    const unsigned char factor = ((color1.a*(aa_band-dis)) >> aa_bits);
                    output={0,0,0, factor};
                }

            }

        private:
        };

    }
}
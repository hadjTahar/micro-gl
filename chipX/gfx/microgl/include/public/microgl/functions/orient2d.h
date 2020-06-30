#pragma once

namespace microgl {
    namespace functions {
        using l64=long long;

//        l64 orient2d(const int &a_x, const int &a_y, const int &b_x, const int &b_y,
//                     const int &c_x, const int &c_y, unsigned char precision) {
//            return ((l64(b_x-a_x)*(c_y-a_y))>>precision) - ((l64(b_y-a_y)*(c_x-a_x))>>precision);
//        }

//        l64 orient2d(const l64 &a_x, const l64 &a_y, const l64 &b_x, const l64 &b_y,
//                     const l64 &c_x, const l64 &c_y, unsigned char precision) {
//            return (((b_x-a_x)*(c_y-a_y)) - ((b_y-a_y)*(c_x-a_x)))>>precision;
//            return (((b_x-a_x)*(c_y-a_y))>>precision) - (((b_y-a_y)*(c_x-a_x))>>precision);
//        }

        template <typename coords, typename result>
        result orient2d(const coords &a_x, const coords &a_y, const coords &b_x, const coords &b_y,
                     const coords &c_x, const coords &c_y, unsigned char precision) {
            return ((result(b_x-a_x)*result(c_y-a_y)) - (result(b_y-a_y)*result(c_x-a_x)))>>precision;
//            return ((l64(b_x-a_x)*l64(c_y-a_y))>>precision) - ((l64(b_y-a_y)*l64(c_x-a_x))>>precision);
        }

        template<typename number>
        inline number
        orient2d(const number &a_x, const number &a_y, const number &b_x, const number &b_y, const number &c_x,
                 const number &c_y) {
            return (b_x-a_x)*(c_y-a_y) - (b_y-a_y)*(c_x-a_x);
        }

    }
}

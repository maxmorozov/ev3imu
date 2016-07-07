#ifndef _MPL_MATH_H
#define _MPL_MATH_H

namespace mpl {
    namespace detail {
        template <unsigned long N, unsigned long P = 0>
        struct log2_impl {
            static const unsigned long value = log2_impl<N / 2, P + 1>::value;
        };

        template <unsigned long P>
        struct log2_impl<0, P> {
            static const unsigned int value = P;
        };

        template <unsigned long P>
        struct log2_impl<1, P> {
            static const unsigned int value = P;
        };
    }

    //Calculates log2 of the specified number
    template <unsigned long N>
    struct log2 {
        static const unsigned long value = detail::log2_impl<N>::value;
    };

    template <unsigned int N>
    struct pow2 {
        static const unsigned long value = 1L << N;
    };

    //Calculates nearest 2^p that is not less than N
    //Rounds to 2^p
    template <unsigned long N>
    struct clp2 {
        static const unsigned long N1 = N - 1;
		static const unsigned long N2 = N1 | (N1 >> 1);
		static const unsigned long N3 = N2 | (N2 >> 2);
        static const unsigned long N4 = N3 | (N3 >> 4);
        static const unsigned long N5 = N4 | (N4 >> 8);
        static const unsigned long N6 = N5 | (N5 >> 16);

        static const unsigned long value = N6 + 1;
    };

    //Checks if the value is a power of 2
    template <unsigned long N>
    struct is_pow2 {
        static const bool value = (N & (N - 1)) == 0;
    };

}

#endif //_MPL_MATH_H
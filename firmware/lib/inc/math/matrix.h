#ifndef __MATH_MATRIX_H
#define __MATH_MATRIX_H

#include <utils/byte_order.h>

namespace math {
    //Represents a dense N x M matrix of 16-bit integers
    //By default assign operation does not perform byte-order correction
	template <
		typename T,
		size_t rows,
		size_t cols,
		typename AssignmentStrategy = utils::ByteCopy>
	struct DenseMatrix {
	    static const int size = rows * cols;

	    T matrix[size];

	    //Initialized the matrix with external data
	    INLINE void assign(const uint8_t* data, uint8_t length) {
            AssignmentStrategy copyer;
            copyer((uint8_t*)matrix, data, length);
	    }

        //Returns size of buffer in bytes
        uint8_t capacity() const { return sizeof(matrix); }

	    operator int16_t*() { return matrix; }
	    operator const int16_t*() const { return matrix; }
	    operator uint8_t*() { return (uint8_t*)matrix; }
	    operator const uint8_t*() const { return (const uint8_t*)matrix; }
	};

}

#endif //__MATH_MATRIX_H
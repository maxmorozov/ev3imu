#ifndef __MATH_CORRECTION_H
#define __MATH_CORRECTION_H

#include <stdint.h>
#include <math/muldiv.h>
#include <utils/inline.h>

namespace math {

    //Transforms the vector of three 16-bit integers by multiplying it to
    //a transformation matrix
	class VectorCorrection {
    private:
        static const uint8_t ROWS = 4;
        static const uint8_t COLUMNS = 3;

        //Returns the matrix for the specified scale
        // matrix - Arrays of matrices for each scale. Each matrix is 4x3 matrix
        const int16_t* getMatrixForScale(const int16_t* matrix, uint8_t scale) const {
            return matrix + uint8_t(scale * ROWS * COLUMNS);
        }

        //calculates a * b / 0x8000
        static int16_t mul(int16_t a, int16_t b) {
            return muldivs16x16_16x(a, b);
        }

        //returns the matrix cell 
        static int16_t get(const int16_t* matrix, uint8_t row, uint8_t col) {
            return matrix[uint8_t(row * COLUMNS) + col];
        }

        //Calculates the matrix expression (data * matrix) * 2
        //The multiplication to 2 controlls the overflow conditons and saturates the result in this case
        //The method produces 16-bit integers in little-endian format
        INLINE static void vector_mul(const int16_t* matrix, const int16_t* data, int16_t* result) {
            result[0] = scale2le(mul(data[0], get(matrix, 0, 0)) + mul(data[1], get(matrix, 1, 0)) + mul(data[2], get(matrix, 2, 0)) + get(matrix, 3, 0));
            result[1] = scale2le(mul(data[0], get(matrix, 0, 1)) + mul(data[1], get(matrix, 1, 1)) + mul(data[2], get(matrix, 2, 1)) + get(matrix, 3, 1));
            result[2] = scale2le(mul(data[0], get(matrix, 0, 2)) + mul(data[1], get(matrix, 1, 2)) + mul(data[2], get(matrix, 2, 2)) + get(matrix, 3, 2));
        }

    public:
        //This method has been put into CPP file to set optimization level to maximum speed
        //The method produces 16-bit integers in little-endian format
        void transform(const int16_t* matrix, uint8_t scale, const int16_t* data, int16_t* result) const;
	};

    //Transformation matrix for the specified device, identified by Tag type
    template <typename Eeprom, Eeprom& eeprom, typename Tag>
    struct Transformation : VectorCorrection {
        INLINE void transform(uint8_t scale, const int16_t* data, int16_t* result) const {
            VectorCorrection::transform(eeprom.template get<Tag>().matrices[0], scale, data, result);
        }
    };
    
}

#endif //__MATH_CORRECTION_H
#include <math/correction.h>

namespace math {
    //This method has been put into CPP file to set optimization level to maximum speed
    //The method produces 16-bit integers in little-endian format
    void VectorCorrection::transform(const int16_t* matrix, uint8_t scale, const int16_t* data, int16_t* result) const {
        vector_mul(getMatrixForScale(matrix, scale), data, result);
    }
}
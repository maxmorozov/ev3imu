#include <vregs.inc>

;-------------------------------------------------------------------------------
	public  muls16x16_32

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
	section `.near_func.text`:CODE:NOROOT(0)
#else
	section `.far_func.text`:CODE:NOROOT(0)
#endif

;-------------------------------------------------------------------------------
; extern "C" int32_t muls16x16_32(int16_t, int16_t);
;
;input in X, Y
;X=a2:a1
;Y=b2:b1
;output in ?w0:?w1
;
;C++ code
;
;int32_t muls16x16_32(int16_t a, int16_t b)
;{
;    bool negative = a < 0;
;    if (negative)
;        a = -a;
;    if (b < 0) {
;        b = -b;
;        negative = !negative;
;    }
;    uint32_t result = mulu16x16_32(a, b);
;    if (negative)
;        result = -result;
;    return result;
;}

muls16x16_32:

      pushw X
      tnzw  X
      jrpl  check_b
      negw  X
check_b:
      tnzw  Y
      jrpl  start
      negw  Y
      cpl   (1, SP)
start:
      pushw X

      ;result.low = a1 * b1
      ld    A, YL
      mul   X, A
      ldw   S:?w1, X

      ;result.high = a2 * b2
      ldw   X, Y
      rrwa  X, A
      ld    A, (1, SP)
      mul   X, A
      ldw   S:?w0, X

      ;Y=a2*b1
      ld    A, YH
      push  A
      ld    A, (2, SP)
      mul   Y, A

      ;X=a1*b2
      pop   A
      popw  X
      mul   X, A

      ;result.high24 += X
      clr   A
      addw  X, S:?b1
      adc   A, S:?b0
      ldw   S:?b1, X

      ;result.high24 += Y
      addw  Y, S:?b1
      adc   A, #0

      ;if (negative)
      ;    result = -result;
      popw  X
      tnzw  X
      jrpl  save_result

      ;calc logical 2's complement of 32-bit integer
      cpl   A
      cplw  Y
      neg   S:?b3
      jrne  save_result
      incw  Y
      jrne  save_result
      inc   A
save_result:
      ldw   S:?b1, Y
      ld    S:?b0, A

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

      end
;*******************************************************************************

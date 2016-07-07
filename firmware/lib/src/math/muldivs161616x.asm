#include <vregs.inc>

;-------------------------------------------------------------------------------
	public  muldivs16x16_16x

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
	section `.near_func.text`:CODE:NOROOT(0)
#else
	section `.far_func.text`:CODE:NOROOT(0)
#endif

;-------------------------------------------------------------------------------
; extern "C" int16_t muldivs16x16_16x(int16_t, int16_t);
;
; Implements following algorithm:
;
;    bool negative = a < 0;
;    if (negative)
;        a = -a;
;    if (b < 0) {
;        b = -b;
;        negative = !negative;
;    }
;
;    result = (uint32_t(a * b) >> 15) & 0xFFFF;
;
;    if (negative)
;       result = -result;
;
; a in range [-32768;32767]
; b is a scaled floating point constant (-2;+2) * 0x4000. The value is in range [-32768;32767]
; the result is in range [-16384;16383]
;
;input in X, Y
;X=a2:a1
;Y=b2:b1
;output in X

muldivs16x16_16x:

;    bool negative = a < 0;
;    if (negative)
;        a = -a;
;    if (b < 0) {
;        b = -b;
;        negative = !negative;
;    }

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

      ;result.high24 += Y
      clr   A
      addw  Y, S:?b1
      adc   A, S:?b0
      ldw   S:?b1, Y

      ;result.high24 += X
      addw  X, S:?b1
      adc   A, #0

      ;result.high24 >> 7
      rrwa  X, A
      sll   A
      rlcw  X

check_negative:
      ;if (negative)
      ;    result = -result;
      popw  Y
      tnzw  Y
      jrpl  exit

      ;calc logical 2’s complement of 16-bit integer
      negw  X
exit:

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

      end
;*******************************************************************************

#include <vregs.inc>

;-------------------------------------------------------------------------------
	public  mulu16x16_32

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
	section `.near_func.text`:CODE:NOROOT(0)
#else
	section `.far_func.text`:CODE:NOROOT(0)
#endif

;-------------------------------------------------------------------------------
; extern "C" int32_t mulu16x16_32(int16_t, int16_t);
;
;input in X, Y
;X=a2:a1
;Y=b2:b1
;output in ?w0:?w1

mulu16x16_32:

      pushw	X

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
      ldw   S:?b1, Y
      ld    S:?b0, A

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

      end
;*******************************************************************************

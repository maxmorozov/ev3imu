#include <vregs.inc>

;-------------------------------------------------------------------------------
	public  scale2

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
	section `.near_func.text`:CODE:NOROOT(0)
#else
	section `.far_func.text`:CODE:NOROOT(0)
#endif

;-------------------------------------------------------------------------------
; extern "C" int16_t scale2(int16_t);
;
; Multiplies the argument by 2
;
; Implements following algorithm:
;
;    bool negative = value < 0;
;
;    value <<= 1;
;
;    if (negative) {
;        if (value > 0)
;            value = SHRT_MIN;
;    } else if (value < 0) {
;        value = SHRT_MAX;
;    }
;    return value;
;
; value in range [-16383;16383]
;
;input in X
;X=a2:a1
;output in X
scale2:
      sllw  X

      jrpl  positive
      jrc   exit              ;negative number
      ldw   X, #$7fff

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

positive:
      jrnc   exit              ;positive number
      ldw   X, #$8000
exit:

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

      end
;*******************************************************************************

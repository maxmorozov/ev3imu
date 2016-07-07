#include <vregs.inc>

;-------------------------------------------------------------------------------
	public  scale2le

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
	section `.near_func.text`:CODE:NOROOT(0)
#else
	section `.far_func.text`:CODE:NOROOT(0)
#endif

;-------------------------------------------------------------------------------
; extern "C" int16_t scale2le(int16_t);
;
; Multiplies the argument by 2 and converts the result to little-endian format
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
;    return swap_bytes(value);
;
; value in range [-16383;16383]
;
;input in X
;output in X
scale2le:
      sllw  X

      jrpl  positive
      jrc   exit              ;negative number
      ldw   X, #$ff7f

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

positive:
      jrnc  exit              ;positive number
      ldw   X, #$8000
exit:
      swapw X

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

      end
;*******************************************************************************

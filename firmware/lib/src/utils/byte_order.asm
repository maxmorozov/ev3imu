#include <vregs.inc>

;-------------------------------------------------------------------------------
	public  swap_bytes

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
	section `.near_func.text`:CODE:NOROOT(0)
#else
	section `.far_func.text`:CODE:NOROOT(0)
#endif

;-------------------------------------------------------------------------------
; extern "C" int16_t swap_bytes(int16_t);
;
; Converts 16-bit integer to little-endian format
;
;input in X
;output in X
swap_bytes:
      swapw  X

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

      end
;*******************************************************************************

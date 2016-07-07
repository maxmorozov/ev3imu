#include <vregs.inc>

;-------------------------------------------------------------------------------
	public  swap_sample

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
	section `.near_func.text`:CODE:NOROOT(0)
#else
	section `.far_func.text`:CODE:NOROOT(0)
#endif

;-------------------------------------------------------------------------------
; extern "C" void swap_sample(int16_t (&data)[3]);
;
; Converts 16-bit integer to little-endian format
;
;input in X
;output in X
swap_sample:
      LDW     Y, X
      LDW     Y, (Y)
      SWAPW   Y
      LDW     (X), Y

      LDW     Y, X
      LDW     Y, (2,Y)
      SWAPW   Y
      LDW     (2,X), Y

      LDW     Y, X
      LDW     Y, (4, Y)
      SWAPW   Y
      LDW     (4,X), Y

#if __CODE_MODEL__ == __SMALL_CODE_MODEL__
      ret
#else
      retf
#endif

      end
;*******************************************************************************

#ifndef __UTILS_INLINE_H
#define __UTILS_INLINE_H

#ifndef INLINE
#define INLINE _Pragma("inline=forced") inline
#endif

#ifndef NOINLINE
#define NOINLINE _Pragma("inline=never")
#endif

#endif //__UTILS_INLINE_H
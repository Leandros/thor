
#ifndef BITS_RESTRICT_H
#define BITS_RESTRICT_H

#if defined(_MSC_VER)
#define __restrict__ __restrict
#else
#define __restrict__
#endif

#endif /* BITS_RESTRICT_H */


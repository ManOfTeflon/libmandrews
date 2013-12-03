#ifndef __MACRO_FOR_H
#define __MACRO_FOR_H

#define SUBSETS(i, s)		for(uint64 i = 0ull, null_flag = 1ull; \
                            i || null_flag; \
                            i = (i-1) & (s), null_flag = 0ull)
#define STEP(i, l, h, s)	for(int i=l;i<h;i+=s)
#define ISTEP(i, l, h, s)	for(int i=h - 1;i >= l;i-=s)
#define FOR(i, l, h)		STEP(i, l, h, 1)
#define IFOR(i, l, h)		ISTEP(i, l, h, 1)
#define REP(i, h)		FOR(i, 0, h)
#define IREP(i, h)		IFOR(i, 0, h)
#define RANGE(p, l, h)		(p >= l && p < h)
#define IN(i, b)		for(Board i = least(b);i;b^=i,i=least(b))
#define SZ(x) 			((int)x.length())

#define INF			32767

#define ARRAYSIZE(ary) (sizeof(ary) / sizeof(*ary))

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

#define FIRST_(first, ...) \
    first
#define FIRST(...) \
    FIRST_(__VA_ARGS__)

#define APPEND(...) \
    APPEND_(__VA_ARGS__,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,)
#define APPEND_(a1, a2, a3, a4, a5, a6, a7, a8, a9, \
                  a10, a11, a12, a13, a14, a15, a16, a17, a18, \
                  a19, a20, a21, a22, a23, a24, a25, a26, a27, \
                  a28, a29, a30, a31, a32, a33, a34, a35, a36, \
                  a37, a38, a39, a40, a41, a42, a43, a44, a45, ...) \
a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 \
a16 a17 a18 a19 a20 a21 a22 a23 a24 a25 a26 a27 a28 a29 a30 \
a31 a32 a33 a34 a35 a36 a37 a38 a39 a40 a41 a42 a43 a44 a44

#define ARG_45(a1, a2, a3, a4, a5, a6, a7, a8, a9, \
                  a10, a11, a12, a13, a14, a15, a16, a17, a18, \
                  a19, a20, a21, a22, a23, a24, a25, a26, a27, \
                  a28, a29, a30, a31, a32, a33, a34, a35, a36, \
                  a37, a38, a39, a40, a41, a42, a43, a44, a45, ...) a45

#define HAS_ARGS(...) \
    ARG_45(dummy, ##__VA_ARGS__, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, NO)

#define NUM_ARGS(...) \
    ARG_45(dummy, ##__VA_ARGS__, 43 , 42 , 41 , 40 , 39 , 38 , 37 , 36 , 35 , 34 , 33 , 32 , 31 , 30 , 29 , 28 , 27 , 26 , 25 , 24 , 23 , 22 , 21 , 20 , 19 , 18 , 17 , 16 , 15 , 14 , 13 , 12 , 11 , 10 , 9 , 8 , 7 , 6 , 5 , 4 , 3 , 2 , 1 , 0)

#define MAP_REDUCE(MAP, REDUCE, ...) \
    CONCAT(MAP_REDUCE_, NUM_ARGS(__VA_ARGS__))(MAP, REDUCE, __VA_ARGS__)

#define MAP_REDUCE_0(...)
#define MAP_REDUCE_1(MAP, REDUCE, a0, ...) MAP(a0)
#define MAP_REDUCE_2(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_1(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_3(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_2(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_4(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_3(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_5(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_4(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_6(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_5(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_7(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_6(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_8(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_7(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_9(MAP, REDUCE, a0, ...) REDUCE(MAP(a0),  MAP_REDUCE_8(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_10(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_9(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_11(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_10(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_12(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_11(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_13(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_12(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_14(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_13(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_15(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_14(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_16(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_15(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_17(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_16(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_18(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_17(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_19(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_18(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_20(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_19(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_21(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_20(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_22(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_21(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_23(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_22(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_24(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_23(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_25(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_24(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_26(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_25(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_27(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_26(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_28(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_27(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_29(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_28(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_30(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_29(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_31(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_30(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_32(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_31(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_33(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_32(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_34(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_33(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_35(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_34(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_36(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_35(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_37(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_36(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_38(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_37(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_39(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_38(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_40(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_39(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_41(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_40(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_42(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_41(MAP, REDUCE, __VA_ARGS__))
#define MAP_REDUCE_43(MAP, REDUCE, a0, ...) REDUCE(MAP(a0), MAP_REDUCE_42(MAP, REDUCE, __VA_ARGS__))

#include "output.h"

#endif  // __MACRO_FOR_H

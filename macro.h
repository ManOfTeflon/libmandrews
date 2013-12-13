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

struct Auto {
    Auto(std::function<void(void)> func) : m_func(func) { }
    ~Auto() {
        m_func();
    }
private:
    std::function<void(void)> m_func;
};
#define AUTO(expr) \
    Auto CONCAT(__auto__, __COUNTER__)([]{ expr })

#define IDENTITY(a) a
#define STRINGIFY(a) #a
#define REVERSE__REDUCE(a, b) b, a
#define COMMA_SEPARATE(a, b) a, b
#define SPACE_SEPARATE(a, b) a b
#define SEMICOLON_SEPARATE(a, b) a; b
#define EQUALS_SEPARATE(a, b) a = b

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

#define FIRST_(first, ...) \
    first
#define FIRST(...) \
    FIRST_(__VA_ARGS__)
#define REST_(first, ...) \
    __VA_ARGS__
#define REST(...) \
    REST_(__VA_ARGS__)

#define LAST(...) \
    MAP_REDUCE(IDENTITY, REST, __VA_ARGS__)

#define APPEND(...) \
    MAP_REDUCE(IDENTITY, SPACE_SEPARATE, __VA_ARGS__)

#define REVERSE(...) \
    MAP_REDUCE(IDENTITY, REVERSE__REDUCE, __VA_ARGS__)

#define ARG_45(a1, a2, a3, a4, a5, a6, a7, a8, a9, \
                  a10, a11, a12, a13, a14, a15, a16, a17, a18, \
                  a19, a20, a21, a22, a23, a24, a25, a26, a27, \
                  a28, a29, a30, a31, a32, a33, a34, a35, a36, \
                  a37, a38, a39, a40, a41, a42, a43, a44, a45, ...) a45

#define HAS_ARGS(...) \
    ARG_45(dummy, ##__VA_ARGS__, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, YES, NO)

#define NUM_ARGS(...) \
    ARG_45(dummy, ##__VA_ARGS__, 43 , 42 , 41 , 40 , 39 , 38 , 37 , 36 , 35 , 34 , 33 , 32 , 31 , 30 , 29 , 28 , 27 , 26 , 25 , 24 , 23 , 22 , 21 , 20 , 19 , 18 , 17 , 16 , 15 , 14 , 13 , 12 , 11 , 10 , 9 , 8 , 7 , 6 , 5 , 4 , 3 , 2 , 1 , 0)

#define DEFAULT__YES(D, ...) __VA_ARGS__
#define DEFAULT__NO(D, ...) D
#define DEFAULT(D, ...) \
    CONCAT(DEFAULT__, HAS_ARGS(__VA_ARGS__))(D, __VA_ARGS__)

#define MAP_REDUCE(M_, R_, ...) \
    CONCAT(MAP_REDUCE_, NUM_ARGS(__VA_ARGS__))(M_, R_, __VA_ARGS__)

#define  MAP_REDUCE_0(...)
#define  MAP_REDUCE_1(M_, R_, a0, ...) M_(a0)
#define  MAP_REDUCE_2(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_1(M_, R_, __VA_ARGS__))
#define  MAP_REDUCE_3(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_2(M_, R_, __VA_ARGS__))
#define  MAP_REDUCE_4(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_3(M_, R_, __VA_ARGS__))
#define  MAP_REDUCE_5(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_4(M_, R_, __VA_ARGS__))
#define  MAP_REDUCE_6(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_5(M_, R_, __VA_ARGS__))
#define  MAP_REDUCE_7(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_6(M_, R_, __VA_ARGS__))
#define  MAP_REDUCE_8(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_7(M_, R_, __VA_ARGS__))
#define  MAP_REDUCE_9(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_8(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_10(M_, R_, a0, ...) R_(M_(a0),  MAP_REDUCE_9(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_11(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_10(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_12(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_11(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_13(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_12(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_14(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_13(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_15(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_14(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_16(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_15(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_17(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_16(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_18(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_17(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_19(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_18(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_20(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_19(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_21(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_20(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_22(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_21(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_23(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_22(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_24(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_23(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_25(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_24(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_26(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_25(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_27(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_26(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_28(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_27(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_29(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_28(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_30(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_29(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_31(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_30(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_32(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_31(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_33(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_32(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_34(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_33(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_35(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_34(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_36(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_35(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_37(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_36(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_38(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_37(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_39(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_38(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_40(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_39(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_41(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_40(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_42(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_41(M_, R_, __VA_ARGS__))
#define MAP_REDUCE_43(M_, R_, a0, ...) R_(M_(a0), MAP_REDUCE_42(M_, R_, __VA_ARGS__))

#define REPEAT(N, A) \
    CONCAT(REPEAT_, N)(IDENTITY, COMMA_SEPARATE, A)

#define REPEAT_0(...)
#define REPEAT_1(MAP, REDUCE, a0) MAP(a0)
#define REPEAT_2(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_1(MAP, REDUCE, a0))
#define REPEAT_3(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_2(MAP, REDUCE, a0))
#define REPEAT_4(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_3(MAP, REDUCE, a0))
#define REPEAT_5(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_4(MAP, REDUCE, a0))
#define REPEAT_6(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_5(MAP, REDUCE, a0))
#define REPEAT_7(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_6(MAP, REDUCE, a0))
#define REPEAT_8(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_7(MAP, REDUCE, a0))
#define REPEAT_9(MAP, REDUCE, a0) REDUCE(MAP(a0),  REPEAT_8(MAP, REDUCE, a0))
#define REPEAT_10(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_9(MAP, REDUCE, a0))
#define REPEAT_11(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_10(MAP, REDUCE, a0))
#define REPEAT_12(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_11(MAP, REDUCE, a0))
#define REPEAT_13(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_12(MAP, REDUCE, a0))
#define REPEAT_14(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_13(MAP, REDUCE, a0))
#define REPEAT_15(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_14(MAP, REDUCE, a0))
#define REPEAT_16(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_15(MAP, REDUCE, a0))
#define REPEAT_17(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_16(MAP, REDUCE, a0))
#define REPEAT_18(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_17(MAP, REDUCE, a0))
#define REPEAT_19(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_18(MAP, REDUCE, a0))
#define REPEAT_20(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_19(MAP, REDUCE, a0))
#define REPEAT_21(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_20(MAP, REDUCE, a0))
#define REPEAT_22(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_21(MAP, REDUCE, a0))
#define REPEAT_23(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_22(MAP, REDUCE, a0))
#define REPEAT_24(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_23(MAP, REDUCE, a0))
#define REPEAT_25(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_24(MAP, REDUCE, a0))
#define REPEAT_26(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_25(MAP, REDUCE, a0))
#define REPEAT_27(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_26(MAP, REDUCE, a0))
#define REPEAT_28(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_27(MAP, REDUCE, a0))
#define REPEAT_29(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_28(MAP, REDUCE, a0))
#define REPEAT_30(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_29(MAP, REDUCE, a0))
#define REPEAT_31(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_30(MAP, REDUCE, a0))
#define REPEAT_32(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_31(MAP, REDUCE, a0))
#define REPEAT_33(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_32(MAP, REDUCE, a0))
#define REPEAT_34(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_33(MAP, REDUCE, a0))
#define REPEAT_35(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_34(MAP, REDUCE, a0))
#define REPEAT_36(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_35(MAP, REDUCE, a0))
#define REPEAT_37(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_36(MAP, REDUCE, a0))
#define REPEAT_38(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_37(MAP, REDUCE, a0))
#define REPEAT_39(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_38(MAP, REDUCE, a0))
#define REPEAT_40(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_39(MAP, REDUCE, a0))
#define REPEAT_41(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_40(MAP, REDUCE, a0))
#define REPEAT_42(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_41(MAP, REDUCE, a0))
#define REPEAT_43(MAP, REDUCE, a0) REDUCE(MAP(a0), REPEAT_42(MAP, REDUCE, a0))

#include "output.h"

#endif  // __MACRO_FOR_H

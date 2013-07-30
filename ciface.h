#ifndef CIFACE_H
#define CIFACE_H

/* Compositional Interfaces */

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/transform.hpp>

#include <boost/preprocessor/list/rest_n.hpp>
#include <boost/preprocessor/list/for_each.hpp>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>

#include <boost/preprocessor/variadic/to_seq.hpp>

#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>

#include <assert.h>
#include <stddef.h>

/* detail */

#define CIFACE_rem(...) __VA_ARGS__
#define CIFACE_eat(...)

#define CIFACE_strip(X) CIFACE_eat X
#define CIFACE_pair(X) CIFACE_rem X

//////////////////////////////////////////////////////////////////////////////

#define RTTI(NAME) BOOST_PP_CAT(ciface_rtti_, NAME)
#define CHECK(NAME) BOOST_PP_CAT(ciface_check_, NAME)

#define GET_RTTI(NAME, PTR) \
    (*(const struct RTTI(NAME) *const *)PTR)

#define VOID void,

#define RTTI_PTR(NAME) const struct RTTI(NAME) *const

#define IFACE(NAME, ...) \
    struct NAME; \
    struct RTTI(NAME) { \
        const size_t offset; \
        const struct { \
            CIFACE_VTABLE(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
        } vtable; \
    }; \
    static inline void \
    CHECK(NAME) (const struct NAME *const self) { \
        assert(self); \
        const struct RTTI(NAME) *const rtti = GET_RTTI(NAME, self); \
        assert(rtti); \
        CIFACE_CHECK(rtti, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    } \
    CIFACE_METHODS(NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define CIFACE_VTABLE(METHODS) \
    BOOST_PP_SEQ_FOR_EACH(CIFACE_VTABLE_EACH, ~, METHODS)

#define CIFACE_VTABLE_EACH(R, DATA, METHOD) \
    RETURN_TYPE(METHOD) (*const SYMBOL(METHOD)) \
    (ENUM_PARAMS(void *self, METHOD));

#define CIFACE_CHECK(PTR, METHODS) \
    BOOST_PP_SEQ_FOR_EACH(CIFACE_CHECK_EACH, PTR, METHODS)

#define CIFACE_CHECK_EACH(R, PTR, METHOD) \
    assert((PTR)->vtable.SYMBOL(METHOD));

#define CIFACE_METHODS(NAME, METHODS) \
    BOOST_PP_SEQ_FOR_EACH(CIFACE_METHOD_EACH, NAME, METHODS)

#define CIFACE_METHOD_EACH(R, NAME, METHOD) \
    static inline RETURN_TYPE(METHOD) \
    SYMBOL(METHOD) \
    (ENUM_PARAMS(const struct NAME *const self, METHOD)) { \
        CHECK(NAME)(self); \
        TO_RETURN_OR_NOT_TO_RETURN(METHOD) \
            GET_RTTI(NAME, self)->vtable.SYMBOL(METHOD) \
                (ENUM_ARGS(self, NAME, METHOD)); \
    }

#define TO_RETURN_OR_NOT_TO_RETURN(METHOD) \
    BOOST_PP_IF(IS_VOID(METHOD), BOOST_PP_EMPTY, RETURN_KEYWORD)()

#define RETURN_KEYWORD() return

#define IS_VOID(METHOD) BOOST_PP_DEC(BOOST_PP_TUPLE_SIZE(BOOST_PP_TUPLE_ELEM(0, METHOD)))
#define RETURN_TYPE(METHOD) BOOST_PP_TUPLE_ELEM(0, BOOST_PP_TUPLE_ELEM(0, METHOD))
#define SYMBOL(METHOD) BOOST_PP_TUPLE_ELEM(1, METHOD)
#define PARAM_LIST(METHOD) \
    BOOST_PP_LIST_REST_N(2, BOOST_PP_TUPLE_TO_LIST(METHOD))

#define ENUM_PARAMS(SELF, METHOD) \
    SELF BOOST_PP_LIST_FOR_EACH(PARAM_EACH, ~, PARAM_LIST(METHOD))

#define PARAM_EACH(R, DATA, PARAM) \
    , CIFACE_pair(PARAM)

#define ENUM_ARGS(SELF, NAME, METHOD) \
    ((void *)((char *)self - GET_RTTI(NAME, self)->offset)) \
    BOOST_PP_LIST_FOR_EACH(ARG_EACH, ~, PARAM_LIST(METHOD))

#define ARG_EACH(R, DATA, PARAM) \
    , CIFACE_strip(PARAM)

#define INITIAL(TYPE, NAME, IFACE, FUNC, ...) \
    struct NAME FUNC (void) { \
        static struct RTTI(NAME) rtti = { \
            .offset = offsetof(TYPE, IFACE), \
            .vtable = { \
                CIFACE_INIT_VTABLE(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
            } \
        }; \
        struct NAME iface = { \
            &rtti \
        }; \
        CHECK(NAME)(&iface); \
        return iface; \
    }

#define CIFACE_INIT_VTABLE(METHODS) \
    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(ADD_DOT, ~, METHODS))

#define ADD_DOT(R, DATA, METHOD) .METHOD

#endif

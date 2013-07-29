#ifndef CIFACE_H
#define CIFACE_H

/* Compositional Interfaces */

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/list/rest_n.hpp>
#include <boost/preprocessor/tuple/to_list.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include <assert.h>
#include <stddef.h>

/* detail */

#define CIFACE_rem(...) __VA_ARGS__
#define CIFACE_eat(...)

#define CIFACE_strip(X) CIFACE_eat X
#define CIFACE_pair(X) CIFACE_rem X

//////////////////////////////////////////////////////////////////////////////

#define CIFACE(NAME, ...) \
    typedef struct NAME NAME; \
    struct BOOST_PP_CAT(ciface_rtti_, NAME) { \
        const size_t offset; \
        const struct { \
            CIFACE_VTABLE(NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
        } vtable; \
    }; \
    struct NAME { \
        const struct BOOST_PP_CAT(ciface_rtti_, NAME) *const _rtti; \
    }; \
    static inline void \
    BOOST_PP_CAT(ciface_check_, NAME) (const NAME *const self) { \
        assert(self); \
        assert(self->_rtti); \
        CIFACE_CHECK(NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    } \
    CIFACE_METHODS(NAME, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define CIFACE_VTABLE(NAME, METHODS) \
    BOOST_PP_SEQ_FOR_EACH(CIFACE_VTABLE_EACH, NAME, METHODS)

#define CIFACE_CHECK(NAME, METHODS) \
    BOOST_PP_SEQ_FOR_EACH(CIFACE_CHECK_EACH, ~, METHODS)

#define CIFACE_CHECK_EACH(R, DATA, METHOD) \
    assert(self->_rtti->vtable.SYMBOL(METHOD));

#define CIFACE_METHODS(NAME, METHODS) \
    BOOST_PP_SEQ_FOR_EACH(CIFACE_METHOD_EACH, NAME, METHODS)

#define CIFACE_VTABLE_EACH(R, NAME, METHOD) \
    RETURN_TYPE(METHOD) (*const SYMBOL(METHOD)) \
    (ENUM_PARAMS(void, METHOD));

#define CIFACE_METHOD_EACH(R, NAME, METHOD) \
    static inline RETURN_TYPE(METHOD) SYMBOL(METHOD) \
    (ENUM_PARAMS(const NAME, METHOD)) { \
        BOOST_PP_CAT(ciface_check_, NAME)(self); \
        BOOST_PP_IF(IS_VOID(METHOD), BOOST_PP_EMPTY, RETURN_KEYWORD)() \
            self->_rtti->vtable.SYMBOL(METHOD)(ENUM_ARGS(METHOD)); \
    }

#define RETURN_KEYWORD() return

#define IS_VOID(METHOD) BOOST_PP_TUPLE_ELEM(0, METHOD)
#define RETURN_TYPE(METHOD) BOOST_PP_TUPLE_ELEM(1, METHOD)
#define SYMBOL(METHOD) BOOST_PP_TUPLE_ELEM(2, METHOD)
#define PARAM_LIST(METHOD) \
    BOOST_PP_LIST_REST_N(3, BOOST_PP_TUPLE_TO_LIST(METHOD))

#define ENUM_PARAMS(NAME, METHOD) \
    NAME *self BOOST_PP_LIST_FOR_EACH(PARAM_EACH, ~, PARAM_LIST(METHOD))

#define PARAM_EACH(R, DATA, PARAM) \
    , CIFACE_pair(PARAM)

#define ENUM_ARGS(METHOD) \
    ((void *)((char *)self - self->_rtti->offset)) \
    BOOST_PP_LIST_FOR_EACH(ARG_EACH, ~, PARAM_LIST(METHOD))

#define ARG_EACH(R, DATA, PARAM) \
    , CIFACE_strip(PARAM)

#define RETURNS(TYPE) 0, TYPE
#define VOID 1, void

#define CIFACE_INIT_FWD(TYPE, IFACE) \
    CIFACE_INIT_PROTOTYPE(TYPE, IFACE);

#define CIFACE_INIT_PROTOTYPE(TYPE, IFACE) \
    IFACE \
    BOOST_PP_CAT(TYPE, BOOST_PP_CAT(_, BOOST_PP_CAT(IFACE, _init))) (void)

#define CIFACE_INIT(TYPE, IFACE, ...) \
    CIFACE_INIT_PROTOTYPE(TYPE, IFACE) { \
        static struct BOOST_PP_CAT(ciface_rtti_, IFACE) rtti = { \
            .offset = offsetof(TYPE, IFACE), \
            .vtable = { \
                CIFACE_INIT_VTABLE(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
            } \
        }; \
        IFACE ciface = { \
            ._rtti = &rtti \
        }; \
        BOOST_PP_CAT(ciface_check_, IFACE)(&ciface); \
        return ciface; \
    }

#define CIFACE_INIT_VTABLE(METHODS) \
    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(ADD_DOT, ~, METHODS))

#define ADD_DOT(R, DATA, METHOD) .METHOD

#endif

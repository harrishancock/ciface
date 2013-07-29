#include "ciface.h"

#include "stdio.h"

CIFACE(link,
        (RETURNS(int), link_connect, (char *) address),
        (RETURNS(int), link_disconnect),
        (VOID, link_poke)
      );

//////////////////////////////////////////////////////////////////////////////

#if 0
#define DERIVE(X) ((void *)((char *)(X) - (X)->_rtti->offset))

typedef struct link link;

struct ciface_rtti_link {
    const size_t offset;
    const struct {
        int (*const link_connect) (void *self, void *address);
        int (*const link_disconnect) (void *self);
    } vtable;
};

struct link {
    const struct ciface_rtti_link *const _rtti;
};

static inline void ciface_check_link (const link *const self) {
    assert(self);
    assert(self->_rtti);
    assert(self->_rtti->vtable.link_connect);
    assert(self->_rtti->vtable.link_disconnect);
}

static inline int link_connect (const link *const self, void *address) {
    ciface_check_link(self);
    return self->_rtti->vtable.link_connect(DERIVE(self), address);
}

static inline int link_disconnect (const link *const self) {
    ciface_check_link(self);
    return self->_rtti->vtable.link_disconnect(DERIVE(self));
}
#endif

//////////////////////////////////////////////////////////////////////////////

/* dongle.h */

/* Declare a link interface inside a user-defined struct called dongle. */

typedef struct dongle {
    int fd;
    const link link;
} dongle;

/* Now dongle objects with *properly-initialized* link interfaces can be used
 * as links. */

/* This next line declares the prototype:
 *
 * link dongle_link_init (void);
 *
 * It is not necessary if dongle_link_init() will only be called from one
 * translation unit. */

CIFACE_INIT_FWD(dongle, link);

//////////////////////////////////////////////////////////////////////////////

/* dongle.c */

/* Implement the interface methods. Note the ciface/self pattern. */

int dongle_link_connect (void *ciface, char *address) {
    dongle *self = ciface;

    printf("dongle_link_connect() self<%p>\n", self);
    printf("dongle_link_connect() fd<%d> address<%s>\n", self->fd, (char *)address);
    self->fd = 123;
    return 0;
}

int dongle_link_disconnect (void *ciface) {
    dongle *self = ciface;

    printf("dongle_link_disconnect() self<%p>\n", self);
    printf("dongle_link_disconnect() fd<%d>\n", self->fd);
    return 0;
}

void dongle_link_poke (void *ciface) {
    dongle *self = ciface;

    printf("dongle_link_poke() self<%p>\n", self);
    printf("bitch, I've killed for less\n");
}

/* Declare dongle_link_init(). This function returns a structure which can be
 * used to properly initialize the link interface of a dongle. */

CIFACE_INIT(dongle, link,
        link_connect = dongle_link_connect,
        link_disconnect = dongle_link_disconnect,
        link_poke = dongle_link_poke
        );

/* The function it declares looks like so:
 *
 * link dongle_link_init () {
 *     static struct ciface_rtti_link rtti = {
 *         .offset = offsetof(dongle, link),
 *         .vtable = {
 *             .link_connect = dongle_link_connect,
 *             .link_disconnect = dongle_link_disconnect,
 *             .link_poke = dongle_link_poke
 *         }
 *     };
 *
 *     link ciface = {
 *         ._rtti = &rtti,
 *     };
 *
 *     ciface_check_link(&ciface);
 *
 *     return ciface;
 * }
 */

int main () {
    /* Finally, to declare and initialize a dongle and its link interface: */

    dongle d = {
        .link = dongle_link_init(),
        .fd = 321
    };

    printf("main() &d<%p>\n", &d);

    /* And to use it, simply take the address of its interface, and pass that
     * to the interface methods. */

    link_connect(&d.link, "Hello, world!");
    link_disconnect(&d.link);
    link_poke(&d.link);
}

#include "ciface.h"

#include "stdio.h"

//////////////////////////////////////////////////////////////////////////////

/* link.h */

IFACE(link,
        /* Virtual functions. You may declare other, regular functions
         * elsewhere, as long as the symbols (link_open, etc.) don't conflict.
         * Virtual functions are described with an n-tuple where n >= 2. The
         * first element of the tuple is the wrapped return type. void
         * functions must use (VOID) as their wrapped return type; all others
         * must use (t), where t is their actual return type. The
         * second element in the tuple is the identifier to use for this
         * virtual function. A short static inline function of that name is
         * defined for you, whose only purpose is to forward control to the
         * implementation function. The rest of the tuple elements, if any,
         * are the virtual function's parameters, in the format (t) i, where
         * t is the parameter's type, and i is the parameter's identifier. */
        ((int), link_open, (char *) address),
        ((int), link_close),
        ((int), link_read, (char *) buf, (size_t) len),
        ((int), link_write, (const char *) buf, (size_t) len),
        ((VOID), link_poke)
      );

struct link {
    /* An interface's RTTI pointer may be named whatever you want; the only
     * constraint is that it be the FIRST member of the interface struct. */
    RTTI_PTR(link) _rtti;

    /* Put whatever other shit you want here. Keep in mind, however, that
     * everything you put here will bloat every instance of every containing
     * type that implements this interface. Also, you can only access any
     * members of this interface from within any non-virtual functions which
     * you create for this interface. Or at least, you SHOULD do so. At any
     * rate, I recommend the only extra stuff you put into an interface should
     * be other interfaces--which you might as well access from the containing
     * type's scope. That seems reasonable enough. */
};

IFACE_POST(link, _rtti);

static inline struct link link_initial (RTTI_PTR(link) rtti) {
    struct link l = {
        ._rtti = rtti
    };

    return l;
}

//////////////////////////////////////////////////////////////////////////////

/* dongle.h */

typedef struct dongle {
    int fd;
    size_t bytes_read;
    size_t bytes_written;
    const struct link link;
} dongle;

int dongle_open (void *_self, char *address);
int dongle_close (void *_self);
int dongle_read (void *_self, char *buf, size_t len);
int dongle_write (void *_self, const char *buf, size_t len);
void dongle_poke (void *_self);

/* dongle.c */

int dongle_open (void *_self, char *address) {
    dongle *self = _self;

    printf("dongle_open() self<%p>\n", self);
    printf("dongle_open() fd<%d> address<%s>\n", self->fd, (char *)address);
    self->fd = 123;
    return 0;
}

int dongle_close (void *_self) {
    dongle *self = _self;

    printf("dongle_close() self<%p>\n", self);
    printf("dongle_close() fd<%d>\n", self->fd);
    return 0;
}

int dongle_read (void *_self, char *buf, size_t len) {
    dongle *self = _self;

    self->bytes_read += len;

    printf("dongle_close() self<%p>\n", self);
    printf("dongle_close() bytes_read<%zu>\n", self->bytes_read);
    return 0;
}

int dongle_write (void *_self, const char *buf, size_t len) {
    dongle *self = _self;

    self->bytes_written += len;

    printf("dongle_close() self<%p>\n", self);
    printf("dongle_close() bytes_written<%zu>\n", self->bytes_written);
    return 0;
}

void dongle_poke (void *_self) {
    dongle *self = _self;

    printf("dongle_poke() self<%p>\n", self);
    printf("bitch, I've killed for less\n");
}

/* The CIFACE_INIT macro declares: */
#if 0
link dongle_link_init (void) {
    /* magic */
    return dongle_link_prototype;
}
#endif
INITIAL(dongle, link, link, dongle_link_initial,
        link_open = dongle_open,
        link_close = dongle_close,
        link_read = dongle_read,
        link_write = dongle_write,
        link_poke = dongle_poke
        );

dongle dongle_initial (void) {
    dongle d = {
        .fd = 321,
        .bytes_read = 0,
        .bytes_written = 0,
        .link = dongle_link_initial()
    };

    return d;
}

//////////////////////////////////////////////////////////////////////////////

int main () {
    /* Finally, to declare and initialize a dongle and its link interface: */

    dongle d = dongle_initial();

    printf("main() &d<%p>\n", &d);

    /* And to use it, simply take the address of its interface, and pass that
     * to the interface methods. */

    link_open(&d.link, "Hello, world!");
    link_close(&d.link);
    link_read(&d.link, NULL, 27);
    link_read(&d.link, NULL, 34);
    link_write(&d.link, NULL, 200);
    link_write(&d.link, NULL, 50);
    link_poke(&d.link);
}

//////////////////////////////////////////////////////////////////////////////

#if 0
struct link;

struct ciface_rtti_link {
    const size_t offset;
    const struct {
        int (*const link_open) (void *self, void *address);
        int (*const link_close) (void *self);
    } vtable;
};

static inline void ciface_check_link (const struct ciface_rtti_link *const rtti) {
    assert(rtti);
    assert(rtti->vtable.link_open);
    assert(rtti->vtable.link_close);
}

#define DERIVE(X) ((void *)((char *)(X) - rtti->offset))

static inline int link_open (const struct link *const self, void *address) {
    assert(self);
    const struct ciface_rtti_link *const rtti = *(ciface_rtti_link **)self;
    ciface_check_link(rtti);
    return rtti->vtable.link_open(DERIVE(self), address);
}

static inline int link_close (const struct link *const self) {
    assert(self);
    const struct ciface_rtti_link *const rtti = *(ciface_rtti_link **)self;
    ciface_check_link(rtti);
    return rtti->vtable.link_close(DERIVE(self));
}

#endif

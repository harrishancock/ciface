/* TODO: Allow "const" functions to be specified. */

#define CLASS(...)
#define VTABLE(...)
#define CLASS_POST(...)

CLASS(show,
        ((const char *), show_to_string),
        ((int), show_fprint, (FILE *) stream),
        ((int), show_snprint, (char *) str, (size_t) size)
     );

struct show;

struct _class_vtable_show {
    const size_t offset;
    const struct {
        const char *(*const show_to_string) (void *self);
        int (*const show_fprint) (void *self, FILE *stream);
        int (*const show_snprint) (void *self, char * str, size_t size);
    } methods;
};

static const struct _class_vtable_show *_class_vtable_show (struct show *self);

static inline const char *show_to_string (struct show *self) {
    const struct _class_vtable_show *const vtable = _class_vtable_show(self);
    return vtable->methods.show_to_string((void *)((char *)self - vtable->offset));
}

static inline int show_fprint (struct show *self, FILE *stream) {
    const struct _class_vtable_show *const vtable = _class_vtable_show(self);
    return vtable->methods.show_to_string((void *)((char *)self - vtable->offset),
            stream);
}

static inline int show_snprintf (struct show *self, char *str, size_t size) {
    const struct _class_vtable_show *const vtable = _class_vtable_show(self);
    return vtable->methods.show_snprintf((void *)((char *)self - vtable->offset),
            str, size);
}

static inline void _class_check_vtable_show (const struct _class_vtable_show *const vtable) {
    assert(vtable);
    assert(vtable->show_to_string);
    assert(vtable->show_fprint);
    assert(vtable->show_snprint);
}

struct show {
    CLASS_VTABLE(show) _vtable;
};

CLASS_POST(show, _vtable);

static inline const struct _class_vtable_show *_class_vtable_show (struct show *self) {
    assert(self);
    _class_check_vtable_show(self->vtable);
    return self->_vtable;
}

//////////////////////////////////////////////////////////////////////////////

CLASS(num,
        ((int), num_add, (int) a, (int) b),
        ((int), num_multiply, (int) a, (int) b)
     );

/* User-written */
struct num {
    struct show show_interface;
    CLASS_VTABLE(num) _vtable;
};

const char *num_to_string (void *_self);
int num_fprint (void *_self, FILE *stream);
int num_snprint (void *_self, char *str, size_t size);

int num_add (int a, int b);
int num_multiply (int a, int b);

CLASS_CTOR(num, _vtable, (),
        (show, show_interface,
         .show_to_string = num_to_string,
         .show_fprint = num_fprint,
         .show_snprint = num_snprint
        )
        );

struct _class_vtable_show *_class_override_num_show (void) {
    static const struct _class_vtable_show vtable = {
        .offset = offsetof(num, show_interface),
        .methods = {
            .show_to_string = num_to_string,
            .show_fprint = num_fprint,
            .show_snprint = num_snprint
        }
    };
    return &vtable;
}

struct num num_ctor (const struct _class_vtable_num *vtable) {
    struct num self = {
        .show_interface = show_ctor(_class_override_num_show()),
        ._vtable = vtable
    };
    return self;
}

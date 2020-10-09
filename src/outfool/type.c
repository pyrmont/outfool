#include "../rope.h"
#include "../outfool.h"

/* Type */

typedef struct {
    rope_t *data;
    uint8_t *cstring;
    size_t last_pos;
    size_t next_byte;
} outfool_rope_t;

/* Utility Methods */

static uint8_t *outfool_rope_cstr(outfool_rope_t *rope) {
    if (NULL == rope->cstring) rope->cstring = rope_create_cstr(rope->data);
    return rope->cstring;
}

/* Garbage Collection */

static void outfool_rope_init(outfool_rope_t *rope, uint8_t *cstring) {
    rope_t *r = rope_new();
    rope_insert(r, 0, cstring);
    rope->data = r;
    rope->cstring = cstring;
    rope->last_pos = 0;
    rope->next_byte = 0;
}

static void outfool_rope_deinit(outfool_rope_t *rope) {
    rope_free(rope->data);
    free(rope->cstring);
}

static int outfool_rope_gc(void *p, size_t size) {
    (void) size;
    outfool_rope_t *rope = (outfool_rope_t *)p;
    outfool_rope_deinit(rope);
    return 0;
}

/* Accessing */

static JanetMethod outfool_rope_methods[2];

static int outfool_rope_get(void *p, Janet key, Janet *out) {
    outfool_rope_t *rope = (outfool_rope_t *)p;

    if (janet_checktype(key, JANET_KEYWORD)) {
        return janet_getmethod(janet_unwrap_keyword(key), outfool_rope_methods, out);
    }

    if (!janet_checksize(key)) janet_panic("expected size as key");
    size_t index = (size_t) janet_unwrap_number(key);

    if (index >= rope->data->num_chars) {
        return 0;
    }

    if (NULL == rope->cstring) rope->cstring = outfool_rope_cstr(rope);

    size_t width;
    if (index == rope->last_pos + 1) {
        width = codepoint_size(rope->cstring[rope->next_byte]);
    } else {
        rope->next_byte = 0;
        width = codepoint_size(rope->cstring[rope->next_byte]);
        size_t i = 0;
        while(index != i) {
            rope->next_byte = rope->next_byte + width;
            width = codepoint_size(rope->cstring[rope->next_byte]);
            i++;
        }
    }

    uint8_t *codepoint = malloc(width + 1);
    memcpy(codepoint, rope->cstring + rope->next_byte, width);
    codepoint[width] = '\0';

    rope->next_byte = rope->next_byte + width;
    rope->last_pos = index;
    *out = janet_cstringv((char *)codepoint);
    free(codepoint);
    return 1;
}

/* Marshalling */

static void outfool_rope_marshal(void *p, JanetMarshalContext *ctx) {
    outfool_rope_t *rope = (outfool_rope_t *)p;
    uint8_t *cstring = outfool_rope_cstr(rope);
    size_t size = strlen((char *)cstring) + 1;
    janet_marshal_abstract(ctx, p);
    janet_marshal_size(ctx, size);
    janet_marshal_bytes(ctx, cstring, size);
    // free(cstring); // FIXME: Do we need to call this?
}

static void *outfool_rope_unmarshal(JanetMarshalContext *ctx) {
    outfool_rope_t *rope = janet_unmarshal_abstract(ctx, sizeof(outfool_rope_t));
    size_t size = janet_unmarshal_size(ctx);
    uint8_t *cstring = janet_smalloc(size);
    janet_unmarshal_bytes(ctx, cstring, size);
    if (cstring[size - 1] != 0)
        janet_panicf("invalid utf8-rope data in unmarshal");
    outfool_rope_init(rope, cstring);
    // free(cstring); // FIXME: Do we need to call this?
    return rope;
}

/* String */

static void outfool_rope_to_string(void *p, JanetBuffer *buf) {
    outfool_rope_t *rope = (outfool_rope_t *)p;
    uint8_t *cstring = outfool_rope_cstr(rope);
    janet_buffer_push_cstring(buf, (char *)cstring);
}

/* Comparison */

static int outfool_rope_compare(void *p1, void *p2) {
    outfool_rope_t *a = (outfool_rope_t *)p1;
    outfool_rope_t *b = (outfool_rope_t *)p2;

    return strcmp((char *)outfool_rope_cstr(a), (char *)outfool_rope_cstr(b));
}

/* Hashing */

/* static int32_t outfool_rope_hash(void *p, size_t size) { */
/*     (void) p; */
/*     (void) size; */
/*     return 0; */
/* } */

/* Traversal */

static Janet outfool_rope_next(void *p, Janet key) {
    outfool_rope_t *rope = (outfool_rope_t *)p;

    if (janet_checktype(key, JANET_NIL)) {
        if (rope->data->num_bytes > 0) {
            return janet_wrap_number(0);
        } else {
            return janet_wrap_nil();
        }
    }

    if (!janet_checksize(key)) janet_panic("expected size as key");
    size_t index = (size_t) janet_unwrap_number(key);
    index++;

    if (index < rope->data->num_chars) {
        return janet_wrap_number((double)index);
    } else {
        return janet_wrap_nil();
    }
}

/* Type Definition */

static const JanetAbstractType outfool_rope_type = {
    "utf8-rope",
    outfool_rope_gc,
    NULL, /* GC Mark */
    outfool_rope_get,
    NULL, /* Push */
    outfool_rope_marshal,
    outfool_rope_unmarshal,
    outfool_rope_to_string,
    outfool_rope_compare,
    NULL, /* Hash */
    outfool_rope_next,
    JANET_ATEND_NEXT
};

/* Constructor */

static Janet cfun_outfool_rope(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);
    const uint8_t *cstring = janet_getstring(argv, 0);
    outfool_rope_t *rope = (outfool_rope_t *)janet_abstract(&outfool_rope_type, sizeof(outfool_rope_t));
    outfool_rope_init(rope, (uint8_t *)cstring);
    return janet_wrap_abstract(rope);
}

static const JanetReg cfuns[] = {
    {"rope", cfun_outfool_rope, NULL},
    {NULL, NULL, NULL}
};

/* Methods */

static Janet outfool_rope_method_length(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);
    outfool_rope_t *rope = (outfool_rope_t *)janet_getabstract(argv, 0, &outfool_rope_type);
    return janet_wrap_number(rope->data->num_chars);
}

static JanetMethod outfool_rope_methods[] = {
    {"length", outfool_rope_method_length},
    {NULL, NULL}
};

/* Environment Registration */

void outfool_register_type(JanetTable *env) {
    janet_register_abstract_type(&outfool_rope_type);
    janet_cfuns(env, "outfool", cfuns);
}

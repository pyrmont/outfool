#include "outfool.h"

JANET_MODULE_ENTRY(JanetTable *env) {
    outfool_register_type(env);
    outfool_register_functions(env);
}

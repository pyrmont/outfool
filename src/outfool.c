#include <utf8.h>
#include "outfool.h"

static Janet cfun_outfool_casecmp(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    const char *src1 = janet_getcstring(argv, 0);
    const char *src2 = janet_getcstring(argv, 1);

    int32_t result = utf8casecmp(src1, src2);

    return janet_wrap_integer(result);
}

static const JanetReg cfuns[] = {
    {"casecmp", cfun_outfool_casecmp, NULL},
    {NULL, NULL, NULL}
};

static void register_aliases(JanetTable *env, const char *aliases[][2]) {
    JanetTable *core = janet_core_env(NULL);
    size_t i = 0;
    while(aliases[i][0]) {
        const char *alias = aliases[i][0];
        const char *name = aliases[i][1];
        JanetTable *entry_table = janet_unwrap_table(janet_table_get(core, janet_wrap_symbol(janet_csymbol(name))));
        Janet fn = janet_table_get(entry_table, janet_ckeywordv("value"));
        const char *doc = (const char*)janet_unwrap_string(janet_table_get(entry_table, janet_ckeywordv("doc")));
        janet_def(env, alias, fn, doc);
        i++;
    }
}

void outfool_register_functions(JanetTable *env) {
    const char *aliases[][2] = {
        {"bytes", "string/bytes"},
        {"from-bytes", "string/from-bytes"},
        {NULL, NULL}
    };

    register_aliases(env, aliases);
    janet_cfuns(env, "outfool", cfuns);
}

JANET_MODULE_ENTRY(JanetTable *env) {
    outfool_register_functions(env);
}

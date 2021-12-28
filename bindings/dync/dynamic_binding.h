//
// Created by charlie on 12/27/21.
//

#ifndef _DYNAMIC_BINDING_H
#define _DYNAMIC_BINDING_H

#define AOTRC_DEDUCE_LENGTH (-1)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>
#include <stddef.h>

    typedef bool (*aotrc_match_t)(const char *, unsigned int);
    typedef char *(*aotrc_get_pattern_t)();

    typedef struct aotrc_ctx {
        void *lib_handle;
    } aotrc_ctx_t;

    aotrc_ctx_t *aotrc_ctx_open(const char *file_path);
    void aotrc_ctx_free(aotrc_ctx_t *ctx);

    bool aotrc_match(aotrc_ctx_t *ctx, const char *pattern_label, const char *input_text, int input_text_length);
    bool aotrc_fullmatch(aotrc_ctx_t *ctx, const char *pattern_label, const char *input_text, int input_text_length);
    char *aotrc_get_pattern(aotrc_ctx_t *ctx, const char *pattern_label);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif //_DYNAMIC_BINDING_H

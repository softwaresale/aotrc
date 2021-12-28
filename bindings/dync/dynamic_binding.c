//
// Created by charlie on 12/27/21.
//

#include "dynamic_binding.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

aotrc_ctx_t *aotrc_ctx_open(const char *file_path) {
    aotrc_ctx_t *ctx = (aotrc_ctx_t *) malloc(sizeof(aotrc_ctx_t));
    ctx->lib_handle = dlopen(file_path, RTLD_NOW | RTLD_GLOBAL);
    if (ctx->lib_handle == NULL) {
        fprintf(stderr, "Error while opening library: %s\n", dlerror());
        free(ctx);
        return NULL;
    }

    dlerror();

    return ctx;
}

void aotrc_ctx_free(aotrc_ctx_t *ctx) {
    if (!ctx)
        return;

    if (ctx->lib_handle) {
        dlclose(ctx->lib_handle);
    }

    free(ctx);
}

bool aotrc_match(aotrc_ctx_t *ctx, const char *pattern_label, const char *input_text, int input_text_length) {
    // Try to find a match function for the label
    size_t function_name_buffer_size = strlen(pattern_label) + strlen("_match");
    char function_name[function_name_buffer_size];
    sprintf(function_name, "match_%s", pattern_label);

    // Check the function cache
    aotrc_match_t match_func = dlsym(ctx->lib_handle, function_name);
    char *error = dlerror();
    if (error) {
        fprintf(stderr, "Error while loading function: %s\n", error);
        return false;
    }

    size_t length = input_text_length == AOTRC_DEDUCE_LENGTH ? strlen(input_text) : input_text_length;
    return match_func(input_text, length);
}

bool aotrc_fullmatch(aotrc_ctx_t *ctx, const char *pattern_label, const char *input_text, int input_text_length) {
    // Try to find a match function for the label
    size_t function_name_buffer_size = strlen(pattern_label) + strlen("_fullmatch");
    char function_name[function_name_buffer_size];
    sprintf(function_name, "fullmatch_%s", pattern_label);

    aotrc_match_t match_func = dlsym(ctx->lib_handle, function_name);
    char *error = dlerror();
    if (error) {
        fprintf(stderr, "Error while loading function: %s\n", error);
        return false;
    }

    size_t length = input_text_length == AOTRC_DEDUCE_LENGTH ? strlen(input_text) : input_text_length;
    return match_func(input_text, length);
}

char *aotrc_get_pattern(aotrc_ctx_t *ctx, const char *pattern_label) {
    // Try to find a match function for the label
    size_t function_name_buffer_size = strlen(pattern_label) + strlen("get_pattern_");
    char function_name[function_name_buffer_size];
    sprintf(function_name, "fullmatch_%s", pattern_label);

    aotrc_get_pattern_t get_pattern = dlsym(ctx->lib_handle, function_name);
    char *error = dlerror();
    if (error) {
        fprintf(stderr, "Error while loading function: %s\n", error);
        return false;
    }

    // Get the pattern
    return get_pattern();
}

#include "libminilisp.h"
#include "emscripten.h"

void *env_constructor[3];
void *root = NULL;
Obj **genv;

const char json_mask_result[] = "{ \"out\": %s, \"err\": %s }";
const char json_mask_err[] = "{ \"msg\": \"%s\", \"idx\": %d }";

char *result_buf_ptr = NULL;
int result_buf_idx = 0;

char json_buf_out[1024] = {0};
int json_buf_out_idx = 0;

char json_buf_err[256] = "null";
int json_buf_err_idx = 0;

static inline void mempush(char *buf, int *ptr, const char *value, int size)
{
    memcpy(buf + *ptr, value, size);
    *ptr += size;
}

void print_out(const char *msg, int size)
{
    mempush(json_buf_out, &json_buf_out_idx, "\"", 1);
    mempush(json_buf_out, &json_buf_out_idx, msg, size);
    mempush(json_buf_out, &json_buf_out_idx, "\"", 1);
    mempush(json_buf_out, &json_buf_out_idx, ",", 1);
}

void print_err(const char *msg, int size)
{
    sprintf(json_buf_err, json_mask_err, msg, lisp_error_idx());
}

EMSCRIPTEN_KEEPALIVE
int version()
{
    return 1;
}

EMSCRIPTEN_KEEPALIVE
int lisp_evaluate(const char* input, char* output)
{
    lisp_set_printers(print_out, print_err);
    result_buf_ptr = output;

    env_constructor[0] = root;
    env_constructor[1] = NULL;
    env_constructor[2] = ROOT_END;
    root = env_constructor;
    genv = (Obj **)(env_constructor + 1);

    lisp_create(4000);

    *genv = make_env(root, &Nil, &Nil);
    define_constants(root, genv);
    define_primitives(root, genv);

    mempush(json_buf_out, &json_buf_out_idx, "[", 1);
    bool success = lisp_eval(root, genv, input);
    json_buf_out_idx--; // to replace last comma in the array
    mempush(json_buf_out, &json_buf_out_idx, "]", 1);

    lisp_destroy();

    memcpy(result_buf_ptr, json_buf_out, json_buf_out_idx);
    result_buf_idx = sprintf(result_buf_ptr, json_mask_result, json_buf_out, json_buf_err);

    int factor = success ? 1 : -1;
    return result_buf_idx * factor;
}

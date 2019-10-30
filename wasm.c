#include "libminilisp.h"
#include "emscripten.h"

const char json_mask_result[] = "{ \"out\": %s, \"err\": %s, \"meta\": { \"time\": %.2g } }";
const char json_mask_err[] = "{ \"msg\": \"%s\", \"idx\": %d }";

char json_buf_out[102400] = {0};
int json_buf_out_idx = 0;

char json_buf_err[256] = {0};
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

static bool lisp_shoot_once(size_t max_heap, const char *input)
{
    lisp_set_printers(print_out, print_err);
    void *root = NULL;
    DEFINE1(env);
    
    lisp_create(max_heap);

    *env = make_env(root, &Nil, &Nil);
    define_constants(root, env);
    define_primitives(root, env);
    bool success = lisp_eval(root, env, input);

    lisp_destroy();

    return success;
}

EMSCRIPTEN_KEEPALIVE
int version()
{
    return 1;
}

EMSCRIPTEN_KEEPALIVE
int lisp_evaluate(size_t max_heap, const char* input, char* output)
{
    memset(json_buf_out, 0, sizeof(json_buf_out));
    memset(json_buf_err, 0, sizeof(json_buf_err));
    memcpy(json_buf_err, "null", 5);
    json_buf_out_idx = 0;
    json_buf_err_idx = 0;

    bool success = false;
    double time_taken = 0;

    if (max_heap >= 2000) {
        mempush(json_buf_out, &json_buf_out_idx, "[", 1);
        double time_started = emscripten_get_now();
        success = lisp_shoot_once(max_heap, input);
        time_taken = emscripten_get_now() - time_started;
        if (',' == json_buf_out[json_buf_out_idx - 1])
            json_buf_out_idx--;
        mempush(json_buf_out, &json_buf_out_idx, "]", 1);
    } else {
        memcpy(json_buf_out, "[]", 3);
        sprintf(json_buf_err, json_mask_err, "Heap must be at least 2000 bytes", 0);
    }

    int output_size = sprintf(output, json_mask_result, json_buf_out, json_buf_err, time_taken);

    int factor = success ? 1 : -1;
    return output_size * factor;
}

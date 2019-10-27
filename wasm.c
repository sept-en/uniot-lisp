#include "libminilisp.h"
#include "emscripten.h"

void *env_constructor[3];
void *root = NULL;
Obj **genv;

char *out_buf_ptr = NULL;
int out_buf_idx = 0;

void printOut(const char *msg, int size)
{
    memcpy(out_buf_ptr + out_buf_idx, msg, size);
    memcpy(out_buf_ptr + out_buf_idx + size, "\n", 1);
    out_buf_idx += size + 1;
}

EMSCRIPTEN_KEEPALIVE
int version()
{
    return 1;
}

EMSCRIPTEN_KEEPALIVE
int lisp_evaluate(const char* input, char* output)
{
    lisp_set_printers(printOut, printOut);
    out_buf_ptr = output;

    env_constructor[0] = root;
    env_constructor[1] = NULL;
    env_constructor[2] = ROOT_END;
    root = env_constructor;
    genv = (Obj **)(env_constructor + 1);

    lisp_create(4000);

    *genv = make_env(root, &Nil, &Nil);
    define_constants(root, genv);
    define_primitives(root, genv);

    bool success = lisp_eval(root, genv, input);

    lisp_destroy();

    int factor = success ? 1 : -1;
    return out_buf_idx * factor;
}

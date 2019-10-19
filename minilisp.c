#include "libminilisp.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

void *env_constructor[3];
void *root = NULL;
Obj **genv;

void printOut(const char *msg, int size)
{
    printf(ANSI_COLOR_GREEN);
    printf("%s\n", msg);
    printf(ANSI_COLOR_RESET);
}

void printErr(const char *msg, int size)
{
    printf(ANSI_COLOR_RED);
    printf("%s\n", msg);
    printf(ANSI_COLOR_RESET);
}

int main()
{
    lisp_set_printers(printOut, printErr);

    env_constructor[0] = root;
    env_constructor[1] = NULL;
    env_constructor[2] = ROOT_END;
    root = env_constructor;
    genv = (Obj **)(env_constructor + 1);

    lisp_create(4000);

    *genv = make_env(root, &Nil, &Nil);
    define_constants(root, genv);
    define_primitives(root, genv);

    lisp_eval(root, genv, "(+ 1 2) (^ 1 2)");

    return 0;
}
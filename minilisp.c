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

    // lisp_eval(root, genv, "(define a 5) (setq a 1) (print #itr) (print #t) (setq #itr 1)");
    // lisp_eval(root, genv, "(print #itr) (while (< #itr 10) (print #itr)) (print #itr)");
    lisp_eval(root, genv, "(define code '(+ 1 2)) (eval '(+ 2 2)) (eval code) (print code) (+ 5 6)");

    lisp_destroy();

    return 0;
}
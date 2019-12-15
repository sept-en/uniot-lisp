#ifndef MEMCHECK_H
#define MEMCHECK_H

#include <stdlib.h>

static const char origin_of_stack_var;
static const char *origin_of_stack_ptr = &origin_of_stack_var;

static inline int is_valid_ptr(void *p)
{
  char *top_of_heap_ptr = (char *)malloc(1);
  free(top_of_heap_ptr);
  char *cp = (char *)p;
  return (cp > origin_of_stack_ptr && cp < top_of_heap_ptr);
}

#endif // MEMCHECK_H

#ifndef DUDECT_FIXTURE_H
#define DUDECT_FIXTURE_H

#include <stdbool.h>
#include "constant.h"

/* Interface to test if function is constant */
bool is_insert_tail_const(void);
bool is_size_const(void);

/* Test function for q_size and q_insert_tail */
void test_size_const(void);
void test_insert_tail_const(void);

#endif

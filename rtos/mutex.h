#ifndef MUTEX_H
#define MUTEX_H

#include <stdbool.h>

#define MUTEX_INIT false

typedef bool mutex_t;

void acquire(mutex_t *mutex);

void release(mutex_t *mutex);

#endif
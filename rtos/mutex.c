#include "mutex.h"

#include <stdio.h>

void acquire(mutex_t *mutex)
{
    while (*mutex)
    {
    };

    *mutex = true;
}

void release(mutex_t *mutex)
{
    *mutex = false;
}
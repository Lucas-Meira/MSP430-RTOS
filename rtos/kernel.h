#ifndef KERNEL_H
#define KERNEL_H

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#include "port.h"

#define MAX_TASKS 10
#define RAM_START 0x2800U
#define DEFAULT_TASK_STACK_SIZE 0x80U
#define SAVED_REGISTERS 12U

typedef struct
{
    void (*taskEntryPoint)(void);
    uint16_t *taskStackPointer;
} osTaskControlBlock;

typedef struct
{
    osTaskControlBlock tasks[MAX_TASKS];
    uint16_t taskCount;
    uint16_t currentTask;
} osTasks_t;

extern osTasks_t osTasks;

void _initStack(void (*taskEntryPoint)(void));

bool osRegisterTask(void (*taskEntryPoint)(void));

void _clearTaskStack(void);

inline void osStart(void);

inline void osInit(void);

#endif

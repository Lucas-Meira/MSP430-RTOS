#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 10
#define RAM_START 0x2800U
#define DEFAULT_TASK_STACK_SIZE 0x80U

typedef struct
{
    void (*taskEntryPoint)(void);
    uint16_t *taskStackPointer;
} osTaskControlBlock;

static volatile struct
{
    osTaskControlBlock tasks[MAX_TASKS];
    uint16_t taskCount;
    uint16_t currentTask;
} osTasks = {
    0,
};

inline void _osReturnFromInterrupt()
{
    asm("reti");
}

inline void _osSaveStackPointer()
{
    asm("mov.w SP,%0"
        : "=m"(osTasks.tasks[osTasks.currentTask].taskStackPointer));
}

inline void _osRestoreStackPointer()
{
    asm("mov.w %0,SP" ::"m"(osTasks.tasks[osTasks.currentTask].taskStackPointer));
}

inline void _osSaveContext()
{
    asm("pushm.a #12,R15");
}

inline void _osRestoreContext()
{
    asm("popm.a #12,R15");
}

void _initStack(void (*taskEntryPoint)(void))
{
    *(--osTasks.tasks[osTasks.taskCount].taskStackPointer) = (uint32_t)taskEntryPoint;
    *(--osTasks.tasks[osTasks.taskCount].taskStackPointer) = ((uint32_t)taskEntryPoint >> 4) & 0xF000 | GIE;
}

void _clearTaskStack(void)
{
    uint8_t numRegisters = 12;
    while (numRegisters--)
    {
        *(--osTasks.tasks[osTasks.taskCount].taskStackPointer) = 0U;
        *(--osTasks.tasks[osTasks.taskCount].taskStackPointer) = 0U;
    }
}

bool osRegisterTask(void (*taskEntryPoint)(void))
{
    if (osTasks.taskCount >= MAX_TASKS)
    {
        return false;
    }

    osTasks.tasks[osTasks.taskCount].taskEntryPoint = taskEntryPoint;
    osTasks.tasks[osTasks.taskCount].taskStackPointer = (uint16_t *)(RAM_START +
                                                                     (osTasks.taskCount + 1) * DEFAULT_TASK_STACK_SIZE);

    _initStack(taskEntryPoint);
    _clearTaskStack();

    osTasks.taskCount++;

    return true;
}

void osInit(void)
{
    osTasks.tasks[0].taskStackPointer += 12 * sizeof(uint16_t);

    asm("mov.w %0,SP" ::"m"(osTasks.tasks[0].taskStackPointer));

    SFRIE1 |= WDTIE;
    __enable_interrupt();

    osTasks.tasks[0].taskEntryPoint();
}

void taskA()
{
    P1DIR |= BIT0;

    while (1)
    {
        __delay_cycles(500000);
        P1OUT ^= BIT0;
    }
}

void taskB()
{
    P4DIR |= BIT7;

    while (1)
    {
        __delay_cycles(500000);
        P4OUT ^= BIT7;
    }
}

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW |         // Password. Security against incorrect memory writes
             WDTTMSEL |      // Interval Time Mode
             WDTSSEL__ACLK | // Use ACLK. 32kHz
             WDTIS__32K;     // Divide by 32k. Active every second.

    osRegisterTask(taskA);
    osRegisterTask(taskB);

    osInit();

    while (1)
    {
    }
}

__attribute__((__interrupt__(WDT_VECTOR), naked)) void osScheduler(void)
{
    _osSaveContext();
    _osSaveStackPointer();

    osTasks.currentTask = (osTasks.currentTask + 1) % osTasks.taskCount;

    _osRestoreStackPointer();
    _osRestoreContext();

    _osReturnFromInterrupt();
}

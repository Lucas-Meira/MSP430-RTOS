#include "rtos/kernel.h"

void taskA()
{
    P1DIR |= BIT0;

    while (1)
    {
        __delay_cycles(100000);
        P1OUT ^= BIT0;
    }
}

void taskB()
{
    P4DIR |= BIT7;

    while (1)
    {
        __delay_cycles(80000);
        P4OUT ^= BIT7;
    }
}

/**
 * main.c
 */
int main(void)
{
    osRegisterTask(taskA);
    osRegisterTask(taskB);

    osInit();

    while (1)
    {
    }
}

#pragma vector = WDT_VECTOR
__attribute__((naked))
__interrupt void osScheduler()
{
    _osSaveContext(&osTasks.tasks[osTasks.currentTask].taskStackPointer);

    if (++osTasks.currentTask == osTasks.taskCount)
    {
        osTasks.currentTask = 0;
    }

    _osRestoreContext(osTasks.tasks[osTasks.currentTask].taskStackPointer);
}

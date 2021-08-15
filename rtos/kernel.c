#include "kernel.h"

osTasks_t osTasks = {
    0,
};

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

inline void osStart(void)
{
    _osRestoreContext(osTasks.tasks[osTasks.currentTask].taskStackPointer);
}

inline void osInit(void)
{
    WDTCTL = WDTPW |         // Password. Security against incorrect memory writes
             WDTTMSEL |      // Interval Time Mode
             WDTSSEL__ACLK | // Use ACLK. 32kHz
             WDTIS_6;        // 2.5ms interval

    SFRIE1 |= WDTIE;

    osStart();
}

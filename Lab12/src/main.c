#include "MemoryAllocation.h"

int main()
{
    MemoryAllocationSimulator simulator;
    int unit_size = 1024;
    int system_size = 128 * 1024 * 1024;
    int user_size = 384 * 1024 * 1024;
    int max_events = 100;
    init_simulator(&simulator, FIRST_FIT, unit_size, system_size, user_size);
    random_simulation(&simulator, max_events);
    return 0;
}
#include "command.h"
#include "utils.h"

int main()
{
    init_shell();
    while (1)
    {
        run_command(input_command());
    }
}
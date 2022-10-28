#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include "command.h"

#define MAGIC_EXIT_CODE 233

void run_external(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length);

#endif
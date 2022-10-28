#include "external.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>

void run_external(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    int pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "错误：子进程创建失败\n");
    }
    else if (pid == 0)
    {
        // 子进程
        const char **argv = (const char **)malloc((commands_length + 1) * sizeof(const char *));
        for (int i = 0; i < commands_length; i++)
        {
            argv[i] = commands[i];
        }
        argv[commands_length] = NULL;
        execvp(commands[0], (char *const *)argv);

        // 如果exec失败，则继续执行以下代码
        free(argv);
        exit(MAGIC_EXIT_CODE);
    }
    else if (pid > 0)
    {
        // 主进程
        int status;
        wait(&status);
        if (WEXITSTATUS(status) == MAGIC_EXIT_CODE)
        {
            fprintf(stderr, "错误：命令不存在：%s\n", commands[0]);
        }
    }
}
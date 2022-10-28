#include "builtin.h"
#include "utils.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <dirent.h>
#include <string.h>
#include <sys/syscall.h>
#include <fcntl.h>

char BUILTIN_COMMANDS[][MAX_COMMAND_LENGTH] = {
    "type",
    "cd",
    "exit",
    "ls",
    "pwd",
    "cat",
    "cp",
    "env"};

void type(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    if (commands_length == 1)
    {
        fprintf(stderr, "错误：需要1个参数\n");
    }
    else if (commands_length == 2)
    {
        int flag = 0;

        // 是否为内部命令
        for (int i = 0; i < NUM_BUILTIN; i++)
        {
            if (strcmp(commands[1], BUILTIN_COMMANDS[i]) == 0)
            {
                flag = 1;
                fprintf(stdout, "%s是shell的内部命令\n", commands[1]);
                break;
            }
        }

        // 是否为外部命令
        if (!flag)
        {
            char external_path[256];
            get_external_path(commands[1], external_path);
            if (external_path[0] != '#')
            {
                fprintf(stdout, "%s是外部命令，路径：%s\n", commands[1], external_path);
            }
            else
            {
                fprintf(stdout, "未找到命令：%s\n", commands[1]);
            }
        }
    }
    else
    {
        fprintf(stderr, "错误：参数过多！\n");
    }
}

void cd(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    if (commands_length == 1)
    {
        // 无参数，不执行任何操作
        return;
    }
    else if (commands_length == 2)
    {
        if (syscall(SYS_chdir, commands[1]) != 0)
        {
            fprintf(stderr, "错误：路径不存在或权限不足：%s\n", commands[1]);
        }
    }
    else
    {
        fprintf(stderr, "错误：参数过多！\n");
    }
}

void exit_shell(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    if (commands_length == 1)
    {
        exit(0);
    }
    else if (commands_length == 2)
    {
        // 使用指定代码退出shell
        if (str_is_int(commands[1]))
        {
            int exit_code = atoi(commands[1]);
            exit(exit_code);
        }
        else
        {
            fprintf(stderr, "错误：%s非整数！\n", commands[1]);
        }
    }
    else
    {
        fprintf(stderr, "错误：参数过多！\n");
    }
}

void ls(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    int show_hidden = 0;

    if (commands_length == 1)
    {
        char path[] = "./";
        print_file_names(path, show_hidden, 0);
    }
    else
    {
        // 查找是否有"-a"参数
        for (int i = 1; i < commands_length; i++)
        {
            if (strcmp(commands[i], "-a") == 0)
            {
                show_hidden = 1;
                break;
            }
        }

        // 输出文件名
        if (commands_length == 2)
        {
            // 输入的命令为"ls -a"或"ls <路径>"
            if (show_hidden)
            {
                char path[] = "./";
                print_file_names(path, show_hidden, 0);
            }
            else
            {
                if (!print_file_names(commands[1], show_hidden, 0))
                {
                    fprintf(stderr, "错误：路径不存在或权限不足：%s\n", commands[1]);
                }
            }
        }
        else if (commands_length == 3)
        {
            // 输入的命令为"ls -a <路径>"或"ls <路径> <路径>"
            for (int i = 1; i < commands_length; i++)
            {
                if (strcmp(commands[i], "-a") != 0)
                {
                    if (!print_file_names(commands[i], show_hidden, !show_hidden))
                    {
                        fprintf(stderr, "错误：路径不存在或权限不足：%s\n", commands[i]);
                    }
                    else if (i < commands_length - 1 && !show_hidden)
                    {
                        fprintf(stdout, "\n");
                    }
                }
            }
        }
        else
        {
            // 输出多个路径
            for (int i = 1; i < commands_length; i++)
            {
                if (strcmp(commands[i], "-a") != 0)
                {
                    if (!print_file_names(commands[i], show_hidden, 1))
                    {
                        fprintf(stderr, "错误：路径不存在或权限不足：%s\n", commands[i]);
                    }
                    else if (i < commands_length - 1)
                    {
                        fprintf(stdout, "\n");
                    }
                }
            }
        }
    }
}

void pwd(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    if (commands_length == 1)
    {
        char cwd[256];
        syscall(SYS_getcwd, cwd, 256);
        fprintf(stdout, "%s\n", cwd);
    }
    else
    {
        fprintf(stderr, "错误：参数过多！\n");
    }
}

void cat(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    if (commands_length == 1)
    {
        fprintf(stderr, "错误：至少提供一个文件\n");
    }
    else
    {
        char buffer[256];
        int fd, count;
        char last = '\0';
        for (int i = 1; i < commands_length; i++)
        {
            fd = syscall(SYS_open, commands[i], O_RDONLY);
            if (fd == -1)
            {
                fprintf(stderr, "错误：文件不存在或权限不足：%s\n", commands[i]);
                continue;
            }
            while ((count = syscall(SYS_read, fd, buffer, sizeof(buffer))) != 0)
            {
                for (int j = 0; j < count; j++)
                {
                    fprintf(stdout, "%c", buffer[j]);
                    last = buffer[j];
                }
            }
            syscall(SYS_close, fd);
        }
        if (last != '\n' && last != '\0')
        {
            fprintf(stdout, "\n");
        }
    }
}

void cp(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    if (commands_length < 3)
    {
        fprintf(stderr, "错误：缺少必要参数\n");
    }
    else if (commands_length > 3)
    {
        fprintf(stderr, "错误：参数过多\n");
    }
    else
    {
        if (syscall(SYS_access, commands[1], F_OK) == -1)
        {
            fprintf(stderr, "错误：文件不存在或权限不足：%s\n", commands[1]);
        }
        else
        {
            if (is_dir(commands[2]))
            {
                // 目标参数是目录，把文件复制到该目录下，文件名不变
                char file_name[MAX_COMMAND_LENGTH];
                char file_dst[2 * MAX_COMMAND_LENGTH];

                split_path(commands[1], NULL, file_name);
                strcpy(file_dst, commands[2]);
                int len = strlen(file_dst);
                if (file_dst[len - 1] != '/')
                {
                    // 补上反斜杠
                    file_dst[len] = '/';
                    file_dst[len + 1] = '\0';
                }
                strcat(file_dst, file_name);
                copy_file(file_dst, commands[1]);
            }
            else
            {
                // 目标参数是文件路径
                char dir_path[MAX_COMMAND_LENGTH];
                char file_name[MAX_COMMAND_LENGTH];

                split_path(commands[2], dir_path, file_name);
                if (is_dir(dir_path) || strcmp(file_name, commands[2]) == 0)
                {
                    copy_file(commands[2], commands[1]);
                }
                else
                {
                    fprintf(stderr, "错误：目标路径不存在或权限不足：%s\n", dir_path);
                }
            }
        }
    }
}

void env(const char commands[MAX_COMMAND_ARGC][MAX_COMMAND_LENGTH], int commands_length)
{
    extern char **environ;

    if (commands_length == 1)
    {
        for (int i = 0; environ[i]; i++)
            fprintf(stdout, "%s\n", environ[i]);
    }
    else if (commands_length == 2)
    {
        if (strcmp(commands[1], "set") == 0 || strcmp(commands[1], "unset") == 0 || strcmp(commands[1], "get") == 0)
        {
            fprintf(stderr, "错误：操作%s缺少必要参数\n", commands[1]);
        }
        else
        {
            fprintf(stderr, "错误：命令env不支持操作%s\n", commands[1]);
        }
    }
    else if (commands_length == 3)
    {
        if (strcmp(commands[1], "set") == 0)
        {
            int flag = 0;
            for (int i = 0; commands[2][i] != '\0'; i++)
            {
                if (commands[2][i] == '=')
                {
                    flag = 1;
                    break;
                }
            }
            if (flag)
            {
                char *current_env = (char *)malloc((strlen(commands[2]) + 1) * sizeof(char));
                strcpy(current_env, commands[2]);
                if (putenv(current_env))
                {
                    fprintf(stderr, "错误：环境变量设置失败：%s\n", commands[2]);
                }
            }
            else
            {
                fprintf(stderr, "错误：参数缺少等号：%s\n", commands[2]);
            }
        }
        else if (strcmp(commands[1], "unset") == 0)
        {
            if (unsetenv(commands[2]))
            {
                fprintf(stderr, "错误：清除环境变量失败：%s\n", commands[2]);
            }
        }
        else if (strcmp(commands[1], "get") == 0)
        {
            fprintf(stdout, "%s\n", getenv(commands[2]));
        }
        else
        {
            fprintf(stderr, "错误：命令env不支持操作%s\n", commands[1]);
        }
    }
    else
    {
        fprintf(stderr, "错误：参数过多！\n");
    }
}

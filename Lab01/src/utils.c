#include "utils.h"
#include "command.h"
#include <string.h>
#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define O_RW 0666

void init_shell()
{
    fprintf(stdout, "\n");
    fprintf(stdout, "__  __                           _____ __         ____\n");
    fprintf(stdout, "\\ \\/ /_  ____  ______ _____     / ___// /_  ___  / / /\n");
    fprintf(stdout, " \\  / / / / / / / __ `/ __ \\    \\__ \\/ __ \\/ _ \\/ / / \n");
    fprintf(stdout, " / / /_/ / /_/ / /_/ / /_/ /   ___/ / / / /  __/ / /  \n");
    fprintf(stdout, "/_/\\__,_/\\__, /\\__,_/\\____/   /____/_/ /_/\\___/_/_/   \n");
    fprintf(stdout, "        /____/                                        \n");
    fprintf(stdout, "\n");
}

int str_is_int(const char *str)
{
    const char *p = str;
    if (*p == '-')
        p++;
    while (*p != '\0')
    {
        if (*p < '0' || *p > '9')
            return 0;
        p++;
    }
    return 1;
}

int cmp_strings(const void *_a, const void *_b)
{
    char **a = (char **)_a;
    char **b = (char **)_b;
    return strcmp(*a, *b);
}

// 输入文件夹路径，返回该路径下所有文件（夹）的文件名
// 返回的指针需要手动free
char **list_dir(const char *path)
{
    int num_files = 0;
    int max_num_files = 16;
    // 多留一个位置存放NULL，作为指针数组结束的标志
    char **file_names = (char **)malloc((max_num_files + 1) * sizeof(char *));
    file_names[0] = NULL;
    DIR *dir = opendir(path);
    if (!dir)
    {
        return NULL;
    }
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != 0)
    {
        if (num_files == max_num_files)
        {
            char **old = file_names;
            max_num_files *= 2;
            file_names = (char **)malloc((max_num_files + 1) * sizeof(char *));
            memcpy(file_names, old, num_files * sizeof(char *));
            free(old);
        }
        file_names[num_files] = entry->d_name;
        file_names[num_files + 1] = NULL; // 结束标志
        num_files++;
    }
    qsort(file_names, num_files, sizeof(file_names[0]), cmp_strings);
    closedir(dir);
    return file_names;
}

int print_file_names(const char *path, int show_hidden, int print_path)
{
    char **file_names = list_dir(path);
    if (file_names == NULL)
    {
        return 0;
    }
    if (print_path)
    {
        fprintf(stdout, "%s:\n", path);
    }

    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    int cols = size.ws_col;

    int current = 0;
    int first = 1;
    for (char **p = file_names; *p != NULL; p++)
    {
        if ((*p)[0] != '.' || show_hidden)
        {
            current += strlen(*p);
            if (first)
            {
                first = 0;
            }
            else
            {
                current += 2;
                if (current <= cols)
                {
                    fprintf(stdout, "  ");
                }
            }
            if (current > cols)
            {
                fprintf(stdout, "\n");
                current = strlen(*p);
            }
            fprintf(stdout, "%s", *p);
        }
    }
    fprintf(stdout, "\n");
    free(file_names);
    return 1;
}

int is_dir(const char *path)
{
    if (path == NULL)
    {
        return 0;
    }
    if (opendir(path) == NULL)
    {
        return 0;
    }
    return 1;
}

void split_path(const char *file_path, char *dir_path, char *file_name)
{
    if (file_path == NULL)
    {
        return;
    }
    int path_length = strlen(file_path);
    int split_index = -1;
    for (int i = path_length - 1; i >= 0; i--)
    {
        if (file_path[i] == '/')
        {
            split_index = i;
            break;
        }
    }
    if (split_index != -1)
    {
        if (dir_path != NULL)
        {
            for (int i = 0; i < split_index; i++)
            {
                dir_path[i] = file_path[i];
            }
        }
        if (file_name != NULL)
        {
            for (int i = split_index + 1; i < path_length; i++)
            {
                file_name[i] = file_path[i];
            }
        }
    }
    else
    {
        if (dir_path != NULL)
            strcpy(dir_path, "");
        if (file_name != NULL)
            strcpy(file_name, file_path);
    }
}

void copy_file(const char *dst, const char *src)
{
    int fd_src, fd_dst, count;
    char buffer[2048];
    int mode = syscall(SYS_access, src, X_OK) == 0 ? 0777 : 0666; // 判断是否可执行
    fd_src = syscall(SYS_open, src, O_RDONLY);
    fd_dst = syscall(SYS_creat, dst, mode);

    while ((count = syscall(SYS_read, fd_src, buffer, sizeof(buffer))) != 0)
    {
        syscall(SYS_write, fd_dst, buffer, count);
    }
    syscall(SYS_close, fd_src);
    syscall(SYS_close, fd_dst);
}

void get_external_path(const char *command, char *command_path)
{
    if (command_path == NULL)
        return;
    char *path = getenv("PATH");
    int left = 0, right = 0;
    int num_paths = 0, max_num_paths = 16;
    char **paths = (char **)malloc(max_num_paths * sizeof(char *));
    int i = 0;

    // 分解PATH的所有路径
    while (1)
    {
        if (path[i] == ':' || path[i] == '\0')
        {
            if (num_paths == max_num_paths)
            {
                char **old = paths;
                max_num_paths *= 2;
                paths = (char **)malloc(max_num_paths * sizeof(char *));
                memcpy(paths, old, num_paths * sizeof(char *));
                free(old);
            }
            right = i;
            paths[num_paths] = (char *)malloc((right - left + 1) * sizeof(char));
            memcpy(paths[num_paths], path + left, right - left);
            paths[num_paths][right - left] = '\0';
            num_paths++;
            left = right + 1;
        }

        if (path[i] == '\0')
            break;
        i++;
    }

    // 在PATH路径下查找文件
    int flag = 0;
    for (int i = 0; i < num_paths; i++)
    {
        DIR *dir = opendir(paths[i]);
        if (!dir)
            continue;
        struct dirent *entry = NULL;
        while ((entry = readdir(dir)) != 0)
        {
            if (strcmp(entry->d_name, command) == 0)
            {
                char temp[256];
                strcpy(temp, paths[i]);
                strcat(temp, "/");
                strcat(temp, command);
                // 要求该文件可执行
                if (syscall(SYS_access, temp, X_OK) == 0)
                {
                    strcpy(command_path, temp);
                    flag = 1;
                    break;
                }
            }
        }
        if (flag)
            break;
    }

    // PATH中没有找到该可执行文件
    if (!flag)
        strcpy(command_path, "#");

    // 释放内存
    for (int i = 0; i < num_paths; i++)
        free(paths[i]);
    free(paths);
}
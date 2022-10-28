#ifndef COMMAND_H_
#define COMMAND_H_

#define INPUT_OK 0
#define INPUT_OVERFLOW 1
#define ARGS_OVERFLOW 2

#define MAX_COMMAND_LENGTH 256
#define MAX_COMMAND_ARGC 16

int input_command();                // 显示等待输入信息，并保存用户输入的命令
int parse_command();                // 将command_buffer的内容按照空格拆分，并存放到commands中
void run_command(int commnad_flag); // 执行命令

#endif
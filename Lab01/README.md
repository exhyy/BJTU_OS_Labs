# Linux命令解释程序设计与实现

> 2022北京交通大学操作系统作业

## 使用
### 克隆本仓库
```
git clone https://github.com/exhyy/OS_Shell.git
```
### 编译
```
make
```
### 运行
```
./myshell
```

## 内部命令
- `type`：获取命令类型——内部命令或外部命令
- `cd`：切换工作目录
- `exit`：退出shell
- `ls`：列出当前（或指定）目录下的所有文件、文件夹，使用`-a`参数显示隐藏文件
- `pwd`：获取当前工作目录
- `cat`：拼接文本文件
- `cp`：复制单个文件
- `env`：环境变量相关操作，包括`get`、`set`和`unset`三个子命令
    - `env get`：获取指定环境变量
    - `env set`：新建或修改指定文件变量，格式为`env set <环境变量名>=<环境变量值>`
    - `env unset`：删除指定环境变量

## 外部命令
通过`fork`和`exec`运行外部命令。可执行`PATH`内的命令和你自己写的程序，例如`gcc`、`python`和`vim`
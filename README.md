# 进程通信

**环境：Linux - CentOS 7**

### 拉取仓库

~~~bash
git clone https://github.com/AkashiNeko/IPC.git
cd IPC
~~~

### 目录结构

~~~bash
IPC
├── pipe
│   ├── bin
│   │   ├── pipe_client
│   │   └── pipe_server
│   ├── cmd.hpp
│   ├── log.hpp
│   ├── Makefile
│   ├── pipe_client.cc
│   └── pipe_server.cc
└── shm
    ├── bin
    │   ├── shm_client
    │   └── shm_server
    ├── cmd.hpp
    ├── log.hpp
    ├── Makefile
    ├── shm_client.cc
    └── shm_server.cc
~~~

### 1. 管道通信

1. 进入 `pipe` 目录，使用 `make` 生成可执行文件。

~~~bash
cd pipe
make
~~~

2. 同时打开两个终端，使二者位于 `IPC/pipe/bin` 目录下。

3. 先在 **终端2** 运行 `pipe_server`。
~~~bash
./pipe_server
~~~
4. 然后在 **终端1** 运行 `pipe_client`。
~~~
./pipe_client
~~~

5. 在 **终端1** 的 `pipe_client` 中发送信息，会被 **终端2** 的 `pipe_server` 收到。

**终端1**

~~~bash
[akashi@CentOS bin]$ ./pipe_client 
Send Message: abc
Send Message: 123
Send Message: qwq
Send Message: Hello World
Send Message: ^C
~~~

**终端2**

~~~bash
[akashi@CentOS bin]$ ./pipe_server 
 | 2023-03-05 18:22:40 | Debug   | Create FIFO | 
 | 2023-03-05 18:22:42 | Debug   | Open FIFO | 
 | 2023-03-05 18:22:46 | Notice  | Receive | Message: abc
 | 2023-03-05 18:22:48 | Notice  | Receive | Message: 123
 | 2023-03-05 18:22:49 | Notice  | Receive | Message: qwq
 | 2023-03-05 18:22:53 | Notice  | Receive | Message: Hello World
 | 2023-03-05 18:22:58 | Warning | Read EOF | Client quit, Server quit too.
 | 2023-03-05 18:22:58 | Debug   | Close FIFO | 
 | 2023-03-05 18:22:58 | Debug   | Unlink FIFO |
~~~

### 2. System V

未完待续

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

### 1. 命名管道

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

~~~bash
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

1. 进入 `shm` 目录，使用 `make` 生成可执行文件。

~~~bash
cd shm
make
~~~

2. 同时打开两个终端，使二者位于 `IPC/shm/bin` 目录下。

3. 先在 **终端2** 运行 `shm_server`。

~~~bash
./shm_server
~~~

4. 然后在 **终端1** 运行 `shm_client`。

~~~bash
./shm_client
~~~

5. 在 **终端1** 的 `shm_client` 中发送信息，会被 **终端2** 的 `shm_server` 收到。

6. 在 **终端1** 中发送 `quit` 终止通信。

**终端1**

~~~bash
[akashi@CentOS bin]$ ./shm_client 
 | 2023-03-05 20:37:26 | Debug   | create key done | key: 335609866
 | 2023-03-05 20:37:26 | Debug   | create shm done | shmid: 15
 | 2023-03-05 20:37:26 | Debug   | attach shm done | shmid: 15
Send Message: Hello server!      
Send Message: I am client   
Send Message: qwq
Send Message: quit
 | 2023-03-05 20:37:54 | Notice  | Quit | Client quit
 | 2023-03-05 20:37:54 | Debug   | detach shm done | shmid: 15
~~~

**终端2**

~~~bash
[akashi@HCYCentOS bin]$ ./shm_server 
 | 2023-03-05 20:37:24 | Debug   | create key done | key: 335609866
 | 2023-03-05 20:37:24 | Debug   | create shm done | shmid: 15
 | 2023-03-05 20:37:24 | Debug   | attach shm done | shmid: 15
 | 2023-03-05 20:37:35 | Notice  | Receive | Message: Hello server!
 | 2023-03-05 20:37:50 | Notice  | Receive | Message: I am client
 | 2023-03-05 20:37:52 | Notice  | Receive | Message: qwq
 | 2023-03-05 20:37:55 | Warning | Quit | Server quit
 | 2023-03-05 20:37:55 | Debug   | detach shm done | shmid: 15
 | 2023-03-05 20:37:55 | Debug   | delete shm done | shmid: 15
~~~

如果您遇到了下面的情况，请尝试使用 `ipcrm` 命令手动删除共享内存。

~~~bash
[akashi@CentOS bin]$ ./shm_server 
 | 2023-03-05 21:02:48 | Debug   | create key done | key: 335609866
 | 2023-03-05 21:02:48 | Error   | shm exists | The share memory is exist, use 'ipcrm' to remove it..

[akashi@CentOS bin]$ ipcs -m

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status      
0x1401000a 19         akashi     666        4096       0                       

[akashi@CentOS bin]$ ipcrm -m 19
~~~
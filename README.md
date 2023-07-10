# 进程通信

测试环境：CentOS7

## 项目结构

~~~bash
git clone https://github.com/AkashiNeko/IPC.git
cd IPC
~~~

~~~bash
IPC
├── include  # 头文件
│   ├── cmd.hpp
│   └── log.hpp
├── pipe     # 1. 命名管道
│   ├── bin
│   │   ├── pipe_client
│   │   └── pipe_server
│   ├── Makefile
│   ├── pipe_client.cc
│   └── pipe_server.cc
├── shm      # 2. 共享内存
│   ├── bin
│   │   ├── shm_client
│   │   └── shm_server
│   ├── Makefile
│   ├── shm_client.cc
│   └── shm_server.cc
└── shm_pipe # 3. 共享内存 + 命名管道
    ├── bin
    │   ├── shm_pipe_client
    │   └── shm_pipe_server
    ├── Makefile
    ├── shm_pipe_client.cc
    └── shm_pipe_server.cc
~~~

## 1. 管道通信（PIPE）

建立一个管道文件，客户端向管道文件中写入，服务端从管道文件中读取，实现进程通信。

### 1.1 服务端

#### (1) 创建管道文件

~~~cpp
int mkfifo(const char *pathname, mode_t mode);
~~~

`pathname`：文件路径。

`mode`：权限。

返回值：成功返回0，失败返回-1并设置 `errno`。

~~~cpp
if (mkfifo("fifo.ipc", 0666) < 0) {
    perror("mkfifo");
    exit(1);
}
~~~

#### (2) 打开管道文件

和打开普通文件相同。

~~~cpp
int open(const char *pathname, int flags);
~~~

`pathname`：文件路径。

`mode`：打开方式。

返回值：成功返回文件描述符fd（file descriptor），失败返回-1。

~~~cpp
int fd = open("fifo.ipc", O_RDONLY);
if (fd < 0) {
    perror("open");
    exit(2);
}
~~~

#### (3) 阻塞读取管道

阻塞从管道中读取内容，阻塞至读到内容。

~~~cpp
ssize_t read(int fd, void *buf, size_t count);
~~~

`fd`：文件描述符。

`buf`：缓冲区，读取到的内容写入该缓冲区中。

`count`：该缓冲区的最大容量。

返回值：读取成功时返回读到的字节数，读到EOF返回0，错误返回-1。

~~~cpp
char buffer[SIZE];
while (true) {
    memset(buffer, 0, SIZE);
    ssize_t s = read(fd, buffer, SIZE - 1);
    if (s > 0) {
        // Read
        cout << buffer << endl;
    } else if (s == 0) {
        // End Of File
        cout << "EOF" << endl;
        break;
    } else {
        // Read Error
        perror("read");
    }
}
~~~

#### (4) 关闭管道文件

和关闭普通文件相同。

~~~cpp
int close(int fd);
~~~

`fd`：文件描述符。

返回值：成功返回0，失败返回-1并设置 `errno`。

~~~cpp
close(fd);
~~~

#### (5) 删除管道文件

~~~cpp
int unlink(const char *pathname);
~~~

`pathname`：文件路径。

返回值：成功返回0，失败返回-1并设置 `errno`。

~~~cpp
unlink("fifo.ipc");
~~~

### 1.2 客户端

#### (1) 打开管道文件

同1.1(2)，但打开方式使用 `O_WRONLY` （只写）的方式。

~~~cpp
int fd = open("fifo.ipc", O_WRONLY);
if (fd < 0) {
    perror("open");
    exit(errno);
}
~~~

#### (2) 写入管道文件

~~~cpp
ssize_t write(int fd, const void *buf, size_t count);
~~~

`fd`：文件描述符。

`buf`：缓冲区，存放要写入的数据。

`count`：要写入的数据的字节数。

返回值：返回写入的字节数，失败返回-1并设置 `errno`。

~~~cpp
while (true) {
    std::string buffer;
    std::cout << "Send Message: ";
    std::getline(std::cin, buffer);
    write(fd, buffer.c_str(), buffer.size());
}
~~~

#### (3) 关闭管道文件

同1.1(4)。

~~~cpp
close(fd);
~~~

### 1.3 构建和运行

1. 进入 `pipe` 目录，使用 `make` 生成可执行文件。

~~~bash
cd pipe
make
~~~

2. 同时连接两个终端，使二者位于 `IPC/pipe/bin` 目录下。

3. 先在 **终端2** 运行 `pipe_server`。

4. 然后在 **终端1** 运行 `pipe_client`。

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

## 2. 共享内存（Share Memory）

~~~cpp
#define PATH_NAME "/home"
#define PROJ_ID 0x114514
#define SHM_SIZE 4096
~~~

### 2.1 服务端

#### (1) 获取key

~~~cpp
key_t ftok(const char *pathname, int proj_id);
~~~

`pathname`：一个现有的，可访问的路径名。

`proj_id`：项目id。

返回值：返回System V IPC key，失败时返回-1。

~~~cpp
key_t k = ftok(PATH_NAME, PROJ_ID);
if (k == -1) {
    perror("ftok");
    exit(errno);
}
~~~

#### (2) 获取共享内存ID

通过上一步获得的key创建共享内存，获得共享内存ID。

~~~cpp
int shmget(key_t key, size_t size, int shmflg);
~~~

`key`：System V IPC key。

`size`：共享内存大小。

`shmflg`：选项。

返回值：共享内存标识符，失败返回-1。

~~~cpp
int shmid = shmget(k, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
if (shmid == -1) {
    perror("shmget");
    exit(errno);
}
~~~

#### (3) 获取共享内存地址

~~~cpp
void *shmat(int shmid, const void *shmaddr, int shmflg);
~~~

`shmid`：上一步获得的共享内存ID。

`shmaddr`：如果为空，那么该内存将被连接在系统选择的第一个可用地址上。

返回值：返回该内存块的其实地址。

~~~cpp
void* shmaddr = shmat(shmid, nullptr, 0);
~~~

#### (4) 进程通信

从共享内存中读取信息，内存块数据结构如下。

| 起始地址，分配4个字节作为flag |  flag之后的地址，作为进程通信的缓冲区   |
| :---------------------------: | :-------------------------------------: |
| int（从起始位置开始4个字节）  | char..（从前4个字节之后，到内存块末尾） |

~~~cpp
memset(shmaddr, 0, SHM_SIZE);
int* p = (int*)shmaddr;
int& flag = *p;
char* msg = (char*)((char*)shmaddr + sizeof(int));
while (true) {
    if (flag) {
        if (strcmp(msg, "quit") == 0) {
            // quit
            std::cout << "Server quit" << std::endl;
            break;
        } else {
            std::cout << "Message: " << msg << std::endl;
            flag = 0;
            memset(msg, 0, strlen(msg) + 1);
        }
    }
    usleep(100000);
}
~~~

#### (5) 分离共享内存

从进程的地址空间中分离共享内存。

~~~cpp
int shmdt(const void *shmaddr);
~~~

`shmaddr`：共享内存块起始地址。

~~~cpp
int n = shmdt(shmaddr);
assert(n != -1);
~~~

#### (6) 移除共享内存

~~~cpp
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
~~~

`shmid`：共享内存ID。

`cmd`：指定的控制操作。

`buf`：指向shmid_ds结构的指针。

~~~cpp
n = shmctl(shmid, IPC_RMID, nullptr);
assert(n != -1);
~~~

### 2.2 客户端

#### (1) 获取key

同2.1(1)。

~~~cpp
key_t k = ftok(PATH_NAME, PROJ_ID);
if (k == -1) {
    perror("ftok");
    exit(errno);
}
~~~

#### (2) 获取共享内存ID

同2.1(2)，`shmflg`设为0。

~~~cpp
int shmid = shmget(k, SHM_SIZE, 0);
if (shmid == -1) {
    perror("shmget");
    exit(errno);
}
~~~

#### (3) 获取共享内存地址

同2.1(3)。

~~~cpp
void* shmaddr = shmat(shmid, nullptr, 0);
~~~

#### (4) 进程通信

向内存块中写入信息，内存块数据结构如下。

| 起始地址，分配4个字节作为flag |  flag之后的地址，作为进程通信的缓冲区   |
| :---------------------------: | :-------------------------------------: |
| int（从起始位置开始4个字节）  | char..（从前4个字节之后，到内存块末尾） |

~~~cpp
int* p = (int*)shmaddr;
int& flag = *p;
char* msg = (char*)((char*)shmaddr + sizeof(int));
while (true) {
    std::cout << "Send Message: ";
    std::string s;
    std::getline(std::cin, s);
	if (s.size() >= SHM_SIZE - sizeof(int)) {
        std::cout << "Message too long!" << std::endl;
    } else if (!s.empty()) {
        strcpy(msg, s.c_str());
	    flag = 1;
    	if (!strcmp(msg, "quit"))
        	break;
    }
}
~~~

#### (5) 分离共享内存

同2.1(5)。

~~~cpp
int n = shmdt(shmaddr);
assert(n != -1);
~~~

### 2.3 构建和运行

1. 进入 `shm` 目录，使用 `make` 生成可执行文件。

~~~bash
cd shm
make
~~~

2. 同时连接两个终端，使二者位于 `IPC/shm/bin` 目录下。

3. 先在 **终端2** 运行 `shm_server`。

4. 然后在 **终端1** 运行 `shm_client`。

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

## 3. 命名管道＋共享内存

### 3.1 服务端和客户端

原理与管道通信、共享内存相同，将其组合使用，管道用于阻塞等待，共享内存用于传递信息。

### 3.2 构建和运行

1. 进入 `shm_pipe` 目录，使用 `make` 生成可执行文件。

~~~bash
cd shm_pipe
make
~~~

2. 同时打开两个终端，使二者位于 `IPC/shm_pipe/bin` 目录下。

3. 先在 **终端2** 运行 `shm_pipe_server`。

4. 然后在 **终端1** 运行 `shm_pipe_client`。

5. 在 **终端1** 的 `shm_pipe_client` 中发送信息，会被 **终端2** 的 `shm_pipe_server` 收到。

6. 在 **终端1** 中发送 `quit` 终止通信。

**终端1**

~~~bash
[akashi@CentOS bin]$ ./shm_pipe_client 
 | 2023-03-06 22:27:05 | Debug   | create key done | key: 335609866
 | 2023-03-06 22:27:05 | Debug   | create shm done | shmid: 27
 | 2023-03-06 22:27:05 | Debug   | attach shm done | shmid: 27
 | Send Message: Hello
 | Send Message: share memory and pipe
 | Send Message: quit
 | 2023-03-06 22:27:33 | Notice  | Quit | Client quit
 | 2023-03-06 22:27:33 | Debug   | detach shm done | shmid: 27

~~~

**终端2**

~~~bash
[akashi@CentOS bin]$ ./shm_pipe_server 
 | 2023-03-06 22:27:01 | Debug   | Create FIFO | 
 | 2023-03-06 22:27:01 | Debug   | create key done | key: 335609866
 | 2023-03-06 22:27:01 | Debug   | create shm done | shmid: 27
 | 2023-03-06 22:27:01 | Debug   | attach shm done | shmid: 27
 | 2023-03-06 22:27:05 | Notice  | waitting | wait for client..
 | 2023-03-06 22:27:11 | Notice  | Receive  | Message: Hello
 | 2023-03-06 22:27:11 | Notice  | waitting | wait for client..
 | 2023-03-06 22:27:26 | Notice  | Receive  | Message: share memory and pipe
 | 2023-03-06 22:27:26 | Notice  | waitting | wait for client..
 | 2023-03-06 22:27:33 | Warning | Quit | Server quit
 | 2023-03-06 22:27:33 | Debug   | detach shm done | shmid: 27
 | 2023-03-06 22:27:33 | Debug   | delete shm done | shmid: 27
 | 2023-03-06 22:27:33 | Debug   | Remove FIFO | 
~~~

### 3.3 注意事项

如果遇到了下面的情况，请尝试使用 `ipcrm -m <shmid>` 手动删除共享内存。

~~~bash
[akashi@CentOS bin]$ ./shm_server
 | 2023-03-05 21:02:48 | Debug   | create key done | key: 335609866
 | 2023-03-05 21:02:48 | Error   | shm exists | The share memory is exist, use 'ipcrm' to remove it..

[akashi@CentOS bin]$ ipcs -m		# 查看当前系统中的共享内存

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status      
0x1401000a 19         akashi     666        4096       0                       

[akashi@CentOS bin]$ ipcrm -m 19	# 删除共享内存的shmid
~~~


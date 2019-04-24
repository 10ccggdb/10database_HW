## 前期工作

#### 简述文件映射的方式如何操作文件。与普通IO区别？为什么写入完成后要调用msync？文件内容什么时候被载入内存？

1、用文件映射的方式操作文件的工作流程是:

(1) 使用open()系统调用打开文件，并返回文件描述符

```
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int fd = open(const char *name, int flags, mode_t mode);
```

(2) 使用mmap()建立内存映射，并返回映射首地址指针

```
#include <sys/mman.h>
void* mmap(void* addr, size_t len, int port, int flags, int fd, off_t offset);
```

(3) 对映射文件进行各种操作，如printf、 sprintf、memcpy等

```
#include <stdio.h>
sprintf(char *string, char *format, arg_list);
```

(4) 用munmap解除映射

```
#include <sys/mman.h>
int munmap(void* addr, size_t len);
```

(5) 调用msync实现磁盘上文件内容与共享内存区的内容一致

```
#incldue <sys/mman.h>
int msync(void* addr, size_t len, int flags);
```

(6)调用close关闭文件fd

```
#include <unistd.h>
int close(int fd);
```

2、文件映射与普通I/O有什么区别？

普通I/O是首先用open系统调用打开文件，然后使用read, write, lseek等调用进行顺序或者随机的I/O，每一次I/O都需要一次系统调用。而文件映射使得进程间通过映射同一个普通文件实现内存共享，或者使用特殊文件提供匿名内存映射。对于普通文件，它减少了数据的复制，对于特殊文件，它用于进程之间的通信。

3、为什么写入完成后要调用msync？

调用msync实现磁盘上文件内容与共享内存区的内容一致。

4、文件内容什么时候被载入内存？

建立文件映射的过程中，并没有实际的拷贝数据，文件没有被载入内存，真正的数据拷贝是在缺页中断处理时进行的。



## 论文阅读

#### 操作SCM为什么要调用CLFLUSH等指令？

因为要访问SCM要经过很长的过程，包括存储缓冲区，CPU缓存，内存控制器缓冲区，所有的软件都很少起到控制的作用。SCMaware文件系统使用mmap函数使得应用层能够直接访问SCM，因此SCM写的有序性和持久性不能得到保证，为了解决这个问题，要调用CLFLUSH等指令。如果写入后不调用，发生系统崩溃会导致缓存行的内容还没有写入到内存里。

### FPTree的指纹技术有什么重要作用？

SCM中的无序叶进行线性扫描需要很大代价。指纹技术是无序的叶键的单字节散列，连续存储在叶的开头。在搜索的过程中先扫描，指纹技术起到过滤的作用，避免搜索那些有指纹但是跟搜索键不匹配的键，使用指纹技术，成功搜索到叶内键的期望次数等于1.这就大大提高了FPTree的性能。
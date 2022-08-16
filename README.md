# plog 
plog means p(erformance) log

## 发现的BUG解决方法汇总

### 1. 关于mmap的使用细节
在第一次测试时发现了问题，mmap报错errno=22，也就是invalid argument。 
分析了整个调用链之后发现是没有设置mmap的大小（在log_config.hpp中设置），
简单设置之后问题仍未解决，原来是mmap映射区域大小必须是物理页大小(page_size)的整倍数（32位系统中通常是4k字节）。
原因是，内存的最小粒度是页，而进程虚拟地址空间和内存的映射也是以页为单位。
为了匹配内存的操作，mmap从磁盘到虚拟地址空间的映射也必须是页。
详细介绍mmap的文章：[详细分析mmap](https://www.cnblogs.com/huxiao-tee/p/4660352.html)

### 2. mmap报错bad file descripter
(尚未解决，下次再整)....

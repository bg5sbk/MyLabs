元旦期间我实验了最简单的setcontext族函数的使用，元旦过后决定利用setcontext族函数加pthread实现一个只针对Linux的协程库，昨天花了一天时间终于在深夜调通了程序。

这个协程库我打算做成像erlang那样的actor模式，libactor这名字已经有人用了，那是一个基于pthread封装的actor库，所以我做的这个库名叫做libact。

libact的基本原理就是程序启动时创建数个pthead线程作为调度器，协程列表则用一个头尾相连的环形链表表示，协程具有不同的状态，分别是：runable，running，waiting，exit。

调度器线程每次调度一个协程，调度算法就是从上一次被调度的协程开始找到最近的一个runable状态的协程，然后利用swapcontext函数把上下文切换到协程的最后状态。

我设想协程会在几种不同的情况下会让出调度器，分别是：

1. 协程自然退出
2. 协程调用act\_yield函数，让出调度器
3. 协程调用act\_sleep函数，进入睡眠
4. 协程调用了act\_receive函数，但消息队列是空的
5. 协程需要等待IO操作完成

作为一个实验原型，act\_yield是最基础也最容易实现的，3 ~ 5的情况都需要做比较多的额外封装，所以我只先实现了act\_yield函数。

需要强调一点：这只是一个实验性的项目，目的是实践C/C++中协程的可行性，产出的代码并没有实用价值，并且未在任何正式项目上使用，所以请大家不要把这里的代码用到实际项目中（setcontext不一定如想象的那么稳定，下面我会提到）。

我最初追求在C/C++下实现协程的目的是像Erlang那样设计服务端程序，虽然初步已经可以实现一个简单的协程库，但是服务端真的非用这种方式不可吗？

在做完这个原型的时候我跟同行的一位前辈进行的讨论，感谢这位前辈帮我理清了思路。

首先，使用协程模型和异步模型的目的都一样，就是在需要等待的时候把CPU让给别的事务，相较于异步，协程的优势在于它将不同事务隔离开，并让程序员可以不用自己管理大量的异步状态，这让程序设计更容易，但这写优势只有在异步方式设计中需要涉及到很多不同事务的不同状态时才会显现出来。

而服务端需要等待的操作其实并不多，无非是：磁盘IO、数据库、网络，而这些环节又是非逻辑性的代码，只需要初期在机制上设计一次就不需要再动到了，完全可以针对这几个主要环节进行针对性的异步设计，也可以用几个线程把事务隔离开，所以协程的优势其实不明显。

其次，因为协程的上下文切换依靠的是保存CPU寄存器，自己实现的协程需要格外小心，否则程序很容易出现很难调试的错误，例如我在网上就了解到setcontext和getcontext函数在i386的CPU架构上是跟pthread不兼容的，因为getcontext保存上下文的寄存器时会忽略存放指向当前线程结构体的寄存器，导致pthread\_self()在跨上下文切换后会出现取到别的线程ID的情况。

并且自己实现的协程不是语言原生支持的，不能像Erlang那样公平调度，也不能像Go语言那样自动扩展栈空间，所以是有一定局限性的。

再加上上面说的协程需要上下文切换的几种情况都需要重新封装一组函数，我觉得目前自己花大量时间来做这些，然后再花大量时间排错和调试，不如直接用Go语言好了。

综合以上观点，的确如同行的前辈所说的，还是按传统套路做最好。

所以接下来一段时间，我会把关注点移到zmq和libevent等这些传统的开源库上，至于协程在C/C++中的实践，暂时就到这里，以后是否自己做个脚本语言自己实现更高效更保险的协程呢？那应该是要很久以后的事了。

有兴趣继续研究协程的朋友，可以了解下GUN Pth项目，我今天上去下了一份代码，发现它封装得很完善，这也间接证明了要做一个真正可用的协程库，不是一个人十天半个月就可以搞定的事情。

By 达达

2012年1月7日 凌晨2点30分

一些关于C++程序嵌入V8引擎的实验

编译v8注意事项：

1. 需要装g++、scons、python
2. ubuntu上除了装g++还需要装g++-multilib，否则会出现bits/predefs.h文件找不到的错误
3. 编译v8使用：scons mode=release library=static snapshots=on


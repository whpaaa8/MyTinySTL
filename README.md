# MySTL

------

## 简介

- 这是一个 基于 C++ 11 的 tinySTL项目，实现了大部分的STL容器与算法。
- 自行添加了 BITMAP，TRIE_TREE, 智能指针等容器
- 添加了 KMP 等算法，用于优化 匹配函数Search()
- 我将学习过程中的一些笔记，和一些重要的实现分析也一同上传至 code 中
- 参考： 《STL源码剖析》与项目 [GitHub - Alinshans/MyTinySTL: Achieve a tiny STL in C++11](https://github.com/Alinshans/MyTinySTL)



## 支持

* 操作系统
  * linux
  * windows
  * osx
* 编译器
  * g++ 5.4 或以上
  * clang++ 3.5 或以上
* 构建
    * 使用 cmake 2.8 来构建项目（**可选**）



## 运行

```bash
$ cd MyTinySTL
$ mkdir build && cd build
$ cmake ..
$ make
$ cd ../bin 
$ ./STL_TEST.exe | tee test_result.txt
```


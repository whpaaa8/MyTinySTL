# TinySTL的整体架构

------

## 简介

- 这是一个 基于 C++ 11 的 tinySTL项目，实现了大部分的STL容器与算法。
- 自行添加了 BITMAP，TRIE_TREE, 智能指针等容器
- 添加了 KMP 等算法，用于优化 匹配函数Search()
- 我将学习过程中的一些笔记，和一些重要的实现分析也一同上传至 code 中
- 参考： 《STL源码剖析》与项目 [GitHub - Alinshans/MyTinySTL: Achieve a tiny STL in C++11](https://github.com/Alinshans/MyTinySTL)

## 空间配置器相关

 1. `alloc.h` ： 封装了 一 二级空间配置器,用于分配空间

 2. `allocator.h`：为满足STL标准的要求 `allocator<T>`，封装的一个对外接口类

    > - 满足 使用 alloc 进行内存分配
    > - 封装了对应的 construct 与 destory 操作

 3. `constructor.h`： 包含一些全局函数，用于 构造 与 析构对象

    > - constructor通过 placement new 进行构造
    >
    > - 构造包括默认构造，复制构造，原地构造
    >
    >   ```C++
    >   // 原地构造
    >   template <class Ty, class... Args>
    >   void construct(Ty* ptr, Args&&... args)
    >   {
    >     ::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
    >   }
    >   ```
    >
    > - 析构包括普通析构，范围析构
    >
    > - 构造与析构对应的类型对象 T 时，通过 type_traits 萃取对象属性，执行对应的重载函数

------

## 萃取器相关: <type_traits.h>

直接使用了 c++11 的 <type_traits>，里面包含各种类型的 属性萃取器（自己写也可以）

------

## 内存处理工具相关

 1. `uninitialzed.h`： 包含一些全局函数,copy(),fill(),fill_n()用于更加高效的进行内存操作

    > - uninitialzed_copy(): 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
    >   用于 复制构造/扩容时 的 数据对象 的 初始化。
    >
    > - uninitialzed_fill():在 [first, last) 区间内填充元素值
    >
    >   用于 在区间间   通过 填充数据来初始化
    >
    > - uninitialzed_fill_n(): 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
    >
    >   类似 vector<int>(n,0) 这种 指定 value，并初始化 n 个对象
    >
    > - uninitialized_move()：把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
    >
    >   用于移动构造初始化

 2. `memory.h`: 封装了一些全局函数与对象，如申请临时缓冲区，智能指针等操作

    > - get_temporary_buffer(): 申请缓冲区，相当于malloc一块区域作为缓冲区
    > - release_temporary_buffer(T* ptr)： 释放缓冲区
    > - class temporary_buffer: 封装了缓冲区对应信息，如缓冲区指针，大小等数据
    > - shared_ptr,unique_ptr,_weak_ptr 等智能指针

 3. `utils.h`：封装了一些 通用工具，全局函数 以及 pair 类

    > - move(),forward() 等全局函数，实现移动语义与完美转发
    > - swap(),swap_range() 等全局函数，实现数据交换
    > - class pair: 用来实现 键值对类型

------

## 迭代器相关

**`iterator.h`： 设计了 迭代器相关 的 类模板，与通用的 迭代器操作相关的全局函数**

1. 五种迭代器类型： input,output,forward,bidirectional,random_access 迭代器

   ```C++
   // 五种迭代器类型
   struct input_iterator_tag {};
   struct output_iterator_tag {};
   struct forward_iterator_tag : public input_iterator_tag {};
   struct bidirectional_iterator_tag : public forward_iterator_tag {};
   struct random_access_iterator_tag : public bidirectional_iterator_tag {};
   ```

   

2. 迭代器类模板 struct iterator：任何 自行设计的迭代器 都应该 继承这个类

3. 迭代器萃取器 iterator_traits<>：针对普通类型，与原生指针设计了 泛化与偏特化版本。用于萃取迭代器的属性

4. 迭代器通用函数:  

   ```C++
   iterator_category(const Iterator&); // 获取迭代器类型: 五种类型之一
   distance_type(const Iterator&); // 获取迭代器的距离类型:difference_type
   value_type(const Iterator&);  // 获取迭代的value_type
   
   // 计算迭代器之间的距离(因为前向迭代器，与随机访问迭代器的计算方法不同，通过traits萃取对应的重载函数进行实现)
   distance(InputIterator first, InputIterator last); 
   // 用于前进迭代器
   void advance(InputIterator& i, Distance n)
   ```

5. 设计了 反向迭代器 reverse_iterator<Iterator>：对一个迭代器进行反向操作

------

## 容器相关

1. `vector.h`： 包含vector的基本操作，包括 移动构造，原地构造等
2. `list.h`： 循环双向链表，包括基本的 插入，删除操作；重要的 切片，合并，排序等操作
3. `deque.h`： 双端队列，map中控器+缓冲区实现。队头队尾常数时间的插入复杂度
4. `stack.h`： 栈，适配器，相当于对 deque 进行了再封装，调用一些接口，自成一种风貌
5. `queue.h`：队列，同 栈；以及实现了 优先队列（通过 堆相关函数）
6. `hashtable.h`： 哈希表，提供了两套支持“重复键值”与“唯一键值”
7. `unordered_set.h`： 哈希集合，哈希表实现
8. `unordered_map.h`： 哈希map，哈希表实现
9. `basic_string.h`与`astring.h`： 字符串的实现
10. `bitmap.h`： 实现了一个位图数据结构
11. `trietree.h`： 实现了一个前缀树数据结构

------

## 算法相关

1. `algorithm.h`： 集合了其他所有算法的头文件

2. `numeric.h`： 包括一些数值算法

   > - `accumulate() `：  对数据进行 二元操作(op),如 加减乘除等
   > - `adjacent_difference()`: 自定义相邻元素的二元操作
   > - `inner_product()` ： 计算区间的内积
   > - `iota()`： 填充[first, last)，以 value 为初值开始递增
   > -  `partial_sum()`：进行局部进行自定义二元操作

3. `algo.h`： 包含常用的算法

   > - sort(),search(),binary_search()等函数
   > - 自行添加 search_KMP() KMP算法，用于加速计算

4. `algobase.h`： 包括一些基础算法

   > - max(),min(),equal()等
   > - copy(),copy_backward(),fill()等算法

------

## 测试相关

通过 宏定义 封装测试函数，测试过程。

对每一种容器都进行了测试：基本的功能测试 与 效率测试

对算法也进行了功能测试，对部分算法如KMP，与search()相比较，进行测试

> PS 测试时踩过的坑：
>
> 	- 当我测试 KMP算法与search()算法的效率时，发现search()无论数据量多大，耗时都为0ms。
> 	- 实际上是因为： 构建项目时 使用了 gcc -O2 进行 代码优化。
> 	- search()返回值类型为迭代器，一开始测试时忽略了返回值，导致代码被优化，将不执行/或者说删除掉相关的语句。导致永远都不执行search(),而 KMP 算法却不会被优化(不知道为啥)。
> 	- 最后强制将返回值设置给变量 并且使用这个变量(通过构建一个新字符串),使得无法被优化.成功测试
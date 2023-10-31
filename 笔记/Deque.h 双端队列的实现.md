# Deque.h: 双端队列的实现



##  简述： Deque

	* 实现方式： MAP中控器+缓冲区实现
	* 特点： 多段连续 地址空间
	* Deque 需要封装迭代器： 随机访问迭代器 
	* 支持常数级别的 头尾插入操作，以及查询



## Deque迭代器：deque_iterator

  		1. 定义数据成员属性：
       *  value_type,pointer等
       * 定义 map_pointer 与 value_pointer类型
       * map_pointer是二级指针

![1698231021396](E:\WeChat Files\wxid_ijm69dn0xkja22\FileStorage\Temp\1698231021396.png)

2. 数据成员
   * 缓冲区的三个指针：first,end,cur------代表着缓冲区的头尾，与当前元素
   * 指向当前 map 的 map_pointer: node
   * 每个缓冲区的元素个数  buffer_size

3. 函数方法

   * 各种构造函数：默认，拷贝，移动构造

   * 重载=赋值运算，移动赋值等

   * 重载运算符++,--,+=,-=,[]等： 用于迭代器的移动

     > 需要先判断 移动后是否 仍然落在 同一个 map上
     >
     > 否则需要移动到 对应 map
     >
     > 因此： MAP（即二级指针需要连续）

   * 重载 >,<等用于比较

   > 迭代器的距离 difference_type 表示的是 元素的距离（个数）
   >
   > map_pointrt(node)之间的 距离 表示的是 缓冲区的距离（个数）



## Deque实现



### 定义成员类型

		1. 迭代器类型：iterator
		1. 元素类型：value_type等
  		3. 空间分配器( 两种)： 
       		1. 用于分配元素节点的空间：allocator<T> 每次配置一个元素大小
       		2. 用于分配map节点的空间： allocator<T*>每次配置一个指针大小

### 数据结构

 - 整个deque的首部与尾部迭代器itertor:  `begin,end `

 - 中控器 map: `map_pointer map` 指向第一个map

 - 整个容器大小 map_size:`size_t map_size`

 - 缓冲区大小 buffer_size

   > 首先要记录 首尾元素（迭代器），保证首尾操作
   >
   > 利用map+map_size相当于记录了中控器map的 首尾位置

## 函数方法

### 构造函数

1. 包括默认构造

2. 通过value + size构造

3. 赋值构造与移动构造

4. 通过迭代器进行复制构造

   > 构造时，为了尽可能减少后续 分配内存操作的可能性
   >
   > 1.先去计算 需要的 缓冲区数量，开辟 map与缓冲区空间
   >
   > 2.让数据位于数据结构的中间： 计算出开始地址与结束地址，进行初始化

   > 构造（init）
   >
   > 分配空间： 像二次函数一样，逐层分配
   >
   > ​					先分配Map的空间create_map()，再在每个map上create_buffer()
   >
   > ​					每次会预分配 多2块（几块）map，减少之后的分配次数
   >
   > ​					将begin__与end设置在 中间位置，即数据从中间开始填充
   >
   > 初始化:  	使用uninitialzied_copy,fill 等 进行初始化

### 迭代器相关操作：begin(),end()等



### 容量相关操作： size( ),resize( ), shrink_to_fit( ) 等



### 重载运算符    [  ]     用于访问操作





## 数据操作相关:  插入，删除等

		### *插入操作相关*

![1698233626285](E:\WeChat Files\wxid_ijm69dn0xkja22\FileStorage\Temp\1698233626285.png)

> 1. 插入元素，需要判断容量是否足够
> 2. 如果容量不够，则需要先去扩容， 再去进行插入
> 3. 扩容时，申请2倍以上的空间，先申请内存，然后复制构造原来的deque，在添加
> 4. 扩容策略： 前端扩容， 后端扩容
> 5. 这些是对外接口，实际上的实现与 一些 helper函数

### helper 函数

![1698233937171](E:\WeChat Files\wxid_ijm69dn0xkja22\FileStorage\Temp\1698233937171.png)

> 接口 的 底层实现，真正的实现于此，接口只负责选择底层的处理函数

![image-20231025194052058](E:\备份\学习\大四\秋招\STL源码解析\学习笔记\image-20231025194052058.png)

> 用于分配内存，扩容使用
>
> 扩容时： 先create_map（） 创建多个map，
>
> 再create_buffer() 创建缓冲区, 将原来数据拷贝到新地址时，也是尽量往中间拷贝



## *删除操作相关*

		1. 删除元素时： 需要将之后的元素 往前移动
		1. 先destory析构元素，然后移动，并不去主动释放空间
		1. 只有当shrink_to_fit 调用时，才会去紧缩空间



###   








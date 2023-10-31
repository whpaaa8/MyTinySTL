# 配接器 Adapters

## 前言

1. 配接器指： 改变对象接口，形成另一种风貌的对象
2. 根据配接对象的不同，分为 function_adapter,container_adapter,iterator_adapter 三种适配器
3. 分别是 修改容器接口，修改迭代器接口，修改仿函数接口

## Container adapters

容器的适配器：

​	stack，queue 通过修改 deque的接口，形成的一种适配器

## iterator adapters

迭代器适配器： 如insert_iterators,reverse_iterators,stream_iterators

 	1. `insert_iterators`： 对迭代器的 = 进行修改封装，将赋值操作，改为插入操作
 	2. `reverse_iterators`： 对迭代器的++，--等进行反向操作，逆向前进

## Function adapters


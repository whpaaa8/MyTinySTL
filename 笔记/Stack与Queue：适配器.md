# Stack与Queue：适配器

------

## 前言

	1. Stack与Queue是适配器，使用了其他容器作为底层，只是封装了一些接口：调用底层的容器的接口
	1. 没有迭代器，不允许进行元素访问
	1. 默认以deque作为底层，list也可以（需要能够在首尾添加元素的序列式容器）
	1. 将 底层容器 Container 设置为 私有，这样，底层容器的方法对外不可见

## Stack：后入先出

1. push_back()与pop_back()即可
2. 所有函数方法都是 调用 底层的接口

```C++
container_type c_;  // 用底层容器表现 stack
stack(size_type n, const value_type& value) 
    :c_(n, value)
  {
  }

  template <class IIter>
  stack(IIter first, IIter last)
    : c_(first, last)
  {
  }
reference       top()       { return c_.back(); }
 void emplace(Args&& ...args)
  { c_.emplace_back(mystl::forward<Args>(args)...); }

  void push(const value_type& value)
  { c_.push_back(value); }
  void push(value_type&& value)      
  { c_.push_back(mystl::move(value)); }

  void pop() 
  { c_.pop_back(); }

  void clear() 
  {
    while (!empty())
      pop();
  }
```

------

## queue：先入后出

1. push_back()但pop_front()
2. 适配器，封装接口

------

## priority_queue: 优先队列-堆

### 实现

1. 底层是vector，完全二叉树
2. 使用<algorithm>中的 heap 相关的算法来实现
3. 传入模板参数 compare:需要是一个仿函数 

## heap相关函数

1. make_heap(迭代器1，迭代器2，仿函数cmp)

   > 构建堆：从最后一个非叶子节点往前，调用adjust_heap()，保证每个子树都是顶堆
   >
   > adjust()用于将一个非叶子节点往下调整，到其应该的位置
   >
   > ```C++
   > template <class RandomIter, class Distance, class Compared>
   > void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compared comp)
   > {
   >   if (last - first < 2)
   >     return;
   >   auto len = last - first;
   >   auto holeIndex = (len - 2) / 2;
   >   while (true)
   >   {
   >     // 重排以 holeIndex 为首的子树
   >     mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
   >     if (holeIndex == 0)
   >       return;
   >     holeIndex--;
   >   }
   > }
   > ```
   >
   > ```C++
   > // 重载版本使用函数对象 comp 代替比较操作
   > template <class RandomIter, class T, class Distance, class Compared>
   > void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value,
   >                  Compared comp)
   > {
   >   // 先进行下溯(percolate down)过程
   >   auto topIndex = holeIndex;
   >   auto rchild = 2 * holeIndex + 2;
   >   while (rchild < len)
   >   {
   >     if (comp(*(first + rchild), *(first + rchild - 1)))  --rchild;
   >     *(first + holeIndex) = *(first + rchild);
   >     holeIndex = rchild;
   >     rchild = 2 * (rchild + 1);
   >   }
   >   if (rchild == len)
   >   {
   >     *(first + holeIndex) = *(first + (rchild - 1));
   >     holeIndex = rchild - 1;
   >   }
   >   // 再执行一次上溯(percolate up)过程
   >   mystl::push_heap_aux(first, holeIndex, topIndex, value, comp);
   > }
   > ```
   >
   > 

2. push_heap(迭代器1，迭代器2，仿函数cmp)

   > push_heap:将元素插入 数组 最后，不断向上调整整个 元素的位置即可
   >
   > 将一个叶子节点向上调整到该去的位置
   >
   > ```C++
   > void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value,
   >                    Compared comp)
   > {
   >   auto parent = (holeIndex - 1) / 2;
   >   while (holeIndex > topIndex && comp(*(first + parent), value))
   >   {
   >     *(first + holeIndex) = *(first + parent);
   >     holeIndex = parent;
   >     parent = (holeIndex - 1) / 2;
   >   }
   >   *(first + holeIndex) = value;
   > }
   > ```

3. pop_heap() 弹出堆顶元素

   > pop_heap()将堆顶元素与最后一个元素交换，最后adjust(堆顶)即可


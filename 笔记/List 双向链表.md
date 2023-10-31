# List 双向链表

## 简述

1. 链表节点+迭代器 实现环状双向链表
2. 链表的 首尾 插入 ，删除；
3. 链表的 splice 切片，merge 合并， sort 排序

------

## 链表结点（list_node）

 	1. 前后指针： next, prev
 	2. 数据: T data

------

## 迭代器  (list_iterator)

 	1. 迭代器种类： 双向迭代器: 定义各种变量类型
 	2. 数据成员:list_node
 	3. 重载运算符 ++ , -- ,* , ->, ==

------

## 双向链表 （list）

### 定义基本类型

		- 空间配置器：allocator<list_node>
		- 各种数据类型; value_type等
		- 定义 迭代器类型： iterator， const iterator等

## 数据结构

  - 由一个list_node构成: `list_node node_`;
  - 链表长度 size_: 记录链表的长度

> 由一个 list_node 构成， 前闭后开原则。
>
> node_->next == node 说明链表为空，尾部节点为空

## 内存与构造

#### 构造函数：

		1. 普通构造,用value填充：fill_init() 
		1. 复制构造：迭代器或者list构造--> copy_init()

### 申请内存与释放

	1. create_node() 创建 node
	1. construct(ptr,T)构造节点
	1. destroy（）析构
	1. deallocate()释放空间

------

	## 元素操作

### 基本元素操作：begin(),end(),cbegin(),cend()等

### 插入，弹出操作：push (),pop()

	1. push_front(),push_back(),pop_front(),pop_back()
	1. unlink(ptr1,ptr2)断开两个节点的连接

------

## 迁移操作相关：transfer() , splice(), merge(), sort()

### splice（）进行迁移操作： 将某段list 移动到 position 前

```C++
// 将 list x 的 [first, last) 内的节点接合于 pos 之前
template <class T>
void list<T>::splice(const_iterator pos, list& x, const_iterator first, const_iterator last)
{
  if (first != last && this != &x)
  {
    size_type n = mystl::distance(first, last);
    THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
    auto f = first.node_;
    auto l = last.node_->prev;

    x.unlink_nodes(f, l);
    link_nodes(pos.node_, f, l);

    size_ += n;
    x.size_ -= n;
  }
}
```

 实际上是重载了 三个 splice来实现：因为其他的函数 也是 用splice实现的

> merge() 通过归并链表来实现

### Sort（） 归并排序

1. 可以采用递归：但是时间消耗较高
2. 不采用递归，采用循环的方式归并

```C++
template<class T>
template <class Compare>
void list<T>::sort(Compare comp){
    if (empty() || head.p->next == tail.p)
        return;

    list carry;
    list counter[64];
    int fill = 0;
    while (!empty()){
        // 每次将链表中的一个元素 归并到 counter中的链表中
        carry.splice(carry.begin(), *this, begin());
        int i = 0;
        while (i < fill && !counter[i].empty()){
            counter[i].merge(carry, comp);
            carry.swap(counter[i++]);
        }
        // 将每次归并的链表 插入到 counter[i]
        carry.swap(counter[i]);
        if (i == fill)
            ++fill;
    }
    // 将counter中的所有链表进行归并
    for (int i = 0; i != fill; ++i){
        counter[i].merge(counter[i - 1], comp);
    }
    swap(counter[fill - 1]);
}
```


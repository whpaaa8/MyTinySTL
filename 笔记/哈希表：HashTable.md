# 哈希表：HashTable

------

## 简述

1. 底层由 vector 容器实现，是unoordered_set和unordered_map的实现
2. 存的值有两种 ： 值，键值对，由不同的value_traits进行 属性萃取
3. 主要构成： vector， 哈希函数， Key_equal函数
4. 提供多种迭代器 ht_itertor 和 ht_local_iterator 提供不同的迭代类型,都继承自 ht_iterator_base 

------

## 哈希节点 : ht_node

```C++
// hashtable 的节点定义
template <class T>
struct hashtable_node
{
  hashtable_node* next;   // 指向下一个节点
  T               value;  // 储存实值

  hashtable_node() = default;
  hashtable_node(const T& n) :next(nullptr), value(n) {}

  hashtable_node(const hashtable_node& node) :next(node.next), value(node.value) {}
  hashtable_node(hashtable_node&& node) :next(node.next), value(mystl::move(node.value))
  {
    node.next = nullptr;
  }
};
```

> T  是 数据类型： 为一种类型，或者 pair类型-用作键值对。

```C++
// 用于萃取出 T 中的 键值对属性
template <class T>
struct ht_value_traits
{
  static constexpr bool is_map = mystl::is_pair<T>::value;

  typedef ht_value_traits_imp<T, is_map> value_traits_type;

  typedef typename value_traits_type::key_type    key_type;
  typedef typename value_traits_type::mapped_type mapped_type;
  typedef typename value_traits_type::value_type  value_type;

  template <class Ty>
  static const key_type& get_key(const Ty& value)
  {
    return value_traits_type::get_key(value);
  }

  template <class Ty>
  static const value_type& get_value(const Ty& value)
  {
    return value_traits_type::get_value(value);
  }
};
```

> 1. value 萃取器，可以通过 T 的类型 萃取出对应的 key
> 2. 如果是set： 存储value，则value就作为 hash 映射的 key
> 3. 如果是map: 存储pair<key,value>, 则 使用 pair.key作为 key进行映射

## 迭代器 (ht_iterator)

```C++
// forward declaration

template <class T, class HashFun, class KeyEqual>
class hashtable; // 哈希表

template <class T, class HashFun, class KeyEqual>
struct ht_iterator; // 迭代器，可以前向迭代 所有 的 哈希表 中的 节点

template <class T, class HashFun, class KeyEqual>
struct ht_const_iterator;

template <class T>
struct ht_local_iterator;// 本地迭代器，只能在 同一个 bucket 中 迭代

template <class T>
struct ht_const_local_iterator;
```

### 基本迭代器（所有哈希表的迭代器 都要 继承于base）

```C++
template <class T, class Hash, class KeyEqual>
struct ht_iterator_base :public mystl::iterator<mystl::forward_iterator_tag, T>
{
  typedef mystl::hashtable<T, Hash, KeyEqual>         hashtable;
  typedef ht_iterator_base<T, Hash, KeyEqual>         base;
  typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;
  typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
  typedef hashtable_node<T>*                          node_ptr;
  typedef hashtable*                                  contain_ptr;
  typedef const node_ptr                              const_node_ptr;
  typedef const contain_ptr                           const_contain_ptr;

  typedef size_t                                      size_type;
  typedef ptrdiff_t                                   difference_type;

  node_ptr    node;  // 迭代器当前所指节点
  contain_ptr ht;    // 保持与容器的连结

  ht_iterator_base() = default;

  bool operator==(const base& rhs) const { return node == rhs.node; }
  bool operator!=(const base& rhs) const { return node != rhs.node; }
};

```

> - node_ptr： 代表当前 迭代器 指向的节点
> - container_ptr： 指向 哈希表的指针，因为可能需要用到 hashtable 中的 bucket

### 全局迭代器( ht_iterator )

```C++
iterator& operator++()
  {
    MYSTL_DEBUG(node != nullptr);
    const node_ptr old = node;
    node = node->next;
    if (node == nullptr)
    { // 如果下一个位置为空，跳到下一个 bucket 的起始处
      auto index = ht->hash(value_traits::get_key(old->value));
      while (!node && ++index < ht->bucket_size_)
        node = ht->buckets_[index];
    }
    return *this;
  }
  iterator operator++(int)
  {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }
```

​	重载时，当本地的bucket结束后，遍历后面的所有 bucket，直到找到下一个值；（类似deque）

------

## 哈希表：HashTable

### 实现原理

> - 底层使用vector，每个vector元素作为一个bucket，维护一个链表
> - 通过hash(key)，将每个hash_node映射到对应的bucket中
> - 通过 “开链法” 解决冲突：将冲突的 node，通过链表串联起来
> - 维护 一个 ht_prime_list[] 作为 bucket_size，里面维护了(28-99)个 质数作为size（即vector 大小）。
> - 质数大小的 hashtable，有利于 减少 冲突 的 发生

```C++
// 这里 维护了 44 个 质数 作为vector大小
static constexpr size_t ht_prime_list[] = {
  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u, 
  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
  2006689337u, 3010034021u, 4294967291u,
};
```

> 使用 vector 作为底层容器的好处：
>
> 1. 连续的地址空间，方便hash映射
> 2. hashtable的 扩容 直接可以 交由 vector 的 扩容机制 完成，不需要自己进行

### 数据结构

```C++
// 定义空间分配器：实际上只用到了 node_allocator
typedef mystl::allocator<T>                         allocator_type;
typedef mystl::allocator<T>                         data_allocator;
typedef mystl::allocator<node_type>                 node_allocator;

// 定义迭代器
typedef mystl::ht_iterator<T, Hash, KeyEqual>       iterator;
typedef mystl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
typedef mystl::ht_local_iterator<T>                 local_iterator;
typedef mystl::ht_const_local_iterator<T>           const_local_iterator;

private:
  // 用以下六个参数来表现 hashtable
  bucket_type buckets_;		// vector 实现的 buckets
  size_type   bucket_size_; // vector 的 实际大小，即buckets数量
  size_type   size_;		// 已经映射到 哈希表中 的 元素 的 数量
  float       mlf_;			// 负载率： 表示 哈希表 的 负载情况； 用于判断是否需要 rehash
  hasher      hash_;		// 哈希函数
  key_equal   equal_;		// 比较 key 值 是否相等
```

### 函数方法

### 构造： init() 与 copy_init()

```C++
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
init(size_type n)
{
  const auto bucket_nums = next_size(n);
  try
  {
    buckets_.reserve(bucket_nums);
    buckets_.assign(bucket_nums, nullptr);
  }
  catch (...)
  {
    bucket_size_ = 0;
    size_ = 0;
    throw;
  }
  bucket_size_ = buckets_.size();
}
```

> 	1. 底层 vector 的 初始化
> 	1. 为 每个 bucket 设置 初始值

#### 插入： insert()  与 emplace_back()

insert():

```C++
// 在不需要重建表格的情况下插入新节点，键值不允许重复
template <class T, class Hash, class KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::
insert_unique_noresize(const value_type& value)
{
  const auto n = hash(value_traits::get_key(value));
  auto first = buckets_[n];
  for (auto cur = first; cur; cur = cur->next)
  {
    if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(value)))
      return mystl::make_pair(iterator(cur, this), false);
  }
  // 让新节点成为链表的第一个节点
  auto tmp = create_node(value);  
  tmp->next = first;
  buckets_[n] = tmp;
  ++size_;
  return mystl::make_pair(iterator(tmp, this), true);
}
```

每种 insert（）都对应 unique与muilt行为，允许插入相同 的 value 与 不相同 的 value

***每次 insert()前 都应该 执行 rehash_if_need (判断是否需要rehash)***

> 1. 通过 hash 获得 对应的buckets
> 2. 新建节点， 并且 插入到 链表顶端

插入时 执行检测 ： 如果 size 大于了 hash_table 的 负载，则需要 rehash

```C++
    if ((float)(size_ + 1) > (float)bucket_size_ * max_load_factor())
      rehash(size_ + 1);
```

### 查询函数： find() 与 count()

```C++
template <class T, class Hash, class KeyEqual>
typename hashtable<T, Hash, KeyEqual>::const_iterator
hashtable<T, Hash, KeyEqual>::
find(const key_type& key) const
{
  const auto n = hash(key);
  node_ptr first = buckets_[n];
  for (; first && !is_equal(value_traits::get_key(first->value), key); first = first->next) {}
  return M_cit(first);
}
```

遍历查询，是否存在 key 值相等的 节点。可以通过迭代器 遍历

### 哈希重整： rehash()

```C++
// 重新对元素进行一遍哈希，插入到新的位置
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
rehash(size_type count)
{
  auto n = ht_next_prime(count);
    // 需要扩容的情况
  if (n > bucket_size_)
  {
    replace_bucket(n);
  }
  else
  {
      // 判断并要性
    if ((float)size_ / (float)n < max_load_factor() - 0.25f &&
        (float)n < (float)bucket_size_ * 0.75)  // worth rehash
    {
      replace_bucket(n);
    }
  }
```

**哈希重整的两种情况**

先进行 ht_next_prime(count); 获取 最接近 count 的 容量，这将是  rehash 后的 大小

> 1. n > bucket_size： 扩容需求，直接去进行扩容 与 rehash
>
> 2. n < bucket_size:   判断必要性，再进行 缩容
>
>     必要性： 缩小size后，利用率 < 75% 并且 缩小规模 < 75% 时 才能进行缩容
>
>    即 缩容后，如果导致 负载率 过高，不进行扩容
>
>    如果 缩减的 幅度 太小，也不缩容，减少内存分配的次数

### rehash的使用场景

> 1. **每次 emplace,insert时**，调用 rehash_if_need(n) 判断当前哈希表是否需要 rehash
>
>    ```C++
>    // rehash_if_need 函数
>    // 计算 当前 元素个数，是否 大于 哈希表的 负载量，大于则去 rehash
>    template <class T, class Hash, class KeyEqual>
>    void hashtable<T, Hash, KeyEqual>::
>    rehash_if_need(size_type n)
>    {
>      if (static_cast<float>(size_ + n) > (float)bucket_size_ * max_load_factor())
>        rehash(size_ + n);
>    }
>    ```
>
>    插入 n 个 元素后 是否 需要 rehash，如果需要则 执行 rehash
>
> 2. **每次扩容时**，如果 哈希表内的元素元素数量>buckets数量， 必须rehash，因为 一定 会发生冲突
>
> 3. **如果 reserve时，元素个数 n <= buckets数量**，则去判断必要性
>
>    ```C++
>    // 负载率 未达到 最大负载率-25% ，并且 缩小规模 > 25% 时 认为 有必要rehash
>    if ((float)size_ / (float)n < max_load_factor() - 0.25f &&
>            (float)n < (float)bucket_size_ * 0.75)  // worth rehash
>        {
>          replace_bucket(n);
>        }
>    // 如果当前负载率 很高，没有必要rehash缩减容量，反而导致冲突加剧
>    // 如果 缩小的 规模小，即如果从 100 -> 95，实际上性能改善不大，而拷贝开销比较大
>    ```

## rehash() 的 具体过程

```C++
template <class T, class Hash, class KeyEqual>
void hashtable<T, Hash, KeyEqual>::
replace_bucket(size_type bucket_count)
{
  bucket_type bucket(bucket_count);
  if (size_ != 0)
  {
    for (size_type i = 0; i < bucket_size_; ++i)
    {
      for (auto first = buckets_[i]; first; first = first->next)
      {
        auto tmp = create_node(first->value);
        const auto n = hash(value_traits::get_key(first->value), bucket_count);
        auto f = bucket[n];
        bool is_inserted = false;
        for (auto cur = f; cur; cur = cur->next)
        {
          if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(first->value)))
          {
            tmp->next = cur->next;
            cur->next = tmp;
            is_inserted = true;
            break;
          }
        }
        if (!is_inserted)
        {
          tmp->next = f;
          bucket[n] = tmp;
        }
      }
    }
  }
  buckets_.swap(bucket);
  bucket_size_ = buckets_.size();
  // 不需要去释放 bucket，因为是局部变量，出作用域自动释放
  // 所有容器只需要管好自己的空间分配，不必要去关心底层容器的空间
}
```

1. 先 分配一个 扩容后的 new_buckets
2. 对 old_buckets中的 元素 进行 再一次 的 哈希映射
3. 将 hashtables 的buckets与 new_buckets交换： 
   - 不需要去释放原来的buckets内存
   - 因为swap调用的是vector的swap，他会进行一个move移动语义的 = 或构造
   - 不需要去关心 vector 底层的内存操作



------

## 哈希函数 hash_

hash_是外部的hash函数，主要是对 key 做一些变换

```C++
// 对于大部分类型，hash function 什么都不做
template <class Key>
struct hash {};

// 针对指针的偏特化版本
template <class T>
struct hash<T*>
{
  size_t operator()(T* p) const noexcept
  { return reinterpret_cast<size_t>(p); }
};

// 对于整型类型，只是返回原值
#define MYSTL_TRIVIAL_HASH_FCN(Type)         
template <> struct hash<Type>                
{                                            
  size_t operator()(Type val) const noexcept 
  { return static_cast<size_t>(val); }       
};

// 对于浮点数，逐位哈希
inline size_t bitwise_hash(const unsigned char* first, size_t count)
{
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
  const size_t fnv_offset = 14695981039346656037ull;
  const size_t fnv_prime = 1099511628211ull;
#else
  const size_t fnv_offset = 2166136261u;
  const size_t fnv_prime = 16777619u;
#endif
  size_t result = fnv_offset;
  for (size_t i = 0; i < count; ++i)
  {
    result ^= (size_t)first[i];
    result *= fnv_prime;
  }
  return result;
}

template <>
struct hash<float>
{
  size_t operator()(const float& val)
  { 
    return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
  }
};

template <>
struct hash<double>
{
  size_t operator()(const double& val)
  {
    return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
  }
};

template <>
struct hash<long double>
{
  size_t operator()(const long double& val)
  {
    return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
  }
};

} // namespace mystl
#endif // !MYTINYSTL_FUNCTIONAL_H_


```

hash_table 的 hash：

```C++
hash(const key_type& key, size_type n) const
{
  return hash_(key) % n;
}
```


# unordered_map 与 unordered_set

------

## 简介

	1. 底层都由HashTable实现，区别： value 类型 不同
	1. 这里实现了 unordered_multi_set类型：因为哈希表 显示时，对 重复的key，与唯一的key都做了处理
	1. 这里的实现有点类似 适配器，将 hashtable的接口进行封装处理

## unordered_set

```C++
// 模板类 unordered_set，键值不允许重复
// 参数一代表键值类型，参数二代表哈希函数，缺省使用 mystl::hash，
// 参数三代表键值比较方式，缺省使用 mystl::equal_to
template <class Key, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
class unordered_set
{
private:
  // 使用 hashtable 作为底层机制
  typedef hashtable<Key, Hash, KeyEqual> base_type;
  base_type ht_;
    
// 只是调用了 hashtable 的接口
// 迭代器相关

  iterator       begin()        noexcept
  { return ht_.begin(); }
  const_iterator begin()  const noexcept
  { return ht_.begin(); }
  iterator       end()          noexcept
  { return ht_.end(); }
  const_iterator end()    const noexcept
  { return ht_.end(); }

  const_iterator cbegin() const noexcept
  { return ht_.cbegin(); }
  const_iterator cend()   const noexcept
  { return ht_.cend(); }

  // 容量相关

  bool      empty()    const noexcept { return ht_.empty(); }
  size_type size()     const noexcept { return ht_.size(); }
  size_type max_size() const noexcept { return ht_.max_size(); }

  // 修改容器操作

  // empalce / empalce_hint

  template <class ...Args>
  pair<iterator, bool> emplace(Args&& ...args)
  { return ht_.emplace_unique(mystl::forward<Args>(args)...); }

  template <class ...Args>
  iterator emplace_hint(const_iterator hint, Args&& ...args)
  { return ht_.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...); }

  // insert

  pair<iterator, bool> insert(const value_type& value)
  { return ht_.insert_unique(value); }
  pair<iterator, bool> insert(value_type&& value)
  { return ht_.emplace_unique(mystl::move(value)); }

  iterator insert(const_iterator hint, const value_type& value)
  { return ht_.insert_unique_use_hint(hint, value); }
  iterator insert(const_iterator hint, value_type&& value)
  { return ht_.emplace_unique_use_hint(hint, mystl::move(value)); }

  template <class InputIterator>
  void insert(InputIterator first, InputIterator last)
  { ht_.insert_unique(first, last); }

  // erase / clear

  void      erase(iterator it)
  { ht_.erase(it); }
  void      erase(iterator first, iterator last)
  { ht_.erase(first, last); }

  size_type erase(const key_type& key)
  { return ht_.erase_unique(key); }

  void      clear()
  { ht_.clear(); }

  void      swap(unordered_set& other) noexcept
  { ht_.swap(other.ht_); }

  // 查找相关

  size_type      count(const key_type& key) const 
  { return ht_.count(key); }

  iterator       find(const key_type& key) 
  { return ht_.find(key); }
  const_iterator find(const key_type& key)  const 
  { return ht_.find(key); }

  pair<iterator, iterator> equal_range(const key_type& key)
  { return ht_.equal_range_unique(key); }
  pair<const_iterator, const_iterator> equal_range(const key_type& key) const
  { return ht_.equal_range_unique(key); }

```

## unordered_mulitset

```C++
/ 模板类 unordered_multiset，键值允许重复
// 参数一代表键值类型，参数二代表哈希函数，缺省使用 mystl::hash，
// 参数三代表键值比较方式，缺省使用 mystl::equal_to
template <class Key, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
class unordered_multiset
{
private:
  // 使用 hashtable 作为底层机制
  typedef hashtable<Key, Hash, KeyEqual> base_type;
  base_type ht_;
```

## unordered_map相同： 只不过 模板参数 为 key+value

```C++
// 模板类 unordered_map，键值不允许重复
// 参数一代表键值类型，参数二代表实值类型，参数三代表哈希函数，缺省使用 mystl::hash
// 参数四代表键值比较方式，缺省使用 mystl::equal_to
template <class Key, class T, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
class unordered_map
{
private:
  // 使用 hashtable 作为底层机制
  typedef hashtable<mystl::pair<const Key, T>, Hash, KeyEqual> base_type;
  base_type ht_;
```


# basic_string: 字符串类型

------

## 前言

1. `<astring.h>`  定义了 四种basic_string类型，分别接收不同的类型作为元素

   ```C++
   using string    = mystl::basic_string<char>;
   using wstring   = mystl::basic_string<wchar_t>;
   using u16string = mystl::basic_string<char16_t>;
   using u32string = mystl::basic_string<char32_t>;
   ```

2. basic_string 类型 只接受 POD 类型做模板参数

3. basic_string 仿造 string类，实现了string的基本功能

4. 除此之外， 新增了一些函数，如 字符串分割 等操作

------

## char_traits：萃取 char 相关方法

萃取 字符串的 copy，move，compare，length()等方法

```C++
template <class CharType>
struct char_traits
{
  typedef CharType char_type;
  
  static size_t length(const char_type* str)
  {
    size_t len = 0;
    for (; *str != char_type(0); ++str)
      ++len;
    return len;
  }
  // 字符串比较
  static int compare(const char_type* s1, const char_type* s2, size_t n)
  {
    for (; n != 0; --n, ++s1, ++s2)
    {
      if (*s1 < *s2)
        return -1;
      if (*s2 < *s1)
        return 1;
    }
    return 0;
  }
  // 字符串复制，当dst和src部分重叠时，会出现错误导致覆盖
  // dst长度必须 > n；当遇到 \0 或者 n时停止
  static char_type* copy(char_type* dst, const char_type* src, size_t n)
  {
    MYSTL_DEBUG(src + n <= dst || dst + n <= src);
    char_type* r = dst;
    for (; n != 0; --n, ++dst, ++src)
      *dst = *src;
    return r;
  }
  // 字符串复制，当dst和src部分重叠时，能保证被覆盖前 重叠区的字节拷贝到目标区域中
  // 当不重叠时，等于strcopy
  static char_type* move(char_type* dst, const char_type* src, size_t n)
  {
    char_type* r = dst;
    // 如果 dst 在 src 前面，从前往后复制，能保证 dst 的有效性：不被src覆盖
    if (dst < src)
    {
      for (; n != 0; --n, ++dst, ++src)
        *dst = *src;
    }
    // 如果 dst 在 src 后面，从后往前复制，能保证 dst 的有效性：不被src覆盖
    else if (src < dst)
    {
      dst += n;
      src += n;
      for (; n != 0; --n)
        *--dst = *--src;
    }
    return r;
  }

  static char_type* fill(char_type* dst, char_type ch, size_t count)
  {
    char_type* r = dst;
    for (; count > 0; --count, ++dst)
      *dst = ch;
    return r;
  }
};
```

------

## String 类 

### 结构

1. 类似 Vector<char>, 许多方法与vector类似，底层数据结构也类似vector
2. 迭代器 为 char_type*类型，元素为char类型。因此construct与destroy为true_type,不需要去显示的执行

### 数据结构

- 指向起始位置的字符指针 iterator buffer
- 字符串的实际长度大小 size
- 字符串数组的实际容量 cap : 因为扩容与数组类似
- 定义字符串末尾的值npos(实际上为MAX_UINT): `static constexpr size_type npos = static_cast<size_type>(-1);`

### 构造、复制、移动与析构

1. 构造方式：很多种

   > - 由 `string(char ch, size_t n)`
   > - `string(string& s,size_t pos )`与string`(string& s,size_t pos，size_t count )`
   > - ...等等

2. 移动：右值引用，与vector相同

## 容量、访问相关操作

- begin(),end(),size(),length()等
- reserve(),shrink_to_fit()等
- 重载 [] ， data(),c_str()等返回 c类型字符串

## 添加、删除等操作

- insert(): STL标准的插入操作
- append(): 在容器末尾插入
- erase(): 删除 [first, last) 的元素

## basic_string 相关操作

- compare( )：字符串比较

  ```C++
  // compare
    int compare(const basic_string& other) const;
    int compare(size_type pos1, size_type count1, const basic_string& other) const;
    int compare(size_type pos1, size_type count1, const basic_string& other,
                size_type pos2, size_type count2 = npos) const;
    int compare(const_pointer s) const;
    int compare(size_type pos1, size_type count1, const_pointer s) const;
    int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const;
  ```

- substr()：字符串截取

  ```C++
  // substr
    basic_string substr(size_type index, size_type count = npos)
    {
      count = mystl::min(count, size_ - index);
      return basic_string(buffer_ + index, buffer_ + index + count);
    }
  ```

- replace()：字符串替换

  ```C++
  // replace
  basic_string& replace(size_type pos, size_type count, const basic_string& str)
  {
  THROW_OUT_OF_RANGE_IF(pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
  return replace_cstr(buffer_ + pos, count, str.buffer_, str.size_);
  }
  // replace的真正底层接口
  basic_string& replace_cstr(const_iterator first, size_type count1, const_pointer str, size_type count2);
  basic_string& replace_fill(const_iterator first, size_type count1, size_type count2, value_type ch);
  template <class Iter>
  basic_string& replace_copy(const_iterator first, const_iterator last, Iter first2, Iter last2);
  ```

- find()： 字符串查找相关

  ```C++
    // find
    // 从下标 pos 开始查找字符串 str，若找到返回起始位置的下标，否则返回 npos
    size_type find(value_type ch, size_type pos = 0)                             const noexcept;
    size_type find(const_pointer str, size_type pos = 0)                         const noexcept;
    size_type find(const_pointer str, size_type pos, size_type count)            const noexcept;
    size_type find(const basic_string& str, size_type pos = 0)                   const noexcept;
  
    // rfind
    // 从下标 pos 开始反向查找字符串 str，与 find 类似 
    size_type rfind(value_type ch, size_type pos = npos)                         const noexcept;
    size_type rfind(const_pointer str, size_type pos = npos)                     const noexcept;
    size_type rfind(const_pointer str, size_type pos, size_type count)           const noexcept;
    size_type rfind(const basic_string& str, size_type pos = npos)               const noexcept;
  
    // find_first_of
    // 从下标 pos 开始查找字符串 s 其中的一个字符出现的第一个位置
    size_type find_first_of(value_type ch, size_type pos = 0)                    const noexcept;
    size_type find_first_of(const_pointer s, size_type pos = 0)                  const noexcept;
    size_type find_first_of(const_pointer s, size_type pos, size_type count)     const noexcept;
    size_type find_first_of(const basic_string& str, size_type pos = 0)          const noexcept;
  
    // find_first_not_of
    // 从下标 pos 开始查找与字符串 s 其中一个字符不相等的第一个位置
    size_type find_first_not_of(value_type ch, size_type pos = 0)                const noexcept;
    size_type find_first_not_of(const_pointer s, size_type pos = 0)              const noexcept;
    size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const noexcept;
    size_type find_first_not_of(const basic_string& str, size_type pos = 0)      const noexcept;
  
    // find_last_of
    // 从下标 pos 开始查找与字符串 s 其中一个字符相等的最后一个位置
    size_type find_last_of(value_type ch, size_type pos = 0)                     const noexcept;
    size_type find_last_of(const_pointer s, size_type pos = 0)                   const noexcept;
    size_type find_last_of(const_pointer s, size_type pos, size_type count)      const noexcept;
    size_type find_last_of(const basic_string& str, size_type pos = 0)           const noexcept;
  
    // find_last_not_of
    // 从下标 pos 开始查找与字符串 s 的字符中不相等的最后一个位置
    size_type find_last_not_of(value_type ch, size_type pos = 0)                 const noexcept;
    size_type find_last_not_of(const_pointer s, size_type pos = 0)               const noexcept;
    size_type find_last_not_of(const_pointer s, size_type pos, size_type count)  const noexcept;
    size_type find_last_not_of(const basic_string& str, size_type pos = 0)       const noexcept;
  
    // count
    size_type count(value_type ch, size_type pos = 0) const noexcept;
  ```

  > find 字符串 可以用 KMP算法优化

- 重载运算符 += ，+， >> , <<

  > 1. 区别于其他容器的+，string类要做的操作是 字符串拼接，调用append即可,返回值为string类型
  >
  >    ```C++
  >      // 重载 operator+= 
  >      basic_string& operator+=(const basic_string& str)
  >      { return append(str); }
  >      basic_string& operator+=(value_type ch)
  >      { return append(1, ch); }
  >      basic_string& operator+=(const_pointer str)
  >      { return append(str, str + char_traits::length(str)); }
  >    ```
  >
  > 2. operator <<,>>:  用于重载输入输出，因为 希望 输出的是 字符串，而不是指针地址
  >
  >    ```C++
  >     // 重载 operator >> / operatror <<
  >     // 必须定义为友元函数,没有隐含的 this指针
  >      friend std::istream& operator >> (std::istream& is, basic_string& str)
  >      {
  >        value_type* buf = new value_type[4096];
  >        // 输入流 流入 字符缓冲区
  >        is >> buf;
  >        // 构造字符串
  >        basic_string tmp(buf);
  >        // 移动语义转发至 str中
  >        str = std::move(tmp);
  >        delete[]buf;
  >        return is;
  >      }
  >    
  >      friend std::ostream& operator << (std::ostream& os, const basic_string& str)
  >      {
  >        // 将字符 一个个 流入 输出流
  >        for (size_type i = 0; i < str.size_; ++i)
  >          os << *(str.buffer_ + i);
  >        return os;
  >      }
  >    ```
  >
  >    > **Tips： 这里复习 一下 重载 运算符 相关**
  >    >
  >    > 运算符重载函数的限制：非静态成员或者全局函数
  >    >
  >    > |              | 特性                                                         | 使用场景                                                     | 必须使用     |
  >    > | :----------- | :----------------------------------------------------------- | ------------------------------------------------------------ | ------------ |
  >    > | **类内重载** | 隐含一个this指针<br />只允许右参数的隐式转化<br />相当于左参数以this指针代替 | 左参数必须为this指针时                                       | [],(),->.*,= |
  >    > | **友元重载** | 不含有this指针<br />需要指明接收参数的数量<br />参数都可以进行隐式类型转换<br />必须有一个参数与类有关 | 指定操作符的个数<br />左参数不是this指针时<br />左参数需要进行隐式类型转换时 | >>,<<        |
  >    >
  >    > 

   ## 一些help functions

help functions 是 大部分对外接口的底部实现

```C++
private:
  // helper functions

  // init / destroy 
  void          try_init() noexcept;

  void          fill_init(size_type n, value_type ch);

  template <class Iter>
  void          copy_init(Iter first, Iter last, mystl::input_iterator_tag);
  template <class Iter>
  void          copy_init(Iter first, Iter last, mystl::forward_iterator_tag);

  void          init_from(const_pointer src, size_type pos, size_type n);

  void          destroy_buffer();

  // get raw pointer
  const_pointer to_raw_pointer() const;

  // shrink_to_fit
  void          reinsert(size_type size);

  // append
  template <class Iter>
  basic_string& append_range(Iter first, Iter last);

  // compare
  int compare_cstr(const_pointer s1, size_type n1, const_pointer s2, size_type n2) const;

  // replace
  basic_string& replace_cstr(const_iterator first, size_type count1, const_pointer str, size_type count2);
  basic_string& replace_fill(const_iterator first, size_type count1, size_type count2, value_type ch);
  template <class Iter>
  basic_string& replace_copy(const_iterator first, const_iterator last, Iter first2, Iter last2);

  // reallocate
  void          reallocate(size_type need);
  iterator      reallocate_and_fill(iterator pos, size_type n, value_type ch);
  iterator      reallocate_and_copy(iterator pos, const_iterator first, const_iterator last);
```



## 可以优化的地方

	1. KMP算法匹配
	1. Python中 counter，切片，分割字符串
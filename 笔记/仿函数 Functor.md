# 仿函数 Functor

------

## 前言

1. 仿函数： 即函数对象，是一个对象通过重载  （）得到。
2. 仿函数与普通函数指针的区别
   - 仿函数可以记录函数的状态，因为其内部可以有成员变量
   - 仿函数能满足 STL 对 抽象性的要求
   - 仿函数能与 STL 其他组件搭配，如适配器
3. 仿函数 应该具备 被 Adapter 修改的能力

------

## Functor 配接的关键

**为了实现 抽象性 与 配接性，仿函数 必须 定义 自己的型别**

- 即 参数型别，返回值型别 

  ```C++
  // 定义一元函数的参数型别和返回值型别
  template <class Arg, class Result>
  struct unarg_function
  {
    typedef Arg       argument_type;
    typedef Result    result_type;
  };
  
  // 定义二元函数的参数型别的返回值型别
  template <class Arg1, class Arg2, class Result>
  struct binary_function
  {
    typedef Arg1      first_argument_type;
    typedef Arg2      second_argument_type;
    typedef Result    result_type;
  };
  // 定义的仿函数 继承 上述类 来实现仿函数的定义
  ```

- 普通指针无法配接： 适配器无法 取出 对应的型别，来进行重载

------

## functional介绍

**定义于 `functional.h`的仿函数**

### 算数型仿函数

**包括 +,-,*,/,%,-等多种运算**

```C++
// 函数对象：加法
template <class T>
struct plus :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x + y; }
};

// 函数对象：减法
template <class T>
struct minus :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x - y; }
};

// 函数对象：乘法
template <class T>
struct multiplies :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x * y; }
};

// 函数对象：除法
template <class T>
struct divides :public binary_function<T, T, T>
{
  T operator()(const T& x, const T& y) const { return x / y; }
};
```

除此之外，还有一个重要的仿函数 ：  

证同元素： 用于保证，进行运算后得到的元素等于 原来的元素

```C++
// 加法的证同元素： +0 不改变元素值
template <class T>
T identity_element(plus<T>) { return T(0); }

// 乘法的证同元素: *1不改变元素值
template <class T> 
T identity_element(multiplies<T>) { return T(1); }

```

### 关系型仿函数

包括 >,<,==,等比较运算

```C++
// 函数对象：等于
template <class T>
struct equal_to :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x == y; }
};

// 函数对象：不等于
template <class T>
struct not_equal_to :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x != y; }
};

// 函数对象：大于
template <class T>
struct greater :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x > y; }
};

// 函数对象：小于
template <class T>
struct less :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x < y; }
};

```

## 逻辑型仿函数

```C++
// 函数对象：逻辑与
template <class T>
struct logical_and :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x && y; }
};

// 函数对象：逻辑或
template <class T>
struct logical_or :public binary_function<T, T, bool>
{
  bool operator()(const T& x, const T& y) const { return x || y; }
};

// 函数对象：逻辑非
template <class T>
struct logical_not :public unarg_function<T, bool>
{
  bool operator()(const T& x) const { return !x; }
};
```


#ifndef MYmystl_MEMORY_H_
#define MYmystl_MEMORY_H_

// 这个头文件负责更高级的动态内存管理
// 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及一个模板类 auto_ptr

#include <cstddef>
#include <cstdlib>
#include <climits>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl
{

// 获取对象地址
template <class Tp>
constexpr Tp* address_of(Tp& value) noexcept
{
  return &value;
}

// 获取 / 释放 临时缓冲区

template <class T>
pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*)
{
  if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
    len = INT_MAX / sizeof(T);
  while (len > 0)
  {
    T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
    if (tmp)
      return pair<T*, ptrdiff_t>(tmp, len);
    len /= 2;  // 申请失败时减少 len 的大小
  }
  return pair<T*, ptrdiff_t>(nullptr, 0);
}

template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len)
{
  return get_buffer_helper(len, static_cast<T*>(0));
}

template <class T>
pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*)
{
  return get_buffer_helper(len, static_cast<T*>(0));
}

template <class T>
void release_temporary_buffer(T* ptr)
{
  free(ptr);
}

// --------------------------------------------------------------------------------------
// 类模板 : temporary_buffer
// 进行临时缓冲区的申请与释放
template <class ForwardIterator, class T>
class temporary_buffer
{
private:
  ptrdiff_t original_len;  // 缓冲区申请的大小
  ptrdiff_t len;           // 缓冲区实际的大小
  T*        buffer;        // 指向缓冲区的指针

public:
  // 构造、析构函数
  temporary_buffer(ForwardIterator first, ForwardIterator last);

  ~temporary_buffer()
  {
    mystl::destroy(buffer, buffer + len);
    free(buffer);
  }

public:

  ptrdiff_t size()           const noexcept { return len; }
  ptrdiff_t requested_size() const noexcept { return original_len; }
  T*        begin()                noexcept { return buffer; }
  T*        end()                  noexcept { return buffer + len; }

private:
  void allocate_buffer();
  void initialize_buffer(const T&, std::true_type) {}
  void initialize_buffer(const T& value, std::false_type)
  { mystl::uninitialized_fill_n(buffer, len, value); }

private:
  temporary_buffer(const temporary_buffer&);
  void operator=(const temporary_buffer&);
};

// 构造函数
template <class ForwardIterator, class T>
temporary_buffer<ForwardIterator, T>::
temporary_buffer(ForwardIterator first, ForwardIterator last)
{
  try
  {
    len = mystl::distance(first, last);
    allocate_buffer();
    if (len > 0)
    {
      initialize_buffer(*first, std::is_trivially_default_constructible<T>());
    }
  }
  catch (...)
  {
    free(buffer);
    buffer = nullptr;
    len = 0;
  }
}

// allocate_buffer 函数
template <class ForwardIterator, class T>
void temporary_buffer<ForwardIterator, T>::allocate_buffer()
{
  original_len = len;
  if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
    len = INT_MAX / sizeof(T);
  while (len > 0)
  {
    buffer = static_cast<T*>(malloc(len * sizeof(T)));
    if (buffer)
      break;
    len /= 2;  // 申请失败时减少申请空间大小
  }
}

// --------------------------------------------------------------------------------------
// 模板类: auto_ptr
// 一个具有严格对象所有权的小型智能指针
template <class T>
class auto_ptr
{
public:
  typedef T    elem_type;

private:
  T* m_ptr;  // 实际指针

public:
  // 构造、复制、析构函数
  explicit auto_ptr(T* p = nullptr) :m_ptr(p) {}
  auto_ptr(auto_ptr& rhs) :m_ptr(rhs.release()) {}
  template <class U>
  auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()) {}

  auto_ptr& operator=(auto_ptr& rhs)
  {
    if (this != &rhs)
    {
      delete m_ptr;
      m_ptr = rhs.release();
    }
    return *this;
  }
  template <class U>
  auto_ptr& operator=(auto_ptr<U>& rhs)
  {
    if (this->get() != rhs.get())
    {
      delete m_ptr;
      m_ptr = rhs.release();
    }
    return *this;
  }

  ~auto_ptr() { delete m_ptr; }

public:
  // 重载 operator* 和 operator->
  T& operator*()  const { return *m_ptr; }
  T* operator->() const { return m_ptr; }

  // 获得指针
  T* get() const { return m_ptr; }

  // 释放指针
  T* release()
  {
    T* tmp = m_ptr;
    m_ptr = nullptr;
    return tmp;
  }

  // 重置指针
  void reset(T* p = nullptr)
  {
    if (m_ptr != p)
    {
      delete m_ptr;
      m_ptr = p;
    }
  }
};
//-------------------------unique_ptr-----------------------//
// 智能指针 unique_ptr 只允许独自享有一个指针
// 取消赋值构造，只允许移动构造
// 赋值运算只接收右值
template<class _T>
class cow_ptr;


template<class T>
struct default_delete{
  void operator ()(T* ptr){ if(ptr) delete ptr; }
};
template<class T>
struct default_delete < T[] > {
  void operator ()(T* ptr){ if(ptr) delete[] ptr; }
};

template<class T, class D = default_delete<T>>
class unique_ptr{
public:
  typedef T element_type;
  typedef D deleter_type;
  typedef element_type *pointer;
public:
  explicit unique_ptr(T *data = nullptr) :data_(data){}
  unique_ptr(T *data, deleter_type del) :data_(data), deleter(del){}

  unique_ptr(unique_ptr&& up) :data_(nullptr){ 
    mystl::swap(data_, up.data_); 
  }
  unique_ptr& operator = (unique_ptr&& up){
    if (&up != this){
      clean();
      mystl::swap(*this, up);
    }
    return *this;
  }

  unique_ptr(const unique_ptr&) = delete;
  unique_ptr& operator = (const unique_ptr&) = delete;

  ~unique_ptr(){ clean(); }

  const pointer get()const{ return data_; }
  pointer get(){ return data_; }
  deleter_type& get_deleter(){ return deleter; }
  const deleter_type& get_deleter()const{ return deleter; }

  operator bool()const{ return get() != nullptr; }

  pointer release(){ 
    T *p = nullptr; 
    mystl::swap(p, data_);
    return p;
  }
  void reset(pointer p = pointer()){
    clean();
    data_ = p;
  }
  void swap(unique_ptr& up){ mystl::swap(data_, up.data_); }

  const element_type& operator *()const{ return *data_; }
  const pointer operator ->()const{ return data_; }
  element_type& operator *(){ return *data_; }
  pointer operator ->(){ return data_; }
private:
  inline void clean(){
    deleter(data_);
    data_ = nullptr;
  }
private:
  element_type *data_;
  deleter_type deleter;
};
template <class T, class D>
void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y){
  x.swap(y);
}
template <class T1, class D1, class T2, class D2>
bool operator == (const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs){
  return lhs.get() == rhs.get();
}
template <class T, class D>
bool operator == (const unique_ptr<T, D>& up, const T* p){
  return up.get() == p;
}
template <class T, class D>
bool operator == (const T* p, const unique_ptr<T, D>& up){
  return up.get() == p;
}
template <class T1, class D1, class T2, class D2>
bool operator != (const unique_ptr<T1, D1>& lhs, const unique_ptr<T2, D2>& rhs){
  return !(lhs == rhs);
}
template <class T, class D>
bool operator != (const unique_ptr<T, D>& up, const T* p){
  return up.get() != p;
}
template <class T, class D>
bool operator != (const T* p, const unique_ptr<T, D>& up){
  return up.get() != p;
}

template <class T, class... Args>
unique_ptr<T> make_unique(Args&&... args){
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
};
//-------------------------shared_ptr-----------------------//
// 智能指针 shared_ptr 允许共享指针
// 维护一个计数器
  template<class T,class D = default_delete<T>>
  struct shared_ptr{
  public: 
    typedef T element_type;
    typedef D deleter_type;
    typedef element_type *pointer;

  private:
    int* counter = nullptr;
    element_type* data_;
    deleter_type deleter;
    void clean()
    {
       if (counter == nullptr) return;
       --(*counter);
       if (*counter == 0) {
          delete counter;
          deleter(data_);
        };
    }
  public: 
    shared_ptr(){}
    explicit shared_ptr(T* data):data_(data){counter = new int(1);}
    shared_ptr(shared_ptr& sp)
    {
        data_ = sp.get();
        deleter = sp.deleter();
        counter = sp.get_counter();
        ++(*counter);
    }
    shared_ptr(shared_ptr&& sp)
    {
        data_ = move(sp.get());
        deleter = move(sp.deleter());
        counter = move(sp.get_counter());
        // 删除其计数器
        sp.set_counter();
        sp.set_data();
    }
    shared_ptr& operator=(shared_ptr& sp){
        clean();
        data_ = sp.get();
        deleter = sp.get_deleter();
        counter = sp.get_counter();
        ++(*counter);
        return *this;
    }
    shared_ptr& operator=(shared_ptr&& sp){
        clean();
        data_ = move(sp.get());
        deleter = move(sp.get_deleter());
        counter = move(sp.get_counter());
        // 删除其计数器
        sp.set_counter();
        sp.set_data();
        return *this;
    }
    ~shared_ptr(){ clean(); }
    operator bool()const{ return get() != nullptr; }
    pointer get() {return data_;}
    const pointer get() const { return data_;}
    deleter_type get_deleter() {return deleter;}
    const deleter_type get_deleter() const{return deleter;}
    const int* get_counter() const{return counter;}
    int* get_counter() {return counter;}
    void set_counter(){ counter = nullptr; }
    void set_data(){ data_ = nullptr; }
    int use_count() const { if (counter == nullptr ) return 0; return *counter;}

    void release(){
      element_type* p = nullptr;
      mystl::swap(p,data_);
      clean();
    }
    void reset(pointer p){
      mystl::swap(p,data_);
      clean();
      counter = new int(1);
    }
    void swap(shared_ptr& sp) {
      swap(data_,sp.get());
      swap(counter,sp.get_counter());
    }
    const element_type& operator*() const{
      return *data_;
    }
    const element_type& operator->() const{
      return data_;
    }
     element_type& operator*() {
      return *data_;
    }
     element_type& operator->() {
      return data_;
    }
  };
  template <class T, class D>
  void swap(shared_ptr<T, D>& x, shared_ptr<T, D>& y){
    x.swap(y);
  }
  template <class T1, class D1, class T2, class D2>
  bool operator == (const shared_ptr<T1, D1>& lhs, const shared_ptr<T2, D2>& rhs){
    return lhs.get() == rhs.get();
  }
  template <class T, class D>
  bool operator == (const shared_ptr<T, D>& up, const T* p){
    return up.get() == p;
  }
  template <class T, class D>
  bool operator == (const T* p, const shared_ptr<T, D>& up){
    return up.get() == p;
  }
  template <class T1, class D1, class T2, class D2>
  bool operator != (const shared_ptr<T1, D1>& lhs, const shared_ptr<T2, D2>& rhs){
    return !(lhs == rhs);
  }
  template <class T, class D>
  bool operator != (const shared_ptr<T, D>& up, const T* p){
    return up.get() != p;
  }
  template <class T, class D>
  bool operator != (const T* p, const shared_ptr<T, D>& up){
    return up.get() != p;
  }

  template <class T, class... Args>
  shared_ptr<T> make_shared(Args&&... args){
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
  };
} // namespace mystl
#endif // !MYmystl_MEMORY_H_


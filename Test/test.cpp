/*
 * @Author: 王海鹏 2713662238@qq.com
 * @Date: 2023-10-24 19:14:00
 * @LastEditors: 王海鹏 2713662238@qq.com
 * @LastEditTime: 2023-10-31 18:26:48
 * @FilePath: \MyTinySTL\Test\test.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// #ifdef _MSC_VER
// #define _SCL_SECURE_NO_WARNINGS
// #endif

#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
#endif // check memory leaks

#include "algorithm_performance_test.h"
#include "algorithm_test.h"
#include "vector_test.h"
#include "list_test.h"
#include "deque_test.h"
#include "queue_test.h"
#include "stack_test.h"
#include "map_test.h"
#include "set_test.h"
#include "unordered_map_test.h"
#include "unordered_set_test.h"
#include "string_test.h"
#include "bitmap_test.h"
#include "trietree_test.h"
#include "memory_test.h"
#include "KMP_test.h"
int main()
{
  using namespace mystl::test;

  std::cout.sync_with_stdio(false);

  
  // 进行algorithm的测试
  RUN_ALL_TESTS();
  
  // KMP_test::KMP_tes1();
  algorithm_performance_test::algorithm_performance_test();
  vector_test::vector_test();
  // list_test::list_test();
  deque_test::deque_test();
  queue_test::queue_test();
  queue_test::priority_test();
  stack_test::stack_test();
  map_test::map_test();
  map_test::multimap_test();
  set_test::set_test();
  set_test::multiset_test();
  unordered_map_test::unordered_map_test();
  unordered_map_test::unordered_multimap_test();
  unordered_set_test::unordered_set_test();
  unordered_set_test::unordered_multiset_test();
  string_test::string_test();
  bitmap_test::bitmap_test();
  trietree_test::trietree_test();
  // memory_test::memory_test();
  

#if defined(_MSC_VER) && defined(_DEBUG)
  _CrtDumpMemoryLeaks();
#endif // check memory leaks

}




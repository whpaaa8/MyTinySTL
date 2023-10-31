/*
 * @Author: 王海鹏 2713662238@qq.com
 * @Date: 2023-10-30 11:33:10
 * @LastEditors: 王海鹏 2713662238@qq.com
 * @LastEditTime: 2023-10-31 17:13:43
 * @FilePath: \MyTinySTL\Test\bitmap_test.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../MyTinySTL/bitmap.h"
#include "test.h"
#include <bitset>
#include <random>
namespace mystl{
namespace test{
namespace bitmap_test{
void bitmap_test(){
    std::cout << "[===============================================================]\n";
    std::cout << "[----------------- Run container test : bitmap -----------------]\n";
    std::cout << "[-------------------------- API test ---------------------------]\n";
    bitmap<80> bm;
    int pos = 50;
    
    bm.set(pos);
    int min = 1,max = 79;
    std::random_device seed;//硬件生成随机数种子
	  std::ranlux48 engine(seed());//利用种子生成随机数引擎
    std::uniform_int_distribution<> distrib(min, max);//设置随机数范围，并为均匀分布
    for (int i = 0; i < 40; ++i){
      int random = distrib(engine);//随机数
      std::cout << "bitmap insert " << random << std::endl; 
      bm.set(random);
    }
    std::cout << "bitmap:";
    std::cout << bm.to_string() << std::endl;
    FUN_VALUE(bm.size());
    FUN_VALUE(bm.count());
    FUN_VALUE(bm.any());
    FUN_VALUE(bm.test(50));
    FUN_VALUE(bm.none());
    FUN_VALUE(bm.all());
    FUN_VALUE(bm.to_string());
    
    bm.flip();
    std::cout << "bitmap filp:";
    std::cout << bm.to_string() << std::endl;
    PASSED;
#if PERFORMANCE_TEST_ON
  std::cout << "[--------------------- Performance Testing ---------------------]\n";
  std::cout << "|---------------------|-------------|-------------|-------------|\n";
  std::cout << "|      push_back      |";
#if LARGER_TEST_DATA_ON
  CCON_TEST_P1_bit(bitmap<100>, set, 50, SCALE_LL(LEN1), SCALE_LL(LEN2), SCALE_LL(LEN3));
#else
  CON_TEST_P1_bit(bitmap<100>, set, 50, SCALE_L(LEN1), SCALE_L(LEN2), SCALE_L(LEN3));
#endif
  std::cout << "\n";
  std::cout << "|---------------------|-------------|-------------|-------------|\n";
  PASSED;
#endif
    std::cout << "[----------------- End container test : bitmap -----------------]\n";
}
}
}
}

/*
 * @Author: 王海鹏 2713662238@qq.com
 * @Date: 2023-10-30 23:06:02
 * @LastEditors: 王海鹏 2713662238@qq.com
 * @LastEditTime: 2023-10-31 17:55:58
 * @FilePath: \MyTinySTL\Test\memory_test.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MYTINYSTL_MEMORY_TEST_H_
#define MYTINYSTL_MEMORY_TEST_H_

#include <memory>
#include "../MyTinySTL/memory.h"
#include "test.h"
namespace mystl{
namespace test{
namespace memory_test{
void memory_test(){
    
    std::cout << "[===============================================================]\n";
    std::cout << "[--------------- Run container test : unique_ptr ---------------]\n";
    std::cout << "[-------------------------- API test ---------------------------]\n";
    int* a = new int(1);
    unique_ptr<int> p1(a);
    std::unique_ptr<int> p2(a);
    // 判断取值
    EXPECT_PTR_EQ(p1.get(), p2.get());
    EXPECT_PTR_EQ(p1, p2);
    FUN_VALUE(*p1);
    std::cout << "[--------------- End container test : unique_ptr ---------------]\n\n";

    std::cout << "[===============================================================]\n";
    std::cout << "[--------------- Run container test : shared_ptr ---------------]\n";
    std::cout << "[-------------------------- API test ---------------------------]\n";
    shared_ptr<int> sp1(a),sp11;
    std::shared_ptr<int> sp2(a),sp22;
    EXPECT_PTR_EQ(sp1.get(), sp2.get());
    EXPECT_PTR_EQ(sp1, sp2);

    int a1 = sp1.use_count(),a2 = sp2.use_count();

    // EXPECT_EQ(a1, a2);

    sp11 = sp1;
    sp22 = sp2;

    EXPECT_PTR_EQ(sp1.get(), sp2.get());
    EXPECT_PTR_EQ(sp1, sp2);

    a1 = sp1.use_count(),a2 = sp2.use_count();

    // EXPECT_EQ(a1,a2);
    FUN_VALUE(*sp1);
    PASSED;
    std::cout << "[--------------- End container test : shared_ptr ---------------]\n\n";
}
}
}
}
#endif
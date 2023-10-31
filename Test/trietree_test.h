/*
 * @Author: 王海鹏 2713662238@qq.com
 * @Date: 2023-10-30 11:47:41
 * @LastEditors: 王海鹏 2713662238@qq.com
 * @LastEditTime: 2023-10-31 17:08:23
 * @FilePath: \MyTinySTL\Test\trietree_test.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MYTINYSTL_TRIE_TEST_H_
#define MYTINYSTL_TRIE_TEST_H_
#include "../MyTinySTL/trietree.h"
#include "test.h"
#include <map>
namespace mystl{
namespace test{
namespace trietree_test{

void trietree_test(){
    std::cout << "[--------------- Run container test : trie_tree ----------------]\n";
    std::cout << "[-------------------------- API test ---------------------------]\n";
    std::cout << "[===============================================================]\n";
    trie_tree t;
    std::string s[] = {"apple","pen","apply","app","pencil"};
    for (std::string ss: s) {
        std::cout <<  "trie insert: " << ss <<  
        (t.insert(ss) ?" success": " failed" )<< std::endl;
        }
    std::cout << "Words in Trie tree:\n" ;
    t.print_tree(std::cout);

    FUN_VALUE(t.empty());
    FUN_VALUE(t.size());
    FUN_VALUE(t.is_existed("apple"));
    FUN_VALUE(t.is_existed("pen"));
    FUN_VALUE(t.is_existed("banana"));

    std::vector<std::string> prefix_words;
    FUN_AFTER(prefix_words,prefix_words = t.get_word_by_prefix("pen"));
    FUN_AFTER(prefix_words,prefix_words = t.get_word_by_prefix("app"));

    PASSED;
    std::cout << "[---------------- End container test : trie_tree ---------------]\n";
}
}
}
}

#endif


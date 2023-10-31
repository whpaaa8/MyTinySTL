/*
 * @Author: 王海鹏 2713662238@qq.com
 * @Date: 2023-10-31 10:11:35
 * @LastEditors: 王海鹏 2713662238@qq.com
 * @LastEditTime: 2023-10-31 16:32:30
 * @FilePath: \MyTinySTL\Test\KMP_test.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// #ifdef __KMP_TEST__H__
// #define __KMP_TEST__H__
#include <algorithm>
#include "../MyTinySTL/algo.h"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <climits>
using namespace std;
namespace mystl{
namespace test{
namespace KMP_test{

void KMP_tes1(){
    std::string s1 = "aabaabaaaabaabaaaabaabaafaa";
    std::string s2 = "aabaaf";
    vector<char> v1,v2;
    for (char c:s1) v2.push_back(c);
    for (char c:s2) v1.push_back(c);
    // 比较效率
    typedef chrono::milliseconds ms;
    typedef chrono::steady_clock clock;
    typedef clock::time_point tp;
    cout << "std::search():\n";
    {
        tp begin = clock::now();
        std::string::iterator tmp;
        for (unsigned int i = 0; i < 100000000; ++i) tmp = std::search(s1.begin(),s1.end(),s2.begin(),s2.end());
        tp end = clock::now();
        std::string ans1 = std::string(tmp,s1.end());
        // cout << ans1 << endl;
        cout << "use time(ms): " << chrono::duration_cast<ms>(end-begin).count() << endl;
    }
    cout << "template_KMP():\n";
    {
        tp begin = clock::now();
        std::string::iterator tmp;
        for (unsigned int i = 0; i < 100000000; ++i)  tmp = mystl::search_KMP(s1.begin(),s1.end(),s2.begin(),s2.end());
        tp end = clock::now();
        std::string ans1 = std::string(tmp,s1.end());
        // cout << ans1 << endl;
        cout << "use time(ms): " << chrono::duration_cast<ms>(end-begin).count() << endl;
    }



    cout <<" -----------------vector-------------" << endl;
    cout << "std::search():\n";
    {
        tp begin = clock::now();
        for (unsigned int i = 0; i < 100000000; ++i)  std::search(v1.begin(),v1.end(),v2.begin(),v2.end());
        tp end = clock::now();
        cout << "use time(ms): " << chrono::duration_cast<ms>(end-begin).count() << endl;
    }
     cout << "template_KMP():\n";
    {
        tp begin = clock::now();
        for (unsigned int i = 0; i < 100000000; ++i)  mystl::search_KMP(v1.begin(),v1.end(),v2.begin(),v2.end());
        tp end = clock::now();
        cout << "use time(ms): " << chrono::duration_cast<ms>(end-begin).count() << endl;
    }
}
}
}
}
// #endif
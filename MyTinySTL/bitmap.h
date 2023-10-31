#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include "allocator.h"
#include "uninitialized.h"
// 可以使用 bitset<N> 实现
namespace mystl{
    // 用于记录状态
    template<size_t N>
    class bitmap{
    private: 
        enum Ealign{ Align8 = 8,Align16 = 16,Align32 = 32 };
        Ealign align = Align8;
        uint8_t* start_;
        uint8_t* finish_;
        const size_t size_;
        const size_t sizeOfUINT8_;
        typedef allocator<uint8_t>  dataAllocator;
    public:
        // 全部设为false
        bitmap();
        // 设置为val的二进制形式
        bitmap(unsigned long val);
        // // 设置为01字符串
        // bitmap(std::string & str);
        // 返回true得数量
        size_t count() const;
        // 返回bitmap的大小
        size_t size() const{ return size_; }
        // 相当于 vector的at，但是有越界检查
        bool test(size_t pos) const;
        // 若存在某一位是true，则返回true
        bool any() const;
		// 若所有位都是 false 则返回 true，否则返回 false。
		bool none() const;
		// 若所有位都是 true 则返回 true
		bool all() const;

        // 将整个map都设置为true
		bitmap& set();
        // 将某一位置设置为 true
		bitmap& set(size_t pos, bool val = true);
        // 将整个map设置为 false
		bitmap& reset();
        // 将某个位置设置为 false
		bitmap& reset(size_t pos);
        // 翻转每一位，即取反每一位
		bitmap& flip();
        // 取反某一位
		bitmap& flip(size_t pos);

        

        //std::string to_string() const;
        // 返回转化的字符串表达
		std::string to_string() const;
        unsigned long long to_ullong() const;

		template<size_t N1>
		friend std::ostream& operator <<(std::ostream& os, const bitmap<N1>& bm);
	private:
		size_t roundUp8(size_t bytes);
		//将i的第nth为置为newVal
		void setNthInInt8(uint8_t& i, size_t nth, bool newVal);
		//取出i的第nth位
		uint8_t getMask(uint8_t i, size_t nth)const{ return (i & (1 << nth)); }
		//将第n个位置转换为其在第几个uint8_t中
		size_t getNth(size_t n)const{ return (n / 8); }
		//将第n个位置转换为其在第N个uint8_t中的第几个bit上
		size_t getMth(size_t n)const{ return (n % align); }
		void allocateAndFillN(size_t n, uint8_t val);
        // 用于判断地址的合法性（有无越界）
		void THROW(size_t n)const;
    };
    template<size_t N>
    // n 个 int8, 
	void bitmap<N>::allocateAndFillN(size_t n, uint8_t val){
		start_ = dataAllocator::allocate(n);
		finish_ = uninitialized_fill_n(start_, n, val);
	}
	template<size_t N>
	void bitmap<N>::THROW(size_t n)const{
		if (!(n < size()))
			throw std::out_of_range("Out Of Range");
	}
	template<size_t N>
	void bitmap<N>::setNthInInt8(uint8_t& i, size_t nth, bool newVal){
		uint8_t temp = getMask(i, nth);
		if ((bool)temp == newVal){
			return;
		}
		else{
			if (temp){//nth位为1
				temp = ~temp;
				i = i & temp;
			}
			else{//nth位为0
				i = i | (1 << nth);
			}
		}
	}
	template<size_t N>
	size_t bitmap<N>::roundUp8(size_t bytes){
		return ((bytes + align - 1) & ~(align - 1));
	}
	template<size_t N>
	bitmap<N>& bitmap<N>::set(){
		uninitialized_fill_n(start_, sizeOfUINT8_, ~0);
		return *this;
	}
	template<size_t N>
	bitmap<N>& bitmap<N>::reset(){
		uninitialized_fill_n(start_, sizeOfUINT8_, 0);
		return *this;
	}
	template<size_t N>
	bool bitmap<N>::test(size_t pos) const{
		THROW(pos);
		const auto nth = getNth(pos);
		const auto mth = getMth(pos);
		uint8_t *ptr = start_ + nth;
		uint8_t temp = getMask(*ptr, mth);
		if (temp)
			return true;
		return false;
	}
	template<size_t N>
	bitmap<N>::bitmap() :size_(roundUp8(N)), sizeOfUINT8_(roundUp8(N) / 8){
		allocateAndFillN(sizeOfUINT8_, 0);
	}
	template<size_t N>
	bitmap<N>::bitmap(unsigned long val) :size_(roundUp8(N)), sizeOfUINT8_(roundUp8(N) / 8){
		unsigned long res = 0;
    	for (int i = 0; i < 32; i++) {
			// 获取 n 的最低位加到 res 上
			res = (res << 1) + (val & 1);
			// n 右移一位
			val >>= 1;
    	} 
		allocateAndFillN(sizeOfUINT8_, 0);
		uninitialized_copy(reinterpret_cast<unsigned long*>(&res),reinterpret_cast<unsigned long*>(&res)+4,start_);
		// 32位共有
		THROW(32);
	}
	template<size_t N>
	bitmap<N>& bitmap<N>::set(size_t pos, bool val){
		THROW(pos);
		const auto nth = getNth(pos);
		const auto mth = getMth(pos);
		uint8_t *ptr = start_ + nth;//get the nth uint8_t
		setNthInInt8(*ptr, mth, val);
		return *this;
	}
	template<size_t N>
	bitmap<N>& bitmap<N>::reset(size_t pos){
		set(pos, false);
		return *this;
	}
	template<size_t N>
	bitmap<N>& bitmap<N>::flip(){
		uint8_t *ptr = start_;
		for (; ptr != finish_; ++ptr){
			uint8_t n = *ptr;
			*ptr = ~n;
		}
		return *this;
	}
	template<size_t N>
	bitmap<N>& bitmap<N>::flip(size_t pos){
		THROW(pos);
		const auto nth = getNth(pos);
		const auto mth = getMth(pos);
		uint8_t *ptr = start_ + nth;
		uint8_t temp = getMask(*ptr, mth);
		if (temp)
			setNthInInt8(*ptr, mth, false);
		else
			setNthInInt8(*ptr, mth, true);
		return *this;
	}
	template<size_t N>
	size_t bitmap<N>::count() const{
		uint8_t *ptr = start_;
		size_t sum = 0;
		for (; ptr != finish_; ++ptr){
			for (int i = 0; i != 8; ++i){
				uint8_t t = getMask(*ptr, i);
				if (t){
					++sum;
				}
			}
		}
		return sum;
	}
	template<size_t N>
	bool bitmap<N>::any() const{
		uint8_t *ptr = start_;
		for (; ptr != finish_; ++ptr){
			uint8_t n = *ptr;
			if (n != 0)
				return true;
		}
		return false;
	}
	template<size_t N>
	bool bitmap<N>::all() const{
		uint8_t *ptr = start_;
		for (; ptr != finish_; ++ptr){
			uint8_t n = *ptr;
			if (n != (uint8_t)~0)
				return false;
		}
		return true;
	}
	template<size_t N>
	bool bitmap<N>::none() const{
		return !any();
	}
	template<size_t N>
	std::string bitmap<N>::to_string() const{
		std::string str;
		uint8_t *ptr = start_;
		for (; ptr != finish_; ++ptr){
			uint8_t n = *ptr;
			for (int i = 0; i != 8; ++i){
				uint8_t t = getMask(n, i);
				if (t) str += "1";
				else str += "0";
			}
		}
		return str;
	}

	template<size_t N1>
	std::ostream& operator <<(std::ostream& os, const bitmap<N1>& bm){
		os << bm.to_string();
		return os;
	}
}

#endif
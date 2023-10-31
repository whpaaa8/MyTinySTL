#ifndef __ALLOC_H_
#define __ALLOC_H_


#include <cstdlib>
namespace mystl{
    /*
        第二级空间配置器
        内部使用
        不需要模板类：因为只分配空间大小，不需要知道类型
    */
   class alloc{
   private: 
        enum EAlign{ ALIGN = 8};//小型区块的上调边界
		enum EMaxBytes{ MAXBYTES = 128};//小型区块的上限，超过的区块由malloc分配
		enum ENFreeLists{ NFREELISTS = (EMaxBytes::MAXBYTES / EAlign::ALIGN)};//free-lists的个数
		enum ENObjs{ NOBJS = 20};//每次增加的节点数
    private:
        // 数据块节点
        union obj{
            union obj *next;
            char cilent[1];
        };
        // 空闲链表
        static obj* free_list[ENFreeLists::NFREELISTS];
    private:
        static char* start_free;// 内存池起始地址
        static char* end_free;// 内存池结束地址
        static size_t heap_size;// 内存池大小
    private:
        // 将 bytes 上调至 8 倍数
        static size_t ROUND_UP(size_t bytes)
        {
            // 位运算  保证 后三位必定为0
            return ((bytes + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
        }
        //根据区块大小，决定使用第n号free-list，n从0开始计算
		static size_t FREELIST_INDEX(size_t bytes){
			return (((bytes)+EAlign::ALIGN - 1) / EAlign::ALIGN - 1);
		}
        // 从 内存池 填充 free_list
        static void *refill(size_t n);
        // 配置 内存池，内存池向操作系统申请内存
        static char *chunk_alloc(size_t size,size_t& nobjs) ;
    public:
        static void* allocate(size_t bytes); //分配内存
        static void  deallocate(void* ptr,size_t bytes);//释放内存
        static void* reallocate(void* ptr,size_t old_sz,size_t new_sz);//重新分配内存
   };
}
namespace mystl{
    // 初始化成员变量
    char *alloc::start_free = 0;
	char *alloc::end_free = 0;
	size_t alloc::heap_size = 0;

    alloc::obj* alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    // 分配内存
    void* alloc::allocate(size_t bytes)
    {
        if (bytes > EMaxBytes::MAXBYTES){
			return malloc(bytes);
		}
        size_t index = FREELIST_INDEX(bytes);
        obj* myfree = alloc::free_list[index];
        //  如果没空间了 就 填充 链表
        if (!myfree) return refill(ROUND_UP(bytes));
        free_list[index]->next = myfree->next;
        return myfree;
    }
    // 释放内存
    void alloc::deallocate(void* ptr,size_t bytes)
    {
        if (bytes > EMaxBytes::MAXBYTES){
            free(ptr);
        }
        else {
            size_t index = FREELIST_INDEX(bytes);
            obj* node = static_cast<obj*>(ptr);
            node->next = free_list[index];
            free_list[index] = node;
        }
    }
    // 重新分配内存
    void* alloc::reallocate(void* ptr,size_t old_sz,size_t new_sz)
    {
        deallocate(ptr,old_sz);
        ptr = allocate(new_sz);
        return ptr;
    }
    // 返回一个 大小为n的节点(8的倍数)
    // 从内存池中获取内存 填充节点
    void* alloc::refill(size_t n){  
        // 一次 申请 20个节点
        size_t nobjs = ENObjs::NOBJS;
        // 获取这么多连续的内存
        char* chunk  = chunk_alloc(n,nobjs);
        // 将内存分配为 n 大小的数据块,串到链表中
        if (nobjs == 1) return chunk;
        obj* my_free_list = free_list[FREELIST_INDEX(n)];
        obj* result = (obj*)(chunk);
        obj* cur,*next = 0;
        my_free_list = next = (obj*)(chunk+n);
        while(nobjs--)
        {
            cur = next;
            next = (obj*)((char*)cur + n);
            if (nobjs == 0) { 
                cur->next = 0;
                break;
            }
            cur->next = next;
        }
        return result;
    }

    // 返回内存的起始地址
    // 从操作系统申请内存
    char* alloc::chunk_alloc(size_t size,size_t& nobjs)
    {
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;
        char* result;
        // 如果 内存足够
        if (bytes_left >= total_bytes){
            result = start_free;
            start_free = start_free + total_bytes;
            return result;
        }
        // 空间不足分配20个节点，但可以分配一个以上节点
        else if (bytes_left >= size) { 
            nobjs = bytes_left / size;
            total_bytes = nobjs * size;
            result  = start_free;
            start_free = start_free + total_bytes;
            return result;
        }else { //  一个都无法提供，则去像 操作系统 申请内存
            // 需要获取的内存
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            // 首先 将 剩余 内存 分配出去
			if (bytes_left > 0){
				obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj *)start_free)->next = *my_free_list;
				*my_free_list = (obj *)start_free;
			}
            // 获取 内存
			start_free = (char *)malloc(bytes_to_get);
            // 获取失败
            if (!start_free)
            {
                // 从 free_list中找一个 空间点，回收到内存池，再次分配
                obj **my_free_list = 0, *p = 0;
				for (int i = 0; i <= EMaxBytes::MAXBYTES; i += EAlign::ALIGN){
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0){
						*my_free_list = p->next;
						start_free = (char *)p;
						end_free = start_free + i;
						return chunk_alloc(size, nobjs);
					}
				}
				end_free = 0;
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size,nobjs);
        }
    }

}


#endif
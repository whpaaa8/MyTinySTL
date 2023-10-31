#ifndef _TRIE_TREE_H_
#define _TRIE_TREE_H_

#include <string.h>
#include <vector>
#include "util.h"
#include <map>
#include <memory>
#include <iostream>
#include "memory.h"
    namespace mystl{
	class trie_tree{
	private:
		struct trie_node{
			char data;
			bool is_a_word;
			std::map<char, std::unique_ptr<trie_node>> map_childs;
			trie_node() :data('\0'), is_a_word(false){}
			trie_node(char ch, bool is) :data(ch), is_a_word(is){}
		};
		typedef std::unique_ptr<trie_node> node_ptr;
	public:
		typedef std::string value_type;
		typedef size_t size_type;
	private:
		trie_node *root_;
		size_type size_;
	public:
		trie_tree();
		~trie_tree();
		trie_tree(const trie_tree&) = delete;
		trie_tree& operator = (const trie_tree&) = delete;

		bool empty()const;
		size_type size()const;

		std::vector<std::string> get_word_by_prefix(const std::string& prefix)const;
		void print_tree(std::ostream& os = std::cout)const;
		bool insert(const std::string& word);
		bool is_existed(const std::string& word)const;
	private:
		node_ptr make_node(char ch, bool is_a_word);
		inline trie_node* get_root()const{ return root_; }
		void _get_word_by_prefix(const std::string& prefix, const node_ptr& up, const std::string& real_prefix, std::vector<std::string>& words)const;
		void __get_word_by_prefix(const node_ptr& up, std::string& word, const std::string& prefix, std::vector<std::string>& words)const;
		void _print_tree(std::ostream& os, const node_ptr& up, std::string word)const;
		bool _insert(const std::string& word, const node_ptr& up);
		bool _is_existed(const std::string& word, const node_ptr& up)const;
	};// end of trie_tree

    trie_tree::trie_tree() :root_(new trie_node), size_(0){}
	trie_tree::~trie_tree(){
		if (root_){
			root_->map_childs.clear();
			delete root_;
		}
	}
	bool trie_tree::empty()const{
		return size() == 0;
	}
	trie_tree::size_type trie_tree::size()const{
		return size_;
	}
	bool trie_tree::is_existed(const std::string& word)const{
		if (word.empty())
			return false;
		auto root = get_root();
		auto res = root->map_childs.find(word[0]);
		if (res == root->map_childs.end())//not found
			return false;
		else
			return _is_existed(word, res->second);
	}
	bool trie_tree::_is_existed(const std::string& word, const node_ptr& up)const{
		if (word.size() == 1)
			return up->is_a_word;
		char ch = word[1];
		auto res = up->map_childs.find(ch);
		if (res == up->map_childs.end())//not found
			return false;
		else
			return _is_existed(word.substr(1), res->second);
	}
	// C++11 引入 make unique
	template<typename T, typename... Args>
	std::unique_ptr<T> make_unique1(Args&&... args) {
  		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
	trie_tree::node_ptr trie_tree::make_node(char ch, bool is_a_word){
		return make_unique1<trie_tree::trie_node>(ch, is_a_word);
	}
	bool trie_tree::insert(const std::string& word){
		if (is_existed(word))
			return true;
		if (word.empty())
			return false;
		char ch = word[0];
		auto root = get_root();
		auto res = root->map_childs.find(ch);
		if (res != root->map_childs.end()){
			return _insert(word.substr(1), res->second);
		}else{
			auto is_a_word = (word.size() == 1 ? true : false);
			auto node = make_node(ch, is_a_word);
			root->map_childs[ch] = std::move(node);
			return _insert(word.substr(1), root->map_childs[ch]);
		}
	}
	bool trie_tree::_insert(const std::string& word, const node_ptr& up){
		if (word.empty()){
			++size_;
			up->is_a_word = true;
			return true;
		}
		char ch = word[0];
		auto res = up->map_childs.find(ch);
		if (res != up->map_childs.end()){
			return _insert(word.substr(1), res->second);
		}else{
			auto is_a_word = (word.size() == 1 ? true : false);
			auto node = make_node(ch, is_a_word);
			up->map_childs[ch] = std::move(node);
			return _insert(word.substr(1), up->map_childs[ch]);
		}
	}
	void trie_tree::print_tree(std::ostream& os)const{
		auto root = get_root();
		if (root == NULL)
			os << "the trie_tree is empty!" << std::endl;
		for (auto cit = root->map_childs.cbegin(); cit != root->map_childs.cend(); ++cit)
			_print_tree(os, cit->second, std::string());
	}
	void trie_tree::_print_tree(std::ostream& os, const node_ptr& up, std::string word)const{
		word += up->data;
		if (up->is_a_word)
			os << word << std::endl;
		for (auto cit = up->map_childs.cbegin(); cit != up->map_childs.cend(); ++cit){
			_print_tree(os, cit->second, word);
		}
	}
	

	std::vector<std::string> trie_tree::get_word_by_prefix(const std::string& prefix)const{
		std::vector<std::string> words;
		auto root = get_root();
		if (root == NULL || prefix.size() == 0)
			return words;
		char ch = prefix[0];
		auto res = root->map_childs.find(ch);
		if (res != root->map_childs.end())
			_get_word_by_prefix(prefix, res->second, prefix, words);
		return words;
	}
	void trie_tree::_get_word_by_prefix(const std::string& prefix, const node_ptr& up,
		const std::string& real_prefix, std::vector<std::string>& words)const{
		if (prefix.size() == 1){
			if (up->is_a_word)
				words.push_back(real_prefix);
			for (auto cit = up->map_childs.cbegin(); cit != up->map_childs.cend(); ++cit){
                std::string tmp;
				__get_word_by_prefix(cit->second, tmp, real_prefix, words);
			}
		}else{
			char ch = prefix[1];
			auto res = up->map_childs.find(ch);
			if (res != up->map_childs.end()){
				_get_word_by_prefix(prefix.substr(1), res->second, real_prefix, words);
			}
		}
	}
	void trie_tree::__get_word_by_prefix(const node_ptr& up, std::string& word, const std::string& prefix, std::vector<std::string>& words)const{
		word += up->data;
		if (up->is_a_word)
			words.push_back(prefix + word);
		for (auto cit = up->map_childs.cbegin(); cit != up->map_childs.cend(); ++cit){
            std::string tmp(word);
			__get_word_by_prefix(cit->second, tmp, prefix, words);
		}
	}
}
#endif
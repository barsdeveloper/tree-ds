#ifndef H1E827EB6_260E_460A_A7C3_98374041137B
#define H1E827EB6_260E_460A_A7C3_98374041137B

#include <TreeDS/node.hpp>
#include <string>
#include <cstddef> // nullptr_t

namespace ds {

template<typename T>
class temporary_node: public node<T> {

	template<typename, typename, typename >
	friend class tree;

private:
	size_t tree_size;

	using node<T>::node; // automatically inherit constructors

	temporary_node(T value) :
			node<T>(value), tree_size(1) {
	}

public:
	size_t get_size() const {
		return tree_size;
	}

	temporary_node operator ()(temporary_node&& left, std::nullptr_t right =
			nullptr) {
		this->tree_size += left.tree_size;
		this->set_left(std::make_unique<node<T>>(std::move(left)));
		return std::move(*this);
	}

	temporary_node operator ()(temporary_node&& left, temporary_node&& right) {
		this->tree_size += left.tree_size + right.tree_size;
		this->set_left(std::make_unique<node<T>>(std::move(left)));
		this->set_right(std::make_unique<node<T>>(std::move(right)));
		return std::move(*this);
	}

	temporary_node operator ()(std::nullptr_t left, temporary_node&& right) {
		this->tree_size += right.tree_size;
		this->set_right(std::make_unique<node<T>>(std::move(right)));
		return std::move(*this);
	}

};

} /* namespace ds */

#endif /* H1E827EB6_260E_460A_A7C3_98374041137B */

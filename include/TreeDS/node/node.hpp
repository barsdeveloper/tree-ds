#ifndef HF6C78AAD_6105_42F9_93D7_FBAE73CF69D1
#define HF6C78AAD_6105_42F9_93D7_FBAE73CF69D1

#include <utility> // std::pair

namespace ds {

template <typename> class tree_base;
template <typename, typename, typename> class tree;

template <typename T>
class node {

public:
	struct info {
		bool next_sibling;
		bool has_children;
	};

protected:
	T _value;
	node *_parent;
	node *_prev_sibling;
	node *_next_sibling;
	node *_first_child;
	node *_last_child;

	node(const node&) = delete;             // Copy constructor
	node(node&&);                           // Move constructor
	node(const T &argument);
	node(T &&argument);
	template <typename ...Args> node(Args&& ...args);
	node(tree_base<T> &&);
	node& operator =(const node&) = delete; // Copy assignment operator
	node& operator =(node&&);               // Move assignment operator

	void nullify();
	void unlink();
	void insert(node&);
	void append_child(node&);
	void prepend_child(node&);
	void append_sibling(node&);
	// void prepend_sibling(node&); // not needed

public:
	node();									// Default constructor
	~node();
	//   ---   Getters   ---   */
	const node* parent() const;
	const node* prev_sibling() const;
	const node* next_sibling() const;
	const node* first_child() const;
	const node* last_child() const;

	//   ---   Serialization   ---   */
	template <typename Archive> void save(Archive&) const;
	template <typename Archive> info load(Archive&);

	//   ---   Tree construction   ---   */
	node& operator ()(const node&);
	node& operator ,(const node&);

};

} /* namespace ds */

#include "node.tpp"

#endif /* HF6C78AAD_6105_42F9_93D7_FBAE73CF69D1 */

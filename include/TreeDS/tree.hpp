#ifndef H3702CC99_74F1_4C8D_9B1A_954EB6E53330
#define H3702CC99_74F1_4C8D_9B1A_954EB6E53330

#include <memory>   // std::allocator, std::allocator_traits
#include <iterator> // std::reverse_iterator
#include <type_traits>

#include "iterator/traversal_algorithm/pre_order.hpp"
#include "iterator/tree_iterator/tree_iterator.hpp"
#include "tree_base.hpp"

namespace ds {

template <typename> class node;

template <typename T, typename Algorithm = pre_order, typename Allocator = std::allocator<T>>
class tree final : public tree_base<T> {

public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const reference;
	using algorithm_type = const Algorithm;
	using allocator_type = Allocator;
	using pointer = typename std::allocator_traits<Allocator>::pointer;
	using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
	using node_type = node<T>;
	using difference_type = std::ptrdiff_t;
	using typename tree_base<T>::size_type;

	// Iterators
	template <typename A = Algorithm> using iterator = tree_iterator<T, A, false>;
	template <typename A = Algorithm> using const_iterator = tree_iterator<T, A, true>;
	template <typename A = Algorithm> using reverse_iterator = std::reverse_iterator<iterator<A>>;
	template <typename A = Algorithm> using const_reverse_iterator = std::reverse_iterator<const_iterator<A>>;

protected:
	template <typename It> node_type* extract_node(It it);

public:
	tree() = default;                  // Default constructor
	tree(const tree&);                 // Copy constructor
	tree(tree&&) = default;            // Move constructor
	tree& operator =(const tree&);     // Copy assignment operator
	tree& operator =(tree&) = default; // Move assignment operator
	~tree() = default;                 // Destructor

	/*   ---   Iterators   ---   *
	 * r => reverse
	 * c => constant
	 */
	// begin
	template <typename A = Algorithm> iterator<A> begin();
	template <typename A = Algorithm> const_iterator<A> begin() const;
	template <typename A = Algorithm> const_iterator<A> cbegin() const;
	// end
	template <typename A = Algorithm> iterator<A> end();
	template <typename A = Algorithm> const_iterator<A> end() const;
	template <typename A = Algorithm> const_iterator<A> cend() const;
	// reverse begin
	template <typename A = Algorithm> reverse_iterator<A> rbegin();
	template <typename A = Algorithm> const_reverse_iterator<A> rbegin() const;
	template <typename A = Algorithm> const_reverse_iterator<A> crbegin() const;
	// reverse end
	template <typename A = Algorithm> reverse_iterator<A> rend();
	template <typename A = Algorithm> const_reverse_iterator<A> rend() const;
	template <typename A = Algorithm> const_reverse_iterator<A> crend() const;

	/*   ---   Capacity   ---   */
	bool empty() const;
	size_type size() const;
	size_type max_size() const;

	/*   ---   Modifiers   ---   */
	void clear();
	template <typename It> iterator<typename It::algorithm_type> insert(It position, const value_type&);
	template <typename It> iterator<typename It::algorithm_type> insert(It position, value_type&&);
	template <typename It> iterator<typename It::algorithm_type> append_child(It position, const value_type&);
	template <typename It> iterator<typename It::algorithm_type> append_child(It position, value_type&&);
	template <typename It> iterator<typename It::algorithm_type> prepend_child(It position, const value_type&);
	template <typename It> iterator<typename It::algorithm_type> prepend_child(It position, value_type&&);
	template <typename It, typename ...Args> iterator<typename It::algorithm_type> emplace(It position, Args&&...);
	template <typename It, typename ...Args> iterator<typename It::algorithm_type> emplace_append_child(It position, Args&&...);
	template <typename It, typename ...Args> iterator<typename It::algorithm_type> emplace_prepend_child(It position, Args&&...);

	/*   ---   Serialization   ---   */// (compatible with boost)
	template <typename Archive> void save(Archive&) const;
	template <typename Archive> void load(Archive&);

};

} /* namespace ds */

#include "tree.tpp"

#endif /* H3702CC99_74F1_4C8D_9B1A_954EB6E53330 */

#ifndef ALGORITHM_HPP_
#define ALGORITHM_HPP_

namespace bds {

//This is just a level of indirection to not repeat the get_instance() method for every concrete visitor class
template <typename T>
class visitor {

protected:
	constexpr visitor() = default;                  // Default constructor
	visitor(const visitor&) = delete;             // Copy constructor
	visitor& operator =(const visitor&) = delete; // Copy assignment operator
	visitor(visitor&&) = delete;                  // Move constructor
	visitor& operator =(visitor&&) = delete;      // Move assignment operator

public:
	~visitor() = default;                           // Destructor
	static const T& get_instance();

};

} /* namespace bds */

#include "algorithm.tpp"

#endif /* ALGORITHM_HPP_ */

#ifndef ALGORITHM_HPP_
#define ALGORITHM_HPP_

namespace bds {

//This is just a level of indirection to not repeat the get_instance() method for every concrete visitor class
template <typename T>
class algorithm {

protected:
	constexpr algorithm() = default;					// Default constructor
	algorithm(const algorithm&) = delete;				// Copy constructor
	algorithm& operator =(const algorithm&) = delete;	// Copy assignment operator
	algorithm(algorithm&&) = delete;					// Move constructor
	algorithm& operator =(algorithm&&) = delete;		// Move assignment operator

public:
	~algorithm() = default;								// Destructor
	static const T& get_instance();

};

} /* namespace bds */

#include "algorithm.tpp"

#endif /* ALGORITHM_HPP_ */

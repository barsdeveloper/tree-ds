#ifndef VISITOR_HPP_
#define VISITOR_HPP_

namespace ds {

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

} /* namespace ds */

#include "visitor.tpp"

#endif /* VISITOR_HPP_ */

#ifndef H2E052275_35DB_4435_99C1_48D834F3941D
#define H2E052275_35DB_4435_99C1_48D834F3941D

namespace ds {

//This is just a level of indirection to not repeat the get_instance() method for every concrete visitor class
template <typename T>
class visitor {

protected:
	constexpr visitor() = default;                // Default constructor
	visitor(const visitor&) = delete;             // Copy constructor
	visitor& operator =(const visitor&) = delete; // Copy assignment operator
	visitor(visitor&&) = delete;                  // Move constructor
	visitor& operator =(visitor&&) = delete;      // Move assignment operator

public:
	~visitor() = default;                         // Destructor
	static const T& get_instance();

};

} /* namespace ds */

#include "visitor.tpp"

#endif /* H2E052275_35DB_4435_99C1_48D834F3941D */

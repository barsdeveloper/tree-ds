#pragma once

#include <cstddef> // std::size_t

#include <TreeDS/node/matching_node.hpp>

namespace ds {

template <std::size_t min, std::size_t max, typename... Children>
class vertical_range_match : public matching_node<Children...> {

    using matching_node<Children...>::matching_node;
};

} // namespace ds

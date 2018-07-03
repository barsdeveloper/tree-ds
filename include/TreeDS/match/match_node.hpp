#pragma once

namespace ds {

class match_node {

    public:
    virtual ~match_node() = default;

    template <typename Node>
    virtual bool match(Node node) const = 0;
};

} // namespace ds

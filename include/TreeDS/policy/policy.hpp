#pragma once

namespace md::detail {

template <typename Node>
class policy {

    protected:
    const Node* root = nullptr;

    constexpr policy() {
    }

    policy(const Node* root) :
            root(root) {
    }
};

} // namespace md::detail

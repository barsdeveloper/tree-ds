#pragma once

#include <TreeDS/match/matcher.hpp>

namespace ds {

class restrictive_matcher : public matcher {

    public:
    template <typename Matchable>
    bool match(const Matchable& m) {
        this->set_matched(nullptr);
        return false;
    }
};

} // namespace ds

#pragma once

#include <TreeDS/match/matcher.hpp>

namespace ds {

class permissive_matcher : public matcher {

    template <typename Matchable>
    bool match(const Matchable& m) {
        this->set_matched(&m);
        return true;
    }
};

} // namespace ds

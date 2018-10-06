#pragma once

#include <TreeDS/match/matcher.hpp>

namespace ds {

class matchable {

    virtual bool accept(const matcher& m) = 0;
};

} // namespace ds

#pragma once

class matcher {

    public:
    const matchable* matched;

    protected:
    void set_matched(const matchable* node) {
        this->match = node;
    }
};

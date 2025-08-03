#pragma once

#include <memory>

template <typename T>
class ConcurrentDeque {
private:
    struct node {
        std::shared_ptr<T> _data;
        std::unique_ptr<node> _left;
        std::unique_ptr<node> _right;
    }
    
    node* leftHead;
    node* rightHead;

public:

};
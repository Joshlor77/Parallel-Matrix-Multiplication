#pragma once

#include <mutex>
#include <memory>
#include <iostream>

/* Mutex-based Queue
 *
 * Based off the fine thread-safe queue from C++ Concurrency in Action 2nd Edition 
*/

template <typename T>
class ConcurrentQueue {
private:
    using mutexType = std::mutex;
    struct node{
        std::shared_ptr<T> _data;
        std::unique_ptr<node> _next;
    };

    std::unique_ptr<node> headPtr;
    node* tailPtr;
    mutable mutexType headMut;
    mutable mutexType tailMut;
public:
    ConcurrentQueue() :
        headPtr(new node), tailPtr(headPtr.get())
    {}

    void push(T data){
        std::shared_ptr<T> newData(std::make_shared<T>(std::move(data)));
        std::unique_ptr<node> newNode(new node);
        {
            std::lock_guard<mutexType> lk(tailMut);
            tailPtr->_data = newData;
            tailPtr->_next = std::move(newNode);
            tailPtr = tailPtr->_next.get();
        }
    }

    std::shared_ptr<T> tryPop(){
        std::lock_guard<mutexType> headlk(headMut);
        {
            std::lock_guard taillk(tailMut);
            if (headPtr.get() == tailPtr)
                return std::shared_ptr<T>();
        }
        std::shared_ptr<T> data = std::move(headPtr->_data);
        std::unique_ptr<node> oldHeadPtr(std::move(headPtr));
        headPtr = std::unique_ptr(std::move(oldHeadPtr->_next));
        return data;
    }

    bool tryPop(T& returnVariable){
        std::lock_guard<mutexType> headlk(headMut);
        {
            std::lock_guard taillk(tailMut);
            if (headPtr.get() == tailPtr)
                return false;
        }
        returnVariable = std::move(*(headPtr->_data));
        std::unique_ptr<node> oldHeadPtr(std::move(headPtr));
        headPtr = std::unique_ptr(std::move(oldHeadPtr->_next));
        
        return true;
    }

    bool empty() const {
        std::lock_guard<mutexType> headlk(headMut);
        std::lock_guard<mutexType> taillk(tailMut);
        if (headPtr.get() == tailPtr)
            return true;
        return false;
    }
};
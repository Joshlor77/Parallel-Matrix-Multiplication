#include "../Implementation/MutexConcurrentQueue.h"

#include <assert.h>
#include <iostream>

template <typename T>
class SerialChecks{
private:
    static void currentCheckNum(){
        static int checkNumber = 0;
        std::cout << "--------------------------------\n";
        std::cout << "---|     Check Number " << checkNumber++ << "     |---" << '\n';
        std::cout << "--------------------------------\n";
    }
public:
    static void emptyCheck(ConcurrentQueue<T>& Q){
        currentCheckNum();
        static int i = 0;
        std::cout << "Queue empty check #" << i++ << '\n';
        assert(Q.empty());
        std::cout << "Queue is empty\n\n";
    }

    static void emptyPopCheck(ConcurrentQueue<T>& Q){
        currentCheckNum();
        std::cout << "Empty Queue Pop Attempt\n";
        assert(Q.empty());
        std::cout << "Queue is empty\n";
        std::cout << "Attempting empty Pop\n";
        assert(Q.pop() == std::shared_ptr<T>());
        std::cout << "Queue successfully returned empty shared_ptr<T>\n\n";
    }

    static void PushPopCheck(ConcurrentQueue<T>& Q){
        currentCheckNum();
        std::cout << "Pushing element 42\n";
        Q.push(420);
        std::cout << "Pushing element 10\n";
        Q.push(69);
        T poppedElement = *Q.pop();
        assert(poppedElement == 420);
        std:: cout << "Popped element " << poppedElement << " is same as pushed element\n";
        poppedElement = *Q.pop();
        assert(poppedElement == 69);
        std:: cout << "Popped element " << poppedElement << " is same as pushed element\n\n";
    }
};

int main(){

    ConcurrentQueue<int> Q;
    SerialChecks<int>::emptyCheck(Q);
    SerialChecks<int>::PushPopCheck(Q);
    SerialChecks<int>::emptyPopCheck(Q);

    return 0;
}
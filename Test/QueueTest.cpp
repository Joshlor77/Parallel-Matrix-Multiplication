#include "../Implementation/LockedConcurrentQueue.h"
#include <thread>
#include <vector>

#include <assert.h>
#include <iostream>

class Check {
public:
    static void currentCheckNum(){
        static int checkNumber = 0;
        std::cout << "--------------------------------\n";
        std::cout << "---|     Check Number " << checkNumber++ << "     |---" << '\n';
        std::cout << "--------------------------------\n";
    }
};

template <typename T>
class SerialChecks : Check {
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
        assert(Q.tryPop() == std::shared_ptr<T>());
        std::cout << "Queue successfully returned empty shared_ptr<T>\n";
        T value;
        assert(!Q.tryPop(value));
        std::cout << "Queue succesfully returned false\n\n";
    }

    static void PushPopCheck(ConcurrentQueue<T>& Q){
        currentCheckNum();
        T element1 = 420;
        T element2 = 69;
        std::cout << "Pushing element " << element1 << '\n';
        Q.push(420);
        std::cout << "Pushing element " << element2 << '\n';
        Q.push(69);
        T poppedElement = *Q.tryPop();
        assert(poppedElement == element1);
        std:: cout << "Popped element " << poppedElement << " is same as pushed element\n";
        Q.tryPop(poppedElement);
        assert(poppedElement == element2);
        std:: cout << "Popped element " << poppedElement << " is same as pushed element\n\n";
    }
};

template <typename T>
class ParallelChecks : Check {
private:
    static void pushProc(ConcurrentQueue<T>& Q, int iterations){
        for (int i = 0; i < iterations; i++){
            Q.push(T{i});
        };
    }
    static void popProc(ConcurrentQueue<T>& Q, int iterations){
        T poppedValue;
        for (int i = 0; i < iterations; i++){
            Q.tryPop(poppedValue);
        };
    }
public:
    static void onePush_onePop(ConcurrentQueue<T>& Q){
        currentCheckNum();
        std::cout << "ONE pushing thread and ONE popping thread test START\n";
        std::thread popper(ParallelChecks::popProc, std::ref(Q), 10000000);
        std::thread pusher(ParallelChecks::pushProc, std::ref(Q), 10000000);

        popper.join();
        pusher.join();
        std::cout << "ONE pushing thread and ONE popping thread test COMPLETE\n";
    }
    static void manyPush_onePop(ConcurrentQueue<T>& Q){
        currentCheckNum();
        std::cout << "MANY pushing threads and ONE popping thread test START\n";
        std::vector<std::thread> pushers(15);
        for (auto& t: pushers){
            t = std::thread(ParallelChecks::pushProc, std::ref(Q), 1000000);
        }
        std::thread popper(ParallelChecks::popProc, std::ref(Q), 10000000);
        for (auto& t : pushers){
            if (t.joinable())
                t.join();
        }
        popper.join();
        std::cout << "MANY pushing threads and ONE popping thread test COMPLETE\n";
    }
    static void onePush_manyPop(ConcurrentQueue<T>& Q){
        currentCheckNum();
        std::cout << "ONE pushing thread and MANY popping threads test START\n";
        std::vector<std::thread> poppers(15);
        for (auto& t: poppers){
            t = std::thread(ParallelChecks::popProc, std::ref(Q), 1000000);
        }
        std::thread pusher(ParallelChecks::pushProc, std::ref(Q), 10000000);
        for (auto& t : poppers){
            if (t.joinable())
                t.join();
        }
        pusher.join();
        std::cout << "ONE pushing thread and MANY popping threads test COMPLETE\n";
    }
    static void manyPush_manyPop(ConcurrentQueue<T>& Q){
        currentCheckNum();
        std::cout << "MANY pushing threads and MANY popping threads test START\n";
        int threadCount = 8;
        std::vector<std::thread> poppers(threadCount);
        std::vector<std::thread> pushers(threadCount);

        for (int i = 0; i < threadCount; i++){
            poppers[i] = std::thread(ParallelChecks::popProc, std::ref(Q), 1000000);
            pushers[i] = std::thread(ParallelChecks::pushProc, std::ref(Q), 1000000);
        }

        for (int i = 0; i < threadCount; i++){
            poppers[i].join();
            pushers[i].join();
        }
        std::cout << "MANY pushing threads and MANY popping threads test COMPLETE\n";
    }
};

template <typename T>
void emptyQueue(ConcurrentQueue<T>& Q){
    T poppedVariable;
    while (!Q.empty()){
        Q.tryPop(poppedVariable);
    }
}

int main(){
    ConcurrentQueue<int> Q;
    SerialChecks<int>::emptyCheck(Q);
    SerialChecks<int>::PushPopCheck(Q);
    SerialChecks<int>::emptyPopCheck(Q);

    ParallelChecks<int>::onePush_onePop(Q);
    emptyQueue<int>(Q);
    ParallelChecks<int>::manyPush_onePop(Q);
    emptyQueue<int>(Q);
    ParallelChecks<int>::onePush_manyPop(Q);
    emptyQueue<int>(Q);
    ParallelChecks<int>::manyPush_manyPop(Q);
    emptyQueue<int>(Q);



    return 0;
}
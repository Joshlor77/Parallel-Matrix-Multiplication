#include "../Implementation/ThreadPool.h"

#include <iostream>
#include <sstream>

void print(){
    std::ostringstream oss;
    oss << "Work done by Thread ID: " << std::this_thread::get_id() << '\n';
    std::cout << oss.str();
}

int main(){

    ThreadPool pool;
    for (int i = 0; i < 16; i++)
        pool.submit(print);

    return 0;
}
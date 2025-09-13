#include <vector>
#include "Order.h"
#include "OrderPool.h"

OrderPool::OrderPool(int cap = 1e6) {
    capacity = cap;
    pool.resize(capacity);
    freeArr.resize(capacity);
    freeList.reserve(capacity);
    for (int i = capacity - 1; i >= 0; i--) {
        freeList.push_back(i);
        freeArr[i] = 0;
    }
}

Order* OrderPool::allocate() {
    if (freeList.empty()) return nullptr; // no space
    int idx = freeList.back();
    freeArr[idx] = 1;
    freeList.pop_back();
    return &pool[idx];
}

void OrderPool::deallocate(Order* order) {
    int idx = order - pool.data(); // pointer arithmetic
    freeArr[idx] = 0;
    freeList.push_back(idx);
}

void OrderPool::Print(ostream &outfile) {
    bool a[1000000] = {};
    for (auto i : freeList)
        a[i] = true;
    for (int i = 0; i < capacity; i++) 
        if (!a[i])
            pool[i].Print(outfile);
}

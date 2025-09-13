#ifndef ORDERPOOL_H
#define ORDERPOOL_H

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

struct Order;

class OrderPool {
public:
    vector<Order> pool;
    vector<int> freeList; // indices of free slots
    vector<int> freeArr;
    int capacity;

    OrderPool(int cap);

    Order* allocate();

    void deallocate(Order* order);

    void Print(ostream &outfile = cout);
};

#endif // ORDERPOOL_H

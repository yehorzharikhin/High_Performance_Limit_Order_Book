#ifndef LIMIT_ORDER_BOOK_H
#define LIMIT_ORDER_BOOK_H

#include <vector>
#include <string>
using namespace std;

struct Order {
    int id;
    double price;
    int quantity;
    bool isBuy;
};

class LimitOrderBook {
public:
    void addOrder(const Order& order);
    void printBook() const;
private:
    vector<Order> buyOrders;
    vector<Order> sellOrders;
};

#endif

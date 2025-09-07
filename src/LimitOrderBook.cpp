#include "LimitOrderBook.h"
#include <iostream>
using namespace std;

void LimitOrderBook::addOrder(const Order& order) {
    if (order.isBuy) 
        buyOrders.push_back(order);
    else 
        sellOrders.push_back(order);
}

void LimitOrderBook::printBook() const {
    cout << "Buy Orders:\n";
    for (auto &o : buyOrders) {
        cout << "ID " << o.id << " " << o.quantity << " @ " << o.price << "\n";
    }
    cout << "Sell Orders:\n";
    for (auto &o : sellOrders) {
        cout << "ID " << o.id << " " << o.quantity << " @ " << o.price << "\n";
    }
}

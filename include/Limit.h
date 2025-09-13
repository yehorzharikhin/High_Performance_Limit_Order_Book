#ifndef LIMIT_H
#define LIMIT_H

#include "Order.h"

class Limit {
public:
    // FIFO queue of orders at this price
    Order *headOrder;
    Order *tailOrder;

    int limitPrice;        // Price level
    int size;              // Number of orders at this price
    int totalVolume;       // Sum of shares at this price

    // Constructor
    Limit(int price = 0)
        : limitPrice(price),
          size(0),
          totalVolume(0),
          headOrder(nullptr),
          tailOrder(nullptr) {}

    // Return price
    int getPrice() const;

    // Return head order (first in queue)
    Order* getHeadOrder() const;

    // Add order to the tail (FIFO)
    void addOrder(Order* order);

    // Remove order from the head (FIFO pop)
    Order* popOrder();

    // Remove a specific order (not just head)
    void removeOrder(Order* order);

    bool empty() const;
};

#endif // LIMIT_H

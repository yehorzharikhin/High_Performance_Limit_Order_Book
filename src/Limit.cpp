#include "Limit.h"
#include "Order.h"

// Return price
int Limit::getPrice() const {
    return limitPrice;
}

// Return head order (first in queue)
Order* Limit::getHeadOrder() const {
    return headOrder;
}

void Limit::addOrder(Order* order) {
    order->parentLimit = this;
    order->nextOrder = nullptr;
    order->prevOrder = tailOrder;
    if (tailOrder) {
        tailOrder->nextOrder = order;
    } else {
        headOrder = order; // empty list before
    }
    tailOrder = order;
    size++;
    totalVolume += order->shares;
}

// Remove order from the head (FIFO pop)
Order* Limit::popOrder() {
    if (!headOrder) return nullptr;
    Order* order = headOrder;
    headOrder = order->nextOrder;
    if (headOrder) {
        headOrder->prevOrder = nullptr;
    } else {
        tailOrder = nullptr; // now empty
    }
    order->nextOrder = order->prevOrder = nullptr;
    size--;
    totalVolume -= order->shares;
    return order;
}

// Remove a specific order (not just head)
void Limit::removeOrder(Order* order) {
    if (!order) return;
    if (order->prevOrder) order->prevOrder->nextOrder = order->nextOrder;
    else headOrder = order->nextOrder;

    if (order->nextOrder) order->nextOrder->prevOrder = order->prevOrder;
    else tailOrder = order->prevOrder;

    size--;
    totalVolume -= order->shares;
    order->nextOrder = order->prevOrder = nullptr;
}

bool Limit::empty() const { return size == 0; }
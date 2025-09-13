#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <fstream>
#include "OrderPool.h"

class Limit;
class OrderPool;

// Enum for order side
enum class OrderSide {
    BUY,
    SELL
};

// Enum for order status
enum class OrderStatus {
    ACTIVE,     // still in the book
    PARTIALLY_FILLED,
    FILLED,
    CANCELED
};

// Represents an individual order in the limit order book.
struct Order {
    // === Parent reference ===
    Limit *parentLimit;      // Pointer back to the price level container

    OrderPool *parentOrderPool;
    
    // === Linked list pointers ===
    Order *nextOrder;        // Next order at same price level
    Order *prevOrder;        // Previous order at same price level

    // === Core fields ===
    int idNumber;            // Unique order ID
    int shares;              // Quantity of shares
    int price;               // Limit price (e.g. in cents)
    int entryTime;           // Time when order was entered
    int eventTime;           // Last updated event time
    OrderStatus status;      // Current status of the order
    OrderSide side;          // BUY or SELL

    // === Constructor ===
    Order(int id = 0, OrderSide s = OrderSide::BUY, int qty = 0, int p = 0, int time = 0, OrderPool *parentPool = nullptr)
        : idNumber(id),
          side(s),
          shares(qty),
          price(p),
          entryTime(time),
          eventTime(time),
          status(OrderStatus::ACTIVE),
          nextOrder(nullptr),
          prevOrder(nullptr),
          parentLimit(nullptr),
          parentOrderPool(parentPool) {}

    void Cancel() {
        nextOrder = nullptr;
        prevOrder = nullptr;
        parentLimit = nullptr;
        status = OrderStatus::CANCELED; // optional safety
        if (parentOrderPool)
            parentOrderPool->deallocate(this);
    }

    void Fill() {
        Cancel();
        status = OrderStatus::FILLED;
    }

    void PartiallyFill() {
        status = OrderStatus::PARTIALLY_FILLED;
    }

    void CopyFrom(Order *other) {
        parentLimit = other->parentLimit;
        parentOrderPool = other->parentOrderPool;
        nextOrder = other->nextOrder;
        prevOrder = other->prevOrder;
        shares = other->shares;
        price = other->price;
        entryTime = other->entryTime;
        eventTime = other->eventTime;
        status = other->status;
        side = other->side;
    }

    void Print(ostream &outfile = cout) {
        if (side == OrderSide::BUY)
            outfile << "BUY:  ";
        else
            outfile << "SELL: ";
        outfile << shares << " @ " << price << "\n";
    }
};

#endif // ORDER_H

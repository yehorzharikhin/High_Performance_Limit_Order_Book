#ifndef ORDER_H
#define ORDER_H

class Limit;

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
    // === Core fields ===
    long long idNumber;      // Unique order ID
    int shares;              // Quantity of shares
    int price;               // Limit price (e.g. in cents)
    int entryTime;           // Time when order was entered
    int eventTime;           // Last updated event time
    OrderSide side;          // BUY or SELL
    OrderStatus status;      // Current status of the order

    // === Linked list pointers ===
    Order *nextOrder;        // Next order at same price level
    Order *prevOrder;        // Previous order at same price level

    // === Parent reference ===
    Limit *parentLimit;      // Pointer back to the price level container

    // === Constructor ===
    Order(long long id = 0, OrderSide s = OrderSide::BUY, int qty = 0, int p = 0, int time = 0)
        : idNumber(id),
          side(s),
          shares(qty),
          price(p),
          entryTime(time),
          eventTime(time),
          status(OrderStatus::ACTIVE),
          nextOrder(nullptr),
          prevOrder(nullptr),
          parentLimit(nullptr) {}

    // === Destructor ===
    ~Order() {
        nextOrder = nullptr;
        prevOrder = nullptr;
        parentLimit = nullptr;
        status = OrderStatus::CANCELED; // optional safety
    }
};

#endif // ORDER_H

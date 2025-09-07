#ifndef LIMIT_ORDER_BOOK_H
#define LIMIT_ORDER_BOOK_H

#include <cstdint>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <iostream>
using namespace std;

using Price = int64_t;
using Quantity = int64_t;
using OrderId = uint64_t;

// A trade: (buyerId, sellerId, price, quantity)
using Trade = tuple<OrderId, OrderId, Price, Quantity>;

struct Order {
    OrderId id;
    Price price;       // integer ticks
    Quantity qty;      // remaining quantity
    bool isBuy;
    uint64_t timestamp;
};

class LimitOrderBook {
public:
    LimitOrderBook();

    // Insert a limit order; returns the order id issued.
    // Emits trades for any matches (returns vector of trades).
    OrderId insertLimitOrder(bool isBuy, Price price, Quantity qty, vector<Trade>& trades);

    // Print top-of-book for debugging
    void printBook(ostream& os = cout) const;

private:
    struct PriceLevel {
        Price price;
        list<Order> orders;   // FIFO queue of orders at this price
        Quantity totalQty = 0;
    };

    using BuyMap  = map<Price, PriceLevel, greater<Price>>;
    using SellMap = map<Price, PriceLevel>;

    BuyMap buys_;
    SellMap sells_;

    // Map order id -> iterator to PriceLevel + iterator to order (for cancellation/lookup)
    // We'll store minimal meta to find and remove orders later if needed.
    struct OrderLocation {
        bool isBuy;
        Price price;
        // We can't store list::iterator type here cleanly in header without forward declarations,
        // so for now keep location info and skip cancel implementation.
    };
    unordered_map<OrderId, OrderLocation> orderIndex_;

    OrderId nextOrderId_;
    uint64_t nextTimestamp_;

    // Matching helper
    void matchBuy(Price price, Quantity &qty, vector<Trade>& trades);
    void matchSell(Price price, Quantity &qty, vector<Trade>& trades);
};

#endif // LIMIT_ORDER_BOOK_H

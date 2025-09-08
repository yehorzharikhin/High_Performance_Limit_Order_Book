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
using Timestamp = uint64_t;

struct Order {
    OrderId id;
    Price price;
    Quantity qty;
    bool isBuy;
    Timestamp ts;
};

struct Trade {
    OrderId buyerId;
    OrderId sellerId;
    Price price;
    Quantity qty;
    Timestamp ts;
};

class LimitOrderBook {
public:
    LimitOrderBook();

    // Insert a limit order and return the assigned order id.
    // The vector trades will be populated with trades executed by this insertion.
    OrderId insertLimitOrder(bool isBuy, Price price, Quantity qty, vector<Trade>& trades);

    // Cancel a resting order by id. Returns true if cancelled.
    bool cancelOrder(OrderId id);

    // Reduce quantity of a resting order (amend) - sets remaining qty to newQty if smaller.
    // Returns true if amended; false if order not found or newQty >= current qty.
    bool reduceOrderQuantity(OrderId id, Quantity newQty);

    // Print top-of-book and summary
    void printBook(ostream& os = cout) const;

private:
    struct PriceLevel {
        Price price;
        list<Order> orders;
        Quantity totalQty = 0;
    };

    using BuyMap  = map<Price, PriceLevel, greater<Price>>;
    using SellMap = map<Price, PriceLevel>;

    BuyMap buys_;
    SellMap sells_;

    // Location of a resting order in the book
    struct OrderLocation {
        bool isBuy;
        // iterator pointing to PriceLevel in buys_ or sells_
        // use map<...>::iterator; types available here:
        // We'll store generic iterators as void*? No — we can typedef them:
    };

    // We'll store the iterators in a separate structure that depends on the class types:
    struct OrderRef {
        bool isBuy;
        // Price level iterator types
        // (declare after type aliases so they are visible to the compiler)
        // these typedefs are here for readability.
    };

    // A map from order id -> pair(priceLevelIt, orderIt)
    // We define type aliases to store iterators.
    using BuyLevelIt = BuyMap::iterator;
    using SellLevelIt = SellMap::iterator;
    using OrderListIt = list<Order>::iterator;

    struct Location {
        bool isBuy;
        // one of these is valid depending on isBuy
        BuyLevelIt buyLevelIt;
        SellLevelIt sellLevelIt;
        OrderListIt orderIt;
    };

    unordered_map<OrderId, Location> orderIndex_;

    OrderId nextOrderId_;
    Timestamp nextTs_;

    // Helpers
    void matchBuy(OrderId incomingId, Price price, Quantity &qty, vector<Trade>& trades);
    void matchSell(OrderId incomingId, Price price, Quantity &qty, vector<Trade>& trades);
};
#endif // LIMIT_ORDER_BOOK_H

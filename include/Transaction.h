#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Order.h"

struct Transaction {
    long long transactionID; // Unique ID for the trade
    int shares;              // Number of shares traded
    int price;               // Trade price (limit level)
    int eventTime;           // Time of execution

    long long buyOrderID;          // ID of the buy order
    long long sellOrderID;         // ID of the sell order

    // Optional: direct pointers for quick backtracking
    Order* buyOrder;
    Order* sellOrder;

    // Constructor
    Transaction(long long id = 0, int qty = 0, int p = 0, int time = 0,
                long long buyID = 0, long long sellID = 0,
                Order* bOrder = nullptr,
                Order* sOrder = nullptr)
        : transactionID(id),
          shares(qty),
          price(p),
          eventTime(time),
          buyOrderID(buyID),
          sellOrderID(sellID),
          buyOrder(bOrder),
          sellOrder(sOrder) {}
};

#endif // TRANSACTION_H

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Order.h"

struct Transaction {
    int transactionID;       // Unique ID for the trade
    int shares;              // Number of shares traded
    int price;               // Trade price (limit level)
    int eventTime;           // Time of execution

    int buyOrderID;          // ID of the buy order
    int sellOrderID;         // ID of the sell order

    // Constructor
    Transaction(int id = 0, int qty = 0, int p = 0, int time = 0,
                int buyID = 0, int sellID = 0)
        : transactionID(id),
          shares(qty),
          price(p),
          eventTime(time),
          buyOrderID(buyID),
          sellOrderID(sellID) {}
};

#endif // TRANSACTION_H

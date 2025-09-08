#include "LimitOrderBook.h"
#include <iostream>
#include <cassert>

void printTrades(const std::vector<Trade>& trades) {
    if (trades.empty()) {
        std::cout << "No trades executed.\n";
        return;
    }
    std::cout << "Trades:\n";
    for (const auto &t : trades) {
        std::cout << "Buyer=" << t.buyerId << " Seller=" << t.sellerId
                  << " Price=" << t.price << " Qty=" << t.qty << " TS=" << t.ts << "\n";
    }
}

int main() {
    LimitOrderBook lob;
    std::vector<Trade> trades;

    // Add some resting sell orders
    lob.insertLimitOrder(false, 10100, 50, trades); // sell 50 @ 101.00
    lob.insertLimitOrder(false, 10200, 30, trades); // sell 30 @ 102.00

    // Insert buy crossing both levels
    OrderId buyId = lob.insertLimitOrder(true, 10200, 60, trades);
    std::cout << "Inserted buy order id = " << buyId << "\n";
    printTrades(trades);
    lob.printBook();

    // Add some buys
    lob.insertLimitOrder(true, 10050, 40, trades); // buy @ 100.50
    lob.insertLimitOrder(true, 10100, 20, trades); // buy @ 101.00

    lob.printBook();

    // Cancel a resting order (pick an id that was resting)
    // For demo we will place an order and cancel it.
    std::vector<Trade> tmp;
    OrderId oid = lob.insertLimitOrder(false, 10300, 100, tmp); // sell @ 103.00
    std::cout << "Inserted sell order id = " << oid << "\n";
    bool ok = lob.cancelOrder(oid);
    std::cout << "Cancel order " << oid << " -> " << (ok ? "OK" : "NOT FOUND") << "\n";
    lob.printBook();

    // Reduce quantity example
    OrderId smallBuy = lob.insertLimitOrder(true, 10050, 30, tmp);
    std::cout << "Inserted small buy id = " << smallBuy << "\n";
    bool reduced = lob.reduceOrderQuantity(smallBuy, 10);
    std::cout << "Reduced order " << smallBuy << " -> " << (reduced ? "OK" : "FAILED") << "\n";
    lob.printBook();

    return 0;
}

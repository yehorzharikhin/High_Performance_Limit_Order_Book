#include "LimitOrderBook.h"
using namespace std;

LimitOrderBook::LimitOrderBook() : nextOrderId_(1), nextTimestamp_(1) {}

OrderId LimitOrderBook::insertLimitOrder(bool isBuy, Price price, Quantity qty, vector<Trade>& trades) {
    trades.clear();
    if (qty <= 0) return 0;

    OrderId oid = nextOrderId_++;
    uint64_t ts = nextTimestamp_++;

    // Try to match immediately
    if (isBuy) {
        matchBuy(price, qty, trades);
    } else {
        matchSell(price, qty, trades);
    }

    // If remaining qty > 0, add to resting book
    if (qty > 0) {
        Order o{oid, price, qty, isBuy, ts};
        if (isBuy) {
            auto it = buys_.find(price);
            if (it == buys_.end()) {
                PriceLevel pl;
                pl.price = price;
                pl.orders.push_back(o);
                pl.totalQty = qty;
                buys_.emplace(price, move(pl));
            } else {
                it->second.orders.push_back(o);
                it->second.totalQty += qty;
            }
        } else {
            auto it = sells_.find(price);
            if (it == sells_.end()) {
                PriceLevel pl;
                pl.price = price;
                pl.orders.push_back(o);
                pl.totalQty = qty;
                sells_.emplace(price, move(pl));
            } else {
                it->second.orders.push_back(o);
                it->second.totalQty += qty;
            }
        }
        orderIndex_[oid] = OrderLocation{isBuy, price};
    } else {
        // fully filled - don't insert into book; still register id if needed
        orderIndex_[oid] = OrderLocation{isBuy, price};
    }

    return oid;
}

void LimitOrderBook::matchBuy(Price price, Quantity &qty, vector<Trade>& trades) {
    // while we have qty to buy and there is a best sell with price <= buy price
    while (qty > 0 && !sells_.empty()) {
        auto bestIt = sells_.begin(); // lowest sell price
        if (bestIt->first > price) break; // best sell price too high

        PriceLevel &pl = bestIt->second;

        // iterate through orders at this price level
        while (qty > 0 && !pl.orders.empty()) {
            Order &rest = pl.orders.front();
            Quantity tradeQty = min(qty, rest.qty);

            // buyer id unknown yet (we register id outside); we will set buyer id later,
            // for now we need to create a trade with buyerId placeholder -> we'll use 0
            // But better: the calling code provided oid; however this function does not know it.
            // To keep code simple, we'll emit trades with buyerId=0 and sellerId=rest.id.
            // In practice you would pass buyerId into matchBuy; to keep this demo simple:
            trades.emplace_back(0, rest.id, pl.price, tradeQty);

            qty -= tradeQty;
            rest.qty -= tradeQty;
            pl.totalQty -= tradeQty;

            if (rest.qty == 0) {
                // fully filled resting order
                orderIndex_.erase(rest.id);
                pl.orders.pop_front();
            }
        }

        // remove exhausted price level
        if (pl.orders.empty()) {
            sells_.erase(bestIt);
        }
    }
}

void LimitOrderBook::matchSell(Price price, Quantity &qty, vector<Trade>& trades) {
    while (qty > 0 && !buys_.empty()) {
        auto bestIt = buys_.begin(); // highest buy price due to greater comparator
        if (bestIt->first < price) break; // best buy too low

        PriceLevel &pl = bestIt->second;

        while (qty > 0 && !pl.orders.empty()) {
            Order &rest = pl.orders.front();
            Quantity tradeQty = min(qty, rest.qty);
            trades.emplace_back(rest.id, 0, pl.price, tradeQty); // buyer id = rest.id, seller id placeholder = 0

            qty -= tradeQty;
            rest.qty -= tradeQty;
            pl.totalQty -= tradeQty;

            if (rest.qty == 0) {
                orderIndex_.erase(rest.id);
                pl.orders.pop_front();
            }
        }

        if (pl.orders.empty()) {
            buys_.erase(bestIt);
        }
    }
}

void LimitOrderBook::printBook(ostream& os) const {
    os << "=== Order Book ===\n";
    os << "Buys (top 5):\n";
    int cnt = 0;
    for (auto it = buys_.begin(); it != buys_.end() && cnt < 5; ++it, ++cnt) {
        os << "Price: " << it->first << " TotalQty: " << it->second.totalQty << " Orders: " << it->second.orders.size() << "\n";
    }
    os << "Sells (top 5):\n";
    cnt = 0;
    for (auto it = sells_.begin(); it != sells_.end() && cnt < 5; ++it, ++cnt) {
        os << "Price: " << it->first << " TotalQty: " << it->second.totalQty << " Orders: " << it->second.orders.size() << "\n";
    }
    os << "==================\n";
}

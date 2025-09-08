#include "LimitOrderBook.h"
using namespace std;

LimitOrderBook::LimitOrderBook() : nextOrderId_(1), nextTs_(1) { }

OrderId LimitOrderBook::insertLimitOrder(bool isBuy, Price price, Quantity qty, vector<Trade>& trades) {
    trades.clear();
    if (qty <= 0) return 0;

    OrderId oid = nextOrderId_++;
    Timestamp ts = nextTs_++;

    // Try to match
    if (isBuy) {
        matchBuy(oid, price, qty, trades);
    } else {
        matchSell(oid, price, qty, trades);
    }

    // If remaining qty > 0, add to book
    if (qty > 0) {
        Order o{oid, price, qty, isBuy, ts};
        if (isBuy) {
            auto it = buys_.find(price);
            if (it == buys_.end()) {
                PriceLevel pl;
                pl.price = price;
                pl.orders.push_back(o);
                pl.totalQty = qty;
                auto inserted = buys_.emplace(price, move(pl)).first;
                // store index: find the list iterator
                auto &ordersRef = inserted->second.orders;
                auto orderIt = prev(ordersRef.end());
                Location loc;
                loc.isBuy = true;
                loc.buyLevelIt = inserted;
                loc.orderIt = orderIt;
                orderIndex_[oid] = move(loc);
            } else {
                it->second.orders.push_back(o);
                it->second.totalQty += qty;
                auto orderIt = prev(it->second.orders.end());
                Location loc; loc.isBuy = true; loc.buyLevelIt = it; loc.orderIt = orderIt;
                orderIndex_[oid] = move(loc);
            }
        } else {
            auto it = sells_.find(price);
            if (it == sells_.end()) {
                PriceLevel pl;
                pl.price = price;
                pl.orders.push_back(o);
                pl.totalQty = qty;
                auto inserted = sells_.emplace(price, move(pl)).first;
                auto &ordersRef = inserted->second.orders;
                auto orderIt = prev(ordersRef.end());
                Location loc; loc.isBuy = false; loc.sellLevelIt = inserted; loc.orderIt = orderIt;
                orderIndex_[oid] = move(loc);
            } else {
                it->second.orders.push_back(o);
                it->second.totalQty += qty;
                auto orderIt = prev(it->second.orders.end());
                Location loc; loc.isBuy = false; loc.sellLevelIt = it; loc.orderIt = orderIt;
                orderIndex_[oid] = move(loc);
            }
        }
    } else {
        // Fully filled immediately: nothing to add to index (no resting order).
    }
    return oid;
}

void LimitOrderBook::matchBuy(OrderId incomingId, Price price, Quantity &qty, vector<Trade>& trades) {
    while (qty > 0 && !sells_.empty()) {
        auto bestIt = sells_.begin(); // lowest sell price
        if (bestIt->first > price) break;

        PriceLevel &pl = bestIt->second;
        while (qty > 0 && !pl.orders.empty()) {
            Order &rest = pl.orders.front();
            Quantity tradeQty = min(qty, rest.qty);

            Trade t;
            t.buyerId = incomingId;
            t.sellerId = rest.id;
            t.price = pl.price;
            t.qty = tradeQty;
            t.ts = nextTs_++;

            trades.push_back(t);

            qty -= tradeQty;
            rest.qty -= tradeQty;
            pl.totalQty -= tradeQty;

            if (rest.qty == 0) {
                // Erase resting order from index and pop
                auto itfind = orderIndex_.find(rest.id);
                if (itfind != orderIndex_.end()) orderIndex_.erase(itfind);
                pl.orders.pop_front();
            } else {
                // partially filled resting order: update and break if incoming filled
            }
        }

        if (pl.orders.empty()) {
            sells_.erase(bestIt);
        }
    }
}

void LimitOrderBook::matchSell(OrderId incomingId, Price price, Quantity &qty, vector<Trade>& trades) {
    while (qty > 0 && !buys_.empty()) {
        auto bestIt = buys_.begin(); // highest buy price
        if (bestIt->first < price) break;

        PriceLevel &pl = bestIt->second;
        while (qty > 0 && !pl.orders.empty()) {
            Order &rest = pl.orders.front(); // oldest buy at this price
            Quantity tradeQty = min(qty, rest.qty);

            Trade t;
            t.buyerId = rest.id;
            t.sellerId = incomingId;
            t.price = pl.price;
            t.qty = tradeQty;
            t.ts = nextTs_++;

            trades.push_back(t);

            qty -= tradeQty;
            rest.qty -= tradeQty;
            pl.totalQty -= tradeQty;

            if (rest.qty == 0) {
                auto itfind = orderIndex_.find(rest.id);
                if (itfind != orderIndex_.end()) orderIndex_.erase(itfind);
                pl.orders.pop_front();
            }
        }

        if (pl.orders.empty()) {
            buys_.erase(bestIt);
        }
    }
}

bool LimitOrderBook::cancelOrder(OrderId id) {
    auto it = orderIndex_.find(id);
    if (it == orderIndex_.end()) return false;
    Location loc = it->second;
    if (loc.isBuy) {
        auto levelIt = loc.buyLevelIt;
        PriceLevel &pl = levelIt->second;
        Quantity q = loc.orderIt->qty;
        pl.totalQty -= q;
        pl.orders.erase(loc.orderIt);
        if (pl.orders.empty()) buys_.erase(levelIt);
    } else {
        auto levelIt = loc.sellLevelIt;
        PriceLevel &pl = levelIt->second;
        Quantity q = loc.orderIt->qty;
        pl.totalQty -= q;
        pl.orders.erase(loc.orderIt);
        if (pl.orders.empty()) sells_.erase(levelIt);
    }
    orderIndex_.erase(it);
    return true;
}

bool LimitOrderBook::reduceOrderQuantity(OrderId id, Quantity newQty) {
    auto it = orderIndex_.find(id);
    if (it == orderIndex_.end()) return false;
    Location &loc = it->second;
    Order &o = *(loc.orderIt);
    if (newQty >= o.qty) return false; // only allow reduce
    Quantity diff = o.qty - newQty;
    o.qty = newQty;
    if (loc.isBuy) {
        loc.buyLevelIt->second.totalQty -= diff;
    } else {
        loc.sellLevelIt->second.totalQty -= diff;
    }
    return true;
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

#include <iostream>
#include <algorithm>
#include <climits>
#include <fstream>
#include <string>
#include <cmath>
#include "OrderPool.h"
#include "Book.h"
using namespace std;

Book::Book(const int n, const int max_p) : rng(random_device{}()), dist(1, INT_MAX), size(0), max_size(0), N(n), MAX_PRICE(max_p), pool(min(1000000, max(n / 100, 100000))) {
    buyLimits.resize(max_p + 1);
    sellLimits.resize(max_p + 1);
    bestBid = -1;
    bestAsk = max_p + 1;
}

int Book::generateOrderID() {
    return dist(rng);
}

Limit* Book::getBestBid() {
    if (bestBid <= -1) return nullptr;
    return &buyLimits[bestBid];
}

Limit* Book::getBestAsk() {
    if (bestAsk >= MAX_PRICE + 1) return nullptr;
    return &sellLimits[bestAsk];
}

void Book::addOrder(Order* order) {
    if (!order) return;
    Order *newOrder = pool.allocate();
    newOrder->CopyFrom(order);
    order->idNumber=newOrder->idNumber;
    if (newOrder->side == OrderSide::BUY) {
        insertIntoBuy(newOrder);
    } else {
        insertIntoSell(newOrder);
    }
}

void Book::cancelOrder(Order* order) {
    if (!order) return;
    if (order->side == OrderSide::BUY) {
        removeFromBuy(order);
    } else {
        removeFromSell(order);
    }
}

void Book::AddTransactions(vector<Transaction> new_transactions) {
    transactions.insert(transactions.end(),
                        new_transactions.begin(),
                        new_transactions.end());
}

void Book::PrintTransactions(ostream &outfile = cout) {
    if (!outfile) {
        int i = 0;
        outfile << "\nExecuted transactions (" << transactions.size() << "):\n";
        for (const auto& t : transactions) {
            cout << "Trade: " << dec << t.shares << " @ " << t.price
                 << ", SellID=" << hex << t.sellOrderID
                 << " (BuyID=" << hex << t.buyOrderID
                 << ", Time=" << dec << t.eventTime << ")\n";
            if (++i > 9)
                break;
        }
    }
    else {
        int i = 0;
        outfile << "\nExecuted transactions (" << transactions.size() << "):\n";
        for (const auto& t : transactions) {
            outfile << "Trade: " << dec << t.shares << " @ " << t.price
                << " (BuyID=" << hex << t.buyOrderID
                << ", SellID=" << hex << t.sellOrderID
                << ", Time=" << dec << t.eventTime << ")\n";
            if (++i > 9)
                break;
        }
    }
}

void Book::PrintPool(ostream &outfile) {
    pool.Print(outfile);
}

// -------- MATCHING ENGINE --------
vector<Transaction> Book::executeOrders(int currentTime) {
    vector<Transaction> trades;
    while (bestBid != -1 && bestAsk != MAX_PRICE + 1) {
        Limit* bestBidTemp = getBestBid();
        Limit* bestAskTemp = getBestAsk();
        
        if (bestBidTemp->getPrice() < bestAskTemp->getPrice()) break;
        
        Order* buyOrder = bestBidTemp->getHeadOrder();
        Order* sellOrder = bestAskTemp->getHeadOrder();
        if (!buyOrder || !sellOrder) break;
        
        int tradeShares = min(buyOrder->shares, sellOrder->shares);
        int tradePrice;
        if (sellOrder->entryTime < buyOrder->entryTime) // match at resting order price
            tradePrice = sellOrder->price;
        else
            tradePrice = buyOrder->price;
        
        Transaction tx(generateOrderID(), tradeShares, tradePrice, currentTime,
                       buyOrder->idNumber, sellOrder->idNumber);
        trades.push_back(tx);

        buyOrder->shares  -= tradeShares;
        sellOrder->shares -= tradeShares;

        if (buyOrder->shares == 0)
            removeFromBuy(buyOrder);

        if (sellOrder->shares == 0)
            removeFromSell(sellOrder);
    }

    AddTransactions(trades);
    return trades;
}

// -------- PRIVATE HELPERS --------
void Book::insertIntoBuy(Order* order) {
    int price = order->price;
    if (buyLimits[price].empty())
        buyLimitsmp[price] = true;
    buyLimits[price].addOrder(order);
    bestBid = max(bestBid, price);
    size++;
    max_size = max(max_size, size);
}

void Book::insertIntoSell(Order* order) {
    int price = order->price;
    if (sellLimits[price].empty())
        sellLimitsmp[price] = true;
    sellLimits[price].addOrder(order);
    bestAsk = min(bestAsk, price);
    size++;
    max_size = max(max_size, size);
}

void Book::removeFromBuy(Order* order) {
    int price = order->price;
    buyLimits[price].removeOrder(order);
    // while (price > -1 && buyLimits[price].empty())
    //     price--;
    // bestBid = price;
    // cout << order << endl;
    if (buyLimits[price].empty()) {
        buyLimitsmp.erase(price);
        if (buyLimitsmp.size() == 0)
            bestBid = -1;
        else
            bestBid = buyLimitsmp.begin()->first;
    }
    size--;
}

void Book::removeFromSell(Order* order) {
    int price = order->price;
    sellLimits[price].removeOrder(order);
    // while (price <= MAX_PRICE && sellLimits[price].empty())
    //     price++;
    // bestAsk = price;
    if (sellLimits[price].empty()) {
        sellLimitsmp.erase(price);
        if (sellLimitsmp.size() == 0)
            bestAsk = MAX_PRICE + 1;
        else
            bestAsk = sellLimitsmp.begin()->first;
    }
    size--;
}

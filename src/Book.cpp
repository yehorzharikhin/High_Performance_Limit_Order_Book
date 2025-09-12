#include "Book.h"
#include <iostream>
#include <algorithm>
#include <climits>
#include <fstream>
#include <string>
using namespace std;

Book::Book() : rng(random_device{}()), dist(1ll, LLONG_MAX) {}

long long Book::generateOrderID() {
    return dist(rng);
}

void Book::addOrder(Order* order) {
    if (order->side == OrderSide::BUY) {
        insertIntoBuyTree(buyTree, order);
    } else {
        insertIntoSellTree(sellTree, order);
    }
}

void Book::cancelOrder(Order* order) {
    if (order->side == OrderSide::BUY) {
        removeFromBuyTree(buyTree, order);
    } else {
        removeFromSellTree(sellTree, order);
    }
}

Limit* Book::getBestBid() const {
    if (buyTree.empty()) return nullptr;
    return buyTree.begin()->second;
}

Limit* Book::getBestAsk() const {
    if (sellTree.empty()) return nullptr;
    return sellTree.begin()->second;
}

void Book::AddTransactions(vector<Transaction> new_transactions) {
    transactions.insert(transactions.end(),
                        new_transactions.begin(),
                        new_transactions.end());
}

void Book::PrintTransactions(ostream &outfile = cout) {
    if (!outfile) {
        cout << "\nExecuted transactions:\n";
        for (const auto& t : transactions) {
            cout << "Trade: " << dec << t.shares << " @ " << t.price
                << " (BuyID=" << hex << t.buyOrderID
                << ", SellID=" << hex << t.sellOrderID
                << ", Time=" << dec << t.eventTime << ")\n";
        }
        // cout << "Total number of transactions: " << transactions.size() << "\n";
    }
    else {
        outfile << "\nExecuted transactions:\n";
        for (const auto& t : transactions) {
            outfile << "Trade: " << dec << t.shares << " @ " << t.price
                << " (BuyID=" << hex << t.buyOrderID
                << ", SellID=" << hex << t.sellOrderID
                << ", Time=" << dec << t.eventTime << ")\n";
        }
        // outfile << "Total number of transactions: " << transactions.size() << "\n";
    }
}

// -------- MATCHING ENGINE --------
vector<Transaction> Book::executeOrders(int currentTime) {
    vector<Transaction> trades;

    while (!buyTree.empty() && !sellTree.empty()) {
        Limit* bestBid = getBestBid();
        Limit* bestAsk = getBestAsk();

        if (!bestBid || !bestAsk) break;

        if (bestBid->getPrice() < bestAsk->getPrice()) break;

        Order* buyOrder = bestBid->getHeadOrder();
        Order* sellOrder = bestAsk->getHeadOrder();
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

        if (buyOrder->shares == 0) {
            bestBid->removeOrder(buyOrder);
            // delete buyOrder;
            if (!bestBid->getHeadOrder()) {
                buyTree.erase(bestBid->getPrice());
                // delete bestBid;
            }
        }

        if (sellOrder->shares == 0) {
            bestAsk->removeOrder(sellOrder);
            // delete sellOrder;
            if (!bestAsk->getHeadOrder()) {
                sellTree.erase(bestAsk->getPrice());
                // delete bestAsk;
            }
        }
    }

    AddTransactions(trades);
    return trades;
}

// -------- PRIVATE HELPERS --------
void Book::insertIntoBuyTree(map<int, Limit*, greater<int>>& tree, Order* order) {
    int price = order->price;
    if (tree.find(price) == tree.end()) {
        tree[price] = new Limit(price);
    }
    tree[price]->addOrder(order);
}

void Book::insertIntoSellTree(map<int, Limit*, less<int>>& tree, Order* order) {
    int price = order->price;
    if (tree.find(price) == tree.end()) {
        tree[price] = new Limit(price);
    }
    tree[price]->addOrder(order);
}

void Book::removeFromBuyTree(map<int, Limit*, greater<int>>& tree, Order* order) {
    int price = order->price;
    auto it = tree.find(price);
    if (it != tree.end()) {
        it->second->removeOrder(order);
        if (!it->second->getHeadOrder()) {
            delete it->second;
            tree.erase(it);
        }
    }
}

void Book::removeFromSellTree(map<int, Limit*, less<int>>& tree, Order* order) {
    int price = order->price;
    auto it = tree.find(price);
    if (it != tree.end()) {
        it->second->removeOrder(order);
        if (!it->second->getHeadOrder()) {
            delete it->second;
            tree.erase(it);
        }
    }
}

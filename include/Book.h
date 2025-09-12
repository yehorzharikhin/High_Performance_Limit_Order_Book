#ifndef BOOK_H
#define BOOK_H

#include "Limit.h"
#include "Transaction.h"
#include <map>
#include <vector>
#include <random>
using namespace std;

class Book {
public:
    Book();

    // Insert a new order into the book
    void addOrder(Order* order);

    // Cancel an existing order
    void cancelOrder(Order* order);

    // Accessors for best bid/ask
    Limit* getBestBid() const;
    Limit* getBestAsk() const;

    // Creating random IDs
    long long generateOrderID();

    // Add new transactions 
    void AddTransactions(vector<Transaction> new_transactions);

    // Print transactions
    void PrintTransactions(ostream &outfile);

    // Match and execute orders (main matching engine)
    vector<Transaction> executeOrders(int currentTime);

private:
    mt19937_64 rng;
    uniform_int_distribution<long long> dist;

    // Use ordered maps for price trees
    map<int, Limit*, greater<int>> buyTree; // highest price first
    map<int, Limit*, less<int>> sellTree;   // lowest price first

    vector<Transaction> transactions;

    void insertIntoBuyTree(map<int, Limit*, greater<int>>& tree, Order* order);
    void insertIntoSellTree(map<int, Limit*, less<int>>& tree, Order* order);
    void removeFromBuyTree(map<int, Limit*, greater<int>>& tree, Order* order);
    void removeFromSellTree(map<int, Limit*, less<int>>& tree, Order* order);
};

#endif

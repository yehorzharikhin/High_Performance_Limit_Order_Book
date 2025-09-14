#ifndef BOOK_H
#define BOOK_H

#include <map>
#include <vector>
#include <random>
#include "OrderPool.h"
#include "Transaction.h"
#include "Limit.h"
using namespace std;

class Book {
public:
    // Use ordered maps for price trees
    vector<Limit> buyLimits;
    vector<Limit> sellLimits;
    vector<Transaction> transactions;
    map<int, bool, greater<int>> buyLimitsmp;
    map<int, bool> sellLimitsmp;
    OrderPool pool;

    int N; // number of test orders
    int MAX_PRICE;

    int size;
    int max_size;

    int bestBid;
    int bestAsk;

    Book(const int n, const int max_p);

    // Creating random IDs
    int generateOrderID();

    // Accessors for best bid/ask
    Limit* getBestBid();
    Limit* getBestAsk();

    // Insert a new order into the book
    void addOrder(Order* order);

    // Cancel an existing order
    void cancelOrder(Order* order);

    // Add new transactions 
    void AddTransactions(vector<Transaction> new_transactions);

    // Print transactions
    void PrintTransactions(ostream &outfile);

    // Match and execute orders (main matching engine)
    vector<Transaction> executeOrders(int currentTime);

    // Debug
    void PrintPool(ostream &outfile = cout);

private:
    mt19937_64 rng;
    uniform_int_distribution<int> dist;

    void insertIntoBuy(Order* order);
    void insertIntoSell(Order* order);
    void removeFromBuy(Order* order);
    void removeFromSell(Order* order);
};

#endif

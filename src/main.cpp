#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include "Book.h"
#include "Order.h"
#include "Transaction.h"

using namespace std;
using namespace std::chrono;

int main() {
    // === Configuration ===
    const int N = 10000000; // number of test orders
    const int MAX_PRICE = 100000;
    const int MAX_SIZE = 1000;

    ofstream outfile("..\\data\\output.txt");

    Book lob;
    vector<Order> orders(N);
    map<long long, Order*> idToOrder;
    outfile << "=== Limit Order Book Performance Test ===\n";

    // Random generator
    random_device rd;
    mt19937 gen(rd());
    // mt19937 gen(123);
    uniform_int_distribution<int> priceDist(1, MAX_PRICE);
    uniform_int_distribution<int> sizeDist(1, MAX_SIZE);
    uniform_int_distribution<int> sideDist(0, 1), typeDist(0, 1); // 0 = BUY, 1 = SELL

    // Latency data
    // vector<pair<long long, int>> latencies(N);
    vector<long long> latencies(N);

    auto start_all = high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        // int orderType = typeDist(gen);
        // Random order fields
        long long id = lob.generateOrderID();
        OrderSide side = sideDist(gen) ? OrderSide::BUY : OrderSide::SELL;
        int shares = sizeDist(gen);
        int price = priceDist(gen);

        Order order(id, side, shares, price, i);
        orders[i] = order;

        auto start = high_resolution_clock::now();
        lob.addOrder(&orders[i]);
        auto end = high_resolution_clock::now();

        long long latency = duration_cast<nanoseconds>(end - start).count();
        latencies.push_back(latency);

        start = high_resolution_clock::now();
        lob.executeOrders(i++);
        end = high_resolution_clock::now();

        latency = duration_cast<nanoseconds>(end - start).count();
        latencies.push_back(latency);
    }

    auto end_all = high_resolution_clock::now();
    // long long total_time = duration_cast<nanoseconds>(end_all - start_all).count();
    long long total_time = 0;
    for (auto i : latencies)
        total_time += i;

    // === Stats ===
    long long min_latency = *min_element(latencies.begin(), latencies.end());
    long long max_latency = *max_element(latencies.begin(), latencies.end());
    double avg_latency = (double)accumulate(latencies.begin(), latencies.end(), 0LL) / N;
    double throughput = (double)N / (total_time / 1000000000.0); // orders per second

    // === Output results ===
    outfile << "Total orders processed: " << N << "\n";
    outfile << "Total time: " << total_time / 1000000 << " ms\n";
    outfile << "Throughput: " << throughput << " orders/sec\n";
    outfile << "Latency (ns): min=" << min_latency
            << " avg=" << avg_latency
            << " max=" << max_latency << "\n";

    // lob.PrintTransactions(outfile);

    outfile << endl;
    outfile.close();
    return 0;
}

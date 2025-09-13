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

// === Configuration ===
const int N = 10000010; // number of test orders
const int MAX_PRICE = 100000;
const int MAX_SIZE = 10000;

// Random generator
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> priceDist(1, MAX_PRICE);
uniform_int_distribution<int> sizeDist(1, MAX_SIZE), size2Dist(1, 10000);
uniform_int_distribution<int> sideDist(0, 1), typeDist(0, 100); // 0 = BUY, 1 = SELL

Order CreateRandomOrder(int i, OrderPool* parentPool) {
    OrderSide side = sideDist(gen) ? OrderSide::BUY : OrderSide::SELL;
    int shares = sizeDist(gen);
    int price = priceDist(gen);

    Order order(-1, side, shares, price, i, parentPool);
    return order;
}

int main() {
    ofstream outfile("..\\data\\output.txt");

    Book lob(N, MAX_PRICE);
    outfile << "=== Limit Order Book Performance Test ===\n";

    // Initial orderbook population
    // for (int i = 0; i < 10 * MAX_SIZE; i++) {
    //     Order order = CreateRandomOrder(i, &lob.pool);
    //     lob.addOrder(&order);
    // }
    // lob.executeOrders(-1);

    // Latency data
    vector<long long> latencies, cancelLatencies, addLatencies, executeLatencies;

    auto start_all = high_resolution_clock::now();

    for (int i = 0; i < N - 10; i++) {
        int type = typeDist(gen);
        // cout << "   " << type << endl;
        if (type < 60 || lob.size == 0) {
            Order order = CreateRandomOrder(i, &lob.pool);

            auto start = high_resolution_clock::now();
            lob.addOrder(&order);
            auto end = high_resolution_clock::now();

            long long latency = duration_cast<nanoseconds>(end - start).count();
            latencies.push_back(latency);
            addLatencies.push_back(latency);

            if (i % 10 == 9) {
                start = high_resolution_clock::now();
                lob.executeOrders(i++);
                end = high_resolution_clock::now();

                latency = duration_cast<nanoseconds>(end - start).count();
                latencies.push_back(latency);
                executeLatencies.push_back(latency);
            }
        }
        else {
            uniform_int_distribution<int> size3Dist(0, max(0, min(lob.size + 1000, int(lob.pool.pool.size() - 1))));
            int idx = size3Dist(gen), j = 0;
            while (lob.size > 0 && !lob.pool.freeArr[idx]) {
                idx = size3Dist(gen);
                if (++j > 99) {
                    j = -1;
                    break;
                }
            }
            if (j == -1) 
                continue;

            auto start = high_resolution_clock::now();
            lob.cancelOrder(&lob.pool.pool[idx]);
            auto end = high_resolution_clock::now();

            long long latency = duration_cast<nanoseconds>(end - start).count();
            latencies.push_back(latency);
            cancelLatencies.push_back(latency);
        }
    }

    auto end_all = high_resolution_clock::now();
    long long total_time = 0;
    for (auto i : latencies)
        total_time += i;

    // === Stats ===
    long long min_latency = *min_element(latencies.begin(), latencies.end());
    long long max_latency = *max_element(latencies.begin(), latencies.end());
    double avgLatency = (double)accumulate(latencies.begin(), latencies.end(), 0LL) / N;
    double avgAddLatency = (double)accumulate(addLatencies.begin(), addLatencies.end(), 0LL) / addLatencies.size();
    double avgCancelLatency = (double)accumulate(cancelLatencies.begin(), cancelLatencies.end(), 0LL) / cancelLatencies.size();
    double avgExecuteLatency = (double)accumulate(executeLatencies.begin(), executeLatencies.end(), 0LL) / executeLatencies.size();
    double throughput = (double)N / (total_time / 1000000000.0); // orders per second

    // === Output results ===
    outfile << "Total orders processed: " << N << "\n";
    outfile << "Total time: " << total_time / 1000000 << " ms\n";
    outfile << "Throughput: " << throughput << " orders/sec\n";
    outfile << "Latency (ns): min=" << min_latency
            << " avg=" << avgLatency
            << " max=" << max_latency << "\n";
    outfile << "Add latency (ns): avg=" << avgAddLatency << " %=" << 1.0 * addLatencies.size() / latencies.size() << "\n";
    outfile << "Cancel latency (ns): avg=" << avgCancelLatency << " %=" << 1.0 * cancelLatencies.size() / latencies.size() << "\n";
    outfile << "Execute latency (ns): avg=" << avgExecuteLatency << " %=" << 1.0 * executeLatencies.size() / latencies.size() << "\n";

    lob.PrintTransactions(outfile);

    outfile << endl;
    outfile.close();
    cout << lob.max_size << endl;
    return 0;
}

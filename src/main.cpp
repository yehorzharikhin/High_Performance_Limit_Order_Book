#include "LimitOrderBook.h"
using namespace std;

int main() {
    LimitOrderBook book;
    book.addOrder({1, 100.5, 10, true});
    book.addOrder({2, 101.0, 5, false});
    book.printBook();
    return 0;
}

#include <map>
#include <string>
#include <ostream>
#include <iostream>

#include "Order.h"

class OrderBook {
private:
    std::map<decltype(BuyOrder::price), OrderContainer> book;

public:
    using OrderIter = OrderContainer::iterator;

    // Buy or Sell
    OrderNode setOrder(BuyOrder& order);
    void cancelOrder(const OrderNode& node);
    void logOrder(std::ostream& logger, const char& delimiter,
                  const Price& price, const Quantity& quantity);
    void printBook(std::ostream& logger, const char& delimiter);
    BuyOrder& peekHigh();
    BuyOrder& peekLow();
    void popLow();
    void popHigh();
    bool empty();
};
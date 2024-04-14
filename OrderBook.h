#include <map>
#include <string>
#include <ostream>
#include <iostream>

#include "Order.h"

class OrderBook {
private:
    //                                            Head      , Tail
    std::map<decltype(BuyOrder::price), std::pair<OrderNode*, OrderNode*>> book;

public:
    ~OrderBook();

    // Buy or Sell
    OrderNode* setOrder(BuyOrder& order);
    void cancelOrder(OrderNode& node);
    void logOrder(std::ostream& logger, const char& delimiter,
                  Price& price, Quantity& quantity);
    void printBook(std::ostream& logger, const char& delimiter);
    BuyOrder& peekHigh();
    BuyOrder& peekLow();
    void popLow();
    void popHigh();
    bool empty();
};
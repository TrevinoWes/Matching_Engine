#include "OrderBook.h"

// Buy or Sell
OrderNode OrderBook::setOrder(BuyOrder& order) {
    book[order.price].push_back(std::move(order));
    
    // order is now invalid
    OrderNode node(--book[order.price].end(), book[order.price].back());
    
    return node;
}

void OrderBook::cancelOrder(const OrderNode& node) {
    book[node.price].erase(node.iter);    
    if(book[node.price].empty()) {
        book.erase(node.price);
    }
}

void OrderBook::logOrder(std::ostream& logger, const char& delimiter,
                const Price& price, const Quantity& quantity) {
    logger << price << delimiter << quantity << std::endl;
}

// !!! keep running record of order counts?
void OrderBook::printBook(std::ostream& logger, const char& delimiter) {
    Price curPrice = 0;
    Quantity orderCount = 0;
    OrderNode* curOrder = nullptr;
    for(auto priceIter = book.rbegin(); priceIter != book.rend(); ++priceIter) {
        orderCount = 0;
        for(auto order: priceIter->second) {
            orderCount += order.quantity;
        }
        logOrder(logger, delimiter, priceIter->first, orderCount);
    }
}

BuyOrder& OrderBook::peekHigh(){
    return book.rbegin()->second.front();
}

BuyOrder& OrderBook::peekLow(){
    return book.begin()->second.front();
}

void OrderBook::popLow(){
    if(empty()) {
        return;
    }

    auto node = book.begin();
    node->second.pop_front();    
    if(node->second.empty()) {
        book.erase(node);
    }
}

void OrderBook::popHigh() {
    if(empty()) {
        return;
    }
    
    auto node = book.rbegin();
    node->second.pop_front();
    if(node->second.empty()) {
        book.erase(--book.end());
    }
}

bool OrderBook::empty() {
    return book.empty();
}

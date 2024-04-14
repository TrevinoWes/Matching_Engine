#include "OrderBook.h"

OrderBook::~OrderBook() {
    OrderNode* node;
    OrderNode* next;
    for(auto& priceList: book) {
        node = priceList.second.first;
        while(node != nullptr) {
            next = node->next;
            delete node;
            node = next;
        }
    }
}

// Buy or Sell
OrderNode* OrderBook::setOrder(BuyOrder& order) {
    auto iter = book.find(order.price);
    if(iter == book.end()) {
        OrderNode* newNode = new OrderNode(std::move(order));
        book[order.price].first = newNode; // !!! move?
        book[order.price].second = newNode;
    } else {
        OrderNode* newNode = new OrderNode(std::move(order)); // !!! move?
        //OrderNode* tail = iter->second.second;
        iter->second.second->next = newNode;
        iter->second.second->next->prev = iter->second.second;
        iter->second.second = iter->second.second->next;
    }
    
    return book[order.price].second;
}

void OrderBook::cancelOrder(OrderNode& node) {
    auto order = node.order;
    // check head
    if(node.prev != nullptr) {
        node.prev->next = node.next;
    } else {
        book[order.price].first = node.next;
    }
    
    // check tail
    if(node.next != nullptr) {
        node.next->prev = node.prev;
    } else {
        book[order.price].second = node.prev;

    }
    
    if(book[order.price].first == nullptr) {
        book.erase(order.price);
    }
    
    delete &node;
}

void OrderBook::logOrder(std::ostream& logger, const char& delimiter,
                Price& price, Quantity& quantity) {
    logger << price << delimiter << quantity << std::endl;
}

void OrderBook::printBook(std::ostream& logger, const char& delimiter) {
    Price curPrice = 0;
    Quantity orderCount = 0;
    OrderNode* curOrder = nullptr;
    for(auto priceIter = book.rbegin(); priceIter != book.rend(); ++priceIter) {
        curOrder = priceIter->second.first; // head of list
        curPrice = curOrder->order.price;
        orderCount = 0;
        while(curOrder != nullptr) {
            orderCount += curOrder->order.quantity;
            curOrder = curOrder->next;
        }
        logOrder(logger, delimiter, curPrice, orderCount);
    }
}

BuyOrder& OrderBook::peekHigh(){
    return book.rbegin()->second.first->order;
}

BuyOrder& OrderBook::peekLow(){
    return book.begin()->second.first->order;
}

void OrderBook::popLow(){
    if(empty()) {
        return;
    }
    OrderNode* node = book.begin()->second.first;
    book.begin()->second.first = book.begin()->second.first->next;
    if(book.begin()->second.first == nullptr) {
        book.erase(book.begin());
    } else {
        book.begin()->second.first->prev = nullptr;
    }
    delete node;
}

void OrderBook::popHigh() {
    if(empty()) {
        return;
    }
    OrderNode* node = book.rbegin()->second.first;
    book.rbegin()->second.first = book.rbegin()->second.first->next;
    if(book.rbegin()->second.first == nullptr) {
        book.erase(--book.end());
    } else {
        book.rbegin()->second.first->prev = nullptr;
    }
    delete node;
}

bool OrderBook::empty() {
    return book.empty();
}

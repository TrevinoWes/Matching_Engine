#include "MatchingEngine.h"

bool MatchingEngine::getNodeByHash(BaseOrder& matchedOrder, HashToNodeMapIter& listIter) {
    listIter = hashToNodeMap.find(matchedOrder.hashedId);
    for(;listIter != hashToNodeMap.end(); ++listIter) {
        if(listIter->first != matchedOrder.hashedId) {
            listIter = hashToNodeMap.end();
            break;
        }
        
        // !!! intermitent crashing in stored order object ID
        // not able to consistently reproduce, would normally
        // use GDB to show input but my assumption is that I am corrupting
        // data somehow         
        if(listIter->second->order.orderId == matchedOrder.orderId) {
            break;
        }
        
    }
    
    if(listIter == hashToNodeMap.end()) {
        return false;
    }
    
    return true;
}
    
void MatchingEngine::printOrders() {
    logger << SELL_TEXT << ":\n";
    sellBook.printBook(logger, DEL_TEXT);
    logger << BUY_TEXT << ":\n";
    buyBook.printBook(logger, DEL_TEXT);
}

void MatchingEngine::modifyOrder(ModifyOrder& modOrder) {
    {
        HashToNodeMapIter iter;
        if(!getNodeByHash(modOrder, iter)) {
            return;
        }
    }
    
    cancelOrder(modOrder);
    
    // Need to dynamic determine type when
    // BuyOrder no longer == SellOrder
    BuyOrder order;
    order.operation = modOrder.newOperation;
    order.price = modOrder.price;
    order.quantity = modOrder.quantity;
    order.orderId = modOrder.orderId;
    order.hashedId = modOrder.hashedId;
    order.orderType = OrderType::GFD;
    
    processOrder(order);
}

void MatchingEngine::processOrder(BuyOrder& order) {
    {
        HashToNodeMapIter iter;
        if(getNodeByHash(order, iter)) {
            return;
        }
    }

    checkPriceCross(order);

    if(order.quantity <= 0) {
        return;
    }

    OrderNode* nodePtr;
    if(OrderType::GFD == order.orderType) {
        if(Operation::Buy == order.operation) {
            nodePtr = buyBook.setOrder(order);
        } else {
            nodePtr = sellBook.setOrder(order);
        }
    } else if(OrderType::IOC == order.orderType) {
        // We don't store IOC orders
    } else{
        // log 
        return;
    }
    hashToNodeMap.insert({order.hashedId, nodePtr});
}

void MatchineEngine::cancelOrder(CancelOrder& order) {
    HashToNodeMapIter iter;
    if(!getNodeByHash(order, iter)) {
        return;
    }
    
    OrderNode* nodePtr = iter->second;
    hashToNodeMap.erase(iter);
    if(Operation::Buy == nodePtr->order.operation) {
        buyBook.cancelOrder(*nodePtr);
    } else {
        sellBook.cancelOrder(*nodePtr);
    }
}

void MatchingEngine::logTrade(const Quantity& tradeQuantity,
                              const BuyOrder& firstOrder,
                              const BuyOrder& secondOrder) {
    logger << TRADE_TEXT << DEL_TEXT << firstOrder.orderId << DEL_TEXT;
    logger << firstOrder.price << DEL_TEXT << tradeQuantity;
    logger << DEL_TEXT << secondOrder.orderId << DEL_TEXT;
    logger << secondOrder.price << DEL_TEXT << tradeQuantity << std::endl;
}

void MatchingEngine::tradeOrder(BuyOrder& incomingOrder, BuyOrder& bookOrder){
    if(incomingOrder.quantity > bookOrder.quantity) {
        logTrade(bookOrder.quantity, bookOrder, incomingOrder);
        incomingOrder.quantity -= bookOrder.quantity;
        bookOrder.quantity = 0;
    } else if (incomingOrder.quantity < bookOrder.quantity) {
        logTrade(incomingOrder.quantity, bookOrder, incomingOrder);
        bookOrder.quantity -= incomingOrder.quantity;
        incomingOrder.quantity = 0;
    } else {
        logTrade(incomingOrder.quantity, bookOrder, incomingOrder);
        incomingOrder.quantity = 0;
        bookOrder.quantity = 0;
    }
}

void MatchingEngine::removeHashFromMap(BuyOrder& order) {
    HashToNodeMapIter iter;
    if(!getNodeByHash(order, iter)) {
        return;
    }
    hashToNodeMap.erase(iter);
}

void MatchingEngine::checkSellPriceCross(BuyOrder& incomingOrder){
    BuyOrder* sellOrder;
    while(incomingOrder.quantity > 0 &&
            !sellBook.empty() &&
            incomingOrder.price >= sellBook.peekLow().price) {
        sellOrder = &sellBook.peekLow();
        tradeOrder(incomingOrder, *sellOrder);
        if(sellOrder->quantity == 0) {
            removeHashFromMap(*sellOrder);
            sellBook.popLow();
        }
    }
}

void MatchingEngine::checkBuyPriceCross(BuyOrder& incomingOrder) {
    BuyOrder* buyOrder;
    while(incomingOrder.quantity > 0 &&
            !buyBook.empty() &&
            buyBook.peekHigh().price >= incomingOrder.price) {
        buyOrder = &buyBook.peekHigh();
        tradeOrder(incomingOrder, *buyOrder);
        if(buyOrder->quantity == 0) {
            removeHashFromMap(*buyOrder);
            buyBook.popHigh();
        }
    }

}


void MatchingEngine::checkPriceCross(BuyOrder& order) {    
    if(Operation::Buy == order.operation) {
        checkSellPriceCross(order);
    } else if(Operation::Sell == order.operation) {
        checkBuyPriceCross(order);
    } else {
        // log error
    }
}  

void MatchingEngine::processXchgMsg(const std::string& exchangeOrder) {
    BaseOrder* orderPtr;
    parser.parseXchgMsg(exchangeOrder, orderPtr);
    // Check for parsing errors
    if(orderPtr->operation == Operation::Error) {
        return;
    }
    
    switch(orderPtr->operation) {
        case Operation::Buy:
        case Operation::Sell:
            {
                BuyOrder* order = static_cast<BuyOrder*>(orderPtr);
                processOrder(*order);
                delete order;
            }
            break;
        case Operation::Modify:
            {
                ModifyOrder* order = static_cast<ModifyOrder*>(orderPtr);
                modifyOrder(*order);
                delete order;
            }
            break;
        case Operation::Cancel:
            {
                CancelOrder* order = static_cast<CancelOrder*>(orderPtr);
                cancelOrder(*order);
                delete order;
            }
            break;
        case Operation::Print:
            {
                PrintOrder* order = static_cast<PrintOrder*>(orderPtr);
                printOrders();
                delete order;
            }
            break;
        default:
            // log
            break;
    }
    // orderPtr no longer valid after this point
}
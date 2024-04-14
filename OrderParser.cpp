#include <memory>

#include "OrderParser.h"

void OrderParser::parseNextVal(std::string::const_iterator& iter,
                               const std::string::const_iterator& endIter,
                               std::string& orderVal) {
    orderVal.clear();

    while(iter != endIter && *iter != delimiter) {
        orderVal.push_back(*iter);
        ++iter;
    }
    if(iter != endIter) {
        ++iter; // Guaranteed single space delimiter, this is flimsy
    }
}

bool OrderParser::convertPrice(const std::string& orderVal,
                               Price& price) {
    try {
        price = std::stoll(orderVal);
    } catch(const std::exception& e) {
        // Should log
        return false;
    }
    
    return true;
}

bool OrderParser::convertQuantity(const std::string& orderVal,
                                  Price& quantity) {
    try {
        quantity = std::stoll(orderVal);
    } catch(const std::exception& e) {
        // Should log
        return false;
    }
    return true;
}

bool OrderParser::convertOrderType(const std::string& strType, OrderType& type) {
    auto oType = strToOrderTypeMap.find(strType);
    if(oType == strToOrderTypeMap.end()) {
        return false;
    }
    type = oType->second;
    return true;         
}

bool OrderParser::convertOperation(const std::string& strOp, Operation& operation) {
    auto op = strToOperationMap.find(strOp);
    if(op == strToOperationMap.end()) {
        return false;
    }
    operation = op->second;
    return true;         
}

void OrderParser::parseSellOrder(const std::string& exchangeOrder,
                                 std::string::const_iterator& iter,
                                 BaseOrder*& orderPtr) {
                
    orderPtr = new SellOrder();
    SellOrder* sellOrderPtr = static_cast<SellOrder*>(orderPtr);
    sellOrderPtr->operation = Operation::Sell;

    std::string orderVal;
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    
    if(!convertOrderType(orderVal, sellOrderPtr->orderType)) {
        sellOrderPtr->operation = Operation::Error;
        return;
    }
    orderVal.clear(); 
    
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    
    if(!convertPrice(orderVal, sellOrderPtr->price)) {
        sellOrderPtr->operation = Operation::Error;
        return;
    }
    
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    
    if(!convertQuantity(orderVal, sellOrderPtr->quantity)) {
        sellOrderPtr->operation = Operation::Error;
        return;
    }
    
    parseNextVal(iter, exchangeOrder.end(), sellOrderPtr->orderId);
    
    sellOrderPtr->hashedId = std::hash<std::string>{}(sellOrderPtr->orderId);
    
    if(!validOrder(*sellOrderPtr)) {
        sellOrderPtr->operation = Operation::Error;
        return;
    }
    
}

void OrderParser::parseBuyOrder(const std::string& exchangeOrder,
                                std::string::const_iterator& iter,
                                BaseOrder*& orderPtr) {
                
    orderPtr = new BuyOrder();
    BuyOrder* buyOrderPtr = static_cast<BuyOrder*>(orderPtr);
    buyOrderPtr->operation = Operation::Buy;

    std::string orderVal;
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    
    if(!convertOrderType(orderVal, buyOrderPtr->orderType)) {
        buyOrderPtr->operation = Operation::Error;
        return;
    }
    orderVal.clear();
    
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    
    if(!convertPrice(orderVal, buyOrderPtr->price)) {
        buyOrderPtr->operation = Operation::Error;
        return;
    }
    
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    
    if(!convertQuantity(orderVal, buyOrderPtr->quantity)) {
        buyOrderPtr->operation = Operation::Error;
        return;
    }
    
    parseNextVal(iter, exchangeOrder.end(), buyOrderPtr->orderId);
    
    buyOrderPtr->hashedId = std::hash<std::string>{}(buyOrderPtr->orderId);
    
    if(!validOrder(*buyOrderPtr)) {
        buyOrderPtr->operation = Operation::Error;
        return;
    }  
}

void OrderParser::parseModOrder(const std::string& exchangeOrder,
                                std::string::const_iterator& iter,
                                BaseOrder*& orderPtr) {

    orderPtr = new ModifyOrder();
    ModifyOrder* modOrderPtr = static_cast<ModifyOrder*>(orderPtr);
    modOrderPtr->operation = Operation::Modify;
                        
    parseNextVal(iter, exchangeOrder.end(), modOrderPtr->orderId);
    modOrderPtr->hashedId = std::hash<std::string>{}(modOrderPtr->orderId);
    
    std::string orderVal;
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    
    if(!convertOperation(orderVal, modOrderPtr->newOperation)) {
        modOrderPtr->operation = Operation::Error;
        return;
    }
    orderVal.clear(); 
    
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    if(!convertPrice(orderVal, modOrderPtr->price)) {
        modOrderPtr->operation = Operation::Error;
        return;
    }
    
    parseNextVal(iter, exchangeOrder.end(), orderVal);
    if(!convertQuantity(orderVal, modOrderPtr->quantity)) {
        modOrderPtr->operation = Operation::Error;
        return;
    }
    
    if(!validOrder(*modOrderPtr)) {
        modOrderPtr->operation = Operation::Error;
        return;
    }
}

void OrderParser::parseCancelOrder(const std::string& exchangeOrder,
                                   std::string::const_iterator& iter,
                                   BaseOrder*& orderPtr) {
    orderPtr = new CancelOrder();
    CancelOrder* canOrderPtr = static_cast<CancelOrder*>(orderPtr);
    canOrderPtr->operation = Operation::Cancel;              
            
    parseNextVal(iter, exchangeOrder.end(), canOrderPtr->orderId);
    canOrderPtr->hashedId = std::hash<std::string>{}(canOrderPtr->orderId);
    if(!validBase(*canOrderPtr)) {
        canOrderPtr->operation = Operation::Error;
        return;
    }
}

void OrderParser::parsePrintOrder(const std::string& exchangeOrder,
                                  std::string::const_iterator& iter,
                                  BaseOrder*& orderPtr){
    orderPtr = new PrintOrder();
    PrintOrder* printOrderPtr = static_cast<PrintOrder*>(orderPtr);
    printOrderPtr->operation = Operation::Print;
    
    printOrderPtr->orderId = "0"; // Dummy value for validation
    printOrderPtr->hashedId = std::hash<std::string>{}(printOrderPtr->orderId);
    
    if(!validBase(*printOrderPtr)) {
        printOrderPtr->operation = Operation::Error;
        return;
    }         
}

bool OrderParser::validOrder(const BuyOrder& order) {
    if(!validBase(order)) {
        return false;
    }
    
    if(order.orderType == OrderType::Undefined ||
        order.price <= 0                        ||
        order.quantity <= 0) {
        return false;
    }
    
    return true;
}

bool OrderParser::validOrder(const ModifyOrder& order) {
    if(!validBase(order)) {
        return false;
    }
    
    if(order.price <= 0    ||
        order.quantity <= 0) {
        return false;
    }
    
    if(order.newOperation != Operation::Buy &&
        order.newOperation != Operation::Sell) {
        return false;       
    }
    
    return true;
}

bool OrderParser::validOrder(const CancelOrder& order) {
    if(!validBase(order)) {
        return false;
    }
    
    return true;
}

bool OrderParser::validBase(const BaseOrder& order) {
    // !!! verify hash id?
    if(order.operation == Operation::Error ||
        order.orderId.empty()) {
        return false;
    }
    
    for(auto& c: order.orderId) {
        if(!std::isprint(c)) {
            return false;
        }
    }
    
    return true;
}                                                 

// Any parsing or validation errors returns Operation::Error
void OrderParser::parseXchgMsg(const std::string& exchangeOrder, BaseOrder*& order) {
    auto orderIter = exchangeOrder.begin();
    std::string operation;

    while(orderIter != exchangeOrder.end() && *orderIter == delimiter){
        ++orderIter;
    }
    
    parseNextVal(orderIter, exchangeOrder.end(), operation);

    Operation op;
    if(!convertOperation(operation, op)) {
        return;
    }

    
    switch(op) {
        case Operation::Buy:
            parseBuyOrder(exchangeOrder, orderIter, order);
            break;
        case Operation::Sell:
            parseSellOrder(exchangeOrder, orderIter, order);
            break;
        case Operation::Print:
            parsePrintOrder(exchangeOrder, orderIter, order);
            break;
        case Operation::Modify:
            parseModOrder(exchangeOrder, orderIter, order);
            break;
        case Operation::Cancel:
            parseCancelOrder(exchangeOrder, orderIter, order);
            break;
        default:
            //log an error
            break;
    }
}
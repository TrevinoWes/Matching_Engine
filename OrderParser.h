#include <map>
#include <string>

#include "Order.h"

// Only supports single character delimiters
class OrderParser {
private:
    std::map<std::string, Operation> strToOperationMap {{"BUY", Operation::Buy},
                                                        {"SELL", Operation::Sell},
                                                        {"CANCEL", Operation::Cancel},
                                                        {"MODIFY", Operation::Modify},
                                                        {"PRINT", Operation::Print}};
    std::map<std::string, OrderType> strToOrderTypeMap {{"IOC", OrderType::IOC},
                                                        {"GFD", OrderType::GFD}};
    const char delimiter;
    
    void parseNextVal(std::string::const_iterator& iter,
                      const std::string::const_iterator& endIter,
                      std::string& orderVal);
    bool convertPrice(const std::string& orderVal,
                      Price& price);
    bool convertQuantity(const std::string& orderVal,
                         Price& quantity);
    bool convertOrderType(const std::string& strType, OrderType& type);
    bool convertOperation(const std::string& strOp, Operation& operation);
    void parseSellOrder(const std::string& exchangeOrder,
                        std::string::const_iterator& iter,
                        BaseOrder*& orderPtr);
    void parseBuyOrder(const std::string& exchangeOrder,
                      std::string::const_iterator& iter,
                      BaseOrder*& orderPtr);
    void parseModOrder(const std::string& exchangeOrder,
                       std::string::const_iterator& iter,
                       BaseOrder*& orderPtr);
    void parseCancelOrder(const std::string& exchangeOrder,
                          std::string::const_iterator& iter,
                          BaseOrder*& orderPtr);
    void parsePrintOrder(const std::string& exchangeOrder,
                         std::string::const_iterator& iter,
                         BaseOrder*& orderPtr);
    bool validOrder(const BuyOrder& order);
    bool validOrder(const ModifyOrder& order);
    bool validOrder(const CancelOrder& order);
    bool validBase(const BaseOrder& order);
    
public:
    OrderParser(char& del): delimiter(del){}; 

    // Any parsing or validation errors returns Operation::Error
    void parseXchgMsg(const std::string& exchangeOrder, BaseOrder*& order);
};
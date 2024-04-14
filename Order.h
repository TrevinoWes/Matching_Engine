#include <string>

enum class Operation {
    Error,
    Buy,
    Sell,
    Modify,
    Cancel,
    Print
};


enum class OrderType {
    Undefined,
    IOC,
    GFD  
};


using Price = uint64_t;
using Quantity = uint64_t;

class BaseOrder {
public:
    Operation operation = Operation::Error;
    std::size_t hashedId;
    std::string orderId;
};

class BuyOrder: public BaseOrder{
public:
    OrderType orderType = OrderType::Undefined;
    Price price;
    Quantity quantity;
};

using SellOrder = BuyOrder;

class ModifyOrder: public BaseOrder{
public:
    Operation newOperation = Operation::Error;
    Price price;
    Quantity quantity;
};

using CancelOrder = BaseOrder;
using PrintOrder = BaseOrder;

class OrderNode {
public:
    BuyOrder order;
    OrderNode* next = nullptr;
    OrderNode* prev = nullptr;
    
    OrderNode(BuyOrder&& o): order(o) {};
};
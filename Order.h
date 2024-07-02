#pragma once

#include <string>
#include <list>

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

class BuyOrder;
using Price = uint64_t;
using Quantity = uint64_t;
using OrderContainer = std::list<BuyOrder>;

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
    Price price;
    OrderContainer::iterator iter;
    std::string orderId;
    
    OrderNode(const OrderContainer::iterator& i, const BuyOrder& bo): price(bo.price), iter(i), orderId(bo.orderId) {};
    OrderNode(OrderNode&& o) noexcept : price(std::move(o.price)), iter(std::move(o.iter)), orderId(std::move(o.orderId)) {};
};

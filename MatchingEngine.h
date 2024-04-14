#include <map>
#include <string>

#include "Order.h"
#include "OrderParser.h"
#include "OrderBook.h"

class MatchingEngine {
private:
    // std::hash doesn't guarantee a unique hash value
    std::multimap<size_t, OrderNode*> hashToNodeMap;
    OrderBook buyBook;
    OrderBook sellBook;
    OrderParser parser;
    inline static const std::string TRADE_TEXT = "TRADE";
    inline static const std::string SELL_TEXT = "SELL";
    inline static const std::string BUY_TEXT = "BUY";
    const char DEL_TEXT;
    std::ostream& logger = std::cout;
    
    using HashToNodeMapIter = decltype(hashToNodeMap)::const_iterator;
    
    bool getNodeByHash(BaseOrder& matchedOrder, HashToNodeMapIter& listIter);
    void printOrders();
    void modifyOrder(ModifyOrder& modOrder);
    void processOrder(BuyOrder& order);
    void cancelOrder(CancelOrder& order);
    void logTrade(const Quantity& tradeQuantity,
                  const BuyOrder& firstOrder,
                  const BuyOrder& secondOrder);
    void tradeOrder(BuyOrder& incomingOrder, BuyOrder& bookOrder);
    void removeHashFromMap(BuyOrder& order);
    void checkSellPriceCross(BuyOrder& incomingOrder);
    void checkBuyPriceCross(BuyOrder& incomingOrder);
    void checkPriceCross(BuyOrder& order);
public:
    MatchingEngine(char delimiter = ' '): DEL_TEXT(delimiter), parser(delimiter){}

    void processXchgMsg(const std::string& exchangeOrder);
};
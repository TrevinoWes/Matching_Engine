#include <string>
#include <iostream>

#include "MatchingEngine.h"

/*
    With the ability to benchmark I would test out 1 other
    design for my OrderBook objects.
    
    I would be interested in seeing if using a deque to store books
    using binary search and shift insert operation, a similar binary
    search and shift for cancel, a constant time pop from both ends,
    and a modify that just ran cancel then insert would be faster because of 
    compiler and cpu optimization, including automatic vectorization. This would
    depend on how many non-contiguous blocks of memory the deque uses but this tends
    to be faster than the constant pointer dereferencing for a linked-list.
    
    In my Matching engine I would then change the multimap to store the operation (buy/sell)
    and the orderid by hashedId. The operation would be used to specify which book to check and
    the orderid would be used to verify std::hash didn't produce a duplicate hash value.
    
    I also assume that this program isn't linked against the appropriate threading library.
    I was interested in trying to come up with a multithreaded solution using cache aligned
    order and atomic OrderBook operations. You would need to store the requests process time
    prioritizing Buy/Sell orders because you don't want to have a cancel or modify order skipped
    because the original order hadn't been processed. I believe this would only increase OrderBooks
    insert runtime complexity because you would need to insert anywhere in the book to maintain your
    price-time prioritization.
*/

/* Enhancement Notes:
1. Make Matching Engine logger configurable

2. Remove access to *Order class members
*/

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    MatchingEngine mEngine;
    std::string request;
    // read input from STDIN
    while(std::getline(std::cin, request)) {
        mEngine.processXchgMsg(request);
    }
    
    return 0;
}
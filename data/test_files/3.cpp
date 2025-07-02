// --- MessageParser.cpp ---
// A utility for processing a stream of network messages.
// It includes functions for filtering and sanitizing message batches.

#include <iostream>
#include <vector>
#include <string>

struct Message {
    int id;
    std::string payload;
    bool isTrivial;
};

class MessageProcessor {
public:
    // Removes all messages where isTrivial is true from the vector.
    // The goal is to modify the vector in-place.
    void filterTrivialMessages(std::vector<Message>& messages) {
        std::cout << "Starting filter. Initial size: " << messages.size() << std::endl;
        
        for (auto it = messages.begin(); it != messages.end(); ++it) {
            if (it->isTrivial) {
                // Erasing an element from a vector invalidates iterators
                // at or after the point of erasure.
                messages.erase(it);
            }
        }
        
        std::cout << "Filtering complete. Final size: " << messages.size() << std::endl;
    }
};

void runMessageProcessing() {
    MessageProcessor processor;
    std::vector<Message> batch = {
        {1, "DATA_PACKET", false},
        {2, "HEARTBEAT", true},    // This will be removed
        {3, "USER_LOGIN", false},
        {4, "PING", true},         // This may or may not be removed
        {5, "DATA_PACKET_2", false}
    };

    processor.filterTrivialMessages(batch);
}
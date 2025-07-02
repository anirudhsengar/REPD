// --- MessageParser_Fixed.cpp ---
// A utility for processing a stream of network messages.
// This version uses the standard, safe, and efficient erase-remove idiom
// to filter messages in-place.

#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // Required for std::remove_if

struct Message {
    int id;
    std::string payload;
    bool isTrivial;
};

class MessageProcessor {
public:
    // Removes all messages where isTrivial is true from the vector.
    // This is done safely using the erase-remove idiom.
    void filterTrivialMessages(std::vector<Message>& messages) {
        std::cout << "Starting filter. Initial size: " << messages.size() << std::endl;
        
        // The std::remove_if algorithm moves all elements that should NOT be removed
        // to the beginning of the range. It returns an iterator to the new "end"
        // of the valid elements. It does not actually change the container's size.
        auto new_end = std::remove_if(messages.begin(), messages.end(),
                                      [](const Message& msg) {
                                          return msg.isTrivial;
                                      });

        // The vector::erase method is then called once to erase all the elements
        // from the new logical end to the physical end of the vector.
        messages.erase(new_end, messages.end());
        
        std::cout << "Filtering complete. Final size: " << messages.size() << std::endl;
    }
};

void runMessageProcessingSafely() {
    MessageProcessor processor;
    std::vector<Message> batch = {
        {1, "DATA_PACKET", false},
        {2, "HEARTBEAT", true},    // Will be removed
        {3, "USER_LOGIN", false},
        {4, "PING", true},         // Will be removed
        {5, "DATA_PACKET_2", false}
    };

    processor.filterTrivialMessages(batch); // This works correctly and safely.
}
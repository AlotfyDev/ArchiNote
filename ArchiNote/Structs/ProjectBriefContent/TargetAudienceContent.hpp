#ifndef __STARGETAUDIENCE_HPP__
#define __STARGETAUDIENCE_HPP__






#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this
#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument




// Flat struct for TargetAudienceContent (no inheritance, pure data + methods)
struct TargetAudienceContent {
    std::string audienceSegment;  // e.g., "Developers"
    std::string needs;  // e.g., "Scalable tools"
    std::string characteristics;  // e.g., "Tech-savvy"

    // Static factory method (factory style)
    static TargetAudienceContent CreateTargetAudienceContent(const std::string& segment, const std::string& needs, const std::string& characteristics) {
        // Basic validation
        if (segment.empty() || needs.empty()) {
            throw std::invalid_argument("Invalid target audience parameters: segment or needs empty");
        }
        return TargetAudienceContent{segment, needs, characteristics};
    }

    // Constructor
    TargetAudienceContent(const std::string& segment, const std::string& needs, const std::string& characteristics)
        : audienceSegment(segment), needs(needs), characteristics(characteristics) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !audienceSegment.empty() && !needs.empty();
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"segment\": \"" + audienceSegment + "\", \"needs\": \"" + needs + "\", \"characteristics\": \"" + characteristics + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    TargetAudienceContent(const TargetAudienceContent& other) = default;
};










#endif // __STARGETAUDIENCE_HPP
#ifndef __SASSUMPTIONCONTENT_HPP__
#define __SASSUMPTIONCONTENT_HPP__





#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this

#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument



// Flat struct for AssumptionsContent (no inheritance, pure data + methods)
struct AssumptionsContent {
    std::string assumption;  // e.g., "Users have high-speed internet"
    std::string rationale;  // e.g., "Based on target market data"

    // Static factory method (factory style)
    static AssumptionsContent CreateAssumptionsContent(const std::string& assumption, const std::string& rationale) {
        // Basic validation
        if (assumption.empty() || rationale.empty()) {
            throw std::invalid_argument("Invalid assumptions parameters: assumption or rationale empty");
        }
        return AssumptionsContent{assumption, rationale};
    }

    // Constructor
    AssumptionsContent(const std::string& assumption, const std::string& rationale)
        : assumption(assumption), rationale(rationale) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !assumption.empty() && !rationale.empty();
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"assumption\": \"" + assumption + "\", \"rationale\": \"" + rationale + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    AssumptionsContent(const AssumptionsContent& other) = default;
};











#endif // __SASSUMPTIONCONTENT_HPP__
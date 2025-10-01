
#ifndef __SRISKCONTENT_HPP__
#define __SRISKCONTENT_HPP__






#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this

#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument




// Flat struct for RiskContent (no inheritance, pure data + methods)
struct RiskContent {
    std::string riskDescription;  // e.g., "Potential server downtime"
    std::string impact;  // e.g., "Service disruption"
    std::string mitigation;  // e.g., "Implement redundant servers"

    // Static factory method (factory style)
    static RiskContent CreateRiskContent(const std::string& riskDescription, const std::string& impact, const std::string& mitigation) {
        // Basic validation
        if (riskDescription.empty() || impact.empty()) {
            throw std::invalid_argument("Invalid risk parameters: riskDescription or impact empty");
        }
        return RiskContent{riskDescription, impact, mitigation};
    }

    // Constructor
    RiskContent(const std::string& riskDescription, const std::string& impact, const std::string& mitigation)
        : riskDescription(riskDescription), impact(impact), mitigation(mitigation) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !riskDescription.empty() && !impact.empty();
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"riskDescription\": \"" + riskDescription + "\", \"impact\": \"" + impact + "\", \"mitigation\": \"" + mitigation + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    RiskContent(const RiskContent& other) = default;
};










#endif // __SRISKCONTENT_HPP__
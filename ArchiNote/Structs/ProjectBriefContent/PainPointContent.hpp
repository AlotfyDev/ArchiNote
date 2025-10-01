#ifndef __SPAINPOINTCONTENT_HPP__
#define __SPAINPOINTCONTENT_HPP__







#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this
#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument






// Flat struct for PainPointsContent (no inheritance, pure data + methods)
struct PainPointContent {
    std::string issue;  // e.g., "Slow performance"
    std::string impact;  // e.g., "Reduces user satisfaction"
    std::string stakeholders;  // e.g., "End users, Dev team"

    // Static factory method (factory style)
    static PainPointContent CreatePainPointsContent(const std::string& issue, const std::string& impact, const std::string& stakeholders) {
        // Basic validation
        if (issue.empty() || impact.empty()) {
            throw std::invalid_argument("Invalid pain points parameters: issue or impact empty");
        }
        return PainPointContent{issue, impact, stakeholders};
    }

    // Constructor
    PainPointContent(const std::string& issue, const std::string& impact, const std::string& stakeholders)
        : issue(issue), impact(impact), stakeholders(stakeholders) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !issue.empty() && !impact.empty();
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"issue\": \"" + issue + "\", \"impact\": \"" + impact + "\", \"stakeholders\": \"" + stakeholders + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    PainPointContent(const PainPointContent& other) = default;
};











#endif // __SPAINPOINTCONTENT_HPP__
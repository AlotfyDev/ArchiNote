
#ifndef __SCONSTRAINTCONTENT_HPP__
#define __SCONSTRAINTCONTENT_HPP__




#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this

#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument





// Flat struct for ConstraintsContent (no inheritance, pure data + methods)
struct ConstraintContent {
    std::string constraint;  // e.g., "Limited to cloud deployment"
    std::string impact;  // e.g., "Increases hosting costs"

    // Static factory method (factory style)
    static ConstraintContent CreateConstraintsContent(const std::string& constraint, const std::string& impact) {
        // Basic validation
        if (constraint.empty() || impact.empty()) {
            throw std::invalid_argument("Invalid constraints parameters: constraint or impact empty");
        }
        return ConstraintContent{constraint, impact};
    }

    // Constructor
    ConstraintContent(const std::string& constraint, const std::string& impact)
        : constraint(constraint), impact(impact) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !constraint.empty() && !impact.empty();
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"constraint\": \"" + constraint + "\", \"impact\": \"" + impact + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    ConstraintContent(const ConstraintContent& other) = default;
};










#endif // __SCONSTRAINTCONTENT_HPP
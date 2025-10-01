#ifndef __SHIGHLEVELFEATURECONTENT_HPP__
#define __SHIGHLEVELFEATURECONTENT_HPP__





#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this

#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument



// Flat struct for HighLevelFeaturesContent (no inheritance, pure data + methods)
struct HighLevelFeatureContent {
    std::string feature;  // e.g., "Real-time analytics"
    std::string description;  // e.g., "Provides live user data"
    int priority;  // e.g., 1-3

    // Static factory method (factory style)
    static HighLevelFeatureContent CreateHighLevelFeaturesContent(const std::string& feature, const std::string& description, int priority) {
        // Basic validation
        if (feature.empty() || description.empty()) {
            throw std::invalid_argument("Invalid high-level features parameters: feature or description empty");
        }
        if (priority < 1 || priority > 3) {
            throw std::invalid_argument("Invalid priority: must be between 1 and 3");
        }
        return HighLevelFeatureContent{feature, description, priority};
    }

    // Constructor
    HighLevelFeatureContent(const std::string& feature, const std::string& description, int priority)
        : feature(feature), description(description), priority(priority) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !feature.empty() && !description.empty() && priority >= 1 && priority <= 3;
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"feature\": \"" + feature + "\", \"description\": \"" + description + "\", \"priority\": " + std::to_string(priority) + " }";
    }

    // Deep copy via copy constructor (used for history)
    HighLevelFeatureContent(const HighLevelFeatureContent& other) = default;
};














#endif // __SHIGHLEVELFEATURECONTENT_HPP__
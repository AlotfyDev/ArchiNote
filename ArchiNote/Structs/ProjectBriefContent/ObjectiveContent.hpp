
#ifndef __OBJECTIVECONTENT_HPP__
#define __OBJECTIVECONTENT_HPP__


#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this
#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument





// Example specific content impl (ObjectiveContent)
// Flat struct for ObjectiveContent (no inheritance, pure data + methods)
struct ObjectiveContent {
    std::string description;
    std::string metric;
    int priority;

    // Static factory method (factory style)
    static ObjectiveContent CreateObjectiveContent(const std::string& desc, const std::string& met, int pri) {
        if (desc.empty() || pri < 1 || pri > 3) {
            throw std::invalid_argument("Invalid objective parameters: description empty or priority out of range");
        }
        return ObjectiveContent{desc, met, pri};
    }

    // Constructor
    ObjectiveContent(const std::string& desc, const std::string& met, int pri)
        : description(desc), metric(met), priority(pri) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !description.empty() && priority > 0 && priority <= 3;
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"desc\": \"" + description + "\", \"metric\": \"" + metric + "\", \"priority\": " + std::to_string(priority) + " }";
    }

    // Deep copy via copy constructor (used for history)
    ObjectiveContent(const ObjectiveContent& other) = default;
};














#endif // __OBJECTIVECONTENT_HPP__
#ifndef __SREVIEWCHECKLISTCONTENT_HPP__
#define __SREVIEWCHECKLISTCONTENT_HPP__




#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this

#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument






// Flat struct for ReviewChecklistContent (no inheritance, pure data + methods)
struct ReviewChecklistContent {
    std::string item;  // e.g., "Objectives are measurable"
    bool isChecked;  // e.g., true/false
    std::string status;  // e.g., "Pass/Fail/Pending"

    // Static factory method (factory style)
    static ReviewChecklistContent CreateReviewChecklistContent(const std::string& item, bool isChecked, const std::string& status) {
        // Basic validation
        if (item.empty()) {
            throw std::invalid_argument("Invalid review checklist parameters: item empty");
        }
        if (status != "Pass" && status != "Fail" && status != "Pending") {
            throw std::invalid_argument("Invalid status: must be Pass, Fail, or Pending");
        }
        return ReviewChecklistContent{item, isChecked, status};
    }

    // Constructor
    ReviewChecklistContent(const std::string& item, bool isChecked, const std::string& status)
        : item(item), isChecked(isChecked), status(status) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !item.empty() && (status == "Pass" || status == "Fail" || status == "Pending");
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"item\": \"" + item + "\", \"isChecked\": " + (isChecked ? "true" : "false") + ", \"status\": \"" + status + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    ReviewChecklistContent(const ReviewChecklistContent& other) = default;
};
















#endif // __SREVIEWCHECKLISTCONTENT_HPP__
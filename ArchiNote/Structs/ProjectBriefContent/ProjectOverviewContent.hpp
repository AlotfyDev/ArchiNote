#ifndef __SPROJECTOVERVIEWCONTENT_HPP__
#define __SPROJECTOVERVIEWCONTENT_HPP__


#include <string>
#include <vector>
#include <map>
#include <time.h>


#include <memory>
#include <cstdint>  // ✅ Add this
#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument



// Flat struct for ProjectOverviewContent (no inheritance, pure data + methods)
struct ProjectOverviewContent {
    std::string title;
    std::string brief;
    std::string date;  // Format: YYYY-MM-DD, e.g., "2025-09-14"

    // Static factory method (factory style)
    static ProjectOverviewContent CreateProjectOverviewContent(const std::string& title, const std::string& brief, const std::string& date) {
        // Basic validation
        if (title.empty() || brief.empty()) {
            throw std::invalid_argument("Invalid project overview parameters: title or brief empty");
        }
        // Simple date format check (YYYY-MM-DD)
        if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
            throw std::invalid_argument("Invalid date format: expected YYYY-MM-DD");
        }
        return ProjectOverviewContent{title, brief, date};
    }

    // Constructor
    ProjectOverviewContent(const std::string& title, const std::string& brief, const std::string& date)
        : title(title), brief(brief), date(date) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !title.empty() && !brief.empty() && date.length() == 10 && date[4] == '-' && date[7] == '-';
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"title\": \"" + title + "\", \"brief\": \"" + brief + "\", \"date\": \"" + date + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    ProjectOverviewContent(const ProjectOverviewContent& other) = default;
};











#endif // __PROJECTOVERVIEWCONTENT_HPP
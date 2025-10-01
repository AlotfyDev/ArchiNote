#ifndef __SSUCCESSMETRICCONTENT_HPP__
#define __SSUCCESSMETRICCONTENT_HPP__





#include <string>
#include <vector>
#include <memory>
#include <cstdint>  // ✅ Add this

#include <functional>  // ✅ Add this
#include <stdexcept> // Required for std::invalid_argument



// Flat struct for SuccessMetricsContent (no inheritance, pure data + methods)
struct SuccessMetricContent {
    std::string metric;  // e.g., "User retention rate"
    std::string target;  // e.g., "80% retention after 3 months"
    std::string timeframe;  // e.g., "Q1 2026"

    // Static factory method (factory style)
    static SuccessMetricContent CreateSuccessMetricsContent(const std::string& metric, const std::string& target, const std::string& timeframe) {
        // Basic validation
        if (metric.empty() || target.empty()) {
            throw std::invalid_argument("Invalid success metrics parameters: metric or target empty");
        }
        return SuccessMetricContent{metric, target, timeframe};
    }

    // Constructor
    SuccessMetricContent(const std::string& metric, const std::string& target, const std::string& timeframe)
        : metric(metric), target(target), timeframe(timeframe) {}

    // Validation method (no virtual)
    bool isContentValid() const {
        return !metric.empty() && !target.empty();
    }

    // JSON serialization (no virtual)
    std::string toJSON() const {
        return "{ \"metric\": \"" + metric + "\", \"target\": \"" + target + "\", \"timeframe\": \"" + timeframe + "\" }";
    }

    // Deep copy via copy constructor (used for history)
    SuccessMetricContent(const SuccessMetricContent& other) = default;
};
















#endif // __SSUCCESSMETRICCONTENT_HPP
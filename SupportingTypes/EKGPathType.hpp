
#ifndef __EKGPATHTYPE_HPP__
#define __EKGPATHTYPE_HPP__


#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

// 1. Enum definition
enum class EKGPathType {
    DEPENDENCY,    // Path representing dependency between nodes (e.g., TASK depends on PROJECT)
    WORKFLOW,      // Path representing a sequence of operations (e.g., pipeline steps)
    HIERARCHICAL,  // Path representing hierarchical relationships (e.g., PROJECT contains TASK)
    KNOWLEDGE,     // Path for knowledge graph traversal (e.g., Lite RAG queries)
    TRACEABILITY,  // Path for traceability in SRS docs
    Unknown        // Default/fallback for invalid or undefined paths
};

// 2. Utility class for EKGPathType conversion
class PathTypeUtils {
public:
    // 3. Static method to return unordered_map of EKGPathType to string (hard-coded)
    static unordered_map<EKGPathType, string> PathTypeMap() {
        return {
            {EKGPathType::DEPENDENCY, "DEPENDENCY"},
            {EKGPathType::WORKFLOW, "WORKFLOW"},
            {EKGPathType::HIERARCHICAL, "HIERARCHICAL"},
            {EKGPathType::KNOWLEDGE, "KNOWLEDGE"},
            {EKGPathType::TRACEABILITY, "TRACEABILITY"},
            {EKGPathType::Unknown, "Unknown"}
        };
    }

    // 4. Static method to check if an EKGPathType value is valid
    static bool IsPathType(EKGPathType value) {
        static const auto typeMap = PathTypeMap();
        return typeMap.find(value) != typeMap.end();
    }

    // 5. Static method to convert EKGPathType to string
    static string PathTypeString(EKGPathType value) {
        static const auto typeMap = PathTypeMap();
        auto it = typeMap.find(value);
        return it != typeMap.end() ? it->second : "Unknown";
    }
};






#endif // EKGPATHTYPE_HPP_
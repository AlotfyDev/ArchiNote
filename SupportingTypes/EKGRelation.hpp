
#ifndef __EKGRELATION_HPP__
#define __EKGRELATION_HPP__


#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

// 1. Enum definition
enum class EdgeRelationshipType {
    DEPENDS_ON,   // Node depends on another (e.g., TASK depends on PROJECT)
    IMPLEMENTS,   // Node implements another (e.g., DEVELOPMENT implements ARCHITECTURE)
    USES,         // Node uses another (e.g., TASK uses KNOWLEDGE)
    CONTAINS,     // Node contains another (e.g., PROJECT contains TASK)
    EXTENDS,      // Node extends another (e.g., specialized TASK extends general TASK)
    RELATED_TO,   // Generic relationship between nodes
    ANY,          // Wildcard for any relationship type
    UNKNOWN       // Default/fallback for invalid or undefined relationships
};

// 2. Utility class for EdgeRelationshipType conversion
class EdgeTypeUtils {
public:
    // 3. Static method to return unordered_map of EdgeRelationshipType to string (hard-coded)
    static unordered_map<EdgeRelationshipType, string> EdgeTypeMap() {
        return {
            {EdgeRelationshipType::DEPENDS_ON, "DEPENDS_ON"},
            {EdgeRelationshipType::IMPLEMENTS, "IMPLEMENTS"},
            {EdgeRelationshipType::USES, "USES"},
            {EdgeRelationshipType::CONTAINS, "CONTAINS"},
            {EdgeRelationshipType::EXTENDS, "EXTENDS"},
            {EdgeRelationshipType::RELATED_TO, "RELATED_TO"},
            {EdgeRelationshipType::ANY, "ANY"},
            {EdgeRelationshipType::UNKNOWN, "Unknown"}
        };
    }

    // 4. Static method to check if an EdgeRelationshipType value is valid
    static bool IsEdgeType(EdgeRelationshipType value) {
        static const auto typeMap = EdgeTypeMap();
        return typeMap.find(value) != typeMap.end();
    }

    // 5. Static method to convert EdgeRelationshipType to string
    static string EdgeTypeString(EdgeRelationshipType value) {
        static const auto typeMap = EdgeTypeMap();
        auto it = typeMap.find(value);
        return it != typeMap.end() ? it->second : "Unknown";
    }
};











#endif // __EKGRELATION_HPP__
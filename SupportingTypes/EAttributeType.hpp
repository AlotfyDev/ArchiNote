
#ifndef __EATTRIBUTETYPE_HPP__
#define __EATTRIBUTETYPE_HPP__





#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

// 1. Enum definition
enum class EAttributeType {
    DATAMEMBER,
    METHOD,
    REFERENCE,  // للإشارة إلى nodes أخرى
    COMPOSITE,  // attributes متداخلة
    GOAL,
    METRIC,
    Unknown     // Added for consistency with NodeType and fallback
};

// 2. Utility class for EAttributeType conversion
class AttributeTypeUtils {
public:
    // 3. Static method to return unordered_map of EAttributeType to string (hard-coded)
    static unordered_map<EAttributeType, string> AttributeTypeMap() {
        return {
            {EAttributeType::DATAMEMBER, "DATAMEMBER"},
            {EAttributeType::METHOD, "METHOD"},
            {EAttributeType::REFERENCE, "REFERENCE"},
            {EAttributeType::COMPOSITE, "COMPOSITE"},
            {EAttributeType::GOAL, "GOAL"},
            {EAttributeType::METRIC, "METRIC"},
            {EAttributeType::Unknown, "Unknown"}
        };
    }

    // 4. Static method to check if an EAttributeType value is valid
    static bool IsAttributeType(EAttributeType value) {
        static const auto typeMap = AttributeTypeMap();
        return typeMap.find(value) != typeMap.end();
    }

    // 5. Static method to convert EAttributeType to string
    static string AttributeTypeString(EAttributeType value) {
        static const auto typeMap = AttributeTypeMap();
        auto it = typeMap.find(value);
        return it != typeMap.end() ? it->second : "Unknown";
    }
};










#endif // __EATTRIBUTETYPE_HPP__
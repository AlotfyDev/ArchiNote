#ifndef __EATTRIBUTECONTENTTYPE_HPP__
#define __EATTRIBUTECONTENTTYPE_HPP__






#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

// 1. Enum definition
enum class EAttributeContentType {
    KEY_VALUE,      // {key: value} pairs
    LIST,           // [item1, item2, ...]
    NESTED_STRUCT,  // Nested structures
    MIXED,          // Mixed types
    Unknown         // Default/fallback for invalid or undefined content types
};

// 2. Utility class for EAttributeContentType conversion
class ContentTypeUtils {
public:
    // 3. Static method to return unordered_map of EAttributeContentType to string (hard-coded)
    static unordered_map<EAttributeContentType, string> ContentTypeMap() {
        return {
            {EAttributeContentType::KEY_VALUE, "KEY_VALUE"},
            {EAttributeContentType::LIST, "LIST"},
            {EAttributeContentType::NESTED_STRUCT, "NESTED_STRUCT"},
            {EAttributeContentType::MIXED, "MIXED"},
            {EAttributeContentType::Unknown, "Unknown"}
        };
    }

    // 4. Static method to check if an EAttributeContentType value is valid
    static bool IsContentType(EAttributeContentType value) {
        static const auto typeMap = ContentTypeMap();
        return typeMap.find(value) != typeMap.end();
    }

    // 5. Static method to convert EAttributeContentType to string
    static string ContentTypeString(EAttributeContentType value) {
        static const auto typeMap = ContentTypeMap();
        auto it = typeMap.find(value);
        return it != typeMap.end() ? it->second : "Unknown";
    }
};

















#endif // __EATTRIBUTECONTENTTYPE_HPP__
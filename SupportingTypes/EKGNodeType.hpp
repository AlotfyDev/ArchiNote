
#ifndef __EKGNODETYPE_HPP__
#define __EKGNODETYPE_HPP__


#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using std::string;

using std::unordered_map;


enum class NodeType {
    PROJECT,
    ARCHITECTURE, 
    DEVELOPMENT,
    QUALITY,
    KNOWLEDGE,
    BACKLOG,
    TASK,
    Unknown
};





// 2. Utility class for NodeType conversion
class NodeTypeUtils {
public:
    // 3. Static method to return map of NodeType to string (hard-coded)
    static unordered_map<NodeType, string> NodeTypeMap() {
        return {
            {NodeType::PROJECT, "PROJECT"},
            {NodeType::ARCHITECTURE, "ARCHITECTURE"},
            {NodeType::DEVELOPMENT, "DEVELOPMENT"},
            {NodeType::QUALITY, "QUALITY"},
            {NodeType::KNOWLEDGE, "KNOWLEDGE"},
            {NodeType::BACKLOG, "BACKLOG"},
            {NodeType::TASK, "TASK"},
            {NodeType::Unknown, "Unknown"}
        };
    }

    // 4. Static method to check if a NodeType value is valid
    static bool IsNodeType(NodeType value) {
        static const auto typeMap = NodeTypeMap();
        return typeMap.find(value) != typeMap.end();
    }

    // 5. Static method to convert NodeType to string
    static string NodeTypeString(NodeType value) {
        static const auto typeMap = NodeTypeMap();
        auto it = typeMap.find(value);
        return it != typeMap.end() ? it->second : "Unknown";
    }

};














#endif // __EKGNODETYPE_HPP__
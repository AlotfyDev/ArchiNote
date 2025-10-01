#ifndef __KGPATH_HPP__
#define __KGPATH_HPP__


#include <vector>
#include <string>
#include <memory>
#include <cstdint>  // ✅ Add this
#include <ctime>
#include <atomic>
#include <map>
#include "KGEdge.hpp"
#include "KGBaseNode.hpp"
#include "EKGPathType.hpp"



/*
class KGPath {
private:
    std::vector<std::string> nodeIds;  // ترتيب الـ Nodes في المسار
    std::vector<KGEdge> edges;         // الـ Edges بين الـ Nodes
    double totalStrength;              // قوة المسار الإجمالية
    int depth;                         // عمق المسار (عدد الـ Edges)

public:
    // Constructor
    KGPath();
    KGPath(const std::string& startNodeId);
    
    // Copy constructor
    KGPath(const KGPath& other);
    
    // Move constructor  
    KGPath(KGPath&& other) noexcept;
    
    // Destructor
    ~KGPath() = default;
    
    // Methods لإدارة المسار
    void addSegment(const std::string& nextNodeId, const KGEdge& edge);
    void removeLastSegment();
    bool containsNode(const std::string& nodeId) const;
    bool containsEdge(const std::string& edgeId) const;
    
    // Getters
    const std::vector<std::string>& getNodeIds() const;
    const std::vector<KGEdge>& getEdges() const;
    double getTotalStrength() const;
    int getDepth() const;
    size_t getLength() const;  // عدد الـ Nodes في المسار
    
    // Get specific elements
    std::string getStartNodeId() const;
    std::string getEndNodeId() const;
    KGEdge getLastEdge() const;
    
    // Validation
    bool isValid() const;
    bool isCyclic() const;  // هل المسار فيه دورة؟
    
    // Utility methods
    void clear();
    bool isEmpty() const;
    
    // Serialization
    std::string toYAML() const;
    std::string toJSON() const;
    std::string toString() const;  // تمثيل نصي بسيط
    
    // Operators
    KGPath& operator=(const KGPath& other);
    KGPath& operator=(KGPath&& other) noexcept;
    bool operator==(const KGPath& other) const;
};
*/

class KGPath {
private:
    // 1. Unique identifier for the path, used for versioning in Lite RAG
    std::string path_id;
    
    // 2. Vector of edges forming the path
    std::vector<KGEdge> edges;
    
    // 3. Vector of node IDs in path order (for explicit tracking)
    std::vector<std::string> nodeIds;
    
    // 4. Timestamp for last update
    std::time_t last_updated;
    
    // 5. Semantic description of the path (e.g., "Goals to Metrics")
    std::string description;
    
    // 6. Cached depth (number of edges)
    int depth;
    
    // 7. Cached total confidence score (average of edge confidences)
    double totalStrength;

    // 8. Path type
    EKGPathType type;

public:
    // 8. Constructors
    KGPath() : path_id(generateId()), edges(), nodeIds(), last_updated(std::time(nullptr)), description(""), depth(0), totalStrength(0.0), type(EKGPathType::Unknown) {}
    KGPath(const std::string& desc) : path_id(generateId()), edges(), nodeIds(), last_updated(std::time(nullptr)), description(desc), depth(0), totalStrength(0.0), type(EKGPathType::Unknown) {}
    KGPath(const std::string& startNodeId, const std::string& desc = "") : path_id(generateId()), edges(), nodeIds{startNodeId}, last_updated(std::time(nullptr)), description(desc), depth(0), totalStrength(0.0), type(EKGPathType::Unknown) {}
    KGPath(const std::string& pathId, const std::vector<KGEdge>& pathEdges, EKGPathType pathType, const std::string& desc) : path_id(pathId), edges(pathEdges), nodeIds(), last_updated(std::time(nullptr)), description(desc), depth(static_cast<int>(pathEdges.size())), totalStrength(0.0), type(pathType) {
        if (!edges.empty()) {
            nodeIds.push_back(edges[0].getSourceNodeId());
            for (const auto& edge : edges) {
                nodeIds.push_back(edge.getTargetNodeId());
            }
            for (const auto& edge : edges) {
                totalStrength += edge.getStrength();
            }
            totalStrength /= edges.size();
        }
    }
    KGPath(const KGPath& other) : path_id(other.path_id), edges(other.edges), nodeIds(other.nodeIds), last_updated(other.last_updated), description(other.description), depth(other.depth), totalStrength(other.totalStrength), type(other.type) {}  // Copy constructor
    KGPath(KGPath&& other) noexcept : path_id(std::move(other.path_id)), edges(std::move(other.edges)), nodeIds(std::move(other.nodeIds)), last_updated(other.last_updated), description(std::move(other.description)), depth(other.depth), totalStrength(other.totalStrength), type(other.type) { other.depth = 0; other.totalStrength = 0.0; other.type = EKGPathType::Unknown; }  // Move constructor
    
    // 9. Destructor
    ~KGPath() = default;

    // 10. Add edge with semantic validation and node ID tracking
    bool addSegment(const KGEdge& edge, const std::map<std::string, NodeType>& node_types) {
        // 10.1 Validate: initialize first node if empty (from previous version)
        if (nodeIds.empty()) {
            nodeIds.push_back(edge.getSourceNodeId());
        }
        // 10.2 Validate: edge connects to previous edge (continuity)
        if (!edges.empty() && edges.back().getTargetNodeId() != edge.getSourceNodeId()) {
            return false;
        }
        // 10.3 Semantic check: ensure relationship matches node types
        if (!node_types.empty()) {  // Allow bypass for subPath
            auto from_type = node_types.at(edge.getSourceNodeId());
            auto to_type = node_types.at(edge.getTargetNodeId());
            if (!isValidEdgeForTypes(from_type, to_type, edge.getRelationshipTypeAsString())) {
                return false;
            }
        }
        // 10.4 Add edge and update nodeIds, depth, totalStrength
        edges.push_back(edge);
        nodeIds.push_back(edge.getTargetNodeId());
        depth = static_cast<int>(edges.size());
        totalStrength += edge.getStrength();  // Direct add (from previous version)
        last_updated = std::time(nullptr);
        return true;
    }

    // 11. Remove last edge and node
    void removeLastSegment() {
        if (!edges.empty()) {
            totalStrength -= edges.back().getStrength();  // Direct subtract (from previous version)
            edges.pop_back();
            nodeIds.pop_back();
            depth = static_cast<int>(edges.size());
            if (totalStrength < 0.0) totalStrength = 0.0;  // Ensure non-negative
            last_updated = std::time(nullptr);
        }
    }

    // 12. Validation methods
    bool containsNode(const std::string& nodeId) const {
        return std::find(nodeIds.begin(), nodeIds.end(), nodeId) != nodeIds.end();
    }
    bool containsEdge(const std::string& edgeId) const {
        for (const auto& edge : edges) {
            if (edge.getId() == edgeId) return true;
        }
        return false;
    }
    bool isValid() const {
        // 12.1 Check nodeIds size consistency (from previous version)
        if (nodeIds.size() != edges.size() + 1) return false;
        // 12.2 Check edge-node mapping (from previous version)
        for (size_t i = 0; i < edges.size(); ++i) {
            if (edges[i].getSourceNodeId() != nodeIds[i] ||
                edges[i].getTargetNodeId() != nodeIds[i + 1]) {
                return false;
            }
        }
        return true;
    }
    bool isCyclic() const {
        // 12.3 Simplified cyclic check (from previous version)
        if (nodeIds.size() < 2) return false;
        const std::string& lastNode = nodeIds.back();
        for (size_t i = 0; i < nodeIds.size() - 1; ++i) {
            if (nodeIds[i] == lastNode) return true;
        }
        return false;
    }

    // 13. Getters
    std::string getPathId() const { return path_id; }
    const std::vector<KGEdge>& getEdges() const { return edges; }
    const std::vector<std::string>& getNodeIds() const { return nodeIds; }
    std::string getStartNodeId() const { return nodeIds.empty() ? "" : nodeIds.front(); }
    std::string getEndNodeId() const { return nodeIds.empty() ? "" : nodeIds.back(); }
    KGEdge getLastEdge() const { return edges.empty() ? KGEdge() : edges.back(); }
    std::string getDescription() const { return description; }
    std::time_t getLastUpdated() const { return last_updated; }
    double getTotalStrength() const { return totalStrength; }
    int getDepth() const { return depth; }
    size_t getLength() const { return nodeIds.size(); }
    EKGPathType getType() const { return type; }

    // 14. Path Weighting: Calculate average confidence score of edges
    void updateTotalStrength() {
        totalStrength = 0.0;
        if (!edges.empty()) {
            for (const auto& edge : edges) {
                totalStrength += edge.getStrength();
            }
            totalStrength /= edges.size();
        }
    }

    // 15. Path Queries: Extract sub-path between start_id and end_id
    KGPath findSubPath(const std::string& start_id, const std::string& end_id) const {
        KGPath sub_path("", "SubPath from " + start_id + " to " + end_id);
        bool started = false;
        for (size_t i = 0; i < edges.size(); ++i) {
            if (edges[i].getSourceNodeId() == start_id) started = true;
            if (started) {
                sub_path.addSegment(edges[i], {});  // Bypass node_types check
                if (edges[i].getTargetNodeId() == end_id) break;
            }
        }
        return sub_path.isValid() ? sub_path : KGPath();
    }

    // 16. Utility methods
    void clear() {
        edges.clear();
        nodeIds.clear();
        depth = 0;
        totalStrength = 0.0;
        last_updated = std::time(nullptr);
    }
    bool isEmpty() const {
        return nodeIds.empty();  // Use nodeIds (from previous version)
    }

    // 17. Serialization
    std::string toYAML() const {
        std::stringstream ss;
        ss << "path:\n";
        ss << "  id: " << path_id << "\n";
        ss << "  description: \"" << description << "\"\n";
        ss << "  depth: " << depth << "\n";
        ss << "  total_strength: " << totalStrength << "\n";
        ss << "  nodes:\n";
        for (const auto& nodeId : nodeIds) {
            ss << "    - " << nodeId << "\n";
        }
        ss << "  edges:\n";
        for (const auto& edge : edges) {
            ss << "    - from: " << edge.getSourceNodeId() << "\n";
            ss << "      to: " << edge.getTargetNodeId() << "\n";
            ss << "      relationship: " << edge.getRelationshipTypeAsString() << "\n";
            ss << "      confidence: " << edge.getStrength() << "\n";
        }
        return ss.str();
    }
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"path\": {";
        ss << "\"id\": \"" << path_id << "\",";
        ss << "\"description\": \"" << description << "\",";
        ss << "\"depth\": " << depth << ",";
        ss << "\"total_strength\": " << totalStrength << ",";
        ss << "\"nodes\": [";
        for (size_t i = 0; i < nodeIds.size(); ++i) {
            ss << "\"" << nodeIds[i] << "\"" << (i < nodeIds.size() - 1 ? "," : "");
        }
        ss << "],";
        ss << "\"edges\": [";
        for (size_t i = 0; i < edges.size(); ++i) {
            ss << "{";
            ss << "\"from\": \"" << edges[i].getSourceNodeId() << "\",";
            ss << "\"to\": \"" << edges[i].getTargetNodeId() << "\",";
            ss << "\"relationship\": \"" << edges[i].getRelationshipTypeAsString() << "\",";
            ss << "\"confidence\": " << edges[i].getStrength();
            ss << "}" << (i < edges.size() - 1 ? "," : "");
        }
        ss << "]}}";
        return ss.str();
    }
    std::string toString() const {
        if (nodeIds.empty()) return "Empty Path";
        std::string result = "Path: " + nodeIds[0];
        for (size_t i = 0; i < edges.size(); ++i) {
            result += " --" + edges[i].getRelationshipTypeAsString() + "--> " + nodeIds[i + 1];
        }
        result += " (Strength: " + std::to_string(totalStrength) + ")";
        return result;
    }

    // 18. Operators
    KGPath& operator=(const KGPath& other) {
        if (this != &other) {
            path_id = other.path_id;
            edges = other.edges;
            nodeIds = other.nodeIds;
            last_updated = other.last_updated;
            description = other.description;
            depth = other.depth;
            totalStrength = other.totalStrength;
        }
        return *this;
    }
    KGPath& operator=(KGPath&& other) noexcept {
        if (this != &other) {
            path_id = std::move(other.path_id);
            edges = std::move(other.edges);
            nodeIds = std::move(other.nodeIds);
            last_updated = other.last_updated;
            description = std::move(other.description);
            depth = other.depth;
            totalStrength = other.totalStrength;
            other.depth = 0;
            other.totalStrength = 0.0;
        }
        return *this;
    }
    bool operator==(const KGPath& other) const {
        return path_id == other.path_id && nodeIds == other.nodeIds && edges.size() == other.edges.size();
    }

private:
    // 19. Generate unique path ID
    static std::string generateId() {
        return "PATH_" + std::to_string(std::time(nullptr));
    }

    // 20. Semantic check for edge types (software engineering domain)
    bool isValidEdgeForTypes(NodeType from, NodeType to, const std::string& relationship) const {
        // 20.1 Rule: PROJECT -> BACKLOG with DEPENDS_ON
        if (from == NodeType::PROJECT && to == NodeType::BACKLOG && relationship == "DEPENDS_ON") {
            return true;
        }
        // 20.2 Rule: BACKLOG -> TASK with CONTAINS
        if (from == NodeType::BACKLOG && to == NodeType::TASK && relationship == "CONTAINS") {
            return true;
        }
        // 20.3 Rule: PROJECT -> TASK with IMPLEMENTS
        if (from == NodeType::PROJECT && to == NodeType::TASK && relationship == "IMPLEMENTS") {
            return true;
        }
        // 20.4 Open for additional rules for software engineering specs
        return false;
    }
};




















#endif // __KGPATH_HPP__